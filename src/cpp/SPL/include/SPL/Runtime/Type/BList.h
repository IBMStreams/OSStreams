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

#ifndef SPL_RUNTIME_TYPE_BLIST_H
#define SPL_RUNTIME_TYPE_BLIST_H

/*!
 * \file BList.h \brief Definitions of the SPL::BList and SPL::blist classes.
 */


#include <SPL/Runtime/Type/BaseList.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Type/TypeTMPHelper.h>
#include <SPL/Runtime/Type/TypeHelper.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    // Forward declaration of list for ctor
    template <class T>
    class list;

    template <class TargetType, class SourceType> class spl_cast;

    /// \brief Class that defines a reflective interface for the bounded list type.
    class BList : public List
    {
    public:
        /// Get the bounded size of the list
        /// @return bounded size of the list
        virtual uint32_t getBoundedSize() const = 0;

        /// Get the number of used elements
        /// @return number of used elements
        virtual uint32_t getUsedSize() const = 0;

        /// Assign from another bounded list
        /// @param ov other list to assign from
        /// @return self
        const BList & operator =(const BList & ov)
        {
            assignFrom(ov); return *this;
        }
    };

#ifndef DOXYGEN_SKIP_FOR_USERS
    // specializations for common lists
    template<class T, class BufferType>
    struct ListSerializer
    {
        inline static void serialize(ByteBuffer<BufferType>& buf, const T *data, uint32_t numElems)
        {
            for (uint32_t i = 0; i < numElems; ++i) {
                buf << data[i];
            }
        }

        inline static void serialize(ByteBuffer<BufferType>& buf, const std::vector<T>& data,
                                     uint32_t numElems)
        {
            for (uint32_t i = 0; i < numElems; ++i) {
                buf << data[i];
            }
        }

        inline static void deserialize (ByteBuffer<BufferType>& buf, T *data, uint32_t numElems)
        {
            for (uint32_t i = 0; i < numElems; ++i) {
                buf >> data[i];
            }
        }

        inline static void deserialize (ByteBuffer<BufferType>& buf, std::vector<T>& data,
                                        uint32_t numElems)
        {
             data.clear();
             data.reserve(numElems);
             T x;
             for(uint32_t i = 0; i<numElems; ++i) {
                buf >> x;
                data.push_back(x);
             }
        }
    };

#define BLIST_SPECIALIZATION(TYPE) \
    template<>                                                                                                     \
    inline void ListSerializer<SPL::TYPE,NativeByteBuffer>::serialize(ByteBuffer<NativeByteBuffer> & buf,          \
            const SPL::TYPE *data, uint32_t numElems)                                                              \
    {                                                                                                              \
        buf.addCharSequence(reinterpret_cast<const char *>(data), sizeof (SPL::TYPE) * numElems);                  \
    }                                                                                                              \
                                                                                                                   \
    template<>                                                                                                     \
    inline void ListSerializer<SPL::TYPE,NativeByteBuffer>::serialize(ByteBuffer<NativeByteBuffer> & buf,          \
            const std::vector<SPL::TYPE>& data, uint32_t numElems)                                                 \
    {                                                                                                              \
        buf.addCharSequence(reinterpret_cast<const char *>(&*data.begin()), sizeof (SPL::TYPE) * numElems);        \
    }                                                                                                              \
                                                                                                                   \
    template <>                                                                                                    \
    inline void ListSerializer<SPL::TYPE, NativeByteBuffer>::deserialize (ByteBuffer<NativeByteBuffer> & buf,      \
            SPL::TYPE *data, uint32_t numElems)                                                                    \
    {                                                                                                              \
        const char *d = buf.getFixedCharSequence(sizeof (SPL::TYPE) * numElems);                                   \
        memcpy (reinterpret_cast<char *>(data), reinterpret_cast<const char *>(d), sizeof (SPL::TYPE) * numElems); \
    }                                                                                                              \
                                                                                                                   \
    template <>                                                                                                    \
    inline void ListSerializer<SPL::TYPE, NativeByteBuffer>::deserialize (ByteBuffer<NativeByteBuffer> & buf,      \
            std::vector<SPL::TYPE>& data, uint32_t numElems)                                                       \
    {                                                                                                              \
        data.clear();                                                                                              \
        data.reserve(numElems);                                                                                    \
        SPL::TYPE *d = reinterpret_cast<SPL::TYPE*>(buf.getFixedCharSequence(sizeof (SPL::TYPE) * numElems));      \
        data.assign (d, d+numElems);                                                                               \
    }

BLIST_SPECIALIZATION(int8)
BLIST_SPECIALIZATION(int16)
BLIST_SPECIALIZATION(int32)
BLIST_SPECIALIZATION(int64)
BLIST_SPECIALIZATION(uint8)
BLIST_SPECIALIZATION(uint16)
BLIST_SPECIALIZATION(uint32)
BLIST_SPECIALIZATION(uint64)
BLIST_SPECIALIZATION(float32)
BLIST_SPECIALIZATION(float64)
BLIST_SPECIALIZATION(boolean)
#undef BLIST_SPECIALIZATION

#endif /* DOXYGEN_SKIP_FOR_USERS */


    /// \brief Class that represents bounded size list type.
    template <class T, int32_t msize>
    class blist : public BList
    {
    public:
        typedef T value_type; //<! value type
        typedef typename TMP::SizeType<msize>::value size_type; //<! size type
        typedef ptrdiff_t difference_type; //<! iterator difference type type
        typedef T * iterator; //<! iterator type
        typedef T & reference; //<! element reference type
        typedef T const * const_iterator; //<! constant iterator type
        typedef T const & const_reference; //<! constant element reference type
        typedef std::reverse_iterator<iterator> reverse_iterator; //<! reverse iterator type
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //<! constant reverse iterator type

        /// Default constructor
        ///
        explicit blist()
        {
            this->initialize();
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Constructor that  use external memory
        /// @param buffer memory to use (must be a size_type followed by array of value_types)
        /// @param initializeIn whether blist must be initialized or not
        explicit blist(void * buffer, bool initializeIn)
        {
            usedsz_ = reinterpret_cast<size_type *>(buffer);
            data_ = reinterpret_cast<value_type *>(((char *)buffer) + sizeof(size_type));
            deallocate_ = false;
            if (initializeIn) {
                *usedsz_ = 0;
            }
        }

        /// Adopt external buffer (similar to external memory constructor)
        /// @param buffer external buffer
        void adopt(void *buffer)
        {
            if (deallocate_) {
                delete [] data_;
                delete usedsz_;
                deallocate_ = false;
            }
            usedsz_ = reinterpret_cast<size_type *>(buffer);
            data_ = reinterpret_cast<value_type *>(((char *)buffer) + sizeof(size_type));
        }
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Destructor
        ///
        ~blist()
        {
            if(deallocate_) {
                delete [] data_;
                delete usedsz_;
            }
        }

        /// Construct using a value
        /// @param n number of elements
        /// @param v value to use
        explicit blist(size_type n, const T & v = T())
        {
            this->initialize(); assign(n, v);
        }

        /// Construct from a sequence
        /// @param first begin iterator, inclusive
        /// @param last end iterator, exclusive
        template <class InputIterator>
        blist(InputIterator first, InputIterator last)
        {
            this->initialize(); assign(first, last);
        }

        /// Construct blist from an assignable list
        /// @param o list to construct from
        template <class T2>
        blist(const std::vector<T2> & o)
        {
            this->initialize();
            uint32_t count = o.size();
            for (uint32_t i = 0; i < count; i++) {
                this->push_back(spl_cast<T, T2>::cast(o[i]));
            }
        }

        /// Construct from an assignable blist
        /// @param o list to construct from
        template <class T2, int32_t msize2>
        blist(const blist<T2,msize2> & o)
        {
            this->initialize();
            uint32_t count = o.size();
            for (uint32_t i = 0; i < count; i++) {
                this->push_back(spl_cast<T, T2>::cast(o[i]));
            }
        }

        /// Copy constructor
        /// @param ot bounded size list to construct from
        blist(const blist<T,msize> & ot)
        {
            this->initialize(); this->operator =(ot);
        }

        /// Copy constructor
        /// @param ot bounded size list to construct from
        template <int32_t osize>
        blist(const blist<T,osize> & ot)
        {
            this->initialize(); this->operator =<osize>(ot);
        }

        /// Construct from an STL vector
        /// @param ot STL vector to construct from
        blist(const std::vector<T> & ot)
        {
            this->initialize(); this->operator =(ot);
        }

        /// Construct from a reflective bounded size list
        /// @param o reflective bounded size list to construct from
        blist(const BList & o)
        {
            this->initialize(); assignFrom(o);
        }

        /// Construct from a value handle
        /// @param o value handle to construct from
        blist(const ConstValueHandle & o)
        {
            this->initialize(); BList const & x = o; assignFrom(x);
        }

        /// Assignment operator (from another bounded size list)
        /// @param ot bounded size list to assign from
        /// @return self
        const blist & operator =(const blist<T,msize> & ot)
        {
            return this->operator =<msize>(ot);
        }

        /// Assignment operator (from another bounded size list)
        /// @param ot bounded size list to assign from
        /// @return self
        template <int32_t osize>
        const blist & operator =(const blist<T,osize> & ot)
        {
            if (osize == msize && this == reinterpret_cast<blist<T, msize> const*>(&ot)) {
                return *this;
            }
            int32_t ousz = ot.getUsedSize();
            if (ousz > msize) {
                ousz = msize;
            }
            for (int32_t i = 0; i < ousz; ++i) {
                data_[i] = ot[i];
            }
            for (int32_t i = ousz + 1; i < *usedsz_; i++) {
                data_[i] = T();
            }
            *usedsz_ = ousz;
            return *this;
        }

        /// Assignment operator (from an STL vector)
        /// @param ot STL vector to assign from
        /// @return self
        const blist & operator =(const std::vector<T> & ot)
        {
            int32_t ousz = ot.size();
            if (ousz > msize) {
                ousz = msize;
            }
            for (int32_t i = 0; i < ousz; ++i) {
                data_[i] = ot[i];
            }
            for (int32_t i = ousz + 1; i < *usedsz_; i++) {
                data_[i] = T();
            }
            *usedsz_ = ousz;
            return *this;
        }

        /// Equality comparison
        /// @param ot bounded size list to compare with
        /// @return true if the contained elements are the same, false otherwise
        bool operator ==(const blist<T,msize> & ot) const
        {
            return this->operator ==<msize>(ot);
        }

        /// Equality comparison
        /// @param ot bounded size list to compare with
        /// @return true if the contained elements are the same, false otherwise
        template <int32_t osize>
        bool operator ==(const blist<T,osize> & ot) const
        {
            if (getUsedSize() != ot.getUsedSize()) {
                return false;
            }
            for (size_type i = 0; i < *usedsz_; ++i) {
                if (data_[i] != ot[i]) {
                    return false;
                }
            }
            return true;
        }

        /// Non-equality comparison
        /// @param ot bounded size list to compare with
        /// @return true if the contained elements are not the same, false otherwise
        bool operator !=(const blist<T,msize> & ot) const
        {
            return !(this->operator ==(ot));
        }

        /// Non-equality comparison
        /// @param ot bounded size list to compare with
        /// @return true if the contained elements are not the same, false otherwise
        template <int32_t osize>
        bool operator !=(const blist<T,osize> & ot) const
        {
            return !(this->operator ==<osize>(ot));
        }

        /// Get a begin iterator
        /// @return begin iterator
        iterator begin()
        {
            return data_;
        }

        /// Get a begin const iterator
        /// @return begin const iterator
        const_iterator begin() const
        {
            return data_;
        }

        /// Get an end iterator
        /// @return end iterator
        iterator end()
        {
            return data_+*usedsz_;
        }

        /// Get an end const iterator
        /// @return end const iterator
        const_iterator end() const
        {
            return data_+*usedsz_;
        }

        /// Get a reverse begin iterator
        /// @return reverse begin iterator
        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }

        /// Get a reverse begin const iterator
        /// @return reverse begin const iterator
        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(end());
        }

        /// Get a reverse end iterator
        /// @return reverse end iterator
        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }

        /// Get a reverse end const iterator
        /// @return reverse end const iterator
        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(begin());
        }

        /// Get the bounded size of the list
        /// @return bounded size of the list
        int32_t max_size() const
        {
            return msize;
        }

        /// Get the number of used elements
        /// @return number of used elements
        size_type size() const
        {
            return *usedsz_;
        }

        /// Resize the list
        /// @param s new size
        void resize(size_type s) const
        {
	  *usedsz_ = std::min((int32_t)s, msize);
        }

        /// Remove all elements
        ///
        void clear()
        {
            for (int32_t i = 0; i < *usedsz_; i++) {
                data_[i] = T();
            }
            *usedsz_ = 0;
        }

        /// Check if empty
        /// @return true if empty, false otherwise
        bool empty() const
        {
            return (*usedsz_==0);
        }

        /// Element access (read-only)
        /// @param idx element index
        /// @return element at index idx
        const_reference at(size_type idx) const
        {
            if (idx >= msize) {
                indexOutOfBound(idx, msize);
            }
            return data_[idx];
        }

        /// Element access (read-write)
        /// @param idx element index
        /// @return element at index idx
        reference at(size_type idx)
        {
            if (idx >= msize) {
                indexOutOfBound(idx, msize);
            }
            if (idx >= *usedsz_) {
                *usedsz_ = idx + 1;
            }
            return data_[idx];
        }

        /// Element access (read-only)
        /// @param idx element index
        /// @return element at index idx
        const_reference operator [](const size_type idx) const
        {
            return data_[idx];
        }

        /// Element access (read-write)
        /// @param idx element index
        /// @return element at index idx
        reference operator [](const size_type idx)
        {
            if (idx >= *usedsz_) {
                *usedsz_ = idx + 1;
            }
            return data_[idx];
        }

        /// Get the element at the front of the list
        /// @return element at the front of the list
        reference front()
        {
            if (empty()) {
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                                    SPL_APPLICATION_RUNTIME_INVALID_LIST_FRONT, SPL_FUNC_DBG);
            }
            return data_[0];
        }

        /// Get the element at the front of the list
        /// @return element at the front of the list
        const_reference front() const
        {
            if (empty()) {
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                                    SPL_APPLICATION_RUNTIME_INVALID_LIST_FRONT, SPL_FUNC_DBG);
            }
            return data_[0];
        }

        /// Get the element at the back of the list
        /// @return element at the back of the list
        reference back()
        {
            if (empty()) {
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                                    SPL_APPLICATION_RUNTIME_INVALID_LIST_BACK, SPL_FUNC_DBG);
            }
            return data_[*usedsz_-1];
        }

        /// Get the element at the back of the list
        /// @return element at the back of the list
        const_reference back() const
        {
            if (empty()) {
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                                    SPL_APPLICATION_RUNTIME_INVALID_LIST_BACK, SPL_FUNC_DBG);
            }
            return data_[*usedsz_-1];
        }

        /// Assign from a sequence
        /// @param first begin iterator, inclusive
        /// @param last end iterator, exclusive
        template <class InputIterator>
        void assign(InputIterator first, InputIterator last)
        {
            size_type ousedSize = *usedsz_;
            *usedsz_ = 0;
            for(InputIterator it = first; it!=last; ++it) {
                if (*usedsz_ >= msize) {
                    break;
                }
                data_[(*usedsz_)++] = *it;
            }
            for (int32_t i = *usedsz_; i < ousedSize; i++) {
                data_[i] = T();
            }
        }

        /// Assign using a value
        /// @param n number of elements
        /// @param u value to use
        void assign(size_type n, const T & u)
        {
            size_type ousedSize = *usedsz_;
            *usedsz_ = std::min((int32_t)n, msize);
            for (size_type i = 0; i < *usedsz_; ++i) {
                data_[i] = u;
            }
            for (size_type i = *usedsz_; i < ousedSize; i++) {
                data_[i] = T();
            }
        }

        /// Append to list
        /// @param v   Value to be added
        /// @return *this
        blist<T,msize> & add(const T & v)
        {
            push_back(v); return *this;
        }

        /// Push a given element to the end of the list
        /// @param t element to push back
        void push_back(const T & t)
        {
            this->insert(this->end(), t);
        }

        /// Pop the last element from the end of the list
        ///
        void pop_back()
        {
            this->erase(this->end()-1);
        }

        /// Insert a given element at a given position
        /// @param pos position of the element
        /// @param x element to insert
        /// @return iterator that points to the newly inserted element
        iterator insert(iterator pos, const T & x)
        {
            size_type idx = 1; // otherwise it picks the templated insert
            this->insert(pos, idx, x);
            return pos;
        }

        /// Insert a given element at a given position
        /// @param pos position of insert
        /// @param n number of times to insert
        /// @param x element to insert
        void insert(iterator pos, size_type n, const T & x)
        {
            difference_type idx_d = pos - data_;
            if (idx_d >= msize) {
                return;
            }
            T y(x); // in case x is in this list
            size_type idx = static_cast<size_type>(idx_d);
            size_type nup = (idx>=*usedsz_) ? 0 : (*usedsz_-idx);
            T * tmp = new T[size_t(nup)];
            for (size_type i = 0; i < nup; ++i) {
                tmp[i] = data_[idx + i];
            }
            *usedsz_ = idx;
            for(size_type i = 0; i<n; ++i) {
                if (*usedsz_ >= msize) {
                    break;
                }
                data_[(*usedsz_)++] = y;
            }
            for(size_type i = 0; i<nup; ++i) {
                if (*usedsz_ >= msize) {
                    break;
                }
                data_[(*usedsz_)++] = tmp[i];
            }
            delete [] tmp;
        }

        /// Insert a given sequence of elements at a given position
        /// @param pos position of insert
        /// @param first begin iterator for the insert sequence, inclusive
        /// @param last end iterator for the insert sequence, exclusive
        template <class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last)
        {
            difference_type idx_d = pos - data_;
            if (idx_d >= msize) {
                return;
            }
            size_type idx = static_cast<size_type>(idx_d);
            size_type nup = (idx>=*usedsz_) ? 0 : (*usedsz_-idx);
            T * tmp = new T[size_t(nup)];
            for (size_type i = 0; i < nup; ++i) {
                tmp[i] = data_[idx + i];
            }
            *usedsz_ = idx;
            for(InputIterator it = first; it!=last; ++it) {
                if (*usedsz_ >= msize) {
                    break;
                }
                data_[(*usedsz_)++] = *it;
            }
            for(size_type i = 0; i<nup; ++i) {
                if (*usedsz_ >= msize) {
                    break;
                }
                data_[(*usedsz_)++] = tmp[i];
            }
            delete [] tmp;
        }

        /// Remove an element at a given position
        /// @param pos iterator pointing to the element
        /// @return iterator pointing to the element following the last element deleted
        iterator erase(iterator pos)
        {
            return erase(pos, pos+1);
        }

        /// Remove a sequence of elements
        /// @param first begin iterator for the remove sequence, inclusive
        /// @param last end iterator for the remove sequence, exclusive
        /// @return iterator pointing to the element following the last element deleted
        iterator erase(iterator first, iterator last)
        {
            difference_type sidx_d = first - data_;
            difference_type eidx_d = last - data_;
            if (sidx_d >= eidx_d || sidx_d >= msize) {
                return last;
            }
            size_type sidx = static_cast<size_type>(sidx_d);
            size_type eidx = static_cast<size_type>(eidx_d);
            if (sidx >= *usedsz_) {
                return last;
            }
            if (eidx > *usedsz_) {
                eidx = *usedsz_;
            }
            for (size_type i = 0, iu = *usedsz_ - eidx; i < iu; ++i) {
                data_[sidx + i] = data_[eidx + i];
            }
            *usedsz_ -= (eidx-sidx);
            for (size_type i = 0; i < eidx - sidx; i++) {
                data_[*usedsz_ + i] = T();
            }
            return data_+eidx;
        }

        /// Get the C array representation
        /// @return C array representation
        T const * getData() const
        {
            return data_;
        }

        /// Swap
        /// @param ot bounded size list to swap with
        template <int32_t osize>
        void swap(blist<T,osize> & ot)
        {
            if(osize == msize && deallocate_ && ot.deallocate_) {
                // Just swap pointers, size used and both controlling memory
                value_type* dTmp = data_;
                size_type *sTmp = usedsz_;
                data_ = ot.data_;
                usedsz_ = reinterpret_cast<size_type *>(ot.usedsz_);
                ot.data_ = dTmp;
                ot.usedsz_ = reinterpret_cast<typename blist<T,osize>::size_type *>(sTmp);
            } else {
                int32_t ousedsz = ot.getUsedSize();
                if (ousedsz > msize) {
                    ousedsz = msize;
                }
                T * tmp = new T[ousedsz];
                for (int32_t i = 0; i < ousedsz; ++i) {
                    tmp[i] = ot[i];
                }
                ot.template operator=<msize>(* this);
                for (int32_t i = 0; i < ousedsz; ++i) {
                    data_[i] = tmp[i];
                }
                for (int32_t i = ousedsz + 1; i < *usedsz_; i++) {
                    data_[i] = T();
                }
                *usedsz_ = ousedsz;
                delete [] tmp;
            }
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            buf << *usedsz_;
            for (size_type i = 0; i < msize; ++i) {
                buf << data_[i];
            }
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            buf >> *usedsz_;
            for (size_type i = 0; i < msize; ++i) {
                buf >> data_[i];
            }
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            buf << *usedsz_;
            ListSerializer<T,BufferType>::serialize (buf, data_, msize);
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            buf >> *usedsz_;
            ListSerializer<T,BufferType>::deserialize (buf, data_, msize);
        }

        /// Serialize (character)
        /// @param ostr output stream
        void serialize(std::ostream & ostr) const
        {
            ostr << "[";
            for(size_type i = 0,iu = *usedsz_; i<iu; ++i) {
                if (i != 0) {
                    ostr << ",";
                }
                ostr << this->operator [](i);
            }
            ostr << "]";
        }

        /// Serialize (character), with maximum precision for floats
        /// @param ostr output stream
        void serializeWithPrecision(std::ostream & ostr) const
        {
            ostr << "[";
            for(size_type i = 0,iu = *usedsz_; i<iu; ++i) {
                if (i != 0) {
                    ostr << ",";
                }
                SPL::serializeWithPrecision(ostr, this->operator [](i));
            }
            ostr << "]";
        }

        /// Deserialize (character)
        /// @param istr input stream
        /// @param withSuffix if true then consume suffix, otherwise (false)
        /// assume no suffix is present
        void deserialize(std::istream & istr,bool withSuffix=false);

        /// Deserialize with Inf and Nan (character)
        /// @param istr input stream
        /// @param withSuffix if true then consume suffix, otherwise (false)
        /// assume no suffix is present
        void deserializeWithNanAndInfs(std::istream & istr,bool withSuffix=false);

        /// Get hash code
        /// @return hash code
        size_t hashCode() const
        {
            size_t r = 17;
            std::tr1::hash<T> hs;
            for (size_type i = 0; i < *usedsz_; ++i) {
                r = 37 * r + hs(this->operator[](i));
            }
            return r;
        }

        /*! @name Reflective interfaces
         */
        //@{
        virtual uint32_t getBoundedSize() const
        {
            return msize;
        }

        virtual uint32_t getUsedSize() const
        {
            return *usedsz_;
        }

        virtual Meta::Type getElementMetaType() const
        {
            return Meta::Type::typeOf<T>();
        }

        virtual ValueHandle createElement() const
        {
            return ValueHandle(*(new T()));
        }

        virtual bool equals(const Collection & ot) const
        {
            BList const * o = dynamic_cast<BList const *>(&ot);
            if (o == NULL) {
                return false;
            }
            if (getUsedSize() != o->getUsedSize()) {
                return false;
            }
            for (size_type i = 0; i < *usedsz_; ++i) {
                if (!getElement(i).equals(o->getElement(i))) {
                    return false;
                }
            }
            return true;
        }

        virtual void assignFrom(const Collection & ot)
        {
            BList const * o = dynamic_cast<BList const *>(&ot);
            if (o == NULL) {
                SPLTRACEMSGANDTHROW(
                  SPLRuntimeTypeMismatch, L_ERROR,
                  SPL_APPLICATION_RUNTIME_INVALID_BLIST_COLLECTION(typeid(T).name(), msize),
                  SPL_FUNC_DBG);
            }
            int32_t ousz = o->getUsedSize();
            if (ousz > msize) {
                ousz = msize;
            }
            for(int32_t i = 0; i<ousz; ++i) {
                T const & x = o->getElement(i);
                data_[i] = x;
            }
            *usedsz_ = ousz;
        }

        virtual void swapWith(Collection & ot)
        {
            if (!SPL::equals(typeid(*this), typeid(ot))) {
                SPLTRACEMSGANDTHROW(
                  SPLRuntimeTypeMismatch, L_ERROR,
                  SPL_APPLICATION_RUNTIME_INVALID_BLIST_COLLECTION(typeid(T).name(), msize),
                  SPL_FUNC_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            blist<T,msize> & o = static_cast<blist<T,msize> &>(ot);
            this->swap<msize>(o);
        }

        virtual Collection * clone() const
        {
            return new blist<T,msize>(*this);
        }

        virtual int32_t getSize() const
        {
            return *usedsz_;
        };

        virtual ValueHandle getElement(const size_t & i)
        {
            return ValueHandle(this->operator [](i));
        }

        virtual ConstValueHandle getElement(const size_t & i) const
        {
            return ConstValueHandle(((blist<T,msize> const *) this)->operator [](i));
        }

        virtual ValueHandle getElement(const ConstListIterator & it)
        {
            return ValueHandle(this->operator [](it.index()));
        }

        virtual ConstValueHandle getElement(const ConstListIterator & it) const
        {
            return ConstValueHandle(((blist<T,msize> const *) this)->operator [](it.index()));
        }

        virtual void insertElement(const ConstListIterator & it, const ConstValueHandle & v)
        {
            const T & x = v; this->insert(this->begin()+it.index(),x);
        }

        virtual void removeElement(const ConstListIterator & it)
        {
            this->erase(this->begin()+it.index());
        }

        virtual void removeAllElements()
        {
            this->clear();
        }

        virtual void pushBack(const ConstValueHandle & v)
        {
            const T & x = v; this->push_back(x);
        }

        virtual void popBack()
        {
            this->pop_back();
        }

        virtual size_t getSerializedSize() const
        {
            size_t sz = sizeof(size_type);
            for (size_type i = 0; i < msize; ++i) {
                sz += SPL::getSerializedSize(data_[i]);
            }
            return sz;
        }
        //@}

    private:
        void initialize()
        {
            usedsz_ = new size_type;
            *usedsz_ = 0;
            data_ = new value_type[msize];
            deallocate_ = true;
        }

        int32_t setUsedSize(int32_t sizeIn)
        {
            int32_t tmp = *usedsz_; *usedsz_ = sizeIn; return tmp;
        }

        size_type *usedsz_;
        value_type *data_;
        bool deallocate_;
        template <class,int32_t>
        friend class blist;
    };

    template <class T, int32_t msize>
    void blist<T,msize>::deserialize(std::istream & istr,bool withSuffix)
    {
        *usedsz_ = 0;
        char c;
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!='[') {
            istr.setstate(std::ios_base::failbit);
            return;
        }
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!=']') {
            istr.putback(c);
            T v;
            if (withSuffix) {
                SPL::deserializeWithSuffix(istr,v);
            } else {
                istr >> v;
            }
            if (!istr) {
                return;
            }
            if (*usedsz_ < msize) {
                data_[(*usedsz_)++] = v;
            }
        } else {
            return;
        }
        while(true) {
            istr >> c;
            if (!istr) {
                return;
            }
            if (c == ']') {
                return;
            }
            if(c!=',') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            T v;
            if (withSuffix) {
                SPL::deserializeWithSuffix(istr,v);
            } else {
                istr >> v;
            }
            if (!istr) {
                return;
            }
            if (*usedsz_ < msize) {
                data_[(*usedsz_)++] = v;
            }
        }
    }

    template <class T, int32_t msize>
    void blist<T,msize>::deserializeWithNanAndInfs(std::istream & istr,bool withSuffix)
    {
        *usedsz_ = 0;
        char c;
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!='[') {
            istr.setstate(std::ios_base::failbit);
            return;
        }
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!=']') {
            istr.putback(c);
            T v;
            SPL::deserializeWithNanAndInfs(istr,v,withSuffix);
            if (!istr) {
                return;
            }
            if (*usedsz_ < msize) {
                data_[(*usedsz_)++] = v;
            }
        } else {
            return;
        }
        while(true) {
            istr >> c;
            if (!istr) {
                return;
            }
            if (c == ']') {
                return;
            }
            if(c!=',') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            T v;
            SPL::deserializeWithNanAndInfs(istr,v,withSuffix);
            if (!istr) {
                return;
            }
            if (*usedsz_ < msize) {
                data_[(*usedsz_)++] = v;
            }
        }
    }

    template <class T, int32_t size>
    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const blist<T,size> & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    template <class T, int32_t size>
    inline std::ostream & operator<<(std::ostream & ostr, const blist<T,size> & x)
    {
        x.serialize(ostr); return ostr;
    }

    template <class T, int32_t size>
    inline std::istream & operator>>(std::istream & istr, blist<T,size> & x)
    {
        x.deserialize(istr); return istr;
    }

    template <class T, int32_t size>
    inline void deserializeWithSuffix(std::istream & istr, blist<T,size> & x)
    {
        x.deserialize(istr,true);
    }

    template <class T, int32_t size>
    inline void deserializeWithNanAndInfs(std::istream & istr, blist<T,size> & x, bool withSuffix = false)
    {
        x.deserializeWithNanAndInfs(istr,withSuffix);
    }

    template <class S, class T, int32_t size>
    inline ByteBuffer<S> & operator<<(ByteBuffer<S> & buf, const blist<T,size> & x)
    {
        x.serialize(buf); return buf;
    }

    template <class T, int32_t size>
    inline NetworkByteBuffer& operator<<(NetworkByteBuffer & buf, const blist<T,size> & x)
    {
        x.serialize(buf); return buf;
    }

    template <class T, int32_t size>
    inline NativeByteBuffer& operator<<(NativeByteBuffer & buf, const blist<T,size> & x)
    {
        x.serialize(buf); return buf;
    }

    template <class S, class T, int32_t size>
    inline ByteBuffer<S> & operator>>(ByteBuffer<S> & buf, blist<T,size> & x)
    {
        x.deserialize(buf); return buf;
    }

    template <class T, int32_t size>
    inline NetworkByteBuffer & operator>>(NetworkByteBuffer & buf, blist<T,size> & x)
    {
        x.deserialize(buf); return buf;
    }

    template <class T, int32_t size>
    inline NativeByteBuffer & operator>>(NativeByteBuffer & buf, blist<T,size> & x)
    {
        x.deserialize(buf); return buf;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class T, int32_t size>
    inline VirtualByteBuffer & operator<<(VirtualByteBuffer & buf, const blist<T,size> & x)
    {
        x.serialize(buf); return buf;
    }

    template <class T, int32_t size>
    inline VirtualByteBuffer & operator>>(VirtualByteBuffer & buf, blist<T,size> & x)
    {
        x.deserialize(buf); return buf;
    }

    template<class T, int32_t size>
    struct has_bounded_set_or_map<blist<T,size> >
    {
        enum { value = has_bounded_set_or_map<T>::value };
    };

    template<class T, int32_t size>
    void normalizeBoundedSetsAndMaps(blist<T,size> & v)
    {
        if (has_bounded_set_or_map<T>::value) {
            for (typename blist<T, size>::iterator it = v.begin(); it != v.end(); ++it) {
                normalizeBoundedSetsAndMaps(*it);
            }
        }
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */
};

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace std
{
    template <class T, int32_t sz>
    inline void swap(SPL::blist<T,sz> & lhs, SPL::blist<T,sz> & rhs)
    {
        lhs.swap(rhs);
    }

    template <class T, int32_t lsz, int32_t rsz>
    inline void swap(SPL::blist<T,lsz> & lhs, SPL::blist<T,rsz> & rhs)
    {
        lhs.swap(rhs);
    }
}

// Hashability requirements
namespace std {
    namespace tr1 {
        template <class T, int32_t n>
        struct hash<SPL::blist<T,n> >
        {
            inline size_t operator ()(const SPL::blist<T,n> & l) const
            {
                return l.hashCode();
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */
POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_BLIST_H */
