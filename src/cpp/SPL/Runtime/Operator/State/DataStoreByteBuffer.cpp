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
 * Implementation of SPL::DataStoreByteBuffer class
 */

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Function/TimeFunctions.h>
#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreEOFException.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Type/String.h>
#include <algorithm>
#include <assert.h>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <exception>
#include <limits>
#include <memory>
#include <new>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace SPL;

DataStoreByteBuffer::DataStoreByteBuffer()
  : storeEntry_(NULL)
  , buffer_(NULL)
  , cursor_(0)
  , chunkNum_(0)
  , bufferSize_(0)
  , chunkSize_(0)
  , totalSize_(0)
  , lastChunkSize_(0)
  , maxChunkNum_(0)
  , batch_(NULL)
  , writeFinished_(false)
  , userMetaData_(NULL)
  , userMetaDataSize_(0)
{}

DataStoreByteBuffer::DataStoreByteBuffer(DataStoreEntryImpl* entry,
                                         const std::string& key,
                                         const DataStoreByteBuffer::Options& options,
                                         DataStoreUpdateBatchImpl* batch)
  : key_(key)
  , storeEntry_(entry)
  , mode_(options.mode)
  , buffer_(NULL)
  , maxChunkNum_(0)
  , batch_(batch)
  , writeFinished_(false)
  , userMetaData_(NULL)
  , userMetaDataSize_(0)
{
    assert(entry != NULL);
    assert(!key.empty() && key.size() <= entry->getKeySizeLimit());

    if (mode_ == BB_MODE_WRITE) {
        if (options.chunkSize > entry->getValueSizeLimit()) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW(DataStore, "Cannot create DataStoreByteBuffer for "
                               << key << ": chunk size is beyond limit ("
                               << entry->getValueSizeLimit() << ")");
        }
        if (options.truncate == true) {
            try {
                truncate();
            } catch (DataStoreException const& e) {
                if (batch_) {
                    batch_->setState(DataStoreUpdateBatch::ERROR);
                }
                THROW_NESTED(DataStore, "Cannot create DataStoreByteBuffer for " << key, e);
            }
        }
        // set the chunk size, last chunk Number, and size of last chunk
        chunkSize_ =
          (options.chunkSize == 0) ? storeEntry_->getDefaultChunkSize() : options.chunkSize;
        // use the passed totalSize parameter as a hint to decide intial size of buffer
        if (options.totalSize > chunkSize_) {
            bufferSize_ = chunkSize_;
        } else if (options.totalSize > 0) {
            bufferSize_ = options.totalSize;
        } else {
            bufferSize_ = DEFAULT_SIZE; // initial size is 4096 Bytes, defined in ByteBuffer.h
        }
        cursor_ = 0;
        chunkNum_ = 0;
        lastChunkSize_ = 0;
        timestamp_ = SPL::Functions::Time::getTimestamp();

        // allocate local buffer
        try {
            buffer_ = new char[bufferSize_];
        } catch (std::exception const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW(DataStore, "Cannot create DataStoreByteBuffer for key "
                               << key << " : received exception: " << e.what());
        }
    } else if (mode_ == BB_MODE_READ) {
        if (batch != NULL) {
            THROW(DataStore, "Cannot create DataStoreByteBuffer for key "
                               << key
                               << " : buffer cannot be used in READ mode and within a batch");
        }
        // get chunk header
        boost::scoped_ptr<DataStoreChunkHeader> header(NULL);
        try {
            header.reset(DataStoreChunking::getChunkHeader(key, entry));
            if (!header) {
                THROW_CHAR(DataStore, "data does not exist");
            }
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "Cannot create DataStoreByteBuffer for key " << key, e);
        }
        try {
            const char* meta = header->getUserMetaData(userMetaDataSize_);
            if (userMetaDataSize_ > 0) {
                userMetaData_ = new char[userMetaDataSize_];
                memcpy(userMetaData_, meta, userMetaDataSize_);
            }
        } catch (std::exception const& e) {
            THROW(DataStore, "Cannot create DataStoreByteBuffer for key "
                               << key << " : received exception: " << e.what());
        }
        uint32_t chunkSize = header->getChunkSize();
        maxChunkNum_ = header->getLastChunkNum();
        lastChunkSize_ = header->getLastChunkSize();
        bufferSize_ = (maxChunkNum_ == 0) ? lastChunkSize_ : chunkSize;
        timestamp_ = header->getTimeStamp();
        totalSize_ = lastChunkSize_ + uint64_t(maxChunkNum_) * bufferSize_;
        // set chunkNum_ and cursor_ according to start offset
        cursor_ = options.startOffset % chunkSize;
        chunkNum_ = options.startOffset / chunkSize;
        if (totalSize_ == 0) { // empty byte buffer
            buffer_ = NULL;
            return;
        }
        // load the first chunk from backend store to local memory buffer
        try {
            bool chunkExist;
            uint64_t retSize;
            storeEntry_->get(DataStoreChunking::getChunkKey(key, chunkNum_), buffer_, retSize,
                             chunkExist);
            if (chunkExist == false || uint64_t(bufferSize_) != retSize) {
                THROW_CHAR(DataStore, "data does not exist");
            }
            // in READ mode, chunkSize_ is the size of current chunk loaded in buffer
            chunkSize_ = bufferSize_;
        } catch (DataStoreException const& e) {
            if (buffer_) {
                delete[] buffer_;
                buffer_ = NULL;
            }
            if (userMetaData_) {
                delete[] userMetaData_;
                userMetaData_ = NULL;
            }
            THROW_NESTED(DataStore, "Cannot create DataStoreByteBuffer for key " << key, e);
        }
    } else if (mode_ == BB_MODE_APPEND) {
        if (batch != NULL) {
            THROW(DataStore, "Cannot create DataStoreByteBuffer for key "
                               << key
                               << " : buffer cannot be used in APPEND mode and within a batch");
        }
        // get chunk header
        boost::scoped_ptr<DataStoreChunkHeader> header(NULL);
        try {
            header.reset(DataStoreChunking::getChunkHeader(key, entry));
            if (!header) {
                THROW_CHAR(DataStore, "data does not exist");
            }
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "Cannot create DataStoreByteBuffer for key " << key, e);
        }
        chunkSize_ = header->getChunkSize();
        maxChunkNum_ = header->getLastChunkNum();
        lastChunkSize_ = header->getLastChunkSize();
        cursor_ = lastChunkSize_;
        chunkNum_ = maxChunkNum_;
        bufferSize_ = lastChunkSize_;
        timestamp_ = SPL::Functions::Time::getTimestamp();
        totalSize_ = lastChunkSize_ + uint64_t(maxChunkNum_) * chunkSize_;
        // load the last chunk from backend store to local memory buffer
        try {
            uint64_t retSize;
            buffer_ = new char[bufferSize_];
            bool lastChunkExist;
            storeEntry_->get(DataStoreChunking::getChunkKey(key, maxChunkNum_), buffer_,
                             bufferSize_, retSize, lastChunkExist);
            if (lastChunkExist == false || retSize != uint64_t(lastChunkSize_)) {
                THROW_CHAR(DataStore, "data does not exist");
            }
        } catch (DataStoreException const& e) {
            if (buffer_) {
                delete[] buffer_;
                buffer_ = NULL;
            }
            THROW_NESTED(DataStore, "Cannot create DataStoreByteBuffer for key " << key, e);
        } catch (std::bad_alloc const& e) {
            THROW(DataStore, "Cannot create DataStoreByteBuffer for key "
                               << key << " : received exception: " << e.what());
        }
    } else {
        THROW(DataStore, "Cannot create DataStoreByteBuffer for key "
                           << key << ": the operation mode is invalid");
    }
}

DataStoreByteBuffer::~DataStoreByteBuffer()
{
    if (buffer_) {
        delete[] buffer_;
    }
    if (userMetaData_) {
        delete[] userMetaData_;
    }
}

#define ASSERT_WRITE_MODE(method)                                                                  \
    if (mode_ == BB_MODE_READ) {                                                                   \
        if (batch_ != NULL)                                                                        \
            batch_->setState(DataStoreUpdateBatch::ERROR);                                         \
        THROW_CHAR(DataStore, #method "() failed: the buffer is not for update");                  \
    }

#define ASSERT_READ_MODE(method)                                                                   \
    if (mode_ != BB_MODE_READ) {                                                                   \
        THROW_CHAR(DataStore, #method "() failed: the buffer is not for read");                    \
    }

// for primitive types
#define DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(type, method)                                          \
    void DataStoreByteBuffer::add##method(type val)                                                \
    {                                                                                              \
        ASSERT_WRITE_MODE(add##method);                                                            \
        addInternal(reinterpret_cast<const char*>(&val), sizeof(type));                            \
    }                                                                                              \
                                                                                                   \
    type DataStoreByteBuffer::get##method()                                                        \
    {                                                                                              \
        ASSERT_READ_MODE(get##method);                                                             \
        try {                                                                                      \
            type temp;                                                                             \
            getInternal(reinterpret_cast<char*>(&temp), sizeof(type));                             \
            return temp;                                                                           \
        } catch (DataStoreEOFException const& e) {                                                 \
            THROW_NESTED(DataStoreEOF, "get" << #method << "() failed", e);                        \
        } catch (DataStoreException const& e) {                                                    \
            THROW_NESTED(DataStore, "get" << #method << "() failed", e);                           \
        }                                                                                          \
    }

DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(char, Char)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(bool, Bool)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(uint8_t, UInt8)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(int8_t, Int8)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(uint16_t, UInt16)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(uint32_t, UInt32)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(uint64_t, UInt64)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(int16_t, Int16)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(int32_t, Int32)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(int64_t, Int64)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(float, Float)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(double, Double)
DATA_STORE_BYTE_BUFFER_ADD_FUNCTION(long double, LongDouble)

#undef DATA_STORE_BYTE_BUFFER_ADD_FUNCTION

void DataStoreByteBuffer::addNTStr(char const* str)
{
    ASSERT_WRITE_MODE(addNTStr);
    addInternal(str, strlen(str) + 1);
}

char* DataStoreByteBuffer::getNTStr()
{
    ASSERT_READ_MODE(getNTStr);
    char* result = NULL;
    uint64_t totalLen = 0; // total length of string
    uint64_t chunkLen = 0; // length of the part contained in current chunk
    try {
        // search for '\0' which terminates the string
        while (1) {
            char* end = (char*)memchr(buffer_ + cursor_, '\0', bufferSize_ - cursor_);
            if (end != NULL) { // found the end of string
                chunkLen = uint32_t(end - buffer_) - cursor_ + 1;
            } else if (chunkNum_ < maxChunkNum_) {
                // load the next chunk and keep searching for '\0'
                chunkLen = bufferSize_ - cursor_;
            } else {
                THROW_CHAR(DataStore, "string is not terminated properly");
            }
            if (result == NULL) {
                result = new char[totalLen + chunkLen];
            } else {
                char* temp = new char[totalLen + chunkLen];
                memcpy(temp, result, totalLen);
                delete[] result;
                result = temp;
            }
            memcpy(result + totalLen, buffer_ + cursor_, chunkLen);
            totalLen += chunkLen;
            if (end != NULL) {
                cursor_ = uint32_t(end - buffer_) + 1;
                return result;
            } else {
                readNextChunk(buffer_);
            }
        }
    } catch (DataStoreEOFException const& e) {
        if (result) {
            delete[] result;
        }
        THROW_NESTED(DataStoreEOF, "getNTStr() failed", e);
    } catch (DataStoreException const& e) {
        if (result) {
            delete[] result;
        }
        THROW_NESTED(DataStore, "getNTStr() failed", e);
    } catch (std::exception const e) {
        if (result) {
            delete[] result;
        }
        THROW(DataStore, "getNTStr() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::addSTLString(std::string const& str)
{
    ASSERT_WRITE_MODE(addSTLString);
    uint32_t stlen = str.size();
    serializeLength(stlen);
    if (stlen > 0) {
        addInternal(str.c_str(), stlen);
    }
}

std::string DataStoreByteBuffer::getSTLString()
{
    ASSERT_READ_MODE(getSTLString);
    try {
        uint32_t stlen = deserializeLength();
        boost::scoped_array<char> p(new char[stlen]);
        getInternal(p.get(), stlen);
        return std::string(p.get(), stlen);
    } catch (DataStoreEOFException const& e) {
        THROW_NESTED(DataStoreEOF, "getSTLString() failed", e);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getSTLString() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getSTLString() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::getSTLString(std::string& str)
{
    ASSERT_READ_MODE(getSTLString);
    try {
        uint32_t stlen = deserializeLength();
        boost::scoped_array<char> p(new char[stlen]);
        getInternal(p.get(), stlen);
        str.assign(p.get(), stlen);
    } catch (DataStoreEOFException const& e) {
        THROW_NESTED(DataStoreEOF, "getSTLString() failed", e);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getSTLString() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getSTLString() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::addSPLString(RSTRING_BB_TYPE const& str)
{
    ASSERT_WRITE_MODE(addSPLString);
    uint32_t stlen = str.size();
    serializeLength(stlen);
    if (stlen > 0) {
        addInternal(str.c_str(), stlen);
    }
}

RSTRING_BB_TYPE
DataStoreByteBuffer::getSPLString()
{
    ASSERT_READ_MODE(getSPLString);
    try {
        uint32_t stlen = deserializeLength();
        boost::scoped_array<char> p(new char[stlen]);
        getInternal(p.get(), stlen);
        return RSTRING_BB_TYPE(p.get(), stlen);
    } catch (DataStoreEOFException const& e) {
        THROW_NESTED(DataStoreEOF, "getSPLString() failed", e);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getSPLString() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getSPLString() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::getSPLString(RSTRING_BB_TYPE& str)
{
    ASSERT_READ_MODE(getSPLString);
    try {
        uint32_t stlen = deserializeLength();
        boost::scoped_array<char> p(new char[stlen]);
        getInternal(p.get(), stlen);
        str.assign(p.get(), stlen);
    } catch (DataStoreEOFException const& e) {
        THROW_NESTED(DataStoreEOF, "getSPLString() failed", e);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getSPLString() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getSPLString() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::addUnicodeString(USTRING_BB_TYPE const& ustr)
{
    ASSERT_WRITE_MODE(addUnicodeString);
    uint32_t stlen = ustr.length();
    serializeLength(stlen);
    if (stlen > 0) {
        const uint16_t* ptr = ustr.getBuffer();
        addInternal(reinterpret_cast<const char*>(ptr), stlen * sizeof(uint16_t));
    }
}

USTRING_BB_TYPE
DataStoreByteBuffer::getUnicodeString()
{
    ASSERT_READ_MODE(getUnicodeString);
    try {
        uint32_t stlen = deserializeLength();
        boost::scoped_array<uint16_t> uch(new uint16_t[stlen]);
        getInternal(reinterpret_cast<char*>(uch.get()), stlen * sizeof(uint16_t));
        USTRING_BB_TYPE retStr(uch.get(), stlen);
        return retStr;
    } catch (DataStoreEOFException const& e) {
        THROW_NESTED(DataStoreEOF, "getUnicodeString() failed", e);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getUnicodeString() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getUnicodeString() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::addBlob(const void* blob, const uint32_t blobsize)
{
    ASSERT_WRITE_MODE(addBlob);
    addUInt32(blobsize);
    if (blobsize > 0) {
        addInternal(reinterpret_cast<const char*>(blob), blobsize);
    }
}

unsigned char* DataStoreByteBuffer::getBlob(uint32_t& size)
{
    ASSERT_READ_MODE(getBlob);
    unsigned char* temp = NULL;
    try {
        size = getUInt32();
        temp = new unsigned char[size];
        getInternal(reinterpret_cast<char*>(temp), size);
        return temp;
    } catch (DataStoreEOFException const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW_NESTED(DataStoreEOF, "getBlob() failed", e);
    } catch (DataStoreException const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW_NESTED(DataStore, "getBlob() failed", e);
    } catch (std::exception const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW(DataStore, "getBlob() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::addCharSequence(char const* chbuf, uint32_t chbufsize)
{
    addCharSequence(chbuf, uint64_t(chbufsize));
}

void DataStoreByteBuffer::addCharSequence(char const* chbuf, const uint64_t& chbufsize)
{
    ASSERT_WRITE_MODE(addCharSequence);
    addInternal(chbuf, chbufsize);
}

char* DataStoreByteBuffer::getCharSequence(uint32_t& sizeTillEOB)
{
    ASSERT_READ_MODE(getCharSequence);
    uint64_t size;
    getNRemainingBytes(size);
    if (size <= std::numeric_limits<unsigned int>::max()) {
        char* temp = getCharSequence(size);
        sizeTillEOB = uint32_t(size);
        return temp;
    } else {
        THROW(DataStore,
              "getCharSequence() failed: size (" << size << ") larger than 32-bit integer limit");
    }
}

char* DataStoreByteBuffer::getCharSequence(uint64_t& sizeTillEOB)
{
    ASSERT_READ_MODE(getCharSequence);
    char* temp = NULL;
    try {
        getNRemainingBytes(sizeTillEOB);
        temp = new char[sizeTillEOB];
        getInternal(temp, sizeTillEOB);
        return temp;
    } catch (DataStoreEOFException const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW_NESTED(DataStoreEOF, "getCharSequence() failed", e);
    } catch (DataStoreException const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW_NESTED(DataStore, "getCharSequence() failed", e);
    } catch (std::exception const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW(DataStore, "getCharSequence() failed: received exception: " << e.what());
    }
}

char* DataStoreByteBuffer::getFixedCharSequence(uint32_t mysize)
{
    return getFixedCharSequence(uint64_t(mysize));
}

char* DataStoreByteBuffer::getFixedCharSequence(const uint64_t& mysize)
{
    ASSERT_READ_MODE(getFixedCharSequence);
    uint64_t sizeTillEOB;
    getNRemainingBytes(sizeTillEOB);
    if (mysize > sizeTillEOB) {
        THROW(DataStoreEOF, "getFixedCharSequence() failed: try to get "
                              << mysize << "Bytes, but only " << sizeTillEOB << "Bytes left.");
    }
    char* temp = NULL;
    try {
        temp = new char[mysize];
        getInternal(temp, mysize);
        return temp;
    } catch (DataStoreException const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW_NESTED(DataStore, "getFixedCharSequence() failed", e);
    } catch (std::exception const& e) {
        if (temp) {
            delete[] temp;
        }
        THROW(DataStore, "getFixedCharSequence() failed: received exception: " << e.what());
    }
}

char* DataStoreByteBuffer::getFixedCharSequence(char* buffer, uint32_t bsize)
{
    ASSERT_READ_MODE(getFixedCharSequence);
    return getFixedCharSequence(buffer, uint64_t(bsize));
}

char* DataStoreByteBuffer::getFixedCharSequence(char* buffer, const uint64_t& bsize)
{
    ASSERT_READ_MODE(getFixedCharSequence);
    uint64_t sizeTillEOB;
    getNRemainingBytes(sizeTillEOB);
    if (bsize > sizeTillEOB) {
        THROW(DataStoreEOF, "getFixedCharSequence() failed: try to get "
                              << bsize << "Bytes, but only " << sizeTillEOB << "Bytes left.");
    }
    char* temp = buffer;
    try {
        if (temp == NULL) {
            temp = new char[bsize];
        }
        getInternal(temp, bsize);
        return temp;
    } catch (DataStoreException const& e) {
        if (temp != NULL && buffer == NULL) {
            delete[] temp;
        }
        THROW_NESTED(DataStore, "getFixedCharSequence() failed", e);
    } catch (std::exception const& e) {
        if (temp != NULL && buffer == NULL) {
            delete[] temp;
        }
        THROW(DataStore, "getFixedCharSequence() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::getFixedCharSequence(char* buffer,
                                               const uint64_t& bsize,
                                               uint64_t& returnSize)
{
    ASSERT_READ_MODE(getFixedCharSequence);
    assert(buffer);

    if (buffer == NULL) {
        THROW_CHAR(DataStore, "getFixedCharSequence() failed: buffer cannot be NULL");
    }
    uint64_t sizeToRead;
    getNRemainingBytes(sizeToRead);
    if (sizeToRead > bsize) {
        sizeToRead = bsize;
    }
    try {
        getInternal(buffer, sizeToRead);
        returnSize = sizeToRead;
    } catch (DataStoreEOFException const& e) {
        THROW_NESTED(DataStoreEOF, "getFixedCharSequence() failed", e);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getFixedCharSequence() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getFixedCharSequence() failed: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::addPointer(void const* ptr)
{
    ASSERT_WRITE_MODE(addPointer);
#ifdef __LP64__
    assert(sizeof(void*) == sizeof(int64_t));
    addChar(PTR_64BIT);
    int64_t addr = reinterpret_cast<int64_t>(ptr);
    addInt64(addr);
#else
    assert(sizeof(void*) == sizeof(int32_t));
    addChar(PTR_32BIT);
    int32_t addr = reinterpret_cast<int32_t>(ptr);
    addInt32(addr);
#endif
}

void* DataStoreByteBuffer::getPointer()
{
    ASSERT_READ_MODE(getPointer);
    char c = getChar();
#ifdef __LP64__
    assert(sizeof(void*) == sizeof(int64_t));
    if (c != PTR_64BIT) {
        THROW_STRING(DataStore, SPL_APPLICATION_RUNTIME_NBB_POINTER(64));
    }
    int64_t ptr = getInt64();
    return reinterpret_cast<void*>(ptr);
#else
    assert(sizeof(void*) == sizeof(int32_t));
    if (c != PTR_32BIT)
        THROW_STRING(DataStore, SPL_APPLICATION_RUNTIME_NBB_POINTER(32));
    int32_t ptr = getInt32();
    return reinterpret_cast<void*>(ptr);
#endif
}

uint32_t DataStoreByteBuffer::getContentSize() const
{
    uint64_t size;
    getContentSize(size);
    if (size <= std::numeric_limits<unsigned int>::max()) {
        return uint32_t(size);
    } else {
        THROW(DataStore,
              "getContentSize() failed: size (" << size << ") larger than 32-bit integer limit");
    }
};

void DataStoreByteBuffer::getContentSize(uint64_t& size) const
{
    if (mode_ == BB_MODE_READ) {
        size = totalSize_;
    } else {
        size = uint64_t(chunkNum_) * chunkSize_ + cursor_;
    }
};

uint32_t DataStoreByteBuffer::getNRemainingBytes() const
{
    ASSERT_READ_MODE(getNRemainingBytes);
    uint64_t sizeTillEOB;
    getNRemainingBytes(sizeTillEOB);
    if (sizeTillEOB <= std::numeric_limits<unsigned int>::max()) {
        return uint32_t(sizeTillEOB);
    } else {
        THROW(DataStore, "getNRemainingBytes() failed: remaining size ("
                           << sizeTillEOB << ") larger than 32-bit integer limit");
    }
}

void DataStoreByteBuffer::getNRemainingBytes(uint64_t& size) const
{
    ASSERT_READ_MODE(getNRemainingBytes);
    size = totalSize_ - uint64_t(chunkNum_) * bufferSize_ - cursor_;
}

uint32_t DataStoreByteBuffer::getOCursor() const
{
    ASSERT_READ_MODE(getOCursor);
    uint64_t cursor;
    getOCursor(cursor);
    if (cursor <= std::numeric_limits<unsigned int>::max()) {
        return uint32_t(cursor);
    } else {
        THROW(DataStore,
              "getOCursor() failed: cursor (" << cursor << ") larger than 32-bit integer limit");
    }
}

void DataStoreByteBuffer::getOCursor(uint64_t& cursor) const
{
    ASSERT_READ_MODE(getOCursor);
    cursor = uint64_t(chunkNum_) * bufferSize_ + cursor_;
}

void DataStoreByteBuffer::setOCursor(uint64_t offset)
{
    ASSERT_READ_MODE(setOCursor);
    // calculate the chunk which contains the offset position
    uint32_t newChunkNum = offset / bufferSize_;
    if (newChunkNum == chunkNum_) { // just adjust cursor_
        cursor_ = offset % bufferSize_;
    } else { // fetch the new chunk
        bool isExisting;
        uint64_t retSize;
        chunkNum_ = newChunkNum;
        cursor_ = offset % bufferSize_;
        storeEntry_->get(DataStoreChunking::getChunkKey(key_, chunkNum_), buffer_,
                         uint64_t(bufferSize_), retSize, isExisting);
        if (isExisting == false) {
            THROW(DataStore, "Cannot read chunk " << chunkNum_ << ": data does not exist");
        }
        // in READ mode, chunkSize_ is the size of current chunk loaded in buffer
        chunkSize_ = uint32_t(retSize);
    }
}

void DataStoreByteBuffer::finishWrite(const char* metaData, const uint8_t size)
{
    if (mode_ == BB_MODE_WRITE || mode_ == BB_MODE_APPEND) {
        if (size > MAX_META_DATA_SIZE) {
            THROW(DataStore, "finishWrite() failed: the meta-data size ("
                               << size << ") is larger than limit (" << MAX_META_DATA_SIZE << ")");
        }
        try {
            if (cursor_ > 0) { // write out the current chunk
                writeCurrentChunk(buffer_);
            }
            // write chunk header
            DataStoreChunkHeader header(chunkSize_, chunkNum_, cursor_, timestamp_, metaData, size);
            storeEntry_->put(DataStoreChunking::getChunkHeaderKey(key_), header.getSerializedData(),
                             header.getSerializedSize(), batch_);
            writeFinished_ = true;
        } catch (DataStoreException const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW_NESTED(DataStore, "finishWrite() failed", e);
        } catch (std::exception const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW(DataStore, "finishWrite() failed: received exception: " << e.what());
        }
    } else {
        THROW_CHAR(DataStore, "finishWrite() can only be called in WRITE or APPEND mode.");
    }
}

const char* DataStoreByteBuffer::getUserMetaData(uint8_t& size) const
{
    ASSERT_READ_MODE(setOCursor);
    size = userMetaDataSize_;
    return userMetaData_;
}

SPL::timestamp DataStoreByteBuffer::getCreateTime() const
{
    return timestamp_;
}

void DataStoreByteBuffer::addInternal(const char* value, const uint64_t& size)
{
    if (size <= (bufferSize_ - cursor_)) { // this is the common case
        memcpy(buffer_ + cursor_, value, size);
        cursor_ += size;
        return;
    }
    const char* position = value;              // remaining data to add
    uint64_t totalSize = size;                 // amount of remaining data to add
    uint32_t availSize = chunkSize_ - cursor_; // amount of data to add to current chunk
    if (cursor_ > 0 &&
        totalSize >= availSize) { // fill up current chunk and write it to backend store
        // resize buffer if necessary
        if (chunkSize_ > bufferSize_) {
            resize(chunkSize_ - bufferSize_);
        }
        memcpy(buffer_ + cursor_, position, availSize);
        cursor_ += availSize;
        writeCurrentChunk(buffer_);
        if (batch_) {
            batch_->wait();
        }
        totalSize -= availSize;
        position += availSize;
        chunkNum_++;
        cursor_ = 0;
        availSize = chunkSize_;
    }
    while (totalSize >= availSize) {
        // send the full chunk of application data directly instead of copying into internal buffer
        cursor_ = availSize;
        writeCurrentChunk(position);
        if (batch_) {
            batch_->wait();
        }
        totalSize -= availSize;
        position += availSize;
        chunkNum_++;
        cursor_ = 0;
    }
    // add the remaining data to current chunk
    if (totalSize > 0) {
        // resize buffer if necessary
        if (totalSize > bufferSize_ - cursor_) {
            uint32_t extraSize = uint32_t(totalSize) - (bufferSize_ - cursor_);
            resize(extraSize);
        }
        memcpy(buffer_ + cursor_, position, totalSize);
        cursor_ += totalSize;
    }
}

void DataStoreByteBuffer::getInternal(char* value, const uint64_t& size)
{
    uint64_t remainingSize;
    getNRemainingBytes(remainingSize);
    // If size requested is greater than remaing size, we are at the end of the stream.
    if (size > remainingSize) {
        THROW(DataStoreEOF,
              "try to get " << size << " Bytes, only " << remainingSize << " Bytes left");
    }
    char* position = value;                    // remaining data to get
    uint64_t totalSize = size;                 // amount of remaining data to get
    uint64_t availSize = chunkSize_ - cursor_; // amount of data to get from current chunk
    if (totalSize > availSize) {               // get all the data left in current chunk
        memcpy(position, buffer_ + cursor_, availSize);
        totalSize -= availSize;
        position += availSize;
        cursor_ = chunkSize_;
        // calculate the size of next chunk to load
        if (chunkNum_ + 1 < maxChunkNum_) {
            availSize = chunkSize_;
        } else {
            availSize = lastChunkSize_;
        }
    }
    while (totalSize > availSize) {
        // directly read next chunk into destination buffer instead of loading into buffer_
        readNextChunk(position);
        cursor_ = availSize;
        totalSize -= availSize;
        position += availSize;
        if (chunkNum_ + 1 == maxChunkNum_) {
            availSize = lastChunkSize_;
        }
    }
    if (totalSize > 0) { // read remaining data and fill up buffer if necessary
        if (cursor_ == chunkSize_) {
            readNextChunk(buffer_);
        }
        memcpy(position, buffer_ + cursor_, totalSize);
        cursor_ += totalSize;
    }
}

void DataStoreByteBuffer::serializeLength(uint32_t len)
{
    if (len < 128) {   // if fits in a signed int8
        addUInt8(len); // put the length using the first byte
    } else {
        addUInt8(128);  // put the first byte as a negative int8
        addUInt32(len); // put the length using the next 4 bytes
    }
}

uint32_t DataStoreByteBuffer::deserializeLength()
{
    int8_t len = getInt8();
    if (len >= 0) { // if we have a positive int8
        return len; // it is the length
    } else {
        return getUInt32(); // next 4 bytes is the length
    }
}

void DataStoreByteBuffer::writeCurrentChunk(const char* address)
{
    try {
        storeEntry_->put(DataStoreChunking::getChunkKey(key_, chunkNum_), address, cursor_, batch_);
    } catch (DataStoreException const& e) {
        if (batch_) {
            batch_->setState(DataStoreUpdateBatch::ERROR);
        }
        THROW_NESTED(DataStore, "writeCurrentChunk() failed", e);
    }
}

void DataStoreByteBuffer::readNextChunk(char* address)
{
    bool isExisting;
    uint64_t retSize;
    cursor_ = 0;
    chunkNum_++;
    storeEntry_->get(DataStoreChunking::getChunkKey(key_, chunkNum_), address,
                     uint64_t(bufferSize_), retSize, isExisting);
    // in READ mode, chunkSize_ is the size of current chunk loaded in buffer
    chunkSize_ = uint32_t(retSize);
    if (isExisting == false) {
        THROW(DataStore, "Cannot read chunk " << chunkNum_ << ": data does not exist");
    }
}

void DataStoreByteBuffer::resize(const uint32_t extra)
{
    uint32_t growth;
    uint32_t maxGrowth = chunkSize_ - bufferSize_;
    if (extra > bufferSize_) {
        growth = extra; // grow by the required amount
    } else {
        // we know that we can grow by the necessary amount, but
        // let's see if we can double the size
        if (bufferSize_ > maxGrowth) {
            // we can't double the size, so let's grow half of that
            growth = std::max(maxGrowth / 2, extra);
        } else {
            // we can double the size, so let's do it
            growth = bufferSize_;
        }
    }
    try {
        char* tempBuffer = new char[bufferSize_ + growth];
        memcpy(tempBuffer, buffer_, cursor_);
        delete[] buffer_;
        buffer_ = tempBuffer;
        bufferSize_ += growth;
    } catch (std::exception const& e) {
        if (batch_) {
            batch_->setState(DataStoreUpdateBatch::ERROR);
        }
        THROW(DataStore, "Cannot resize buffer: received exception: " << e.what());
    }
}

void DataStoreByteBuffer::truncate()
{
    boost::scoped_ptr<DataStoreChunkHeader> header(NULL);
    if (batch_ == NULL) { // not in a batch
        try {
            header.reset(DataStoreChunking::getChunkHeader(key_, storeEntry_));
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "Cannot truncate DataStoreByteBuffer " << key_, e);
        }
    }
    if (header) { // truncate old chunks if there is any
        try {
            uint32_t maxChunkNum = header->getLastChunkNum();
            for (uint32_t i = 0; i <= maxChunkNum; i++) {
                storeEntry_->remove(DataStoreChunking::getChunkKey(key_, i), batch_);
            }
        } catch (DataStoreException const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW_NESTED(DataStore, "Cannot truncate DataStoreByteBuffer " << key_, e);
        }
    }
}
