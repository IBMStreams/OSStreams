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
 * Implementation of SPL::CheckpointBatch class
 */

#include <SPL/Runtime/Operator/State/CheckpointBatch.h>
#include <SPL/Runtime/Operator/State/CheckpointConfig.h>
#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatch.h>
#include <assert.h>

using namespace std;
using namespace SPL;

CheckpointBatch::CheckpointBatch()
  : seqID_(0)
  , state_(NO_BATCH)
  , storeAdapter_(CheckpointConfig::instance()->getDataStoreAdapter())
  , batchImpl_(NULL)
{}

CheckpointBatch::~CheckpointBatch() {}

void CheckpointBatch::begin(int64_t seqID)
{
    assert(storeAdapter_ != NULL);

    if (batchImpl_) {
        THROW(DataStore, "begin() failed: there is an ongoing batch");
    }
    seqID_ = seqID;
    try {
        batchImpl_.reset(storeAdapter_->createUpdateBatch());
        state_ = INPROGRESS;
    } catch (DataStoreException const& e) {
        state_ = ERROR;
        THROW_NESTED(DataStore, "begin() failed", e);
    }
}

void CheckpointBatch::commit()
{
    assert(storeAdapter_ != NULL);
    assert(batchImpl_);

    if (state_ != INPROGRESS) {
        THROW(DataStore, "commit() failed with checkpoint sequence id "
                           << seqID_ << ": the batch is in " << getStateStr()
                           << " state and cannot be committed");
    }
    try {
        batchImpl_->commit();
        batchImpl_.reset(NULL);
        state_ = COMMITTED;
    } catch (DataStoreException const& e) {
        batchImpl_.reset(NULL);
        state_ = ERROR;
        THROW_NESTED(DataStore, "commit() failed", e);
    }
}

void CheckpointBatch::abort()
{
    assert(storeAdapter_ != NULL);
    assert(batchImpl_);

    if (state_ == COMMITTED || state_ == ABORTED || state_ == NO_BATCH) {
        THROW(DataStore, "abort() failed with checkpoint sequence id "
                           << seqID_ << ": the batch is in " << getStateStr()
                           << " state and cannot be aborted");
    }
    try {
        batchImpl_->abort();
        batchImpl_.reset(NULL);
        state_ = ABORTED;
    } catch (DataStoreException const& e) {
        batchImpl_.reset(NULL);
        state_ = ERROR;
        THROW_NESTED(DataStore, "abort() failed", e);
    }
}

void CheckpointBatch::wait()
{
    assert(storeAdapter_ != NULL);
    assert(batchImpl_);

    if (state_ != INPROGRESS) {
        THROW(DataStore, "wait() failed with checkpoint sequence id "
                           << seqID_ << ": the batch is in " << getStateStr() << " state");
    }
    try {
        batchImpl_->wait();
    } catch (DataStoreException const& e) {
        state_ = ERROR;
        THROW_NESTED(DataStore, "wait() failed", e);
    }
}

enum CheckpointBatch::State CheckpointBatch::getState() const
{
    return state_;
}

std::string CheckpointBatch::getStateStr() const
{
    switch (state_) {
        case INPROGRESS:
            return "INPROGRESS";
        case ERROR:
            return "ERROR";
        case ABORTED:
            return "ABORTED";
        case COMMITTED:
            return "COMMITTED";
        case NO_BATCH:
            return "NO_BATCH";
        default:
            return "INVALID";
    }
}

void CheckpointBatch::setState(enum CheckpointBatch::State state)
{
    state_ = state;
}

DataStoreUpdateBatch* CheckpointBatch::getBatchImpl()
{
    return batchImpl_.get();
}
