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

#ifndef SPL_RUNTIME_SERIALIZATION_BYTE_BUFFER_H
#define SPL_RUNTIME_SERIALIZATION_BYTE_BUFFER_H

/*!
 * \file ByteBuffer.h \brief Definition of the SPL::ByteBuffer class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#include <inttypes.h>
#include <string>
#include <dst-config.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

#include <SPL/Runtime/Type/UString.h>

namespace SPL
{
    class rstring;
    #define RSTRING_BB_TYPE SPL::rstring
    #define USTRING_BB_TYPE SPL::ustring

    /// \brief Class that serves as the base for the NativeByteBuffer and
    /// NetworkByteBuffer classes.
    ///
    /// This is a class template that uses the <i>curiously recurring template
    /// pattern</i> to implement compile-time polymorphism.
    template <class BufferType>
    class DLL_PUBLIC ByteBuffer
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        enum
        {
            DEFAULT_SIZE = 4096  //!< Default buffer size in bytes
        };
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Add/serialize a character to the serialization buffer
        /// @param c a character
        void addChar(char c)
        {
            static_cast<BufferType *>(this)->addChar(c);
        }

        /// Extract/de-serialize a character from the serialization buffer
        /// @return a character
        char getChar()
        {
            return static_cast<BufferType *>(this)->getChar();
        }

        /// Add/serialize a boolean to the serialization buffer
        /// @param b a boolean
        void addBool(bool b)
        {
            static_cast<BufferType *>(this)->addBool(b);
        }

        /// Extract/de-serialize a boolean from the serialization buffer
        /// @return a boolean
        bool getBool()
        {
            return static_cast<BufferType *>(this)->getBool();
        }

        /// Add/serialize an uint8_t to the serialization buffer
        /// @param i a uint8_t
        void addUInt8(uint8_t i)
        {
            static_cast<BufferType *>(this)->addUInt8(i);
        }

        /// Extract/de-serialize a uint8_t from the serialization buffer
        /// @return an uint8_t
        uint8_t getUInt8()
        {
            return static_cast<BufferType *>(this)->getUInt8();
        }

        /// Add/serialize an int8_t to the serialization buffer
        /// @param i an int8_t
        void addInt8(int8_t i)
        {
            static_cast<BufferType *>(this)->addInt8(i);
        }

        /// Extract/de-serialize an int8_t from the serialization buffer
        /// @return an int8_t
        int8_t getInt8()
        {
            return static_cast<BufferType *>(this)->getInt8();
        }

        /// Add/serialize an uint16_t to the serialization buffer
        /// @param i a uint16_t
        void addUInt16(uint16_t i)
        {
            static_cast<BufferType *>(this)->addUInt16(i);
        }

        /// Extract/de-serialize a uint16_t from the serialization buffer
        /// @return a uint16_t
        uint16_t getUInt16()
        {
            return static_cast<BufferType *>(this)->getInt16();
        }

        /// Add/serialize an int16_t to the serialization buffer
        /// @param i an int16_t
        void addInt16(int16_t i)
        {
            static_cast<BufferType *>(this)->addInt16(i);
        }

        /// Extract/de-serialize an int16_t from the serialization buffer
        /// @return an int16_t
        int16_t getInt16()
        {
            return static_cast<BufferType *>(this)->getInt16();
        }

        /// Add/serialize a uint32_t to the serialization buffer
        /// @param i a uint32_t
        void addUInt32(uint32_t i)
        {
            static_cast<BufferType *>(this)->addUInt32(i);
        }

        /// Extract/de-serialize a uint32_t from the serialization buffer
        /// @return a uint32_t
        uint32_t getUInt32()
        {
            return static_cast<BufferType *>(this)->getUInt32();
        }

        /// Add/serialize an int32_t to the serialization buffer
        /// @param i an int32_t
        void addInt32(int32_t i)
        {
            static_cast<BufferType *>(this)->addInt32(i);
        }

        /// Extract/de-serialize an int32_t from the serialization buffer
        /// @return an int32_t
        int32_t getInt32()
        {
            return static_cast<BufferType *>(this)->getInt32();
        }

        /// Add/serialize a uint64_t to the serialization buffer
        /// @param i a uint64_t
        void addUInt64(uint64_t i)
        {
            static_cast<BufferType *>(this)->addUInt64(i);
        }

        /// Extract/de-serialize a uint64_t from the serialization buffer
        /// @return a uint64_t
        uint64_t getUInt64()
        {
            return static_cast<BufferType *>(this)->getUInt64();
        }

        /// Add/serialize an int64_t to the serialization buffer
        /// @param i an int64_t
        void addInt64(int64_t i)
        {
            static_cast<BufferType *>(this)->addInt64(i);
        }

        /// Extract/de-serialize an int64_t from the serialization buffer
        /// @return an int64_t
        int64_t getInt64()
        {
            return static_cast<BufferType *>(this)->getInt64();
        }

        /// Add/serialize a float to the serialization buffer
        /// @param f a float (floating point number)
        void addFloat(float f)
        {
            static_cast<BufferType *>(this)->addFloat(f);
        }

        /// Extract/de-serialize a float from the serialization buffer
        /// @return a float
        float getFloat()
        {
            return static_cast<BufferType *>(this)->getFloat();
        }

        /// Add/serialize a double to the serialization buffer
        /// @param d a double floating point number)
        void addDouble(double d)
        {
            static_cast<BufferType *>(this)->addDouble(d);
        }

        /// Extract/de-serialize a double from the serialization buffer
        /// @return a double
        double getDouble()
        {
            return static_cast<BufferType *>(this)->getDouble();
        }

        /// Add/serialize a long double to the serialization buffer
        /// @param d a long double floating point number)
        void addLongDouble(long double d)
        {
            static_cast<BufferType *>(this)->addLongDouble(d);
        }

        /// Extract/de-serialize a long double from the serialization buffer
        /// @return a double
        long double getLongDouble()
        {
            return static_cast<BufferType *>(this)->getLongDouble();
        }

        /// Add/serialize a NULL-terminated string to the serialization buffer
        /// @param str a NULL terminated string
        void addNTStr(char const * str)
        {
            static_cast<BufferType *>(this)->addNTStr(str);
        }

        /// Extract/de-serialize a NULL-terminated string from the
        /// serialization buffer
        /// @return a pointer to a NULL-terminated string
        char * getNTStr()
        {
            return static_cast<BufferType *>(this)->getNTStr();
        }

        /// Add/serialize an STL string to the serialization buffer
        /// @param str an STL string
        void addSTLString(std::string const & str)
        {
            static_cast<BufferType *>(this)->addSTLString(str);
        }

        /// Extract/de-serialize an STL string from the serialization buffer
        /// @return an STL string
        std::string getSTLString()
        {
            return static_cast<BufferType *>(this)->getSTLString();
        }

        /// Extract/de-serialize an STL string from the serialization buffer
        /// @param str string to be assigned
        void getSTLString(std::string& str)
        {
            static_cast<BufferType *>(this)->getSTLString(str);
        }

       /// Add/serialize an SPL string to the serialization buffer
        /// @param str an SPL string
        inline void addSPLString(RSTRING_BB_TYPE const & str);


        /// Extract/de-serialize an SPL string from the serialization buffer
        /// @return an STL string
        inline RSTRING_BB_TYPE getSPLString();


        /// Extract/de-serialize an SPL string from the serialization buffer
        /// @param str string to be assigned
        inline void getSPLString(RSTRING_BB_TYPE & str);


        /// Add/serialize a ustring to the serialization buffer
        /// @param ustr a ustring
        void addUnicodeString(USTRING_BB_TYPE const & ustr)
        {
            static_cast<BufferType *>(this)->addUnicodeString(ustr);
        }

        /// Extract/de-serialize a ustring from the
        /// serialization buffer
        /// @return a ustring
        USTRING_BB_TYPE getUnicodeString()
        {
            return static_cast<BufferType *>(this)->getUnicodeString();
        }

        /// Add/serialize an opaque blob to the serialization buffer
        /// @param blob a pointer to an opaque blob
        /// @param blobsize the opaque blob size
        void addBlob(void const * blob, uint32_t blobsize)
        {
            static_cast<BufferType *>(this)->addBlob(blob, blobsize);
        }

        /// Extract/de-serialize an opaque blob from the serialization buffer
        /// @param size the size of the returned opaque blob
        /// @return a pointer to an opaque blob
        unsigned char * getBlob(uint32_t & size)
        {
            return static_cast<BufferType *>(this)->getBlob(size);
        }

        /// Add/serialize a sequence of characters to the serialization buffer
        /// NOTE: the size of the sequence is not serialized (for that use
        /// addBlob)
        /// @param chbuf a pointer to a sequence of characters
        /// @param chbufsize the size for the sequence of characters
        void addCharSequence(char const * chbuf, uint32_t chbufsize)
        {
            static_cast<BufferType *>(this)->addCharSequence(chbuf, chbufsize);
        }

        /// Extract/de-serialize a sequence of charaters from the serialization
        /// buffer -- the sequence begins at the output cursor position and
        /// ends at the end of the serialization buffer
        /// @param sizeTillEOB this is an out-parameter that will store the size
        /// of the returned char sequence
        /// @return a pointer to sequence of characters
        char * getCharSequence(uint32_t & sizeTillEOB)
        {
            return static_cast<BufferType *>(this)->getCharSequence(sizeTillEOB);
        }

        /// Extract/de-serialize a fixed-size sequence of characters from
        /// the serialization buffer -- the sequence begins at the output cursor
        /// position
        /// @param mysize the size of the sequence
        /// serialization buffer
        /// @return a pointer to sequence of characters
        char * getFixedCharSequence(uint32_t mysize)
        {
            return static_cast<BufferType *>(this)->getFixedCharSequence(mysize);
        }

        /// Add/serialize a pointer to the serialization buffer
        /// @param ptr a void* pointer
        void addPointer(void const * ptr)
        {
            static_cast<BufferType *>(this)->addPointer(ptr);
        }

        /// Extract/de-serialize a pointer from the serialization buffer
        /// @return a void* pointer
        void * getPointer()
        {
            return static_cast<BufferType *>(this)->getPointer();
        }

        /// Obtain the size of the content current in the buffer
        /// @return the size of the content current in the buffer
        uint32_t getContentSize() const
        {
            return static_cast<BufferType const *>(this)->getContentSize();
        }

        /// Obtain the pointer for the internal buffer
        /// @return a unsigned char* pointer to the internal buffer
        unsigned char * getPtr()
        {
            return static_cast<BufferType *>(this)->getCharPtr();
        }

        /// Obtain the (const) pointer for the internal buffer
        /// @return a unsigned char* pointer to the internal buffer
        unsigned char const * getPtr() const
        {
            return static_cast<BufferType *>(this)->getCharPtr();
        }

        /// Obtain number of serialized bytes remaining from the location
        /// pointed by the output cursor till the end of the buffer
        uint32_t getNRemainingBytes() const
        {
            return static_cast<BufferType const *>(this)->getNRemainingBytes();
        }

        /// Get the internal buffer output cursor
        /// @return current output cursor buffer cursor
        uint32_t getOCursor() const
        {
            return static_cast<BufferType const *>(this)->getOCursor();
        };
#ifndef DOXYGEN_SKIP_FOR_USERS
    protected:
        static const char PTR_32BIT;
        static const char PTR_64BIT;
#endif /* DOXYGEN_SKIP_FOR_USERS */
    };

#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class BufferType>
    const char ByteBuffer<BufferType>::PTR_32BIT='3';
    template <class BufferType>
    const char ByteBuffer<BufferType>::PTR_64BIT='6';
#endif /* DOXYGEN_SKIP_FOR_USERS */

};

#include <SPL/Runtime/Serialization/ByteBufferHelpers.h>

#endif /* SPL_RUNTIME_SERIALIZATION_BYTE_BUFFER */
