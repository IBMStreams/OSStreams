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

#include <SPL/FrontEnd/Typ.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/Toolkit.h>

#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/Runtime/Type/Blob.h>
#include <SPL/Runtime/Type/Complex.h>
#include <SPL/Runtime/Type/Decimal.h>
#include <SPL/Runtime/Type/Float.h>
#include <SPL/Runtime/Type/Integer.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/String.h>
#include <SPL/Runtime/Type/Timestamp.h>
#include <SPL/Utility/SourceLocation.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/HashMapHelpers.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace bf = boost::filesystem;

using namespace SPL;
using namespace std;

RootTyp const& RootTyp::fromRuntimeRepresentation(Meta::BaseType const& btype)
{
    typedef std::tr1::unordered_map<uintptr_t, boost::shared_ptr<RootTyp const> > Map;
    static Map map;
    uintptr_t ptr = reinterpret_cast<uintptr_t>(&btype);
    Map::iterator it = map.find(ptr);
    if (it != map.end()) {
        return *it->second.get();
    }
    RootTyp const& type = constructType(btype);
    map.insert(std::make_pair(ptr, boost::shared_ptr<RootTyp const>(&type)));
    return type;
}

RootTyp const& RootTyp::constructType(Meta::BaseType const& btype)
{
    Meta::Type type = btype.getMetaType();
    MetaType ctype = static_cast<MetaType::value>(type.getValue());
    switch (ctype) {
        case MetaType::BOOLEAN:
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
        case MetaType::TIMESTAMP:
        case MetaType::RSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
        case MetaType::XML:
            return *new SimplePrimitiveTyp(ctype);
        case MetaType::BSTRING:
            return *new BStringTyp(btype.as<Meta::BStringType>().getBound());
        case MetaType::ENUM:
            return *new EnumTyp(btype.as<Meta::EnumType>().getValidValues());
        case MetaType::LIST:
            return *new ListTyp(
              RootTyp::fromRuntimeRepresentation(btype.as<Meta::ListType>().getElementType()));
        case MetaType::BLIST: {
            Meta::BListType const& t = btype.as<Meta::BListType>();
            return *new BListTyp(RootTyp::fromRuntimeRepresentation(t.getElementType()),
                                 t.getBound());
        }
        case MetaType::SET:
            return *new SetTyp(
              RootTyp::fromRuntimeRepresentation(btype.as<Meta::SetType>().getElementType()));
        case MetaType::BSET: {
            Meta::BSetType const& t = btype.as<Meta::BSetType>();
            return *new BSetTyp(RootTyp::fromRuntimeRepresentation(t.getElementType()),
                                t.getBound());
        }
        case MetaType::MAP: {
            Meta::MapType const& t = btype.as<Meta::MapType>();
            return *new MapTyp(RootTyp::fromRuntimeRepresentation(t.getKeyType()),
                               RootTyp::fromRuntimeRepresentation(t.getValueType()));
        }
        case MetaType::BMAP: {
            Meta::BMapType const& t = btype.as<Meta::BMapType>();
            return *new BMapTyp(RootTyp::fromRuntimeRepresentation(t.getKeyType()),
                                RootTyp::fromRuntimeRepresentation(t.getValueType()), t.getBound());
        }
        case MetaType::TUPLE: {
            Meta::TupleType const& t = btype.as<Meta::TupleType>();
            TupleTyp::Name2Index name2index(t.getAttributeNames());
            TupleTyp::Attributes attributes;
            for (uint32_t i = 0, iu = t.getNumberOfAttributes(); i < iu; ++i) {
                attributes.push_back(
                  std::make_pair(t.getAttributeName(i),
                                 &RootTyp::fromRuntimeRepresentation(t.getAttributeType(i))));
            }
            return *new TupleTyp(name2index, attributes);
        }
        default:
            assert(!"cannot happen");
    }
    return *((RootTyp const*)NULL);
}

// RootTyp ..........................................................
std::ostream& SPL::operator<<(std::ostream& os, RootTyp const* t)
{
    if (NULL == t) {
        return os << "NULL_TYPE";
    }
    TypeFormal const* const tf = dynamic_cast<TypeFormal const* const>(t);
    if (NULL != tf) {
        return os << tf->getIdentifier();
    }
    return os << t->getName();
}

std::ostream& SPL::operator<<(std::ostream& os, RootTyp const& t)
{
    return os << &t;
}

// EmptyCurly ........................................................
EmptyCurlyTyp::EmptyCurlyTyp()
  : RootTyp(MetaType(MetaType::EMPTYCURLY).toString(), MetaType::EMPTYCURLY)
{}

bool EmptyCurlyTyp::equals(RootTyp const& other) const
{
    return other.is<EmptyCurlyTyp>();
}

size_t EmptyCurlyTyp::hash() const
{
    return 5749;
}

// TypeFormal ........................................................
bool TypeFormal::equals(RootTyp const& other) const
{
    if (other.is<TypeFormal>()) {
        TypeFormal const& y = other.as<TypeFormal>();
        return getFunction() == y.getFunction() && getIdentifier() == y.getIdentifier();
    }
    return false;
}

size_t TypeFormal::hash() const
{
    std::tr1::hash<std::string> stringHash;
    const size_t hf = (size_t)getFunction();
    const size_t hs = stringHash(getIdentifier());
    return 1901 * hf + 1759 * hs;
}

TypeFormal::TypeFormal(const FunctionHeadSym* f, const std::string& c, const std::string& i)
  : RootTyp(makeName(f, i), MetaType::TYPEFORMAL)
  , function_(f)
  , constraint_(c)
  , identifier_(i)
{}

std::string TypeFormal::makeName(const FunctionHeadSym* f, const std::string& i)
{
    return i;
}

// BoundsFormal ......................................................
bool BoundsFormal::equals(BoundsFormal const& other) const
{
    return getFunction() == other.getFunction() && getIdentifier() == other.getIdentifier();
}

size_t BoundsFormal::hash() const
{
    std::tr1::hash<std::string> stringHash;
    const size_t hf = (size_t)getFunction();
    const size_t hs = stringHash(getIdentifier());
    return hf + 2791 * hs;
}

std::ostream& SPL::operator<<(std::ostream& os, BoundsFormal const* b)
{
    if (NULL == b) {
        return os << "NULL_BOUND";
    }
    return os << b->getIdentifier();
}

// TypeBound .........................................................
bool TypeBound::equals(TypeBound const& other) const
{
    if (isVariable()) {
        if (other.isVariable()) {
            return toVar().equals(other.toVar());
        } else {
            return false;
        }
    } else if (other.isVariable()) {
        return false;
    } else {
        return toInt() == other.toInt();
    }
}

size_t TypeBound::hash() const
{
    return isVariable() ? toVar().hash() : 149 * toInt() + 29;
}

std::ostream& SPL::operator<<(std::ostream& os, TypeBound const& b)
{
    if (b.isVariable()) {
        return os << &b.toVar();
    }
    return os << b.toInt();
}

// BStringTyp .......................................................
BStringTyp::BStringTyp(TypeBound const& bound)
  : PrimitiveTyp(makeName(bound), MetaType::BSTRING)
  , bound_(bound)
{}

bool BStringTyp::equals(RootTyp const& other) const
{
    if (other.is<BStringTyp>()) {
        return getBound().equals(other.as<BStringTyp>().getBound());
    }
    return false;
}

size_t BStringTyp::hash() const
{
    return 4051 + 3 * getBound().hash();
}

std::string BStringTyp::makeName(TypeBound const& bound)
{
    std::ostringstream os;
    os << MetaType::RSTRING;
    os << '[' << bound << ']';
    return os.str();
}

uint64_t BStringTyp::getSizeEstimate() const
{
    return bound_.toInt();
}

// ListTyp ..........................................................
ListTyp::ListTyp(const RootTyp& etype)
  : CollectionTyp(makeName(etype), MetaType::LIST)
  , etype_(etype)
{}

bool ListTyp::equals(RootTyp const& other) const
{
    if (other.is<ListTyp>()) {
        return getElementType().equals(other.as<ListTyp>().getElementType());
    }
    return false;
}

size_t ListTyp::hash() const
{
    return 7789 + 3 * getElementType().hash();
}

std::string ListTyp::makeName(const RootTyp& e)
{
    std::ostringstream os;
    os << MetaType::LIST << '<' << e.getName() << '>';
    return os.str();
}

uint64_t ListTyp::getSizeEstimate() const
{
    return 30 * etype_.getSizeEstimate();
}

// BListTyp .........................................................
BListTyp::BListTyp(RootTyp const& etype, TypeBound const& bound)
  : BCollectionType(makeName(etype, bound), MetaType::BLIST, bound)
  , etype_(etype)
{}

bool BListTyp::equals(RootTyp const& other) const
{
    if (other.is<BListTyp>()) {
        BListTyp const& y = other.as<BListTyp>();
        return getElementType().equals(y.getElementType()) && getBound().equals(y.getBound());
    }
    return false;
}

size_t BListTyp::hash() const
{
    return 1657 + 3 * getElementType().hash() + 5 * getBound().hash();
}

std::string BListTyp::makeName(RootTyp const& elemType, TypeBound const& bound)
{
    std::ostringstream os;
    os << ListTyp::makeName(elemType) << '[' << bound << ']';
    return os.str();
}

uint64_t BListTyp::getSizeEstimate() const
{
    return bound_.toInt() * etype_.getSizeEstimate();
}

uint64_t BListTyp::getFixedSize() const
{
    uint32_t boundSize = TMP::getSizeTypeSize(bound_.toInt());
    return boundSize + bound_.toInt() * etype_.getFixedSize();
}

// SetTyp ..........................................................
SetTyp::SetTyp(RootTyp const& etype)
  : CollectionTyp(makeName(etype), MetaType::SET)
  , etype_(etype)
{}

bool SetTyp::equals(RootTyp const& other) const
{
    if (other.is<SetTyp>()) {
        return getElementType().equals(other.as<SetTyp>().getElementType());
    }
    return false;
}

size_t SetTyp::hash() const
{
    return 5281 + 3 * getElementType().hash();
}

std::string SetTyp::makeName(RootTyp const& e)
{
    std::ostringstream os;
    os << MetaType::SET << '<' << e.getName() << '>';
    return os.str();
}

uint64_t SetTyp::getSizeEstimate() const
{
    return 30 * etype_.getSizeEstimate();
}

// BSetTyp .........................................................
BSetTyp::BSetTyp(RootTyp const& etype, TypeBound const& bound)
  : BCollectionType(makeName(etype, bound), MetaType::BSET, bound)
  , etype_(etype)
{}

bool BSetTyp::equals(RootTyp const& other) const
{
    if (other.is<BSetTyp>()) {
        BSetTyp const& y = other.as<BSetTyp>();
        return getElementType().equals(y.getElementType()) && getBound().equals(y.getBound());
    }
    return false;
}

size_t BSetTyp::hash() const
{
    return 4219 + 3 * getElementType().hash() + 5 * getBound().hash();
}

std::string BSetTyp::makeName(RootTyp const& elemType, TypeBound const& bound)
{
    std::ostringstream os;
    os << SetTyp::makeName(elemType) << '[' << bound << ']';
    return os.str();
}

uint64_t BSetTyp::getSizeEstimate() const
{
    return bound_.toInt() * etype_.getSizeEstimate();
}

uint64_t BSetTyp::getFixedSize() const
{
    uint32_t boundSize = TMP::getSizeTypeSize(bound_.toInt());
    // 1 is for used_ array
    return boundSize + bound_.toInt() * (1 + etype_.getFixedSize());
}

// MapTyp ..........................................................
MapTyp::MapTyp(RootTyp const& ktype, RootTyp const& vtype)
  : CollectionTyp(makeName(ktype, vtype), MetaType::MAP)
  , ktype_(ktype)
  , vtype_(vtype)
{}

bool MapTyp::equals(RootTyp const& other) const
{
    if (other.is<MapTyp>()) {
        MapTyp const& y = other.as<MapTyp>();
        return getKeyType().equals(y.getKeyType()) && getValueType().equals(y.getValueType());
    }
    return false;
}

size_t MapTyp::hash() const
{
    return 3319 + 3 * getKeyType().hash() + 5 * getValueType().hash();
}

std::string MapTyp::makeName(RootTyp const& k, RootTyp const& v)
{
    std::ostringstream os;
    os << MetaType::MAP << '<' << k.getName() << ',' << v.getName() << '>';
    return os.str();
}

uint64_t MapTyp::getSizeEstimate() const
{
    return 30 * (ktype_.getSizeEstimate() + vtype_.getSizeEstimate());
}

// BMapTyp ..........................................................
BMapTyp::BMapTyp(RootTyp const& ktype, RootTyp const& vtype, TypeBound const& bound)
  : BCollectionType(makeName(ktype, vtype, bound), MetaType::BMAP, bound)
  , ktype_(ktype)
  , vtype_(vtype)
{}

bool BMapTyp::equals(RootTyp const& other) const
{
    if (other.is<BMapTyp>()) {
        BMapTyp const& y = other.as<BMapTyp>();
        return getKeyType().equals(y.getKeyType()) && getValueType().equals(y.getValueType()) &&
               getBound().equals(y.getBound());
    }
    return false;
}

size_t BMapTyp::hash() const
{
    return 3467 + 3 * getKeyType().hash() + 5 * getValueType().hash() + 7 * getBound().hash();
}

std::string BMapTyp::makeName(RootTyp const& keyType,
                              RootTyp const& valType,
                              TypeBound const& bound)
{
    std::ostringstream os;
    os << MapTyp::makeName(keyType, valType) << '[' << bound << ']';
    return os.str();
}

uint64_t BMapTyp::getSizeEstimate() const
{
    return bound_.toInt() * (ktype_.getSizeEstimate() + vtype_.getSizeEstimate());
}

uint64_t BMapTyp::getFixedSize() const
{
    uint32_t boundSize = TMP::getSizeTypeSize(bound_.toInt());
    // Add 1 for the used array
    return boundSize + bound_.toInt() * (1 + ktype_.getFixedSize() + vtype_.getFixedSize());
}

// EnumTyp ..........................................................
EnumTyp::EnumTyp(Values const& values)
  : PrimitiveTyp(makeName(values), MetaType::ENUM)
  , _values(values)
{}

std::string EnumTyp::makeName(Values const& values)
{
    std::stringstream os;
    os << MetaType::ENUM << '{';
    for (int i = 0, n = values.size(); i < n; i++) {
        if (0 != i) {
            os << ',';
        }
        os << values.at(i);
    }
    os << '}';
    return os.str();
}

uint64_t EnumTyp::getSizeEstimate() const
{
    return sizeof(int);
}

bool EnumTyp::equals(RootTyp const& other) const
{
    if (other.is<EnumTyp>()) {
        EnumTyp const& y = other.as<EnumTyp>();
        if (_values.size() != y._values.size()) {
            return false;
        }
        for (int i = 0, n = _values.size(); i < n; i++) {
            if (_values.at(i) != y._values.at(i)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

size_t EnumTyp::hash() const
{
    std::tr1::hash<std::string> stringHash;
    size_t result = 4201;
    for (int i = 0, n = _values.size(); i < n; i++) {
        result = 3 * result + stringHash(_values.at(i));
    }
    return result;
}

// XMLTyp ...........................................................
bool isURLSchema(const string& schema)
{
    string urlStart("http://");
    if (schema.substr(0, urlStart.size()) == urlStart) {
        return true;
    }
    return false;
}

XMLTyp::XMLTyp(const std::string& sch, uint32_t toolkitIndex_)
  : PrimitiveTyp(makeName(sch, toolkitIndex_), MetaType::XML)
  , _schema(sch)
  , _toolkitIndex(toolkitIndex_)
{
    // If the schema is file-based, make sure it exists
    if (!sch.empty()) {
        if (!isURLSchema(sch)) {
            bf::path s(compileTimeSchema());
            if (!bf::exists(s)) {
                SysOut::die(SPL_CORE_XML_SCHEMA_NOT_FOUND(s.string()));
            }
        }
    }
}

std::string XMLTyp::makeName(const std::string& schema, uint32_t toolkitIndex)
{
    std::ostringstream os;
    os << MetaType::XML;
    if (!schema.empty()) {
        os << "<\"" << schema << "\">";
        if (schema.substr(0, 5) != "http:") {
            os << "(" << toolkitIndex << ")";
        }
    }
    return os.str();
}

size_t XMLTyp::hash() const
{
    size_t sh = 0;
    if (!_schema.empty()) {
        std::tr1::hash<std::string> stringHash;
        sh = stringHash(_schema);
    }
    return 1009 + sh;
}

uint64_t XMLTyp::getSizeEstimate() const
{
    return 1024;
}

bool XMLTyp::equals(RootTyp const& other) const
{
    if (other.is<XMLTyp>()) {
        const XMLTyp& otherXMLType = other.as<XMLTyp>();
        const string& lhsSchema = compileTimeSchema();
        const string& rhsSchema = otherXMLType.compileTimeSchema();

        // If the compile-time schemas are the same then consider them to be the same schema
        if (lhsSchema == rhsSchema) {
            return true;
        }

        // Technically speaking we could have two schemas that have different file names
        // but actually have the same XML schema. As of this implementation they are
        // considered different schemas. We would have to do an intelligent file compare
        // to ensure that the schemas are identical in their representation, not just a bit-wise
        // compare. That is currently deamed beyond the scope of our support.
    }
    return false;
}

std::string XMLTyp::compileTimeSchema() const
{
    // If the schema is not a fully qualified path or a uri/url, then point it to the etc dir
    if (!_schema.empty() && '/' != _schema[0] && !isURLSchema(_schema)) {
        // We need to find the toolkit directory for the toolkit in which the type is defined
        const Toolkit* tk = PathSearch::instance().toolkits()[_toolkitIndex];
        bf::path p = tk->path();
        if (!p.is_complete()) {
            p = bf::system_complete(p);
        }
        p /= "etc";
        p /= _schema;
        return p.string();
    }
    return _schema;
}

std::string XMLTyp::runTimeSchema() const
{
    // This needs to be relative to the output directory if not already fully qualified
    if (!_schema.empty() && '/' != _schema[0] && !isURLSchema(_schema)) {
        // We need to find the toolkit directory for the toolkit in which the type is defined
        const Toolkit* tk = PathSearch::instance().toolkits()[_toolkitIndex];
        bf::path p("toolkits");
        p /= tk->name();
        p /= "etc";
        p /= _schema;
        return p.string();
    }
    return _schema;
}

// TupleTyp .........................................................
TupleTyp::TupleTyp(Name2Index& name2index, Attributes& attributes)
  : CompositeTyp(makeName(attributes), MetaType::TUPLE)
  , _name2index(name2index)
  , _attributes(attributes)
{}

bool TupleTyp::equals(RootTyp const& other) const
{
    if (other.is<TupleTyp>()) {
        TupleTyp const& y = other.as<TupleTyp>();
        if (getNumberOfAttributes() != y.getNumberOfAttributes()) {
            return false;
        }
        for (int i = 0, n = getNumberOfAttributes(); i < n; i++) {
            if (getAttributeName(i) != y.getAttributeName(i)) {
                return false;
            }
            if (!getAttributeType(i).equals(y.getAttributeType(i))) {
                return false;
            }
        }
        return true;
    }
    return false;
}

size_t TupleTyp::hash() const
{
    std::tr1::hash<std::string> stringHash;
    size_t result = 2843;
    for (int i = 0, n = getNumberOfAttributes(); i < n; i++) {
        result = 3 * result + 5 * stringHash(getAttributeName(i)) + 7 * getAttributeType(i).hash();
    }
    return result;
}

std::string TupleTyp::makeName(Attributes& attributes)
{
    std::ostringstream os;
    os << MetaType::TUPLE << '<';
    for (int i = 0, n = attributes.size(); i < n; i++) {
        if (0 != i) {
            os << ',';
        }
        os << attributes.at(i).second->getName() << ' ' << attributes.at(i).first;
    }
    os << '>';
    return os.str();
}

uint64_t TupleTyp::getSizeEstimate() const
{
    uint64_t sz = 0;
    for (int i = 0, n = _attributes.size(); i < n; i++) {
        sz += _attributes.at(i).second->getSizeEstimate();
    }
    return sz;
}

uint64_t TupleTyp::getFixedSize() const
{
    uint64_t sz = 0;
    for (int i = 0, n = _attributes.size(); i < n; i++) {
        sz += _attributes.at(i).second->getFixedSize();
    }
    return sz;
}

// SimplePrimitiveTyp .........................................
SimplePrimitiveTyp::SimplePrimitiveTyp(MetaType const& meta)
  : PrimitiveTyp(meta.toString(), meta)
{}

uint64_t SimplePrimitiveTyp::getSizeEstimate() const
{
    switch (type_) {
        case MetaType::BOOLEAN:
        case MetaType::INT8:
        case MetaType::UINT8:
        case MetaType::INT16:
        case MetaType::UINT16:
        case MetaType::INT32:
        case MetaType::UINT32:
        case MetaType::FLOAT32:
        case MetaType::DECIMAL32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
        case MetaType::TIMESTAMP:
            return getFixedSize();
        case MetaType::RSTRING:
            return 48; // estimate
        case MetaType::USTRING:
            return 2 * 48; // estimate
        case MetaType::BLOB:
            return 2 * 1024; // estimate
        case MetaType::XML:
            return 512; // estimate
        default:
            assert(!"cannot happen!");
    }
    return 0; // just in case
}

uint64_t SimplePrimitiveTyp::getFixedSize() const
{
    switch (type_) {
        case MetaType::BOOLEAN:
            return 1;
        case MetaType::INT8:
        case MetaType::UINT8:
            return 1;
        case MetaType::INT16:
        case MetaType::UINT16:
            return 2;
        case MetaType::INT32:
        case MetaType::UINT32:
            return 4;
        case MetaType::INT64:
        case MetaType::UINT64:
            return 8;
        case MetaType::FLOAT32:
        case MetaType::DECIMAL32:
            return 4;
        case MetaType::FLOAT64:
        case MetaType::DECIMAL64:
            return 8;
        case MetaType::DECIMAL128:
            return 16;
        case MetaType::COMPLEX32:
            return 2 * 4;
        case MetaType::COMPLEX64:
            return 2 * 8;
        case MetaType::TIMESTAMP:
            return 8 + 2 * 4;
        case MetaType::RSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
        case MetaType::XML:
            assert(!"cannot happen");
        default:
            assert(!"cannot happen");
    }
    return 0;
}

#define C(a) XXX##a
#define B(a)                                                                                       \
    struct C(a)                                                                                    \
    {                                                                                              \
        char dummy;                                                                                \
        a data;                                                                                    \
    };
#define A(a)                                                                                       \
    {                                                                                              \
        B(a);                                                                                      \
        return sizeof(C(a)) - sizeof(a);                                                           \
    }

uint8_t SimplePrimitiveTyp::getAlignment() const
{
    switch (type_) {
        case MetaType::BOOLEAN:
            A(boolean);
        case MetaType::INT8:
            A(int8);
        case MetaType::UINT8:
            A(uint8);
        case MetaType::INT16:
            A(int16);
        case MetaType::UINT16:
            A(uint16);
        case MetaType::INT32:
            A(int32);
        case MetaType::UINT32:
            A(uint32);
        case MetaType::INT64:
            A(int64);
        case MetaType::UINT64:
            A(uint64);
        case MetaType::FLOAT32:
            A(float32);
        case MetaType::DECIMAL32:
            A(decimal32);
        case MetaType::FLOAT64:
            A(float64);
        case MetaType::DECIMAL64:
            A(decimal64);
        case MetaType::DECIMAL128:
            A(decimal64);
        case MetaType::COMPLEX32:
            A(complex32);
        case MetaType::COMPLEX64:
            A(complex64);
        case MetaType::TIMESTAMP:
            A(timestamp);
        case MetaType::RSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
        case MetaType::XML:
            assert(!"cannot happen");
        default:
            assert(!"cannot happen");
    }
    return 0;
}

bool SimplePrimitiveTyp::isIntegralType() const
{
    switch (type_) {
        case MetaType::INT8:
        case MetaType::UINT8:
        case MetaType::INT16:
        case MetaType::UINT16:
        case MetaType::INT32:
        case MetaType::UINT32:
        case MetaType::INT64:
        case MetaType::UINT64:
            return true;
        default:
            break;
    }
    return false;
}

bool SimplePrimitiveTyp::isNumericType() const
{
    switch (type_) {
        case MetaType::INT8:
        case MetaType::UINT8:
        case MetaType::INT16:
        case MetaType::UINT16:
        case MetaType::INT32:
        case MetaType::UINT32:
        case MetaType::INT64:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::DECIMAL32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
            return true;
        default:
            break;
    }
    return false;
}

// OptionalTyp .........................................
OptionalTyp::OptionalTyp(const RootTyp& underlyingType)
  : RootTyp(makeName(underlyingType), MetaType::OPTIONAL)
  , underlyingType_(underlyingType)
{}

std::string OptionalTyp::makeName(const RootTyp& underlyingType)
{
    std::ostringstream os;
    os << MetaType::OPTIONAL;
    os << '<' << underlyingType.getName() << '>';
    return os.str();
}

bool OptionalTyp::equals(RootTyp const& other) const
{
    if (other.is<OptionalTyp>()) {
        return getUnderlyingType().equals(other.as<OptionalTyp>().getUnderlyingType());
    }
    return false;
}

size_t OptionalTyp::hash() const
{
    return 2143 + 3 * getUnderlyingType().hash();
}
