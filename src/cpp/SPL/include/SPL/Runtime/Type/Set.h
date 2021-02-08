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

#ifndef SPL_RUNTIME_TYPE_SET_H
#define SPL_RUNTIME_TYPE_SET_H

/*!
 * \file Set.h \brief Definition of the SPL::set class.
 */

#include <SPL/Runtime/Type/BSet.h>
#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    /// \class std::tr1::unordered_set

    /// \brief Class that represents a set type (extends from
    /// std::tr1::unordered_set).
    /// \extends std::tr1::unordered_set
    template <class K>
    class set : public Set
#ifndef DOXYGEN_SKIP_FOR_USERS
    // doxygen does not pickup overloaded functions with multiple inheritance
              , public std::tr1::unordered_set<K>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
    {
    public:
        /// Default constructor
        ///
        explicit set() : std::tr1::unordered_set<K>() {}

        /// Construct from a unordered_set
        /// @param x unordered_set to use
        set(const std::tr1::unordered_set<K> & x) : std::tr1::unordered_set<K>(x) {}

        /// Copy constructor
        /// @param x set to construct from
        set(const set<K> & x) : std::tr1::unordered_set<K>(x) {}

        /// Construct from a bounded set
        /// @param x bounded set to construct from
        template <int32_t msize>
        set(const bset<K,msize> & x) : std::tr1::unordered_set<K>(x.begin(), x.end()) {}

        /// Construct from a sequence
        /// @param first begin iterator, inclusive
        /// @param last end iterator, exclusive
        template <class InputIterator>
        set(InputIterator first, InputIterator last) : std::tr1::unordered_set<K>(first,last) {}

        /// Construct from a reflective set
        /// @param o reflective set to construct from
        set(const Set & o)
        {
            assignFrom(o);
        }

        /// Construct from a value handle
        /// @param o value handle to construct from
        set(const ConstValueHandle & o)
        {
            Set const & x = o; assignFrom(x);
        }

        /// Construct from an assignable set
        /// @param o set to construct from
        template <class K2>
        set(const set<K2> & o)
        {
            typename set<K2>::const_iterator it;
            for (it = o.begin(); it != o.end(); it++) {
                this->insert(spl_cast<K, K2>::cast(*it));
            }
        }

        /// Construct from an assignable bset
        /// @param o set to construct from
        template <class K2, int32_t msize>
        set(const bset<K2,msize> & o)
        {
            typename bset<K2,msize>::const_iterator bit = o.begin();
            typename bset<K2,msize>::const_iterator eit = o.end();
            for (typename bset<K2, msize>::const_iterator it = bit; it != eit; ++it) {
                this->insert(spl_cast<K, K2>::cast(*it));
            }
        }

        /// Destructor
        ///
        virtual ~set() {}

        /// Insert into set
        /// @param k   Key to be added
        /// @return *this
        set<K> & add(const K & k)
        {
            this->insert(k); return *this;
        }

        /// Assignment operator (from another set)
        /// @param o set to assign from
        /// @return self
        const set<K> & operator =(const set<K> & o)
        {
            std::tr1::unordered_set<K>::operator =(o); return *this;
        }

        /// Assignment operator (from a unordered_set)
        /// @param o unordered_set to assign from
        /// @return self
        const set<K> & operator =(const std::tr1::unordered_set<K> & o)
        {
            std::tr1::unordered_set<K>::operator =(o); return *this;
        }

        /// Equality comparison
        /// @param o set to compare with
        /// @return true if the contained elements are the same, false otherwise
        bool operator ==(const set<K> & o) const
        {
            return *(std::tr1::unordered_set<K> const *)(this)==o;
        }

        /// Non-equality comparison
        /// @param o set to compare with
        /// @return true if the contained elements are not the same, false otherwise
        bool operator !=(const set<K> & o) const
        {
            return !(o==*this);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            SPL::serializeLength(buf, this->size());
            typename set<K>::const_iterator bit = this->begin();
            typename set<K>::const_iterator eit = this->end();
            for (typename set<K>::const_iterator it = bit; it != eit; ++it) {
                buf << *it;
            }
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            this->clear();
            uint32_t n = SPL::deserializeLength(buf);
            K k;
            for(uint32_t i = 0; i<n; ++i) {
                buf >> k;
                this->insert(k);
            }
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            SPL::serializeLength(buf, this->size());
            typename set<K>::const_iterator bit = this->begin();
            typename set<K>::const_iterator eit = this->end();
            for (typename set<K>::const_iterator it = bit; it != eit; ++it) {
                buf << *it;
            }
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            this->clear();
            uint32_t n = SPL::deserializeLength(buf);
            K k;
            for(uint32_t i = 0; i<n; ++i) {
                buf >> k;
                this->insert(k);
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
            typename set<K>::const_iterator bit = this->begin();
            typename set<K>::const_iterator eit = this->end();
            for (typename set<K>::const_iterator it = bit; it != eit; ++it) {
                r = 37 * r + hsk(*it);
            }
            return r;
        }

        /*! @name Reflective interfaces
         */
        //@{
        virtual bool equals(const Collection & ot) const
        {
            if (!SPL::equals(typeid(*this), typeid(ot))) {
                return false;
            }
            // the below is safe, based on our inheritance hierarchy
            set<K> const & o = static_cast<set<K> const &>(ot);
            return *(const std::tr1::unordered_set<K> *)(this)==o;
        }

        virtual Collection * clone() const
        {
            return new set<K>(*this);
        }

        virtual void assignFrom(const Collection & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_SET_COLLECTION(typeid(K).name()), SPL_TYPE_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            set<K> const & o = static_cast<set<K> const &>(ot);
            this->clear();
            (*this) = o;
        }

        virtual void swapWith(Collection & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_SET_COLLECTION(typeid(K).name()), SPL_TYPE_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            set<K> & o = static_cast<set<K> &>(ot);
            this->swap(o);
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
            typename set<K>::iterator rit = this->find(k);
            void * it = new typename set<K>::iterator(rit);
            return SetIterator(*this,it);
        }

        virtual ConstSetIterator findElement(const ConstValueHandle & key) const
        {
            const K & k = key;
            typename set<K>::iterator rit = const_cast<set<K> *>(this)->find(k);
            void * it = new typename set<K>::iterator(rit);
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
            typename set<K>::iterator it = this->find(k);
            if (it != this->end()) {
                this->erase(it);
            }
        }

        virtual void removeAllElements()
        {
            this->clear();
        }

        virtual SetIterator getBeginIterator()
        {
            typename set<K>::iterator rit = this->begin();
            void * it = new typename set<K>::iterator(rit);
            return SetIterator(*this,it);
        }

        virtual ConstSetIterator getBeginIterator() const
        {
            typename set<K>::iterator rit = const_cast<set<K> *>(this)->begin();
            void * it = new typename set<K>::iterator(rit);
            return ConstSetIterator(*this,it);
        }

        virtual SetIterator getEndIterator()
        {
            typename set<K>::iterator rit = this->end();
            void * it = new typename set<K>::iterator(rit);
            return SetIterator(*this,it);
        }

        virtual ConstSetIterator getEndIterator() const
        {
            typename set<K>::iterator rit = const_cast<set<K> *>(this)->end();
            void * it = new typename set<K>::iterator(rit);
            return ConstSetIterator(*this,it);
        }

        virtual size_t getSerializedSize() const
        {
            size_t sz = SPL::getSerializedSizeOfLength(this->size());
            typename set<K>::const_iterator bit = this->begin();
            typename set<K>::const_iterator eit = this->end();
            for (typename set<K>::const_iterator it = bit; it != eit; ++it) {
                sz += SPL::getSerializedSize(*it);
            }
            return sz;
        }
        //@}

    protected:
        const SetIterManager & getIterManager() const
        {
            return *iterManager_;
        }
    private:
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
            return new typename set<K>::iterator(*castIterator(it));
        }
        static ConstValueHandle dereferenceIterator(void * it)
        {
            return ConstValueHandle(*(*castIterator(it)));
        }
        static typename set<K>::iterator * castIterator(void * it)
        {
            return static_cast<typename set<K>::iterator *>(it);
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
    };

    template <class K>
    std::auto_ptr<SetIterManager> set<K>::iterManager_(initIterManager());

    template <class K>
    void set<K>::serialize(std::ostream & ostr) const
    {
        ostr << "{";
        typename set<K>::const_iterator bit = this->begin();
        typename set<K>::const_iterator eit = this->end();
        for(typename set<K>::const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            ostr << *it;
        }
        ostr << "}";
    }

    template <class K>
    void set<K>::serializeWithPrecision(std::ostream & ostr) const
    {
        ostr << "{";
        typename set<K>::const_iterator bit = this->begin();
        typename set<K>::const_iterator eit = this->end();
        for(typename set<K>::const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            SPL::serializeWithPrecision(ostr, *it);
        }
        ostr << "}";
    }


    template <class K>
    void set<K>::deserialize(std::istream & istr,bool withSuffix)
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
            this->insert(k);
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
            this->insert(k);
        }
    }

    template <class K>
    void set<K>::deserializeWithNanAndInfs(std::istream & istr,bool withSuffix)
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
            this->insert(k);
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
            this->insert(k);
        }
    }

    template <class K>
    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const set<K> & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    template <class K>
    inline std::ostream & operator <<(std::ostream & ostr, const set<K> & x)
    {
        x.serialize(ostr); return ostr;
    }

    template <class K>
    inline std::istream & operator >>(std::istream & istr, set<K> & x)
    {
        x.deserialize(istr); return istr;
    }

    template <class K>
    inline void deserializeWithSuffix(std::istream & istr, set<K> & x)
    {
        x.deserialize(istr,true);
    }

    template <class K>
    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & sbuf, const set<K> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K>
    inline NativeByteBuffer & operator <<(NativeByteBuffer & sbuf, const set<K> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class S, class K>
    inline ByteBuffer<S> & operator <<(ByteBuffer<S> & sbuf, const set<K> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class S, class K>
    inline ByteBuffer<S> & operator >>(ByteBuffer<S> & sbuf, set<K> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K>
    inline NativeByteBuffer & operator >>(NativeByteBuffer & sbuf, set<K> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K>
    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & sbuf, set<K> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class K>
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, const set<K> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K>
    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, set<K> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template<class K>
    struct has_bounded_set_or_map<set<K> >
    {
        enum { value = has_bounded_set_or_map<K>::value };
    };

    template<class K>
    void normalizeBoundedSetsAndMaps(set<K> & v)
    {
        if (has_bounded_set_or_map<K>::value) {
            for (typename set<K>::iterator it = v.begin(); it != v.end(); ++it) {
                normalizeBoundedSetsAndMaps(*it);
            }
        }
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */

};

#ifndef DOXYGEN_SKIP_FOR_USERS
// Hashability requirements
namespace std {
    namespace tr1 {
        template <class K>
        struct hash<SPL::set<K> >
        {
            inline size_t operator ()(const SPL::set<K> & s) const
            {
                return s.hashCode();
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_SET_H */
