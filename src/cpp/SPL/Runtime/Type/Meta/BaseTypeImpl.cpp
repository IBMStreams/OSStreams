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

#include <SPL/Runtime/Type/Meta/BaseTypeImpl.h>

using namespace std;

namespace SPL {
namespace Meta {

// BaseTypeImpl ..........................................................
Distillery::Mutex BaseTypeFactory::mutex_;
SPL::Meta::BaseTypeFactory::TypeMap BaseTypeFactory::types_;

const BaseTypeImpl& BaseTypeFactory::makeType(const ConstValueHandle& vh)
{
    Distillery::AutoMutex am(mutex_);
    return makeTypeRaw(vh);
}

const BaseTypeImpl& BaseTypeFactory::makeTypeRaw(const ConstValueHandle& vh)
{
    const string& typenm = vh.getTypeId().name();
    TypeMap::const_iterator it = types_.find(typenm);
    if (it == types_.end()) {
        BaseTypeFactory::P sp(constructType(vh));
        types_.insert(make_pair(typenm, sp));
        return *sp.get();
    } else {
        return *(it->second.get());
    }
}

BaseTypeImpl* BaseTypeFactory::constructType(const ConstValueHandle& vh)
{
    Type type = vh.getMetaType();
    switch (type) {
        case Type::BOOLEAN:
        case Type::INT8:
        case Type::INT16:
        case Type::INT32:
        case Type::INT64:
        case Type::UINT8:
        case Type::UINT16:
        case Type::UINT32:
        case Type::UINT64:
        case Type::FLOAT32:
        case Type::FLOAT64:
        case Type::DECIMAL32:
        case Type::DECIMAL64:
        case Type::DECIMAL128:
        case Type::COMPLEX32:
        case Type::COMPLEX64:
        case Type::TIMESTAMP:
        case Type::RSTRING:
        case Type::USTRING:
        case Type::BLOB:
            return new SimplePrimitiveTypeImpl(type);
        case Type::BSTRING:
            return new BStringTypeImpl(((const BString&)vh).getBoundedSize());
        case Type::XML:
            return new XmlTypeImpl((const xml&)vh);
        case Type::ENUM: {
            const Enum& en = vh;
            return new EnumTypeImpl(new EnumTypeImpl::Values(en.getValidValues()));
        }
        case Type::LIST: {
            ValueHandle nvh = ((const List&)vh).createElement();
            ListTypeImpl* rval = new ListTypeImpl(BaseTypeFactory::makeTypeRaw(nvh));
            nvh.deleteValue();
            return rval;
        }
        case Type::OPTIONAL: {
            ValueHandle nvh = ((const Optional&)vh).createValue();
            OptionalTypeImpl* rval = new OptionalTypeImpl(BaseTypeFactory::makeTypeRaw(nvh));
            nvh.deleteValue();
            return rval;
        }
        case Type::BLIST: {
            const BList& ls = vh;
            ValueHandle nvh = ls.createElement();
            BListTypeImpl* rval =
              new BListTypeImpl(BaseTypeFactory::makeTypeRaw(nvh), ls.getBoundedSize());
            nvh.deleteValue();
            return rval;
        }
        case Type::SET: {
            ValueHandle nvh = ((const Set&)vh).createElement();
            SetTypeImpl* rval = new SetTypeImpl(BaseTypeFactory::makeTypeRaw(nvh));
            nvh.deleteValue();
            return rval;
        }
        case Type::BSET: {
            const BSet& st = vh;
            ValueHandle nvh = st.createElement();
            BSetTypeImpl* rval =
              new BSetTypeImpl(BaseTypeFactory::makeTypeRaw(nvh), st.getBoundedSize());
            nvh.deleteValue();
            return rval;
        }
        case Type::MAP: {
            pair<ValueHandle, ValueHandle> nvh = ((const Map&)vh).createElement();
            MapTypeImpl* rval = new MapTypeImpl(BaseTypeFactory::makeTypeRaw(nvh.first),
                                                BaseTypeFactory::makeTypeRaw(nvh.second));
            nvh.first.deleteValue();
            nvh.second.deleteValue();
            return rval;
        }
        case Type::BMAP: {
            const BMap& mp = vh;
            pair<ValueHandle, ValueHandle> nvh = mp.createElement();
            BMapTypeImpl* rval =
              new BMapTypeImpl(BaseTypeFactory::makeTypeRaw(nvh.first),
                               BaseTypeFactory::makeTypeRaw(nvh.second), mp.getBoundedSize());
            nvh.first.deleteValue();
            nvh.second.deleteValue();
            return rval;
        }
        case Type::TUPLE: {
            const Tuple& tp = vh;
            TupleTypeImpl::Name2Index* name2index =
              new TupleTypeImpl::Name2Index(tp.getAttributeNames());
            TupleTypeImpl::Attributes* attrs = new TupleTypeImpl::Attributes();
            for (uint32_t i = 0, iu = tp.getNumberOfAttributes(); i < iu; ++i) {
                attrs->push_back(make_pair(tp.getAttributeName(i),
                                           &BaseTypeFactory::makeTypeRaw(tp.getAttributeValue(i))));
            }
            return new TupleTypeImpl(name2index, attrs);
        }
        default:
            assert(!"cannot happen");
    }
    return NULL;
}

namespace SPL {
namespace Meta {
ostream& operator<<(ostream& os, BaseTypeImpl const& t)
{
    return os << t.getName();
}
}
}

// BStringTypeImpl .......................................................
BStringTypeImpl::BStringTypeImpl(int32_t bound)
  : PrimitiveTypeImpl(makeName(bound), Type::BSTRING)
  , bound_(bound)
{}

string BStringTypeImpl::makeName(int32_t bound)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << Type::RSTRING;
    os << '[' << bound << ']';
    return os.str();
}

// ListTypeImpl ..........................................................
ListTypeImpl::ListTypeImpl(const BaseTypeImpl& etype)
  : CollectionTypeImpl(makeName(etype), Type::LIST)
  , etype_(etype)
{}

string ListTypeImpl::makeName(const BaseTypeImpl& e)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << Type::LIST << '<' << e.getName() << '>';
    return os.str();
}

// BListTypeImpl .........................................................
BListTypeImpl::BListTypeImpl(BaseTypeImpl const& etype, int32_t bound)
  : BCollectionTypeImpl(makeName(etype, bound), Type::BLIST, bound)
  , etype_(etype)
{}

string BListTypeImpl::makeName(BaseTypeImpl const& elemType, int32_t bound)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << ListTypeImpl::makeName(elemType) << '[' << bound << ']';
    return os.str();
}

// SetTypeImpl ..........................................................
SetTypeImpl::SetTypeImpl(BaseTypeImpl const& etype)
  : CollectionTypeImpl(makeName(etype), Type::SET)
  , etype_(etype)
{}

string SetTypeImpl::makeName(BaseTypeImpl const& e)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << Type::SET << '<' << e.getName() << '>';
    return os.str();
}

// BSetTypeImpl .........................................................
BSetTypeImpl::BSetTypeImpl(BaseTypeImpl const& etype, int32_t bound)
  : BCollectionTypeImpl(makeName(etype, bound), Type::BSET, bound)
  , etype_(etype)
{}

string BSetTypeImpl::makeName(BaseTypeImpl const& elemType, int32_t bound)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << SetTypeImpl::makeName(elemType) << '[' << bound << ']';
    return os.str();
}

// MapTypeImpl ..........................................................
MapTypeImpl::MapTypeImpl(BaseTypeImpl const& ktype, BaseTypeImpl const& vtype)
  : CollectionTypeImpl(makeName(ktype, vtype), Type::MAP)
  , ktype_(ktype)
  , vtype_(vtype)
{}

string MapTypeImpl::makeName(BaseTypeImpl const& k, BaseTypeImpl const& v)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << Type::MAP << '<' << k.getName() << ',' << v.getName() << '>';
    return os.str();
}

// BMapTypeImpl ..........................................................
BMapTypeImpl::BMapTypeImpl(BaseTypeImpl const& ktype, BaseTypeImpl const& vtype, int32_t bound)
  : BCollectionTypeImpl(makeName(ktype, vtype, bound), Type::BMAP, bound)
  , ktype_(ktype)
  , vtype_(vtype)
{}

string BMapTypeImpl::makeName(BaseTypeImpl const& keyType,
                              BaseTypeImpl const& valType,
                              int32_t bound)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << MapTypeImpl::makeName(keyType, valType) << '[' << bound << ']';
    return os.str();
}

// EnumTypeImpl ..........................................................
EnumTypeImpl::EnumTypeImpl(Values* values)
  : PrimitiveTypeImpl(makeName(values), Type::ENUM)
  , values_(values)
{}

string EnumTypeImpl::makeName(Values* values)
{
    stringstream os;
    os.imbue(locale::classic());
    os << Type::ENUM << '{';
    for (int i = 0, n = values->size(); i < n; i++) {
        if (0 != i) {
            os << ',';
        }
        os << (*values)[i];
    }
    os << '}';
    return os.str();
}

// TupleTypeImpl .........................................................
TupleTypeImpl::TupleTypeImpl(Name2Index* name2index, Attributes* attrs)
  : CompositeTypeImpl(makeName(attrs), Type::TUPLE)
  , name2index_(name2index)
  , attrs_(attrs)
{}

string TupleTypeImpl::makeName(Attributes* attrs)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << Type::TUPLE << '<';
    for (int i = 0, n = attrs->size(); i < n; i++) {
        if (0 != i) {
            os << ',';
        }
        os << (*attrs)[i].second->getName() << ' ' << (*attrs)[i].first;
    }
    os << '>';
    return os.str();
}

// SimplePrimitiveTypeImpl .........................................
SimplePrimitiveTypeImpl::SimplePrimitiveTypeImpl(Type const& meta)
  : PrimitiveTypeImpl(meta.toString(), meta)
{}

// XmlPrimitiveTypeImpl .........................................
XmlTypeImpl::XmlTypeImpl(xml const& v)
  : BaseTypeImpl(makeName(v), Type::XML)
  , schema_(v.getSchema())
{}

string XmlTypeImpl::makeName(xml const& v)
{
    if (v.hasSchema()) {
        return "xml<" + makeRStringLiteral(v.getSchema()) + ">";
    }
    return "xml";
}

// OptionalTypeImpl .........................................
OptionalTypeImpl::OptionalTypeImpl(const BaseTypeImpl& utype)
  : utype_(utype)
{}

std::string OptionalTypeImpl::makeName(const BaseTypeImpl& utype)
{
    return "optional<" + utype.getName() + ">";
}

};
};
