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
 * \file RedisStoreByteBuffer.h \brief  Definition of the RedisStoreByteBuffer class.
 */
#ifndef SPL_DSA_REDIS_STORE_BYTE_BUFFER_H
#define SPL_DSA_REDIS_STORE_BYTE_BUFFER_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>

namespace SPL {
class RedisStoreEntry;

/// \brief The class that represents a byte buffer on Redis backend data store
class DLL_PUBLIC RedisStoreByteBuffer : public DataStoreByteBuffer
{
  public:
    /// Constructor
    /// @throws DataStoreException if the constructor fails
    /// @param entry the Data Store Entry containing this Byte Buffer
    /// @param key the key of this Byte Buffer
    /// @param mode the operation mode
    /// @param chunkSize size of chunk in Bytes
    /// @param totalSize total size of the buffer in Bytes. This can be approximate and is used as
    /// a hint for determining initial size of internal buffer
    /// @param batch the Update Batch to which this operation belongs; set to NULL if
    /// this DataStoreByteBuffer is not created within a batch
    /// @throws DataStoreException if the Byte Buffer cannot be constructed
    RedisStoreByteBuffer(RedisStoreEntry* entry,
                         const std::string& key,
                         const DataStoreByteBuffer::Options& options,
                         DataStoreUpdateBatchImpl* batch);

    /// Destructor
    ~RedisStoreByteBuffer();

    /// @copydoc DataStoreByteBuffer#finishWrite()
    void finishWrite(const char* metaData, const uint8_t size);

    /// @copydoc DataStoreByteBuffer#addInternal()
    void addInternal(const char* value, const uint64_t& size);

  private:
    /// Calculate the total size of packed buffer when formatting as a HMSET command (the default)
    /// @param chunkKey the key of current chunk
    /// @param valueSize size of value
    /// @param valuePosition returns the starting position of value within the packed buffer
    /// @return the total size of packed buffer
    uint32_t getPackedBufferSize(const std::string& chunkKey,
                                 const uint32_t valueSize,
                                 uint32_t& valuePosition);

    /// Calculate the total size of packed buffer when formatting as a HSET command
    /// @param chunkKey the key of current chunk
    /// @param valueSize size of value
    /// @param valuePosition returns the starting position of value within the packed buffer
    /// @return the total size of packed buffer
    uint32_t getPackedBufferSizeHSET(const std::string& chunkKey,
                                     const uint32_t valueSize,
                                     uint32_t& valuePosition);

    /// Write the packed buffer to backend store
    /// @throws DataStoreException if any error happens
    void writePackedBuffer();

    /// Write the last packed buffer to backend store.
    /// This includes the last chunk and the header.
    /// @param metaData user-provided meta-data
    /// @param size size of user-provided meta-data
    /// @throws DataStoreException if any error happens
    void writeLastPackedBuffer(const char* metaData, const uint8_t size);

    /// Re-allocate the packed buffer
    /// @param extra additional size (in Bytes) to allocate
    /// @throws DataStoreException if resizing fails
    void resizePackedBuffer(const uint32_t extra);

    RedisStoreEntry* redisEntry_; // handle to the Redis Data Store Entry
    char* packedBuffer_;          // the buffer containing Redis PUT command, serialized key and
      // key size, and value and value size; The buffer_ pointer inherited
      // from DataStoreByteBuffer points to the value portion
    uint32_t packedBufferSize_;     // total size of packed buffer
    uint32_t entryNameSizes_;       // the serialized entry name and its size
    std::string headerKey_;         // header key
    uint32_t serializedHeaderSize_; // the serialized header key/value pair
};

} // namespace SPL

#endif // SPL_DSA_REDIS_STORE_BYTE_BUFFER_H
