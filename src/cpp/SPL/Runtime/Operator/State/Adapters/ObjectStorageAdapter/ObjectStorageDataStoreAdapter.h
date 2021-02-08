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
 * \file ObjectStorageDataStoreAdapter.h \brief Definition of the SPL::ObjectStorageDataStoreAdapter
 * class.
 */

#ifndef SPL_DSA_OBJECT_STORAGE_DATA_STORE_ADAPTER_H
#define SPL_DSA_OBJECT_STORAGE_DATA_STORE_ADAPTER_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include "SPL/Runtime/Operator/State/Adapters/S3ClientAdapter/S3ClientAdapterInterface.h"
#include "SPL/Runtime/Operator/State/DataStoreAdapter.h"

namespace SPL {

/// \brief Class that represents an adapter to an ObjectStorage instance
class DLL_PUBLIC ObjectStorageDataStoreAdapter : public DataStoreAdapter
{
  public:
    /// Constructor
    /// @param config a string containing configuration information
    /// @throws DataStoreException if any error happened in constructor
    ObjectStorageDataStoreAdapter(const std::string& config);

    /// Destructor
    ~ObjectStorageDataStoreAdapter();

    /// Return the type of this Data Store Adapter
    /// @return the type of this Data Store Adapter
    std::string getDSAType() { return "ibmCloudObjectStorage"; }

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
    void parseConfiguration();

    std::string configStr_; // configuration string
    std::string endpoint_;  // S3 object storage endpoint url
    bool isHttps_;
    std::string accessKey_;  // S3 object storage accessId
    std::string secretKey_;  // S3 object storage secretKey
    std::string bucketName_; // S3 object storage bucket

    S3ClientAdapterInterface& s3clientAdapter_;
};
} // namespace SPL

#endif // SPL_DSA_OBJECT_STORAGE_DATA_STORE_ADAPTER_H
