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
 * \file ObjectStorageDataStoreEntry.h \brief Definition of the SPL::ObjectStorageDataStoreEntry
 * class.
 */

#ifndef SPL_DSA_OBJECT_STORAGE_DATA_STORE_ENTRY_H
#define SPL_DSA_OBJECT_STORAGE_DATA_STORE_ENTRY_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include "SPL/Runtime/Operator/State/Adapters/S3ClientAdapter/FlexBuff.h"
#include "SPL/Runtime/Operator/State/Adapters/S3ClientAdapter/S3ClientAdapterInterface.h"
#include "SPL/Runtime/Operator/State/DataStoreEntryImpl.h"

namespace SPL {

/// \brief Class that represents a Data Store Entry in ObjectStorage
class DLL_PUBLIC ObjectStorageDataStoreEntry : public DataStoreEntryImpl
{
  public:
    /// Constructor
    /// @param name name of the Data Store Entry
    /// @throws DataStoreException if a ObjectStorageDataStoreEntry instance cannot be created
    ObjectStorageDataStoreEntry(const std::string& name, S3ClientAdapterInterface& s3ClientAdapter);

    /// Destructor
    /// When a Data Store Entry object is destructed, its data still remains in the Data Store, but
    /// this Data Store Entry object handle is deleted.
    ~ObjectStorageDataStoreEntry();

    /// @copydoc DataStoreEntryImpl#put()
    void put(const std::string& key,
             const char* value,
             const uint64_t& size,
             DataStoreUpdateBatchImpl* batch);

    /// @copydoc DataStoreEntryImpl#get(const std::string &,char * &,uint64_t &,bool &)
    void get(const std::string& key, char*& value, uint64_t& size, bool& isExisting);

    /// @copydoc DataStoreEntryImpl#get(const std::string &,char *,const uint64_t &,uint64_t &,bool
    /// &)
    void get(const std::string& key,
             char* value,
             const uint64_t& size,
             uint64_t& returnSize,
             bool& isExisting);

    /// @copydoc DataStoreEntryImpl#get(const std::string &,std::vector<std::pair<char *, uint64_t>
    /// > &,bool &)
    void get(const std::string& key,
             std::vector<std::pair<char*, uint64_t> >& values,
             bool& isExisting);

    /// @copydoc DataStoreEntryImpl#remove(const std::string &,DataStoreUpdateBatchImpl *)
    void remove(const std::string& key, DataStoreUpdateBatchImpl* batch);

    /// @copydoc DataStoreEntryImpl#remove(const std::tr1::unordered_set<std::string> &,const
    /// DataStoreUpdateBatchImpl *)
    void remove(const std::tr1::unordered_set<std::string>& keys, DataStoreUpdateBatchImpl* batch);

    /// @copydoc DataStoreEntryImpl#clear()
    void clear();

    /// @copydoc DataStoreEntryImpl#isExistingKey()
    bool isExistingKey(const std::string& key);

    /// @copydoc DataStoreEntryImpl#getKeySizeLimit()
    uint64_t getKeySizeLimit() const;

    /// @copydoc DataStoreEntryImpl#getValueSizeLimit()
    uint64_t getValueSizeLimit() const;

    /// @copydoc DataStoreEntryImpl#getDefaultChunkSize()
    uint32_t getDefaultChunkSize() const;

    /// @copydoc DataStoreEntryImpl#getKeys()
    void getKeys(std::tr1::unordered_set<std::string>& keys);

  private:
    // append '/' and key to the name
    std::string prepS3Key(std::string const& key);

    S3ClientAdapterInterface& s3ClientAdapter_;
};
} // namespace SPL

#endif // SPL_DSA_OBJECT_STORAGE_DATA_STORE_ENTRY_H
