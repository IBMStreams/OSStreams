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
 * Implementation of SPL::DataStoreChunking class
 */

#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <boost/scoped_array.hpp>
#include <exception>
#include <sstream>

using namespace std;
using namespace SPL;

const uint32_t DataStoreChunkHeader::defaultChunkHeaderSize =
  sizeof(uint32_t) * 3 + SPL::timestamp().getSerializedSize() + 1;

DataStoreChunkHeader::DataStoreChunkHeader(const uint32_t chunkSize,
                                           const uint32_t lastChunkNum,
                                           const uint32_t lastChunkSize,
                                           const SPL::timestamp& timestamp,
                                           const char* userData,
                                           const uint8_t userDataSize)
  : chunkSize_(chunkSize)
  , lastChunkNum_(lastChunkNum)
  , lastChunkSize_(lastChunkSize)
  , timestamp_(timestamp)
  , userData_(userData)
  , userDataSize_(userDataSize)
  , serialBuf_(defaultChunkHeaderSize + userDataSize)
{
    try {
        DataStoreChunkHeader::serializeToBuffer(chunkSize_, lastChunkNum_, lastChunkSize_,
                                                timestamp_, userData_, userDataSize_, serialBuf_);
    } catch (std::exception const& e) {
        THROW(DataStore, "cannot create DataStoreChunkHeader: received exception: " << e.what());
    }
}

DataStoreChunkHeader::DataStoreChunkHeader(char* buffer, const uint64_t size)
  : chunkSize_(0)
  , lastChunkNum_(0)
  , lastChunkSize_(0)
  , timestamp_()
  , userData_(NULL)
  , userDataSize_(0)
  , serialBuf_(reinterpret_cast<unsigned char*>(buffer), size)
{
    assert(buffer != NULL);
    assert(size > 0);

    try {
        serialBuf_.setAutoDealloc(true);
        serialBuf_ >> chunkSize_;
        serialBuf_ >> lastChunkNum_;
        serialBuf_ >> lastChunkSize_;
        serialBuf_ >> timestamp_;
        serialBuf_ >> userDataSize_;
        if (userDataSize_ > 0) {
            userData_ = serialBuf_.getFixedCharSequence(uint32_t(userDataSize_));
        } else {
            userData_ = NULL;
        }
    } catch (std::exception const& e) {
        THROW(DataStore, "cannot create DataStoreChunkHeader: received exception: " << e.what());
    }
}

uint32_t DataStoreChunkHeader::getChunkHeaderSize(uint8_t userDataSize)
{
    return defaultChunkHeaderSize + userDataSize;
}

void DataStoreChunkHeader::serializeToBuffer(const uint32_t chunkSize,
                                             const uint32_t lastChunkNum,
                                             const uint32_t lastChunkSize,
                                             const SPL::timestamp& timestamp,
                                             const char* userData,
                                             const uint8_t userDataSize,
                                             NetworkByteBuffer& buffer)
{
    buffer << chunkSize;
    buffer << lastChunkNum;
    buffer << lastChunkSize;
    buffer << timestamp;
    buffer << userDataSize;
    if (userDataSize > 0 && userData != NULL) {
        buffer.addCharSequence(userData, uint32_t(userDataSize));
    }
}

std::string DataStoreChunking::getChunkHeaderKey(const std::string& key)
{
    assert(!key.empty());

    return "H" + key;
}

DataStoreChunkHeader* DataStoreChunking::getChunkHeader(const std::string& key,
                                                        DataStoreEntryImpl* storeEntry)
{
    assert(!key.empty());
    assert(storeEntry != NULL);

    try {
        bool isExisting;
        char* buffer = NULL;
        uint64_t size;
        storeEntry->get(DataStoreChunking::getChunkHeaderKey(key), buffer, size, isExisting);
        if (isExisting == true) {
            return new DataStoreChunkHeader(buffer, size);
        } else {
            return NULL;
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getChunkHeader() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getChunkHeader() failed: received exception: " << e.what());
    }
}

std::string DataStoreChunking::getChunkKey(const std::string& key, const uint32_t chunkNum)
{
    assert(!key.empty());

    std::stringstream str;
    str << hex << chunkNum << ":" << key;
    return str.str();
}

std::string DataStoreChunking::getUniqueShortKey(const std::string& key)
{
    assert(!key.empty());

    return "S" + key;
}

uint32_t DataStoreChunking::getKeyPrefixSize()
{
    return 9;
}

void DataStoreChunking::parseKey(const std::string& key,
                                 enum KeyType& type,
                                 std::string& origKey,
                                 uint32_t& chunkNum)
{
    assert(!key.empty());

    if (key[0] == 'H') {
        type = BYTE_BUFFER_HEADER_KEY;
        origKey = key.substr(1);
    } else if (key[0] == 'S') {
        type = SHORT_KEY;
        origKey = key.substr(1);
    } else {
        size_t commaPos = key.find(":");
        if (commaPos != string::npos) {
            type = BYTE_BUFFER_CHUNK_KEY;
            std::stringstream str(key);
            str >> hex >> chunkNum;
            origKey = key.substr(commaPos + 1);
        } else {
            THROW(DataStore, "parseKey() failed: the key (" << key << ") is invalid");
        }
    }
}
