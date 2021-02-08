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

#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Serialization/WrapperByteBuffer.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Type/ValueHandle.h>

#define IMPL_VALUE_HANDLE_IS_PRESENT(type)

#define IMPL_VALUE_HANDLE_READ(type)                                                               \
    ValueHandle::operator type const&() const                                                      \
    {                                                                                              \
        if (__builtin_expect((Meta::Type::typeOf<type>() != type_), 0))                            \
            SPLTRACEMSGANDTHROW(                                                                   \
              SPLRuntimeTypeMismatch, L_ERROR,                                                     \
              SPL_RUNTIME_VALUEHANDLE_INVALID_CONVERSION(Meta::Type::typeOf<type>().toString(),    \
                                                         Meta::Type(type_).toString()),            \
              SPL_FUNC_DBG);                                                                       \
        return *static_cast<type const*>(value_);                                                  \
    }

#define IMPL_VALUE_HANDLE_READ_SIMPLETYPE(type, spltype)                                           \
    ValueHandle::operator type const&() const                                                      \
    {                                                                                              \
        if (__builtin_expect((Meta::Type::spltype != type_), 0))                                   \
            SPLTRACEMSGANDTHROW(                                                                   \
              SPLRuntimeTypeMismatch, L_ERROR,                                                     \
              SPL_RUNTIME_VALUEHANDLE_INVALID_CONVERSION(Meta::Type::typeOf<type>().toString(),    \
                                                         Meta::Type(type_).toString()),            \
              SPL_FUNC_DBG);                                                                       \
        return *static_cast<type const*>(value_);                                                  \
    }

#define IMPL_CONST_VALUE_HANDLE_READ(type)                                                         \
    ConstValueHandle::operator type const&() const                                                 \
    {                                                                                              \
        if (__builtin_expect((Meta::Type::typeOf<type>() != type_), 0))                            \
            SPLTRACEMSGANDTHROW(                                                                   \
              SPLRuntimeTypeMismatch, L_ERROR,                                                     \
              SPL_RUNTIME_VALUEHANDLE_INVALID_CONVERSION(Meta::Type::typeOf<type>().toString(),    \
                                                         Meta::Type(type_).toString()),            \
              SPL_FUNC_DBG);                                                                       \
        return *static_cast<type const*>(value_);                                                  \
    }

#define IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(type, spltype)                                     \
    ConstValueHandle::operator type const&() const                                                 \
    {                                                                                              \
        if (__builtin_expect((Meta::Type::spltype != type_), 0)) {                                 \
            if (Meta::Type::OPTIONAL == type_) {                                                   \
                const Optional& o = *this;                                                         \
                if (o.getValueMetaType() == Meta::Type::spltype) {                                 \
                    return static_cast<type const&>(o.getValue());                                 \
                }                                                                                  \
            }                                                                                      \
            SPLTRACEMSGANDTHROW(                                                                   \
              SPLRuntimeTypeMismatch, L_ERROR,                                                     \
              SPL_RUNTIME_VALUEHANDLE_INVALID_CONVERSION(Meta::Type::typeOf<type>().toString(),    \
                                                         Meta::Type(type_).toString()),            \
              SPL_FUNC_DBG);                                                                       \
        }                                                                                          \
        return *static_cast<type const*>(value_);                                                  \
    }

#define IMPL_VALUE_HANDLE_WRITE(type)                                                              \
    ValueHandle::operator type&()                                                                  \
    {                                                                                              \
        if (__builtin_expect((Meta::Type::typeOf<type>() != type_), 0))                            \
            SPLTRACEMSGANDTHROW(                                                                   \
              SPLRuntimeTypeMismatch, L_ERROR,                                                     \
              SPL_RUNTIME_VALUEHANDLE_INVALID_CONVERSION(Meta::Type::typeOf<type>().toString(),    \
                                                         Meta::Type(type_).toString()),            \
              SPL_FUNC_DBG);                                                                       \
        return *static_cast<type*>(value_);                                                        \
    }

#define IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(type, spltype)                                          \
    ValueHandle::operator type&()                                                                  \
    {                                                                                              \
        if (__builtin_expect((Meta::Type::spltype != type_), 0))                                   \
            SPLTRACEMSGANDTHROW(                                                                   \
              SPLRuntimeTypeMismatch, L_ERROR,                                                     \
              SPL_RUNTIME_VALUEHANDLE_INVALID_CONVERSION(Meta::Type::typeOf<type>().toString(),    \
                                                         Meta::Type(type_).toString()),            \
              SPL_FUNC_DBG);                                                                       \
        return *static_cast<type*>(value_);                                                        \
    }

#define IMPL_CONST_VALUE_HANDLE_WRITE(type)                                                                                                                                              \
    ConstValueHandle::operator type&()                                                                                                                                                   \
    {                                                                                                                                                                                    \
        if (__builtin_expect(Meta::Type::typeOf<type>() != type_), 0)) \
    SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_RUNTIME_VALUEHANDLE_INVALID_CONVERSION(Meta::Type::typeOf<type>().toString(), Meta::Type(type_).toString()), SPL_FUNC_DBG); \
        return *static_cast<type*>(value_);                                                                                                                                              \
    }

#define VALUE_HANDLE_SET(type)                                                                     \
    {                                                                                              \
        type& v = *this;                                                                           \
        v = (type const&)oh;                                                                       \
    }                                                                                              \
    break;

#define VALUE_HANDLE_SWAP_N(type)                                                                  \
    {                                                                                              \
        type& v = *this;                                                                           \
        type& ov = oh;                                                                             \
        std::swap(v, ov);                                                                          \
    }                                                                                              \
    break;

#define VALUE_HANDLE_SWAP_M(type)                                                                  \
    {                                                                                              \
        type& v = *this;                                                                           \
        type& ov = oh;                                                                             \
        v.swapWith(ov);                                                                            \
    }                                                                                              \
    break;

#define VALUE_HANDLE_SERIALIZE(type)                                                               \
    {                                                                                              \
        type const& v = *this;                                                                     \
        ostr << v;                                                                                 \
    }                                                                                              \
    break;

#define VALUE_HANDLE_SERIALIZE_WP(type)                                                            \
    {                                                                                              \
        type const& v = *this;                                                                     \
        SPL::serializeWithPrecision(ostr, v);                                                      \
    }                                                                                              \
    break;

#define VALUE_HANDLE_DESERIALIZE(type)                                                             \
    {                                                                                              \
        type& v = *this;                                                                           \
        istr >> v;                                                                                 \
    }                                                                                              \
    break;

#define VALUE_HANDLE_DESERIALIZE_WS(type)                                                          \
    {                                                                                              \
        type& v = *this;                                                                           \
        SPL::deserializeWithSuffix(istr, v);                                                       \
    }                                                                                              \
    break;

#define VALUE_HANDLE_DESERIALIZE_IN(type)                                                          \
    {                                                                                              \
        type& v = *this;                                                                           \
        SPL::deserializeWithNanAndInfs(istr, v, withSuffix);                                       \
    }                                                                                              \
    break;

#define VALUE_HANDLE_GET_SIZE_N(type)                                                              \
    {                                                                                              \
        type const& v = *this;                                                                     \
        return sizeof(v);                                                                          \
    }

#define VALUE_HANDLE_GET_SIZE_M(type)                                                              \
    {                                                                                              \
        type const& v = *this;                                                                     \
        return v.getSerializedSize();                                                              \
    }

#define VALUE_HANDLE_COMPARE(type)                                                                 \
    {                                                                                              \
        type const& v = *this;                                                                     \
        return v == (type const&)oh;                                                               \
    }

#define VALUE_HANDLE_NEW(type)                                                                     \
    {                                                                                              \
        type const& v = *this;                                                                     \
        return ValueHandle(new type(v), type_, tpid_);                                             \
    }

#define VALUE_HANDLE_NEW_CLONE(type)                                                               \
    {                                                                                              \
        type const& v = *this;                                                                     \
        return ValueHandle(v.clone(), type_, tpid_);                                               \
    }

#define VALUE_HANDLE_DELETE(type)                                                                  \
    {                                                                                              \
        delete static_cast<type*>(value_);                                                         \
    }                                                                                              \
    break;

using namespace SPL;
using namespace std;

IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(blob, BLOB)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(float32, FLOAT32)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(float64, FLOAT64)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(decimal32, DECIMAL32)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(decimal64, DECIMAL64)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(decimal128, DECIMAL128)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(complex32, COMPLEX32)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(complex64, COMPLEX64)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(boolean, BOOLEAN)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(uint8, UINT8)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(uint16, UINT16)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(uint32, UINT32)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(uint64, UINT64)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(int8, INT8)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(int16, INT16)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(int32, INT32)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(int64, INT64)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(rstring, RSTRING)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(ustring, USTRING)
IMPL_CONST_VALUE_HANDLE_READ_SIMPLETYPE(timestamp, TIMESTAMP)
IMPL_CONST_VALUE_HANDLE_READ(BString)
IMPL_CONST_VALUE_HANDLE_READ(List)
IMPL_CONST_VALUE_HANDLE_READ(BList)
IMPL_CONST_VALUE_HANDLE_READ(Map)
IMPL_CONST_VALUE_HANDLE_READ(BMap)
IMPL_CONST_VALUE_HANDLE_READ(Set)
IMPL_CONST_VALUE_HANDLE_READ(BSet)
IMPL_CONST_VALUE_HANDLE_READ(Tuple)
IMPL_CONST_VALUE_HANDLE_READ(Enum)
IMPL_CONST_VALUE_HANDLE_READ(xml)
IMPL_CONST_VALUE_HANDLE_READ(Optional)

IMPL_VALUE_HANDLE_READ_SIMPLETYPE(blob, BLOB)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(float32, FLOAT32)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(float64, FLOAT64)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(decimal32, DECIMAL32)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(decimal64, DECIMAL64)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(decimal128, DECIMAL128)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(complex32, COMPLEX32)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(complex64, COMPLEX64)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(boolean, BOOLEAN)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(uint8, UINT8)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(uint16, UINT16)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(uint32, UINT32)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(uint64, UINT64)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(int8, INT8)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(int16, INT16)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(int32, INT32)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(int64, INT64)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(rstring, RSTRING)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(ustring, USTRING)
IMPL_VALUE_HANDLE_READ_SIMPLETYPE(timestamp, TIMESTAMP)
IMPL_VALUE_HANDLE_READ(BString)
IMPL_VALUE_HANDLE_READ(List)
IMPL_VALUE_HANDLE_READ(BList)
IMPL_VALUE_HANDLE_READ(Map)
IMPL_VALUE_HANDLE_READ(BMap)
IMPL_VALUE_HANDLE_READ(Set)
IMPL_VALUE_HANDLE_READ(BSet)
IMPL_VALUE_HANDLE_READ(Tuple)
IMPL_VALUE_HANDLE_READ(Enum)
IMPL_VALUE_HANDLE_READ(xml)
IMPL_VALUE_HANDLE_READ(Optional)

IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(blob, BLOB)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(float32, FLOAT32)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(float64, FLOAT64)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(decimal32, DECIMAL32)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(decimal64, DECIMAL64)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(decimal128, DECIMAL128)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(complex32, COMPLEX32)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(complex64, COMPLEX64)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(boolean, BOOLEAN)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(uint8, UINT8)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(uint16, UINT16)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(uint32, UINT32)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(uint64, UINT64)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(int8, INT8)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(int16, INT16)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(int32, INT32)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(int64, INT64)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(rstring, RSTRING)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(ustring, USTRING)
IMPL_VALUE_HANDLE_WRITE_SIMPLETYPE(timestamp, TIMESTAMP)
IMPL_VALUE_HANDLE_WRITE(BString)
IMPL_VALUE_HANDLE_WRITE(List)
IMPL_VALUE_HANDLE_WRITE(BList)
IMPL_VALUE_HANDLE_WRITE(Map)
IMPL_VALUE_HANDLE_WRITE(BMap)
IMPL_VALUE_HANDLE_WRITE(Set)
IMPL_VALUE_HANDLE_WRITE(BSet)
IMPL_VALUE_HANDLE_WRITE(Tuple)
IMPL_VALUE_HANDLE_WRITE(Enum)
IMPL_VALUE_HANDLE_WRITE(xml)
IMPL_VALUE_HANDLE_WRITE(Optional)

template<class T>
inline size_t stdHash(const ValueHandle& handle)
{
    return std::tr1::hash<T>()(static_cast<const T&>(handle));
}

template<class T>
inline size_t splHash(const ValueHandle& handle)
{
    return static_cast<const T&>(handle).hashCode();
}

size_t ValueHandle::hashCode() const
{
    switch (type_) {
        case SPL::Meta::Type::BLOB:
            return stdHash<SPL::blob>(*this);
        case SPL::Meta::Type::FLOAT32:
            return stdHash<SPL::float32>(*this);
        case SPL::Meta::Type::FLOAT64:
            return stdHash<SPL::float64>(*this);
        case SPL::Meta::Type::DECIMAL32:
            return stdHash<SPL::decimal32>(*this);
        case SPL::Meta::Type::DECIMAL64:
            return stdHash<SPL::decimal64>(*this);
        case SPL::Meta::Type::DECIMAL128:
            return stdHash<SPL::decimal128>(*this);
        case SPL::Meta::Type::COMPLEX32:
            return stdHash<SPL::complex32>(*this);
        case SPL::Meta::Type::COMPLEX64:
            return stdHash<SPL::complex64>(*this);
        case SPL::Meta::Type::BOOLEAN:
            return stdHash<SPL::boolean>(*this);
        case SPL::Meta::Type::UINT8:
            return stdHash<SPL::uint8>(*this);
        case SPL::Meta::Type::UINT16:
            return stdHash<SPL::uint16>(*this);
        case SPL::Meta::Type::UINT32:
            return stdHash<SPL::uint32>(*this);
        case SPL::Meta::Type::UINT64:
            return stdHash<SPL::uint64>(*this);
        case SPL::Meta::Type::INT8:
            return stdHash<SPL::int8>(*this);
        case SPL::Meta::Type::INT16:
            return stdHash<SPL::int16>(*this);
        case SPL::Meta::Type::INT32:
            return stdHash<SPL::int32>(*this);
        case SPL::Meta::Type::INT64:
            return stdHash<SPL::int64>(*this);
        case SPL::Meta::Type::RSTRING:
            return stdHash<SPL::rstring>(*this);
        case SPL::Meta::Type::USTRING:
            return stdHash<SPL::ustring>(*this);
        case SPL::Meta::Type::TIMESTAMP:
            return stdHash<SPL::timestamp>(*this);
        case SPL::Meta::Type::BSTRING:
            return splHash<SPL::BString>(*this);
        case SPL::Meta::Type::LIST:
            return splHash<SPL::List>(*this);
        case SPL::Meta::Type::BLIST:
            return splHash<SPL::BList>(*this);
        case SPL::Meta::Type::MAP:
            return splHash<SPL::Map>(*this);
        case SPL::Meta::Type::BMAP:
            return splHash<SPL::BMap>(*this);
        case SPL::Meta::Type::SET:
            return splHash<SPL::Set>(*this);
        case SPL::Meta::Type::BSET:
            return splHash<SPL::BSet>(*this);
        case SPL::Meta::Type::TUPLE:
            return splHash<SPL::Tuple>(*this);
        case SPL::Meta::Type::ENUM:
            return splHash<SPL::Enum>(*this);
        case SPL::Meta::Type::XML:
            return stdHash<SPL::xml>(*this);
        case SPL::Meta::Type::OPTIONAL:
            return splHash<SPL::Optional>(*this);
        default:
            assert(!"cannot happen");
    }
    return 0;
}

bool ValueHandle::assignFrom(const ConstValueHandle& oh)
{
    if (!SPL::equals(getTypeId(), oh.getTypeId())) {
        return false;
    }
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_SET(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_SET(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_SET(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_SET(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_SET(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_SET(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_SET(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_SET(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_SET(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_SET(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_SET(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_SET(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_SET(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_SET(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_SET(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_SET(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_SET(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_SET(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_SET(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_SET(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_SET(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_SET(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_SET(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_SET(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_SET(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_SET(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_SET(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_SET(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_SET(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_SET(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_SET(Optional);
        default:
            assert(!"cannot happen");
    }
    return true;
}

bool ValueHandle::swapWith(ValueHandle& oh)
{
    if (!SPL::equals(getTypeId(), oh.getTypeId())) {
        return false;
    }
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_SWAP_N(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_SWAP_N(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_SWAP_N(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_SWAP_N(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_SWAP_N(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_SWAP_N(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_SWAP_N(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_SWAP_N(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_SWAP_N(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_SWAP_N(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_SWAP_N(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_SWAP_N(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_SWAP_N(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_SWAP_N(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_SWAP_N(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_SWAP_N(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_SWAP_N(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_SWAP_N(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_SWAP_N(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_SWAP_N(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_SWAP_M(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_SWAP_M(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_SWAP_M(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_SWAP_M(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_SWAP_M(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_SWAP_M(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_SWAP_M(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_SWAP_M(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_SWAP_M(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_SWAP_N(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_SWAP_M(Optional);
        default:
            assert(!"cannot happen");
    }
    return true;
}

string ValueHandle::toString() const
{
    ostringstream ostr;
    ostr.imbue(locale::classic());
    this->serializeWithPrecision(ostr);
    return ostr.str();
}

bool ValueHandle::equals(const ConstValueHandle& oh) const
{
    if (!SPL::equals(getTypeId(), oh.getTypeId())) {
        return false;
    }
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_COMPARE(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_COMPARE(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_COMPARE(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_COMPARE(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_COMPARE(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_COMPARE(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_COMPARE(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_COMPARE(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_COMPARE(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_COMPARE(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_COMPARE(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_COMPARE(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_COMPARE(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_COMPARE(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_COMPARE(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_COMPARE(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_COMPARE(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_COMPARE(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_COMPARE(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_COMPARE(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_COMPARE(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_COMPARE(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_COMPARE(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_COMPARE(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_COMPARE(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_COMPARE(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_COMPARE(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_COMPARE(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_COMPARE(Enum);
        case Meta::Type::XML:
            return false; // can't compare xml
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_COMPARE(Optional);
        default:
            assert(!"cannot happen");
    }
    return false;
}

void ValueHandle::deleteValue() const
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_DELETE(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_DELETE(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_DELETE(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_DELETE(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_DELETE(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_DELETE(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_DELETE(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_DELETE(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_DELETE(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_DELETE(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_DELETE(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_DELETE(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_DELETE(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_DELETE(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_DELETE(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_DELETE(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_DELETE(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_DELETE(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_DELETE(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_DELETE(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_DELETE(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_DELETE(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_DELETE(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_DELETE(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_DELETE(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_DELETE(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_DELETE(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_DELETE(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_DELETE(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_DELETE(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_DELETE(Optional);
        default:
            assert(!"cannot happen");
    }
}

ValueHandle ValueHandle::newValue() const
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_NEW(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_NEW(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_NEW(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_NEW(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_NEW(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_NEW(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_NEW(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_NEW(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_NEW(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_NEW(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_NEW(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_NEW(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_NEW(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_NEW(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_NEW(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_NEW(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_NEW(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_NEW(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_NEW(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_NEW(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_NEW_CLONE(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_NEW_CLONE(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_NEW_CLONE(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_NEW_CLONE(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_NEW_CLONE(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_NEW_CLONE(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_NEW_CLONE(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_NEW_CLONE(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_NEW_CLONE(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_NEW(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_NEW_CLONE(Optional);
        default:
            assert(!"cannot happen");
    }
    return ValueHandle(NULL, Meta::Type::INVALID, NULL);
}

void ValueHandle::serialize(ostream& ostr) const
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_SERIALIZE(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_SERIALIZE(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_SERIALIZE(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_SERIALIZE(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_SERIALIZE(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_SERIALIZE(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_SERIALIZE(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_SERIALIZE(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_SERIALIZE(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_SERIALIZE(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_SERIALIZE(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_SERIALIZE(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_SERIALIZE(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_SERIALIZE(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_SERIALIZE(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_SERIALIZE(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_SERIALIZE(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_SERIALIZE(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_SERIALIZE(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_SERIALIZE(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_SERIALIZE(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_SERIALIZE(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_SERIALIZE(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_SERIALIZE(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_SERIALIZE(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_SERIALIZE(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_SERIALIZE(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_SERIALIZE(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_SERIALIZE(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_SERIALIZE(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_SERIALIZE(Optional);
        default:
            assert(!"cannot happen");
    }
}

void ValueHandle::serializeWithPrecision(ostream& ostr) const
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_SERIALIZE_WP(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_SERIALIZE_WP(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_SERIALIZE_WP(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_SERIALIZE_WP(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_SERIALIZE_WP(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_SERIALIZE_WP(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_SERIALIZE_WP(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_SERIALIZE_WP(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_SERIALIZE_WP(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_SERIALIZE_WP(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_SERIALIZE_WP(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_SERIALIZE_WP(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_SERIALIZE_WP(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_SERIALIZE_WP(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_SERIALIZE_WP(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_SERIALIZE_WP(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_SERIALIZE_WP(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_SERIALIZE_WP(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_SERIALIZE_WP(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_SERIALIZE_WP(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_SERIALIZE_WP(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_SERIALIZE_WP(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_SERIALIZE_WP(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_SERIALIZE_WP(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_SERIALIZE_WP(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_SERIALIZE_WP(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_SERIALIZE_WP(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_SERIALIZE_WP(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_SERIALIZE_WP(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_SERIALIZE_WP(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_SERIALIZE_WP(Optional);
        default:
            assert(!"cannot happen");
    }
}

void ValueHandle::serialize(VirtualByteBuffer& ostr) const
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_SERIALIZE(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_SERIALIZE(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_SERIALIZE(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_SERIALIZE(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_SERIALIZE(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_SERIALIZE(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_SERIALIZE(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_SERIALIZE(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_SERIALIZE(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_SERIALIZE(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_SERIALIZE(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_SERIALIZE(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_SERIALIZE(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_SERIALIZE(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_SERIALIZE(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_SERIALIZE(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_SERIALIZE(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_SERIALIZE(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_SERIALIZE(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_SERIALIZE(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_SERIALIZE(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_SERIALIZE(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_SERIALIZE(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_SERIALIZE(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_SERIALIZE(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_SERIALIZE(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_SERIALIZE(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_SERIALIZE(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_SERIALIZE(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_SERIALIZE(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_SERIALIZE(Optional);
        default:
            assert(!"cannot happen");
    }
}

void ValueHandle::serialize(NetworkByteBuffer& ostr) const
{
    WrapperByteBuffer<NetworkByteBuffer> x(ostr);
    serialize(x);
}

void ValueHandle::serialize(NativeByteBuffer& ostr) const
{
    WrapperByteBuffer<NativeByteBuffer> x(ostr);
    serialize(x);
}

void ValueHandle::deserialize(istream& istr)
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_DESERIALIZE(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_DESERIALIZE(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_DESERIALIZE(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_DESERIALIZE(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_DESERIALIZE(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_DESERIALIZE(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_DESERIALIZE(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_DESERIALIZE(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_DESERIALIZE(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_DESERIALIZE(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_DESERIALIZE(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_DESERIALIZE(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_DESERIALIZE(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_DESERIALIZE(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_DESERIALIZE(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_DESERIALIZE(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_DESERIALIZE(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_DESERIALIZE(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_DESERIALIZE(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_DESERIALIZE(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_DESERIALIZE(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_DESERIALIZE(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_DESERIALIZE(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_DESERIALIZE(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_DESERIALIZE(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_DESERIALIZE(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_DESERIALIZE(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_DESERIALIZE(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_DESERIALIZE(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_DESERIALIZE(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_DESERIALIZE(Optional);
        default:
            assert(!"cannot happen");
    }
}

void ValueHandle::deserialize(VirtualByteBuffer& istr)
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_DESERIALIZE(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_DESERIALIZE(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_DESERIALIZE(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_DESERIALIZE(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_DESERIALIZE(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_DESERIALIZE(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_DESERIALIZE(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_DESERIALIZE(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_DESERIALIZE(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_DESERIALIZE(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_DESERIALIZE(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_DESERIALIZE(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_DESERIALIZE(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_DESERIALIZE(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_DESERIALIZE(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_DESERIALIZE(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_DESERIALIZE(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_DESERIALIZE(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_DESERIALIZE(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_DESERIALIZE(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_DESERIALIZE(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_DESERIALIZE(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_DESERIALIZE(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_DESERIALIZE(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_DESERIALIZE(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_DESERIALIZE(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_DESERIALIZE(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_DESERIALIZE(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_DESERIALIZE(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_DESERIALIZE(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_DESERIALIZE(Optional);
        default:
            assert(!"cannot happen");
    }
}

void ValueHandle::deserializeWithSuffix(istream& istr)
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_DESERIALIZE_WS(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_DESERIALIZE_WS(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_DESERIALIZE_WS(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_DESERIALIZE_WS(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_DESERIALIZE_WS(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_DESERIALIZE_WS(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_DESERIALIZE_WS(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_DESERIALIZE_WS(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_DESERIALIZE_WS(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_DESERIALIZE_WS(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_DESERIALIZE_WS(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_DESERIALIZE_WS(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_DESERIALIZE_WS(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_DESERIALIZE_WS(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_DESERIALIZE_WS(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_DESERIALIZE_WS(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_DESERIALIZE_WS(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_DESERIALIZE_WS(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_DESERIALIZE_WS(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_DESERIALIZE_WS(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_DESERIALIZE_WS(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_DESERIALIZE_WS(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_DESERIALIZE_WS(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_DESERIALIZE_WS(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_DESERIALIZE_WS(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_DESERIALIZE_WS(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_DESERIALIZE_WS(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_DESERIALIZE_WS(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_DESERIALIZE_WS(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_DESERIALIZE_WS(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_DESERIALIZE_WS(xml);
        default:
            assert(!"cannot happen");
    }
}

void ValueHandle::deserializeWithNanAndInfs(istream& istr, bool withSuffix)
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_DESERIALIZE_IN(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_DESERIALIZE_IN(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_DESERIALIZE_IN(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_DESERIALIZE_IN(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_DESERIALIZE_IN(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_DESERIALIZE_IN(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_DESERIALIZE_IN(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_DESERIALIZE_IN(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_DESERIALIZE_IN(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_DESERIALIZE_IN(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_DESERIALIZE_IN(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_DESERIALIZE_IN(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_DESERIALIZE_IN(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_DESERIALIZE_IN(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_DESERIALIZE_IN(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_DESERIALIZE_IN(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_DESERIALIZE_IN(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_DESERIALIZE_IN(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_DESERIALIZE_IN(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_DESERIALIZE_IN(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_DESERIALIZE_IN(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_DESERIALIZE_IN(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_DESERIALIZE_IN(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_DESERIALIZE_IN(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_DESERIALIZE_IN(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_DESERIALIZE_IN(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_DESERIALIZE_IN(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_DESERIALIZE_IN(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_DESERIALIZE_IN(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_DESERIALIZE_IN(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_DESERIALIZE_IN(Optional);
        default:
            assert(!"cannot happen");
    }
}

void ValueHandle::deserialize(NetworkByteBuffer& istr)
{
    WrapperByteBuffer<NetworkByteBuffer> x(istr);
    deserialize(x);
}

void ValueHandle::deserialize(NativeByteBuffer& istr)
{
    WrapperByteBuffer<NativeByteBuffer> x(istr);
    deserialize(x);
}

size_t ConstValueHandle::getSerializedSize() const
{
    switch (type_) {
        case Meta::Type::BLOB:
            VALUE_HANDLE_GET_SIZE_M(blob);
        case Meta::Type::FLOAT32:
            VALUE_HANDLE_GET_SIZE_N(float32);
        case Meta::Type::FLOAT64:
            VALUE_HANDLE_GET_SIZE_N(float64);
        case Meta::Type::DECIMAL32:
            VALUE_HANDLE_GET_SIZE_N(decimal32);
        case Meta::Type::DECIMAL64:
            VALUE_HANDLE_GET_SIZE_N(decimal64);
        case Meta::Type::DECIMAL128:
            VALUE_HANDLE_GET_SIZE_N(decimal128);
        case Meta::Type::COMPLEX32:
            VALUE_HANDLE_GET_SIZE_N(complex32);
        case Meta::Type::COMPLEX64:
            VALUE_HANDLE_GET_SIZE_N(complex64);
        case Meta::Type::BOOLEAN:
            VALUE_HANDLE_GET_SIZE_N(boolean);
        case Meta::Type::UINT8:
            VALUE_HANDLE_GET_SIZE_N(uint8);
        case Meta::Type::UINT16:
            VALUE_HANDLE_GET_SIZE_N(uint16);
        case Meta::Type::UINT32:
            VALUE_HANDLE_GET_SIZE_N(uint32);
        case Meta::Type::UINT64:
            VALUE_HANDLE_GET_SIZE_N(uint64);
        case Meta::Type::INT8:
            VALUE_HANDLE_GET_SIZE_N(int8);
        case Meta::Type::INT16:
            VALUE_HANDLE_GET_SIZE_N(int16);
        case Meta::Type::INT32:
            VALUE_HANDLE_GET_SIZE_N(int32);
        case Meta::Type::INT64:
            VALUE_HANDLE_GET_SIZE_N(int64);
        case Meta::Type::RSTRING:
            VALUE_HANDLE_GET_SIZE_M(rstring);
        case Meta::Type::USTRING:
            VALUE_HANDLE_GET_SIZE_M(ustring);
        case Meta::Type::TIMESTAMP:
            VALUE_HANDLE_GET_SIZE_M(timestamp);
        case Meta::Type::BSTRING:
            VALUE_HANDLE_GET_SIZE_M(BString);
        case Meta::Type::LIST:
            VALUE_HANDLE_GET_SIZE_M(List);
        case Meta::Type::BLIST:
            VALUE_HANDLE_GET_SIZE_M(BList);
        case Meta::Type::MAP:
            VALUE_HANDLE_GET_SIZE_M(Map);
        case Meta::Type::BMAP:
            VALUE_HANDLE_GET_SIZE_M(BMap);
        case Meta::Type::SET:
            VALUE_HANDLE_GET_SIZE_M(Set);
        case Meta::Type::BSET:
            VALUE_HANDLE_GET_SIZE_M(BSet);
        case Meta::Type::TUPLE:
            VALUE_HANDLE_GET_SIZE_M(Tuple);
        case Meta::Type::ENUM:
            VALUE_HANDLE_GET_SIZE_M(Enum);
        case Meta::Type::XML:
            VALUE_HANDLE_GET_SIZE_M(xml);
        case Meta::Type::OPTIONAL:
            VALUE_HANDLE_GET_SIZE_M(Optional);
        default:
            assert(!"cannot happen");
    }
    return 0;
}

size_t ValueHandle::getSerializedSize() const
{
    ConstValueHandle vh(value_, type_, tpid_);
    return vh.getSerializedSize();
}

ConstValueHandle::ConstValueHandle(ValueHandle const& o)
  : value_(o.value_)
  , type_(o.type_)
  , tpid_(o.tpid_)
{}

size_t ConstValueHandle::hashCode() const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    return vh.hashCode();
}

void ConstValueHandle::serialize(ostream& ostr) const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    vh.serialize(ostr);
}

void ConstValueHandle::serializeWithPrecision(ostream& ostr) const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    vh.serializeWithPrecision(ostr);
}

void ConstValueHandle::serialize(VirtualByteBuffer& buf) const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    vh.serialize(buf);
}

void ConstValueHandle::serialize(NativeByteBuffer& buf) const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    vh.serialize(buf);
}

void ConstValueHandle::serialize(NetworkByteBuffer& buf) const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    vh.serialize(buf);
}

string ConstValueHandle::toString() const
{
    ostringstream ostr;
    ostr.imbue(locale::classic());
    this->serializeWithPrecision(ostr);
    return ostr.str();
}

bool ConstValueHandle::equals(const ConstValueHandle& oh) const
{
    ValueHandle vlhs(const_cast<void*>(value_), type_, tpid_);
    ValueHandle vrhs(const_cast<void*>(oh.value_), oh.type_, oh.tpid_);
    return vlhs.equals(vrhs);
}

ValueHandle ConstValueHandle::newValue() const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    return vh.newValue();
}

void ConstValueHandle::deleteValue() const
{
    ValueHandle vh(const_cast<void*>(value_), type_, tpid_);
    return vh.deleteValue();
}
