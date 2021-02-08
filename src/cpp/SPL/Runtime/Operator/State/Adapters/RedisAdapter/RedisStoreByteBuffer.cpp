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
 * Implementation of DSA::RedisStoreByteBuffer class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Function/TimeFunctions.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreEntry.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <UTILS/HostToNetwork.h>
#include <assert.h>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <exception>
#include <limits>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;
using namespace SPL;

RedisStoreByteBuffer::RedisStoreByteBuffer(RedisStoreEntry* entry,
                                           const std::string& key,
                                           const DataStoreByteBuffer::Options& options,
                                           DataStoreUpdateBatchImpl* batch)
  : DataStoreByteBuffer()
  , redisEntry_(entry)
  , packedBuffer_(NULL)
  , headerKey_(DataStoreChunking::getChunkHeaderKey(key))
{
    assert(entry != NULL);
    assert(!key.empty() && key.size() <= entry->getKeySizeLimit());

    storeEntry_ = static_cast<DataStoreEntryImpl*>(entry);
    batch_ = batch;
    key_ = key;
    mode_ = options.mode;

    // calculate size of serialize entry name
    // HASHTABLE_NAME_SIZE\r\nHASHTABLE_NAME
    entryNameSizes_ = redisEntry_->getName().size() + 1;
    entryNameSizes_ += 2 + RedisUtils::getStringLen(entryNameSizes_);

    // calculate size of serialize entry name
    // HEADER_KEY_SIZE\r\nHEADER_KEY
    uint32_t headerKeySize = headerKey_.size();
    headerKeySize += 2 + RedisUtils::getStringLen(headerKeySize);

    // calculate size of the serialized header key and value
    // $HEADER_KEY_SIZE\r\nHEADER_KEY\r\n$HEADER_SIZE\r\nHEADER\r\n
    uint32_t headerSize = DataStoreChunkHeader::getChunkHeaderSize(MAX_META_DATA_SIZE);
    uint32_t lengthOfHeaderSize = RedisUtils::getStringLen(headerSize);
    serializedHeaderSize_ = 1 + headerKeySize + 2 + 1 + lengthOfHeaderSize + 2 + headerSize + 2;

    if (mode_ == BB_MODE_WRITE) {
        assert(options.chunkSize <= entry->getValueSizeLimit());

        if (options.truncate == true) {
            try {
                truncate();
            } catch (DataStoreException const& e) {
                if (batch_ != NULL) {
                    batch_->setState(DataStoreUpdateBatch::ERROR);
                }
                THROW_NESTED(DataStore, "Cannot create RedisStoreByteBuffer for " << key, e);
            }
        }
        // set the chunk size, last chunk No. and size of last chunk
        chunkSize_ =
          (options.chunkSize == 0) ? redisEntry_->getDefaultChunkSize() : options.chunkSize;
        lastChunkSize_ = 0;
        chunkNum_ = 0;
        cursor_ = 0;
        timestamp_ = SPL::Functions::Time::getTimestamp();

        // use the passed totalSize parameter as a hint to decide intial size of buffer
        if (options.totalSize > chunkSize_) {
            bufferSize_ = chunkSize_;
        } else if (options.totalSize > 0) {
            bufferSize_ = options.totalSize;
        } else {
            bufferSize_ = DEFAULT_SIZE; // initial size is 4096 Bytes, defined in ByteBuffer.h
        }

        // set the packed buffer size
        uint32_t valuePosition;
        packedBufferSize_ = getPackedBufferSize(DataStoreChunking::getChunkKey(key, chunkNum_),
                                                bufferSize_, valuePosition);

        // allocate packed buffer
        try {
            packedBuffer_ = new char[packedBufferSize_];
            buffer_ = packedBuffer_ + valuePosition;
        } catch (std::exception const& e) {
            if (batch_ != NULL) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW(DataStore, "Cannot create RedisStoreByteBuffer for key "
                               << key << " : received exception: " << e.what());
        }
    } else if (mode_ == BB_MODE_READ) {
        // re-use the implementation from DataStoreByteBuffer base class
        packedBuffer_ = NULL;
        packedBufferSize_ = 0;
        buffer_ = NULL;
        if (batch != NULL) {
            THROW(DataStore, "Cannot create RedisStoreByteBuffer for key "
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
            THROW_NESTED(DataStore, "Cannot create RedisStoreByteBuffer for key " << key, e);
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
            THROW_NESTED(DataStore, "Cannot create RedisStoreByteBuffer for key " << key, e);
        }
    } else if (mode_ == BB_MODE_APPEND) {
        if (batch != NULL) {
            THROW(DataStore, "Cannot create RedisStoreByteBuffer for key "
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
            THROW_NESTED(DataStore, "Cannot create RedisStoreByteBuffer for key " << key, e);
        }
        chunkSize_ = header->getChunkSize();
        maxChunkNum_ = header->getLastChunkNum();
        lastChunkSize_ = header->getLastChunkSize();
        bufferSize_ = lastChunkSize_;
        chunkNum_ = maxChunkNum_;
        cursor_ = lastChunkSize_;
        timestamp_ = SPL::Functions::Time::getTimestamp();
        totalSize_ = lastChunkSize_ + uint64_t(maxChunkNum_) * chunkSize_;

        // set the packed buffer size
        string lastKey = DataStoreChunking::getChunkKey(key, maxChunkNum_);
        uint32_t valuePosition;
        packedBufferSize_ = getPackedBufferSize(lastKey, bufferSize_, valuePosition);

        // load the last chunk from backend store to local memory buffer
        try {
            packedBuffer_ = new char[packedBufferSize_];
            buffer_ = packedBuffer_ + valuePosition;
            bool lastChunkExist;
            uint64_t retSize;
            storeEntry_->get(lastKey, buffer_, bufferSize_, retSize, lastChunkExist);
            assert(lastChunkExist == true && retSize == lastChunkSize_);
        } catch (DataStoreException const& e) {
            if (packedBuffer_) {
                delete[] packedBuffer_;
                packedBuffer_ = NULL;
                buffer_ = NULL;
            }
            THROW_NESTED(DataStore, "Cannot create RedisStoreByteBuffer for key " << key, e);
        } catch (std::bad_alloc const& e) {
            THROW(DataStore, "Cannot create RedisStoreByteBuffer for key "
                               << key << " : received exception: " << e.what());
        }
    } else {
        THROW(DataStore,
              "Cannot create RedisStoreByteBuffer for key " << key << " : the mode is invalid");
    }
}

RedisStoreByteBuffer::~RedisStoreByteBuffer()
{
    if (mode_ == BB_MODE_WRITE || mode_ == BB_MODE_APPEND) {
        if (packedBuffer_ != NULL) {
            delete[] packedBuffer_;
        }
    } else {
        if (buffer_ != NULL) {
            delete[] buffer_;
        }
        if (userMetaData_ != NULL) {
            delete[] userMetaData_;
            userMetaData_ = NULL;
        }
    }
    buffer_ = NULL;
}

void RedisStoreByteBuffer::finishWrite(const char* metaData, const uint8_t size)
{
    if (mode_ == BB_MODE_WRITE || mode_ == BB_MODE_APPEND) {
        if (size > MAX_META_DATA_SIZE) {
            if (batch_ != NULL) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW(DataStore, "finishWrite() failed: the meta-data size ("
                               << size << ") is larger than limit (" << MAX_META_DATA_SIZE << ")");
        }
        try {
            if (cursor_ > 0) { // write out the current chunk
                writeLastPackedBuffer(metaData, size);
                // give ownership of packed buffer to batch
                if (batch_) {
                    packedBuffer_ = NULL;
                }
            } else {
                // packedBuffer_ is not needed anymore
                if (packedBuffer_ != NULL) {
                    delete[] packedBuffer_;
                    packedBuffer_ = NULL;
                }
                // write chunk header
                DataStoreChunkHeader header(chunkSize_, chunkNum_, cursor_, timestamp_, metaData,
                                            size);
                storeEntry_->put(headerKey_, header.getSerializedData(), header.getSerializedSize(),
                                 batch_);
            }
            writeFinished_ = true;
        } catch (DataStoreException const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW_NESTED(DataStore, "finishWrite() failed", e);
        } catch (SPL::SPLRuntimeSerializationException const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW_NESTED(DataStore, "finishWrite() failed: " << e.getExplanation(), e);
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

void RedisStoreByteBuffer::addInternal(const char* value, const uint64_t& size)
{
    if (size <= (bufferSize_ - cursor_)) { // this is the common case
        memcpy(buffer_ + cursor_, value, size);
        cursor_ += size;
        return;
    }
    const char* position = value;              // remaining data to add
    uint64_t totalSize = size;                 // amount of remaining data to add
    uint32_t availSize = chunkSize_ - cursor_; // amount of data to add to current chunk
    if (totalSize >= availSize && chunkSize_ > bufferSize_) {
        // resize buffer if possible
        resizePackedBuffer(chunkSize_ - bufferSize_);
    }
    while (totalSize >= availSize) {
        // send the full chunk
        memcpy(buffer_ + cursor_, position, availSize);
        cursor_ += availSize;
        writePackedBuffer();
        totalSize -= availSize;
        position += availSize;
        chunkNum_++;
        cursor_ = 0;
        availSize = bufferSize_;
    }
    // add the remaining data to current chunk
    if (totalSize > 0) {
        // resize buffer if necessary
        uint32_t availBufferSize = bufferSize_ - cursor_;
        ;
        if (totalSize > availBufferSize) {
            uint32_t extraSize = uint32_t(totalSize) - availBufferSize;
            resizePackedBuffer(extraSize);
        }
        memcpy(buffer_ + cursor_, position, totalSize);
        cursor_ += totalSize;
    }
}

uint32_t RedisStoreByteBuffer::getPackedBufferSize(const std::string& chunkKey,
                                                   const uint32_t valueSize,
                                                   uint32_t& valuePosition)
{
    // The packed buffer contains a HMSET command for the chunk and header:
    // HMSET storeEntryName chunkKey chunk headerKey header
    //
    // We make sure the binary format complies with Redis protocol specification
    // (http://redis.io/topics/protocol). Below is the binary format: 2A 36 0D 0A 24 35 0D 0A
    // HMSET_COMMAND 0D 0A 24 SE_NAME_SIZE 0D 0A SE_NAME 0D 0A 24 CHUNK_KEY_SIZE 0D 0A CKEY 0D 0A 24
    // VALUE_SIZE 0D 0A VALUE 0D 0A 24 HEADER_KEY_SIZE 0D 0A HKEY 0D 0A 24 VALUE_SIZE 0D 0A HEADER
    // 0A 0A which prints out as follows:
    // *6\r\n$5\r\nHMSET\r\n$SE_NAME_SIZE\r\nSE_NAME\r\n$CHUNK_KEY_SIZE\r\nCKEY\r\n$VALUE_SIZE\r\nVALUE\r\n
    // $HEADER_KEY_SIZE\r\nHEADER_KEY\r\n$HEADER_SIZE\r\nHEADER\r\n
    //
    // Note that there is a special case in which we write last full chunk and header separately. In
    // this case, the packed buffer contains the serialized HSET command and key/value pair for the
    // chunk. That is, it contains one HSET command HSET storeEntryName chunkKey chunk value Below
    // is the binary format: 2A 34 0D 0A 24 34 0D 0A HSET_COMMAND 0D 0A 24 SE_NAME_SIZE 0D 0A
    // SE_NAME 0D 0A 24 CHUNK_KEY_SIZE 0D 0A CKEY 0D 0A 24 VALUE_SIZE 0D 0A VALUE 0D 0A which prints
    // out as follows:
    // *4\r\n$4\r\nHSET\r\n$SE_NAME_SIZE\r\nSE_NAME\r\n$CHUNK_KEY_SIZE\r\nCKEY\r\n$VALUE_SIZE\r\nVALUE\r\n
    //
    // When calculate the buffer size and value position (where the actual data chunk resides), we
    // use the larger on of the two formats, which is HMSET
    uint32_t chunkKeySize = chunkKey.size();
    uint32_t lengthOfChunkKeySize =
      RedisUtils::getStringLen(chunkKeySize); // size of the string representing chunkKeySize itself
    uint32_t lengthOfValueSize = RedisUtils::getStringLen(valueSize);
    valuePosition = 16 + entryNameSizes_ + 2 + 1 + lengthOfChunkKeySize + 2 + chunkKeySize + 2 + 1 +
                    lengthOfValueSize + 2;
    return valuePosition + valueSize + 2 + serializedHeaderSize_;
}

uint32_t RedisStoreByteBuffer::getPackedBufferSizeHSET(const std::string& chunkKey,
                                                       const uint32_t valueSize,
                                                       uint32_t& valuePosition)
{
    // as mentioned in getPackedBufferSize(), there is a special case in which we format the buffer
    // as a HSET command and key/value pair for the chunk
    uint32_t chunkKeySize = chunkKey.size();
    uint32_t lengthOfChunkKeySize =
      RedisUtils::getStringLen(chunkKeySize); // size of the string representing chunkKeySize itself
    uint32_t lengthOfValueSize = RedisUtils::getStringLen(valueSize);
    valuePosition = 15 + entryNameSizes_ + 2 + 1 + lengthOfChunkKeySize + 2 + chunkKeySize + 2 + 1 +
                    lengthOfValueSize + 2;
    return valuePosition + valueSize + 2;
}

void RedisStoreByteBuffer::writePackedBuffer()
{
    std::string chunkKey = DataStoreChunking::getChunkKey(key_, chunkNum_);
    // note that pack buffer was sized by assuming a full buffer size of value, but there is
    // actually cursor_ bytes of value, so we need to re-caulate the total size
    uint32_t valuePosition;
    uint32_t actualSize = getPackedBufferSizeHSET(chunkKey, cursor_, valuePosition);
    char* actualStartPosition = buffer_ - valuePosition;

    // enlarge the packed buffer if nesessary
    if (actualStartPosition < packedBuffer_) {
        try {
            resizePackedBuffer(0);
            actualStartPosition = buffer_ - valuePosition;
        } catch (DataStoreException const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW_NESTED(DataStore, "writePackedBuffer() failed", e);
        }
    }

    // add HSET command, size of hashtable name, hashtable name, key size, key, and value size into
    // packed buffer, right before the value
    uint32_t entryNameSize =
      redisEntry_->getName().size() + 1; // hashtable name for the entry is "D"+actualEntryName
    sprintf(actualStartPosition, "*4\r\n$4\r\nHSET\r\n$%u\r\nD%s\r\n$%u\r\n%s\r\n$%u\r",
            entryNameSize, redisEntry_->getName().c_str(), (uint32_t)chunkKey.size(),
            chunkKey.c_str(), cursor_);
    *(buffer_ - 1) = '\n'; // we do not print this last character in sprintf() because sprintf() add
                           // '\0' at the end
    // set the last two bytes after value to be "\r\n"
    char* footer = buffer_ + cursor_;
    footer[0] = '\r';
    footer[1] = '\n';

    // send the packed buffer out to redis server without copying into hiredis internal output
    // buffer
    try {
        redisEntry_->put(actualStartPosition, actualSize, batch_, NULL);
        if (batch_) {
            batch_->wait();
        }
    } catch (DataStoreException const& e) {
        if (batch_) {
            batch_->setState(DataStoreUpdateBatch::ERROR);
        }
        THROW_NESTED(DataStore, "writePackedBuffer() failed", e);
    }
}

void RedisStoreByteBuffer::writeLastPackedBuffer(const char* metaData, const uint8_t size)
{
    std::string chunkKey = DataStoreChunking::getChunkKey(key_, chunkNum_);
    // note that pack buffer was sized by assuming a full buffer size of value, but there is
    // actually cursor_ bytes of value, so we need to re-calculate the total size
    uint32_t valuePosition;
    getPackedBufferSize(chunkKey, cursor_, valuePosition);

    // enlarge the packed buffer if nesessary
    char* actualStartPosition = buffer_ - valuePosition;
    if (actualStartPosition < packedBuffer_) {
        try {
            resizePackedBuffer(0);
            actualStartPosition = buffer_ - valuePosition;
        } catch (DataStoreException const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW_NESTED(DataStore, "writeLastPackedBuffer() failed", e);
        }
    }

    // add HMSET command, size of hashtable name, hashtable name, key size, key, and value size into
    // packed buffer, right before the value
    uint32_t entryNameSize =
      redisEntry_->getName().size() + 1; // hashtable name for the entry is "D"+actualEntryName
    sprintf(actualStartPosition, "*6\r\n$5\r\nHMSET\r\n$%u\r\nD%s\r\n$%u\r\n%s\r\n$%u\r",
            entryNameSize, redisEntry_->getName().c_str(), (uint32_t)chunkKey.size(),
            chunkKey.c_str(), cursor_);
    *(buffer_ - 1) = '\n'; // we do not print this last character in sprintf() because sprintf() add
                           // '\0' at the end
    char* footer = buffer_ + cursor_;
    footer[0] = '\r';
    footer[1] = '\n';

    // add the serialized header key and header as part of the HMSET command
    uint32_t headerSize = DataStoreChunkHeader::getChunkHeaderSize(size);
    char* headerKVStart = footer + 2;
    int headerStartPosition = sprintf(headerKVStart, "$%u\r\n%s\r\n$%u\r\n",
                                      (uint32_t)headerKey_.size(), headerKey_.c_str(), headerSize);
    char* header = headerKVStart + headerStartPosition;
    NetworkByteBuffer nbuf(reinterpret_cast<unsigned char*>(header), uint64_t(headerSize));
    nbuf.setICursor(0);
    DataStoreChunkHeader::serializeToBuffer(chunkSize_, chunkNum_, cursor_, timestamp_, metaData,
                                            size, nbuf);
    footer = header + headerSize;
    footer[0] = '\r';
    footer[1] = '\n';
    uint32_t actualSize = uint32_t(footer + 2 - actualStartPosition);

    // send the packed buffer out to redis server without copying into hiredis internal output
    // buffer
    std::vector<enum RedisCmdType> types(1, REDIS_CMD_HMSET);
    try {
        redisEntry_->sendUpdates(actualStartPosition, actualSize, types, batch_, packedBuffer_);
    } catch (DataStoreException const& e) {
        if (batch_) {
            batch_->setState(DataStoreUpdateBatch::ERROR);
        }
        THROW_NESTED(DataStore, "writeLastPackedBuffer() failed", e);
    }
}

void RedisStoreByteBuffer::resizePackedBuffer(const uint32_t extra)
{
    // when growing the buffer for the value, the value size portion changes as well, and
    // the whole packed buffer need to grow to accommondate both
    // calculate how many bytes are needed for the whole packed buffer
    uint32_t growth;
    uint32_t maxGrowth = chunkSize_ - bufferSize_;
    if (extra <= maxGrowth) {
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
            // grow the whole packed buffer
            uint32_t valuePosition;
            uint32_t newPackedBufferSize = getPackedBufferSize(
              DataStoreChunking::getChunkKey(key_, chunkNum_), bufferSize_ + growth, valuePosition);
            char* tempPackedBuffer = new char[newPackedBufferSize];
            char* tempBuffer = tempPackedBuffer + valuePosition;
            memcpy(tempBuffer, buffer_, cursor_);
            delete[] packedBuffer_;
            packedBuffer_ = tempPackedBuffer;
            buffer_ = tempBuffer;
            packedBufferSize_ = newPackedBufferSize;
            bufferSize_ += growth;
        } catch (std::exception const& e) {
            if (batch_) {
                batch_->setState(DataStoreUpdateBatch::ERROR);
            }
            THROW(DataStore, "Cannot resize buffer: received exception: " << e.what());
        }
    } else { // cannot grow beyond chunk size
        if (batch_) {
            batch_->setState(DataStoreUpdateBatch::ERROR);
        }
        THROW(DataStore, "Cannot grow internal chunk buffer beyond " << chunkSize_ << " Bytes");
    }
}
