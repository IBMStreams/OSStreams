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

#ifndef SPL_RUNTIME_TYPE_BSTRING_H
#define SPL_RUNTIME_TYPE_BSTRING_H

/*!
 * \file BString.h \brief Definitions of the SPL::BString and SPL::bstring classes.
 */

#include <SPL/Runtime/Type/ValueHandle.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Type/TypeTMPHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Type/TypeHelper.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    /// \brief Class that defines reflective interfaces for bounded strings.
    class BString
    {
    public:
        /// Destructor
        ///
        virtual ~BString() {}

        /// Get the fixed size of the string
        /// @return fixed size of the string
        virtual uint32_t getBoundedSize() const = 0;

        /// Get the number of used characters
        /// @return number of used characters
        virtual uint32_t getUsedSize() const = 0;

        /// Get the C string representation (NULL-terminated)
        /// @return C string representation
        virtual char const * getCString() const = 0;

        /// Get the raw character data
        /// @return raw character data
        virtual char const * getData() const = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(VirtualByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(VirtualByteBuffer & buf) = 0;
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (character)
        /// @param ostr output stream
        virtual void serialize(std::ostream & ostr) const = 0;

        /// Deserialize (character)
        /// @param istr input stream
        virtual void deserialize(std::istream & istr) = 0;

        /// Assign this string using another one
        /// @param ot other string to assign from
        virtual void assignFrom(const BString & ot) = 0;

        /// Swap this string with another one
        /// @param ot other collection to swap with
        virtual void swapWith(BString & ot) = 0;

        /// Equality comparison
        /// @param ov bounded size string to compare with
        /// @return true if equal, false otherwise
        bool operator==(const BString & ov) const
        {
            if (getUsedSize() != ov.getUsedSize()) {
                return false;
            }
            return (0==memcmp(getData(), ov.getData(), getUsedSize()));
        }

        /// Non-equality comparison
        /// @param ov bounded size string to compare with
        /// @return true if not equal, false otherwise
        bool operator!=(const BString & ov) const
            {  return !(ov==*this); }

        /// Assign from another bounded size string
        /// @param ov bounded size string to assign from
        /// @return this
        const BString & operator=(const BString & ov)
            { assignFrom(ov.getData(), ov.getUsedSize()); return *this; }

        /// Assign from an STL string
        /// @param ov STL string to assign from
        /// @return this
        const BString & operator=(const std::string & ov)
            { assignFrom(ov.data(), ov.size()); return *this; }

        /// Assign from an SPL string
        /// @param ov SPL string to assign from
        /// @return this
        const BString & operator=(const rstring & ov)
            { assignFrom(ov.data(), ov.size()); return *this; }

        /// Cast to a SPL string operator
        /// @return a SPL string
        operator rstring () const
            { return rstring(getCString()); }

        /// Make a copy of this bounded size string
        ///
        virtual BString * clone() const = 0;

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        virtual size_t getSerializedSize() const = 0;

     protected:
        /// Assign from a char buffer
        /// @param data char buffer
        /// @param size used size of the string
        virtual void assignFrom(const char * data, int32_t size) = 0;

    public:
        /// Get hash code
        /// @return hash code
        virtual size_t hashCode() const = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(NativeByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(NativeByteBuffer & buf) = 0;

        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(NetworkByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(NetworkByteBuffer & buf) = 0;
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Remove all characters
        virtual void clear() = 0;

    };

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator<<(VirtualByteBuffer & buf, const BString & x)
        { x.serialize(buf); return buf; }

    inline VirtualByteBuffer & operator>>(VirtualByteBuffer & buf, BString & x)
        { x.deserialize(buf); return buf; }
#endif /* DOXYGEN_SKIP_FOR_USERS */

    template <class S>
    inline ByteBuffer<S> & operator<<(ByteBuffer<S> & buf, const BString & x)
        { x.serialize(buf); return buf; }

    inline NetworkByteBuffer & operator<<(NetworkByteBuffer & buf, const BString & x)
        { x.serialize(buf); return buf; }

    inline NativeByteBuffer & operator<<(NativeByteBuffer & buf, const BString & x)
        { x.serialize(buf); return buf; }

    template <class S>
    inline ByteBuffer<S> & operator>>(ByteBuffer<S> & buf, BString & x)
        { x.deserialize(buf); return buf; }

    inline NetworkByteBuffer & operator>>(NetworkByteBuffer & buf, BString & x)
        { x.deserialize(buf); return buf; }

    inline NativeByteBuffer & operator>>(NativeByteBuffer & buf, BString & x)
        { x.deserialize(buf); return buf; }

    inline std::ostream & operator<<(std::ostream & ostr, const BString & x)
        { x.serialize(ostr); return ostr; }

    inline std::istream & operator>>(std::istream & istr, BString & x)
        { x.deserialize(istr); return istr; }


    /// Class that represents bounded size rstring type
    template<int32_t msize>
    class bstring : public BString
    {
    public:
        typedef typename TMP::SizeType<msize>::value size_type; //!< size type
        typedef char * iterator;  //!< iterator type
        typedef char const * const_iterator; //!< const iterator type
        typedef std::reverse_iterator<iterator> reverse_iterator; //!< reverse iterator type
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //!< constant  reverse iterator type

        enum
        {
            bounded_size = msize //!< bounded size
        };

        enum
        {
            npos = -1 //!< Invalid position
        };

        /// Empty constructor
        ///
        bstring()
            { this->initialize(); }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Constructor that uses external memory
        /// @param buffer memory to use (must be in same format as bstring::Data)
        /// @param initializeIn whether bstring must be initialized or not
        bstring(void *buffer, bool initializeIn)
        {
            info_ = reinterpret_cast<Data*>(buffer);
            deallocate_ = false;
            if (initializeIn) {
                info_->usedsz_ = 0;
                info_->data_[0] = '\0';
            }
        }

        /// Adopt external buffer (similar to external memory constructor)
        /// @param buffer external buffer
        void adopt(void *buffer)
        {
            if (deallocate_) {
                delete [] info_;
            }
            info_ = reinterpret_cast<Data*>(buffer);
            deallocate_ = false;
        }
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Copy constructor
        /// @param ot bounded size string to construct from
        bstring(const bstring<msize> & ot)
            { this->initialize(); this->operator=(ot);  }

        /// Copy constructor
        /// @param ot bounded size string to construct from
        template <int32_t osize>
        bstring(const bstring<osize> & ot)
            { this->initialize(); this->operator=<osize>(ot);  }

        /// Construct from an STL string
        /// @param ot STL string to construct from
        bstring(const std::string & ot)
            { this->initialize(); this->operator=(ot);  }

        /// Construct from an SPL string
        /// @param ot SPL string to construct from
        bstring(const rstring & ot)
            { this->initialize(); this->operator=(ot);  }

        /// Construct from a C string
        /// @param ot C string to construct from
        bstring(char const * ot)
            { this->initialize(); this->operator=(ot); }

        /// Construct from a C string with a known length
        /// @param ot C string to construct from
        /// @param len number of bytes in the string
        bstring(char const * ot, uint32_t len)
            { this->initialize();
              int32_t ousz = len <= msize ? len : msize;
              memcpy (info_->data_, ot, ousz);
              info_->usedsz_ = ousz;
              info_->data_[ousz] = '\0';
            }

        /// Construct from an BString
        /// @param ot BString to construct from
        bstring(const BString & ot)
        {
            this->initialize();
            assignFrom(ot.getData(), ot.getUsedSize());
        }

        /// Construct from a value handle
        /// @param ot value handle to construct from
        bstring(const ConstValueHandle & ot)
        {
            this->initialize();
            BString const & x = ot;
            assignFrom(x.getData(), x.getUsedSize());
        }

        /// Destructor
        ///
        ~bstring()
        {
            if (deallocate_) {
                delete [] info_;
            }
        }

        BString * clone() const
            { return new bstring<msize>(*this); }

        /// Assignment operator (from another bounded size string)
        /// @param ot bounded size string to assign from
        /// @return self
        inline const bstring & operator=(const bstring<msize> & ot)
        {
            if (this == &ot) {
                return *this;
            }
            int32_t ousz = ot.info_->usedsz_;
            memcpy(info_->data_, ot.info_->data_, ousz);
            info_->data_[ousz] = '\0';
            info_->usedsz_ = ousz;
            return *this;
        }

        /// Assignment operator (from another bounded size string)
        /// @param ot bounded size string to assign from
        /// @return self
        template <int32_t osize>
        inline const bstring & operator=(const bstring<osize> & ot)
        {
            if (osize == msize && this == reinterpret_cast<bstring<msize> const*>(&ot)) {
                return *this;
            }
            int32_t ousz = ot.getUsedSize();
            if (ousz > msize) {
                ousz = msize;
            }
            memcpy(info_->data_, ot.getData(), ousz);
            info_->data_[ousz] = '\0';
            info_->usedsz_ = ousz;
            return *this;
        }

        /// Assignment operator (from an STL string)
        /// @param ot STL string to assign from
        /// @return self
        inline const bstring & operator=(const std::string & ot)
        {
            int32_t ousz = static_cast<int32_t>(ot.size());
            if (ousz > msize) {
                ousz = msize;
            }
            memcpy(info_->data_, ot.data(), ousz);
            info_->data_[ousz] = '\0';
            info_->usedsz_ = ousz;
            return *this;
        }

        /// Assignment operator (from an SPL string)
        /// @param ot SPL string to assign from
        /// @return self
        inline const bstring & operator=(const rstring & ot)
        {
            int32_t ousz = static_cast<int32_t>(ot.size());
            if (ousz > msize) {
                ousz = msize;
            }
            memcpy(info_->data_, ot.data(), ousz);
            info_->data_[ousz] = '\0';
            info_->usedsz_ = ousz;
            return *this;
        }

        /// Assignment operator (from a C string)
        /// @param ot C string to assign from
        /// @return self
        inline const bstring & operator=(char const * ot)
        {
            int32_t ousz = strlen(ot);
            if (ousz > msize) {
                ousz = msize;
            }
            memcpy(info_->data_, ot, ousz);
            info_->data_[ousz] = '\0';
            info_->usedsz_ = ousz;
            return *this;
        }

        /// Assign from a char buffer
        /// @param ot char buffer
        /// @param ousz used size of the string
        inline void assignFrom(const char * ot, int32_t ousz)
        {
            if (ousz > msize) {
                ousz = msize;
            }
            memcpy(info_->data_, ot, ousz);
            info_->data_[ousz] = '\0';
            info_->usedsz_ = ousz;
        }

        /// Assign this string using another one
        /// @param ot other string to assign from
        inline void assignFrom(const BString & ot)
            { assignFrom(ot.getData(), ot.getUsedSize()); }

        /// Swap this string with another one
        /// @param ot other collection to swap with
        void swapWith(BString & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_BSTRING_COLLECTION(msize), SPL_TYPE_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            bstring<msize> & o = static_cast<bstring<msize> &>(ot);
            this->swap<msize>(o);
        }

        /// Equality comparison
        /// @param ot bounded size string to compare with
        /// @return true if the contained strings are the same, false otherwise
        bool operator==(const bstring<msize> & ot) const
            { return this->operator==<msize>(ot); }

        /// Equality comparison
        /// @param ot bounded size string to compare with
        /// @return true if the contained strings are the same, false otherwise
        template <int32_t osize>
        bool operator==(const bstring<osize> & ot) const
        {
            if (getUsedSize() != ot.getUsedSize()) {
                return false;
            }
            return (0==memcmp(info_->data_, ot.getData(), info_->usedsz_));
        }

        /// Equality comparison
        /// @param ot C string to compare with
        /// @return true if the contained strings are the same, false otherwise
        bool operator==(char const * ot) const
        {
            if (getUsedSize() != strlen(ot)) {
                return false;
            }
            return (0==memcmp(info_->data_, ot, info_->usedsz_));
        }

        /// Equality comparison
        /// @param ot STL string to compare with
        /// @return true if the contained strings are the same, false otherwise
        bool operator==(const std::string & ot) const
        {
            if (getUsedSize() != ot.size()) {
                return false;
            }
            return (0==memcmp(info_->data_, ot.data(), info_->usedsz_));
        }

        /// Equality comparison
        /// @param ot SPL string to compare with
        /// @return true if the contained strings are the same, false otherwise
        bool operator==(const rstring & ot) const
        {
            if (getUsedSize() != ot.size()) {
                return false;
            }
            return (0==memcmp(info_->data_, ot.data(), info_->usedsz_));
        }

        /// Non-equality comparison
        /// @param ot bounded size string to compare with
        /// @return true if the contained strings are not the same, false otherwise
        bool operator!=(const bstring<msize> & ot) const
            { return !(this->operator==(ot)); }

        /// Non-equality comparison
        /// @param ot bounded size string to compare with
        /// @return true if the contained strings are not the same, false otherwise
        template <int32_t osize>
        bool operator!=(const bstring<osize> & ot) const
            { return !(this->operator==<osize>(ot)); }

        /// Non-equality comparison
        /// @param ot C string to compare with
        /// @return true if the contained strings are not the same, false otherwise
        bool operator!=(char const * ot) const
            { return !(*this==ot); }

        /// Non-equality comparison
        /// @param ot STL string to compare with
        /// @return true if the contained strings are not the same, false otherwise
        bool operator!=(const std::string & ot) const
            { return !(*this==ot); }

        /// Non-equality comparison
        /// @param ot SPL string to compare with
        /// @return true if the contained strings are not the same, false otherwise
        bool operator!=(const rstring & ot) const
            { return !(*this==ot); }

        /// Greater than comparison
        /// @param ot bounded size string to compare with
        /// @return true if (*this) > ot
        template <int32_t osize>
        bool operator>(const bstring<osize> & ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = ot.getUsedSize();
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            const char *rhs = ot.getData();
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] > rhs[i]) {
                    return true;
                }
                if (info_->data_[i] < rhs[i]) {
                    return true;
                }
            }
            // compared == for minSize characters
            if (lSize > rSize) {
                return true; // LHS is bigger; must be >
            }
            return false;
        }

        /// Greater than comparison
        /// @param ot C string to compare with
        /// @return true if (*this) > ot
        bool operator>(char const * ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = strlen(ot);
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] > ot[i]) {
                    return true;
                }
                if (info_->data_[i] < ot[i]) {
                    return true;
                }
            }
            // compared == for minSize characters
            if (lSize > rSize) {
                return true; // LHS is bigger; must be >
            }
            return false;
        }

        /// Greater than comparison
        /// @param ot std::string to compare with
        /// @return true if (*this) > ot
        bool operator>(const std::string & ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = ot.size();
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] > ot[i]) {
                    return true;
                }
                if (info_->data_[i] < ot[i]) {
                    return true;
                }
            }
            // compared == for minSize characters
            if (lSize > rSize) {
                return true; // LHS is bigger; must be >
            }
            return false;
        }

        /// Greater than comparison
        /// @param ot rstring to compare with
        /// @return true if (*this) > ot
        bool operator>(const rstring & ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = ot.size();
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] > ot[i]) {
                    return true;
                }
                if (info_->data_[i] < ot[i]) {
                    return true;
                }
            }
            // compared == for minSize characters
            if (lSize > rSize) {
                return true; // LHS is bigger; must be >
            }
            return false;
        }

        /// Less Than or Equal comparison
        /// @param ot bounded size string to compare with
        /// @return true if (*this) <= ot
        template <int32_t osize>
        bool operator<=(const bstring<osize> & ot) const
        {
            return !((*this)>ot);
        }

        /// Less Than or Equal comparison
        /// @param ot C string to compare with
        /// @return true if (*this) <= ot
        bool operator<=(char const * ot) const
        {
            return !((*this)>ot);
        }

        /// Less Than or Equal comparison
        /// @param ot std::string to compare with
        /// @return true if (*this) <= ot
        bool operator<=(const std::string & ot) const
        {
            return !((*this)>ot);
        }

        /// Less Than or Equal comparison
        /// @param ot rstring to compare with
        /// @return true if (*this) <= ot
        bool operator<=(const rstring & ot) const
        {
            return !((*this)>ot);
        }

        /// Less than comparison
        /// @param ot bounded size string to compare with
        /// @return true if (*this) < ot
        template <int32_t osize>
        bool operator<(const bstring<osize> & ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = ot.getUsedSize();
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            const char *rhs = ot.getData();
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] < rhs[i]) {
                    return true;
                }
                if (info_->data_[i] > rhs[i]) {
                    return false;
                }
            }
            // compared == for minSize characters
            if (lSize < rSize) {
                return true; // LHS is smaller; must be <
            }
            return false;
        }

        /// Less than comparison
        /// @param ot C string to compare with
        /// @return true if (*this) < ot
        bool operator<(char const * ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = strlen(ot);
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] < ot[i]) {
                    return true;
                }
                if (info_->data_[i] > ot[i]) {
                    return false;
                }
            }
            // compared == for minSize characters
            if (lSize < rSize) {
                return true; // LHS is smaller; must be <
            }
            return false;
        }

        /// Less than comparison
        /// @param ot std::string to compare with
        /// @return true if (*this) < ot
        bool operator<(const std::string & ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = ot.size();
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] < ot[i]) {
                    return true;
                }
                if (info_->data_[i] > ot[i]) {
                    return false;
                }
            }
            // compared == for minSize characters
            if (lSize < rSize) {
                return true; // LHS is smaller; must be <
            }
            return false;
        }

        /// Less than comparison
        /// @param ot rstring to compare with
        /// @return true if (*this) < ot
        bool operator<(const rstring & ot) const
        {
            uint32_t lSize = getUsedSize();
            uint32_t rSize = ot.size();
            uint32_t minSize = (lSize < rSize) ? lSize : rSize;
            for (uint32_t i = 0; i < minSize; i++) {
                if (info_->data_[i] < ot[i]) {
                    return true;
                }
                if (info_->data_[i] > ot[i]) {
                    return false;
                }
            }
            // compared == for minSize characters
            if (lSize < rSize) {
                return true; // LHS is smaller; must be <
            }
            return false;
        }

        /// Greater Than or Equal comparison
        /// @param ot bounded size string to compare with
        /// @return true if (*this) >= ot
        template <int32_t osize>
        bool operator>=(const bstring<osize> & ot) const
        {
            return !((*this)<ot);
        }

        /// Greater Than or Equal comparison
        /// @param ot C string to compare with
        /// @return true if (*this) >= ot
        bool operator>=(char const * ot) const
        {
            return !((*this)<ot);
        }

        /// Greater Than or Equal comparison
        /// @param ot std::string to compare with
        /// @return true if (*this) >= ot
        bool operator>=(const std::string & ot) const
        {
            return !((*this)<ot);
        }


        /// Greater Than or Equal comparison
        /// @param ot std::string to compare with
        /// @return true if (*this) >= ot
        bool operator>=(const rstring & ot) const
        {
            return !((*this)<ot);
        }

        /// Character subscript
        /// @param idx element index
        /// @return bstring containing the character at index idx
        /// @throws SPLRuntimeInvalidIndexException exception if the index is out of range
        inline bstring<msize> at(const size_type& idx) const
        {
            if (idx >= msize) {
                indexOutOfBound(idx, msize);
            }
            return bstring(&info_->data_[idx], 1);
        }

        /// Character access (read-only)
        /// @param idx element index
        /// @return character at index idx
        inline char const & operator[](const size_type& idx) const
            { return info_->data_[idx]; }

        /// Character access (read-write)
        /// @param idx element index
        /// @return character at index idx
        inline char & operator[](const size_type& idx)
        {
            if(idx>=info_->usedsz_) {
                info_->usedsz_ = idx+1;
                info_->data_[info_->usedsz_] = '\0';
            }
            return info_->data_[idx];
        }

        /// Get the bounded size of the string
        /// @return bounded size of the string
        inline virtual uint32_t getBoundedSize() const
            { return msize; }

        /// Get the bounded size of the string
        /// @return bounded size of the string
        inline size_type max_size() const
            { return msize; }

        /// Get the number of used characters
        /// @return number of used characters
        inline virtual uint32_t getUsedSize() const
            { return info_->usedsz_; }

        /// Get the number of used characters
        /// @return number of used characters
        inline size_type size() const
            { return info_->usedsz_; }

        /// Resize the string
        /// @param s new size
        inline void resize(size_type s) const
            { info_->usedsz_ = std::min(s, msize); }

        /// Remove all characters
        inline void clear()
            { info_->usedsz_ = 0; }

        /// Check if empty
        /// @return true if empty, false otherwise
        inline bool empty () const
            { return (info_->usedsz_==0); }

        /// Get a begin iterator
        /// @return begin iterator
        iterator begin()
            { return info_->data_; }

        /// Get a begin const iterator
        /// @return begin const iterator
        const_iterator begin () const
            { return info_->data_; }

        /// Get an end iterator
        /// @return end iterator
        iterator end()
            { return info_->data_+info_->usedsz_; }

        /// Get an end const iterator
        /// @return end const iterator
        const_iterator end() const
            { return info_->data_+info_->usedsz_; }

        /// Get a reverse begin iterator
        /// @return reverse begin iterator
        reverse_iterator rbegin()
            { return reverse_iterator(end()); }

        /// Get a reverse begin const iterator
        /// @return reverse begin const iterator
        const_reverse_iterator rbegin() const
            { return const_reverse_iterator(end()); }

        /// Get a reverse end iterator
        /// @return reverse end iterator
        reverse_iterator rend()
            { return reverse_iterator(begin()); }

        /// Get a reverse end const iterator
        /// @return reverse end const iterator
        const_reverse_iterator rend() const
            { return const_reverse_iterator(begin()); }

        /// Append bounded size string
        /// @param ot string to append
        /// @return self
        template <int32_t osize>
        inline bstring<msize> & operator+=(const bstring<osize> &  ot)
            { return this->append(ot);  }

        /// Append STL string
        /// @param ot string to append
        /// @return self
        inline bstring<msize> & operator+=(const std::string &  ot)
            { return this->append(ot); }

        /// Append SPL string
        /// @param ot string to append
        /// @return self
        inline bstring<msize> & operator+=(const rstring &  ot)
            { return this->append(ot); }

        /// Append C string
        /// @param ot string to append
        /// @return self
        inline bstring<msize> & operator+=(const char *  ot)
            { return this->append(ot); }

        /// Append character
        /// @param c character to append
        /// @return self
        inline bstring<msize> & operator+=(const char c)
            { return this->append(c); }

        /// Append bounded size string
        /// @param ot string to append
        /// @return self
        template <int32_t osize>
        inline bstring<msize> & append(const bstring<osize> & ot)
            { return this->append(ot.info_->data_, 0, ot.info_->usedsz_); }

        /// Append STL string
        /// @param ot string to append
        /// @return self
        inline bstring<msize> & append(const std::string & ot)
            { return this->append(ot.data(), 0, static_cast<int32_t>(ot.size())); }


        /// Append SPL string
        /// @param ot string to append
        /// @return self
        inline bstring<msize> & append(const rstring & ot)
            { return this->append(ot.data(), 0, static_cast<int32_t>(ot.size())); }

        /// Append bounded size string
        /// @param ot string to append
        /// @param pos position to start from in ot
        /// @param n number of characters to use from ot
        /// @return self
        template <int32_t osize>
        inline bstring<msize> & append(const bstring<osize> & ot, typename bstring<osize>::size_type pos, size_type n)
            { return this->append(ot.info_->data_, pos, n); }

        /// Append STL string
        /// @param ot string to append
        /// @param pos position to start from in ot
        /// @param n number of characters to use from ot
        /// @return self
        inline bstring<msize> & append(const std::string & ot, int32_t pos, size_type n)
            { return this->append(ot.data(), pos, n); }

        /// Append SPL string
        /// @param ot string to append
        /// @param pos position to start from in ot
        /// @param n number of characters to use from ot
        /// @return self
        inline bstring<msize> & append(const rstring & ot, int32_t pos, size_type n)
            { return this->append(ot.data(), pos, n); }

        /// Append C string
        /// @param s string to append
        /// @return self
        inline bstring<msize> & append(const char* s)
            { return this->append(s, 0, strlen(s)); }

        /// Append C string
        /// @param s string to append
        /// @param n number of characters to use from s
        /// @return self
        inline bstring<msize> & append(const char* s, int32_t n)
            { return this->append(s, 0, n); }

        /// Append character
        /// @param c character to append
        /// @return self
        inline bstring<msize> & append(char c)
            { return this->append(&c, 0, 1); }

        /// Append character
        /// @param c character to append
        /// @param n number of times to append
        /// @return self
        bstring<msize> & append(size_type n, char c)
        {
            for (size_type i = 0; i < n; ++i) {
                this->append(c);
            }
            return *this;
        }

        /// Append a sequence of characters
        /// @param first begin iterator for the sequence, inclusive
        /// @param last end iterator for the sequence, exclusive
        /// @return self
        template <class InputIterator>
        bstring<msize> & append(InputIterator first, InputIterator last)
        {
            for(InputIterator it=first; it!=last; ++it)
                { char c = *it; this->append(c); }
            return *this;
        }

        /// Append character
        /// @param c character to append
        /// @return self
        inline bstring<msize> & push_back(char c)
            { return this->append(&c, 0, 1); }

        /// Assign from a bounded size string
        /// @param ot string to assign from
        /// @return self
        template <int32_t osize>
        bstring<msize> & assign(const bstring<osize> & ot)
            { return this->assign(ot.info_->data_, 0, ot.info_->usedsz_); }

        /// Assign from an STL string
        /// @param ot string to assign from
        /// @return self
        inline bstring<msize> & assign(const std::string & ot)
            { return this->assign(ot.data(), 0, static_cast<int32_t>(ot.size())); }

        /// Assign from an SPL string
        /// @param ot string to assign from
        /// @return self
        inline bstring<msize> & assign(const rstring & ot)
            { return this->assign(ot.data(), 0, static_cast<int32_t>(ot.size())); }

        /// Assign from a bounded size string
        /// @param ot string to assign from
        /// @param pos position to start from in ot
        /// @param n number of characters to use from ot
        /// @return self
        template <int32_t osize>
        inline bstring<msize> & assign(const bstring<osize> & ot, typename bstring<osize>::size_type pos, size_type n)
            { return this->assign(ot.info_->data_, pos, n); }

        /// Assign from an STL string
        /// @param ot string to assign from
        /// @param pos position to start from in ot
        /// @param n number of characters to use from ot
        /// @return self
        inline bstring<msize> & assign(const std::string & ot, int32_t pos, size_type n)
            { return this->assign(ot.data(), pos, n); }

        /// Assign from an SPL string
        /// @param ot string to assign from
        /// @param pos position to start from in ot
        /// @param n number of characters to use from ot
        /// @return self
        inline bstring<msize> & assign(const rstring & ot, int32_t pos, size_type n)
            { return this->assign(ot.data(), pos, n); }

        /// Assign from a C string
        /// @param s string to assign from
        /// @return self
        inline bstring<msize> & assign(const char* s)
            { return this->assign(s, 0, strlen(s)); }

        /// Assign from a C string
        /// @param s string to assign from
        /// @param n number of characters to use from s
        /// @return self
        inline bstring<msize> & assign(const char* s, size_type n)
            { return this->assign(s, 0, n); }

        /// Assign from a character
        /// @param c character to use
        /// @param n number of characters
        /// @return self
        inline bstring<msize> & assign(size_type n, char c)
            { this->clear(); return this->append(n,c); }

        /// Assign from a sequence of characters
        /// @param first begin iterator for the sequence, inclusive
        /// @param last end iterator for the sequence, exclusive
        /// @return self
        template <class InputIterator>
        bstring<msize> & assign(InputIterator first, InputIterator last)
            { this->clear(); return this->append(first,last); }



        /// Get the C string representation
        /// @return C string representation
        inline char const * getCString() const
            { return info_->data_; }

        /// Get the C string representation
        /// @return C string representation
        inline char const * c_str() const
            { return info_->data_; }

        /// Get the character array representation (not necessarily NULL-terminated)
        /// @return character array representation
        inline char const * data() const
            { return info_->data_; }

        /// Get the character array representation (not necessarily NULL-terminated)
        /// @return character array representation
        inline char const * getData() const
            { return info_->data_; }

        /// Swap
        /// @param ot bounded size string to swap with
        template <int32_t osize>
        void swap(bstring<osize> & ot)
        {
            if (osize == msize && deallocate_ && ot.deallocate_) {
                // just swap pointers, size same and both controlling memory
                Data *tmp = info_;
                info_ = reinterpret_cast<Data*>(ot.info_);
                ot.info_ = reinterpret_cast<typename bstring<osize>::Data*>(tmp);
            } else {
                int32_t ousedsz = ot.getUsedSize();
                if (ousedsz > msize) {
                    ousedsz = msize;
                }
                char * tmp = new char[ousedsz];
                memcpy(tmp, ot.getData(), ousedsz);
                ot.template operator=<msize>(*this);
                memcpy(info_->data_, tmp, ousedsz);
                info_->data_[ousedsz]='\0';
                info_->usedsz_ = ousedsz;
                delete [] tmp;
            }
        }

        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            buf << info_->usedsz_;
            buf.addCharSequence(info_->data_, msize+1);
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            buf >> info_->usedsz_;
            buf.getFixedCharSequence(info_->data_, msize+1);
            info_->data_[info_->usedsz_] = '\0';
        }

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            buf << info_->usedsz_;
            buf.addCharSequence(info_->data_, msize+1);
        }

        /// Serialize (binary)
        /// @param buf byte buffer to use
        void serialize(NativeByteBuffer & buf) const
        {
            buf << info_->usedsz_;
            buf.addCharSequence(info_->data_, msize+1);
        }

        /// Serialize (binary)
        /// @param buf byte buffer to use
        void serialize(NetworkByteBuffer & buf) const
        {
            buf << info_->usedsz_;
            buf.addCharSequence(info_->data_, msize+1);
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            buf >> info_->usedsz_;
            memcpy(info_->data_, buf.getFixedCharSequence(msize+1), msize+1);
            info_->data_[info_->usedsz_] = '\0';
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(NativeByteBuffer & buf)
        {
            buf >> info_->usedsz_;
            memcpy(info_->data_, buf.getFixedCharSequence(msize+1), msize+1);
            info_->data_[info_->usedsz_] = '\0';
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(NetworkByteBuffer & buf)
        {
            buf >> info_->usedsz_;
            memcpy(info_->data_, buf.getFixedCharSequence(msize+1), msize+1);
            info_->data_[info_->usedsz_] = '\0';
        }

        /// Serialize (character)
        /// @param ostr output stream
        void serialize(std::ostream & ostr) const
            { ostr << SPL::rstring(info_->data_); }

        /// Deserialize (character)
        /// @param istr input stream
        void deserialize(std::istream & istr)
        {
            SPL::rstring temp;
            istr >> temp;
            if (!istr) {
                return;
            }
            this->operator=(temp);
        }

        size_t hashCode() const
        {
            const std::collate<char> & c =
                std::use_facet<std::collate<char> >(std::locale::classic());
            return c.hash(info_->data_, info_->data_ + info_->usedsz_);
        }

        size_t getSerializedSize() const
            { return sizeof(Data); }

    private:
        void initialize()
        {
            info_ = new Data;
            info_->usedsz_ = 0;
            info_->data_[0] = '\0';
            deallocate_ = true;
        }

        char * setData (char *dataIn)
            { char * tmp = info_->data_; info_->data_ = dataIn; return tmp; }

        int32_t setUsedSize (int32_t sizeIn)
            { int32_t tmp = info_->usedsz_; info_->usedsz_ = sizeIn; return tmp; }

        inline bstring<msize> & append(const char * ot, int32_t pos, size_type n)
        {
            size_type tgt_pos = info_->usedsz_;
            ot = ot + pos;
            // limit copy to remaining space
            if ((tgt_pos + n) > msize) {
                n = msize - tgt_pos;
            }
            // perhaps we should not do anything if appending zero bytes
            if (n == 0) {
                return *this;
            }
            memcpy(info_->data_+tgt_pos, ot, n);
            info_->data_[tgt_pos+n] = '\0';
            info_->usedsz_ = tgt_pos+n;
            return *this;
        }

        inline bstring<msize> & assign(const char * ot, int32_t pos, size_type n)
        {
            this->clear();
            return this->append(ot, pos, n);
        }

    private:
        struct Data {
            size_type usedsz_;
            char data_[msize+1];
        };
        Data *info_;
        bool deallocate_;
        template <int32_t> friend class bstring;
    };

    template <int32_t size>
    inline std::ostream & operator<<(std::ostream & ostr, const bstring<size> & x)
        { x.serialize(ostr); return ostr; }

    template <int32_t size>
    inline std::istream & operator>>(std::istream & istr, bstring<size> & x)
        { x.deserialize(istr); return istr; }

    template <int32_t size>
    inline VirtualByteBuffer & operator<<(VirtualByteBuffer & buf, const bstring<size> & x)
        { x.serialize(buf); return buf; }

    template <int32_t size>
    inline VirtualByteBuffer & operator>>(VirtualByteBuffer & buf, bstring<size> & x)
        { x.deserialize(buf); return buf; }

    template <class S, int32_t size>
    inline ByteBuffer<S> & operator<<(ByteBuffer<S> & buf, const bstring<size> & x)
        { x.serialize(buf); return buf; }

    template <int32_t size>
    inline NetworkByteBuffer & operator<<(NetworkByteBuffer & buf, const bstring<size> & x)
        { x.serialize(buf); return buf; }

    template <int32_t size>
    inline NativeByteBuffer & operator<<(NativeByteBuffer & buf, const bstring<size> & x)
        { x.serialize(buf); return buf; }

    template <class S, int32_t size>
    inline ByteBuffer<S> & operator>>(ByteBuffer<S> & buf, bstring<size> & x)
        { x.deserialize(buf); return buf; }

    template <int32_t size>
    inline NetworkByteBuffer & operator>>(NetworkByteBuffer & buf, bstring<size> & x)
        { x.deserialize(buf); return buf; }

    template <int32_t size>
    inline NativeByteBuffer & operator>>(NativeByteBuffer & buf, bstring<size> & x)
        { x.deserialize(buf); return buf; }

    /// Serialize a bstring in unquoted form
    /// @param stm the output stream object
    /// @param str the string to serialize
    /// @return the output stream object
    template <class Stream, int32_t size>
    inline Stream & serializeUnquoted(Stream & stm, SPL::bstring<size> const & str)
    {
        stm << str.c_str();
        return stm;
    }
};

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace std {
  template <int32_t sz>
  inline void swap(SPL::bstring<sz> & lhs, SPL::bstring<sz> & rhs)
    { lhs.swap(rhs); }

  template <int32_t lsz, int32_t rsz>
  inline void swap(SPL::bstring<lsz> & lhs, SPL::bstring<rsz> & rhs)
    { lhs.swap(rhs); }
}

// Hashability requirements
namespace std {
    namespace tr1 {
        template<int32_t n>
        struct hash<SPL::bstring<n> > {
            inline size_t operator()(const SPL::bstring<n> & s) const
            { return s.hashCode(); }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_BSTRING_H */
