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

//
// Class Description:
//
// Implements a resizable buffer used for serialization purposes

#ifndef SERIALIZATIONBUFFER_H
#define SERIALIZATIONBUFFER_H

#include "unicode/unistr.h"
#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>
#include <cassert>
#include <exception>
#include <inttypes.h>
#include <string.h>

UTILS_NAMESPACE_BEGIN

/// This class implements an abstract serialization buffer class, where
/// objects can be serialiazed into the buffer or de-serialized from the
/// buffer
class SerializationBuffer
{
  public:
    virtual ~SerializationBuffer(void) {} // To silence compiler warning
    enum
    {
        /// buffer default size
        DEFAULT_SIZE = 4096
    };
    static const char PTR_32BIT;
    static const char PTR_64BIT;

    /// Is the underlying implementation being implemented without a memory buffer?
    /// @return 'true' if there is no contiguous buffer that can be easily examined.
    /// This means that getCharPointer, and other functions that return a pointer into
    /// a buffer will THROW an error if called.  Use their alternate forms if needed
    virtual bool isMemoryless() const { return false; }

    /// Add/serialize a character to the serialization buffer
    /// @param c a character
    virtual void addChar(const char c) = 0;

    /// Extract/de-serialize a character from the serialization buffer
    /// @return a character
    /// @throws a SerializationException if a character cannot be extracted
    /// from the buffer
    virtual char getChar(void) = 0;

    /// Add/serialize a UChar to the serialization buffer
    /// @param c a UChar
    virtual void addUChar(const UChar c) = 0;

    /// Extract/de-serialize a UChar from the serialization buffer
    /// @return a UChar
    /// @throws a SerializationException if a UChar cannot be extracted
    /// from the buffer
    virtual UChar getUChar(void) = 0;

    /// Add/serialize a boolean to the serialization buffer
    /// @param b a boolean
    virtual void addBool(const bool b) = 0;

    /// Extract/de-serialize a boolean from the serialization buffer
    /// @return a boolean
    /// @throws a SerializationException if a boolean cannot be extracted
    /// from the buffer
    virtual bool getBool(void) = 0;

    /// Add/serialize an uint8_t to the serialization buffer
    /// @param i a uint8_t
    virtual void addUInt8(const uint8_t i) = 0;

    /// Extract/de-serialize a uint8_t from the serialization buffer
    /// @return an uint8_t
    /// @throws a SerializationException if a uint8_t cannot be extracted
    /// from the buffer
    virtual uint8_t getUInt8(void) = 0;

    /// Add/serialize an int8_t to the serialization buffer
    /// @param i an int8_t
    virtual void addInt8(const int8_t i) = 0;

    /// Extract/de-serialize an int8_t from the serialization buffer
    /// @return an int8_t
    /// @throws a SerializationException if an int8_t cannot be extracted
    /// from the buffer
    virtual int8_t getInt8(void) = 0;

    /// Add/serialize an uint16_t to the serialization buffer
    /// @param i a uint16_t
    virtual void addUInt16(const uint16_t i) = 0;

    /// Extract/de-serialize a uint16_t from the serialization buffer
    /// @return an uint16_t
    /// @throws a SerializationException if a uint16_t cannot be extracted
    /// from the buffer
    virtual uint16_t getUInt16(void) = 0;

    /// Add/serialize an int16_t to the serialization buffer
    /// @param i an int16_t
    virtual void addInt16(const int16_t i) = 0;

    /// Extract/de-serialize an int16_t from the serialization buffer
    /// @return an int16_t
    /// @throws a SerializationException if an int16_t cannot be extracted
    /// from the buffer
    virtual int16_t getInt16(void) = 0;

    /// Add/serialize a uint32_t to the serialization buffer
    /// @param i a uint32_t
    virtual void addUInt32(const uint32_t i) = 0;

    /// Extract/de-serialize a uint32_t from the serialization buffer
    /// @return a uint32_t
    /// @throws a SerializationException if a uint32_t cannot be extracted
    /// from the buffer
    virtual uint32_t getUInt32(void) = 0;

    /// Add/serialize an int32_t to the serialization buffer
    /// @param i an int32_t
    virtual void addInt32(const int32_t i) = 0;

    /// Extract/de-serialize an int32_t from the serialization buffer
    /// @return an int32_t
    /// @throws a SerializationException if an int32_t cannot be extracted
    /// from the buffer
    virtual int32_t getInt32(void) = 0;

    /// Add/serialize a uint64_t to the serialization buffer
    /// @param i a uint64_t
    virtual void addUInt64(const uint64_t i) = 0;

    /// Extract/de-serialize a uint64_t from the serialization buffer
    /// @return a uint64_t
    /// @throws a SerializationException if a uint64_t cannot be extracted
    /// from the buffer
    virtual uint64_t getUInt64(void) = 0;

    /// Add/serialize an int64_t to the serialization buffer
    /// @param i an int64_t
    virtual void addInt64(const int64_t i) = 0;

    /// Extract/de-serialize an int64_t from the serialization buffer
    /// @return an int64_t
    /// @throws a SerializationException if an int64_t cannot be extracted
    /// from the buffer
    virtual int64_t getInt64(void) = 0;

    /// Add/serialize a float to the serialization buffer
    /// @param f a float (floating point number)
    virtual void addFloat(const float f) = 0;

    /// Extract/de-serialize a float from the serialization buffer
    /// @return a float
    /// @throws a SerializationException if a float cannot be extracted
    /// from the buffer
    virtual float getFloat(void) = 0;

    /// Add/serialize a double to the serialization buffer
    /// @param d a double floating point number)
    virtual void addDouble(const double d) = 0;

    /// Extract/de-serialize a double from the serialization buffer
    /// @return a double
    /// @throws a SerializationException if a double cannot be extracted
    /// from the buffer
    virtual double getDouble(void) = 0;

    /// Add/serialize a NULL-terminated string to the serialization buffer
    /// @param str a NULL terminated string
    virtual void addNTStr(const char* str) = 0;

    /// Extract/de-serialize a NULL-terminated string from the
    /// serialization buffer
    /// @return a pointer to a NULL-terminated string
    /// @throws a SerializationException if a NULL-terminated string cannot
    /// be extracted from the buffer
    virtual char* getNTStr(void) = 0;

    /// Extract/de-serialize a NULL-terminated string from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size bsize) or NULL
    /// @param bsize     size of buffer (if buffer != NULL)
    /// @return a pointer to a NULL-terminated string.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SerializationException if a NULL-terminated string cannot
    /// be extracted from the buffer
    virtual char* getNTStr(char* buffer, unsigned bsize)
    {
        // default behaviour
        char* s = getNTStr();
        assert(s);
        unsigned len = strlen(s);
        if (!buffer || len >= bsize) {
            buffer = new char[len + 1];
        }
        strcpy(buffer, s);
        return buffer;
    }

    /// Add/serialize an STL string to the serialization buffer
    /// @param str an STL string
    virtual void addSTLString(const std::string& str) = 0;

    /// Extract/de-serialize an STL string from the serialization buffer
    /// @return an STL string
    /// @throws a SerializationException if an STL string cannot
    /// be extracted from the buffer
    virtual std::string getSTLString(void) = 0;

    /// Add/serialize an ICU Unicode string to the serialization buffer
    /// @param ustr an ICU Unicode string
    virtual void addUnicodeString(const icu::UnicodeString& ustr) = 0;

    /// Extract/de-serialize an ICU Unicode string from the
    /// serialization buffer
    /// @return an ICU Unicode string
    virtual icu::UnicodeString getUnicodeString(void) = 0;

    /// Add/serialize an opaque blob to the serialization buffer
    /// @param blob a pointer to an opaque blob
    /// @param blobsize the opaque blob size
    virtual void addBlob(const void* blob, const uint32_t blobsize) = 0;

    /// Extract/de-serialize an opaque blob from the serialization buffer
    /// @param size the size of the returned opaque blob
    /// @return a pointer to an opaque blob
    /// @throws a SerializationException if an opaque blob cannot
    /// be extracted from the buffer
    virtual unsigned char* getBlob(unsigned& size) = 0;

    /// Extract/de-serialize an opaque blob from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size bsize) or NULL
    /// @param bsize     size of buffer (if buffer != NULL)
    /// @return a pointer to n opaque blob.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SerializationException if an opaque blob cannot
    /// be extracted from the buffer
    virtual unsigned char* getBlob(unsigned char* buffer, unsigned bsize)
    {
        // default behaviour
        unsigned size;
        unsigned char* s = getBlob(size);
        assert(s);
        if (!buffer || size > bsize) {
            buffer = new unsigned char[size];
        }
        memcpy(buffer, s, size);
        return buffer;
    }

    /// Add/serialize a sequence of characters to the serialization buffer
    /// NOTE: the size of the sequence is not serialized (for that use
    /// addBlob)
    /// @param buf a pointer to a sequence of characters
    /// @param bufsize the size for the sequence of characters
    virtual void addCharSequence(const char* chbuf, const uint32_t chbufsize) = 0;

    /// Extract/de-serialize a sequence of charaters from the serialization
    /// buffer -- the sequence begins at the output cursor position and
    /// ends at the end of the serialization buffer
    /// @param sizeTillEOB the size of the sequence until the end of the
    /// serialization buffer
    /// @return a pointer to sequence of characters
    virtual char* getCharSequence(uint32_t& sizeTillEOB) = 0;

    /// Extract/de-serialize a fixed-size sequence of characters from
    /// the serialization buffer -- the sequence begins at the output cursor
    /// position
    /// @param mysize the size of the sequence
    /// serialization buffer
    /// @return a pointer to sequence of characters
    virtual char* getFixedCharSequence(const uint32_t mysize) = 0;

    /// Extract/de-serialize a fixed-size sequence of characters from the
    /// serialization buffer into a provided or allocated buffer
    /// @param buffer    User provided buffer (of size bsize) or NULL
    /// @param bsize     the size of the sequence
    /// @return a pointer to sequence of characters.  This must be freed by the
    /// caller if it is not == buffer.
    /// @throws a SerializationException if a NULL-terminated string cannot
    /// be extracted from the buffer
    virtual char* getFixedCharSequence(char* buffer, unsigned bsize)
    {
        // default behaviour
        char* s = getFixedCharSequence(bsize);
        assert(s);
        if (!buffer) {
            buffer = new char[bsize];
        }
        memcpy(buffer, s, bsize);
        return buffer;
    }

    /// Add/serialize a pointer to the serialization buffer
    /// @param ptr a void* pointer
    virtual void addPointer(const void* ptr) = 0;

    /// Extract/de-serialize a pointer from the serialization buffer
    /// @return an void* pointer
    /// @throws a SerializationException if a void* pointer cannot
    /// be extracted from the buffer
    virtual void* getPointer(void) = 0;

    /// Obtain the size of the content current in the buffer
    /// @return the size of the content current in the buffer
    virtual unsigned getContentSize(void) const = 0;

    /// Obtain the pointer for the internal buffer
    /// @return a char* pointer to the internal buffer
    virtual char* getCharPtr(void)
    {
        // no default implementation provided
        assert(0);
        return NULL;
    };

    /// Obtain the (const) pointer for the internal buffer
    /// @return a char* pointer to the internal buffer
    virtual const char* getCharPtr(void) const
    {
        // no default implementation provided
        assert(0);
        return NULL;
    };

    /// Obtain number of serialized bytes remaining from the location
    /// pointed by the output cursor till the end of the buffer
    virtual unsigned getNRemainingBytes(void) const
    {
        // no default implementation provided
        assert(0);
        return 0;
    }

    /// Get the internal buffer output cursor
    /// @return current output cursor buffer cursor
    virtual unsigned getOCursor(void) const
    {
        // no default implementation provided
        assert(0);
        return 0;
    };
};

DECL_EXCEPTION(UTILS_NAMESPACE, Serialization, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, ReadOnlyBuffer, Serialization);

UTILS_NAMESPACE_END

#include <UTILS/SerializationBufferHelpers.h>

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
