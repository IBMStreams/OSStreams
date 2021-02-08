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
 * \file ObjectStorageUpdateBatch.h \brief Definition of the SPL::ObjectStorageUpdateBatch class.
 */
#ifndef SPL_DSA_OBJECT_STORAGE_UPDATE_BATCH_H
#define SPL_DSA_OBJECT_STORAGE_UPDATE_BATCH_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <deque>
#include <stdint.h>
#include <tr1/unordered_map>

namespace SPL {

class ObjectStorageDataStoreAdapter;

/// \brief Class that represents a batch of update operations. Each update operation
/// is a PUT or REMOVE command.
class DLL_PUBLIC ObjectStorageUpdateBatch : public DataStoreUpdateBatchImpl
{
  public:
    /// Constructor
    /// @param serverPool server pool handle
    /// @param adapter ObjectStorageStoreAdapter handle
    /// @throws DataStoreException if a ObjectStorageUpdateBatch cannot be created
    ObjectStorageUpdateBatch(ObjectStorageDataStoreAdapter* adapter);

    /// Destructor
    ~ObjectStorageUpdateBatch();

    /// Commit the batch
    /// @throws DataStoreException if there is any error
    void commit();

    /// Abort the batch
    /// @throws DataStoreException if there is any error
    void abort();

    /// Wait for currently batched commands to finish
    /// @throws DataStoreException if there is any error
    void wait();
};
} // namespace SPL

#endif // SPL_DSA_OBJECT_STORAGE_UPDATE_BATCH_H
