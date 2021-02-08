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
 * Implementation of SPL::CheckpointThread and related classes
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/State/CheckpointLogTrace.h>
#include <SPL/Runtime/Operator/State/CheckpointThread.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContextImpl.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionEventHandler.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/ProcessingElement/PEConsistentRegionService.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <boost/lexical_cast.hpp>

using namespace SPL;
using namespace std;

__thread CheckpointThreadState* CheckpointThread::state_ = NULL;

CheckpointThreadState::CheckpointThreadState() {}

CheckpointThreadState::~CheckpointThreadState()
{
    for (std::tr1::unordered_map<int32_t, CheckpointThreadState::CRegionState>::iterator iter =
           regionState_.begin();
         iter != regionState_.end(); ++iter) {
        delete iter->second.batch_;
    }
}

CheckpointThread::CheckpointThread()
  : UTILS_NAMESPACE::WorkerThread()
{}

CheckpointThread::~CheckpointThread() {}

CheckpointThreadState* CheckpointThread::getCheckpointThreadState()
{
    return state_;
}

void CheckpointThread::setCheckpointThreadState(CheckpointThreadState* state)
{
    if (state_ != NULL) {
        delete state_;
    }
    state_ = state;
}

void* CheckpointThread::run(void* threadArgs)
{
    APPTRC(L_DEBUG, "Non-blocking checkpointing thread started ...", SPL_CKPT);
    OperatorTracker::init();
    // set thread local state
    setCheckpointThreadState(new CheckpointThreadState());

    // the loop for processing work items
    void* ret = UTILS_NAMESPACE::WorkerThread::run(threadArgs);

    // clean up thread local state
    setCheckpointThreadState(NULL);
    OperatorTracker::finalize();
    APPTRC(L_DEBUG, "Non-blocking checkpointing thread ended ...", SPL_CKPT);
    return ret;
}

CheckpointThreadFactory::CheckpointThreadFactory() {}

UTILS_NAMESPACE::WorkerThread* CheckpointThreadFactory::createThread()
{
    return static_cast<UTILS_NAMESPACE::WorkerThread*>(new CheckpointThread());
}

CheckpointCreationWorkItem::CheckpointCreationWorkItem(PEImpl& pe,
                                                       OperatorImpl& opImpl,
                                                       CheckpointContextImpl& ckptContext,
                                                       ConsistentRegionContextImpl& crContext,
                                                       const int64_t seqID)
  : pe_(pe)
  , opImpl_(opImpl)
  , ckptContext_(ckptContext)
  , crContext_(crContext)
  , seqID_(seqID)
{}

CheckpointCreationWorkItem::~CheckpointCreationWorkItem() {}

void CheckpointCreationWorkItem::satisfy()
{
    SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
               "Checkpoint stage started [" << seqID_ << "]");
    int32_t regionID = crContext_.getIndex();
    if (!pe_.getConsistentRegionService().shouldProceedWithCheckpoint(regionID, seqID_)) {
        SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
                   "Checkpoint stage skipped [" << seqID_ << "]");
        return;
    }
    SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
               "Proceeding with checkpoint [" << seqID_ << "]");
    OperatorTracker::setCurrentOperator(opImpl_.getContext().getIndex());
    try {
        ckptContext_.createCheckpoint(seqID_);
        SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
                   "Checkpoint stage ended [" << seqID_ << "]");
        crContext_.checkpointCompleted(seqID_);
    } catch (SPLRuntimeShutdownException const& e) {
        SPLCKPTTRC(L_INFO, opImpl_.getContext().getName(),
                   "Exception received while the PE is shutting down: " << e);
    } catch (DataStoreException const& e) {
        std::string msg = "Cannot create checkpoint with Sequence ID " +
                          boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                          boost::lexical_cast<std::string>(regionID);
        SPLCKPTLOGMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.getExplanation());
        SPLCKPTTRCMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.getExplanation());
        if (!pe_.getShutdownRequested()) {
            pe_.handleOperatorFailure(msg, e.getExplanation());
            pe_.shutdownFromWithinOperators();
        }
    } catch (std::exception const& e) {
        std::string msg = "Cannot create checkpoint with Sequence ID " +
                          boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                          boost::lexical_cast<std::string>(regionID);
        SPLCKPTLOGMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.what());
        SPLCKPTTRCMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.what());
        if (!pe_.getShutdownRequested()) {
            pe_.handleOperatorFailure(msg, e.what());
            pe_.shutdownFromWithinOperators();
        }
    } catch (...) {
        std::string msg = "Cannot create checkpoint with Sequence ID " +
                          boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                          boost::lexical_cast<std::string>(regionID);
        SPLCKPTLOGMSG(L_ERROR, opImpl_.getContext().getName(), msg, "unknown exception");
        SPLCKPTTRCMSG(L_ERROR, opImpl_.getContext().getName(), msg, "unknown exception");
        if (!pe_.getShutdownRequested()) {
            pe_.handleOperatorFailure(msg, "unknown exception");
            pe_.shutdownFromWithinOperators();
        }
    }
    OperatorTracker::resetCurrentOperator();
}

CheckpointResetWorkItem::CheckpointResetWorkItem(PEImpl& pe,
                                                 OperatorImpl& opImpl,
                                                 ConsistentRegionContextImpl& crContext,
                                                 ConsistentRegionEventHandler& crEventHandler,
                                                 const int64_t seqID,
                                                 const int32_t resetAttempt)
  : pe_(pe)
  , opImpl_(opImpl)
  , crContext_(crContext)
  , crEventHandler_(crEventHandler)
  , seqID_(seqID)
  , resetAttempt_(resetAttempt)
{}

CheckpointResetWorkItem::~CheckpointResetWorkItem() {}

void CheckpointResetWorkItem::satisfy()
{
    SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
               "Reset stage started [" << seqID_ << "," << resetAttempt_ << "]");
    if (!pe_.getConsistentRegionService().shouldProceedWithReset(crContext_.getIndex(), seqID_,
                                                                 resetAttempt_)) {
        SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
                   "Reset stage skipped [" << seqID_ << "," << resetAttempt_ << "]");
        return;
    }
    SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
               "Proceeding with reset [" << seqID_ << "," << resetAttempt_ << "]");
    OperatorTracker::setCurrentOperator(opImpl_.getContext().getIndex());
    try {
        crEventHandler_.resetOperatorState(seqID_);
        crContext_.resetCompleted(seqID_, resetAttempt_);
        SPLCKPTTRC(L_DEBUG, opImpl_.getContext().getName(),
                   "Reset stage ended [" << seqID_ << "," << resetAttempt_ << "]");
    } catch (SPLRuntimeShutdownException const& e) {
        SPLCKPTTRC(L_INFO, opImpl_.getContext().getName(),
                   "Exception received while the PE is shutting down: " << e);
    } catch (SPLRuntimeException const& e) {
        std::string msg =
          "Cannot restore checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID_);
        SPLCKPTLOGMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.getExplanation());
        SPLCKPTTRCMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.getExplanation());
        if (!pe_.getShutdownRequested()) {
            pe_.handleOperatorFailure(msg, e.getExplanation());
            pe_.shutdownFromWithinOperators();
        }
    } catch (std::exception const& e) {
        std::string msg =
          "Cannot restore checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID_);
        SPLCKPTLOGMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.what());
        SPLCKPTTRCMSG(L_ERROR, opImpl_.getContext().getName(), msg, e.what());
        if (!pe_.getShutdownRequested()) {
            pe_.handleOperatorFailure(msg, e.what());
            pe_.shutdownFromWithinOperators();
        }
    } catch (...) {
        std::string msg =
          "Cannot restore checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID_);
        SPLCKPTLOGMSG(L_ERROR, opImpl_.getContext().getName(), msg, "unknown exception");
        SPLCKPTTRCMSG(L_ERROR, opImpl_.getContext().getName(), msg, "unknown exception");
        if (!pe_.getShutdownRequested()) {
            pe_.handleOperatorFailure(msg, "unknown exception");
            pe_.shutdownFromWithinOperators();
        }
    }
    OperatorTracker::resetCurrentOperator();
}

CheckpointBeginWorkItem::CheckpointBeginWorkItem(PEImpl& pe,
                                                 const int64_t seqID,
                                                 const int32_t regionID)
  : pe_(pe)
  , seqID_(seqID)
  , regionID_(regionID)
{}

CheckpointBeginWorkItem::~CheckpointBeginWorkItem() {}

void CheckpointBeginWorkItem::satisfy()
{
    APPTRC(L_DEBUG, "Beginning a checkpoint batch [" << seqID_ << "]", SPL_CKPT);
    try {
        CheckpointThreadState* state = CheckpointThread::getCheckpointThreadState();
        assert(state != NULL);
        std::tr1::unordered_map<int32_t, CheckpointThreadState::CRegionState>::iterator iter =
          state->regionState_.find(regionID_);
        CheckpointBatch* batch = NULL;
        if (iter != state->regionState_.end()) {
            batch = iter->second.batch_;
        } else {
            batch = new CheckpointBatch();
            iter =
              state->regionState_
                .insert(std::make_pair(regionID_, CheckpointThreadState::CRegionState(batch, 0)))
                .first;
        }
        if (iter->second.currentSeqID_ < seqID_) {
            batch->begin(seqID_);
            iter->second.currentSeqID_ = seqID_;
        }
        APPTRC(L_DEBUG, "Checkpoint batch begun [" << seqID_ << "]", SPL_CKPT);
    } catch (DataStoreException const& e) {
        SPLCKPTLOGMSG(L_ERROR, "",
                      "Cannot begin checkpoint batch with Sequence ID " +
                        boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                        boost::lexical_cast<std::string>(regionID_),
                      e.getExplanation());
        SPLCKPTTRCMSG(L_ERROR, "",
                      "Cannot begin checkpoint batch with Sequence ID " +
                        boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                        boost::lexical_cast<std::string>(regionID_),
                      e.getExplanation());
        if (!pe_.getShutdownRequested()) {
            pe_.shutdownFromWithinOperators();
        }
    } catch (std::exception const& e) {
        SPLCKPTLOGMSG(L_ERROR, "",
                      "Cannot begin checkpoint batch with Sequence ID " +
                        boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                        boost::lexical_cast<std::string>(regionID_),
                      e.what());
        SPLCKPTTRCMSG(L_ERROR, "",
                      "Cannot begin checkpoint batch with Sequence ID " +
                        boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                        boost::lexical_cast<std::string>(regionID_),
                      e.what());
        if (!pe_.getShutdownRequested()) {
            pe_.shutdownFromWithinOperators();
        }
    }
}

CheckpointCommitWorkItem::CheckpointCommitWorkItem(PEImpl& pe,
                                                   const int64_t seqID,
                                                   const int32_t regionID)
  : pe_(pe)
  , seqID_(seqID)
  , regionID_(regionID)
{}

CheckpointCommitWorkItem::~CheckpointCommitWorkItem() {}

void CheckpointCommitWorkItem::satisfy()
{
    APPTRC(L_DEBUG, "Committing a checkpoint batch [" << seqID_ << "]", SPL_CKPT);
    CheckpointThreadState* state = CheckpointThread::getCheckpointThreadState();
    assert(state != NULL);
    std::tr1::unordered_map<int32_t, CheckpointThreadState::CRegionState>::iterator iter =
      state->regionState_.find(regionID_);
    CheckpointBatch* batch = NULL;
    if (iter != state->regionState_.end() && iter->second.currentSeqID_ == seqID_) {
        batch = iter->second.batch_;
        try {
            batch->commit();
            APPTRC(L_DEBUG, "Checkpoint batch committed [" << seqID_ << "]", SPL_CKPT);
        } catch (DataStoreException const& e) {
            SPLCKPTLOGMSG(L_ERROR, "",
                          "Cannot commit checkpoint batch with Sequence ID " +
                            boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                            boost::lexical_cast<std::string>(regionID_),
                          e.getExplanation());
            SPLCKPTTRCMSG(L_ERROR, "",
                          "Cannot commit checkpoint batch with Sequence ID " +
                            boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                            boost::lexical_cast<std::string>(regionID_),
                          e.getExplanation());
            if (!pe_.getShutdownRequested()) {
                pe_.shutdownFromWithinOperators();
            }
        }
    } else {
        SPLCKPTLOGMSG(L_ERROR, "",
                      "Cannot commit checkpoint batch with Sequence ID " +
                        boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                        boost::lexical_cast<std::string>(regionID_),
                      "the checkpoint batch is invalid");
        SPLCKPTTRCMSG(L_ERROR, "",
                      "Cannot commit checkpoint batch with Sequence ID " +
                        boost::lexical_cast<std::string>(seqID_) + " for Consistent Region " +
                        boost::lexical_cast<std::string>(regionID_),
                      "the checkpoint batch is invalid");
        if (!pe_.getShutdownRequested()) {
            pe_.shutdownFromWithinOperators();
        }
    }
}

CheckpointAbortWorkItem::CheckpointAbortWorkItem(PEImpl& pe,
                                                 const int64_t seqID,
                                                 const int32_t regionID)
  : pe_(pe)
  , seqID_(seqID)
  , regionID_(regionID)
{}

CheckpointAbortWorkItem::~CheckpointAbortWorkItem() {}

void CheckpointAbortWorkItem::satisfy()
{
    APPTRC(L_DEBUG, "Aborting a checkpoint batch [" << seqID_ << "]", SPL_CKPT);
    CheckpointThreadState* state = CheckpointThread::getCheckpointThreadState();
    assert(state != NULL);
    std::tr1::unordered_map<int32_t, CheckpointThreadState::CRegionState>::iterator iter =
      state->regionState_.find(regionID_);
    CheckpointBatch* batch = NULL;
    if (iter != state->regionState_.end()) {
        if (iter->second.currentSeqID_ == seqID_) {
            batch = iter->second.batch_;
            try {
                batch->abort();
                APPTRC(L_DEBUG, "Checkpoint batch aborted [" << seqID_ << "]", SPL_CKPT);
            } catch (DataStoreException const& e) {
                SPLCKPTLOGMSG(L_ERROR, "",
                              "Cannot abort checkpoint batch with Sequence ID " +
                                boost::lexical_cast<std::string>(seqID_) +
                                " for Consistent Region " +
                                boost::lexical_cast<std::string>(regionID_),
                              e.getExplanation());
                SPLCKPTTRCMSG(L_ERROR, "",
                              "Cannot abort checkpoint batch with Sequence ID " +
                                boost::lexical_cast<std::string>(seqID_) +
                                " for Consistent Region " +
                                boost::lexical_cast<std::string>(regionID_),
                              e.getExplanation());
                if (!pe_.getShutdownRequested()) {
                    pe_.shutdownFromWithinOperators();
                }
            }
        }
    }
}
