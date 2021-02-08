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
 * \file DataStoreAdapter.h \brief Definition of the SPL::DataStoreAdapter class.
 */
#ifndef SPL_DSA_DATA_STORE_ADAPTER_H
#define SPL_DSA_DATA_STORE_ADAPTER_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreEntry.h>
#include <SPL/Runtime/Operator/State/Option.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <string>
#include <tr1/unordered_set>

namespace SPL {
/// Forward declarations
class DataStoreUpdateBatch;
class DataStoreUpdateBatchImpl;

/// \brief Class that provides the interface to a backend store
class DLL_PUBLIC DataStoreAdapter : private boost::noncopyable
{
  public:
    /// Default constructor
    DataStoreAdapter();

    /// Destructor
    virtual ~DataStoreAdapter();

    /// Return the type of this Data Store Adapter
    /// @return a string to identify the type of this Data Store Adapter
    virtual std::string getDSAType() = 0;

    /// Get a Data Store Entry.
    /// To create a new Data Store Entry, set option.create_if_missing = true and
    /// option.error_if_exist = true; To open an existing Data Store Entry, set
    /// option.create_if_missing = false and option.error_if_exist = false.
    /// @param name name of the Data Store Entry
    /// @param option contains control options
    /// @return the Data Store Entry handle
    /// @throws DataStoreException if the Data Store Entry handle cannot be obtained
    virtual DataStoreEntry* getDataStoreEntry(const std::string& name, const Option& option) = 0;

    /// Remove a Data Store Entry
    /// @param name name of the Data Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be removed
    virtual void removeDataStoreEntry(const std::string& name) = 0;

    /// Check if a Data Store Entry of the given name exists
    /// @param name name of the Data Store Entry
    /// @return true if the Data Store Entry of the given name exists, false otherwise
    /// @throws DataStoreException if any error happens during operation
    virtual bool isExistingDataStoreEntry(const std::string& name) = 0;

    /// Get the names of all Data Store Entries which prefix-match the given prefix.
    /// For example, names "/a/b/c", "/a/b/", and "/a/bc" all prefix-match "/a/b".
    /// @param prefix the prefix to match with
    /// @param names return a set of matching Data Store Entry names
    /// @throws DataStoreException if the names cannot be obtained due to error
    virtual void getDataStoreEntryNames(const std::string& prefix,
                                        std::tr1::unordered_set<std::string>& names) = 0;

    /// Remove all Data Store Entries which prefix-match the given prefix
    /// @param prefix the prefix to match with
    /// @throws DataStoreException if there is any error during operation
    virtual void removeDataStoreEntries(const std::string& prefix) = 0;

    /// Create/begin a batch of update operations
    /// @return a DataStoreUpdateBatch handle; Any operators to be included in this
    /// batch should pass this handle (e.g., in put() or remove() functions)
    /// @throws DataStoreException if a batch cannot be created
    DataStoreUpdateBatch* createUpdateBatch();

  protected:
    /// Create/begin a batch of update operations
    /// @return a DataStoreUpdateBatchImpl handle
    /// @throws DataStoreException if a batch cannot be created
    virtual DataStoreUpdateBatchImpl* createUpdateBatchImpl();
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_ADAPTER_H
