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

#ifndef SPL_RUNTIME_SERIALIZATION_WRAPPER_BYTE_BUFFER_H
#define SPL_RUNTIME_SERIALIZATION_WRAPPER_BYTE_BUFFER_H

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Serialization/ByteBuffer.h>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <cstring>

namespace SPL {
/// This class implements a ByteBuffer wrapper.
template<class T>
class WrapperByteBuffer : public VirtualByteBuffer
{
  public:
    /// Constructor
    WrapperByteBuffer(ByteBuffer<T>& buf)
      : buf_(buf)
    {}

    /// Destructor
    ~WrapperByteBuffer() {}

    /// Return the wrapped byte buffer
    /// @return the byte buffer used for serialization/deserialization
    ByteBuffer<T>& getByteBuffer() { return buf_; }

    /// Return the wrapped byte buffer
    /// @return the byte buffer used for serialization/deserialization
    const ByteBuffer<T>& getByteBuffer() const { return buf_; }

    /// Is the underlying implementation being implemented without a memory buffer?
    /// @return 'true' if there is no contiguous buffer that can be easily examined.
    /// This means that getCharPointer, and other functions that return a pointer into
    /// a buffer will THROW an error if called.  Use their alternate forms if needed
    virtual bool isMemoryless() const { return false; }

    /// Add/serialize a character to the serialization buffer
    /// @param c a character
    void addChar(const char c) { buf_.addChar(c); }

    /// Extract/de-serialize a character from the serialization buffer
    /// @return a character
    /// @throws a SPLRuntimeSerializationException if a character cannot be extracted
    /// from the buffer
    char getChar() { return buf_.getChar(); }

    /// Add/serialize a boolean to the serialization buffer
    /// @param b a boolean
    void addBool(const bool b) { buf_.addBool(b); }

    /// Extract/de-serialize a boolean from the serialization buffer
    /// @return a boolean
    /// @throws a SPLRuntimeSerializationException if a boolean cannot be extracted
    /// from the buffer
    bool getBool() { return buf_.getBool(); }

    // HANDLING UINT8 -------------------------------------------------------------

    /// Add/serialize an uint8_t to the serialization buffer
    /// @param i an uint8_t
    void addUInt8(const uint8_t i) { buf_.addUInt8(i); }

    /// Extract/de-serialize an uint8_t from the serialization buffer
    /// @return an uint8_t
    /// @throws a SPLRuntimeSerializationException if an uint8_t cannot be extracted
    /// from the buffer
    uint8_t getUInt8() { return buf_.getUInt8(); }

    // HANDLING INT8 --------------------------------------------------------------

    /// Add/serialize an int8_t to the serialization buffer
    /// @param i an int8_t
    void addInt8(const int8_t i) { buf_.addInt8(i); }

    /// Extract/de-serialize an int8_t from the serialization buffer
    /// @return an int8_t
    /// @throws a SPLRuntimeSerializationException if an int8_t cannot be extracted
    /// from the buffer
    int8_t getInt8() { return buf_.getInt8(); }

    // HANDLING UINT16 ------------------------------------------------------------

    /// Add/serialize an uint16_t to the serialization buffer
    /// @param i an uint16_t
    void addUInt16(const uint16_t i) { buf_.addUInt16(i); }

    /// Extract/de-serialize a uint16_t from the serialization buffer
    /// @return a uint16_t
    /// @throws a SPLRuntimeSerializationException if a uint16_t cannot be extracted
    /// from the buffer
    uint16_t getUInt16() { return buf_.getUInt16(); }

    // HANDLING INT16 -------------------------------------------------------------

    /// Add/serialize an int16_t to the serialization buffer
    /// @param i an int16_t
    void addInt16(const int16_t i) { buf_.addInt16(i); }

    /// Extract/de-serialize an int16_t from the serialization buffer
    /// @return an int16_t
    /// @throws a SPLRuntimeSerializationException if an int16_t cannot be extracted
    /// from the buffer
    int16_t getInt16() { return buf_.getInt16(); }

    // HANDLING UINT32 ------------------------------------------------------------

    /// Add/serialize an uint32_t to the serialization buffer
    /// @param i an uint32_t
    void addUInt32(const uint32_t i) { buf_.addUInt32(i); }

    /// Extract/de-serialize a uint32_t from the serialization buffer
    /// @return a uint32_t
    /// @throws a SPLRuntimeSerializationException if a uint32_t cannot be extracted
    /// from the buffer
    uint32_t getUInt32() { return buf_.getUInt32(); }

    // HANDLING INT32 -------------------------------------------------------------

    /// Add/serialize an int32_t to the serialization buffer
    /// @param i an int32_t
    void addInt32(const int32_t i) { buf_.addInt32(i); }

    /// Extract/de-serialize an int32_t from the serialization buffer
    /// @return an int32_t
    /// @throws a SPLRuntimeSerializationException if an int32_t cannot be extracted
    /// from the buffer
    int32_t getInt32() { return buf_.getInt32(); }

    // HANDLING UINT64 ------------------------------------------------------------

    /// Add/serialize a uint64_t to the serialization buffer
    /// @param i a uint64_t
    void addUInt64(const uint64_t i) { buf_.addUInt64(i); }

    /// Extract/de-serialize a uint64_t from the serialization buffer
    /// @return a uint64_t
    /// @throws a SPLRuntimeSerializationException if a uint64_t cannot be extracted
    /// from the buffer
    uint64_t getUInt64() { return buf_.getUInt64(); }

    // HANDLING INT64 -------------------------------------------------------------

    /// Add/serialize an int64_t to the serialization buffer
    /// @param i an int64_t
    void addInt64(const int64_t i) { buf_.addInt64(i); }

    /// Extract/de-serialize an int64_t from the serialization buffer
    /// @return an int64_t
    /// @throws a SPLRuntimeSerializationException if an int64_t cannot be extracted
    /// from the buffer
    int64_t getInt64() { return buf_.getInt64(); }

    // HANDLING FLOAT -------------------------------------------------------------

    /// Add/serialize a float to the serialization buffer
    /// @param f a single-precision floating point number
    void addFloat(const float f) { buf_.addFloat(f); }

    /// Extract/de-serialize a float from the serialization buffer
    /// @return a float
    /// @throws a SPLRuntimeSerializationException if a float cannot be extracted
    /// from the buffer
    float getFloat() { return buf_.getFloat(); }

    // HANDLING DOUBLE -------------------------------------------------------------

    /// Add/serialize a double to the serialization buffer
    /// @param d a double precision floating point number
    void addDouble(const double d) { buf_.addDouble(d); }

    /// Extract/de-serialize a double from the serialization buffer
    /// @return a double
    /// @throws a SPLRuntimeSerializationException if a double cannot be extracted
    /// from the buffer
    double getDouble() { return buf_.getDouble(); }

    // HANDLING LONG DOUBLE -------------------------------------------------------

    /// Add/serialize a long double to the serialization buffer
    /// @param d a long double floating point number
    void addLongDouble(const long double d) { buf_.addLongDouble(d); }

    /// Extract/de-serialize a long double from the serialization buffer
    /// @return a long double
    /// @throws a SPLRuntimeSerializationException if a long double cannot be extracted
    /// from the buffer
    long double getLongDouble() { return buf_.getLongDouble(); }

    // ----------------------------------------------------------------------------

    /// Add/serialize a NULL-terminated string to the serialization buffer
    /// @param str a NULL terminated string
    void addNTStr(const char* str) { buf_.addNTStr(str); }

    /// Extract/de-serialize a NULL-terminated string from the
    /// serialization buffer
    /// @return a pointer to a NULL-terminated string
    char* getNTStr() { return buf_.getNTStr(); }

    /// Add/serialize an STL string to the serialization buffer
    /// @param str an STL string
    void addSTLString(const std::string& str) { buf_.addSTLString(str); }

    /// Extract/de-serialize an STL string from the serialization buffer
    /// @return an STL string
    /// @throws a SPLRuntimeSerializationException if an STL string cannot
    /// be extracted from the buffer
    std::string getSTLString() { return buf_.getSTLString(); }

    /// Extract/de-serialize an STL string from the serialization buffer
    /// @param str an STL string
    /// @throws a SPLRuntimeSerializationException if an STL string cannot
    /// be extracted from the buffer
    void getSTLString(std::string& str) { buf_.getSTLString(str); }

    /// Add/serialize an SPL string to the serialization buffer
    /// @param str an SPL string
    void addSPLString(const RSTRING_BB_TYPE& str) { buf_.addSPLString(str); }

    /// Extract/de-serialize an SPL string from the serialization buffer
    /// @return an STL string
    /// @throws a SPLRuntimeSerializationException if an SPL string cannot
    /// be extracted from the buffer
    RSTRING_BB_TYPE getSPLString() { return buf_.getSPLString(); }

    /// Extract/de-serialize an SPL string from the serialization buffer
    /// @param str an SPL string
    /// @throws a SPLRuntimeSerializationException if an SPL string cannot
    /// be extracted from the buffer
    void getSPLString(RSTRING_BB_TYPE& str) { buf_.getSPLString(str); }

    /// Add/serialize a ustring to the serialization buffer
    /// @param ustr a ustring
    void addUnicodeString(const USTRING_BB_TYPE& ustr) { buf_.addUnicodeString(ustr); }

    /// Extract/de-serialize a ustring from the
    /// serialization buffer
    /// @return a ustring
    USTRING_BB_TYPE getUnicodeString() { return buf_.getUnicodeString(); }

    /// Add/serialize an opaque blob to the serialization buffer
    /// @param blob a pointer to an opaque blob
    /// @param bsize the opaque blob size
    void addBlob(const void* blob, const uint32_t bsize) { buf_.addBlob(blob, bsize); }

    /// Extract/de-serialize an opaque blob from the serialization buffer
    /// @param mysize the size of the returned opaque blob
    /// @return a pointer to an opaque blob
    /// @throws a SPLRuntimeSerializationException - Not Supported
    unsigned char* getBlob(uint32_t& mysize) { return buf_.getBlob(mysize); }

    /// Add/serialize a sequence of characters to the serialization buffer
    /// NOTE: the size of the sequence is not serialized (for that use
    /// addBlob)
    /// @param chsbuf a pointer to a sequence of characters
    /// @param chssize the size for the sequence of characters
    void addCharSequence(const char* chsbuf, const uint32_t chssize)
    {
        buf_.addCharSequence(chsbuf, chssize);
    }

    /// Extract/de-serialize a sequence of characters from the serialization
    /// buffer -- the sequence begins at the output cursor position and
    /// ends at the end of the serialization buffer
    /// @param sizeTillEOB the size of the sequence until the end of the
    /// serialization buffer
    /// @return a pointer to sequence of characters
    /// @throws SPLRuntimeSerializationException
    inline char* getCharSequence(uint32_t& sizeTillEOB)
    {
        return buf_.getCharSequence(sizeTillEOB);
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
        return buf_.getFixedCharSequence(size);
    }

    /// Extract/de-serialize a fixed-size sequence of characters from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size mysize) or NULL
    /// @param bsize     the size of the sequence
    /// @return a pointer to sequence of characters.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SPLRuntimeSerializationException if the string cannot be extracted from the buffer
    char* getFixedCharSequence(char* buffer, uint32_t bsize)
    {
        if (!buffer) {
            buffer = new char[bsize];
        }
        char* result = getFixedCharSequence(bsize);
        memcpy(buffer, result, bsize);
        return buffer;
    }

    /// Add/serialize a pointer to the serialization buffer
    /// @param ptr a void* pointer
    void addPointer(const void* ptr) { buf_.addPointer(ptr); }

    /// Extract/de-serialize a pointer from the serialization buffer
    /// @return an void* pointer
    /// @throws a SPLRuntimeSerializationException if a void* pointer cannot
    /// be extracted from the buffer
    void* getPointer() { return buf_.getPointer(); }

    /// Obtain the size of the content currently in the buffer
    /// @return the size of the content currently in the buffer
    uint32_t getContentSize() const { return buf_.getContentSize(); }

  private:
    ByteBuffer<T>& buf_;
};
}

#endif /* SPL_RUNTIME_SERIALIZATION_WRAPPER_BYTE_BUFFER */
