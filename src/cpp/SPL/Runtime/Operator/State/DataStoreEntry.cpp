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
 * Implementation of SPL::DataStoreEntry class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreEntry.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/HostToNetwork.h>
#include <assert.h>
#include <boost/scoped_ptr.hpp>
#include <memory>
#include <new>
#include <string>

using namespace std;
using namespace SPL;

DataStoreEntry::DataStoreEntry(DataStoreEntryImpl* impl)
  : impl_(impl)
{}

DataStoreEntry::~DataStoreEntry() {}

std::string DataStoreEntry::getName() const
{
    return impl_->getName();
}

void DataStoreEntry::put(const std::string& key,
                         const char* value,
                         const uint64_t& size,
                         DataStoreUpdateBatch* batch)
{
    if (key.empty()) {
        THROW_CHAR(DataStore, "put() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore, "put() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    try {
        putInternal(key, value, size, batch);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "put() failed for key " << key, e);
    }
}

void DataStoreEntry::putInternal(const std::string& key,
                                 const char* value,
                                 const uint64_t& size,
                                 DataStoreUpdateBatch* batch)
{
    if (value == NULL) {
        THROW_CHAR(DataStore, "value cannot be NULL");
    }
    if (size == 0) {
        THROW_CHAR(DataStore, "value size cannot be zero");
    }
    if (size > getValueSizeLimit()) {
        THROW(DataStore, "value size exceeds limit: " << getValueSizeLimit());
    }
    if (batch == NULL) {
        impl_->put(DataStoreChunking::getUniqueShortKey(key), value, size, NULL);
    } else {
        DataStoreUpdateBatchImpl* batchImpl = batch->getImpl();
        if (batchImpl == NULL) {
            THROW_CHAR(DataStore, "batch parameter is invalid");
        }
        try {
            impl_->put(DataStoreChunking::getUniqueShortKey(key), value, size, batchImpl);
        } catch (DataStoreException const& e) {
            batchImpl->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "received exception", e);
        }
    }
}

void DataStoreEntry::get(const std::string& key, char*& value, uint64_t& size, bool& isExisting)
{
    if (key.empty()) {
        THROW_CHAR(DataStore, "get() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore, "get() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    try {
        getInternal(key, value, size, isExisting);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "get() failed for key " << key, e);
    }
}

void DataStoreEntry::get(const std::string& key,
                         char* value,
                         const uint64_t& size,
                         uint64_t& returnSize,
                         bool& isExisting)
{
    if (key.empty()) {
        THROW_CHAR(DataStore, "get() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore, "get() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    if (value == NULL) {
        THROW(DataStore, "get() failed for key " << key << ": value cannot be NULL");
    }
    try {
        impl_->get(DataStoreChunking::getUniqueShortKey(key), value, size, returnSize, isExisting);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "get() failed for key " << key, e);
    }
}

void DataStoreEntry::get(const std::string& key,
                         std::vector<std::pair<char*, uint64_t> >& values,
                         bool& isExisting)
{
    impl_->get(DataStoreChunking::getUniqueShortKey(key), values, isExisting);
}

void DataStoreEntry::getInternal(const std::string& key,
                                 char*& value,
                                 uint64_t& size,
                                 bool& isExisting)
{
    impl_->get(DataStoreChunking::getUniqueShortKey(key), value, size, isExisting);
}

bool DataStoreEntry::isExistingKey(const std::string& key)
{
    if (key.empty()) {
        THROW_CHAR(DataStore, "isExistingKey() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore, "isExistingKey() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    return impl_->isExistingKey(DataStoreChunking::getUniqueShortKey(key));
}

void DataStoreEntry::remove(const std::string& key, DataStoreUpdateBatch* batch)
{
    if (key.empty()) {
        THROW_CHAR(DataStore, "remove() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore, "remove() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    if (batch == NULL) {
        impl_->remove(DataStoreChunking::getUniqueShortKey(key), NULL);
    } else {
        DataStoreUpdateBatchImpl* batchImpl = batch->getImpl();
        if (batchImpl == NULL) {
            THROW(DataStore, "remove() failed for key " << key << ": batch parameter is invalid");
        }
        try {
            impl_->remove(DataStoreChunking::getUniqueShortKey(key), batchImpl);
        } catch (DataStoreException const& e) {
            batchImpl->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "remove() failed for key " << key, e);
        }
    }
}

DataStoreByteBuffer* DataStoreEntry::openByteBuffer(const std::string& key,
                                                    const DataStoreByteBuffer::Options& options,
                                                    DataStoreUpdateBatch* batch)
{
    if (key.empty()) {
        THROW_CHAR(DataStore, "openByteBuffer() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore, "openByteBuffer() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    if (options.mode == DataStoreByteBuffer::BB_MODE_WRITE &&
        options.chunkSize > impl_->getValueSizeLimit()) {
        THROW(DataStore, "openByteBuffer() failed: chunk size ("
                           << options.chunkSize << ") is larger than backend store's limit ("
                           << impl_->getValueSizeLimit() << ")");
    }
    if (batch == NULL) {
        return impl_->openByteBuffer(key, options, NULL);
    } else {
        DataStoreUpdateBatchImpl* batchImpl = batch->getImpl();
        if (batchImpl == NULL) {
            THROW(DataStore,
                  "openByteBuffer() failed for key " << key << ": batch parameter is invalid");
        }
        try {
            return impl_->openByteBuffer(key, options, batchImpl);
        } catch (DataStoreException const& e) {
            batchImpl->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "openByteBuffer() failed for key " << key, e);
        }
        return NULL;
    }
}

bool DataStoreEntry::isExistingByteBuffer(const std::string& key)
{
    if (key.empty()) {
        THROW_CHAR(DataStore, "isExistingByteBuffer() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore,
              "isExistingByteBuffer() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    return impl_->isExistingByteBuffer(key);
}

void DataStoreEntry::removeByteBuffer(const std::string& key, DataStoreUpdateBatch* batch)
{
    APPTRC(L_DEBUG, "Enter", SPL_CKPT);
    if (key.empty()) {
        THROW_CHAR(DataStore, "removeByteBuffer() failed: key cannot be empty string");
    }
    if (key.size() > getKeySizeLimit()) {
        THROW(DataStore,
              "removeByteBuffer() failed: key size exceeds limit: " << getKeySizeLimit());
    }
    if (batch == NULL) {
        impl_->removeByteBuffer(key, NULL);
    } else {
        DataStoreUpdateBatchImpl* batchImpl = batch->getImpl();
        if (batchImpl == NULL) {
            THROW(DataStore,
                  "removeByteBuffer() failed for key " << key << ": batch parameter is invalid");
        }
        try {
            impl_->removeByteBuffer(key, batchImpl);
        } catch (DataStoreException const& e) {
            batchImpl->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "removeByteBuffer() failed for key " << key, e);
        }
    }
}

void DataStoreEntry::getUniqueShortKeys(std::tr1::unordered_set<std::string>& keys)
{
    std::tr1::unordered_set<std::string> allKeys;
    impl_->getKeys(allKeys);
    for (std::tr1::unordered_set<std::string>::const_iterator iter = allKeys.begin();
         iter != allKeys.end(); ++iter) {
        try {
            // check if *iter is a unique short key
            DataStoreChunking::KeyType type;
            std::string origKey;
            uint32_t chunkNum;
            DataStoreChunking::parseKey(*iter, type, origKey, chunkNum);
            if (type == DataStoreChunking::SHORT_KEY) {
                keys.insert(origKey);
            }
        } catch (DataStoreException const& e) {
            continue;
        }
    }
}

void DataStoreEntry::getByteBufferKeys(std::tr1::unordered_set<std::string>* headerKeys,
                                       std::tr1::unordered_set<ChunkKey>* chunkKeys)
{
    std::tr1::unordered_set<std::string> allKeys;
    impl_->getKeys(allKeys);
    for (std::tr1::unordered_set<std::string>::const_iterator iter = allKeys.begin();
         iter != allKeys.end(); ++iter) {
        try {
            DataStoreChunking::KeyType type;
            std::string origKey;
            uint32_t chunkNum;
            DataStoreChunking::parseKey(*iter, type, origKey, chunkNum);
            if (type == DataStoreChunking::BYTE_BUFFER_HEADER_KEY && headerKeys != NULL) {
                headerKeys->insert(origKey);
            } else if (type == DataStoreChunking::BYTE_BUFFER_CHUNK_KEY && chunkKeys != NULL) {
                chunkKeys->insert(std::make_pair(origKey, chunkNum));
            }
        } catch (DataStoreException const& e) {
            continue;
        }
    }
}

void DataStoreEntry::remove(const std::tr1::unordered_set<std::string>* headerKeys,
                            const std::tr1::unordered_set<ChunkKey>* chunkKeys,
                            const std::tr1::unordered_set<std::string>* shortKeys,
                            DataStoreUpdateBatch* batch)
{
    std::tr1::unordered_set<std::string> keys;
    if (headerKeys != NULL) {
        for (std::tr1::unordered_set<std::string>::const_iterator iter = headerKeys->begin();
             iter != headerKeys->end(); ++iter) {
            keys.insert(DataStoreChunking::getChunkHeaderKey(*iter));
        }
    }
    if (chunkKeys != NULL) {
        for (std::tr1::unordered_set<ChunkKey>::const_iterator iter = chunkKeys->begin();
             iter != chunkKeys->end(); ++iter) {
            keys.insert(DataStoreChunking::getChunkKey(iter->first, iter->second));
        }
    }
    if (shortKeys != NULL) {
        for (std::tr1::unordered_set<std::string>::const_iterator iter = shortKeys->begin();
             iter != shortKeys->end(); ++iter) {
            keys.insert(DataStoreChunking::getUniqueShortKey(*iter));
        }
    }
    if (keys.empty()) {
        return;
    }
    if (batch == NULL) {
        impl_->remove(keys, NULL);
    } else {
        DataStoreUpdateBatchImpl* batchImpl = batch->getImpl();
        if (batchImpl == NULL) {
            THROW(DataStore, "remove() failed: batch parameter is invalid");
        }
        try {
            impl_->remove(keys, batchImpl);
        } catch (DataStoreException const& e) {
            batchImpl->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "remove() failed", e);
        }
    }
}

void DataStoreEntry::clear()
{
    impl_->clear();
}

uint64_t DataStoreEntry::getKeySizeLimit() const
{
    return impl_->getKeySizeLimit() - DataStoreChunking::getKeyPrefixSize();
}

uint64_t DataStoreEntry::getValueSizeLimit() const
{
    return impl_->getValueSizeLimit();
}

uint32_t DataStoreEntry::getDefaultChunkSize() const
{
    return impl_->getDefaultChunkSize();
}

void DataStoreEntry::compactDatabaseIfNeeded()
{
    impl_->compactDatabaseIfNeeded();
}
