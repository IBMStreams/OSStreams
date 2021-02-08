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

#ifndef SPL_RUNTIME_TYPE_BSET_H
#define SPL_RUNTIME_TYPE_BSET_H

/*!
 * \file BSet.h \brief Definitions of the SPL::BSet and SPL::bset classes.
 */

#include <SPL/Runtime/Type/BaseSet.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Type/TypeTMPHelper.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/


PUSH_DLL_PUBLIC
namespace SPL
{
    // Forward declaration of set for ctor
    template <class T>
    class set;

    template <class TargetType, class SourceType> class spl_cast;

    /// \brief Class that defines a reflective interface for the bounded set type.
    class BSet : public Set
    {
    public:
        /// Get the bounded size of the set
        /// @return bounded size of the set
        virtual uint32_t getBoundedSize() const = 0;

        /// Get the number of used elements
        /// @return number of used elements
        virtual uint32_t getUsedSize() const = 0;

        /// Assign from another BSet
        /// @param ov other list to assign from
        /// @return this
        const BSet & operator =(const BSet & ov)
        {
            assignFrom(ov); return *this;
        }
    };

    template <class K, int32_t msize>
    class bset;
    template <class K, int32_t msize>
    class const_bset_iterator;

#ifndef DOXYGEN_SKIP_FOR_USERS
    /// \brief Class that serves as an iterator for reflective bounded set iteration.
    template <class K, int32_t msize>
    struct bset_iterator
    {
        /// Default constructor
        ///
        bset_iterator() {}

        /// Copy constructor
        /// @param ot iterator to construct from
        bset_iterator(const bset_iterator & ot)
            : set_(ot.set_), index_(ot.index_) {}

        /// Constructor
        /// @param set bounded set
        /// @param index index to start from
        bset_iterator(bset<K,msize> & set, int32_t index);

        /// Post increment operator
        /// @return iterator before increment
        bset_iterator operator ++(int);

        /// Pre increment operator
        /// @return iterator after increment
        bset_iterator & operator ++();

        /// Dereference operator
        /// @return the current element
        K & operator *() const;

        /// Pointer access operator
        /// @return pointer to the current element
        K * operator ->() const;

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator ==(const bset_iterator & ot) const
        {
            return set_==ot.set_ && index_==ot.index_;
        }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator !=(const bset_iterator & ot) const
        {
            return !(*this==ot);
        }
    private:
        bset<K,msize> * set_;
        int32_t index_;
        friend class const_bset_iterator<K,msize>;
    };

    /// \brief Class that serves as a constant iterator for reflective bounded set iteration.
    template <class K, int32_t msize>
    struct const_bset_iterator
    {
        /// Default constructor
        ///
        const_bset_iterator() {}

        /// Copy constructor
        /// @param ot iterator to construct from
        const_bset_iterator(const const_bset_iterator & ot)
            : set_(ot.set_), index_(ot.index_) {}

        /// Copy constructor
        /// @param ot iterator to construct from
        const_bset_iterator(const bset_iterator<K,msize> & ot)
            : set_(ot.set_), index_(ot.index_) {}

        /// Constructor
        /// @param set bounded set
        /// @param index index to start from
        const_bset_iterator(const bset<K,msize> & set, int32_t index);

        /// Post increment operator
        /// @return iterator before increment
        const_bset_iterator operator ++(int);

        /// Pre increment operator
        /// @return iterator after increment
        const_bset_iterator & operator ++();

        /// Dereference operator
        /// @return the current element
        const K & operator *() const;

        /// Pointer access operator
        /// @return pointer to the current element
        const K * operator ->() const;

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator ==(const const_bset_iterator & ot) const
        {
            return set_==ot.set_ && index_==ot.index_;
        }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator !=(const const_bset_iterator & ot) const
        {
            return !(*this==ot);
        }
    private:
        const bset<K,msize> * set_;
        int32_t index_;
    };

#endif /*DOXYGEN_SKIP_FOR_USERS*/

    /// \brief Class that represents bounded size set type.
    template <class K, int32_t msize>
    class bset : public BSet
    {
    public:
        typedef K value_type; //!< value type
        typedef typename TMP::SizeType<msize>::value size_type; //!< size type
        typedef std::tr1::hash<K> hasher; //!< hasher type
        typedef K & reference; //!< element reference type
        typedef K const & const_reference; //!< element constant reference type
        typedef K * pointer; //!< element pointer type
        typedef K const * const_pointer; //!< element constant pointer type
        typedef bset_iterator<K,msize> iterator; //!< iterator type
        typedef const_bset_iterator<K,msize> const_iterator; //!< constant iterator type

        /// Empty constructor
        ///
        explicit bset()
        {
            this->initialize();
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Constructor to use external memory
        /// @param buffer memory to use (must be a size_type followed by array of bytes, then
        /// array of value_types)
        /// @param initializeIn whether bset must be initialized or not
        explicit bset(void * buffer, bool initializeIn)
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
            if (deallocate_) {
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
        bset(InputIterator first, InputIterator last)
        {
            this->initialize();
            for (InputIterator it = first; it != last; ++it) {
                insert(*it);
            }
        }

        /// Construct from an assignable set
        /// @param o set to construct from
        template <class K2>
        bset(const std::tr1::unordered_set<K2> & o)
        {
            this->initialize();
            typename std::tr1::unordered_set<K2>::const_iterator it;
            for (it = o.begin(); it != o.end(); it++) {
                this->insert(spl_cast<K, K2>::cast(*it));
            }
        }

        /// Construct from an assignable bset
        /// @param o set to construct from
        template <class K2, int32_t msize2>
        bset(const bset<K2,msize2> & o)
        {
            this->initialize();
            typename bset<K2,msize2>::const_iterator bit = o.begin();
            typename bset<K2,msize2>::const_iterator eit = o.end();
            for (typename bset<K2, msize2>::const_iterator it = bit; it != eit; ++it) {
                this->insert(spl_cast<K, K2>::cast(*it));
            }
        }

        /// Copy constructor
        /// @param ot bounded size set to construct from
        bset(const bset<K,msize> & ot)
        {
            this->initialize(); this->operator =(ot);
        }

        /// Copy constructor
        /// @param ot bounded size set to construct from
        template <int32_t osize>
        bset(const bset<K,osize> & ot)
        {
            this->initialize(); this->operator =<osize>(ot);
        }

        /// Construct from an std::tr1::unordered_set
        /// @param ot unordered set to construct from
        bset(const std::tr1::unordered_set<K> & ot)
        {
            this->initialize(); this->operator =(ot);
        }

        /// Construct from a reflective bounded size set
        /// @param o reflective bounded size set to construct from
        bset(const BSet & o)
        {
            this->initialize(); assignFrom(o);
        }

        /// Construct from a value handle
        /// @param o value handle to construct from
        bset(const ConstValueHandle & o)
        {
            this->initialize(); BSet const & x = o; assignFrom(x);
        }

        /// Destructor
        ~bset()
        {
            if(deallocate_) {
                delete info_;
                delete [] data_;
            }
        }

        /// Assignment operator (from another bounded size set)
        /// @param ot bounded size set to assign from
        /// @return self
        const bset & operator =(const bset<K,msize> & ot)
        {
            return this->operator =<msize>(ot);
        }

        /// Assignment operator (from another bounded size set)
        /// @param ot bounded size set to assign from
        /// @return self
        template <int32_t osize>
        const bset & operator =(const bset<K,osize> & ot)
        {
            if (osize == msize && this == reinterpret_cast<bset<K, msize> const*>(&ot)) {
                return *this;
            }
            this->clear();
            typename bset<K,osize>::const_iterator bit = ot.begin();
            typename bset<K,osize>::const_iterator eit = ot.end();
            for (typename bset<K, osize>::const_iterator it = bit; it != eit; ++it) {
                insert(*it);
            }
            return *this;
        }

        /// Assignment operator (from an std::tr1 unordered_set)
        /// @param ot std::tr1 unordered_set to assign from
        /// @return self
        const bset & operator =(const std::tr1::unordered_set<K> & ot)
        {
            this->clear();
            typename std::tr1::unordered_set<K>::const_iterator bit = ot.begin();
            typename std::tr1::unordered_set<K>::const_iterator eit = ot.end();
            for (typename std::tr1::unordered_set<K>::const_iterator it = bit; it != eit; ++it) {
                insert(*it);
            }
            return *this;
        }

        /// Equality comparison
        /// @param ot bounded size set to compare with
        /// @return true if the contained elements are the same, false otherwise
        bool operator ==(const bset<K,msize> & ot) const
        {
            return this->operator ==<msize>(ot);
        }

        /// Equality comparison
        /// @param ot bounded size set to compare with
        /// @return true if the contained elements are the same, false otherwise
        template <int32_t osize>
        bool operator ==(const bset<K,osize> & ot) const
        {
            if (getUsedSize() != ot.getUsedSize()) {
                return false;
            }
            typename bset<K,osize>::const_iterator bit = ot.begin();
            typename bset<K,osize>::const_iterator eit = ot.end();
            for (typename bset<K, osize>::const_iterator it = bit; it != eit; ++it) {
                if (count(*it) == 0) {
                    return false;
                }
            }
            return true;
        }

        /// Non-equality comparison
        /// @param ot bounded size set to compare with
        /// @return true if the contained elements are not the same, false otherwise
        bool operator !=(const bset<K,msize> & ot) const
        {
            return !(this->operator ==(ot));
        }

        /// Non-equality comparison
        /// @param ot bounded size set to compare with
        /// @return true if the contained elements are not the same, false otherwise
        template <int32_t osize>
        bool operator !=(const bset<K,osize> & ot) const
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

        /// Get the bounded size of the set
        /// @return bounded size of the set
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
        ///
        void clear()
        {
            for (int32_t i = 0; i < info_->usedsz_; i++) {
                data_[i] = K();
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

        /// Find an element in the set
        /// @param k search key
        /// @return an iterator for the element if found, the end iterator otherwise
        iterator find(const value_type & k)
        {
            int32_t p = findItem(k);
            if (p < 0) {
                return this->end();
            }
            return iterator(*this,p);
        }

        /// Find an element in the set
        /// @param k search key
        /// @return an iterator for the element if found, the end iterator otherwise
        const_iterator find(const value_type & k) const
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
        size_type count(const value_type & k) const
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
        value_type & operator [](const value_type & k)
        {
            int32_t p = findItem(k);
            if (p < 0) {
                return *(insert(k).first);
            }
            return data_[p];
        }

        /// Insert into set
        /// @param k   Key to be added
        /// @return *this
        bset<K,msize> & add(const K & k)
        {
            insert(k); return *this;
        }

        /// Insert a given element. If there is no space left in the bounded set, one of
        /// the existing items is replaced.
        /// @param x element to insert
        /// @return a iterator-bool pair, where the iterator points to the inserted
        /// element and the bool is true if no existing element was replaced, and
        /// false otherwise
        std::pair<iterator, bool> insert(const value_type & x)
        {
            int32_t p = findSlot(x);
            if(p<0) {
                p = getHash(x);
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
            erase(*pos);
        }

        /// Remove elements with a given key
        /// @param k search key
        /// @return the number of elements removed
        size_type erase(const value_type & k)
        {
            int32_t p = findItem(k);
            if (p < 0) {
                return 0;
            }
            data_[p] = K();
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
                erase(*it);
            }
        }

        /// Get the C array representation
        /// @return C array representation
        value_type const * getData() const
        {
            return data_;
        }

        /// Swap
        /// @param ot bounded size set to swap with
        template <int32_t osize>
        void swap(bset<K,osize> & ot)
        {
            if(osize == msize && deallocate_ && ot.deallocate_) {
                Info * tmp = info_;
                value_type *dTmp = data_;
                info_ = reinterpret_cast<Info *>(ot.info_);
                data_ = ot.data_;
                ot.info_ = reinterpret_cast<typename bset<K,osize>::Info *>(tmp);
                ot.data_ = dTmp;
            } else {
                bset<K,osize> o(ot);
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
                buf << data_[i];
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
                buf >> data_[i];
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
                buf << data_[i];
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
                buf >> data_[i];
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
            const_iterator bit = this->begin();
            const_iterator eit = this->end();
            for(const_iterator it = bit; it!=eit; ++it) {
                r = 37 * r + hsk(*it);
            }
            return r;
        }

        /// Normalize the contents of the set via rehashing
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

        /// Get the number of used elements
        /// @return number of used elements
        virtual uint32_t getUsedSize() const
        {
            return info_->usedsz_;
        }

        virtual bool equals(const Collection & ot) const
        {
            BSet const * o = dynamic_cast<BSet const *>(&ot);
            if (o == NULL) {
                return false;
            }
            if (getUsedSize() != o->getUsedSize()) {
                return false;
            }
            ConstSetIterator myend = getEndIterator();
            ConstSetIterator bit = o->getBeginIterator();
            ConstSetIterator eit = o->getEndIterator();
            for(ConstSetIterator it = bit; it!=eit; ++it) {
                ConstSetIterator rit = findElement(*it);
                if (rit == myend) {
                    return false;
                }
                if (!(*rit).equals(*it)) {
                    return false;
                }
            }
            return true;
        }

        virtual void assignFrom(const Collection & ot)
        {
            BSet const * o = dynamic_cast<BSet const *>(&ot);
            if (o == NULL) {
                THROW(SPLRuntimeTypeMismatch,
                      "Cannot create templated bset<K,size> from Collection, where K = "
                        << typeid(K).name());
            }
            this->clear();
            ConstSetIterator bit = o->getBeginIterator();
            ConstSetIterator eit = o->getEndIterator();
            for(ConstSetIterator it = bit; it!=eit; ++it) {
                const value_type & v = *it;
                insert(v);
            }
        }

        virtual void swapWith(Collection & ot)
        {
            if (!SPL::equals(typeid(*this), typeid(ot))) {
                THROW(SPLRuntimeTypeMismatch,
                      "Cannot create templated bset<K,size> from Collection, where T = "
                        << typeid(K).name() << ", and size=" << msize);
            }
            // the below is safe, based on our inheritance hierarchy
            bset<K,msize> & o = static_cast<bset<K,msize> &>(ot);
            this->swap<msize>(o);
        }

        virtual Collection * clone() const
        {
            return new bset<K,msize>(*this);
        }

        virtual Meta::Type getElementMetaType() const
        {
            return Meta::Type::typeOf<K>();
        }

        virtual ValueHandle createElement() const
        {
            return ValueHandle(*(new K()));
        }

        virtual int32_t getSize() const
        {
            return this->size();
        }

        virtual SetIterator findElement(const ConstValueHandle & key)
        {
            const K & k = key;
            iterator rit = this->find(k);
            void * it = new iterator(rit);
            return SetIterator(*this,it);
        }

        virtual ConstSetIterator findElement(const ConstValueHandle & key) const
        {
            const K & k = key;
            iterator rit = const_cast<bset<K,msize> *>(this)->find(k);
            void * it = new iterator(rit);
            return ConstSetIterator(*this,it);
        }

        virtual void insertElement(const ConstValueHandle & key)
        {
            const K & k = key;
            this->insert(k);
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

        virtual SetIterator getBeginIterator()
        {
            iterator rit = this->begin();
            void * it = new iterator(rit);
            return SetIterator(*this,it);
        }

        virtual ConstSetIterator getBeginIterator() const
        {
            iterator rit = const_cast<bset<K,msize> *>(this)->begin();
            void * it = new iterator(rit);
            return ConstSetIterator(*this,it);
        }

        virtual SetIterator getEndIterator()
        {
            iterator rit = this->end();
            void * it = new iterator(rit);
            return SetIterator(*this,it);
        }

        virtual ConstSetIterator getEndIterator() const
        {
            iterator rit = const_cast<bset<K,msize> *>(this)->end();
            void * it = new iterator(rit);
            return ConstSetIterator(*this,it);
        }

        virtual size_t getSerializedSize() const
        {
            size_t sz = sizeof(size_type);
            for (size_type i = 0; i < msize; ++i) {
                sz += 1 + SPL::getSerializedSize(data_[i]);
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

        /// Set the used element map
        /// @param map used element map
        /// @return previous used element map
        unsigned char * setUsed(unsigned char * map)
        {
            unsigned char * tmp = info_->used_; info_->used_ = map; return tmp;
        }
#endif /*DOXYGEN_SKIP_FOR_USERS*/

    protected:
        const SetIterManager & getIterManager() const
        {
            return *iterManager_;
        }
    private:
        void initialize()
        {
            info_ = new Info;
            info_->usedsz_ = 0;
            data_ = new value_type[msize];
            deallocate_ = true;
            memset(info_->used_, 0, msize);
        }

        int32_t setUsedSize(int32_t sizeIn)
        {
            int32_t tmp = info_->usedsz_; info_->usedsz_ = sizeIn; return tmp;
        }

        int32_t getHash(const value_type & k) const
        {
            return static_cast<int32_t>(hasher() (k) % msize);
        }

        int32_t findSlot(const value_type & k) const
        {
            int32_t i = getHash(k);
            int32_t sloc = i;
            while(true) {
                if (!info_->used_[i] || data_[i] == k) {
                    return i;
                }
                i = (i + 1) % msize;
                if (i == sloc) {
                    break;
                }
            }
            return -1;
        }

        int32_t findItem(const value_type & k) const
        {
            int32_t i = getHash(k);
            int32_t sloc = i;
            while(true) {
                if (info_->used_[i] && data_[i] == k) {
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
        static ConstValueHandle dereferenceIterator(void * it)
        {
            return ConstValueHandle(*(*castIterator(it)));
        }
        static iterator * castIterator(void * it)
        {
            return static_cast<iterator *>(it);
        }
        static SetIterManager * initIterManager()
        {
            SetIterManager * iterManager = new SetIterManager;
            iterManager->deleter_ = deleteIterator;
            iterManager->incrementer_ = incrementIterator;
            iterManager->comparer_ = compareIterator;
            iterManager->cloner_ = cloneIterator;
            iterManager->dereferencer_ = dereferenceIterator;
            return iterManager;
        }
        static std::auto_ptr<SetIterManager> iterManager_;

        struct Info
        {
            size_type usedsz_;
            unsigned char used_[msize];
        };
        Info * info_;
        value_type *data_;
        bool deallocate_;
        template <class,int32_t>
        friend class bset;
        friend class bset_iterator<K,msize>;
        friend class const_bset_iterator<K,msize>;
    };

    template <class K, int32_t msize>
    std::auto_ptr<SetIterManager> bset<K,msize>::iterManager_(initIterManager());

    template <class K, int32_t msize>
    void bset<K,msize>::serialize(std::ostream & ostr) const
    {
        ostr << "{";
        const_iterator bit = this->begin();
        const_iterator eit = this->end();
        for(const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            ostr << *it;
        }
        ostr << "}";
    }

    template <class K, int32_t msize>
    void bset<K,msize>::serializeWithPrecision(std::ostream & ostr) const
    {
        ostr << "{";
        const_iterator bit = this->begin();
        const_iterator eit = this->end();
        for(const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            SPL::serializeWithPrecision(ostr, *it);
        }
        ostr << "}";
    }

    template <class K, int32_t msize>
    void bset<K,msize>::deserialize(std::istream & istr,bool withSuffix)
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
            insert(k);
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
            insert(k);
        }
    }

    template <class K, int32_t msize>
    void bset<K,msize>::deserializeWithNanAndInfs(std::istream & istr,bool withSuffix)
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
            SPL::deserializeWithNanAndInfs(istr,k, withSuffix);
            if (!istr) {
                return;
            }
            insert(k);
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
            SPL::deserializeWithNanAndInfs(istr,k, withSuffix);
            if (!istr) {
                return;
            }
            insert(k);
        }
    }


    template <class K, int32_t msize>
    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const bset<K,msize> & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    template <class K, int32_t msize>
    inline std::ostream & operator <<(std::ostream & ostr, const bset<K,msize> & x)
    {
        x.serialize(ostr); return ostr;
    }

    template <class K, int32_t msize>
    inline std::istream & operator >>(std::istream & istr, bset<K,msize> & x)
    {
        x.deserialize(istr); return istr;
    }

    template <class K, int32_t msize>
    inline void deserializeWithSuffix(std::istream & istr, bset<K,msize> & x)
    {
        x.deserialize(istr,true);
    }

    template <class K, int32_t msize>
    inline void deserializeWithNanAndInfs(std::istream & istr, bset<K,msize> & x, bool withSuffix = false)
    {
        x.deserialize(istr,withSuffix);
    }

    template <class S, class K, int32_t msize>
    inline ByteBuffer<S> & operator <<(ByteBuffer<S> & sbuf, const bset<K,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, int32_t msize>
    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & sbuf, const bset<K,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, int32_t msize>
    inline NativeByteBuffer & operator <<(NativeByteBuffer & sbuf, const bset<K,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class S, class K, int32_t msize>
    inline ByteBuffer<S> & operator >>(ByteBuffer<S> & sbuf, bset<K,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K, int32_t msize>
    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & sbuf, bset<K,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K, int32_t msize>
    inline NativeByteBuffer & operator >>(NativeByteBuffer & sbuf, bset<K,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class K, int32_t msize>
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, const bset<K,msize> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, int32_t msize>
    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, bset<K,msize> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template<class K, int32_t msize>
    struct has_bounded_set_or_map<bset<K,msize> >
    {
        enum { value = true };
    };

    template<class K, int32_t msize>
    void normalizeBoundedSetsAndMaps(bset<K,msize> & v)
    {
        if (has_bounded_set_or_map<K>::value) {
            for (typename bset<K, msize>::iterator it = v.begin(); it != v.end(); ++it) {
                normalizeBoundedSetsAndMaps(*it);
            }
        }
        v.normalize();
    }

    template <class K, int32_t msize>
    bset_iterator<K,msize>::bset_iterator(
        bset<K,msize> & set, int32_t index)
        : set_(&set), index_(index)
    {
        if (index_ >= msize || set_->info_->used_[index_]) {
            return;
        }
        while (++index_ < msize) {
            if (set_->info_->used_[index_]) {
                break;
            }
        }
    }

    template <class K, int32_t msize>
    bset_iterator<K,msize>
    bset_iterator<K,msize>::operator ++(int)
    {
        bset_iterator<K,msize> old = *this;
        while (++index_ < msize) {
            if (set_->info_->used_[index_]) {
                break;
            }
        }
        return old;
    }

    template <class K, int32_t msize>
    bset_iterator<K,msize> &
    bset_iterator<K,msize>::operator ++()
    {
        while (++index_ < msize) {
            if (set_->info_->used_[index_]) {
                break;
            }
        }
        return *this;
    }

    template <class K, int32_t msize>
    K & bset_iterator<K,msize>::operator *() const
    {
        return set_->data_[index_];
    }

    template <class K, int32_t msize>
    K * bset_iterator<K,msize>::operator ->() const
    {
        return &(set_->data_[index_]);
    }

    template <class K, int32_t msize>
    const_bset_iterator<K,msize>::const_bset_iterator(
        const bset<K,msize> & set, int32_t index)
        : set_(&set), index_(index)
    {
        if (index_ >= msize || set_->info_->used_[index_]) {
            return;
        }
        while (++index_ < msize) {
            if (set_->info_->used_[index_]) {
                break;
            }
        }
    }

    template <class K, int32_t msize>
    const_bset_iterator<K,msize>
    const_bset_iterator<K,msize>::operator ++(int)
    {
        const_bset_iterator<K,msize> old = *this;
        while (++index_ < msize) {
            if (set_->info_->used_[index_]) {
                break;
            }
        }
        return old;
    }

    template <class K, int32_t msize>
    const_bset_iterator<K,msize> &
    const_bset_iterator<K,msize>::operator ++()
    {
        while (++index_ < msize) {
            if (set_->info_->used_[index_]) {
                break;
            }
        }
        return *this;
    }

    template <class K, int32_t msize>
    const K &const_bset_iterator<K,msize>::operator *() const
    {
        return set_->data_[index_];
    }

    template <class K, int32_t msize>
    const K * const_bset_iterator<K,msize>::operator ->() const
    {
        return &(set_->data_[index_]);
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */
};

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace std
{
    template <class K, int32_t size>
    inline void swap(SPL::bset<K,size> & lhs, SPL::bset<K,size> & rhs)
    {
        lhs.swap(rhs);
    }

    template <class K, int32_t lsize, int32_t rsize>
    inline void swap(SPL::bset<K,lsize> & lhs, SPL::bset<K,rsize> & rhs)
    {
        lhs.swap(rhs);
    }
}

// Hashability requirements
namespace std {
    namespace tr1 {
        template <class K, int32_t size>
        struct hash<SPL::bset<K,size> >
        {
            inline size_t operator ()(const SPL::bset<K,size> & m) const
            {
                return m.hashCode();
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_BSET_H */
