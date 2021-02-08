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
 * \file CheckpointCleaner.h \brief Definition of SPL::CheckpointCleaner class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CLEANER_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CLEANER_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdint.h>
#include <string>
#include <tr1/unordered_set>

namespace SPL {
/// Forward declaration
class DataStoreAdapterFactory;
class DataStoreAdapter;

/// \brief The class that contains utility functions for cleaning up inactive checkpoints
/// from backend stores
class DLL_PUBLIC CheckpointCleaner : private boost::noncopyable
{
  public:
    /// Destructor
    ~CheckpointCleaner();

    /// Get names of all Checkpoint Data Store Entries in the backend store
    /// Each Checkpoint Data Store Entry contains checkpoints of one operator
    /// One can use the utility functions in CheckpointNaming class to manipulate
    /// Checkpoint Data Store Entry names
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @param ckptStoreEntryNames return a set of Checkpoint Data Store Entry names
    /// @throws DataStoreException if the names cannot be obtained
    void getCkptStoreEntryNames(const std::string& domainID,
                                const std::string& instanceID,
                                std::tr1::unordered_set<std::string>& ckptStoreEntryNames);

    /// Delete the given Checkpoint Data Store Entry
    /// @param ckptStoreEntryName name of the Checkpoint Data Store Entr to be deleted
    /// @throws DataStoreException if the given Checkpoint Data Store Entry cannot be deleted
    void deleteCkptStoreEntry(const std::string& ckptStoreEntryName);

    /// Delete checkpoints associated with the given job
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @param jobID job ID
    /// @throws DataStoreException if there is any error
    void deleteCkptStoreEntries(const std::string& domainID,
                                const std::string& instanceID,
                                const uint64_t jobID);

    /// Delete checkpoints associated with the given job
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @param jobID job ID
    /// @throws DataStoreException if there is any error
    void deleteCkptStoreEntries(const std::string& domainID,
                                const std::string& instanceID,
                                const std::string& jobID);

    /// Delete checkpoints associated with the given instance
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @throws DataStoreException if there is any error
    void deleteCkptStoreEntries(const std::string& domainID, const std::string& instanceID);

    /// Get CheckpointCleaner instance
    /// @param backendTye type of backend store
    /// @param backendConfig configuration of backend store
    /// @return a handle to the singleton CheckpointCleaner instance
    /// @throws DataStoreException if the instance cannot be obtained
    static CheckpointCleaner* instance(const std::string& backendType,
                                       const std::string& backendConfig);

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Constructor
    /// @param backendTye type of backend store
    /// @param backendConfig configuration of backend store
    /// @throws DataStoreException if a CheckpointCleaner instance cannot be created
    CheckpointCleaner(const std::string& backendType, const std::string& backendConfig);

    boost::scoped_ptr<DataStoreAdapterFactory> storeAdapterFactory_; // factory for DataStoreAdapter
    boost::scoped_ptr<DataStoreAdapter> storeAdapter_;               // Data Store Adapter instance

    static CheckpointCleaner* instance_; // singleton instance
    static SPL::Mutex mutex_;            // for thread safety
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CLEANER_H
