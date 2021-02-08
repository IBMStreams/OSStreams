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
 * \file CheckpointNaming.h \brief Definition of SPL::CheckpointNaming class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_NAMING_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_NAMING_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#include <string>

namespace SPL {
/// \brief The class that contains utility functions for manipulating Checkpoint IDs
///
/// Checkpoint data are organized in a two-level name space on backend store:
/// - Each operator has an unique Data Store Entry named by a combination of domain ID,
/// instance ID, job ID, and operator's global index.
/// - Within each operator's Data Store Entry, each Checkpoint is represented as two
///   key-value pairs, and the keys are derived from Checkpoint sequence ID:
///   - Meta-Data Key (seqID) --> meta-data (latest sequence ID) as a serialized byte string
///   - Data Key (seqID) --> checkpoint data as a serialized byte string
class DLL_PUBLIC CheckpointNaming
{
  public:
    /// Compose a unique Data Store Entry name for an operator's checkpoints
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @param jobID job ID
    /// @param operatorIndex global index of the operator
    /// @return unique Data Store Entry name
    static std::string getCkptStoreEntryName(const std::string& domainID,
                                             const std::string& instanceID,
                                             const uint64_t& jobID,
                                             const uint32_t& operatorIndex);

    /// Compose the common prefix for Checkpoint Data Store Entry names of a given job
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @param jobID job ID
    /// @return prefix for Checkpoint Data Store Entry names
    static std::string getCkptStoreEntryNamePrefix(const std::string& domainID,
                                                   const std::string& instanceID,
                                                   const uint64_t& jobID);

    /// Compose the common prefix for Checkpoint Data Store Entry names of a given job
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @param jobID job ID
    /// @return prefix for Checkpoint Data Store Entry names
    static std::string getCkptStoreEntryNamePrefix(const std::string& domainID,
                                                   const std::string& instanceID,
                                                   const std::string& jobID);

    /// Compose the common prefix for Checkpoint Data Store Entry names of a given instance
    /// @param domainID Streams domain ID
    /// @param instanceID Streams instance ID
    /// @return prefix for Checkpoint Data Store Entry names
    static std::string getCkptStoreEntryNamePrefix(const std::string& domainID,
                                                   const std::string& instanceID);

    /// Parse the Checkpoint Data Store Entry name
    /// @param storeEntryName Data Store Entry name
    /// @param hasDomainID return whether the name contains a domain ID
    /// @param hasInstanceID return whether the name contains an instance ID
    /// @param hasJobID return whether the name contains a job ID
    /// @param hasOperatorIndex return whether the name contains an operator index
    /// @param domainID return Streams domain ID
    /// @param instanceID return Streams instance ID
    /// @param jobID return job ID
    /// @param operatorIndex return global index of the operator
    /// @throws DataStoreException if there is any error in parsing
    static void parseCkptStoreEntryName(const std::string& storeEntryName,
                                        bool& hasDomainID,
                                        bool& hasinstanceID,
                                        bool& hashJobID,
                                        bool& hasOperatorIndex,
                                        std::string& domainID,
                                        std::string& instanceID,
                                        uint64_t& jobID,
                                        uint32_t& operatorIndex);

    /// key type
    enum KeyType
    {
        NORM_DATA_KEY,        // key to normal checkpoint data
        INCREMENTAL_DATA_KEY, // key to incremental checkpoint data
        INCREMENTAL_INDEX_KEY // key to index to incremental checkpoint data
    };

    /// Get the Data Key of a Checkpoint
    /// @param id Sequence ID of a Checkpoint
    /// @return the composed Data Key
    static std::string getDataKey(const int64_t& id);

    /// Get the Incremental Data Key of a Checkpoint
    /// @param id Sequence ID of a Checkpoint
    /// @return the composed Incremtnal Data Key
    static std::string getIncrementalDataKey(const int64_t& id);

    /// Get the Incremental Index Key of a Checkpoint
    /// @param id Sequence ID of a Checkpoint
    /// @return the composed Incremtnal Index Key
    static std::string getIncrementalIndexKey(const int64_t& id);

    /// Get the key of Meta-Data
    /// @return the Meta-Data Key
    static std::string getMetaDataKey();

    /// Parse the composed Data Key to get the original checkpoint sequence ID
    /// @param dataKey the composed Data Key
    /// @param type return the key type
    /// @return Sequence ID of a Checkpoint
    static int64_t parseDataKey(const std::string& dataKey, enum KeyType& type);
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_NAMING_H
