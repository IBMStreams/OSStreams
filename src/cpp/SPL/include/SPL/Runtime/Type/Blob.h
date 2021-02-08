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

#ifndef SPL_RUNTIME_TYPE_BLOB_H
#define SPL_RUNTIME_TYPE_BLOB_H

/*!
 * \file Blob.h \brief Definition of the SPL::blob class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <limits>
#include <string.h>
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

PUSH_DLL_PUBLIC
namespace SPL
{
    class uint8;
    class List;

    /// \brief Class that represents blob primitive type
    class blob
    {
    public:
        /// Default constructor
        ///
        blob() : size_(0), data_(NULL) {}

        /// Constructor. The data is copied from the memory block
        /// passed in as a parameter, into the internal buffer hold by the blob
        /// @param data pointer to the memory block to be used for copying data
        /// @param size size of the memory block to be used for copying data
        blob(unsigned char const * data, uint64_t size);

        /// Constructor. The data is copied from the list
        /// passed in as a parameter, into the internal buffer hold by the blob
        /// @param data list of bytes
        blob(const List& data);

        /// Copy constructor
        /// @param ot blob to copy from
        blob(const blob & ot);

        /// Destructor
        ///
        ~blob();

        /// Assignment operator
        /// @param ot blob to assign from
        /// @return self
        blob & operator =(const blob & ot);

        /// Concatenation operator
        /// @param ot blob to assign from
        /// @return self (with appended contents)
        blob & operator +=(const blob & ot);

        /// Concatenation operator
        /// @param byte byte to be appended
        /// @return self (with appended contents)
        blob & operator +=(uint8 byte);

        /// Concatenation operator
        /// @param bytes list of bytes to be appended
        /// @return self (with appended contents)
        blob & operator +=(const List & bytes);

        /// Append to a blob
        /// @param data bytes to be appended
        /// @param size number of bytes to be appended
        void append (unsigned char const * data, uint64_t size);

        /// Concatenation operator
        /// @param ot blob to concatenate to
        /// @return blob containing concatenation of *this and ot
        blob operator +(const blob & ot) const;

        /// Equality comparison operator
        /// @param ot blob to compare with
        /// @return true if equal, false otherwise
        bool operator ==(const blob & ot) const;

        /// Non-equality comparison operator
        /// @param ot blob to compare with
        /// @return true if not equal, false otherwise
        bool operator !=(const blob & ot) const
        {
            return !(ot==*this);
        }

        /// Less than comparison operator
        /// @param ot blob to compare with
        /// @return true if less than, false otherwise
        bool operator <(const blob & ot) const;

        /// Greater than or equal comparison operator
        /// @param ot blob to compare with
        /// @return true if greater than or equal, false otherwise
        bool operator >=(const blob & ot) const
        {
            return !((*this)<ot);
        }

        /// Greater than comparison operator
        /// @param ot blob to compare with
        /// @return true if greater than, false otherwise
        bool operator >(const blob & ot) const;

        /// Less than or equal comparison operator
        /// @param ot blob to compare with
        /// @return true if less than or equal, false otherwise
        bool operator <=(const blob & ot) const
        {
            return !((*this)>ot);
        }

        /// Clear the data
        ///
        void clear()
        {
            deleteData();
        }

        /// Element access (read-only)
        /// @param idx element index
        /// @return byte at index idx
        unsigned char const & operator [](uint64_t idx) const;

        /// Element access (read-only)
        /// @param idx element index
        /// @return byte at index idx
        unsigned char const & at(uint64_t idx) const
        {
            return operator[](idx);
        }

        /// Element access (read-write)
        /// @param idx element index
        /// @return byte at index idx
        unsigned char & operator [](uint64_t idx);

        /// Element access (read-write)
        /// @param idx element index
        /// @return byte at index idx
        unsigned char & at(uint64_t idx)
        {
            return operator[](idx);
        }

        /// Get the size of the blob
        /// @return size of the blob
        uint64_t getSize() const
        {
            return size_ & ~DONT_FREE;
        }

        /// Get a pointer to the data of the blob. The management of the returned
        /// memory block still belongs to the blob object.
        /// @param size this is an out-param used to return the size of the blob's data
        /// @return pointer to the data of the blob
        unsigned char const * getData(uint64_t & size) const
        {
            size = size_ & ~DONT_FREE; return data_;
        }

        /// Get a pointer to the data of the blob. The management of the returned
        /// memory block still belongs to the blob object.
        /// @return pointer to the data of the blob
        unsigned char const * getData() const
        {
            return data_;
        }

        /// Set the data of the blob. The data is copied from the memory block
        /// passed in as a parameter, into the internal buffer hold by the blob.  The
        /// blob owns the copied in memory.
        /// @param data pointer to the memory block to be used for copying data
        /// @param size size of the memory block to be used for copying data
        void setData(unsigned char const * data, uint64_t size);

        /// Set the data of the blob by taking over the management of the memory
        /// block passed in as a parameter. No copies are performed.
        /// @param data pointer to the memory block to be adopted
        /// @param size size of the memory block to be adopted
        void adoptData(unsigned char * data, uint64_t size);

        /// Get a pointer to the data of the blob, and at the same time disassociate
        /// the blob from its data. The management of the returned memory block now
        /// does not belong to this blob anymore.
        /// @pre ownsData() == true;
        /// @param size this is an in-param used to return the size of the blob's data
        /// @return pointer to the data of the blob
        /// @throws SPLRuntimeBlobNotOwnedException if !ownsData()
        unsigned char * releaseData(uint64_t & size);

        /// Swap with another blob
        /// @param ot the blob to swap with
        void swap(blob & ot);

        /// Does this blob own the data, or it is holding memory owned by someone else?
        /// @return true if the blob owns the data
        bool ownsData() const
        {
            if (size_ & DONT_FREE) {
                return false;
            }
            return true;
        }

        /// Use external data in the blob. No copies will be done.
        /// This memory will NOT be freed when the blob is destroyed.
        /// It is the responsibility of the developer to ensure that the memory remains
        /// allocated for the duration of the blob object.
        /// @param data pointer to the memory block to be used
        /// @param size size of the memory block to be used
        void useExternalData(unsigned char * data, uint64_t size)
        {
            deleteData();
            data_ = data;
            size_ = size | DONT_FREE;
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            uint32_t size = size_ & ~DONT_FREE;
            buf.addUInt64(size);
            uint64_t ms32(std::numeric_limits<int32_t>::max());
            unsigned char * data(data_);
            while(size>ms32) {
                buf.addCharSequence((char *)(data), ms32);
                data += ms32;
                size -= ms32;
            }
            buf.addCharSequence((char *)(data), size);
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            deleteData();
            size_ = buf.getUInt64();
            uint64_t size(size_);
            uint64_t ms32(std::numeric_limits<int32_t>::max());
            data_ = new unsigned char[size_];
            unsigned char * data(data_);
            while(size>ms32) {
                buf.getFixedCharSequence((char *) data, ms32);
                data += ms32;
                size -= ms32;
            }
            buf.getFixedCharSequence((char *) data, size);
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */


        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            uint64_t size(size_ & ~DONT_FREE);
            buf.addUInt64(size);
            uint64_t ms32(std::numeric_limits<int32_t>::max());
            unsigned char * data(data_);
            while(size>ms32) {
                buf.addCharSequence((char *)(data), ms32);
                data += ms32;
                size -= ms32;
            }
            buf.addCharSequence((char *)(data), size);
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            deleteData();
            size_ = buf.getUInt64();
            uint64_t size(size_);
            uint64_t ms32(std::numeric_limits<int32_t>::max());
            data_ = new unsigned char[size_];
            unsigned char * data(data_);
            while(size>ms32) {
                char * ldata = buf.getFixedCharSequence(ms32);
                memcpy(data, ldata, ms32);
                data += ms32;
                size -= ms32;
            }
            char * ldata = buf.getFixedCharSequence(size);
            memcpy(data, ldata, size);
        }

        /// Serialize (character)
        /// @param ostr output stream
        void serialize(std::ostream & ostr) const;

        /// Deserialize (character)
        /// @param istr input stream
        void deserialize(std::istream & istr);

        /// Get hash code
        /// @return hash code
        size_t hashCode() const;

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        size_t getSerializedSize() const
        {
            return sizeof(size_) + (size_&~DONT_FREE);
        }

    private:
        // If DONT_FREE is set in size, we don't own the memory;
        enum { DONT_FREE = 0x8000000000000000ULL };
        void deleteData();
        void copyData(unsigned char const * data, uint64_t size);

        uint64_t size_;
        unsigned char * data_;
    };

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & buf, const blob & x)
    {
        x.serialize(buf); return buf;
    }

    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & buf, blob & x)
    {
        x.deserialize(buf); return buf;
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */

    inline NetworkByteBuffer& operator <<(NetworkByteBuffer& buf, const blob & x)
    {
        x.serialize(buf); return buf;
    }

    inline NativeByteBuffer& operator <<(NativeByteBuffer& buf, const blob & x)
    {
        x.serialize(buf); return buf;
    }

    template <class S>
    inline ByteBuffer<S> & operator <<(ByteBuffer<S> & buf, const blob & x)
    {
        x.serialize(buf); return buf;
    }

    template <class S>
    inline ByteBuffer<S> & operator >>(ByteBuffer<S> & buf, blob & x)
    {
        x.deserialize(buf); return buf;
    }

    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & buf, blob & x)
    {
        x.deserialize(buf); return buf;
    }

    inline NativeByteBuffer & operator >>(NativeByteBuffer & buf, blob & x)
    {
        x.deserialize(buf); return buf;
    }

    inline std::ostream & operator <<(std::ostream & ostr, const blob & x)
    {
        x.serialize(ostr); return ostr;
    }

    inline std::istream & operator >>(std::istream & istr, blob & x)
    {
        x.deserialize(istr); return istr;
    }
};

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace std
{
    template <>
    inline void swap(SPL::blob & a, SPL::blob & b)
    {
        a.swap(b);
    }
};

// Hashability requirements
namespace std {
    namespace tr1 {
        template <>
        struct hash<SPL::blob>
        {
            inline size_t operator ()(const SPL::blob & b) const
            {
                return b.hashCode();
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */
PUSH_DLL_PUBLIC

#endif /* SPL_RUNTIME_TYPE_BLOB_H */
