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
 * Implementation of the SPL::ObjectStorageDataStoreEntry class.
 */

#include "SPL/Runtime/Operator/State/Adapters/ObjectStorageAdapter/ObjectStorageDataStoreEntry.h"
#include "SPL/Runtime/Common/RuntimeDebug.h"
#include "SPL/Runtime/Operator/State/Adapters/S3ClientAdapter/FlexBuff.h"
#include "SPL/Runtime/Operator/State/DataStoreChunking.h"
#include "SPL/Runtime/Operator/State/DataStoreException.h"
#include "SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h"
#include <assert.h>
#include <dlfcn.h>
#include <exception>
#include <string>
#include <vector>

using namespace std;
using namespace SPL;

ObjectStorageDataStoreEntry::ObjectStorageDataStoreEntry(const std::string& name,
                                                         S3ClientAdapterInterface& s3ClientAdapter)
  : DataStoreEntryImpl(name)
  , s3ClientAdapter_(s3ClientAdapter)
{}

ObjectStorageDataStoreEntry::~ObjectStorageDataStoreEntry() {}

void ObjectStorageDataStoreEntry::put(const std::string& key,
                                      const char* value,
                                      const uint64_t& size,
                                      DataStoreUpdateBatchImpl* batch)
{
    assert(value);
    assert(size > 0);
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::put key=" << key, SPL_CKPT);
    std::string s3Key = prepS3Key(key);
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::put s3Key=" << s3Key, SPL_CKPT);
    FlexBuff errorString;
    if (!s3ClientAdapter_.put(s3Key.c_str(), value, size, errorString)) {
        THROW(DataStore, "S3 put error: " << errorString.str());
    }
}

/// Get(read) the value of the given key from this Data Store Entry.
/// Upon return, value points to an internally allocated buffer which contains the retrieved value,
/// and size returns the size (in Bytes) of the retrieved value. The user is responsible for
/// de-allocating the buffer after use with delete[].
/// @param key key to get
/// @param value return retrieved value. The caller must de-allocate the memory after use
/// @param size return size of retrieved value in Bytes
/// @param isExisting return whether the key exists (true) or not (false)
/// @throws DataStoreException if the value referenced by the key cannot be retrieved from the Data
/// Store Entry
void ObjectStorageDataStoreEntry::get(const std::string& key,
                                      char*& value,
                                      uint64_t& size,
                                      bool& isExisting)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::get 1 " << key, SPL_CKPT);
    std::string s3Key = prepS3Key(key);
    FlexBuff errorString;
    if (!s3ClientAdapter_.getValueIfExists(s3Key.c_str(), NULL, 0, value, size, isExisting,
                                           errorString)) {
        THROW(DataStore, "S3 get error: " << errorString.str());
    }
}

/// Get(read) the value of the given key from this Data Store Entry into a user-provided buffer.
/// The value parameter points to a user-provided buffer and size specifies its size in Bytes.
/// Upon return, the buffer pointed to by value contains the retrieved value, and returnSize
/// returns the size (in Bytes) of the retrieved value.
/// @param key key to get
/// @param value point to a user-provided buffer which contains retrieved data upon return
/// @param size size of the buffer pointed by value
/// @param returnSize return size of retrieved value in Bytes
/// @param isExisting return whether the key exists (true) or not (false)
/// @throws DataStoreException if the value referenced by the key cannot be retrieved from the Data
/// Store Entry
void ObjectStorageDataStoreEntry::get(const std::string& key,
                                      char* value,
                                      const uint64_t& size,
                                      uint64_t& returnSize,
                                      bool& isExisting)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::get 2 " << key, SPL_CKPT);
    std::string s3Key = prepS3Key(key);
    FlexBuff errorString;
    char* returnBuffer;
    if (!s3ClientAdapter_.getValueIfExists(s3Key.c_str(), value, size, returnBuffer, returnSize,
                                           isExisting, errorString)) {
        THROW(DataStore, "S3 get error: " << errorString.str());
    } else {
        assert(returnBuffer == value);
    }
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::get 2 exit" << key, SPL_CKPT);
}

void ObjectStorageDataStoreEntry::get(const std::string& key,
                                      std::vector<std::pair<char*, uint64_t> >& values,
                                      bool& isExisting)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::get 3 " << key, SPL_CKPT);
    std::string s3Key = prepS3Key(key);
    FlexBuff errorString;
    std::pair<char*, uint64_t> pr;
    // Note: get throws exceptions in case of error
    if (!s3ClientAdapter_.getValueIfExists(s3Key.c_str(), NULL, 0, pr.first, pr.second, isExisting,
                                           errorString)) {
        THROW(DataStore, "S3 get error: " << errorString.str());
    }
    values.clear(); //????
    if (isExisting) {
        values.push_back(pr);
    }
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::get 3 exit" << key, SPL_CKPT);
}

void ObjectStorageDataStoreEntry::remove(const std::string& key, DataStoreUpdateBatchImpl* batch)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::remove 1 (single key) " << key, SPL_CKPT);
    std::string s3Key = prepS3Key(key);
    FlexBuff errorString;
    if (!s3ClientAdapter_.remove(s3Key.c_str(), errorString)) {
        APPTRC(L_ERROR,
               "Removal of object store key '" << s3Key << "' failed: " << errorString.str(),
               SPL_CKPT);
    }
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::remove: key " << key << " removed.", SPL_CKPT);
}

void ObjectStorageDataStoreEntry::remove(const std::tr1::unordered_set<std::string>& keys,
                                         DataStoreUpdateBatchImpl* batch)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::remove 2 (set of keys)", SPL_CKPT);
    assert(!keys.empty());
    for (std::tr1::unordered_set<std::string>::const_iterator it = keys.begin(); it != keys.end();
         ++it) {
        remove(*it, batch);
    }
}

void ObjectStorageDataStoreEntry::clear()
{
    /// remove all key-value pairs from the S3 object store, which have the prefix
    /// 'this->getName()'.
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::clear " << getName(), SPL_CKPT);
    FlexBuff errorString;
    if (!s3ClientAdapter_.removeAll(getName().c_str(), errorString)) {
        THROW(DataStore, "S3 removeAll error: " << errorString.str());
    }
}

bool ObjectStorageDataStoreEntry::isExistingKey(const std::string& key)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::isExistingKey " << key, SPL_CKPT);
    std::string s3Key = prepS3Key(key);
    FlexBuff errorString;
    bool exists = s3ClientAdapter_.isExistingKey(s3Key.c_str(), errorString);
    if (!errorString.empty()) {
        THROW(DataStore, "S3 isExistingKey error: " << errorString.str());
    }
    return exists;
}

void ObjectStorageDataStoreEntry::getKeys(std::tr1::unordered_set<std::string>& keys)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::getKeys " << getName(), SPL_CKPT);
    FlexBuff s3error;
    FlexBuff s3Keys;
    if (s3ClientAdapter_.listKeys(getName().c_str(), s3Keys, s3error)) {
        // The keys in the object store have the form '<name>/key'.
        // <name> is that part, which is returned by getName().
        // The function returns all key parts of the object store keys that begin with <name>.
        std::string::size_type keyPos = getName().size() + 1; // +1 for the '/' character
        keys.clear();
        int nKeys = 0;
        unsigned char const* ptr = s3Keys.start();
        while (ptr) {
            char const* keyChars = 0;
            size_t sizeOfKeyChars = 0;
            ptr = s3Keys.get(ptr, keyChars, sizeOfKeyChars);
            assert(sizeOfKeyChars != FlexBuff::npos);
            std::string s3key(keyChars, sizeOfKeyChars);
            assert(keyPos <= s3key.size());
            // remove (this->getName() + "/") from the front of the s3key.
            std::string key = s3key.substr(keyPos);
            keys.insert(key);
            APPTRC(L_DEBUG, "ObjectStorageDataStoreEntry::getKeys: " << ++nKeys << ") " << key,
                   SPL_CKPT);
        }
    } else {
        // s3ClientAdapter_.listKeys error
        THROW(DataStore, "S3 list objects error: " << s3error.str());
    }
}

uint64_t ObjectStorageDataStoreEntry::getKeySizeLimit() const
{
    return 1048576; // 1MB
}

uint64_t ObjectStorageDataStoreEntry::getValueSizeLimit() const
{
    return 1073741824; // 1GB
}

uint32_t ObjectStorageDataStoreEntry::getDefaultChunkSize() const
{
    return 1048576 * 64; // 64MB
}

std::string ObjectStorageDataStoreEntry::prepS3Key(std::string const& key)
{
    assert(!key.empty());
    return getName() + "/" + key;
}
