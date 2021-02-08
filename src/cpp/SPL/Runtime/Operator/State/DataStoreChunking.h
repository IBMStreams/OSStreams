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
 * \file DataStoreChunking.h \brief Definition of SPL::DataStoreChunking class
 */

#ifndef SPL_DSA_DATA_STORE_CHUNKING_H
#define SPL_DSA_DATA_STORE_CHUNKING_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#include <string>

namespace SPL {
/// Forward declaration
class DataStoreEntryImpl;

/// \brief The class that represents Chunk Header (also called Counter) of a DataStoreByteBuffer
class DLL_PUBLIC DataStoreChunkHeader
{
  public:
    /// Constructor (used for writing)
    /// @param chunkSize size of a chunk in Bytes
    /// @param lastChunkNum No. of last chunk
    /// @param lastChunkSize size of last chunk in Bytes
    /// @param timestamp create time
    /// @param userData user-provided meta-data
    /// @param userDataSize size of user-provided meta-data
    /// @throws DataStoreException if a DataStoreChunkHeader instance cannot be created
    DataStoreChunkHeader(const uint32_t chunkSize,
                         const uint32_t lastChunkNum,
                         const uint32_t lastChunkSize,
                         const SPL::timestamp& timestamp,
                         const char* userData,
                         const uint8_t userDataSize);

    /// Constructor by deserializing from a byte string (used for reading).
    /// The ownership of buffer is given to the constructed DataStoreChunkHeader.
    /// @param buffer points to a byte string
    /// @param size size of the byte string in Bytes
    /// @throws DataStoreException if a DataStoreChunkHeader instance cannot be created
    DataStoreChunkHeader(char* buffer, const uint64_t size);

    /// Get chunk size in Bytes
    /// @return chunk size
    uint32_t getChunkSize() const { return chunkSize_; }

    /// Get No. of last chunk
    /// @return No. of last chunk
    uint32_t getLastChunkNum() const { return lastChunkNum_; }

    /// Get size of last chunk in Bytes
    /// @return size of last chunk
    uint32_t getLastChunkSize() const { return lastChunkSize_; }

    /// Get create time
    /// @return create time
    SPL::timestamp getTimeStamp() const { return timestamp_; }

    /// Get user-provided meta-data if there is any
    /// @param size return the size of user-provided meta-data
    /// @return user-provided meta-data
    const char* getUserMetaData(uint8_t& size) const
    {
        size = userDataSize_;
        return userData_;
    }

    /// Get serialized header (used for writing)
    /// @return address of serialized header
    const char* getSerializedData() const
    {
        return reinterpret_cast<const char*>(serialBuf_.getPtr());
    }

    /// Get size of serialized header (used for writing)
    /// @return size of serialized header
    const uint32_t getSerializedSize() const { return serialBuf_.getContentSize(); }

    /// Calculate the size of serielized Chunk Header
    /// @param userDataSize size of user-provided meta-data
    /// @returns the size of serielized Chunk Header
    static uint32_t getChunkHeaderSize(uint8_t userDataSize);

    /// Serialize chunk header information into a NetworkByteBuffer
    /// @param chunkSize size of a chunk in Bytes
    /// @param lastChunkNum No. of last chunk
    /// @param lastChunkSize size of last chunk in Bytes
    /// @param timestamp create time
    /// @param userData user-provided meta-data
    /// @param userDataSize size of user-provided meta-data
    /// @param buffer handle to a NetworkByteBuffer
    /// @throws std::exception if serialization fails
    static void serializeToBuffer(const uint32_t chunkSize,
                                  const uint32_t lastChunkNum,
                                  const uint32_t lastChunkSize,
                                  const SPL::timestamp& timestamp,
                                  const char* userData,
                                  const uint8_t userDataSize,
                                  NetworkByteBuffer& buffer);

    /// Get the size of serielized Chunk Header
    /// Constants
    static const uint32_t defaultChunkHeaderSize; // default size of serialized chunk header

  private:
    uint32_t chunkSize_;          // size of a chunk in Bytes
    uint32_t lastChunkNum_;       // No. of last chunk
    uint32_t lastChunkSize_;      // size of last chunk in Bytes
    SPL::timestamp timestamp_;    // create time
    const char* userData_;        // user-provided meta-data
    uint8_t userDataSize_;        // size of user-provided meta-data
    NetworkByteBuffer serialBuf_; // buffer containing serialized header (used for writing)
};

/// \brief The class that contains utility functions for chunking DataStoreByteBuffer
class DLL_PUBLIC DataStoreChunking
{
  public:
    /// Get the key of Chunk Header of a DataStoreByteBuffer
    /// @param key the key of a DataStoreByteBuffer
    /// @return the composed Chunk Header key
    static std::string getChunkHeaderKey(const std::string& key);

    /// Get the Chunk Header
    /// @param key the key of a DataStoreByteBuffer
    /// @param storeEntry the Data Store Entry which contains the DataStoreByteBuffer
    /// @param isExisting return if the Chunk Header exists (true) or not (false)
    /// @return a handle to the Chunk Header, return NULL if the Chunk Header does not exist
    /// @throws DataStoreException if the Chunk Header cannot be retrieved
    static DataStoreChunkHeader* getChunkHeader(const std::string& key,
                                                DataStoreEntryImpl* storeEntry);

    /// Get the unique sub-key of the given chunk in a DataStoreByteBuffer
    /// @param key the key of a DataStoreByteBuffer
    /// @param chunkNum Number of the chunk
    /// @return the composed chunk key
    static std::string getChunkKey(const std::string& key, const uint32_t chunkNum);

    /// Get the unique key for a short non-chunked key-value pair (i.e., value size is within the
    /// default chunk size)
    /// @param key the key of a short non-chunked key-value pair
    /// @return the composed unique key
    static std::string getUniqueShortKey(const std::string& key);

    /// Return the size of prefix (in Bytes) which is added to a key to generate ChunkHeader, Chunk,
    /// or Unique Short Key
    /// @return the size of prefix (in Bytes)
    static uint32_t getKeyPrefixSize();

    /// Key types
    enum KeyType
    {
        SHORT_KEY,              // key to short non-chunked key-value pair
        BYTE_BUFFER_HEADER_KEY, // key to a Data Store Byte Buffer header
        BYTE_BUFFER_CHUNK_KEY   // key to a Data Store Byte Buffer chunk
    };

    /// Parse an encoded key to get its type and the original key
    /// @param key the encoded key to parse
    /// @param type return the type of the key
    /// @param origKey return the original key
    /// @param chunkNum if the key to parse is a Byte Buffer chunk key, return the chunk No.
    ///@throws DataStoreException if there is any error in parsing
    static void parseKey(const std::string& key,
                         enum KeyType& type,
                         std::string& origKey,
                         uint32_t& chunkNum);
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_CHUNKING_H
