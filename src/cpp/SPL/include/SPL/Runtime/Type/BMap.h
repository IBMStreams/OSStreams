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

#ifndef SPL_RUNTIME_TYPE_BMAP_H
#define SPL_RUNTIME_TYPE_BMAP_H

/*!
 * \file BMap.h \brief Definitions of the SPL::BMap and SPL::bmap classes.
 */

#include <SPL/Runtime/Type/BaseMap.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Type/TypeTMPHelper.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    // Forward declaration of map for ctor
    template <class K, class V>
    class map;

    template <class TargetType, class SourceType> class spl_cast;

    /// \brief Class that defines a reflective interface for the bounded map type.
    class BMap : public Map
    {
    public:
        /// Get the bounded size of the map
        /// @return bounded size of the map
        virtual uint32_t getBoundedSize() const = 0;

        /// Get the number of used elements
        /// @return number of used elements
        virtual uint32_t getUsedSize() const = 0;

        /// Assign from another BMap
        /// @param ov other list to assign from
        /// @return this
        const BMap & operator =(const BMap & ov)
        {
            assignFrom(ov); return *this;
        }
    };

    template <class K, class V, int32_t msize>
    class bmap;
    template <class K, class V, int32_t msize>
    class const_bmap_iterator;

#ifndef DOXYGEN_SKIP_FOR_USERS
    /// \brief Class that serves as an iterator for reflective bounded map iteration.
    template <class K, class V, int32_t msize>
    struct bmap_iterator
    {
        /// Default constructor
        ///
        bmap_iterator() {}

        /// Copy constructor
        /// @param ot iterator to construct from
        bmap_iterator(const bmap_iterator & ot)
            : map_(ot.map_), index_(ot.index_) {}

        /// Constructor
        /// @param map bounded set
        /// @param index index to start from
        bmap_iterator(bmap<K,V,msize> & map, int32_t index);

        /// Post increment operator
        /// @return iterator before increment
        bmap_iterator operator ++(int);

        /// Pre increment operator
        /// @return iterator after increment
        bmap_iterator & operator ++();

        /// Dereference operator
        /// @return the current element
        std::pair<K,V> & operator *() const;

        /// Pointer access operator
        /// @return pointer to the current element
        std::pair<K,V> * operator ->() const;

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator ==(const bmap_iterator & ot) const
        {
            return map_==ot.map_ && index_==ot.index_;
        }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator !=(const bmap_iterator & ot) const
        {
            return !(*this==ot);
        }
    private:
        bmap<K,V,msize> * map_;
        int32_t index_;
        friend class const_bmap_iterator<K,V,msize>;
    };


    /// \brief Class that serves as a constant iterator for reflective bounded set iteration.
    template <class K, class V, int32_t msize>
    struct const_bmap_iterator
    {
        /// Default constructor
        ///
        const_bmap_iterator() {}

        /// Copy constructor
        /// @param ot iterator to construct from
        const_bmap_iterator(const const_bmap_iterator & ot)
            : map_(ot.map_), index_(ot.index_) {}

        /// Copy constructor
        /// @param ot iterator to construct from
        const_bmap_iterator(const bmap_iterator<K,V,msize> & ot)
            : map_(ot.map_), index_(ot.index_) {}

        /// Constructor
        /// @param map bounded map
        /// @param index index to start from
        const_bmap_iterator(const bmap<K,V,msize> & map, int32_t index);

        /// Post increment operator
        /// @return iterator before increment
        const_bmap_iterator operator ++(int);

        /// Pre increment operator
        /// @return iterator after increment
        const_bmap_iterator & operator ++();

        /// Dereference operator
        /// @return the current element
        const std::pair<K,V> & operator *() const;

        /// Pointer access operator
        /// @return pointer to the current element
        const std::pair<K,V> * operator ->() const;

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator ==(const const_bmap_iterator & ot) const
        {
            return map_==ot.map_ && index_==ot.index_;
        }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator !=(const const_bmap_iterator & ot) const
        {
            return !(*this==ot);
        }
    private:
        const bmap<K,V,msize> * map_;
        int32_t index_;
    };
#endif /*DOXYGEN_SKIP_FOR_USERS*/

    /// \brief Class that represents bounded size map type.
    template <class K, class V, int32_t msize>
    class bmap : public BMap
    {
    public:
        typedef K key_type; //!< key type
        typedef V data_type; //!< data type
        typedef std::pair<K,V> value_type; //!< value type
        typedef typename TMP::SizeType<msize>::value size_type; //!< size type
        typedef std::tr1::hash<K> hasher; //!< hasher type
        typedef std::pair<K,V> & reference; //!< element reference type
        typedef std::pair<K,V> const & const_reference; //!< element constant reference type
        typedef std::pair<K,V> * pointer; //!< element pointer type
        typedef std::pair<K,V> const * const_pointer; //!< element constant pointer type
        typedef bmap_iterator<K,V,msize> iterator; //!< iterator type
        typedef const_bmap_iterator<K,V,msize> const_iterator; //!< constant iterator type

        /// Empty constructor
        ///
        explicit bmap()
        {
            this->initialize();
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Constructor to use external memory
        /// @param buffer memory to use (must be a size_type followed by array of bytes, then
        /// array of value_types)
        /// @param initializeIn whether bmap must be initialized or not
        explicit bmap(void * buffer, bool initializeIn)
        {
            info_ = reinterpret_cast<Info *>(buffer);
            data_ = reinterpret_cast<value_type*>(((char *) buffer) + sizeof (Info));
            deallocate_ = false;
            if(initializeIn) {
                info_->usedsz_ = 0;
                memset(info_->used_, 0, msize);
            }
        }

        /// Adopt external buffer (similar to external memory constructor)
        /// @param buffer external buffer
        void adopt(void *buffer)
        {
            if(deallocate_) {
                delete info_;
                delete [] data_;
                deallocate_ = false;
            }
            info_ = reinterpret_cast<Info *>(buffer);
            data_ = reinterpret_cast<value_type*>(((char *) buffer) + sizeof (Info));
        }
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Construct from a sequence
        /// @param first begin iterator, inclusive
        /// @param last end iterator, exclusive
        template <class InputIterator>
        bmap(InputIterator first, InputIterator last)
        {
            this->initialize();
            for (InputIterator it = first; it != last; ++it) {
                insert(*it);
            }
        }

        /// Construct bmap from an assignable map
        /// @param o map to construct from
        template <class K2, class V2>
        bmap(const std::tr1::unordered_map<K2,V2> & o)
        {
            this->initialize();
            typename std::tr1::unordered_map<K2,V2>::const_iterator it;
            for (it = o.begin(); it != o.end(); it++) {
                this->insert(std::make_pair(spl_cast<K, K2>::cast(it->first),
                                            spl_cast<V, V2>::cast(it->second)));
            }
        }

        /// Construct from an assignable bmap
        /// @param o map to construct from
        template <class K2, class V2, uint32_t msize2>
        bmap(const bmap<K2,V2,msize2> & o)
        {
            this->initialize();
            typename bmap<K2,V2,msize2>::const_iterator it;
            for (it = o.begin(); it != o.end(); it++) {
                this->insert(std::make_pair(spl_cast<K, K2>::cast(it->first),
                                            spl_cast<V, V2>::cast(it->second)));
            }
        }

        /// Copy constructor
        /// @param ot bounded size map to construct from
        bmap(const bmap<K,V,msize> & ot)
        {
            this->initialize(); this->operator =(ot);
        }

        /// Copy constructor
        /// @param ot bounded size map to construct from
        template <int32_t osize>
        bmap(const bmap<K,V,osize> & ot)
        {
            this->initialize(); this->operator =<osize>(ot);
        }

        /// Construct from an std::tr1::unordered_map
        /// @param ot unordered map to construct from
        bmap(const std::tr1::unordered_map<K,V> & ot)
        {
            this->initialize(); this->operator =(ot);
        }

        /// Construct from a reflective bounded size map
        /// @param o reflective bounded size map to construct from
        bmap(const BMap & o)
        {
            this->initialize(); assignFrom(o);
        }

        /// Construct from a value handle
        /// @param o value handle to construct from
        bmap(const ConstValueHandle & o)
        {
            BMap const & x = o; this->initialize(); assignFrom(x);
        }

        /// Destructor
        ///
        ~bmap()
        {
            if(deallocate_) {
                delete info_;
                delete [] data_;
            }
        }

        /// Assignment operator (from another bounded size map)
        /// @param ot bounded size map to assign from
        /// @return self
        const bmap & operator =(const bmap<K,V,msize> & ot)
        {
            return this->operator =<msize>(ot);
        }

        /// Assignment operator (from another bounded size map)
        /// @param ot bounded size map to assign from
        /// @return self
        template <int32_t osize>
        const bmap & operator =(const bmap<K,V,osize> & ot)
        {
            if (osize == msize && this == reinterpret_cast<bmap<K, V, msize> const*>(&ot)) {
                return *this;
            }
            this->clear();
            typename bmap<K,V,osize>::const_iterator bit = ot.begin();
            typename bmap<K,V,osize>::const_iterator eit = ot.end();
            for (typename bmap<K, V, osize>::const_iterator it = bit; it != eit; ++it) {
                insert(*it);
            }
            return *this;
        }

        /// Assignment operator (from an std::tr1 unordered_map)
        /// @param ot std::tr1 unordered_map to assign from
        /// @return self
        const bmap & operator =(const std::tr1::unordered_map<K,V> & ot)
        {
            this->clear();
            typename std::tr1::unordered_map<K,V>::const_iterator bit = ot.begin();
            typename std::tr1::unordered_map<K,V>::const_iterator eit = ot.end();
            for (typename std::tr1::unordered_map<K, V>::const_iterator it = bit; it != eit; ++it) {
                insert(*it);
            }
            return *this;
        }

        /// Equality comparison
        /// @param ot bounded size map to compare with
        /// @return true if the contained elements are the same, false otherwise
        bool operator ==(const bmap<K,V,msize> & ot) const
        {
            return this->operator ==<msize>(ot);
        }

        /// Equality comparison
        /// @param ot bounded size map to compare with
        /// @return true if the contained elements are the same, false otherwise
        template <int32_t osize>
        bool operator ==(const bmap<K,V,osize> & ot) const
        {
            if (getUsedSize() != ot.getUsedSize()) {
                return false;
            }
            typename bmap<K,V,osize>::const_iterator bit = ot.begin();
            typename bmap<K,V,osize>::const_iterator eit = ot.end();
            for(typename bmap<K,V,osize>::const_iterator it = bit; it!=eit; ++it) {
                if (count(it->first) == 0) {
                    return false;
                }
                if (find(it->first)->second != it->second) {
                    return false;
                }
            }
            return true;
        }

        /// Non-equality comparison
        /// @param ot bounded size map to compare with
        /// @return true if the contained elements are not the same, false otherwise
        bool operator !=(const bmap<K,V,msize> & ot) const
        {
            return !(this->operator ==(ot));
        }

        /// Non-equality comparison
        /// @param ot bounded size map to compare with
        /// @return true if the contained elements are not the same, false otherwise
        template <int32_t osize>
        bool operator !=(const bmap<K,V,osize> & ot) const
        {
            return !(this->operator ==<osize>(ot));
        }

        /// Get a begin iterator
        /// @return begin iterator
        iterator begin()
        {
            return iterator(*this, 0);
        }

        /// Get a begin const iterator
        /// @return begin const iterator
        const_iterator begin() const
        {
            return const_iterator(*this, 0);
        }

        /// Get an end iterator
        /// @return end iterator
        iterator end()
        {
            return iterator(*this,msize);
        }

        /// Get an end const iterator
        /// @return end const iterator
        const_iterator end() const
        {
            return const_iterator(*this,msize);
        }

        /// Get the bounded size of the map
        /// @return bounded size of the map
        int32_t max_size() const
        {
            return msize;
        }

        /// Get the number of used elements
        /// @return number of used elements
        size_type size() const
        {
            return info_->usedsz_;
        }

        /// Get the hash function
        /// @return the hash function
        hasher hash_funct() const
        {
            return hasher();
        }

        /// Remove all elements
        void clear()
        {
            for (int32_t i = 0; i < info_->usedsz_; i++) {
                data_[i] = value_type();
            }
            info_->usedsz_ = 0;
            memset(info_->used_,0,msize);
        }

        /// Check if empty
        /// @return true if empty, false otherwise
        bool empty() const
        {
            return (info_->usedsz_==0);
        }

        /// Find an element in the map
        /// @param k search key
        /// @return an iterator for the element if found, the end iterator otherwise
        iterator find(const key_type & k)
        {
            int32_t p = findItem(k);
            if (p < 0) {
                return this->end();
            }
            return iterator(*this,p);
        }

        /// Find an element in the map
        /// @param k search key
        /// @return an iterator for the element if found, the end iterator otherwise
        const_iterator find(const key_type & k) const
        {
            int32_t p = findItem(k);
            if (p < 0) {
                return this->end();
            }
            return const_iterator(*this,p);
        }

        /// Count the number of elements with a given key
        /// @param k search key
        /// @return the number of elements with the given key
        size_type count(const key_type & k) const
        {
            int32_t p = findItem(k);
            if (p < 0) {
                return 0;
            }
            return 1;
        }

        /// Get the element with a given key, create it if it does not exist
        /// @param k search key
        /// @return reference to the element
        data_type & operator [](const key_type & k)
        {
            int32_t p = findItem(k);
            if(p<0) {
                value_type v(k, makeTemporary<data_type>());
                return insert(v).first->second;
            }
            return data_[p].second;
        }

        /// Get the element with a given key, throw exception if not present
        /// @param k search key
        /// @return reference to the element
        data_type & at(const key_type & k)
        {
            int32_t p = findItem(k);
            if(p<0) {
                typedef SPL::spl_cast<SPL::rstring,K> Converter;
                THROW_STRING(SPLRuntimeInvalidIndex,
                        SPL_APPLICATION_RUNTIME_MAP_INVALID_INDEX(Converter::cast(k)));
            }
            return data_[p].second;
        }

        /// Get the element with a given key, throw exception if not present
        /// @param k search key
        /// @return constant reference to the element
        const data_type & at(const key_type & k) const
        {
            int32_t p = findItem(k);
            if(p<0) {
                typedef SPL::spl_cast<SPL::rstring,K> Converter;
                THROW_STRING(SPLRuntimeInvalidIndex,
                        SPL_APPLICATION_RUNTIME_MAP_INVALID_INDEX(Converter::cast(k)));
            }
            return data_[p].second;
        }

        /// Append to list
        /// @param k key to be added
        /// @param v value to be added
        /// @return this
        bmap<K,V,msize> & add(const K & k, const V & v)
        {
            this->insert(std::make_pair(k,v)); return *this;
        }

        /// Insert a given element. If there is no space left in the bounded map, one of
        /// the existing items is replaced.
        /// @param x element to insert
        /// @return an iterator-bool pair, where the iterator points to the inserted
        /// element and the bool is true if no existing element was replaced, and
        /// false otherwise
        std::pair<iterator, bool> insert(const value_type & x)
        {
            int32_t p = findSlot(x.first);
            if(p<0) {
                p = getHash(x.first);
                data_[p] = x;
                return std::make_pair(iterator(*this,p),false);
            } else {
                data_[p] = x;
                if(info_->used_[p]) {
                    return std::make_pair(iterator(*this,p),false);
                } else {
                    info_->used_[p] = true;
                    info_->usedsz_++;
                    return std::make_pair(iterator(*this,p),true);
                }
            }
        }

        /// Insert a given sequence of elements
        /// @param first begin iterator for the insert sequence, inclusive
        /// @param last end iterator for the insert sequence, exclusive
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            for (InputIterator it = first; it != last; ++it) {
                insert(*it);
            }
        }

        /// Remove an element at a given position
        /// @param pos iterator pointing to the element
        void erase(iterator pos)
        {
            erase(pos->first);
        }

        /// Remove elements with a given key
        /// @param k search key
        /// @return the number of elements removed
        size_type erase(const key_type & k)
        {
            int32_t p = findItem(k);
            if (p < 0) {
                return 0;
            }
            data_[p] = value_type();
            info_->used_[p] = false;
            info_->usedsz_--;
            return 1;
        }

        /// Remove a sequence of elements
        /// @param first begin iterator for the remove sequence, inclusive
        /// @param last end iterator for the remove sequence, exclusive
        void erase(iterator first, iterator last)
        {
            for (iterator it = first; it != last; ++it) {
                erase(it->first);
            }
        }

        /// Get the C array representation
        /// @return C array representation
        value_type const * getData() const
        {
            return data_;
        }

        /// Swap
        /// @param ot bounded size map to swap with
        template <int32_t osize>
        void swap(bmap<K,V,osize> & ot)
        {
            if(msize == osize && deallocate_ && ot.deallocate_) {
                // Just swap pointers, size used and both controlling memory
                Info * tmp = info_;
                value_type *dTmp = data_;
                info_ = reinterpret_cast<Info *>(ot.info_);
                data_ = ot.data_;
                ot.info_ = reinterpret_cast<typename bmap<K,V,osize>::Info *>(tmp);
                ot.data_ = dTmp;
            } else {
                bmap<K,V,osize> o(ot);
                ot.template operator=<msize>(* this);
                this->operator =<osize>(o);
            }
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            buf << info_->usedsz_;
            for (size_type i = 0; i < msize; ++i) {
                buf << data_[i].first << data_[i].second;
            }
            for (size_type i = 0; i < msize; ++i) {
                buf << info_->used_[i];
            }
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            buf >> info_->usedsz_;
            for (size_type i = 0; i < msize; ++i) {
                buf >> data_[i].first >> data_[i].second;
            }
            for (size_type i = 0; i < msize; ++i) {
                buf >> info_->used_[i];
            }
        }

#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            buf << info_->usedsz_;
            for (size_type i = 0; i < msize; ++i) {
                buf << data_[i].first << data_[i].second;
            }
            for (size_type i = 0; i < msize; ++i) {
                buf << info_->used_[i];
            }
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            buf >> info_->usedsz_;
            for (size_type i = 0; i < msize; ++i) {
                buf >> data_[i].first >> data_[i].second;
            }
            for (size_type i = 0; i < msize; ++i) {
                buf >> info_->used_[i];
            }
        }

        /// Serialize (character)
        /// @param ostr output stream
        void serialize(std::ostream & ostr) const;

        /// Serialize (character), with maximum precision for floats
        /// @param ostr output stream
        void serializeWithPrecision(std::ostream & ostr) const;

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
            std::tr1::hash<K> hsk;
            std::tr1::hash<V> hsv;
            const_iterator bit = this->begin();
            const_iterator eit = this->end();
            for(const_iterator it = bit; it!=eit; ++it) {
                r = 37 * r + hsk(it->first);
                r = 37 * r + hsv(it->second);
            }
            return r;
        }

        /// Normalize the contents of the map via rehashing
        ///
        void normalize()
        {
            value_type tmp[msize];
            size_type sz = 0;
            for (size_type i = 0; i < msize; ++i) {
                if (info_->used_[i]) {
                    tmp[sz++] = data_[i];
                }
            }
            this->clear();
            for (size_type i = 0; i < sz; ++i) {
                this->insert(tmp[i]);
            }
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
            return info_->usedsz_;
        }

        virtual bool equals(const Collection & ot) const
        {
            BMap const * o = dynamic_cast<BMap const *>(&ot);
            if (o == NULL) {
                return false;
            }
            if (getUsedSize() != o->getUsedSize()) {
                return false;
            }
            ConstMapIterator myend = getEndIterator();
            ConstMapIterator bit = o->getBeginIterator();
            ConstMapIterator eit = o->getEndIterator();
            for(ConstMapIterator it = bit; it!=eit; ++it) {
                ConstMapIterator rit = findElement((*it).first);
                if (rit == myend) {
                    return false;
                }
                if (!((*rit).second.equals((*it).second))) {
                    return false;
                }
            }
            return true;
        }

        virtual void assignFrom(const Collection & ot)
        {
            BMap const * o = dynamic_cast<BMap const *>(&ot);
            if (o == NULL) {
                THROW_STRING(SPLRuntimeTypeMismatch,
                             SPL_APPLICATION_RUNTIME_INVALID_BMAP_COLLECTION(
                               typeid(K).name(), typeid(V).name(), msize));
            }
            this->clear();
            ConstMapIterator bit = o->getBeginIterator();
            ConstMapIterator eit = o->getEndIterator();
            for(ConstMapIterator it = bit; it!=eit; ++it) {
                const key_type & k = (*it).first;
                const data_type & d = (*it).second;
                insert(std::make_pair(k,d));
            }
        }

        virtual void swapWith(Collection & ot)
        {
            if (!SPL::equals(typeid(*this), typeid(ot))) {
                THROW_STRING(SPLRuntimeTypeMismatch,
                             SPL_APPLICATION_RUNTIME_INVALID_BMAP_COLLECTION(
                               typeid(K).name(), typeid(V).name(), msize));
            }
            // the below is safe, based on our inheritance hierarchy
            bmap<K,V,msize> & o = static_cast<bmap<K,V,msize> &>(ot);
            this->swap<msize>(o);
        }

        virtual Collection * clone() const
        {
            return new bmap<K,V,msize>(*this);
        }

        virtual Meta::Type getKeyMetaType() const
        {
            return Meta::Type::typeOf<K>();
        }

        virtual Meta::Type getValueMetaType() const
        {
            return Meta::Type::typeOf<V>();
        }

        virtual std::pair<Meta::Type,Meta::Type> getElementMetaType() const
        {
            return std::make_pair(getKeyMetaType(), getValueMetaType());
        }

        virtual ValueHandle createKey() const
        {
            return ValueHandle(*(new K()));
        }

        virtual ValueHandle createValue() const
        {
            return ValueHandle(*(new V()));
        }

        virtual std::pair<ValueHandle,ValueHandle> createElement() const
        {
            return std::make_pair(createKey(),createValue());
        }

        virtual int32_t getSize() const
        {
            return this->size();
        }

        virtual MapIterator findElement(const ConstValueHandle & key)
        {
            const K & k = key;
            iterator rit = this->find(k);
            void * it = new iterator(rit);
            return MapIterator(*this,it);
        }

        virtual ConstMapIterator findElement(const ConstValueHandle & key) const
        {
            const K & k = key;
            iterator rit = const_cast<bmap<K,V,msize> *>(this)->find(k);
            void * it = new iterator(rit);
            return ConstMapIterator(*this,it);
        }

        virtual void insertElement(const ConstValueHandle & key,
                           const ConstValueHandle & value)
        {
            const K & k = key;
            const V & v = value;
            this->insert(std::make_pair(k,v));
        }

        virtual void removeElement(const ConstValueHandle & key)
        {
            const K & k = key;
            this->erase(k);
        }

        virtual void removeAllElements()
        {
            this->clear();
        }

        virtual MapIterator getBeginIterator()
        {
            iterator rit = this->begin();
            void * it = new iterator(rit);
            return MapIterator(*this,it);
        }

        virtual ConstMapIterator getBeginIterator() const
        {
            iterator rit = const_cast<bmap<K,V,msize> *>(this)->begin();
            void * it = new iterator(rit);
            return ConstMapIterator(*this,it);
        }

        virtual MapIterator getEndIterator()
        {
            iterator rit = this->end();
            void * it = new iterator(rit);
            return MapIterator(*this,it);
        }

        virtual ConstMapIterator getEndIterator() const
        {
            iterator rit = const_cast<bmap<K,V,msize> *>(this)->end();
            void * it = new iterator(rit);
            return ConstMapIterator(*this,it);
        }

        virtual size_t getSerializedSize() const
        {
            size_t sz = sizeof(size_type);
            for (size_type i = 0; i < msize; ++i) {
                sz += 1 + SPL::getSerializedSize(data_[i].first) +
                      SPL::getSerializedSize(data_[i].second);
            }
            return sz;
        }
        //@}

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Get the used element map
        /// @return used element map
        unsigned char * getUsed() const
        {
            return info_->used_;
        }

    protected:
        const MapIterManager & getIterManager() const
        {
            return *iterManager_;
        }
#endif /*DOXYGEN_SKIP_FOR_USERS*/

    private:
        void initialize()
        {
            info_ = new Info;
            deallocate_ = true;
            info_->usedsz_ = 0;
            data_ = new value_type[msize];
            memset(info_->used_, 0, msize);
        }

        int32_t setUsedSize(int32_t sizeIn)
        {
            int32_t tmp = info_->usedsz_; info_->usedsz_ = sizeIn; return tmp;
        }

        int32_t getHash(const key_type & k) const
        {
            return static_cast<int32_t>(hasher() (k) % msize);
        }

        int32_t findSlot(const key_type & k) const
        {
            int32_t i = getHash(k);
            int32_t sloc = i;
            while(true) {
                if (!info_->used_[i] || data_[i].first == k) {
                    return i;
                }
                i = (i + 1) % msize;
                if (i == sloc) {
                    break;
                }
            }
            return -1;
        }

        int32_t findItem(const key_type & k) const
        {
            int32_t i = getHash(k);
            int32_t sloc = i;
            while(true) {
                if (info_->used_[i] && data_[i].first == k) {
                    return i;
                }
                i = (i + 1) % msize;
                if (i == sloc) {
                    break;
                }
            }
            return -1;
        }

        static void deleteIterator(void * it)
        {
            delete castIterator(it);
        }
        static void incrementIterator(void * it)
        {
            ++(*castIterator(it));
        }
        static bool compareIterator(void * it0, void * it1)
        {
            return *castIterator(it0)==*castIterator(it1);
        }
        static void * cloneIterator(void * it)
        {
            return new iterator(*castIterator(it));
        }
        static std::pair<ConstValueHandle,ValueHandle> dereferenceIterator(void * it)
        {
            return std::make_pair(ConstValueHandle((*castIterator(it))->first),
                                  ValueHandle((*castIterator(it))->second));
        }
        static iterator * castIterator(void * it)
        {
            return static_cast<iterator *>(it);
        }
        static MapIterManager * initIterManager()
        {
            MapIterManager * iterManager = new MapIterManager;
            iterManager->deleter_ = deleteIterator;
            iterManager->incrementer_ = incrementIterator;
            iterManager->comparer_ = compareIterator;
            iterManager->cloner_ = cloneIterator;
            iterManager->dereferencer_ = dereferenceIterator;
            return iterManager;
        }
        static std::auto_ptr<MapIterManager> iterManager_;

#ifndef DOXYGEN_SKIP_FOR_USERS
        struct Info
        {
            size_type usedsz_;
            unsigned char used_[msize];
        };
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        Info * info_;
        value_type *data_;
        bool deallocate_;
        template <class,class,int32_t>
        friend class bmap;
        friend class bmap_iterator<K,V,msize>;
        friend class const_bmap_iterator<K,V,msize>;
    };

    template <class K, class V, int32_t msize>
    std::auto_ptr<MapIterManager> bmap<K,V,msize>::iterManager_(initIterManager());

    template <class K, class V, int32_t msize>
    void bmap<K,V,msize>::serialize(std::ostream & ostr) const
    {
        ostr << "{";
        const_iterator bit = this->begin();
        const_iterator eit = this->end();
        for(const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            ostr << it->first;
            ostr << ":";
            ostr << it->second;
        }
        ostr << "}";
    }

    template <class K, class V, int32_t msize>
    void bmap<K,V,msize>::serializeWithPrecision(std::ostream & ostr) const
    {
        ostr << "{";
        const_iterator bit = this->begin();
        const_iterator eit = this->end();
        for(const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            SPL::serializeWithPrecision(ostr, it->first);
            ostr << ":";
            SPL::serializeWithPrecision(ostr, it->second);
        }
        ostr << "}";
    }


    template <class K, class V, int32_t msize>
    void bmap<K,V,msize>::deserialize(std::istream & istr,bool withSuffix)
    {
        this->clear();
        char c;
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!='{') {
            istr.setstate(std::ios_base::failbit);
            return;
        }
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!='}') {
            istr.putback(c);
            K k;
            if (withSuffix) {
                SPL::deserializeWithSuffix(istr,k);
            } else {
                istr >> k;
            }
            if (!istr) {
                return;
            }
            istr >> c;
            if (!istr) {
                return;
            }
            if(c!=':') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            V v;
            if (withSuffix) {
                SPL::deserializeWithSuffix(istr,v);
            } else {
                istr >> v;
            }
            if (!istr) {
                return;
            }
            insert(std::make_pair(k,v));
        } else {
            return;
        }
        while(true) {
            istr >> c;
            if (!istr) {
                return;
            }
            if (c == '}') {
                return;
            }
            if(c!=',') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            K k;
            if (withSuffix) {
                SPL::deserializeWithSuffix(istr,k);
            } else {
                istr >> k;
            }
            if (!istr) {
                return;
            }
            istr >> c;
            if (!istr) {
                return;
            }
            if(c!=':') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            V v;
            if (withSuffix) {
                SPL::deserializeWithSuffix(istr,v);
            } else {
                istr >> v;
            }
            if (!istr) {
                return;
            }
            insert(std::make_pair(k,v));
        }
    }

    template <class K, class V, int32_t msize>
    void bmap<K,V,msize>::deserializeWithNanAndInfs(std::istream & istr,bool withSuffix)
    {
        this->clear();
        char c;
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!='{') {
            istr.setstate(std::ios_base::failbit);
            return;
        }
        istr >> c;
        if (!istr) {
            return;
        }
        if(c!='}') {
            istr.putback(c);
            K k;
            SPL::deserializeWithNanAndInfs(istr,k,withSuffix);
            if (!istr) {
                return;
            }
            istr >> c;
            if (!istr) {
                return;
            }
            if(c!=':') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            V v;
            SPL::deserializeWithNanAndInfs(istr,v,withSuffix);
            if (!istr) {
                return;
            }
            insert(std::make_pair(k,v));
        } else {
            return;
        }
        while(true) {
            istr >> c;
            if (!istr) {
                return;
            }
            if (c == '}') {
                return;
            }
            if(c!=',') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            K k;
            SPL::deserializeWithNanAndInfs(istr,k,withSuffix);
            if (!istr) {
                return;
            }
            istr >> c;
            if (!istr) {
                return;
            }
            if(c!=':') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            V v;
            SPL::deserializeWithNanAndInfs(istr,v,withSuffix);
            if (!istr) {
                return;
            }
            insert(std::make_pair(k,v));
        }
    }


    template <class K, class V, int32_t msize>
    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const bmap<K,V,msize> & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    template <class K, class V, int32_t msize>
    inline std::ostream & operator <<(std::ostream & ostr, const bmap<K,V,msize> & x)
    {
        x.serialize(ostr); return ostr;
    }

    template <class K, class V, int32_t msize>
    inline std::istream & operator >>(std::istream & istr, bmap<K,V,msize> & x)
    {
        x.deserialize(istr); return istr;
    }

    template <class K, class V, int32_t msize>
    inline void deserializeWithSuffix(std::istream & istr, bmap<K,V,msize> & x)
    {
        x.deserialize(istr,true);
    }

    template <class K, class V, int32_t msize>
    inline void deserializeWithNanAndInfs(std::istream & istr, bmap<K,V,msize> & x, bool withSuffix = false)
    {
        x.deserializeWithNanAndInfs(istr,withSuffix);
    }

    template <class S, class K, class V, int32_t msize>
    inline ByteBuffer<S> & operator <<(ByteBuffer<S> & sbuf, const bmap<K,V,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, class V, int32_t msize>
    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & sbuf, const bmap<K,V,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, class V, int32_t msize>
    inline NativeByteBuffer & operator <<(NativeByteBuffer & sbuf, const bmap<K,V,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class S, class K, class V, int32_t msize>
    inline ByteBuffer<S> & operator >>(ByteBuffer<S> & sbuf, bmap<K,V,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K, class V, int32_t msize>
    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & sbuf, bmap<K,V,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K, class V, int32_t msize>
    inline NativeByteBuffer & operator >>(NativeByteBuffer & sbuf, bmap<K,V,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class K, class V, int32_t msize>
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, const bmap<K,V,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, class V, int32_t msize>
    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, bmap<K,V,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template<class K, class V, int32_t msize>
    struct has_bounded_set_or_map<bmap<K,V,msize> >
    {
        enum { value = true };
    };

    template<class K, class V, int32_t msize>
    void normalizeBoundedSetsAndMaps(bmap<K,V,msize> & v)
    {
        if (has_bounded_set_or_map<K>::value) {
            for (typename bmap<K, V, msize>::iterator it = v.begin(); it != v.end(); ++it) {
                normalizeBoundedSetsAndMaps(it->first);
            }
        }
        if (has_bounded_set_or_map<V>::value) {
            for (typename bmap<K, V, msize>::iterator it = v.begin(); it != v.end(); ++it) {
                normalizeBoundedSetsAndMaps(it->second);
            }
        }
        v.normalize();
    }

    template <class K, class V, int32_t msize>
    bmap_iterator<K,V,msize>::bmap_iterator(
        bmap<K,V,msize> & map, int32_t index)
        : map_(&map), index_(index)
    {
        if (index_ >= msize || map_->info_->used_[index_]) {
            return;
        }
        while (++index_ < msize) {
            if (map_->info_->used_[index_]) {
                break;
            }
        }
    }

    template <class K, class V, int32_t msize>
    bmap_iterator<K,V,msize>
    bmap_iterator<K,V,msize>::operator ++(int)
    {
        bmap_iterator<K,V,msize> old = *this;
        while (++index_ < msize) {
            if (map_->info_->used_[index_]) {
                break;
            }
        }
        return old;
    }

    template <class K, class V, int32_t msize>
    bmap_iterator<K,V,msize> &
    bmap_iterator<K,V,msize>::operator ++()
    {
        while (++index_ < msize) {
            if (map_->info_->used_[index_]) {
                break;
            }
        }
        return *this;
    }

    template <class K, class V, int32_t msize>
    std::pair<K,V> &bmap_iterator<K,V,msize>::operator *() const
    {
        return map_->data_[index_];
    }

    template <class K, class V, int32_t msize>
    std::pair<K,V> * bmap_iterator<K,V,msize>::operator ->() const
    {
        return &(map_->data_[index_]);
    }

    template <class K, class V, int32_t msize>
    const_bmap_iterator<K,V,msize>::const_bmap_iterator(
        const bmap<K,V,msize> & map, int32_t index)
        : map_(&map), index_(index)
    {
        if (index_ >= msize || map_->info_->used_[index_]) {
            return;
        }
        while (++index_ < msize) {
            if (map_->info_->used_[index_]) {
                break;
            }
        }
    }

    template <class K, class V, int32_t msize>
    const_bmap_iterator<K,V,msize>
    const_bmap_iterator<K,V,msize>::operator ++(int)
    {
        const_bmap_iterator<K,V,msize> old = *this;
        while (++index_ < msize) {
            if (map_->info_->used_[index_]) {
                break;
            }
        }
        return old;
    }

    template <class K, class V, int32_t msize>
    const_bmap_iterator<K,V,msize> &
    const_bmap_iterator<K,V,msize>::operator ++()
    {
        while (++index_ < msize) {
            if (map_->info_->used_[index_]) {
                break;
            }
        }
        return *this;
    }

    template <class K, class V, int32_t msize>
    const std::pair<K,V> &const_bmap_iterator<K,V,msize>::operator *() const
    {
        return map_->data_[index_];
    }

    template <class K, class V, int32_t msize>
    const std::pair<K,V> * const_bmap_iterator<K,V,msize>::operator ->() const
    {
        return &(map_->data_[index_]);
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */
};

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace std
{
    template <class K, class V, int32_t size>
    inline void swap(SPL::bmap<K,V,size> & lhs, SPL::bmap<K,V,size> & rhs)
    {
        lhs.swap(rhs);
    }

    template <class K, class V, int32_t lsize, int32_t rsize>
    inline void swap(SPL::bmap<K,V,lsize> & lhs, SPL::bmap<K,V,rsize> & rhs)
    {
        lhs.swap(rhs);
    }
}

// Hashability requirements
namespace std {
    namespace tr1 {
        template <class K, class V, int32_t size>
        struct hash<SPL::bmap<K,V,size> >
        {
            inline size_t operator ()(const SPL::bmap<K,V,size> & m) const
            {
                return m.hashCode();
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_BMAP_H */
