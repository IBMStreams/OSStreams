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
 * Implementation of SPL::DataStoreUpdateBatch class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <assert.h>
#include <exception>
#include <sstream>
#include <vector>

using namespace std;
using namespace SPL;

DataStoreUpdateBatchImpl::DataStoreUpdateBatchImpl(DataStoreAdapter* adapter)
  : state_(DataStoreUpdateBatch::INPROGRESS)
  , storeAdapter_(adapter)
{
    assert(adapter);
}

DataStoreUpdateBatchImpl::~DataStoreUpdateBatchImpl() {}

void DataStoreUpdateBatchImpl::commit()
{
    if (state_ == DataStoreUpdateBatch::INPROGRESS) {
        state_ = DataStoreUpdateBatch::COMMITTED;
    }
}

void DataStoreUpdateBatchImpl::abort()
{
    if (state_ == DataStoreUpdateBatch::INPROGRESS || state_ == DataStoreUpdateBatch::ERROR) {
        state_ = DataStoreUpdateBatch::ABORTED;
    }
}

void DataStoreUpdateBatchImpl::wait() {}

enum DataStoreUpdateBatch::State DataStoreUpdateBatchImpl::getState() const
{
    return state_;
}

std::string DataStoreUpdateBatchImpl::getStateStr() const
{
    switch (state_) {
        case DataStoreUpdateBatch::INPROGRESS:
            return "INPROGRESS";
        case DataStoreUpdateBatch::ERROR:
            return "ERROR";
        case DataStoreUpdateBatch::ABORTED:
            return "ABORTED";
        case DataStoreUpdateBatch::COMMITTED:
            return "COMMITTED";
        default:
            return "INVALID";
    }
}

void DataStoreUpdateBatchImpl::setState(enum DataStoreUpdateBatch::State state)
{
    state_ = state;
}

const DataStoreAdapter* DataStoreUpdateBatchImpl::getDataStoreAdapter() const
{
    return storeAdapter_;
}
