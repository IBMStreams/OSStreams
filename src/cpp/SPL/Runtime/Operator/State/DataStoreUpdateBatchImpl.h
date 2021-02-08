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
 * \file DataStoreBatchImpl.h \brief Definition of the SPL::DataStoreUpdateBatchImpl class.
 */

#ifndef SPL_DSA_DATA_STORE_UPDATE_BATCH_IMPL_H
#define SPL_DSA_DATA_STORE_UPDATE_BATCH_IMPL_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatch.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <string>
#include <tr1/unordered_map>
#include <vector>

namespace SPL {
/// Forward declaration
class DataStoreAdapter;

/// \brief Class that defines a Batch of PUT/REMOVE operations.
class DLL_PUBLIC DataStoreUpdateBatchImpl : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param adapter the Data Store Adapter
    /// @throws DataStoreException if a batch cannot be created
    DataStoreUpdateBatchImpl(DataStoreAdapter* adapter);

    /// Destructor
    virtual ~DataStoreUpdateBatchImpl();

    /// Commit the batch
    virtual void commit();

    /// Abort the batch
    virtual void abort();

    /// Wait for pending operations to complete
    virtual void wait();

    /// Get batch state
    /// @return the current state of this batch
    enum DataStoreUpdateBatch::State getState() const;

    /// Get batch state in string ("INPROGRESS", "ERROR", "ABORTED", or "COMMITTED")
    /// @return the current status of this batch as string
    std::string getStateStr() const;

    /// Set state of batch
    /// @param state the state to set to
    void setState(enum DataStoreUpdateBatch::State state);

    /// Get Data Store Adapter handle
    /// @return Data Store Entry handle
    const DataStoreAdapter* getDataStoreAdapter() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    enum DataStoreUpdateBatch::State state_; // state of the batch
    DataStoreAdapter* storeAdapter_;         // handle to the Data Store Adapter
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_UPDATE_BATCH_IMPL_H
