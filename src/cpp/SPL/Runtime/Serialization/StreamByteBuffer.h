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

#ifndef SPL_RUNTIME_SERIALIZATION_STREAM_BYTE_BUFFER_H
#define SPL_RUNTIME_SERIALIZATION_STREAM_BYTE_BUFFER_H

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
/// This class implements a serialization buffer, where an iostream is used as the buffer.
/// This allows serialization to/from an iostream.  There is no buffer associated with
/// this serialization buffer
class DLL_PUBLIC StreamByteBuffer : public VirtualByteBuffer
{
  public:
    /// Constructor
    /// @param stream IO Stream to be used for the serialization
    StreamByteBuffer(std::iostream& stream)
      : _stream(stream)
    {}

    /// Destructor
    ~StreamByteBuffer() {}

    /// Return the stream used
    /// @return the iostream used for serialization/deserialization
    std::iostream& getStream() { return _stream; }

    /// Return the stream used
    /// @return the iostream used for serialization/deserialization
    const std::iostream& getStream() const { return _stream; }

    /// Is the underlying implementation being implemented without a memory buffer?
    /// @return 'true' if there is no contiguous buffer that can be easily examined.
    /// This means that getCharPointer, and other functions that return a pointer into
    /// a buffer will THROW an error if called.  Use their alternate forms if needed
    virtual bool isMemoryless() const { return true; }

    /// Add/serialize a character to the serialization buffer
    /// @param c a character
    void addChar(const char c);

    /// Extract/de-serialize a character from the serialization buffer
    /// @return a character
    /// @throws a SPLRuntimeSerializationException if a character cannot be extracted
    /// from the buffer
    char getChar(void);

    /// Add/serialize a boolean to the serialization buffer
    /// @param b a boolean
    void addBool(const bool b);

    /// Extract/de-serialize a boolean from the serialization buffer
    /// @return a boolean
    /// @throws a SPLRuntimeSerializationException if a boolean cannot be extracted
    /// from the buffer
    bool getBool(void);

    // HANDLING UINT8 -------------------------------------------------------------

    /// Add/serialize an uint8_t to the serialization buffer
    /// @param i an uint8_t
    void addUInt8(const uint8_t i);

    /// Extract/de-serialize an uint8_t from the serialization buffer
    /// @return an uint8_t
    /// @throws a SPLRuntimeSerializationException if an uint8_t cannot be extracted
    /// from the buffer
    uint8_t getUInt8(void);

    // HANDLING INT8 --------------------------------------------------------------

    /// Add/serialize an int8_t to the serialization buffer
    /// @param i an int8_t
    void addInt8(const int8_t i);

    /// Extract/de-serialize an int8_t from the serialization buffer
    /// @return an int8_t
    /// @throws a SPLRuntimeSerializationException if an int8_t cannot be extracted
    /// from the buffer
    int8_t getInt8(void);

    // HANDLING UINT16 ------------------------------------------------------------

    /// Add/serialize an uint16_t to the serialization buffer
    /// @param i an uint16_t
    void addUInt16(const uint16_t i);

    /// Extract/de-serialize a uint16_t from the serialization buffer
    /// @return a uint16_t
    /// @throws a SPLRuntimeSerializationException if a uint16_t cannot be extracted
    /// from the buffer
    uint16_t getUInt16(void);

    // HANDLING INT16 -------------------------------------------------------------

    /// Add/serialize an int16_t to the serialization buffer
    /// @param i an int16_t
    void addInt16(const int16_t i);

    /// Extract/de-serialize an int16_t from the serialization buffer
    /// @return an int16_t
    /// @throws a SPLRuntimeSerializationException if an int16_t cannot be extracted
    /// from the buffer
    int16_t getInt16(void);

    // HANDLING UINT32 ------------------------------------------------------------

    /// Add/serialize an uint32_t to the serialization buffer
    /// @param i an uint32_t
    void addUInt32(const uint32_t i);

    /// Extract/de-serialize a uint32_t from the serialization buffer
    /// @return a uint32_t
    /// @throws a SPLRuntimeSerializationException if a uint32_t cannot be extracted
    /// from the buffer
    uint32_t getUInt32(void);

    // HANDLING INT32 -------------------------------------------------------------

    /// Add/serialize an int32_t to the serialization buffer
    /// @param i an int32_t
    void addInt32(const int32_t i);

    /// Extract/de-serialize an int32_t from the serialization buffer
    /// @return an int32_t
    /// @throws a SPLRuntimeSerializationException if an int32_t cannot be extracted
    /// from the buffer
    int32_t getInt32(void);

    // HANDLING UINT64 ------------------------------------------------------------

    /// Add/serialize a uint64_t to the serialization buffer
    /// @param i a uint64_t
    void addUInt64(const uint64_t i);

    /// Extract/de-serialize a uint64_t from the serialization buffer
    /// @return a uint64_t
    /// @throws a SPLRuntimeSerializationException if a uint64_t cannot be extracted
    /// from the buffer
    uint64_t getUInt64(void);

    // HANDLING INT64 -------------------------------------------------------------

    /// Add/serialize an int64_t to the serialization buffer
    /// @param i an int64_t
    void addInt64(const int64_t i);

    /// Extract/de-serialize an int64_t from the serialization buffer
    /// @return an int64_t
    /// @throws a SPLRuntimeSerializationException if an int64_t cannot be extracted
    /// from the buffer
    int64_t getInt64(void);

    // HANDLING FLOAT -------------------------------------------------------------

    /// Add/serialize a float to the serialization buffer
    /// @param f a single-precision floating point number
    void addFloat(const float f);

    /// Extract/de-serialize a float from the serialization buffer
    /// @return a float
    /// @throws a SPLRuntimeSerializationException if a float cannot be extracted
    /// from the buffer
    float getFloat(void);

    // HANDLING DOUBLE -------------------------------------------------------------

    /// Add/serialize a double to the serialization buffer
    /// @param d a double precision floating point number
    void addDouble(const double d);

    /// Extract/de-serialize a double from the serialization buffer
    /// @return a double
    /// @throws a SPLRuntimeSerializationException if a double cannot be extracted
    /// from the buffer
    double getDouble(void);

    // HANDLING LONG DOUBLE -------------------------------------------------------

    /// Add/serialize a long double to the serialization buffer
    /// @param d a long double floating point number
    void addLongDouble(const long double d);

    /// Extract/de-serialize a long double from the serialization buffer
    /// @return a long double
    /// @throws a SPLRuntimeSerializationException if a long double cannot be extracted
    /// from the buffer
    long double getLongDouble(void);

    // ----------------------------------------------------------------------------

    /// Add/serialize a NULL-terminated string to the serialization buffer
    /// @param str a NULL terminated string
    void addNTStr(const char* str);

    /// Extract/de-serialize a NULL-terminated string from the
    /// serialization buffer
    /// @return a pointer to a NULL-terminated string
    char* getNTStr() { THROW_CHAR(SPLRuntimeSerialization, "getNTStr not supported"); }

    /// Extract/de-serialize a NULL-terminated string from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size bsize) or NULL
    /// @param bsize     size of buffer (if buffer != NULL)
    /// @return a pointer to a NULL-terminated string.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SPLRuntimeSerializationException if a NULL-terminated string cannot
    /// be extracted from the buffer
    char* getNTStr(char* buffer, uint32_t bsize);

    /// Add/serialize an STL string to the serialization buffer
    /// @param str an STL string
    void addSTLString(const std::string& str);

    /// Extract/de-serialize an STL string from the serialization buffer
    /// @return an STL string
    /// @throws a SPLRuntimeSerializationException if an STL string cannot
    /// be extracted from the buffer
    std::string getSTLString();

    /// Extract/de-serialize an STL string from the serialization buffer
    /// @param str an STL string
    /// @throws a SPLRuntimeSerializationException if an STL string cannot
    /// be extracted from the buffer
    void getSTLString(std::string& str);

    /// Add/serialize an SPL string to the serialization buffer
    /// @param str an SPL string
    void addSPLString(const RSTRING_BB_TYPE& str);

    /// Extract/de-serialize an SPL string from the serialization buffer
    /// @return an SPL string
    /// @throws a SPLRuntimeSerializationException if an SPL string cannot
    /// be extracted from the buffer
    RSTRING_BB_TYPE getSPLString();

    /// Extract/de-serialize an SPL string from the serialization buffer
    /// @param str an SPL string
    /// @throws a SPLRuntimeSerializationException if an SPL string cannot
    /// be extracted from the buffer
    void getSPLString(RSTRING_BB_TYPE& str);

    /// Add/serialize a ustring to the serialization buffer
    /// @param ustr a ustring
    void addUnicodeString(const USTRING_BB_TYPE& ustr);

    /// Extract/de-serialize a ustring from the
    /// serialization buffer
    /// @return a ustring
    USTRING_BB_TYPE getUnicodeString(void);

    /// Add/serialize an opaque blob to the serialization buffer
    /// @param blob a pointer to an opaque blob
    /// @param bsize the opaque blob size
    void addBlob(const void* blob, const uint32_t bsize);

    /// Extract/de-serialize an opaque blob from the serialization buffer
    /// @param size the size of the returned opaque blob
    /// @return a pointer to an opaque blob
    /// @throws a SPLRuntimeSerializationException - Not Supported
    unsigned char* getBlob(uint32_t& size)
    {
        THROW_CHAR(SPLRuntimeSerialization, "getNTStr not supported");
    }

    /// Extract/de-serialize an opaque blob from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size bsize) or NULL
    /// @param bsize     size of buffer (if buffer != NULL)
    /// @return a pointer to n opaque blob.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SPLRuntimeSerializationException if an opaque blob cannot
    /// be extracted from the buffer
    unsigned char* getBlob(unsigned char* buffer, uint32_t bsize);

    /// Add/serialize a sequence of characters to the serialization buffer
    /// NOTE: the size of the sequence is not serialized (for that use
    /// addBlob)
    /// @param chsbuf a pointer to a sequence of characters
    /// @param chssize the size for the sequence of characters
    void addCharSequence(const char* chsbuf, const uint32_t chssize);

    /// Extract/de-serialize a sequence of characters from the serialization
    /// buffer -- the sequence begins at the output cursor position and
    /// ends at the end of the serialization buffer
    /// @param sizeTillEOB the size of the sequence until the end of the
    /// serialization buffer
    /// @return a pointer to sequence of characters
    /// @throws SPLRuntimeSerializationException
    inline char* getCharSequence(uint32_t& sizeTillEOB)
    {
        THROW_CHAR(SPLRuntimeSerialization, "getCharSequence not supported");
    }

    /// Extract/de-serialize a fixed-size sequence of characters from
    /// the serialization buffer -- the sequence begins at the output cursor
    /// position
    /// @param size the size of the sequence
    /// serialization buffer
    /// @return a pointer to sequence of characters
    /// @throws SPLRuntimeSerializationException
    inline char* getFixedCharSequence(const uint32_t size)
    {
        THROW_CHAR(SPLRuntimeSerialization, "getCharSequence not supported");
    }

    /// Extract/de-serialize a fixed-size sequence of characters from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size mysize) or NULL
    /// @param bsize     the size of the sequence
    /// @return a pointer to sequence of characters.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SPLRuntimeSerializationException if the string cannot be extracted from the buffer
    char* getFixedCharSequence(char* buffer, uint32_t bsize);

    /// Extract/de-serialize a fixed-size sequence of characters from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size mysize) or NULL
    /// @param bsize     the size of the sequence
    /// @param resultSize set to the actual number of characters read
    /// @return a pointer to sequence of characters.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SPLRuntimeSerializationException if the string cannot be extracted from the buffer
    char* getFixedCharSequence(char* buffer, uint32_t bsize, uint32_t& resultSize);

    /// Add/serialize a pointer to the serialization buffer
    /// @param ptr a void* pointer
    void addPointer(const void* ptr);

    /// Extract/de-serialize a pointer from the serialization buffer
    /// @return an void* pointer
    /// @throws a SPLRuntimeSerializationException if a void* pointer cannot
    /// be extracted from the buffer
    void* getPointer();

    /// Obtain the size of the content currently in the buffer
    /// @return the size of the content currently in the buffer
    uint32_t getContentSize() const
    {
        THROW_CHAR(SPLRuntimeSerialization, "StreamByteBuffer does not support getContentSize");
    }

  private:
    std::iostream& _stream;
};
}

#endif /* SPL_RUNTIME_SERIALIZATION_STREAM_BYTE_BUFFER */
