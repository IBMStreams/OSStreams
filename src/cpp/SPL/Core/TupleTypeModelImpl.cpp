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

#include <SPL/Core/TupleTypeModelImpl.h>
#include <SPL/Core/tupleTypeModel.h>
#include <SPL/FrontEnd/Typ.h>

using namespace std;
using namespace SPL;
using namespace SPL::TupleType;

static bool isScalar(const RootTyp& type)
{
    switch (type.getMetaType()) {
        case MetaType::TUPLE:
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::SET:
        case MetaType::BSET:
        case MetaType::MAP:
        case MetaType::BMAP:
        case MetaType::OPTIONAL:
            return false;
        default:;
    }
    return true;
}

static auto_ptr<tupletypemodel::listOrSetType> doListOrSet(const RootTyp& type);
static auto_ptr<tupletypemodel::typeType> doTypeType(const RootTyp& type);
static auto_ptr<tupletypemodel::tupleType> doTuple(const TupleTyp& type);
static auto_ptr<tupletypemodel::mapType> doMap(const RootTyp& type);
static auto_ptr<tupletypemodel::optionalType> doOptional(const OptionalTyp& type);

static void doElementType(tupletypemodel::listOrSetType& lst, const RootTyp& type)
{
    switch (type.getMetaType()) {
        case MetaType::TUPLE:
            lst.tuple(doTuple(type.as<TupleTyp>()));
            break;
        case MetaType::LIST:
        case MetaType::BLIST:
            lst.list(doListOrSet(type));
            break;
        case MetaType::SET:
        case MetaType::BSET:
            lst.set(doListOrSet(type));
            break;
        case MetaType::MAP:
        case MetaType::BMAP:
            lst.map(doMap(type));
            break;
        case MetaType::OPTIONAL:
            lst.optional(doOptional(type.as<OptionalTyp>()));
            break;
        default:;
            break;
    }
}

static void doElementType(tupletypemodel::optionalType& ot, const RootTyp& type)
{
    switch (type.getMetaType()) {
        case MetaType::TUPLE:
            ot.tuple(doTuple(type.as<TupleTyp>()));
            break;
        case MetaType::LIST:
        case MetaType::BLIST:
            ot.list(doListOrSet(type));
            break;
        case MetaType::SET:
        case MetaType::BSET:
            ot.set(doListOrSet(type));
            break;
        case MetaType::MAP:
        case MetaType::BMAP:
            ot.map(doMap(type));
            break;
        default:;
            break;
    }
}

static auto_ptr<tupletypemodel::optionalType> doOptional(const OptionalTyp& type)
{
    auto_ptr<tupletypemodel::optionalType> ot(new tupletypemodel::optionalType);
    const RootTyp& underlyingType = type.getUnderlyingType();
    if (isScalar(underlyingType)) {
        ot->type(underlyingType.getName());
    } else {
        doElementType(*ot, underlyingType);
    }

    return ot;
}

static auto_ptr<tupletypemodel::listOrSetType> doListOrSet(const RootTyp& type)
{
    auto_ptr<tupletypemodel::listOrSetType> ls(new tupletypemodel::listOrSetType);
    switch (type.getMetaType()) {
        case MetaType::LIST: {
            const ListTyp& lt = type.as<ListTyp>();
            const RootTyp& elem = lt.getElementType();
            if (isScalar(elem))
                ls->elementType(elem.getName());
            else
                doElementType(*ls, elem);
        } break;
        case MetaType::BLIST: {
            const BListTyp& lt = type.as<BListTyp>();
            const RootTyp& elem = lt.getElementType();
            ls->bound(lt.getBoundSize());
            if (isScalar(elem))
                ls->elementType(elem.getName());
            else
                doElementType(*ls, elem);
        } break;
        case MetaType::SET: {
            const SetTyp& st = type.as<SetTyp>();
            const RootTyp& elem = st.getElementType();
            if (isScalar(elem))
                ls->elementType(elem.getName());
            else
                doElementType(*ls, elem);
        } break;
        case MetaType::BSET: {
            const BSetTyp& lt = type.as<BSetTyp>();
            const RootTyp& elem = lt.getElementType();
            ls->bound(lt.getBoundSize());
            if (isScalar(elem))
                ls->elementType(elem.getName());
            else
                doElementType(*ls, elem);
        } break;
        default:;
            break;
    }
    return ls;
}

static auto_ptr<tupletypemodel::mapType> doMap(const RootTyp& type)
{
    auto_ptr<tupletypemodel::mapType> m(new tupletypemodel::mapType);
    if (type.getMetaType() == MetaType::MAP) {
        const MapTyp& mt = type.as<MapTyp>();
        const RootTyp& key = mt.getKeyType();
        if (isScalar(key))
            m->keyType(key.getName());
        else
            m->key(doTypeType(key));
        const RootTyp& value = mt.getValueType();
        if (isScalar(value))
            m->valueType(value.getName());
        else
            m->value(doTypeType(value));
    } else {
        const BMapTyp& mt = type.as<BMapTyp>();
        m->bound(mt.getBoundSize());
        const RootTyp& key = mt.getKeyType();
        if (isScalar(key))
            m->keyType(key.getName());
        else
            m->key(doTypeType(key));
        const RootTyp& value = mt.getValueType();
        if (isScalar(value))
            m->valueType(value.getName());
        else
            m->value(doTypeType(value));
    }
    return m;
}

static void doElementType(tupletypemodel::attrType& at, const RootTyp& type)
{
    switch (type.getMetaType()) {
        case MetaType::TUPLE:
            at.tuple(doTuple(type.as<TupleTyp>()));
            break;
        case MetaType::LIST:
        case MetaType::BLIST:
            at.list(doListOrSet(type));
            break;
        case MetaType::SET:
        case MetaType::BSET:
            at.set(doListOrSet(type));
            break;
        case MetaType::MAP:
        case MetaType::BMAP:
            at.map(doMap(type));
            break;
        case MetaType::OPTIONAL:
            at.optional(doOptional(type.as<OptionalTyp>()));
        default:;
            break;
    }
}

static auto_ptr<tupletypemodel::tupleType> doTuple(const TupleTyp& type)
{
    auto_ptr<tupletypemodel::tupleType> t(new tupletypemodel::tupleType);
    for (uint32_t i = 0, ui = type.getNumberOfAttributes(); i < ui; i++) {
        auto_ptr<tupletypemodel::attrType> at(new tupletypemodel::attrType());
        at->name(type.getAttributeName(i));
        const RootTyp& attrType = type.getAttributeType(i);
        if (isScalar(attrType))
            at->type(attrType.getName());
        else {
            // need to create the subtype
            doElementType(*at, attrType);
        }
        t->attr().push_back(at);
    }
    return t;
}

static auto_ptr<tupletypemodel::typeType> doTypeType(const RootTyp& type)
{
    auto_ptr<tupletypemodel::typeType> tt(new tupletypemodel::typeType);
    switch (type.getMetaType()) {
        case MetaType::TUPLE:
            tt->tuple(doTuple(type.as<TupleTyp>()));
            break;
        case MetaType::LIST:
        case MetaType::BLIST:
            tt->list(doListOrSet(type));
            break;
        case MetaType::SET:
        case MetaType::BSET:
            tt->set(doListOrSet(type));
            break;
        case MetaType::MAP:
        case MetaType::BMAP:
            tt->map(doMap(type));
            break;
        case MetaType::OPTIONAL:
            tt->optional(doOptional(type.as<OptionalTyp>()));
            break;
        default:;
    }
    return tt;
}

auto_ptr<tupletypemodel::tupleType> TupleTypeModel::toXsdInstance() const
{
    return doTuple(_tuple);
}
