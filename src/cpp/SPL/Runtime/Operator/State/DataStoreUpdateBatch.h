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
 * \file DataStoreBatch.h \brief Definition of the SPL::DataStoreUpdateBatch class.
 */

#ifndef SPL_DSA_DATA_STORE_UPDATE_BATCH_H
#define SPL_DSA_DATA_STORE_UPDATE_BATCH_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>

namespace SPL {
class DataStoreUpdateBatchImpl;

/// \brief Class that defines a Batch of PUT/REMOVE operations.
class DLL_PUBLIC DataStoreUpdateBatch : private boost::noncopyable
{
  public:
    /// Batch state
    enum State
    {
        INPROGRESS = 0,
        ERROR,
        ABORTED,
        COMMITTED
    };

    /// Destructor
    ~DataStoreUpdateBatch();

    /// Commit the batch
    void commit();

    /// Abort the batch
    void abort();

    /// Wait for pending operations to complete
    void wait();

    /// Get batch state
    /// @return the current state of this batch
    enum DataStoreUpdateBatch::State getState() const;

    /// Get batch state in string ("INPROGRESS", "ERROR", "ABORTED", or "COMMITTED")
    /// @return the current state of this batch as string
    std::string getStateStr() const;

    friend class DataStoreAdapter;
    friend class DataStoreEntry;

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Constructor
    /// User should not create DataStoreUpdateBatch instance directly; instead, one
    /// should call DataStoreAdapter::beginUpdateBatch() to create a batch
    /// @param impl internal implementation handle
    DataStoreUpdateBatch(DataStoreUpdateBatchImpl* impl);

    /// Get internal handle
    /// @return handle of DataStoreUpdateBatchImpl
    DataStoreUpdateBatchImpl* getImpl() const;

    DataStoreUpdateBatchImpl* impl_;
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_UPDATE_BATCH_H
