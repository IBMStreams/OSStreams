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

#include <SPL/FrontEnd/TypeFactory.h>

#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <algorithm>
#include <antlr3interfaces.h>

namespace SPL {
template class Singleton<TypeFactory>; // explicitly instantiate the singleton here

TypeFactory::TypeFactory()
  : Singleton<TypeFactory>(*this)
{}

EnumTyp const& TypeFactory::enumT(EnumTyp::Values& names)
{
    EnumTyp key(names);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new EnumTyp(names));
    }
    return (*_typeSet.find(&key))->as<EnumTyp>();
}

XMLTyp const& TypeFactory::xmlT()
{
    return xmlT("", 0);
}

XMLTyp const& TypeFactory::xmlT(std::string const& schema, uint32_t toolkitIndex_)
{
    XMLTyp key(schema, toolkitIndex_);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new XMLTyp(schema, toolkitIndex_));
    }
    return (*_typeSet.find(&key))->as<XMLTyp>();
}

EmptyCurlyTyp const& TypeFactory::emptyCurlyT()
{
    EmptyCurlyTyp key;
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new EmptyCurlyTyp());
    }
    return (*_typeSet.find(&key))->as<EmptyCurlyTyp>();
}

ListTyp const& TypeFactory::listT(RootTyp const& elemType)
{
    ListTyp key(elemType);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new ListTyp(elemType));
    }
    return (*_typeSet.find(&key))->as<ListTyp>();
}
BListTyp const& TypeFactory::listT(RootTyp const& elemType, TypeBound const& bound)
{
    BListTyp key(elemType, bound);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new BListTyp(elemType, bound));
    }
    return (*_typeSet.find(&key))->as<BListTyp>();
}
BListTyp const& TypeFactory::listT(RootTyp const& elemType, int32_t const bound)
{
    return listT(elemType, TypeBound(bound));
}
BListTyp const& TypeFactory::listT(RootTyp const& elemType, BoundsFormal const& bound)
{
    return listT(elemType, TypeBound(&bound));
}

MapTyp const& TypeFactory::mapT(RootTyp const& keyType, RootTyp const& valType)
{
    MapTyp key(keyType, valType);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new MapTyp(keyType, valType));
    }
    return (*_typeSet.find(&key))->as<MapTyp>();
}
BMapTyp const& TypeFactory::mapT(RootTyp const& keyType,
                                 RootTyp const& valType,
                                 TypeBound const& bound)
{
    BMapTyp key(keyType, valType, bound);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new BMapTyp(keyType, valType, bound));
    }
    return (*_typeSet.find(&key))->as<BMapTyp>();
}
BMapTyp const& TypeFactory::mapT(RootTyp const& keyType,
                                 RootTyp const& valType,
                                 int32_t const bound)
{
    return mapT(keyType, valType, TypeBound(bound));
}
BMapTyp const& TypeFactory::mapT(RootTyp const& keyType,
                                 RootTyp const& valType,
                                 BoundsFormal const& bound)
{
    return mapT(keyType, valType, TypeBound(&bound));
}

SimplePrimitiveTyp const& TypeFactory::numberT(char typeSuffix, int bitsSuffix)
{
    return primitiveT(numberMetaType(typeSuffix, bitsSuffix));
}

MetaType TypeFactory::numberMetaType(char typeSuffix, int bitsSuffix)
{
    switch (typeSuffix) {
        case 's':
            switch (bitsSuffix) {
                case 8:
                    return MetaType::INT8;
                case 16:
                    return MetaType::INT16;
                case 32:
                    return MetaType::INT32;
                case 64:
                    return MetaType::INT64;
                default:
                    return MetaType::INT32;
            }
            assert(false);
        case 'u':
            switch (bitsSuffix) {
                case 8:
                    return MetaType::UINT8;
                case 16:
                    return MetaType::UINT16;
                case 32:
                    return MetaType::UINT32;
                case 64:
                    return MetaType::UINT64;
                default:
                    return MetaType::UINT32;
            }
        case 'f':
            switch (bitsSuffix) {
                case 32:
                    return MetaType::FLOAT32;
                case 64:
                    return MetaType::FLOAT64;
                default:
                    return MetaType::FLOAT64;
            }
        case 'd':
            switch (bitsSuffix) {
                case 32:
                    return MetaType::DECIMAL32;
                case 64:
                    return MetaType::DECIMAL64;
                case 128:
                    return MetaType::DECIMAL128;
                default:
                    return MetaType::DECIMAL64;
            }
        default:
            assert(false);
    }
    return MetaType::INT32;
}

SimplePrimitiveTyp const& TypeFactory::primitiveT(const MetaType& metaType)
{
    SimplePrimitiveTyp key(metaType);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new SimplePrimitiveTyp(metaType));
    }
    return (*_typeSet.find(&key))->as<SimplePrimitiveTyp>();
}
SimplePrimitiveTyp const& TypeFactory::primitiveT(const std::string& typeName)
{
    if (false) {
        ; // empty first branch, allows all others to be "else if"
    } else if (typeName == "boolean") {
        return primitiveT(MetaType::BOOLEAN);
    } else if (typeName == "int8") {
        return primitiveT(MetaType::INT8);
    } else if (typeName == "int16") {
        return primitiveT(MetaType::INT16);
    } else if (typeName == "int32") {
        return primitiveT(MetaType::INT32);
    } else if (typeName == "int64") {
        return primitiveT(MetaType::INT64);
    } else if (typeName == "uint8") {
        return primitiveT(MetaType::UINT8);
    } else if (typeName == "uint16") {
        return primitiveT(MetaType::UINT16);
    } else if (typeName == "uint32") {
        return primitiveT(MetaType::UINT32);
    } else if (typeName == "uint64") {
        return primitiveT(MetaType::UINT64);
    } else if (typeName == "float32") {
        return primitiveT(MetaType::FLOAT32);
    } else if (typeName == "float64") {
        return primitiveT(MetaType::FLOAT64);
    } else if (typeName == "decimal32") {
        return primitiveT(MetaType::DECIMAL32);
    } else if (typeName == "decimal64") {
        return primitiveT(MetaType::DECIMAL64);
    } else if (typeName == "decimal128") {
        return primitiveT(MetaType::DECIMAL128);
    } else if (typeName == "complex32") {
        return primitiveT(MetaType::COMPLEX32);
    } else if (typeName == "complex64") {
        return primitiveT(MetaType::COMPLEX64);
    } else if (typeName == "timestamp") {
        return primitiveT(MetaType::TIMESTAMP);
    } else if (typeName == "rstring") {
        return primitiveT(MetaType::RSTRING);
    } else if (typeName == "ustring") {
        return primitiveT(MetaType::USTRING);
    } else if (typeName == "blob") {
        return primitiveT(MetaType::BLOB);
    } else if (typeName == "void") {
        return primitiveT(MetaType::VOID);
    } else if (typeName == "xml") {
        return primitiveT(MetaType::XML);
    }
    std::cerr << "TypeFactory::primitiveT called for unknown type " << typeName << std::endl;
    assert(false);
    return primitiveT(MetaType::UNKNOWN);
}

SimplePrimitiveTyp const& TypeFactory::rstringT()
{
    return primitiveT(MetaType::RSTRING);
}
BStringTyp const& TypeFactory::rstringT(TypeBound const& bound)
{
    BStringTyp key(bound);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new BStringTyp(bound));
    }
    return (*_typeSet.find(&key))->as<BStringTyp>();
}
BStringTyp const& TypeFactory::rstringT(int32_t const bound)
{
    return rstringT(TypeBound(bound));
}
BStringTyp const& TypeFactory::rstringT(BoundsFormal const& bound)
{
    return rstringT(TypeBound(&bound));
}

SetTyp const& TypeFactory::setT(RootTyp const& elemType)
{
    SetTyp key(elemType);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new SetTyp(elemType));
    }
    return (*_typeSet.find(&key))->as<SetTyp>();
}
BSetTyp const& TypeFactory::setT(RootTyp const& elemType, TypeBound const& bound)
{
    BSetTyp key(elemType, bound);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new BSetTyp(elemType, bound));
    }
    return (*_typeSet.find(&key))->as<BSetTyp>();
}
BSetTyp const& TypeFactory::setT(RootTyp const& elemType, int32_t const bound)
{
    return setT(elemType, TypeBound(bound));
}
BSetTyp const& TypeFactory::setT(RootTyp const& elemType, BoundsFormal const& bound)
{
    return setT(elemType, TypeBound(&bound));
}

TupleTyp const& TypeFactory::tupleT(TupleTyp::Attributes& attributes)
{
    assert(0 < attributes.size());
    TupleTyp::Name2Index name2index;
    for (int i = 0, n = attributes.size(); i < n; i++) {
        name2index[attributes.at(i).first] = i;
    }
    TupleTyp key(name2index, attributes);
    TypeSet::iterator iter = _typeSet.find(&key);
    if (_typeSet.end() == iter) {
        TupleTyp* result = new TupleTyp(name2index, attributes);
        _typeSet.insert(result);
        return *result;
    } else {
        return (*iter)->as<TupleTyp>();
    }
}
TupleTyp const& TypeFactory::tupleT(std::vector<std::string>& names,
                                    std::vector<RootTyp const*>& types)
{
    assert(names.size() == types.size());
    TupleTyp::Attributes attributes;
    for (int i = 0, n = names.size(); i < n; i++) {
        attributes.push_back(std::make_pair(names[i], types[i]));
    }
    return tupleT(attributes);
}
TupleTyp const& TypeFactory::tupleT(Scope& scope)
{
    TupleTyp::Attributes attributes;
    for (int i = 0, n = scope.size(); i < n; i++) {
        SymbolTableEntry& attr = scope.get(i);
        RootTyp const& type = attr.type();
        attributes.push_back(std::make_pair(attr.name(), &type));
    }
    TupleTyp const& result = tupleT(attributes);
    assert(scope.size() == (int)result.getNumberOfAttributes());
    return result;
}

TypeFormal const& TypeFactory::typeFormalT(FunctionHeadSym const* functionHead,
                                           std::string const& constraint,
                                           std::string const& identifier)
{
    TypeFormal key(functionHead, constraint, identifier);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new TypeFormal(functionHead, constraint, identifier));
    }
    return (*_typeSet.find(&key))->as<TypeFormal>();
}

SimplePrimitiveTyp const& TypeFactory::unknownT()
{
    return primitiveT(MetaType::UNKNOWN);
}

SimplePrimitiveTyp const& TypeFactory::ustringT()
{
    return primitiveT(MetaType::USTRING);
}

SimplePrimitiveTyp const& TypeFactory::voidT()
{
    return primitiveT(MetaType::VOID);
}

SimplePrimitiveTyp const& TypeFactory::nullT()
{
    return primitiveT(MetaType::NUL);
}

OptionalTyp const& TypeFactory::optionalT(const RootTyp& underlyingType)
{
    OptionalTyp key(underlyingType);
    if (_typeSet.end() == _typeSet.find(&key)) {
        _typeSet.insert(new OptionalTyp(underlyingType));
    }
    return (*_typeSet.find(&key))->as<OptionalTyp>();
}
};
