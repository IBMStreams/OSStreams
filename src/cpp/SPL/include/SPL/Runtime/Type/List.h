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

#ifndef SPL_RUNTIME_TYPE_LIST_H
#define SPL_RUNTIME_TYPE_LIST_H

/*!
 * \file List.h \brief Definition of the SPL::list class.
 */

#include <SPL/Runtime/Type/BList.h>

PUSH_DLL_PUBLIC
namespace SPL
{
    /// \brief Class that represents a list type (extends from std::vector).
    /// \extends std::vector
    template <class T>
    class list : public List
#ifndef DOXYGEN_SKIP_FOR_USERS
    /* Note: SPL::Functions::Math::median() depends on SPL::list
       iterator being random access (as provided by std::vector). */
    // doxygen does not pickup overloaded functions with multiple inheritance
               , public std::vector<T>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
    {
    public:
        /// Default constructor
        ///
        explicit list() : std::vector<T>() {}

        /// Construct using a value
        /// @param n number of elements
        /// @param v value to use
        explicit list(typename list<T>::size_type n, const T & v = T()) : std::vector<T>(n,v) {}

        /// Construct from an STL vector
        /// @param x STL vector to use
        list(const std::vector<T> & x) : std::vector<T>(x) {}

        /// Copy constructor
        /// @param x list to construct from
        list(const list<T> & x) : std::vector<T>(x) {}

        /// Construct from a sequence
        /// @param first begin iterator, inclusive
        /// @param last end iterator, exclusive
        template <class InputIterator>
        list(InputIterator first, InputIterator last) : std::vector<T>(first,last) {}

        /// Construct from a value handle
        /// @param o value handle to construct from
        list(const ConstValueHandle & o)
        {
            List const & x = o; assignFrom(x);
        }

        /// Construct from an assignable list
        /// @param o list to construct from
        template <class T2>
        list(const list<T2> & o)
        {
            uint32_t count = o.size();
            for (uint32_t i = 0; i < count; i++) {
                this->push_back(spl_cast<T, T2>::cast(o[i]));
            }
        }

        /// Construct from an assignable blist
        /// @param o list to construct from
        template <class T2, int32_t msize>
        list(const blist<T2,msize> & o)
        {
            uint32_t count = o.size();
            for (uint32_t i = 0; i < count; i++) {
                this->push_back(spl_cast<T, T2>::cast(o[i]));
            }
        }

        /// Construct from a reflective list
        /// @param o reflective list to construct from
        list(const List & o)
        {
            assignFrom(o);
        }

        /// Destructor
        ///
        virtual ~list() {}

        /// Subscript the list (read-only)
        /// @param idx element index
        /// @return element at index idx
        /// @throws SPLRuntimeInvalidIndexException exception if the index is out of range
        typename std::vector<T>::const_reference at (typename std::vector<T>::size_type idx) const
        {
            if (idx >= this->size()) {
                indexOutOfBound(idx, this->size());
            }
            return this->operator[] (idx);
        }

        /// Subscript the list (read-write)
        /// @param idx element index
        /// @return element at index idx
        /// @throws SPLRuntimeInvalidIndexException exception if the index is out of range
        typename std::vector<T>::reference at (typename std::vector<T>::size_type idx)
        {
            if (idx >= this->size()) {
                indexOutOfBound(idx, this->size());
            }
            return this->operator[] (idx);
        }

        /// Append to list
        /// @param v   Value to be added
        /// @return *this
        list<T> & add(const T & v)
        {
            this->push_back(v); return *this;
        }

        /// Assignment operator (from another list)
        /// @param o list to assign from
        /// @return self
        const list<T> & operator =(const list<T> & o)
        {
            std::vector<T>::operator =(o); return *this;
        }

        /// Assignment operator (from an STL vector)
        /// @param o STL list to assign from
        /// @return self
        const list<T> & operator =(const std::vector<T> & o)
        {
            std::vector<T>::operator =(o); return *this;
        }

        /// Equality comparison
        /// @param o list to compare with
        /// @return true if the contained elements are the same, false otherwise
        bool operator ==(const list<T> & o) const
        {
            return *(std::vector<T> const *)(this)==o;
        }

        /// Non-equality comparison
        /// @param ov list to compare with
        /// @return true if the contained elements are not the same, false otherwise
        bool operator !=(const list<T> & ov) const
        {
            return !(ov==*this);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            SPL::serializeLength(buf, this->size());
            for (size_t i = 0, iu = this->size(); i < iu; ++i) {
                buf << this->operator[](i);
            }
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            this->clear();
            int32_t n = SPL::deserializeLength(buf);
            this->reserve(n);
            T x;
            for(int32_t i = 0; i<n; ++i) {
                buf >> x;
                this->push_back(x);
            }
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            size_t u = this->size();
            SPL::serializeLength(buf, u);
            ListSerializer<T,BufferType>::serialize(buf, *this, u);
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            int32_t n = SPL::deserializeLength(buf);
            ListSerializer<T,BufferType>::deserialize(buf, *this, n);
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
            std::tr1::hash<T> hs;
            for (int32_t i = 0, iu = this->getSize(); i < iu; ++i) {
                r = 37 * r + hs(this->operator[](i));
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
            list<T> const & o = static_cast<list<T> const &>(ot);
            return *(const std::vector<T> *)(this)==o;
        }

        virtual Collection * clone() const
        {
            return new list<T>(*this);
        }

        virtual void assignFrom(const Collection & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_LIST_COLLECTION(typeid(T).name()), SPL_TYPE_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            list<T> const & o = static_cast<list<T> const &>(ot);
            this->clear();
            (*this) = o;
        }

        virtual void swapWith(Collection & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_LIST_COLLECTION(typeid(T).name()), SPL_TYPE_DBG);
            }
            // the below is safe, based on our inheritance hierarchy
            list<T> & o = static_cast<list<T> &>(ot);
            this->swap(o);
        }

        virtual Meta::Type getElementMetaType() const
        {
            return Meta::Type::typeOf<T>();
        }

        virtual ValueHandle createElement() const
        {
            return ValueHandle(*(new T()));
        }

        virtual int32_t getSize() const
        {
            return this->size();
        }

        virtual ValueHandle getElement(const size_t & i)
        {
            return ValueHandle(this->operator [](i));
        }

        virtual ConstValueHandle getElement(const size_t & i) const
        {
            return ConstValueHandle(((list<T> const *) this)->operator [](i));
        }

        virtual ValueHandle getElement(const ConstListIterator & it)
        {
            return ValueHandle(this->operator [](it.index()));
        }

        virtual ConstValueHandle getElement(const ConstListIterator & it) const
        {
            return ConstValueHandle(((list<T> const *) this)->operator [](it.index()));
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
            size_t sz = SPL::getSerializedSizeOfLength(this->size());
            for (size_t i = 0, iu = this->size(); i < iu; ++i) {
                sz += SPL::getSerializedSize(this->operator[](i));
            }
            return sz;
        }
        //@}
    };

    template <class T>
    void list<T>::serialize(std::ostream & ostr) const
    {
        ostr << "[";
        for(size_t i = 0,iu = this->size(); i<iu; ++i) {
            if (i != 0) {
                ostr << ",";
            }
            ostr << this->operator [](i);
        }
        ostr << "]";
    }

    template <class T>
    void list<T>::serializeWithPrecision(std::ostream & ostr) const
    {
        ostr << "[";
        for(size_t i = 0,iu = this->size(); i<iu; ++i) {
            if (i != 0) {
                ostr << ",";
            }
            SPL::serializeWithPrecision(ostr, this->operator [](i));
        }
        ostr << "]";
    }

    template <class T>
    void list<T>::deserialize(std::istream & istr,bool withSuffix)
    {
        this->clear();
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
            this->push_back(v);
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
            this->push_back(v);
        }
    }

    template <class T>
    void list<T>::deserializeWithNanAndInfs(std::istream & istr,bool withSuffix)
    {
        this->clear();
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
            SPL::deserializeWithNanAndInfs(istr,v, withSuffix);
            if (!istr) {
                return;
            }
            this->push_back(v);
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
            SPL::deserializeWithNanAndInfs(istr,v, withSuffix);
            if (!istr) {
                return;
            }
            this->push_back(v);
        }
    }


    template <class T>
    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const list<T> & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    template <class T>
    inline std::ostream & operator <<(std::ostream & ostr, const list<T> & x)
    {
        x.serialize(ostr); return ostr;
    }

    template <class T>
    inline std::istream & operator >>(std::istream & istr, list<T> & x)
    {
        x.deserialize(istr); return istr;
    }

    template <class T>
    inline void deserializeWithSuffix(std::istream & istr, list<T> & x)
    {
        x.deserialize(istr,true);
    }

    template <class T>
    inline void deserializeWithNanAndInfs(std::istream & istr, list<T> & x, bool withSuffix = false)
    {
        x.deserializeWithNanAndInfs(istr,withSuffix);
    }

    template <class S, class T>
    inline ByteBuffer<S> & operator <<(ByteBuffer<S> & sbuf, const list<T> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class T>
    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & sbuf, const list<T> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class T>
    inline NativeByteBuffer & operator <<(NativeByteBuffer & sbuf, const list<T> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class S, class T>
    inline ByteBuffer<S> & operator >>(ByteBuffer<S> & sbuf, list<T> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class T>
    inline NativeByteBuffer & operator >>(NativeByteBuffer & sbuf, list<T> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template <class T>
    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & sbuf, list<T> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class T>
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, const list<T> & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    template <class T>
    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, list<T> & x)
    {
        x.deserialize(sbuf); return sbuf;
    }

    template<class T>
    struct has_bounded_set_or_map<list<T> >
    {
        enum { value = has_bounded_set_or_map<T>::value };
    };

    template<class T>
    void normalizeBoundedSetsAndMaps(list<T> & v)
    {
        if (has_bounded_set_or_map<T>::value) {
            for (typename list<T>::iterator it = v.begin(); it != v.end(); ++it) {
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
        template <class T>
        struct hash<SPL::list<T> >
        {
            inline size_t operator ()(const SPL::list<T> & l) const
            {
                return l.hashCode();
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_LIST_H */
