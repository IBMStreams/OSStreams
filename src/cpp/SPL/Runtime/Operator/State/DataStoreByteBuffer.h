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
 * \file DataStoreByteBuffer.h \brief Definition of SPL::DataStoreByteBuffer class
 */

#ifndef SPL_DSA_DATA_STORE_BYTE_BUFFER_H
#define SPL_DSA_DATA_STORE_BYTE_BUFFER_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Serialization/ByteBuffer.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>

namespace SPL {
/// Forward declaration
class DataStoreEntryImpl;
class DataStoreUpdateBatchImpl;

/// \brief The class that represents a Byte Buffer on backend data store
class DLL_PUBLIC DataStoreByteBuffer
  : private boost::noncopyable
  , public ByteBuffer<DataStoreByteBuffer>
{
  public:
    /// Operation mode for DataStoreByteBuffer
    enum Mode
    {
        BB_MODE_WRITE, // Only allow write operation
        BB_MODE_READ,  // Only allow read operation
        BB_MODE_APPEND // Only allow write operation but data are appended to the existing content
    };

    /// \brief Class that is used to specify various options for creating or opening a Data Store
    /// Entry.
    class DLL_PUBLIC Options
    {
      public:
        Mode mode;          // the operation mode
        uint64_t chunkSize; // size of chunk in Bytes (only for WRITE mode)
        uint32_t
          totalSize; // total size of the buffer in Bytes. This can be approximate and is used as
                     // a hint for determining initial size of internal buffer (only for WRITE mode)
        bool truncate; // whether to truncate previous contents in the Byte Buffer (only for WRITE
                       // mode)
        uint64_t startOffset; // starting offset (only for READ mode)
    };

    /// Constructor
    /// @param entry the Data Store Entry containing this Data Store Byte Buffer
    /// @param key the key (i.e., name) of this Data Store Byte Buffer
    /// @param batch the Update Batch to which this operation belongs; set to NULL if
    /// this DataStoreByteBuffer is not created within a batch
    /// @throws DataStoreException if a DataStoreByteBuffer instance cannot be created
    DataStoreByteBuffer(DataStoreEntryImpl* entry,
                        const std::string& key,
                        const Options& option,
                        DataStoreUpdateBatchImpl* batch);

    /// Destructor
    virtual ~DataStoreByteBuffer();

    /// Add/serialize a character to the buffer
    /// @param c a character
    /// @throws DataStoreException if a character cannot be added to the buffer
    void addChar(char c);

    /// Extract/de-serialize a character from the buffer
    /// @return a character
    /// @throws DataStoreException if a character cannot be extracted from the buffer
    char getChar();

    /// Add/serialize a boolean to the buffer
    /// @param b a boolean
    /// @throws DataStoreException if a character cannot be added to the buffer
    void addBool(bool b);

    /// Extract/de-serialize a boolean from the buffer
    /// @return a boolean
    /// @throws DataStoreException if a boolean cannot be extracted from the buffer
    bool getBool();

    /// Add/serialize a uint8_t to the buffer
    /// @param i a uint8_t
    /// @throws DataStoreException if a uint8_t cannot be added to the buffer
    void addUInt8(uint8_t i);

    /// Extract/de-serialize a uint8_t from the buffer
    /// @return a uint8_t
    /// @throws DataStoreException if a uint8_t cannot be extracted from the buffer
    uint8_t getUInt8();

    /// Add/serialize an int8_t to the buffer
    /// @param i an int8_t
    /// @throws DataStoreException if an int8_t cannot be added to the buffer
    void addInt8(int8_t i);

    /// Extract/de-serialize an int8_t from the buffer
    /// @return an int8_t
    /// @throws DataStoreException if an int8_t cannot be extracted from the buffer
    int8_t getInt8();

    /// Add/serialize a uint16_t to the buffer
    /// @param i a uint16_t
    /// @throws DataStoreException if a uint16_t cannot be added to the buffer
    void addUInt16(uint16_t i);

    /// Extract/de-serialize a uint16_t from the buffer
    /// @return a uint16_t
    /// @throws DataStoreException if a uint16_t cannot be extracted from the buffer
    uint16_t getUInt16();

    /// Add/serialize an int16_t to the buffer
    /// @param i an int16_t
    /// @throws DataStoreException if an int16_t cannot be added to the buffer
    void addInt16(int16_t i);

    /// Extract/de-serialize an int16_t from the buffer
    /// @return an int16_t
    /// @throws DataStoreException if an int16_t cannot be extracted from the buffer
    int16_t getInt16();

    /// Add/serialize a uint32_t to the buffer
    /// @param i a uint32_t
    /// @throws DataStoreException if a uint32_t cannot be added to the buffer
    void addUInt32(uint32_t i);

    /// Extract/de-serialize a uint32_t from the buffer
    /// @return a uint32_t
    /// @throws DataStoreException if a uint32_t cannot be extracted from the buffer
    uint32_t getUInt32();

    /// Add/serialize an int32_t to the buffer
    /// @param i an int32_t
    /// @throws DataStoreException if an int32_t cannot be added to the buffer
    void addInt32(int32_t i);

    /// Extract/de-serialize an int32_t from the buffer
    /// @return an int32_t
    /// @throws DataStoreException if an int32_t cannot be extracted from the buffer
    int32_t getInt32();

    /// Add/serialize a uint64_t to the buffer
    /// @param i a uint64_t
    /// @throws DataStoreException if a uint64_t cannot be added to the buffer
    void addUInt64(uint64_t i);

    /// Extract/de-serialize a uint64_t from the buffer
    /// @return a uint64_t
    /// @throws DataStoreException if a uint64_t cannot be extracted from the buffer
    uint64_t getUInt64();

    /// Add/serialize an int64_t to the buffer
    /// @param i an int64_t
    /// @throws DataStoreException if an int64_t cannot be added to the buffer
    void addInt64(int64_t i);

    /// Extract/de-serialize an int64_t from the buffer
    /// @return an int64_t
    /// @throws DataStoreException if an int64_t cannot be extracted from the buffer
    int64_t getInt64();

    /// Add/serialize a float to the buffer
    /// @param f a single-precision floating point number
    /// @throws DataStoreException if a float cannot be added to the buffer
    void addFloat(float f);

    /// Extract/de-serialize a float from the buffer
    /// @param a single-precision floating point number
    /// @throws DataStoreException if a float cannot be extracted from the buffer
    float getFloat();

    /// Add/serialize a double to the buffer
    /// @param d a double-precision floating point number
    /// @throws DataStoreException if a double cannot be added to the buffer
    void addDouble(double d);

    /// Extract/de-serialize a double from the buffer
    /// @return a double-precision floating point number
    /// @throws DataStoreException if a double cannot be extracted from the buffer
    double getDouble();

    /// Add/serialize a long double to the buffer
    /// @param d a long double floating point number
    /// @throws DataStoreException if a long double cannot be added to the buffer
    void addLongDouble(long double d);

    /// Extract/de-serialize a long double from the buffer
    /// @return a long double floating point number
    /// @throws DataStoreException if a long double cannot be extracted from the buffer
    long double getLongDouble();

    /// Add/serialize a NULL-terminated string to the buffer
    /// @param str a NULL-terminated string
    /// @throws DataStoreException if a NULL-terminated string cannot be added to the buffer
    void addNTStr(char const* str);

    /// Extract/de-serialize a NULL-terminated string from the buffer.
    /// User is responsible for de-allocating the returned memory with delete[].
    /// @return a pointer to a NULL-terminated string
    /// @throws DataStoreException if a NULL-terminated string cannot be extracted from the buffer
    char* getNTStr();

    /// Add/serialize an STL string to the buffer
    /// @param str an STL string
    /// @throws DataStoreException if an STL string cannot be added to the buffer
    void addSTLString(const std::string& str);

    /// Extract/de-serialize an STL string from the buffer
    /// @return an STL string
    /// @throws DataStoreException if an STL string cannot be extracted from the buffer
    std::string getSTLString();

    /// Extract/de-serialize an STL string from the buffer
    /// @param str STL string to assign to
    /// @throws DataStoreException if an STL string cannot be extracted from the buffer
    void getSTLString(std::string& str);

    /// Add/serialize an SPL string to the buffer
    /// @param str an SPL string
    /// @throws DataStoreException if an SPL string cannot be added to the buffer
    void addSPLString(const RSTRING_BB_TYPE& str);

    /// Extract/de-serialize an SPL string from the buffer
    /// @return an SPL string
    /// @throws DataStoreException if an SPL string cannot be extracted from the buffer
    RSTRING_BB_TYPE getSPLString();

    /// Extract/de-serialize an SPL string from the buffer
    /// @param str SPL string to assign to
    /// @throws DataStoreException if an SPL string cannot be extracted from the buffer
    void getSPLString(RSTRING_BB_TYPE& str);

    /// Add/serialize a ustring to the buffer
    /// @param ustr a ustring
    /// @throws DataStoreException if a ustring cannot be added to the buffer
    void addUnicodeString(const USTRING_BB_TYPE& ustr);

    /// Extract/de-serialize a ustring from the buffer
    /// @return a ustring
    /// @throws DataStoreException if a ustring cannot be added to the buffer
    USTRING_BB_TYPE getUnicodeString();

    /// Add/serialize an opaque blob to the buffer
    /// @param blob a pointer to an opaque blob
    /// @param blobsize the opaque blob size
    /// @throws DataStoreException if the data cannot be added to the buffer
    void addBlob(const void* blob, const uint32_t blobsize);

    /// Extract/de-serialize an opaque blob from the buffer.
    /// User is responsible for de-allocating the returned memory with delete[].
    /// @param size the size of the returned opaque blob
    /// @return a pointer to an opaque blob
    /// @throws DataStoreException if an opaque blob cannot be extracted from the buffer
    unsigned char* getBlob(uint32_t& size);

    /// Add/serialize data pointed by the given pointer to the buffer.
    /// The type T must be serializable to buffer.
    /// @param value a pointer
    /// @return the DataStoreByteBuffer instance
    /// @throws DataStoreException if the DataStoreByteBuffer is in READ mode or
    /// if the given data cannot be added to DataStoreByteBuffer
    template<class T>
    DataStoreByteBuffer& operator<<(const T*& value)
    {
        return (*this << *value);
    }

    /// Extract/de-serialize data of type T from the DataStoreByteBuffer.
    /// The type T must be de-serializable from DataStoreByteBuffer.
    /// The caller is responsible for de-allocating the memory returned by value with delete.
    /// @param value return the data extracted from the DataStoreByteBuffer
    /// @return the DataStoreByteBuffer instance
    /// @throws DataStoreException if the DataStoreByteBuffer is in WRITE or APPEND mode or
    /// if data cannot be extracted from DataStoreByteBuffer
    /// @throws std::bad_alloc if memory allocation fails
    template<class T>
    DataStoreByteBuffer& operator>>(T*& value)
    {
        value = new T;
        *this >> *value;
        return *this;
    }

    /// Add/serialize a sequence of characters to the buffer.
    /// NOTE: the size of the sequence is not serialized (for that use addBlob).
    /// @param chbuf a pointer to a sequence of characters
    /// @param chbufsize the size for the sequence of characters
    /// @throws DataStoreException if the data cannot be added to the buffer
    void addCharSequence(char const* chbuf, uint32_t chbufsize);
    virtual void addCharSequence(char const* chbuf, const uint64_t& chbufsize);

    /// Extract/de-serialize a sequence of charaters from the serialization
    /// buffer -- the sequence begins at the output cursor position and
    /// ends at the end of the buffer
    /// User is responsible for de-allocating the returned memory
    /// @param sizeTillEOB return the size of the sequence until the end of the buffer
    /// @return a pointer to sequence of characters
    /// @throws DataStoreException if a sequence of charaters cannot be extracted from the buffer
    char* getCharSequence(uint32_t& sizeTillEOB);
    virtual char* getCharSequence(uint64_t& sizeTillEOB);

    /// Extract/de-serialize a fixed-size sequence of characters from
    /// the buffer -- the sequence begins at the output cursor position.
    /// User is responsible for de-allocating the returned memory with delete[].
    /// @param mysize the size of the sequence
    /// @return a pointer to sequence of characters
    /// @throws DataStoreException if a sequence of charaters cannot be extracted from the buffer
    char* getFixedCharSequence(uint32_t mysize);
    virtual char* getFixedCharSequence(const uint64_t& mysize);

    /// Extract/de-serialize a fixed-size sequence of characters from the
    /// buffer into a provided or allocated buffer
    /// @param buffer User provided buffer (of size bsize) or NULL
    /// @param bsize the size of the sequence to extract
    /// @return a pointer to sequence of characters. This must be freed by the user if it is not
    /// equal to buffer.
    /// @throws DataStoreException if a fixed-size sequence of charaters cannot be extracted from
    /// the buffer
    char* getFixedCharSequence(char* buffer, uint32_t bsize);
    virtual char* getFixedCharSequence(char* buffer, const uint64_t& bsize);

    /// Extract/de-serialize a sequence of charaters from the buffer into a user-provided buffer --
    /// the sequence begins at the output cursor position
    /// @param buffer user provided buffer (of size bsize)
    /// @param bsize the size of the sequence to extract
    /// @param returnSize return the size of the actually retrieved sequence
    /// @throws DataStoreException if a sequence of charaters cannot be extracted from the buffer
    virtual void getFixedCharSequence(char* buffer, const uint64_t& bsize, uint64_t& returnSize);

    /// Add/serialize a pointer to the buffer
    /// @param ptr a void* pointer
    /// @throws DataStoreException if a void* pointer cannot be added to the buffer
    void addPointer(const void* ptr);

    /// Extract/de-serialize a pointer from the buffer
    /// @return an void* pointer
    /// @throws DataStoreException if a void* pointer cannot be extracted from the buffer
    void* getPointer();

    /// Obtain the size of the current content in the buffer
    /// @return the size of the current content in the buffer
    /// @throws DataStoreException if the size cannot be determined
    uint32_t getContentSize() const;

    /// Obtain the size of the current content in the buffer
    /// @param size return the size of the current content in the buffer
    /// @throws DataStoreException if the size cannot be determined
    virtual void getContentSize(uint64_t& size) const;

    /// Obtain number of serialized bytes remaining from the location
    /// pointed by the output cursor till the end of the buffer
    /// @return number of serialized bytes remaining in buffer
    /// @throws DataStoreException if the size cannot be determined
    uint32_t getNRemainingBytes() const;

    /// Obtain number of serialized bytes remaining from the location
    /// pointed by the output cursor till the end of the buffer
    /// @param size return number of serialized bytes remaining in buffer
    /// @throws DataStoreException if the size cannot be determined
    virtual void getNRemainingBytes(uint64_t& size) const;

    /// Get the internal buffer output cursor
    /// @return current output cursor
    /// @throws DataStoreException if the buffer is in WRITE or APPEND mode
    uint32_t getOCursor() const;

    /// Get the internal buffer output cursor
    /// @param size return current output cursor
    /// @throws DataStoreException if the buffr is in WRITE or APPEND mode
    virtual void getOCursor(uint64_t& cursor) const;

    /// Set the cursor for the next de-serialization operation
    /// @param offset the new offset
    void setOCursor(uint64_t offset);

    /// Mark the end of WRITE or APPEND operation.
    /// Call this function when all the WRITE or APPEND operations are done
    /// so that the data can be written to backend store.
    /// To use this function, the Data Store Byte Buffer must be in WRITE or
    /// APPEND mode, otherwise a DataStoreException will be thrown.
    /// User can optionally pass meta-data, the size of which can be no larger than
    /// DataStoreByteBuffer::MAX_META_DATA_SIZE Bytes. Such meta-data is stored
    /// within the header of the Data Store Byte Buffer and can be read via
    /// getUserMetaData() function.
    /// @param metaData user-provided meta-data
    /// @param size size of user-provided meta-data; Note that it cannot be more than
    /// DataStoreByteBuffer::MAX_META_DATA_SIZE Bytes.
    /// @throws DataStoreException if any error happens
    virtual void finishWrite(const char* metaData = NULL, const uint8_t size = 0);

    /// Get the user-provided meta-data written in finishWrite() function.
    /// To use this function, the Data Store Byte Buffer must be in READ mode, otherwise a
    /// DataStoreException will be thrown.
    /// @param size return the size of meta-data
    /// @return the meta-data written in finishWrite() function, return NULL if no meta-data
    /// was written
    const char* getUserMetaData(uint8_t& size) const;

    /// Get create time of this DataStoreByteBuffer
    /// @return create time of this DataStoreByteBuffer
    SPL::timestamp getCreateTime() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
  protected:
    /// Default Constructor
    DataStoreByteBuffer();

    /// Add data to the buffer, and handle chunking if necessary
    /// @param value the data to add
    /// @param size the size of value (in Bytes)
    /// @throws DataStoreException if any error happens
    virtual void addInternal(const char* value, const uint64_t& size);

    /// Get data from the buffer, and handle chunking if necessary
    /// @param value return the retrieved data, the memory is provided by caller
    /// @param size the size of value to read (in Bytes)
    /// @throws DataStoreException if any error happens
    virtual void getInternal(char* value, const uint64_t& size);

    /// Encode length
    /// @param len length to encode
    /// @throws DataStoreException if any error happens
    void serializeLength(uint32_t len);

    /// Decode length
    /// @return length decoded
    /// @throws DataStoreException if any error happens
    uint32_t deserializeLength();

    /// Write current chunk in the given buffer to backend store
    /// @param address address of current chunk
    /// @throws DataStoreException if any error happens
    void writeCurrentChunk(const char* address);

    /// Load next chunk from backend store into the given destination buffer
    /// @param address address of destination buffer
    /// @throws DataStoreException if any error happens
    void readNextChunk(char* address);

    /// Re-allocate the chunk buffer
    /// @param extra additional size (in Bytes) to allocate
    /// @throws DataStoreException if resizing fails
    void resize(const uint32_t extra);

    /// Truncate the Byte Buffer (remove the chunks from 0 to the given maximum chunk Number)
    void truncate();

    const static char PTR_32BIT = '3';
    const static char PTR_64BIT = '6';
    const static uint8_t MAX_META_DATA_SIZE = 35;

    std::string key_;                 // key of this Data Store Byte Buffer
    DataStoreEntryImpl* storeEntry_;  // handle to the Data Store Entry
    Mode mode_;                       // operation mode
    char* buffer_;                    // internal buffer for serializing/deserializating one chunk
    uint32_t cursor_;                 // read/write position in current chunk
    uint32_t chunkNum_;               // current chunk's Number
    uint32_t bufferSize_;             // size of internal buffer
    uint32_t chunkSize_;              // size of current chunk in buffer
    uint64_t totalSize_;              // total size of the data (used in READ mode)
    uint32_t lastChunkSize_;          // size of last chunk (used in READ mode)
    uint32_t maxChunkNum_;            // the last chunk's Number
    DataStoreUpdateBatchImpl* batch_; // handle to the Data Store Update Batch
    SPL::timestamp timestamp_;        // create time
    bool writeFinished_;       // whether writing is finished successfully (true) or not (false)
    char* userMetaData_;       // user-provided meta-data
    uint8_t userMetaDataSize_; // size of user-provided meta-data
#endif
};

} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_BYTE_BUFFER_H
