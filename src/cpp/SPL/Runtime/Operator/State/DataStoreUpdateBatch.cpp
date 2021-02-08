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

#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatch.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <assert.h>
#include <sstream>

using namespace std;
using namespace SPL;

DataStoreUpdateBatch::DataStoreUpdateBatch(DataStoreUpdateBatchImpl* impl)
  : impl_(impl)
{}

DataStoreUpdateBatch::~DataStoreUpdateBatch()
{
    if (impl_) {
        delete impl_;
    }
}

void DataStoreUpdateBatch::commit()
{
    assert(impl_ != NULL);

    impl_->commit();
}

void DataStoreUpdateBatch::abort()
{
    assert(impl_ != NULL);

    impl_->abort();
}

void DataStoreUpdateBatch::wait()
{
    assert(impl_ != NULL);

    impl_->wait();
}

enum DataStoreUpdateBatch::State DataStoreUpdateBatch::getState() const
{
    assert(impl_ != NULL);

    return impl_->getState();
}

std::string DataStoreUpdateBatch::getStateStr() const
{
    assert(impl_ != NULL);

    return impl_->getStateStr();
}

DataStoreUpdateBatchImpl* DataStoreUpdateBatch::getImpl() const
{
    assert(impl_ != NULL);

    return impl_;
}
