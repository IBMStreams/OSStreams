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
 * Implementation of SPL::UpdateLog class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Checkpoint.h>
#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/UpdateLog.h>
#include <boost/scoped_ptr.hpp>
#include <exception>

using namespace std;
using namespace SPL;

UpdateLog::UpdateLog(void* param)
  : offset_(0)
  , indexOffset_(0)
  , prevID_(0)
  , prevIndexOffset_(0)
  , baseID_(0)
  , baseOffset_(0)
  , isBase_(true)
  , isNested_(false)
  , isEnabled_(false)
  , ckpt_(NULL)
{}

UpdateLog::~UpdateLog() {}

void UpdateLog::clear() {}

void UpdateLog::serialize(Checkpoint& ckpt, void* data) {}

void UpdateLog::deserializeAndApply(Checkpoint& ckpt, void* data) {}

void UpdateLog::startCheckpoint(Checkpoint& ckpt)
{
    isNested_ = ckpt.isIncrementalMode();
    ckpt.setIncrementalMode();
    ckpt.deltaBuffer_->getContentSize(offset_);
    prevIndexOffset_ = indexOffset_;
    ckpt.indexBuffer_->getContentSize(indexOffset_);
    isBase_ = ckpt.isBase() || (isEnabled_ == false);

    // write the index to index byte buffer
    int64_t id = ckpt.getSequenceId();
    if (isBase_ == false) {
        if (index_.getType() == IncrementalCheckpointIndex::BASE) {
            // the first delta checkpoint
            index_.setType(IncrementalCheckpointIndex::FIRST_DELTA);
        } else {
            index_.setType(IncrementalCheckpointIndex::DELTA);
        }
        index_.setPrevID(prevID_);
        index_.setPrevIndexOffset(prevIndexOffset_);
    } else {
        index_.setType(IncrementalCheckpointIndex::BASE);
        index_.setPrevID(0);
        index_.setPrevIndexOffset(0);
    }
    prevID_ = id;
    index_.setOffset(offset_);
    index_.serialize(*(ckpt.indexBuffer_));
}

void UpdateLog::finishCheckpoint(Checkpoint& ckpt)
{
    if (isNested_ == false) {
        ckpt.unsetIncrementalMode();
    }
    isEnabled_ = ckpt.shouldEnableLogging();
}

void UpdateLog::startReset(Checkpoint& ckpt)
{
    isNested_ = ckpt.isIncrementalMode();
    ckpt.setIncrementalMode();
    // read the index
    index_.deserialize(*(ckpt.indexBuffer_));
    isBase_ = index_.isBase();
    ckpt_ = &ckpt;
    if (isBase_ == false) {
        // get previous indices in reverse order
        int64_t prevID = index_.getPrevID();
        uint64_t prevIndexOffset = index_.getPrevIndexOffset();
        uint32_t i = 1;
        baseID_ = 0;
        while (prevID != 0 && i <= ckpt.getIncrementalCheckpointInterval()) {
            // read the index byte buffer of the prevID
            DataStoreByteBuffer* indexBuffer = ckpt.getIndexBuffer(prevID, prevIndexOffset);
            IncrementalCheckpointIndex prevIndex;
            prevIndex.deserialize(*indexBuffer);
            if (prevIndex.isBase()) {
                baseID_ = prevID;
                break;
            }
            prevIDs_.push_back(prevID);
            prevIndices_.push_back(prevIndex);
            prevID = prevIndex.getPrevID();
            prevIndexOffset = prevIndex.getPrevIndexOffset();
            i++;
        }
        if (baseID_ != 0) { // read the base index
            DataStoreByteBuffer* baseIndexBuffer = ckpt.getIndexBuffer(baseID_, prevIndexOffset);
            IncrementalCheckpointIndex baseIndex;
            baseIndex.deserialize(*baseIndexBuffer);
            baseOffset_ = baseIndex.getOffset();
        } else {
            THROW(DataStore, "startReset() failed: cannot find base checkpoint");
        }
    }
}

void UpdateLog::finishReset(Checkpoint& ckpt)
{
    // reset the state to an empty log
    ckpt_ = NULL;
    prevIDs_.clear();
    prevIndices_.clear();
    clear();
    if (isNested_ == false) {
        ckpt.unsetIncrementalMode();
    }
}

bool UpdateLog::isBase() const
{
    return isBase_;
}

Checkpoint* UpdateLog::getBaseCheckpoint()
{
    if (isBase() == false) {
        return new Checkpoint(*ckpt_, baseID_, baseOffset_);
    } else {
        return NULL;
    }
}

bool UpdateLog::hasPreviousDeltaCheckpoint() const
{
    return !prevIDs_.empty();
}

Checkpoint* UpdateLog::nextPreviousDeltaCheckpoint()
{
    // advance to next "previous delta checkpoint"
    if (prevIDs_.empty() == false) {
        int64_t seqID = prevIDs_.back();
        uint64_t offset = prevIndices_.back().getOffset();
        prevIDs_.pop_back();
        prevIndices_.pop_back();
        return new Checkpoint(*ckpt_, seqID, offset);
    } else {
        return NULL;
    }
}

void UpdateLog::onExceptionDuringLogging()
{
    // disable logging
    isEnabled_ = false;
    clear();
}
