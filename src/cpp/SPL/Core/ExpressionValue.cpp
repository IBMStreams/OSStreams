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

#include <SPL/Core/ExpressionValue.h>

#include <SPL/Core/CompilerError.h>
#include <SPL/Core/META.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Function/BuiltinSPLFunctions.h>
#include <SPL/Utility/SysOut.h>

namespace SPL {

#define PRINT(R, _, T)                                                                             \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        serializeWithPrecision(ostr, v.BOOST_PP_CAT(get_, T)());                                   \
        break;
#define LPRINT(R, _, T)                                                                            \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        serializeWithPrecision(ostr, v.BOOST_PP_CAT(get_list_, T)());                              \
        break;
#define SPRINT(R, _, T)                                                                            \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        serializeWithPrecision(ostr, v.BOOST_PP_CAT(get_set_, T)());                               \
        break;

std::ostream& operator<<(std::ostream& ostr, const ExpressionValue& v)
{
    switch (v.type()) {
        case MetaType::SET:
            switch (v.elementType()) {
                BOOST_PP_LIST_FOR_EACH(SPRINT, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;

        case MetaType::LIST:
            switch (v.elementType()) {
                BOOST_PP_LIST_FOR_EACH(LPRINT, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;

            BOOST_PP_LIST_FOR_EACH(PRINT, _, EV_BUILTIN_TYPES)
        default:
            assert(!"Cannot happen");
    }
    return ostr;
}

#define EQ(R, _, T)                                                                                \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        return (BOOST_PP_CAT(get_, T)() == rhs.BOOST_PP_CAT(get_, T)());
#define LEQ(R, _, T)                                                                               \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        return (BOOST_PP_CAT(get_list_, T)() == rhs.BOOST_PP_CAT(get_list_, T)());
#define SEQ(R, _, T)                                                                               \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        return (BOOST_PP_CAT(get_set_, T)() == rhs.BOOST_PP_CAT(get_set_, T)());

bool ExpressionValue::operator==(const ExpressionValue& rhs) const
{
    if (!typesMatch(rhs))
        return false;

    switch (_type) {
        case MetaType::SET:
            switch (_elementType) {
                BOOST_PP_LIST_FOR_EACH(SEQ, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;

        case MetaType::LIST:
            switch (_elementType) {
                BOOST_PP_LIST_FOR_EACH(LEQ, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;

            BOOST_PP_LIST_FOR_EACH(EQ, _, EV_BUILTIN_TYPES)
        default:
            assert(!"Cannot happen");
    }
    return false;
}

#define GT(R, _, T)                                                                                \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        return (BOOST_PP_CAT(get_, T)() > rhs.BOOST_PP_CAT(get_, T)());
#define LGT(R, _, T)                                                                               \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        num = BOOST_PP_CAT(_list_, T)->size();                                                     \
        if (num != rhs.BOOST_PP_CAT(_list_, T)->size())                                            \
            return num > rhs.BOOST_PP_CAT(_list_, T)->size();                                      \
        for (uint32_t i = 0; i < num; i++)                                                         \
            if ((*BOOST_PP_CAT(_list_, T))[i] > (*rhs.BOOST_PP_CAT(_list_, T))[i])                 \
                return true;                                                                       \
        return false;

bool ExpressionValue::operator>(const ExpressionValue& rhs) const
{
    if (!typesMatch(rhs))
        return false;

    uint32_t num;
    switch (_type) {
        case MetaType::LIST:
            switch (_elementType) {
                BOOST_PP_LIST_FOR_EACH(LGT, _, ORDERED_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;

            BOOST_PP_LIST_FOR_EACH(GT, _, ORDERED_TYPES)
        default:
            assert(!"Cannot happen");
    }
    return false;
}

ExpressionValue::ExpressionValue(const ExpressionValue& rhs)
  : _type(MetaType::INVALID)
  , _elementType(MetaType::INVALID)
{
    this->operator=(rhs);
}

#define FROMCVH(R, _, T)                                                                           \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        BOOST_PP_CAT(_, T) = new SPL::T((const SPL::T&)rhs);                                       \
        break;

#define LFROMCVH(R, _, T)                                                                          \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        BOOST_PP_CAT(_list_, T) = new SPL::list<SPL::T>((const SPL::list<SPL::T>&)rhs);            \
        break;

#define SFROMCVH(R, _, T)                                                                          \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        BOOST_PP_CAT(_set_, T) = new SPL::set<SPL::T>((const SPL::set<SPL::T>&)rhs);               \
        break;

ExpressionValue::ExpressionValue(const ConstValueHandle& rhs)
  : _type(static_cast<MetaType::value>(rhs.getMetaType().getValue()))
  , _elementType(MetaType::INVALID)
{

    switch (_type) {
        case MetaType::SET: {
            Set const& s = rhs;
            _elementType = static_cast<MetaType::value>(s.getElementMetaType().getValue());
            switch (_elementType) {
                BOOST_PP_LIST_FOR_EACH(SFROMCVH, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
        } break;

        case MetaType::LIST: {
            List const& l = rhs;
            _elementType = static_cast<MetaType::value>(l.getElementMetaType().getValue());
            switch (_elementType) {
                BOOST_PP_LIST_FOR_EACH(LFROMCVH, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
        } break;

            BOOST_PP_LIST_FOR_EACH(FROMCVH, _, EV_BUILTIN_TYPES)
        case MetaType::INVALID:
            cleanup();
            break;
        default:
            assert(!"Cannot happen");
    }
}

// CTOR - from MetaType
ExpressionValue::ExpressionValue(const MetaType& typeIn)
{
    _type = MetaType::INVALID;
#define CTOR(R, _, T)                                                                              \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        assign(SPL::T());                                                                          \
        break;

    switch (typeIn) {
        BOOST_PP_LIST_FOR_EACH(CTOR, _, EV_BUILTIN_TYPES)
        default:;
    }
#undef CTOR
}

// Ctor functions - from values
#define CTOR(R, _, T)                                                                              \
    ExpressionValue::ExpressionValue(const SPL::T& val)                                            \
      : _elementType(MetaType::INVALID)                                                            \
    {                                                                                              \
        BOOST_PP_CAT(_, T) = new SPL::T(val);                                                      \
        _type = MetaType::BOOST_PP_CAT(META_, T);                                                  \
    }

#define LCTOR(R, _, T)                                                                             \
    ExpressionValue::ExpressionValue(const SPL::list<SPL::T>& val)                                 \
      : _type(MetaType::LIST)                                                                      \
    {                                                                                              \
        BOOST_PP_CAT(_list_, T) = new SPL::list<SPL::T>(val);                                      \
        _elementType = MetaType::BOOST_PP_CAT(META_, T);                                           \
    }

#define SCTOR(R, _, T)                                                                             \
    ExpressionValue::ExpressionValue(const SPL::set<SPL::T>& val)                                  \
      : _type(MetaType::SET)                                                                       \
    {                                                                                              \
        BOOST_PP_CAT(_set_, T) = new SPL::set<SPL::T>(val);                                        \
        _elementType = MetaType::BOOST_PP_CAT(META_, T);                                           \
    }

BOOST_PP_LIST_FOR_EACH(CTOR, _, EV_BUILTIN_TYPES)
BOOST_PP_LIST_FOR_EACH(LCTOR, _, EV_BUILTIN_TYPES)
BOOST_PP_LIST_FOR_EACH(SCTOR, _, EV_BUILTIN_TYPES)

ExpressionValue& ExpressionValue::operator=(const ExpressionValue& rhs)
{
#define TOREQ(R, _, T)                                                                             \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        if (_type == rhs._type)                                                                    \
            *BOOST_PP_CAT(_, T) = *rhs.BOOST_PP_CAT(_, T);                                         \
        else {                                                                                     \
            cleanup();                                                                             \
            BOOST_PP_CAT(_, T) = new SPL::T(*rhs.BOOST_PP_CAT(_, T));                              \
        }                                                                                          \
        break;

#define LTOREQ(R, _, T)                                                                            \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        if (_type == rhs._type && _elementType == rhs._elementType)                                \
            *BOOST_PP_CAT(_list_, T) = *rhs.BOOST_PP_CAT(_list_, T);                               \
        else {                                                                                     \
            cleanup();                                                                             \
            BOOST_PP_CAT(_list_, T) = new SPL::list<SPL::T>(*rhs.BOOST_PP_CAT(_list_, T));         \
        }                                                                                          \
        break;

#define STOREQ(R, _, T)                                                                            \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        if (_type == rhs._type && _elementType == rhs._elementType)                                \
            *BOOST_PP_CAT(_set_, T) = *rhs.BOOST_PP_CAT(_set_, T);                                 \
        else {                                                                                     \
            cleanup();                                                                             \
            BOOST_PP_CAT(_set_, T) = new SPL::set<SPL::T>(*rhs.BOOST_PP_CAT(_set_, T));            \
        }                                                                                          \
        break;

    // Shortcut...
    if (this == &rhs)
        return *this;

    switch (rhs._type) {
        case MetaType::SET:
            switch (rhs._elementType) {
                BOOST_PP_LIST_FOR_EACH(STOREQ, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;
        case MetaType::LIST:
            switch (rhs._elementType) {
                BOOST_PP_LIST_FOR_EACH(LTOREQ, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;
            BOOST_PP_LIST_FOR_EACH(TOREQ, _, EV_BUILTIN_TYPES)
        case MetaType::INVALID:
            cleanup();
            break;
        default:
            assert(!"Cannot happen");
    };

    _type = rhs._type;
    _elementType = rhs._elementType;

    return *this;
}

void ExpressionValue::cleanup()
{
    if (_type == MetaType::INVALID)
        return;

        // Clean up the appropriate value
#define CLEANUP(R, _, T)                                                                           \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        delete BOOST_PP_CAT(_, T);                                                                 \
        break;

#define LCLEANUP(R, _, T)                                                                          \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        delete BOOST_PP_CAT(_list_, T);                                                            \
        break;

#define SCLEANUP(R, _, T)                                                                          \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        delete BOOST_PP_CAT(_set_, T);                                                             \
        break;

    switch (_type) {
        BOOST_PP_LIST_FOR_EACH(CLEANUP, _, EV_BUILTIN_TYPES)
        case MetaType::SET:
            switch (_elementType) {
                BOOST_PP_LIST_FOR_EACH(SCLEANUP, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;
        case MetaType::LIST:
            switch (_elementType) {
                BOOST_PP_LIST_FOR_EACH(LCLEANUP, _, EV_BUILTIN_TYPES)
                default:
                    assert(!"Cannot happen");
            }
            break;
        default:
            assert(!"Cannot happen");
    }
    _type = MetaType::INVALID;
    _elementType = MetaType::INVALID;
}

bool ExpressionValue::listsMatch(const ExpressionValue& rhs) const
{
    if (_type != MetaType::LIST || rhs._type != MetaType::LIST || _elementType != rhs._elementType)
        return false;

    // We have a list.   Fake them out as if they are the same
    List& l = static_cast<List&>(*_list_int8);
    List& r = static_cast<List&>(*rhs._list_int8);
    return l.getSize() == r.getSize();
}

bool ExpressionValue::setsMatch(const ExpressionValue& rhs) const
{
    if (_type != MetaType::SET || rhs._type != MetaType::SET || _elementType != rhs._elementType)
        return false;

    // We have a list.   Fake them out as if they are the same
    Set& l = static_cast<Set&>(*_set_int8);
    Set& r = static_cast<Set&>(*rhs._set_int8);
    return l.getSize() == r.getSize();
}

// Assignment functions
#define ASSIGN(R, _, T)                                                                            \
    void ExpressionValue::assign(const SPL::T& val)                                                \
    {                                                                                              \
        if (_type == MetaType::BOOST_PP_CAT(META_, T))                                             \
            *BOOST_PP_CAT(_, T) = val;                                                             \
        else {                                                                                     \
            cleanup();                                                                             \
            BOOST_PP_CAT(_, T) = new SPL::T(val);                                                  \
        }                                                                                          \
        _type = MetaType::BOOST_PP_CAT(META_, T);                                                  \
        _elementType = MetaType::INVALID;                                                          \
    }

#define LASSIGN(R, _, T)                                                                           \
    void ExpressionValue::assign(const SPL::list<SPL::T>& val)                                     \
    {                                                                                              \
        if (_type == MetaType::LIST && _elementType == MetaType::BOOST_PP_CAT(META_, T))           \
            *BOOST_PP_CAT(_list_, T) = val;                                                        \
        else {                                                                                     \
            cleanup();                                                                             \
            BOOST_PP_CAT(_list_, T) = new SPL::list<SPL::T>(val);                                  \
        }                                                                                          \
        _type = MetaType::LIST;                                                                    \
        _elementType = MetaType::BOOST_PP_CAT(META_, T);                                           \
    }

#define SASSIGN(R, _, T)                                                                           \
    void ExpressionValue::assign(const SPL::set<SPL::T>& val)                                      \
    {                                                                                              \
        if (_type == MetaType::SET && _elementType == MetaType::BOOST_PP_CAT(META_, T))            \
            *BOOST_PP_CAT(_set_, T) = val;                                                         \
        else {                                                                                     \
            cleanup();                                                                             \
            BOOST_PP_CAT(_set_, T) = new SPL::set<SPL::T>(val);                                    \
        }                                                                                          \
        _type = MetaType::SET;                                                                     \
        _elementType = MetaType::BOOST_PP_CAT(META_, T);                                           \
    }

BOOST_PP_LIST_FOR_EACH(ASSIGN, _, EV_BUILTIN_TYPES)
BOOST_PP_LIST_FOR_EACH(LASSIGN, _, EV_BUILTIN_TYPES)
BOOST_PP_LIST_FOR_EACH(SASSIGN, _, EV_BUILTIN_TYPES)

// Getter functions
#define GET(R, _, T)                                                                               \
    const SPL::T& ExpressionValue::BOOST_PP_CAT(get_, T)() const                                   \
    {                                                                                              \
        assert(_type == MetaType::BOOST_PP_CAT(META_, T));                                         \
        return *BOOST_PP_CAT(_, T);                                                                \
    }

#define LGET(R, _, T)                                                                              \
    const SPL::list<SPL::T>& ExpressionValue::BOOST_PP_CAT(get_list_, T)() const                   \
    {                                                                                              \
        assert(_type == MetaType::LIST && _elementType == MetaType::BOOST_PP_CAT(META_, T));       \
        return *BOOST_PP_CAT(_list_, T);                                                           \
    }

#define SGET(R, _, T)                                                                              \
    const SPL::set<SPL::T>& ExpressionValue::BOOST_PP_CAT(get_set_, T)() const                     \
    {                                                                                              \
        assert(_type == MetaType::SET && _elementType == MetaType::BOOST_PP_CAT(META_, T));        \
        return *BOOST_PP_CAT(_set_, T);                                                            \
    }

BOOST_PP_LIST_FOR_EACH(GET, _, EV_BUILTIN_TYPES)
BOOST_PP_LIST_FOR_EACH(LGET, _, EV_BUILTIN_TYPES)
BOOST_PP_LIST_FOR_EACH(SGET, _, EV_BUILTIN_TYPES)

bool ExpressionValue::fromString(const std::string& str, const SourceLocation& loc)
{
#define FROMSTRING(R, VAR, T)                                                                      \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        assign(::SPL::spl_cast<SPL::T, SPL::rstring>::cast(str));                                  \
        break;

    try {
        switch (_type) {
            BOOST_PP_LIST_FOR_EACH(FROMSTRING, _, EV_BUILTIN_TYPES)
            default:
                return false;
        }
    } catch (const SPLRuntimeException& e) {
        SysOut::warnln(SPL_CORE_COMPILE_TIME_CAST_FAILURE(e.getExplanation()), loc);
        return false;
    } catch (...) {
        return false;
    }

    // It worked!
    return true;
}

bool ExpressionValue::toString(std::string& result, const SourceLocation& loc) const
{
#define TOSTRING(R, VAR, T)                                                                        \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        result = ::SPL::spl_cast<SPL::rstring, SPL::T>::cast(BOOST_PP_CAT(get_, T)());             \
        break;

    try {
        switch (_type) {
            BOOST_PP_LIST_FOR_EACH(TOSTRING, _, EV_BUILTIN_TYPES)
            default:
                return false;
        }
    } catch (const SPLRuntimeException& e) {
        SysOut::warnln(SPL_CORE_COMPILE_TIME_CAST_FAILURE(e.getExplanation()), loc);
        return false;
    } catch (...) {
        return false;
    }

    // It worked!
    return true;
}

bool ExpressionValue::fromString(const SPL::rstring& str, const SourceLocation& loc)
{
#define FROMSTRING(R, VAR, T)                                                                      \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        assign(::SPL::spl_cast<SPL::T, SPL::rstring>::cast(str));                                  \
        break;

    try {
        switch (_type) {
            BOOST_PP_LIST_FOR_EACH(FROMSTRING, _, EV_BUILTIN_TYPES)
            default:
                return false;
        }
    } catch (const SPLRuntimeException& e) {
        SysOut::warnln(SPL_CORE_COMPILE_TIME_CAST_FAILURE(e.getExplanation()), loc);
        return false;
    } catch (...) {
        return false;
    }

    // It worked!
    return true;
}

bool ExpressionValue::toString(SPL::rstring& result, const SourceLocation& loc) const
{
#define TOSTRING(R, VAR, T)                                                                        \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        result = ::SPL::spl_cast<SPL::rstring, SPL::T>::cast(BOOST_PP_CAT(get_, T)());             \
        break;

    try {
        switch (_type) {
            BOOST_PP_LIST_FOR_EACH(TOSTRING, _, EV_BUILTIN_TYPES)
            default:
                return false;
        }
    } catch (const SPLRuntimeException& e) {
        SysOut::warnln(SPL_CORE_COMPILE_TIME_CAST_FAILURE(e.getExplanation()), loc);
        return false;
    } catch (...) {
        return false;
    }

    // It worked!
    return true;
}

bool ExpressionValue::cast(const RootTyp& typeIn)
{
#define INT_CAST(R, VAR, T)                                                                        \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        if (!getIntegerValue(VAR))                                                                 \
            return false;                                                                          \
        assign(static_cast<SPL::T>(VAR));                                                          \
        break;

#define FLOAT_CAST(R, VAR, T)                                                                      \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        if (!getFloatValue(VAR))                                                                   \
            return false;                                                                          \
        assign(static_cast<SPL::T>(VAR));                                                          \
        break;

#define INT_TYPES BOOST_PP_TUPLE_TO_LIST(4, (int8, int16, int32, int64))
#define UINT_TYPES BOOST_PP_TUPLE_TO_LIST(4, (uint8, uint16, uint32, uint64))

    uint64_t u;
    int64_t i;
    long double ld;
    switch (typeIn.getMetaType()) {
        BOOST_PP_LIST_FOR_EACH(INT_CAST, i, INT_TYPES)
        BOOST_PP_LIST_FOR_EACH(INT_CAST, u, UINT_TYPES)
        BOOST_PP_LIST_FOR_EACH(FLOAT_CAST, ld, FLOAT_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionValue::getIntegerValue(int64_t& val) const
{
#define INTVAL(R, _, T)                                                                            \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        val = static_cast<int64_t>(*BOOST_PP_CAT(_, T));                                           \
        break;

    switch (_type) {
        BOOST_PP_LIST_FOR_EACH(INTVAL, _, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(INTVAL, _, FLOAT_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionValue::getIntegerValue(uint64_t& val) const
{
#define UINTVAL(R, _, T)                                                                           \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        val = static_cast<int64_t>(*BOOST_PP_CAT(_, T));                                           \
        break;

    switch (_type) {
        BOOST_PP_LIST_FOR_EACH(UINTVAL, _, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(UINTVAL, _, FLOAT_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionValue::getFloatValue(float& val) const
{
#define FVAL(R, _, T)                                                                              \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        val = static_cast<float>(*BOOST_PP_CAT(_, T));                                             \
        break;

    switch (_type) {
        BOOST_PP_LIST_FOR_EACH(FVAL, _, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(FVAL, _, FLOAT_TYPES)
        default:
            return false;
    }
    return true;
#undef FVAL
}

bool ExpressionValue::getFloatValue(double& val) const
{
#define FVAL(R, _, T)                                                                              \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        val = static_cast<double>(*BOOST_PP_CAT(_, T));                                            \
        break;

    switch (_type) {
        BOOST_PP_LIST_FOR_EACH(FVAL, _, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(FVAL, _, FLOAT_TYPES)
        default:
            return false;
    }
    return true;
#undef FVAL
}

bool ExpressionValue::getFloatValue(long double& val) const
{
#define FVAL(R, _, T)                                                                              \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        val = static_cast<long double>(*BOOST_PP_CAT(_, T));                                       \
        break;

    switch (_type) {
        BOOST_PP_LIST_FOR_EACH(FVAL, _, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(FVAL, _, FLOAT_TYPES)
        default:
            return false;
    }
    return true;
#undef FVAL
}

bool ExpressionValue::subscript(uint64_t index)
{
    if (_type != MetaType::LIST)
        return false;

#define SUBSCRIPT(R, _, T)                                                                         \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        if (index >= BOOST_PP_CAT(_list_, T)->size())                                              \
            return false;                                                                          \
        SPL::T val = (*BOOST_PP_CAT(_list_, T))[index];                                            \
        assign(val);                                                                               \
    } break;

    switch (_elementType) {
        BOOST_PP_LIST_FOR_EACH(SUBSCRIPT, _, EV_BUILTIN_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionValue::subscript(uint64_t* lower, uint64_t* upper)
{
    if (_type != MetaType::LIST)
        return false;

    int32_t l, u;
    l = lower ? *lower : 0;

#undef SUBSCRIPT
#define SUBSCRIPT(R, _, T)                                                                         \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        if (upper) {                                                                               \
            u = *upper;                                                                            \
            if (u > BOOST_PP_CAT(_list_, T)->size())                                               \
                return false;                                                                      \
        } else                                                                                     \
            u = BOOST_PP_CAT(_list_, T)->size();                                                   \
        SPL::list<SPL::T> result;                                                                  \
        SPL::list<SPL::T>& input = (*BOOST_PP_CAT(_list_, T));                                     \
        for (uint32_t i = l; i < u; i++)                                                           \
            result.push_back(input[i]);                                                            \
        assign(result);                                                                            \
    } break;

    switch (_elementType) {
        BOOST_PP_LIST_FOR_EACH(SUBSCRIPT, _, EV_BUILTIN_TYPES)
        default:
            return false;
    }
    return true;
}

}
