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

#ifndef SPL_RUNTIME_TYPE_TMP_TUPLE_H
#define SPL_RUNTIME_TYPE_TMP_TUPLE_H

#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Type/Tuple.h>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#include <cxxabi.h>
#include <typeinfo>

#define SPL_TUPLE_MAX_ATTRIBUTES 10

// class Type0, class Atrb0, class Type1, class Atrb1, ...
#define SPL_TUPLE_TEMPLATE_DEFS                                                                    \
    class Type0,                                                                                   \
      class Atrb0 BOOST_PP_REPEAT_FROM_TO(1, SPL_TUPLE_MAX_ATTRIBUTES, SPL_TUPLE_TYPE_ATTR_C, 0)

// class Type0=null_type, class Atrb0=null_type, class Type1=null_type, class Atrb1=null_type, ...
#define SPL_TUPLE_DEFAULTED_TEMPLATE_DEFS                                                          \
    class Type0 = null_type, class Atrb0 = null_type BOOST_PP_REPEAT_FROM_TO(                      \
                               1, SPL_TUPLE_MAX_ATTRIBUTES, SPL_TUPLE_TYPE_ATTR_CI, 0)

// Type0, Type1, ...
#define SPL_TUPLE_TYPE_TEMPLATES BOOST_PP_ENUM_PARAMS(SPL_TUPLE_MAX_ATTRIBUTES, Type)

// Atrb0, Atrb1, ...
#define SPL_TUPLE_ATRB_TEMPLATES BOOST_PP_ENUM_PARAMS(SPL_TUPLE_MAX_ATTRIBUTES, Atrb)

// Type0, Atrb0, Type1, Atrb1, ...
#define SPL_TUPLE_TEMPLATES                                                                        \
    Type0, Atrb0 BOOST_PP_REPEAT_FROM_TO(1, SPL_TUPLE_MAX_ATTRIBUTES, SPL_TUPLE_TYPE_ATTR, 0)

#define SPL_TUPLE_TYPE_ATTR(z, index, data) , Type##index, Atrb##index
#define SPL_TUPLE_TYPE_ATTR_C(z, index, data) , class Type##index, class Atrb##index
#define SPL_TUPLE_TYPE_ATTR_CI(z, index, data)                                                     \
    , class Type##index = null_type, class Atrb##index = null_type

// macro for tuple attribute names
#define FIELD_TYPE(x) struct __spl_##x*
// macro for use in a definition: tuple<rstring FIELD(name)> t;
#define FIELD(x) , FIELD_TYPE(x)
// macro for use in an acessor: t.getFIELD(name) = "a";
#define getFIELD(x) get<FIELD_TYPE(x)>()
#define MAKE_SPL_TUPLE_FIELD(x)                                                                    \
    struct __spl_##x                                                                               \
    {};

namespace SPL {

typedef boost::tuples::null_type null_type;
template<SPL_TUPLE_DEFAULTED_TEMPLATE_DEFS>
class tuple
  : public Tuple
  , public boost::tuple<SPL_TUPLE_TYPE_TEMPLATES>
{
    typedef SPL::tuple<SPL_TUPLE_TEMPLATES> tuple_type;
    typedef boost::tuple<SPL_TUPLE_TYPE_TEMPLATES> boost_tuple_type;
    typedef boost::mpl::vector<SPL_TUPLE_ATRB_TEMPLATES> name_vector;
    enum
    {
        num_elements = boost::tuples::length<tuple_type>::value
    };

  public:
    /// Empty constructor
    explicit tuple()
      : Tuple()
      , boost_tuple_type()
    {}

    tuple(const boost_tuple_type& x)
      : Tuple()
      , boost_tuple_type(x)
    {}

    /// Copy constructor
    /// @param x tuple to construct from
    tuple(const tuple_type& x)
      : Tuple()
      , boost_tuple_type(x)
    {
        constructPayload(x);
    }

    /// Construct from a reflective tuple
    /// @param o reflective tuple to construct from
    tuple(const Tuple& o)
      : Tuple()
    {
        assignFrom(o);
    }

    /// Construct from a value handle
    /// @param o value handle to construct from
    tuple(const ConstValueHandle& o)
      : Tuple()
    {
        Tuple const& x = o;
        assignFrom(x);
    }

    /// Destructor
    virtual ~tuple() {}

    /// Get the value of the named attribute (read-only)
    /// @return a const reference to the value of the named attribute
    template<class Atrbn>
    typename boost::tuples::element<boost::mpl::find<name_vector, Atrbn>::type::pos::value,
                                    tuple_type>::type const&
    get() const
    {
        return boost::get<boost::mpl::find<name_vector, Atrbn>::type::pos::value>(*this);
    }

    /// Get the value of the named attribute
    /// @return a reference to the value of the named attribute
    template<class Atrbn>
    typename boost::tuples::element<boost::mpl::find<name_vector, Atrbn>::type::pos::value,
                                    tuple_type>::type&
    get()
    {
        return boost::get<boost::mpl::find<name_vector, Atrbn>::type::pos::value>(*this);
    }

    /// Get the value of the attribute at the given index
    /// @return a reference to the value of the named attribute
    template<uint32_t n>
    typename boost::tuples::element<n, tuple_type>::type const& at() const
    {
        return boost::get<n>(*this);
    }

    /// Get the value of the attribute at the given index (read-only)
    /// @return a reference to the value of the attribute at the given index
    template<uint32_t n>
    typename boost::tuples::element<n, tuple_type>::type& at()
    {
        return boost::get<n>(*this);
    }

    /// Assignment operator (from another tuple)
    /// @param o tuple to assign from
    /// @return self
    const tuple_type& operator=(const tuple_type& o)
    {
        boost_tuple_type::operator=(o);
        assignPayload(o);
        return *this;
    }

    /// Assignment operator (from any other tuple)
    /// @param o tuple to assign from
    /// @return self
    const tuple_type& operator=(const Tuple& o)
    {
        this->assignFrom(o);
        return *this;
    }

    /// Assignment operator (from a tuple of same type)
    /// @param o tuple to assign from
    /// @return self
    void assign(const Tuple& o) { *this = static_cast<tuple_type const&>(o); }

    /// Equality comparison
    /// @param o tuple to compare with
    /// @return true if the contained attributes are the same, false otherwise
    bool operator==(const tuple_type& o) const { return *(boost_tuple_type const*)(this) == o; }

    /// Non-equality comparison
    /// @param o tuple to compare with
    /// @return true if the contained attributes are not the same, false otherwise
    bool operator!=(const tuple_type& o) const { return !(o == *this); }

    // same values, different attributes
    bool operator==(const boost_tuple_type& o) const { return false; }
    bool operator!=(const boost_tuple_type& o) const { return true; }

    // BEGIN reflective interfaces
    bool equals(const Tuple& ot) const
    {
        if (typeid(*this) != typeid(ot)) {
            return false;
        }
        // the below is safe, based on our inheritance hierarchy
        tuple_type const& o = static_cast<tuple_type const&>(ot);
        return (*this == o);
    }
    Tuple* clone() const { return new tuple_type(*this); }

    uint32_t getNumberOfAttributes() const { return num_elements; }

    void reset() {}

    TupleIterator getBeginIterator() { return TupleIterator(*this, 0); }

    ConstTupleIterator getBeginIterator() const { return ConstTupleIterator(*this, 0); }

    TupleIterator getEndIterator() { return TupleIterator(*this, num_elements); }

    ConstTupleIterator getEndIterator() const { return ConstTupleIterator(*this, num_elements); }

    TupleIterator findAttribute(const std::string& attrb)
    {
        std::tr1::unordered_map<std::string, uint32_t>::const_iterator it =
          mappings_->nameToIndex_.find(attrb);
        if (it == mappings_->nameToIndex_.end()) {
            return this->getEndIterator();
        }
        return TupleIterator(*this, it->second);
    }

    ConstTupleIterator findAttribute(const std::string& attrb) const
    {
        return const_cast<tuple_type*>(this)->findAttribute(attrb);
    }

    TupleAttribute getAttribute(uint32_t index)
    {
        return TupleAttribute(mappings_->indexToName_[index], index, *this);
    }

    ConstTupleAttribute getAttribute(uint32_t index) const
    {
        return const_cast<tuple_type*>(this)->getAttribute(index);
    }

    ConstValueHandle getAttributeValue(const std::string& attrb) const
    {
        return const_cast<tuple_type*>(this)->getAttributeValueRaw(
          mappings_->nameToIndex_.find(attrb)->second);
    }

    ValueHandle getAttributeValue(const std::string& attrb)
    {
        return this->getAttributeValueRaw(mappings_->nameToIndex_[attrb]);
    }

    ConstValueHandle getAttributeValue(const uint32_t index) const
    {
        return const_cast<tuple_type*>(this)->getAttributeValueRaw(index);
    }

    ValueHandle getAttributeValue(const uint32_t index)
    {
        return this->getAttributeValueRaw(index);
    }
    // END reflective interfaces

    /// Serialize (binary)
    /// @param buf serialization buffer to use
    void serialize(VirtualByteBuffer& buf) const { serializeToSerializationBuffer(buf, *this); }

    /// Deserialize (binary)
    /// @param buf serialization buffer to use
    void deserialize(VirtualByteBuffer& buf) { deserializeFromSerializationBuffer(buf, *this); }

    /// Serialize (binary)
    /// @param buf byte buffer to use
    template<class BufferType>
    void serialize(ByteBuffer<BufferType>& buf) const
    {
        serializeToSerializationBuffer(buf, *this);
    }

    /// Serialize (binary)
    /// @param buf byte buffer to use
    void serialize(NetworkByteBuffer& buf) const { serializeToSerializationBuffer(buf, *this); }

    /// Serialize (binary)
    /// @param buf byte buffer to use
    void serialize(NativeByteBuffer& buf) const { serializeToSerializationBuffer(buf, *this); }

    /// Deserialize (binary)
    /// @param buf byte buffer to use
    template<class BufferType>
    void deserialize(ByteBuffer<BufferType>& buf)
    {
        deserializeFromSerializationBuffer(buf, *this);
    }

    /// Deserialize (binary)
    /// @param buf byte buffer to use
    void deserialize(NetworkByteBuffer& buf) { deserializeFromSerializationBuffer(buf, *this); }

    /// Deserialize (binary)
    /// @param buf byte buffer to use
    void deserialize(NativeByteBuffer& buf) { deserializeFromSerializationBuffer(buf, *this); }

    /// Serialize (character)
    /// @param ostr output stream
    void serialize(std::ostream& ostr) const;

    /// Serialize (character), with maximum precision for floats
    /// @param ostr output stream
    void serializeWithPrecision(std::ostream& ostr) const;

    /// Deserialize (character)
    /// @param istr input stream
    /// @param withSuffix  if true then consume any suffix, otherwise (false)
    /// assume no suffix is present
    void deserialize(std::istream& istr, bool withSuffix = false);

    /// Deserialize with Inf and Nan (character)
    /// @param istr input stream
    /// @param withSuffix if true then consume suffix, otherwise (false)
    /// assume no suffix is present
    void deserializeWithNanAndInfs(std::istream& istr, bool withSuffix = false);

    /// Swap
    /// @param ot tuple to swap with
    void swap(tuple_type& ot)
    {
        swapAttribute(*this, ot);
        std::swap(payload_, ot.payload_);
    }

    /// Get hash code
    /// @return hash code
    size_t hashCode() const { return computeHash(*this); }

  protected:
    const std::string& getAttributeName(uint32_t index) const
    {
        return mappings_->indexToName_[index];
    }

    const std::tr1::unordered_map<std::string, uint32_t>& getAttributeNames() const
    {
        return mappings_->nameToIndex_;
    }

  private:
    template<class H, class T>
    void serializeToSerializationBuffer(VirtualByteBuffer& buf,
                                        const boost::tuples::cons<H, T>& t) const
    {
        buf << t.get_head();
        serializeToSerializationBuffer(buf, t.get_tail());
    }
    template<class S, class H, class T>
    void serializeToSerializationBuffer(ByteBuffer<S>& buf,
                                        const boost::tuples::cons<H, T>& t) const
    {
        buf << t.get_head();
        serializeToSerializationBuffer(buf, t.get_tail());
    }
    template<class H, class T>
    void serializeToSerializationBuffer(NetworkByteBuffer& buf,
                                        const boost::tuples::cons<H, T>& t) const
    {
        buf << t.get_head();
        serializeToSerializationBuffer(buf, t.get_tail());
    }
    template<class H, class T>
    void serializeToSerializationBuffer(NativeByteBuffer& buf,
                                        const boost::tuples::cons<H, T>& t) const
    {
        buf << t.get_head();
        serializeToSerializationBuffer(buf, t.get_tail());
    }
    void serializeToSerializationBuffer(VirtualByteBuffer& buf, const null_type&) const {}
    template<class S>
    void serializeToSerializationBuffer(ByteBuffer<S>& buf, const null_type&) const
    {}
    void serializeToSerializationBuffer(NetworkByteBuffer& buf, const null_type&) const {}
    void serializeToSerializationBuffer(NativeByteBuffer& buf, const null_type&) const {}
    template<class H, class T>
    void deserializeFromSerializationBuffer(VirtualByteBuffer& buf, boost::tuples::cons<H, T>& t)
    {
        buf >> t.get_head();
        deserializeFromSerializationBuffer(buf, t.get_tail());
    }
    template<class S, class H, class T>
    void deserializeFromSerializationBuffer(ByteBuffer<S>& buf, boost::tuples::cons<H, T>& t)
    {
        buf >> t.get_head();
        deserializeFromSerializationBuffer(buf, t.get_tail());
    }
    template<class H, class T>
    void deserializeFromSerializationBuffer(NetworkByteBuffer& buf, boost::tuples::cons<H, T>& t)
    {
        buf >> t.get_head();
        deserializeFromSerializationBuffer(buf, t.get_tail());
    }
    template<class H, class T>
    void deserializeFromSerializationBuffer(NativeByteBuffer& buf, boost::tuples::cons<H, T>& t)
    {
        buf >> t.get_head();
        deserializeFromSerializationBuffer(buf, t.get_tail());
    }
    void deserializeFromSerializationBuffer(VirtualByteBuffer& buf, const null_type&) {}
    template<class S>
    void deserializeFromSerializationBuffer(ByteBuffer<S>& buf, const null_type&)
    {}
    void deserializeFromSerializationBuffer(NetworkByteBuffer& buf, const null_type&) {}
    void deserializeFromSerializationBuffer(NativeByteBuffer& buf, const null_type&) {}
    template<class H, class T>
    void serializeToCharStream(std::ostream& ostr,
                               const boost::tuples::cons<H, T>& t,
                               const int idx) const;
    void serializeToCharStream(std::ostream& ostr, const null_type&, const int) const {}
    template<class H, class T>
    void serializeWithPrecisionToCharStream(std::ostream& ostr,
                                            const boost::tuples::cons<H, T>& t,
                                            const int idx) const;
    void serializeWithPrecisionToCharStream(std::ostream& ostr, const null_type&, const int) const
    {}
    template<class H, class T>
    void deserializeFromCharStream(std::istream& istr,
                                   boost::tuples::cons<H, T>& t,
                                   const int idx,
                                   bool withSuffix);
    void deserializeFromCharStream(std::istream& istr, const null_type&, const int, bool withSuffix)
    {}
    template<class H, class T>
    void deserializeFromCharStreamWithNanAndInf(std::istream& istr,
                                                boost::tuples::cons<H, T>& t,
                                                const int idx,
                                                bool withSuffix);
    void deserializeFromCharStreamWithNanAndInf(std::istream& istr,
                                                const null_type&,
                                                const int,
                                                bool withSuffix)
    {}
    template<class H>
    void deserializeAttribute(std::istream& istr, H& x, const int idx, bool withSuffix);
    template<class H, class T>
    size_t computeHash(const boost::tuples::cons<H, T>& t) const
    {
        return std::tr1::hash<H>()(t.get_head()) + 37 * computeHash(t.get_tail());
    }
    size_t computeHash(const null_type&) const { return 17; }
    template<class H, class T>
    void swapAttribute(boost::tuples::cons<H, T>& lhs, boost::tuples::cons<H, T>& rhs)
    {
        std::swap(lhs.get_head(), rhs.get_head());
        swapAttribute(lhs.get_tail(), rhs.get_tail());
    }
    void swapAttribute(null_type&, null_type&) {}

    ValueHandle getAttributeValueRaw(const uint32_t index)
    {
        if (index >= num_elements) {
            THROW(SPLRuntimeInvalidIndex, "Tuple index out of bounds '"
                                            << index << "', valid range is [0.." << num_elements - 1
                                            << "]");
        }
        const TypeOffset& toffset = mappings_->indexToTypeOffset_[index];
        return ValueHandle((char*)this + toffset.getOffset(), toffset.getMetaType(),
                           &toffset.getTypeId());
    }
    static TupleMappings* initMappings()
    {
        tuple_type tp;
        // assigned to a auto_ptr after return of this call
        // see static member initialization for mappings_
        TupleMappings* tm = new TupleMappings();
        addMapping<0>(tp, &tp, tm);
        return tm;
    }
    template<int index, class H, class T>
    static void addMapping(const boost::tuples::cons<H, T>& x, void* base, TupleMappings* tm);
    template<int index>
    static void addMapping(const null_type&, void*, TupleMappings*)
    {}

  private:
    static std::auto_ptr<TupleMappings> mappings_;
};

template<SPL_TUPLE_TEMPLATE_DEFS>
std::auto_ptr<TupleMappings> tuple<SPL_TUPLE_TEMPLATES>::mappings_(
  tuple<SPL_TUPLE_TEMPLATES>::initMappings());

template<SPL_TUPLE_TEMPLATE_DEFS>
template<int index, class H, class T>
inline void tuple<SPL_TUPLE_TEMPLATES>::addMapping(const boost::tuples::cons<H, T>& x,
                                                   void* base,
                                                   TupleMappings* tm)
{
    int status = 0;
    size_t length = 0;
    std::string prefix = "__spl_";
    typedef typename boost::mpl::at<name_vector, boost::mpl::int_<index> >::type atbr_type;
    std::string typestr = typeid(atbr_type).name();
    char* result = __cxxabiv1::__cxa_demangle(typestr.c_str(), NULL, &length, &status);
    if (status != 0 || result == NULL) {
        throw "Unable to demangle class name through __cxa_demangle";
    }
    typestr = result;
    free(result);
    assert(typestr.find_first_of(prefix) != rstring::npos);
    assert(typestr[typestr.size() - 1] == '*');
    assert(typestr.size() >= 2);
    assert(typestr[typestr.size() - 2] != ' ');
    typestr.erase(typestr.size() - 1, 1);
    typestr = typestr.substr(typestr.find(prefix) + prefix.size());
    tm->nameToIndex_[typestr] = index;
    tm->indexToName_.push_back(typestr);
    tm->indexToTypeOffset_.push_back(TypeOffset(
      (size_t)((intptr_t)(&x.get_head()) - (intptr_t)base), Meta::Type::typeOf<H>(), &typeid(H)));
    addMapping<index + 1>(x.get_tail(), base, tm);
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline void tuple<SPL_TUPLE_TEMPLATES>::serialize(std::ostream& ostr) const
{
    ostr << "{";
    ostr << this->getAttributeName(0) << "=";
    ostr << this->get_head();
    serializeToCharStream(ostr, this->get_tail(), 1);
    ostr << "}";
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline void tuple<SPL_TUPLE_TEMPLATES>::serializeWithPrecision(std::ostream& ostr) const
{
    ostr << "{";
    ostr << this->getAttributeName(0) << "=";
    SPL::serializeWithPrecision(ostr, this->get_head());
    serializeToCharStream(ostr, this->get_tail(), 1);
    ostr << "}";
}

template<SPL_TUPLE_TEMPLATE_DEFS>
template<class H, class T>
inline void tuple<SPL_TUPLE_TEMPLATES>::serializeToCharStream(std::ostream& ostr,
                                                              const boost::tuples::cons<H, T>& t,
                                                              const int idx) const
{
    ostr << ",";
    ostr << getAttributeName(idx) << "=";
    ostr << t.get_head();
    serializeToCharStream(ostr, t.get_tail(), idx + 1);
}

template<SPL_TUPLE_TEMPLATE_DEFS>
template<class H, class T>
inline void tuple<SPL_TUPLE_TEMPLATES>::serializeWithPrecisionToCharStream(
  std::ostream& ostr,
  const boost::tuples::cons<H, T>& t,
  const int idx) const
{
    ostr << ",";
    ostr << getAttributeName(idx) << "=";
    serializeToCharStream(ostr, t.get_head());
    serializeToCharStream(ostr, t.get_tail(), idx + 1);
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline void tuple<SPL_TUPLE_TEMPLATES>::deserialize(std::istream& istr, bool withSuffix)
{
    char c;
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != '{') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    deserializeAttribute(istr, this->get_head(), 0, withSuffix);
    if (!istr) {
        return;
    }
    deserializeFromCharStream(istr, this->get_tail(), 1, withSuffix);
    if (!istr) {
        return;
    }
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != '}') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline void tuple<SPL_TUPLE_TEMPLATES>::deserializeWithNanAndInfs(std::istream& istr,
                                                                  bool withSuffix)
{
    char c;
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != '{') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    deserializeAttribute(istr, this->get_head(), 0, withSuffix);
    if (!istr) {
        return;
    }
    deserializeFromCharStreamWithNanAndInf(istr, this->get_tail(), 1, withSuffix);
    if (!istr) {
        return;
    }
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != '}') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
}

template<SPL_TUPLE_TEMPLATE_DEFS>
template<class H, class T>
inline void tuple<SPL_TUPLE_TEMPLATES>::deserializeFromCharStream(std::istream& istr,
                                                                  boost::tuples::cons<H, T>& t,
                                                                  const int idx,
                                                                  bool withSuffix)
{
    char c;
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != ',') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    deserializeAttribute(istr, t.get_head(), idx, withSuffix);
    if (!istr) {
        return;
    }
    deserializeFromCharStream(istr, t.get_tail(), idx + 1, withSuffix);
}

template<SPL_TUPLE_TEMPLATE_DEFS>
template<class H, class T>
inline void tuple<SPL_TUPLE_TEMPLATES>::deserializeFromCharStreamWithNanAndInf(
  std::istream& istr,
  boost::tuples::cons<H, T>& t,
  const int idx,
  bool withSuffix)
{
    char c;
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != ',') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    deserializeAttribute(istr, t.get_head(), idx, withSuffix);
    if (!istr) {
        return;
    }
    deserializeFromCharStreamWithNanAndInf(istr, t.get_tail(), idx + 1, withSuffix);
}

template<SPL_TUPLE_TEMPLATE_DEFS>
template<class H>
inline void tuple<SPL_TUPLE_TEMPLATES>::deserializeAttribute(std::istream& istr,
                                                             H& x,
                                                             const int idx,
                                                             bool withSuffix)
{
    std::string s;
    readAttributeIdentifier(istr, s);
    if (!istr) {
        return;
    }
    if (s != getAttributeName(idx)) {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    char c;
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != '=') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    SPL::deserializeWithNanAndInfs(istr, x, withSuffix);
    if (!istr) {
        return;
    }
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline std::ostream& serializeWithPrecision(std::ostream& ostr, const tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.serializeWithPrecision(ostr);
    return ostr;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline std::istream& deserializeWithSuffix(std::istream& istr, tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.deserialize(istr, true);
    return istr;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline std::istream& deserializeWithNanAndInfs(std::istream& istr,
                                               tuple<SPL_TUPLE_TEMPLATES>& x,
                                               bool withSuffix = false)
{
    x.deserializeWithNanAndInfs(istr, withSuffix);
    return istr;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline std::ostream& operator<<(std::ostream& ostr, const tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.serialize(ostr);
    return ostr;
}
template<SPL_TUPLE_TEMPLATE_DEFS>
inline std::istream& operator>>(std::istream& istr, tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.deserialize(istr);
    return istr;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, const tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.serialize(sbuf);
    return sbuf;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.deserialize(sbuf);
    return sbuf;
}

template<class S, SPL_TUPLE_TEMPLATE_DEFS>
inline ByteBuffer<S>& operator<<(ByteBuffer<S>& sbuf, const tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.serialize(sbuf);
    return sbuf;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline NetworkByteBuffer& operator<<(NetworkByteBuffer& sbuf, const tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.serialize(sbuf);
    return sbuf;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline NativeByteBuffer& operator<<(NativeByteBuffer& sbuf, const tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.serialize(sbuf);
    return sbuf;
}

template<class S, SPL_TUPLE_TEMPLATE_DEFS>
inline ByteBuffer<S>& operator>>(ByteBuffer<S>& sbuf, tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.deserialize(sbuf);
    return sbuf;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline NetworkByteBuffer& operator>>(NetworkByteBuffer& sbuf, tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.deserialize(sbuf);
    return sbuf;
}

template<SPL_TUPLE_TEMPLATE_DEFS>
inline NativeByteBuffer& operator>>(NativeByteBuffer& sbuf, tuple<SPL_TUPLE_TEMPLATES>& x)
{
    x.deserialize(sbuf);
    return sbuf;
}

};

namespace std {
template<SPL_TUPLE_TEMPLATE_DEFS>
inline void swap(const SPL::tuple<SPL_TUPLE_TEMPLATES>& a, const SPL::tuple<SPL_TUPLE_TEMPLATES>& b)
{
    a.swap(b);
}
};

// Hashability requirements
namespace std {
namespace tr1 {
template<SPL_TUPLE_TEMPLATE_DEFS>
struct hash<SPL::tuple<SPL_TUPLE_TEMPLATES> >
{
    inline size_t operator()(const SPL::tuple<SPL_TUPLE_TEMPLATES>& t) const
    {
        return t.hashCode();
    }
};
}
}

#endif /* SPL_RUNTIME_TYPE_TMP_TUPLE_H */
