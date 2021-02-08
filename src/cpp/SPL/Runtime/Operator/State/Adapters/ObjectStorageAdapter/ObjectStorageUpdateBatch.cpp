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
 * Implementation of the SPL::ObjectStorageUpdateBatch class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/ObjectStorageAdapter/ObjectStorageDataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/Adapters/ObjectStorageAdapter/ObjectStorageUpdateBatch.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <utility>

using namespace std;
using namespace SPL;

ObjectStorageUpdateBatch::ObjectStorageUpdateBatch(ObjectStorageDataStoreAdapter* adapter)
  : DataStoreUpdateBatchImpl(static_cast<DataStoreAdapter*>(adapter))
{
    APPTRC(L_TRACE, "ObjectStorageUpdateBatch::ObjectStorageUpdateBatch", SPL_CKPT);
}

ObjectStorageUpdateBatch::~ObjectStorageUpdateBatch()
{
    APPTRC(L_TRACE, "ObjectStorageUpdateBatch::~ObjectStorageUpdateBatch", SPL_CKPT);
}

void ObjectStorageUpdateBatch::wait()
{
    APPTRC(L_TRACE, "ObjectStorageUpdateBatch::wait", SPL_CKPT);
}

void ObjectStorageUpdateBatch::commit()
{
    APPTRC(L_TRACE, "ObjectStorageUpdateBatch::commit", SPL_CKPT);
}

void ObjectStorageUpdateBatch::abort()
{
    APPTRC(L_TRACE, "ObjectStorageUpdateBatch::abort", SPL_CKPT);
}
