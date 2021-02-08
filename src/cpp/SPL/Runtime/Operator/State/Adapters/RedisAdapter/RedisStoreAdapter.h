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
 * \file RedisStoreAdapter.h \brief Definition of the SPL::RedisStoreAdapter class.
 */

#ifndef SPL_DSA_REDIS_STORE_ADAPTER_H
#define SPL_DSA_REDIS_STORE_ADAPTER_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>

namespace SPL {
/// Forward declarations
class RedisServerPool;
class RedisStoreEntry;

/// \brief Class that represents an adapter to a set of Redis server instances
class DLL_PUBLIC RedisStoreAdapter : public DataStoreAdapter
{
  public:
    /// Constructor
    /// @param config a string containing configuration information
    /// @throws DataStoreException if any error happend in constructor
    RedisStoreAdapter(const std::string& config);

    /// Destructor
    ~RedisStoreAdapter();

    /// Return the type of this Data Store Adapter
    /// @return the type of this Data Store Adapter
    std::string getDSAType() { return "redis"; }

    /// @copydoc DataStoreAdapter#getDataStoreEntry()
    DataStoreEntry* getDataStoreEntry(const std::string& name, const Option& option);

    /// @copydoc DataStoreAdapter#removeDataStoreEntry()
    void removeDataStoreEntry(const std::string& name);

    /// @copydoc DataStoreAdapter#isExistingStoreEntry()
    bool isExistingDataStoreEntry(const std::string& name);

    /// @copydoc DataStoreAdapter#getDataStoreEntryNames()
    void getDataStoreEntryNames(const std::string& prefix,
                                std::tr1::unordered_set<std::string>& names);

    /// @copydoc DataStoreAdapter#removeDataStoreEntries()
    void removeDataStoreEntries(const std::string& prefix);

    /// @copydoc DataStoreAdapter#createUpdateBatchImpl()
    DataStoreUpdateBatchImpl* createUpdateBatchImpl();

  private:
    std::string configStr_;       // configuration string
    RedisServerPool* serverPool_; // Redis server pool
};
} // namespace SPL

#endif // SPL_DSA_REDIS_STORE_ADAPTER_H
