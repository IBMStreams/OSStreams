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

#ifndef SPL_RUNTIME_SERIALIZATION_NATIVE_BYTE_BUFFER_H
#define SPL_RUNTIME_SERIALIZATION_NATIVE_BYTE_BUFFER_H

/*!
 * \file NativeByteBuffer.h \brief Definition of the SPL::NativeByteBuffer class.
 */

#include <limits>
#include <string.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Serialization/ByteBuffer.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <cassert>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{

	class PayloadContainer;
    /// \brief Class that represents a binary serialization buffer, using
    /// native byte ordering.
    ///
    /// Objects can be serialiazed into the buffer or de-serialized from the
    /// buffer in binary form. In a typical case, the underlying memory is
    /// managed (allocated/resized/deallocated) by the NativeByteBuffer
    /// class. Such a behavior can be changed by handing a buffer to the
    /// constructor that takes an externally allocated buffer.  In such case the
    /// underlying memory will have to be deallocated externally, unless
    /// setAutoDealloc is invoked to re-assign the memory management tasks to
    /// the class.
    ///
    /// Important note: This class is highly optimized for speed, and thus does
    /// not perform any bounds checking for its operations.
    class DLL_PUBLIC NativeByteBuffer : public ByteBuffer<NativeByteBuffer>
    {
    public:
        /// Default Constructor
        ///
        NativeByteBuffer();

        /// Constructor
        /// @param initialSize buffer initial size (the buffer is automatically
        /// resized when necessary if autoDealloc is true, i.e., the buffer
        /// itself has not been externally alloced)
        NativeByteBuffer(uint64_t initialSize);

        /// Constructor -- used for handing externally allocated buffers to
        /// NativeByteBuffer (the default behavior is for the memory management
        /// capabilities to reside with the external entity, i.e., by default
        /// memory will have to be deallocated externally).
        /// The input cursor for serialization is initialized to point to the
        /// end of the buffer, and the output cursor for deserialization is
        /// initialized to point to the beginning of the buffer.
        /// @param mybuf an externally allocated buffer
        /// @param mysize the buffer size (this buffer is allocated externally --
        /// resizings can only be done later if autoDealloc is set to be true)
        NativeByteBuffer(unsigned char * mybuf, uint64_t mysize);

        /// Copy constructor
        /// @param sb an existing NativeByteBuffer object
        NativeByteBuffer(NativeByteBuffer const & sb);

        /// Destructive copy Constructor
        /// @param sb an existing NativeByteBuffer object
        /// @param isDestructive true means that the original object will be
        /// destroyed (i.e., the internal buffer for the original buffer will be
        /// nullified)
        NativeByteBuffer(NativeByteBuffer & sb, bool isDestructive);

        /// Destructor
        ///
        ~NativeByteBuffer();

        /// Assign a serialization buffer object to another
        /// @param sb an existing NativeByteBuffer object
        NativeByteBuffer & assign(NativeByteBuffer const & sb);

        /// Copy the serialized contents of another buffer object
        /// Do not change any properties of the buffer, just the data gets copied
        /// @param sb an existing NativeByteBuffer object
        NativeByteBuffer & copyData(NativeByteBuffer const & sb);

        /// Add/serialize a character to the serialization buffer
        /// @param ch a character
        void addChar(char ch)
        {
            ensure(sizeof(char));
            buf[icursor] = ch;
            icursor++;
        }

        /// Extract/de-serialize a character from the serialization buffer
        /// @return a character
        char getChar()
        {
            char ch = buf[ocursor];
            ocursor++;
            return ch;
        }

        /// Add/serialize a boolean to the serialization buffer
        /// @param b a boolean
        void addBool(bool b)
        {
            ensure(sizeof(bool));
            buf[icursor] = b;
            icursor++;
        }

        /// Extract/de-serialize a boolean from the serialization buffer
        /// @return a boolean
        bool getBool()
        {
            bool b = buf[ocursor];
            ocursor++;
            return b;
        }

        /// Add/serialize an uint8_t to the serialization buffer
        /// @param i a uint8_t
        void addUInt8(uint8_t i)
        {
            ensure(sizeof(uint8_t));
            buf[icursor] = i;
            icursor++;
        }

        /// Extract/de-serialize an uint8_t from the serialization buffer
        /// @return a uint8_t
        uint8_t getUInt8()
        {
            char i = buf[ocursor];
            ocursor++;
            return i;
        }

        /// Add/serialize an int8_t to the serialization buffer
        /// @param i an int8_t
        void addInt8(int8_t i)
        {
            ensure(sizeof(int8_t));
            buf[icursor] = i;
            icursor++;
        }

        /// Extract/de-serialize an int8_t from the serialization buffer
        /// @return an int8_t
        int8_t getInt8()
        {
            char i = buf[ocursor];
            ocursor++;
            return i;
        }

        /// Add/serialize an uint16_t to the serialization buffer
        /// @param i a uint16_t
        void addUInt16(uint16_t i)
        {
            ensure(sizeof(uint16_t));
            memcpy(buf+icursor,&i,sizeof(uint16_t));
            icursor += sizeof(uint16_t);
        }

        /// Extract/de-serialize a uint16_t from the serialization buffer
        /// @return a uint16_t
        uint16_t getUInt16()
        {
            uint16_t i;
            memcpy(&i,buf+ocursor,sizeof(uint16_t));
            ocursor += sizeof(uint16_t);
            return i;
        }

        /// Add/serialize an int16_t to the serialization buffer
        /// @param i an int16_t
        void addInt16(int16_t i)
        {
            ensure(sizeof(int16_t));
            memcpy(buf+icursor,&i,sizeof(int16_t));
            icursor += sizeof(int16_t);
        }

        /// Extract/de-serialize an int16_t from the serialization buffer
        /// @return an int16_t
        int16_t getInt16()
        {
            int16_t i;
            memcpy(&i,buf+ocursor,sizeof(int16_t));
            ocursor += sizeof(int16_t);
            return i;
        }

        /// Add/serialize an uint32_t to the serialization buffer
        /// @param i a uint32_t
        void addUInt32(uint32_t i)
        {
            ensure(sizeof(uint32_t));
            memcpy(buf+icursor,&i,sizeof(uint32_t));
            icursor += sizeof(uint32_t);
        }

        /// Extract/de-serialize a uint32_t from the serialization buffer
        /// @return a uint32_t
        uint32_t getUInt32()
        {
            uint32_t i;
            memcpy(&i,buf+ocursor,sizeof(uint32_t));
            ocursor += sizeof(uint32_t);
            return i;
        }

        /// Add/serialize an int32_t to the serialization buffer
        /// @param i an int32_t
        void addInt32(int32_t i)
        {
            ensure(sizeof(int32_t));
            memcpy(buf+icursor,&i,sizeof(int32_t));
            icursor += sizeof(int32_t);
        }

        /// Extract/de-serialize an int32_t from the serialization buffer
        /// @return an int32_t
        int32_t getInt32()
        {
            int32_t i;
            memcpy(&i,buf+ocursor,sizeof(int32_t));
            ocursor += sizeof(int32_t);
            return i;
        }

        /// Add/serialize a uint64_t to the serialization buffer
        /// @param i a uint64_t
        void addUInt64(uint64_t i)
        {
            ensure(sizeof(uint64_t));
            memcpy(buf+icursor,&i,sizeof(uint64_t));
            icursor += sizeof(uint64_t);
        }

        /// Extract/de-serialize a uint64_t from the serialization buffer
        /// @return a uint64_t
        uint64_t getUInt64()
        {
            uint64_t i;
            memcpy(&i,buf+ocursor,sizeof(uint64_t));
            ocursor += sizeof(uint64_t);
            return i;
        }

        /// Add/serialize an int64_t to the serialization buffer
        /// @param i an int64_t
        void addInt64(int64_t i)
        {
            ensure(sizeof(int64_t));
            memcpy(buf+icursor,&i,sizeof(int64_t));
            icursor += sizeof(int64_t);
        }

        /// Extract/de-serialize an int64_t from the serialization buffer
        /// @return an int64_t
        int64_t getInt64()
        {
            int64_t i;
            memcpy(&i,buf+ocursor,sizeof(int64_t));
            ocursor += sizeof(int64_t);
            return i;
        }

        /// Add/serialize a float to the serialization buffer
        /// @param f a single-precision floating point number
        void addFloat(float f)
        {
            ensure(sizeof(float));
            memcpy(buf+icursor,&f,sizeof(float));
            icursor += sizeof(float);
        }

        /// Extract/de-serialize a float from the serialization buffer
        /// @return a single-precision floating point number
        float getFloat()
        {
            float f;
            memcpy(&f,buf+ocursor,sizeof(float));
            ocursor += sizeof(float);
            return f;
        }

        /// Add/serialize a double to the serialization buffer
        /// @param d a double-precision floating point number
        void addDouble(double d)
        {
            ensure(sizeof(double));
            memcpy(buf+icursor,&d,sizeof(double));
            icursor += sizeof(double);
        }

        /// Extract/de-serialize a double from the serialization buffer
        /// @return a double-precision floating point number
        double getDouble()
        {
            double d;
            memcpy(&d,buf+ocursor,sizeof(double));
            ocursor += sizeof(double);
            return d;
        }

        /// Add/serialize a long double to the serialization buffer
        /// @param d a long double floating point number
        void addLongDouble(long double d)
        {
            // We want to ensure that we have the same size on all machines (x86 & x64 in particular)
            ensure(16);
            memcpy(buf+icursor,&d,sizeof(long double));
            icursor += 16;
        }

        /// Extract/de-serialize a long double from the serialization buffer
        /// @return a long double floating point number
        long double getLongDouble()
        {
            long double d;
            // We want to ensure that we have the same size on all machines (x86 & x64 in particular)
            memcpy(&d,buf+ocursor,sizeof(long double));
            ocursor += 16;
            return d;
        }

        /// Add/serialize a NULL-terminated string to the serialization buffer
        /// @param str a NULL terminated string
        void addNTStr(char const * str)
        {
            // adds a NULL Terminated String
            uint32_t stlen = strlen(str)+1;
            ensure(stlen);
            memcpy(buf+icursor,str,stlen);
            icursor += stlen;
        }

        /// Extract/de-serialize a NULL-terminated string from the
        /// serialization buffer
        /// @return a pointer to a NULL-terminated string
        char * getNTStr()
        {
            uint32_t stlen = strlen(reinterpret_cast<char *>(buf)+ocursor)+1;
            char * ptr = reinterpret_cast<char *>(buf)+ocursor;
            ocursor += stlen;
            return ptr;
        }

        /// Add/serialize an STL string to the serialization buffer
        /// @param str an STL string
        void addSTLString(std::string const & str)
        {
            uint32_t stlen = str.size();
            serializeLength(stlen);
            if(stlen>0) {
                ensure(stlen);
                memcpy(buf+icursor,str.c_str(),stlen);
                icursor += stlen;
            }
        }

        /// Extract/de-serialize an STL string from the serialization buffer
        /// @return an STL string
        std::string getSTLString()
        {
            uint32_t stlen = deserializeLength();
            if(stlen>0) {
                char * ptr = reinterpret_cast<char *>(buf)+ocursor;
                ocursor += stlen;
                return std::string(ptr,stlen);
            }
            return std::string("");
        }

        /// Extract/de-serialize an STL string from the serialization buffer
        /// @param str STL string to be extracted
        void getSTLString(std::string& str)
        {
            uint32_t stlen = deserializeLength();
            char * ptr = reinterpret_cast<char *>(buf)+ocursor;
            ocursor += stlen;
            str.assign (ptr, stlen);
        }

        /// Add/serialize an SPL string to the serialization buffer
        /// @param str an SPL string
        void addSPLString(RSTRING_BB_TYPE const & str)
        {
            uint32_t stlen = str.size();
            serializeLength(stlen);
            if(stlen>0) {
                ensure(stlen);
                memcpy(buf+icursor,str.c_str(),stlen);
                icursor += stlen;
            }
        }

        /// Extract/de-serialize an SPL string from the serialization buffer
        /// @return an SPL string
        RSTRING_BB_TYPE getSPLString()
        {
            uint32_t stlen = deserializeLength();
            if(stlen>0) {
                char * ptr = reinterpret_cast<char *>(buf)+ocursor;
                ocursor += stlen;
                return RSTRING_BB_TYPE(ptr,stlen);
            }
            return RSTRING_BB_TYPE("");
        }

        /// Extract/de-serialize an SPL string from the serialization buffer
        /// @param str SPL string to be extracted
        void getSPLString(RSTRING_BB_TYPE& str)
        {
            uint32_t stlen = deserializeLength();
            char * ptr = reinterpret_cast<char *>(buf)+ocursor;
            ocursor += stlen;
            str.assign (ptr, stlen);
        }

        /// Add/serialize a ustring to the serialization buffer
        /// @param ustr a ustring
        void addUnicodeString(USTRING_BB_TYPE const & ustr)
        {
            uint32_t stlen = ustr.length();
            serializeLength(stlen);
            const uint16_t * ptr = ustr.getBuffer();
            for (uint32_t i = 0; i < stlen; ++i) {
                addUInt16(ptr[i]);
            }
        }

        /// Extract/de-serialize a ustring from the
        /// serialization buffer
        /// @return a ustring
        USTRING_BB_TYPE getUnicodeString();

        /// Add/serialize an opaque blob to the serialization buffer
        /// @param blob a pointer to an opaque blob
        /// @param bsize the opaque blob size
        void addBlob(void const * blob, uint32_t bsize)
        {
            addUInt32(bsize);
            ensure(bsize);
            memcpy(buf+icursor,blob,bsize);
            icursor += bsize;
        }

        /// Extract/de-serialize an opaque blob from the serialization buffer
        /// @param mysize the size of the returned opaque blob
        /// @return a pointer to an opaque blob
        unsigned char * getBlob(uint32_t & mysize)
        {
            mysize = getUInt32();
            unsigned char * ptr = buf+ocursor;
            ocursor += mysize;
            return ptr;
        }

        /// Add/serialize a sequence of characters to the serialization buffer
        /// NOTE: the size of the sequence is not serialized (for that use
        /// addBlob)
        /// @param chsbuf a pointer to a sequence of characters
        /// @param chssize the size for the sequence of characters
        void addCharSequence(char const * chsbuf, uint32_t chssize)
        {
            ensure(chssize);
            memcpy(buf+icursor,chsbuf,chssize);
            icursor += chssize;
        }

        /// Extract/de-serialize a sequence of characters from the serialization
        /// buffer -- the sequence begins at the output cursor position and
        /// ends at the end of the serialization buffer
        /// @param sizeTillEOB the size of the sequence until the end of the
        /// serialization buffer
        /// @return a pointer to sequence of characters
        char * getCharSequence(uint32_t & sizeTillEOB)
        {
            sizeTillEOB = icursor-ocursor;
            unsigned char * ptr = buf+ocursor;
            ocursor = size;
            return reinterpret_cast<char *>(ptr);
        }

        /// Extract/de-serialize a fixed-size sequence of characters from
        /// the serialization buffer -- the sequence begins at the output cursor
        /// position
        /// @param mysize the size of the sequence
        /// serialization buffer
        /// @return a pointer to sequence of characters
        char * getFixedCharSequence(uint32_t mysize)
        {
            unsigned char * ptr = buf+ocursor;
            ocursor += mysize;
            return reinterpret_cast<char *>(ptr);
        }

        /// Add/serialize a pointer to the serialization buffer
        /// @param ptr a void* pointer
        void addPointer(void const * ptr)
        {
#ifdef __LP64__
            assert(sizeof(void *)==sizeof(int64_t));
            addChar(PTR_64BIT);
            int64_t addr = reinterpret_cast<int64_t>(ptr);
            addInt64(addr);
#else
            assert(sizeof(void *)==sizeof(int32_t));
            addChar(PTR_32BIT);
            int32_t addr = reinterpret_cast<int32_t>(ptr);
            addInt32(addr);
#endif
        }

        /// Extract/de-serialize a pointer from the serialization buffer
        /// @return an void* pointer
        /// @throws a SPLRuntimeDeserializationException if a pointer cannot
        /// be extracted from the buffer
        void * getPointer()
        {
            char c = getChar();
#ifdef __LP64__
            assert(sizeof(void *)==sizeof(int64_t));
            if (c != PTR_64BIT) {
                THROW_STRING(SPLRuntimeDeserialization, SPL_APPLICATION_RUNTIME_NBB_POINTER(64));
            }
            int64_t ptr = getInt64();
            return reinterpret_cast<void *>(ptr);
#else
            assert(sizeof(void *)==sizeof(int32_t));
            if(c!=PTR_32BIT)
                THROW_STRING(SPLRuntimeDeserialization, SPL_APPLICATION_RUNTIME_NBB_POINTER(32));
            int32_t ptr = getInt32();
            return reinterpret_cast<void *>(ptr);
#endif
        }

        /// Set the cursor for the next serialization operation
        /// @param off the new offset
        void setICursor(uint64_t off)
        {
            icursor = off;
        }

        /// Get the internal buffer input cursor
        /// @return current input cursor buffer cursor
        uint64_t getICursor() const
        {
            return icursor;
        }

        /// Obtain the size of the content currently in the buffer
        /// @return the size of the content currently in the buffer
        uint64_t getContentSize() const
        {
            return icursor;
        }

        /// Obtain the size of the buffer
        /// @return the size of the buffer
        uint64_t getSize() const
        {
            return size;
        }

        /// Set the cursor for the next de-serialization operation
        /// @param off the new offset
        void setOCursor(uint64_t off)
        {
            ocursor = off;
        }

        /// Get the internal buffer output cursor
        /// @return current output cursor buffer cursor
        uint64_t getOCursor() const
        {
            return ocursor;
        }

        /// Obtain number of serialized bytes remaining from the location
        /// pointed by the output cursor till the end of the buffer
        uint64_t getNRemainingBytes() const
        {
            return icursor-ocursor;
        }

        /// Obtain the pointer for the internal buffer
        /// @return an unsigned char* pointer to the internal buffer
        unsigned char * getPtr()
        {
            return buf;
        }

        /// Obtain the (const) pointer for the internal buffer
        /// @return a unsigned char* pointer to the internal buffer
        unsigned char const * getPtr() const
        {
            return buf;
        }

        /// Obtain the actual size of the serialized data (as opposed to the
        /// size of the serialization buffer)
        /// @return the size of the serialized data
        uint64_t getSerializedDataSize() const
        {
            return icursor;
        }

        /// Set/reset the auto deallocation flag (true means that the buffer
        /// is self-managed and false means that the buffer is externally
        /// managed/owned)
        /// @param myAutoDealloc the new value for the auto deallocation flag
        void setAutoDealloc(bool myAutoDealloc)
        {
            autoDealloc = myAutoDealloc;
        }

        /// Equality Operator, checks if the buffer size and binary contents are the same
        /// @param b the sbuffer with which we are checking equality with
        /// @return true if they are equivalent, false otherwise
        bool operator ==(NativeByteBuffer const & b) const;

        /// Inequality operator, checks if the buffer size and binary contents
        /// are the same
        /// @param b the sbuffer with which we are checking equality with
        /// @return true if they are not equivalent, false otherwise
        bool operator !=(NativeByteBuffer const & b) const
        {
            return !(this->operator ==(b));
        }

#ifndef DOXYGEN_SKIP_FOR_USERS

        /// Return the payload
        /// @return payload pointer
        const PayloadContainer * getPayloadContainer() const
        {
            return payload_;
        }

        /// Return the payload
        /// @return payload pointer
        PayloadContainer * getPayloadContainer()
        {
            return payload_;
        }

        /// Set the payload pointer
        /// @param payload New payload pointer
        void setPayloadContainer(PayloadContainer * payload);

        /// Copy the serialized contents of another buffer object
        /// Do not change any properties of the buffer, just the data gets copied
        /// @param sb an existing NativeByteBuffer object
        /// @param offset the offset at which to copy the data
        NativeByteBuffer & copyData(NativeByteBuffer const & sb, size_t offset);

    protected:

        ///constructs the payload
        void constructPayload(NativeByteBuffer const & rhs);

        /// Ensure buffer has extra space, by resizing if needed
        /// @param extra required extra space
        /// @throws a SPLRuntimeSerialization if the original buffer to be
        /// resized was externally allocated
        void ensure(uint64_t extra)
        {
            if (icursor + extra > size) {
                resize(extra + icursor - size);
            }
        }

        /// Resize buffer
        /// @param extra the additional number of bytes necessary in the resizing
        /// @throws a SPLRuntimeSerialization if the original buffer to be
        /// resized was externally allocated
        void resize(uint64_t extra)
        {
            if (!autoDealloc) {
                THROW_STRING(SPLRuntimeSerialization,
                             SPL_APPLICATION_RUNTIME_RESIZE_FIXED_BUFFER(size, extra));
            }

            uint64_t maxGrowth = 1ull + std::numeric_limits<uint32_t>::max() - size;
            uint64_t growth = extra;
            if (extra > maxGrowth) {
                THROW_STRING(SPLRuntimeSerialization,
                             SPL_APPLICATION_RUNTIME_RESIZE_BUFFER_TOO_BIG(size, extra, maxGrowth));
            }
            // testing whether we have to more than double the current buffer
            // size
            if (extra > size) {
                growth = extra; // grow by the required amount
            } else {
                // we know that we can grow by the necessary amount, but
                // let's see if we can double the size
                if (size > maxGrowth) {
                    // we can't double the size, so let's grow half of that
                    growth = std::max(maxGrowth/2, extra);
                } else {
                    // we can double the size, so let's do it
                    growth = size;
                }
            }

            uint64_t newsize = size + growth;
            unsigned char * newbuf = alloc(newsize);

            if (size > 0) {
                memcpy(newbuf, buf, size);
            }

            dealloc(buf);
            size = newsize;
            buf = newbuf;
        }

        /// Allocate internal buffer
        /// @param mysize amount of memory to be allocated
        unsigned char * alloc(uint64_t mysize)
        {
            return new unsigned char[mysize];
        }

        /// Deallocated memory
        void dealloc(unsigned char * mybuf)
        {
            delete[] mybuf;
        }

        /// Encode length
        void serializeLength(uint32_t len)
        {
            if(len<128) { // if fits in a signed int8
                addUInt8(len); // put the length using the first byte
            } else { // otherwise
                addUInt8(128); // put the first byte as a negative int8
                addUInt32(len); // put the length using the next 4 bytes
            }
        }

        /// Decode length
        uint32_t deserializeLength()
        {
            int8_t len = getInt8();
            if (len >= 0) { // if we have a positive int8
                return len; // it is the length
            } else {        // otherwise
                return getUInt32(); // next 4 bytes is the length
            }
        }

        /// is the buffer self-allocated/self-managed
        bool autoDealloc;

        /// buffer
        unsigned char * buf;

        /// buffer size
        uint64_t size;

        /// the cursor for data that is being serialized (i.e., added to
        /// the buffer)
        uint64_t icursor;

        /// ocursor: is the cursor for data that is being de-serialized (i.e.,
        /// being read and de-serialized from the buffer)
        uint64_t ocursor;

        PayloadContainer * payload_;
#endif /* DOXYGEN_SKIP_FOR_USERS */
    };
};

#include <SPL/Runtime/Serialization/NativeByteBufferHelpers.h>

#endif /* SPL_RUNTIME_SERIALIZATION_NATIVE_BYTE_BUFFER_H */
