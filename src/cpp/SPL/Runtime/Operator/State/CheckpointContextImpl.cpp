/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Implementation of SPL::CheckpointContextImpl class
 */
#include <SAM/applicationModel.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/Adapters/FileSystemAdapter/FileSystemDataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/Checkpoint.h>
#include <SPL/Runtime/Operator/State/CheckpointBatch.h>
#include <SPL/Runtime/Operator/State/CheckpointConfig.h>
#include <SPL/Runtime/Operator/State/CheckpointContext.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Operator/State/CheckpointDeletionWorkItem.h>
#include <SPL/Runtime/Operator/State/CheckpointLogTrace.h>
#include <SPL/Runtime/Operator/State/CheckpointNaming.h>
#include <SPL/Runtime/Operator/State/DataStoreEntry.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/IncrementalCkptInfo.h>
#include <SPL/Runtime/Operator/State/IncrementalCkptIntervalSetter.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <UTILS/HostToNetwork.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <exception>
#include <set>
#include <vector>

using namespace SPL;
using namespace Distillery;
using namespace std;

CheckpointContextImpl::CheckpointContextImpl(Operator& op, const OPModel& opmod)
  : op_(op)
  , storeEntry_(NULL)
  , checkpointCounter_(0)
  , isEnabled_(true)
  , lastCkptSizeNorm_(0)
  , lastCkptSizeBase_(0)
  , lastCkptSizeDelta_(0)
  , lastCkptSizeIncr_(0)
  , lastCkptSizeIndex_(0)
  , incrementalCkptInterval_(0)
  , numSuccessCkpt_(0)
  , ckptIDs_(NULL)
  , hasIncrCkpt_(false)
  , mutex_()
  , intervalSetter_()
{
    try {
        storeAdapter_ = CheckpointConfig::instance()->getDataStoreAdapter();
        SPLCKPTTRC(L_DEBUG, opmod.name(),
                   "Create Checkpoint Data Store Entry for operator " << opmod.name());
        // compose unique Data Store Entry name for this operator
        storeEntryName_ = CheckpointNaming::getCkptStoreEntryName(
          ProcessingElement::pe().getDomainID(), ProcessingElement::pe().getInstanceID(),
          ProcessingElement::pe().getJobId(), opmod.index());
        Option option;
        option.create_if_missing = true;
        option.error_if_exist = false;
        storeEntry_.reset(storeAdapter_->getDataStoreEntry(storeEntryName_, option));
        assert(storeEntry_.get() != NULL);
        ckptIDs_ = new CkptIDs();
        intervalSetter_.initialize(NULL); // use the default base/delta size ratio
        incrementalCkptInterval_ = intervalSetter_.determineInterval();
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(opmod.name(), "Cannot create CheckpointContextImpl",
                                        e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(opmod.name(), "Cannot create CheckpointContextImpl", e.what());
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(opmod.name(), "Cannot create CheckpointContextImpl",
                                 "unknown exception");
    }
}

CheckpointContextImpl::~CheckpointContextImpl()
{
    if (ckptIDs_) {
        delete ckptIDs_;
    }
    for (std::deque<CkptIDs*>::iterator it = prevCkptIDs_.begin(); it != prevCkptIDs_.end(); ++it) {
        delete *it;
    }
}

bool CheckpointContextImpl::isExistingCheckpoint(const int64_t& id) const
{
    assert(storeEntry_.get() != NULL);

    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
               "Checking existence of checkpoint with Sequence ID " << id);
    try {
        return storeEntry_->isExistingByteBuffer(CheckpointNaming::getDataKey(id));
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(
          op_.getContext().getName(),
          "Cannot determine the existence of checkpoint with Sequence ID " +
            boost::lexical_cast<std::string>(id),
          e.getExplanation(), e);
    }
}

bool CheckpointContextImpl::deleteCheckpoint(const int64_t& id) const
{
    assert(storeEntry_ != NULL);

    // delete non-incremental checkpoint in checkpoint of the given id
    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Deleting checkpoint with Sequence ID " << id);
    try {
        storeEntry_->removeByteBuffer(CheckpointNaming::getDataKey(id));
    } catch (DataStoreException const& e) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoint with Sequence ID "
                     << id << ": received exception: " << e.getExplanation());
    } catch (std::exception const& e) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoint with Sequence ID "
                     << id << ": received exception: " << e.what());
    } catch (...) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoint with Sequence ID "
                     << id << ": received exception: unknown exception");
    }
    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
               "Finish deleting checkpoint with Sequence ID " << id);

    // delete incremental checkpoints in previous intervals
    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
               "Deleting incremental checkpoints in previous interval");
    std::deque<CkptIDs*> incrCkptIDs; // old incremental checkpoints which are safe to delete now
    {
        Distillery::AutoMutex myAutoMutex(mutex_);
        while (!prevCkptIDs_.empty()) {
            CkptIDs* IDs = prevCkptIDs_.front();
            int64_t lastID = IDs->back().first + IDs->back().second - 1;
            if (id >= lastID) { // now all the incremental checkpoints in the whole interval can be
                                // deleted
                try {
                    incrCkptIDs.push_back(prevCkptIDs_.front());
                    prevCkptIDs_.pop_front();
                } catch (std::exception const& e) {
                    // it's not a fatal error to leave garbage checkpoint
                    break;
                }
            } else {
                break;
            }
        }
    }
    for (std::deque<CkptIDs*>::const_iterator interval = incrCkptIDs.begin();
         interval != incrCkptIDs.end(); ++interval) {
        for (CkptIDs::const_iterator iter = (*interval)->begin(); iter != (*interval)->end();
             ++iter) {
            int64_t i = iter->first;
            int64_t lastID = i + iter->second - 1;
            for (; i <= lastID; i++) {
                try {
                    storeEntry_->removeByteBuffer(CheckpointNaming::getIncrementalDataKey(i));
                    storeEntry_->removeByteBuffer(CheckpointNaming::getIncrementalIndexKey(i));
                } catch (...) {
                    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                               "Failed to delete incremental checkpoint with Sequence ID " << i);
                }
            }
        }
    }
    for (std::deque<CkptIDs*>::const_iterator iter = incrCkptIDs.begin(); iter != incrCkptIDs.end();
         ++iter) {
        delete *iter;
    }
    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
               "Finish deleting incremental checkpoints in previous interval");

    storeEntry_->compactDatabaseIfNeeded();

    return true;
}

bool CheckpointContextImpl::deleteCheckpointAsync(const int64_t& id,
                                                  DeleteCallback callbackFunc,
                                                  void* clientData)
{
    try {
        UTILS_NAMESPACE::FixedThreadPool* threadPool = PEImpl::instance().getThreadPool();
        if (threadPool == NULL) {
            THROW_CHAR(DataStore, "no background thread available");
        }
        // create a deletion work item and submit it to background thread pool
        CheckpointDeletionWorkItem* witem = new CheckpointDeletionWorkItem(
          *this, id, op_.getContext().getName(), callbackFunc, clientData);
        threadPool->submitWork(witem);
    } catch (DataStoreException const& e) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoint with Sequence ID "
                     << id << ": received exception: " << e.getExplanation());
    } catch (std::exception const& e) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoint with Sequence ID "
                     << id << ": received exception: " << e.what());
    } catch (...) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoint with Sequence ID "
                     << id << ": received exception: unknown exception");
    }
    return true;
}

bool CheckpointContextImpl::deleteAllCheckpoints()
{
    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
               "Deleting all checkpoints of operator " << op_.getContext().getName());
    try {
        storeEntry_.reset(NULL);
        storeAdapter_->removeDataStoreEntry(storeEntryName_);
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Finish deleting all checkpoints of operator " << op_.getContext().getName());
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot delete all checkpoints", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "Cannot delete all checkpoints: received exception: " << e.what());
    } catch (...) {
        THROW(DataStore, "Cannot delete all checkpoints: unknown exception");
    }
    return true;
}

bool CheckpointContextImpl::createCheckpointInternal(const int64_t& id, CheckpointBatch* batch)
{
    assert(storeEntry_.get() != NULL);

    try {
        assert(ckptIDs_ != NULL);
        bool isBase = intervalSetter_.isBaseCheckpoint();
        bool enableLogging = intervalSetter_.shouldEnableLogging();
        int64_t baseID = isBase ? id : ckptIDs_->front().first;
        Checkpoint ckpt(storeEntry_.get(), id, true, baseID, numSuccessCkpt_,
                        incrementalCkptInterval_, enableLogging, lastCkptSizeNorm_,
                        (isBase ? lastCkptSizeBase_ : lastCkptSizeDelta_), lastCkptSizeIndex_,
                        batch);

        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Calling Operator's checkpointRaw() callback");

        op_.checkpointRaw(ckpt);
        ckpt.finishCheckpointing();

        // as performance optimization, record checkpoint size so we can determine the initial size
        // of internal buffer more accurately to reduce dynamic memory allocation overhead
        lastCkptSizeNorm_ = ckpt.getNormDataSize();
        if (isBase) {
            lastCkptSizeIncr_ = lastCkptSizeBase_ = ckpt.getIncrementalDataSize();
        } else {
            lastCkptSizeIncr_ = lastCkptSizeDelta_ = ckpt.getIncrementalDataSize();
        }
        lastCkptSizeIndex_ = ckpt.getIncrementalIndexSize();
        if (lastCkptSizeIndex_ > 0) {
            hasIncrCkpt_ = true;
        }
        // record successful checkpoint ID
        numSuccessCkpt_++;
        IncrementalCkptInfo ckptInfo(isBase, id, lastCkptSizeNorm_, lastCkptSizeIncr_,
                                     lastCkptSizeIndex_);
        intervalSetter_.addCheckpointInfo(ckptInfo);
        if (isBase) {
            incrementalCkptInterval_ = intervalSetter_.determineInterval();
            if (!ckptIDs_->empty()) { // swap if this checkpoint is a new base
                if (hasIncrCkpt_) {   // record ckpt IDs in prevCkptIDs_ for lazy deletion
                    CkptIDs* currentIDs = NULL;
                    try {
                        currentIDs = new CkptIDs();
                        currentIDs->push_back(std::make_pair(id, uint32_t(1)));
                        {
                            Distillery::AutoMutex myAutoMutex(mutex_);
                            prevCkptIDs_.push_back(ckptIDs_);
                        }
                        ckptIDs_ = currentIDs;
                    } catch (std::exception const& e) {
                        // it's not fatal error, the worst outcome is to temporarily leave some
                        // garbage checkpoints on backend
                        if (currentIDs) {
                            delete currentIDs;
                        }
                    }
                } else {
                    ckptIDs_->clear();
                    ckptIDs_->push_back(std::make_pair(id, uint32_t(1)));
                }
            } else {
                ckptIDs_->push_back(std::make_pair(id, uint32_t(1)));
            }
            hasIncrCkpt_ = false;
        } else {
            std::pair<int64_t, uint32_t>& last = ckptIDs_->back();
            if (last.first + last.second == id) { // consecutive
                last.second++;
            } else { // add a new (start, runLength) pair
                ckptIDs_->push_back(std::make_pair(id, uint32_t(1)));
            }
        }
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Operator checkpoint information: SequenceID = "
                     << id << ", isBase = " << isBase << ", interval = " << incrementalCkptInterval_
                     << ", enableLogging = " << enableLogging);
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(op_.getContext().getName(),
                                        "Cannot create checkpoint with Sequence ID " +
                                          boost::lexical_cast<std::string>(id),
                                        e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(),
                                 "Cannot create checkpoint with Sequence ID " +
                                   boost::lexical_cast<std::string>(id),
                                 e.what());
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(),
                                 "Cannot create checkpoint with Sequence ID " +
                                   boost::lexical_cast<std::string>(id),
                                 "unknown exception");
    }
    return true;
}

bool CheckpointContextImpl::createCheckpoint(const int64_t& id, CheckpointBatch* batch)
{
    assert(storeEntry_.get() != NULL);

    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Creating checkpoint with Sequence ID " << id);
    // measure checkpoint start time
    SPL::timestamp startTime = SPL::Functions::Time::getTimestamp();
    bool rc = createCheckpointInternal(id, batch);
    // measure checkpoint end time
    SPL::timestamp endTime = SPL::Functions::Time::getTimestamp();
    SPL::float64 ckptTime = SPL::Functions::Time::diffAsSecs(endTime, startTime);
    SPLCKPTTRC(L_INFO, op_.getContext().getName(),
               "Operator checkpointing is complete: SequenceID = "
                 << id << ", Size = " << lastCkptSizeNorm_ + lastCkptSizeIncr_ + lastCkptSizeIndex_
                 << " Bytes (non-incremental checkpoint data: " << lastCkptSizeNorm_
                 << " Bytes, incremental checkpoint data: "
                 << lastCkptSizeIncr_ + lastCkptSizeIndex_ << " Bytes), Time = " << ckptTime
                 << " Seconds");
    return rc;
}

bool CheckpointContextImpl::createCheckpoint(const int64_t& id)
{
    assert(storeEntry_.get() != NULL);

    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Creating checkpoint with Sequence ID " << id);
    // measure checkpoint start time
    SPL::timestamp startTime = SPL::Functions::Time::getTimestamp();
    try {
        CheckpointBatch batch;
        batch.begin(id);
        bool rc = createCheckpointInternal(id, &batch);
        // commit the batch
        batch.commit();
        // measure checkpoint end time
        SPL::timestamp endTime = SPL::Functions::Time::getTimestamp();
        SPL::float64 ckptTime = SPL::Functions::Time::diffAsSecs(endTime, startTime);
        SPLCKPTTRC(
          L_INFO, op_.getContext().getName(),
          "Operator checkpointing is complete: SequenceID = "
            << id << ", Size = " << lastCkptSizeNorm_ + lastCkptSizeIncr_ + lastCkptSizeIndex_
            << " Bytes (non-incremental checkpoint data: " << lastCkptSizeNorm_
            << " Bytes, incremental checkpoint data: " << lastCkptSizeIncr_ + lastCkptSizeIndex_
            << " Bytes), Time = " << ckptTime << " Seconds");
        return rc;
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(op_.getContext().getName(),
                                        "Cannot create checkpoint with Sequence ID " +
                                          boost::lexical_cast<std::string>(id),
                                        e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(),
                                 "Cannot create checkpoint with Sequence ID " +
                                   boost::lexical_cast<std::string>(id),
                                 e.what());
    }
    return true;
}

bool CheckpointContextImpl::createCheckpoint()
{
    assert(storeEntry_.get() != NULL);

    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Creating checkpoint ...");

    // measure checkpoint start time
    SPL::timestamp startTime = SPL::Functions::Time::getTimestamp();
    int64_t sequenceID = checkpointCounter_ + 1;
    checkpointCounter_ = sequenceID;
    try {
        // begin a batch by itself
        CheckpointBatch batch;
        batch.begin(sequenceID);

        // create a new checkpoint
        bool rc = createCheckpointInternal(sequenceID, &batch);

        // commit the batch
        batch.commit();

        // update meta-data
        int64_t temp = Streams::host_to_network<int64_t>(sequenceID).data;
        storeEntry_->put(CheckpointNaming::getMetaDataKey(), reinterpret_cast<char*>(&temp),
                         sizeof(temp));

        // delete old checkpoint asynchronously
        if (checkpointCounter_ != 1) {
            try {
                deleteCheckpointAsync(checkpointCounter_ - 1);
            } catch (DataStoreException const& e) {
                SPLCKPTLOGMSG(L_ERROR, op_.getContext().getName(),
                              "Cannot delete checkpoint with Sequence ID " +
                                boost::lexical_cast<std::string>(checkpointCounter_),
                              e.getExplanation());
                SPLCKPTTRCMSG(L_ERROR, op_.getContext().getName(),
                              "Cannot delete checkpoint with Sequence ID " +
                                boost::lexical_cast<std::string>(checkpointCounter_),
                              e.getExplanation());
            }
        }

        // measure checkpoint end time
        SPL::timestamp endTime = SPL::Functions::Time::getTimestamp();
        SPL::float64 ckptTime = SPL::Functions::Time::diffAsSecs(endTime, startTime);
        SPLCKPTTRC(L_INFO, op_.getContext().getName(),
                   "Operator checkpointing is complete: SequenceID = "
                     << sequenceID
                     << ", Size = " << lastCkptSizeNorm_ + lastCkptSizeIncr_ + lastCkptSizeIndex_
                     << " Bytes (non-incremental checkpoint data: " << lastCkptSizeNorm_
                     << " Bytes, incremental checkpoint data: "
                     << lastCkptSizeIncr_ + lastCkptSizeIndex_ << " Bytes), Time = " << ckptTime
                     << " Seconds");
        return rc;
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(op_.getContext().getName(),
                                        "Cannot create checkpoint with Sequence ID " +
                                          boost::lexical_cast<std::string>(sequenceID),
                                        e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(),
                                 "Cannot create checkpoint with Sequence ID " +
                                   boost::lexical_cast<std::string>(sequenceID),
                                 e.what());
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(),
                                 "Cannot create checkpoint with Sequence ID " +
                                   boost::lexical_cast<std::string>(sequenceID),
                                 "unknown exception");
    }
    return true;
}

bool CheckpointContextImpl::restoreCheckpoint(const int64_t& id)
{
    assert(storeEntry_.get() != NULL);

    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Restoring checkpoint with Sequence ID " << id);
    try {
        Checkpoint ckpt(storeEntry_.get(), id, false);

        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Calling Operator's resetRaw() callback");
        op_.resetRaw(ckpt);

        // as performance optimization, record checkpoint size so we can determine the initial size
        // of internal buffer more accurately to reduce dynamic memory allocation overhead
        lastCkptSizeNorm_ = ckpt.getNormDataSize();
        if (ckpt.isBase()) {
            lastCkptSizeIncr_ = lastCkptSizeBase_ = ckpt.getIncrementalDataSize();
        } else {
            lastCkptSizeIncr_ = lastCkptSizeDelta_ = ckpt.getIncrementalDataSize();
        }
        lastCkptSizeIndex_ = ckpt.getIncrementalIndexSize();

        numSuccessCkpt_ = ckpt.getCheckpointCount();

        // reset the incremental ckpt interval setter
        int64_t incrCkptInterval = ckpt.getIncrementalCheckpointInterval();
        incrementalCkptInterval_ = intervalSetter_.reset(incrCkptInterval);

        // garbage-collect any checkpoints prior to the one just restored
        // it will also restore checkpoint IDs into ckptIDs_
        deleteCheckpointsPriorTo(ckpt.getBaseID());

        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Operator restoring is complete.");
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(op_.getContext().getName(),
                                        "Cannot restore checkpoint with Sequence ID " +
                                          boost::lexical_cast<std::string>(id),
                                        e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(),
                                 "Cannot restore checkpoint with Sequence ID " +
                                   boost::lexical_cast<std::string>(id),
                                 e.what());
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(),
                                 "Cannot restore checkpoint with Sequence ID " +
                                   boost::lexical_cast<std::string>(id),
                                 "unknown exception");
    }
    return true;
}

bool CheckpointContextImpl::restoreCheckpoint()
{
    assert(storeEntry_.get() != NULL);

    SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Restoring checkpoint ...");

    std::vector<std::pair<char*, uint64_t> > values;
    try {
        // read all possible copies of meta-data
        bool isExisting;
        storeEntry_->get(CheckpointNaming::getMetaDataKey(), values, isExisting);
        if (isExisting == true) {
            // pick the latest sequence id
            int64_t latestCkptID = -1;
            for (std::vector<std::pair<char*, uint64_t> >::iterator it = values.begin();
                 it != values.end(); ++it) {
                if (it->second != sizeof(int64_t)) {
                    THROW_CHAR(DataStore, "meta-data retrieved is not valid");
                }
                int64_t temp = *(reinterpret_cast<int64_t*>(it->first));
                latestCkptID = Streams::network_to_host<int64_t>(temp).data;
                if (checkpointCounter_ < latestCkptID) {
                    checkpointCounter_ = latestCkptID;
                }
            }
            for (std::vector<std::pair<char*, uint64_t> >::iterator it = values.begin();
                 it != values.end(); ++it) {
                delete[] it->first;
            }
            values.clear();

            // restore from the latest checkpoint
            restoreCheckpoint(checkpointCounter_);
        } else {
            SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                       "Calling Operator's resetToInitialStateRaw()");
            op_.resetToInitialStateRaw();
            SPLCKPTTRC(L_DEBUG, op_.getContext().getName(), "Operator restoring is complete.");
        }
    } catch (DataStoreException const& e) {
        for (std::vector<std::pair<char*, uint64_t> >::iterator it = values.begin();
             it != values.end(); ++it) {
            delete[] it->first;
        }
        SPLCKPT_HANDLE_EXCEPTION_NESTED(op_.getContext().getName(), "Cannot restore checkpoint",
                                        e.getExplanation(), e);
    } catch (std::exception const& e) {
        for (std::vector<std::pair<char*, uint64_t> >::iterator it = values.begin();
             it != values.end(); ++it) {
            delete[] it->first;
        }
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(), "Cannot restore checkpoint", e.what());
    } catch (...) {
        for (std::vector<std::pair<char*, uint64_t> >::iterator it = values.begin();
             it != values.end(); ++it) {
            delete[] it->first;
        }
        SPLCKPT_HANDLE_EXCEPTION(op_.getContext().getName(), "Cannot restore checkpoint",
                                 "unknown exception");
    }
    return true;
}

bool CheckpointContextImpl::isEnabled() const
{
    return isEnabled_;
}

void CheckpointContextImpl::initialize()
{
    try {
        DataStoreAdapter* adapter = CheckpointConfig::instance()->getDataStoreAdapter();
        // for file system backend, create a common parent directory
        if (adapter->getDSAType() == "fileSystem") {
            FileSystemDataStoreAdapter* fsAdapter =
              static_cast<FileSystemDataStoreAdapter*>(adapter);
            std::string jobCkptDir = CheckpointNaming::getCkptStoreEntryNamePrefix(
              ProcessingElement::pe().getDomainID(), ProcessingElement::pe().getInstanceID(),
              ProcessingElement::pe().getJobId());
            fsAdapter->createDirectory(jobCkptDir);
        }
    } catch (DataStoreException const& e) {
        std::string errMsg =
          "Cannot initialize checkpointing API: received exception: " + e.getExplanation();
        SPLCKPTLOG(L_ERROR, "", errMsg);
        THROW_NESTED(DataStore, "Cannot initialize checkpointing API", e);
    }
}

void CheckpointContextImpl::finalize()
{
    try {
        DataStoreAdapter* adapter = CheckpointConfig::instance()->getDataStoreAdapter();
        if (adapter->getDSAType() == "fileSystem") {
            FileSystemDataStoreAdapter* fsAdapter =
              static_cast<FileSystemDataStoreAdapter*>(adapter);
            std::string jobCkptDir = CheckpointNaming::getCkptStoreEntryNamePrefix(
              ProcessingElement::pe().getDomainID(), ProcessingElement::pe().getInstanceID(),
              ProcessingElement::pe().getJobId());
            fsAdapter->removeDirectory(jobCkptDir, false);
        }
    } catch (DataStoreException const& e) {
        std::string errMsg =
          "Cannot finalize checkpointing API: received exception: " + e.getExplanation();
        SPLCKPTLOG(L_ERROR, "", errMsg);
        THROW_NESTED(DataStore, "Cannot finalize checkpointing API", e);
    }
}

void CheckpointContextImpl::deleteCheckpointsPriorTo(const int64_t& id)
{
    assert(storeEntry_ != NULL);

    try {
        std::tr1::unordered_set<std::string> headerKeys;
        std::tr1::unordered_set<DataStoreEntry::ChunkKey> chunkKeys;
        std::tr1::unordered_set<std::string> headersToDelete;
        std::tr1::unordered_set<DataStoreEntry::ChunkKey> chunksToDelete;
        std::set<int64_t> currentCkptIDs;
        storeEntry_->getByteBufferKeys(&headerKeys, &chunkKeys);
        for (std::tr1::unordered_set<std::string>::const_iterator iter = headerKeys.begin();
             iter != headerKeys.end(); ++iter) {
            enum CheckpointNaming::KeyType type;
            int64_t seqID = CheckpointNaming::parseDataKey(*iter, type);
            if (seqID < id) {
                headersToDelete.insert(*iter);
            } else {
                currentCkptIDs.insert(seqID);
            }
        }
        for (std::tr1::unordered_set<DataStoreEntry::ChunkKey>::const_iterator iter =
               chunkKeys.begin();
             iter != chunkKeys.end(); ++iter) {
            enum CheckpointNaming::KeyType type;
            int64_t seqID = CheckpointNaming::parseDataKey(iter->first, type);
            if (seqID < id) {
                chunksToDelete.insert(*iter);
            } else {
                currentCkptIDs.insert(seqID);
            }
        }
        if (headersToDelete.size() > 0 || chunksToDelete.size() > 0) {
            storeEntry_->remove(&headersToDelete, &chunksToDelete, NULL);
        }
        ckptIDs_->push_back(std::make_pair(id, uint32_t(1)));
        for (std::set<int64_t>::const_iterator iter = currentCkptIDs.begin();
             iter != currentCkptIDs.end(); ++iter) {
            std::pair<int64_t, uint32_t>& last = ckptIDs_->back();
            if (last.first + last.second == *iter) { // consecutive
                last.second++;
            } else { // add a new (start, runLength) pair
                ckptIDs_->push_back(std::make_pair(*iter, uint32_t(1)));
            }
        }
    } catch (DataStoreException const& e) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoints prior to Sequence ID "
                     << id << ": received exception: " << e.getExplanation());
    } catch (std::exception const& e) {
        SPLCKPTTRC(L_DEBUG, op_.getContext().getName(),
                   "Failed to delete checkpoints prior to Sequence ID "
                     << id << ": received exception: " << e.what());
    }
}
