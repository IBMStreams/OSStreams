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

#ifndef SPL_RUNTIME_TYPE_MAP_H
#define SPL_RUNTIME_TYPE_MAP_H

/*!
 * \file Map.h \brief Definition of the SPL::map class.
 */

#include <SPL/Runtime/Type/BMap.h>

PUSH_DLL_PUBLIC
namespace SPL
{
    /// \class std::tr1::unordered_map

    /// \brief Class that represents a map type (extends from std::tr1::unordered_map).
    /// \extends std::tr1::unordered_map
    template <class K, class V>
    class map : public Map
#ifndef DOXYGEN_SKIP_FOR_USERS
    // doxygen does not pickup overloaded functions with multiple inheritance
              , public std::tr1::unordered_map<K, V>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
    {
    public:
        /// Default constructor
        ///
        explicit map() : std::tr1::unordered_map<K,V>() {}

        /// Construct from a unordered_map
        /// @param x unordered_map to use
        map(const std::tr1::unordered_map<K,V> & x) : std::tr1::unordered_map<K,V>(x) {}

        /// Copy constructor
        /// @param x map to construct from
        map(const map<K,V> & x) : std::tr1::unordered_map<K,V>(x) {}

        /// Construct from a sequence
        /// @param first begin iterator, inclusive
        /// @param last end iterator, exclusive
        template <class InputIterator>
        map(InputIterator first, InputIterator last) : std::tr1::unordered_map<K,V>(first,last) {}

        /// Construct from a reflective map
        /// @param o reflective map to construct from
        map(const Map & o)
        {
            assignFrom(o);
        }

        /// Construct from a value handle
        /// @param o value handle to construct from
        map(const ConstValueHandle & o)
        {
            Map const & x = o; assignFrom(x);
        }

        /// Construct from an assignable map
        /// @param o map to construct from
        template <class K2, class V2>
        map(const map<K2,V2> & o)
        {
            typename map<K2,V2>::const_iterator it;
            for (it = o.begin(); it != o.end(); it++) {
                this->insert(std::make_pair(spl_cast<K, K2>::cast(it->first),
                                            spl_cast<V, V2>::cast(it->second)));
            }
        }

        /// Construct from an assignable bmap
        /// @param o map to construct from
        template <class K2, class V2, int32_t msize>
        map(const bmap<K2,V2,msize> & o)
        {
            typename bmap<K2,V2,msize>::const_iterator bit = o.begin();
            typename bmap<K2,V2,msize>::const_iterator eit = o.end();
            for (typename bmap<K2, V2, msize>::const_iterator it = bit; it != eit; it++) {
                this->insert(std::make_pair(spl_cast<K, K2>::cast(it->first),
                                            spl_cast<V, V2>::cast(it->second)));
            }
        }

        /// Destructor
        ///
        virtual ~map() {}

        /// Append to list
        /// @param k key to be added
        /// @param v value to be added
        /// @return *this
        map<K,V> & add(const K & k, const V & v)
        {
            this->insert(std::make_pair(k,v)); return *this;
        }

        /// Assignment operator (from another map)
        /// @param o map to assign from
        /// @return self
        const map<K,V> & operator =(const map<K,V> & o)
        {
            std::tr1::unordered_map<K,V>::operator =(o); return *this;
        }

        /// Assignment operator (from a unordered_map)
        /// @param o unordered_map to assign from
        /// @return self
        const map<K,V> & operator =(const std::tr1::unordered_map<K,V> & o)
        {
            std::tr1::unordered_map<K,V>::operator =(o); return *this;
        }

        /// Equality comparison
        /// @param o map to compare with
        /// @return true if the contained elements are the same, false otherwise
        bool operator ==(const map<K,V> & o) const
        {
            return *(std::tr1::unordered_map<K,V> const *)(this)==o;
        }

        /// Non-equality comparison
        /// @param o map to compare with
        /// @return true if the contained elements are not the same, false otherwise
        bool operator !=(const map<K,V> & o) const
        {
            return !(o==*this);
        }

        /// Get the element with a given key, create it if not present
        /// @param k search key
        /// @return reference to the element
        /// @throws SPLRuntimeInvalidIndexException if the key is not found in
        /// the map
        V & at(const K & k)
        {
            typename map<K,V>::iterator it = this->find(k);
            if(it == this->end()) {
                typedef SPL::spl_cast<SPL::rstring,K> Converter;
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR, SPL_APPLICATION_RUNTIME_MAP_INVALID_INDEX(Converter::cast(k)), SPL_TYPE_DBG);
            }
            return (*it).second;
        }

        /// Get the element with a given key, throw exception if not present
        /// @param k search key
        /// @return constant reference to the element
        const V & at(const K & k) const
        {
            typename map<K,V>::const_iterator it = this->find(k);
            if(it == this->end()) {
                typedef SPL::spl_cast<SPL::rstring,K> Converter;
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR, SPL_APPLICATION_RUNTIME_MAP_INVALID_INDEX(Converter::cast(k)), SPL_TYPE_DBG);
            }
            return (*it).second;
        }

        /// Get the element with a given key, create it if not present
        /// @param k search key
        /// @return reference to the element
        V & operator [](const K & k)
        {
            typename map<K,V>::iterator it = this->find(k);
            if(it == this->end()) {
                const V v = V();
                it = this->insert(std::make_pair(k, v)).first;
            }
            return (*it).second;
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            SPL::serializeLength(buf, this->size());
            typename map<K,V>::const_iterator bit = this->begin();
            typename map<K,V>::const_iterator eit = this->end();
            for(typename map<K,V>::const_iterator it = bit; it!=eit; ++it) {
                buf << it->first; buf << it->second;
            }
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            this->clear();
            uint32_t n = SPL::deserializeLength(buf);
            K k; V v;
            for(uint32_t i = 0; i<n; ++i) {
                buf >> k; buf >> v;
                this->insert(std::make_pair(k,v));
            }
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            SPL::serializeLength(buf, this->size());
            typename map<K,V>::const_iterator bit = this->begin();
            typename map<K,V>::const_iterator eit = this->end();
            for(typename map<K,V>::const_iterator it = bit; it!=eit; ++it) {
                buf << it->first; buf << it->second;
            }
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            this->clear();
            int32_t n = SPL::deserializeLength(buf);
            K k; V v;
            for(int32_t i = 0; i<n; ++i) {
                buf >> k; buf >> v;
                this->insert(std::make_pair(k,v));
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
            typename map<K,V>::const_iterator bit = this->begin();
            typename map<K,V>::const_iterator eit = this->end();
            for(typename map<K,V>::const_iterator it = bit; it!=eit; ++it) {
                r = 37 * r + hsk(it->first);
                r = 37 * r + hsv(it->second);
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
            map<K,V> const & o = static_cast<map<K,V> const &>(ot);
            return *(const std::tr1::unordered_map<K,V> *)(this)==o;
        }

        virtual Collection * clone() const
        {
            return new map<K,V>(*this);
        }

        virtual void assignFrom(const Collection & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_MAP_COLLECTION(typeid(K).name(), typeid(V).name()), SPL_TYPE_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            map<K,V> const & o = static_cast<map<K,V> const &>(ot);
            this->clear();
            (*this) = o;
        }

        virtual void swapWith(Collection & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_MAP_COLLECTION(typeid(K).name(), typeid(V).name()), SPL_TYPE_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            map<K,V> & o = static_cast<map<K,V> &>(ot);
            this->swap(o);
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
            typename map<K,V>::iterator rit = this->find(k);
            void * it = new typename map<K,V>::iterator(rit);
            return MapIterator(*this,it);
        }

        virtual ConstMapIterator findElement(const ConstValueHandle & key) const
        {
            const K & k = key;
            typename map<K,V>::iterator rit = const_cast<map<K,V> *>(this)->find(k);
            void * it = new typename map<K,V>::iterator(rit);
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
            typename map<K,V>::iterator it = this->find(k);
            if (it != this->end()) {
                this->erase(it);
            }
        }

        virtual void removeAllElements()
        {
            this->clear();
        }

        virtual MapIterator getBeginIterator()
        {
            typename map<K,V>::iterator rit = this->begin();
            void * it = new typename map<K,V>::iterator(rit);
            return MapIterator(*this,it);
        }

        virtual ConstMapIterator getBeginIterator() const
        {
            typename map<K,V>::iterator rit = const_cast<map<K,V> *>(this)->begin();
            void * it = new typename map<K,V>::iterator(rit);
            return ConstMapIterator(*this,it);
        }

        virtual MapIterator getEndIterator()
        {
            typename map<K,V>::iterator rit = this->end();
            void * it = new typename map<K,V>::iterator(rit);
            return MapIterator(*this,it);
        }

        virtual ConstMapIterator getEndIterator() const
        {
            typename map<K,V>::iterator rit = const_cast<map<K,V> *>(this)->end();
            void * it = new typename map<K,V>::iterator(rit);
            return ConstMapIterator(*this,it);
        }

        virtual size_t getSerializedSize() const
        {
            size_t sz = SPL::getSerializedSizeOfLength(this->size());
            typename map<K,V>::const_iterator bit = this->begin();
            typename map<K,V>::const_iterator eit = this->end();
            for(typename map<K,V>::const_iterator it = bit; it!=eit; ++it) {
                sz += SPL::getSerializedSize(it->first);
                sz += SPL::getSerializedSize(it->second);
            }
            return sz;
        }
        //@}

    protected:
        const MapIterManager & getIterManager() const
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
            return new typename map<K,V>::iterator(*castIterator(it));
        }
        static std::pair<ConstValueHandle,ValueHandle> dereferenceIterator(void * it)
        {
            return std::make_pair(ConstValueHandle((*castIterator(it))->first),
                                  ValueHandle((*castIterator(it))->second));
        }
        static typename map<K,V>::iterator * castIterator(void * it)
        {
            return static_cast<typename map<K,V>::iterator *>(it);
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
    };

    template <class K, class V>
    std::auto_ptr<MapIterManager> map<K,V>::iterManager_(initIterManager());

    template <class K, class V>
    void map<K,V>::serialize(std::ostream & ostr) const
    {
        ostr << "{";
        typename map<K,V>::const_iterator bit = this->begin();
        typename map<K,V>::const_iterator eit = this->end();
        for(typename map<K,V>::const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            ostr << it->first;
            ostr << ":";
            ostr << it->second;
        }
        ostr << "}";
    }

    template <class K, class V>
    void map<K,V>::serializeWithPrecision(std::ostream & ostr) const
    {
        ostr << "{";
        typename map<K,V>::const_iterator bit = this->begin();
        typename map<K,V>::const_iterator eit = this->end();
        for(typename map<K,V>::const_iterator it = bit; it!=eit; ++it) {
            if (it != bit) {
                ostr << ",";
            }
            SPL::serializeWithPrecision(ostr, it->first);
            ostr << ":";
            SPL::serializeWithPrecision(ostr, it->second);
        }
        ostr << "}";
    }

    template <class K, class V>
    void map<K,V>::deserialize(std::istream & istr,bool withSuffix)
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
            this->insert(std::make_pair(k,v));
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
            this->insert(std::make_pair(k,v));
        }
    }

    template <class K, class V>
    void map<K,V>::deserializeWithNanAndInfs(std::istream & istr,bool withSuffix)
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
            istr >> c;
            if (!istr) {
                return;
            }
            if(c!=':') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            V v;
            SPL::deserializeWithNanAndInfs(istr,v, withSuffix);
            if (!istr) {
                return;
            }
            this->insert(std::make_pair(k,v));
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
            istr >> c;
            if (!istr) {
                return;
            }
            if(c!=':') {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            V v;
            SPL::deserializeWithNanAndInfs(istr,v, withSuffix);
            if (!istr) {
                return;
            }
            this->insert(std::make_pair(k,v));
        }
    }

    template <class K, class V>
    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const map<K,V> & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    template <class K, class V>
    inline std::ostream & operator <<(std::ostream & ostr, const map<K,V> & x)
    {
        x.serialize(ostr); return ostr;
    }

    template <class K, class V>
    inline std::istream & operator >>(std::istream & istr, map<K,V> & x)
    {
        x.deserialize(istr); return istr;
    }

    template <class K, class V>
    inline void deserializeWithSuffix(std::istream & istr, map<K,V> & x)
    {
        x.deserialize(istr,true);
    }

    template <class K, class V>
    inline void deserializeWithNanAndInfs(std::istream & istr, map<K,V> & x, bool withSuffix = false)
    {
        x.deserializeWithNanAndInfs(istr,withSuffix);
    }

    template <class K, class V>
    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & sbuf, const map<K,V> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, class V>
    inline NativeByteBuffer & operator <<(NativeByteBuffer & sbuf, const map<K,V> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class S, class K, class V>
    inline ByteBuffer<S> & operator <<(ByteBuffer<S> & sbuf, const map<K,V> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class S, class K, class V>
    inline ByteBuffer<S> & operator >>(ByteBuffer<S> & sbuf, map<K,V> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K, class V>
    inline NativeByteBuffer & operator >>(NativeByteBuffer & sbuf, map<K,V> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class K, class V>
    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & sbuf, map<K,V> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class K, class V>
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, const map<K,V> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class K, class V>
    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, map<K,V> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template<class K, class V>
    struct has_bounded_set_or_map<map<K,V> >
    {
        enum { value = has_bounded_set_or_map<K>::value ||
               has_bounded_set_or_map<V>::value };
    };

    template<class K, class V>
    void normalizeBoundedSetsAndMaps(map<K,V> & v)
    {
        if (has_bounded_set_or_map<K>::value) {
            for (typename map<K, V>::iterator it = v.begin(); it != v.end(); ++it) {
                normalizeBoundedSetsAndMaps(it->first);
            }
        }
        if (has_bounded_set_or_map<V>::value) {
            for (typename map<K, V>::iterator it = v.begin(); it != v.end(); ++it) {
                normalizeBoundedSetsAndMaps(it->second);
            }
        }
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */
};

#ifndef DOXYGEN_SKIP_FOR_USERS
// Hashability requirements
namespace std {
    namespace tr1 {
        template <class K, class V>
        struct hash<SPL::map<K,V> >
        {
            inline size_t operator ()(const SPL::map<K,V> & m) const
            {
                return m.hashCode();
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_MAP_H */
