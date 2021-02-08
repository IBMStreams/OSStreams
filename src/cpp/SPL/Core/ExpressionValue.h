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

#ifndef EXPRESSION_VALUE_H
#define EXPRESSION_VALUE_H

#include <SPL/FrontEnd/MetaType.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Type/List.h>
#include <SPL/Runtime/Type/Set.h>
#include <SPL/Runtime/Type/Xml.h>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>
#include <iostream>

#define EV_BUILTIN_TYPES                                                                           \
    BOOST_PP_TUPLE_TO_LIST(20, (int8, int16, int32, int64, uint8, uint16, uint32, uint64, float32, \
                                float64, decimal32, decimal64, decimal128, complex32, complex64,   \
                                rstring, ustring, boolean, timestamp, xml))

#define EV_BUILTIN_TYPES_NO_COMPLEX                                                                \
    BOOST_PP_TUPLE_TO_LIST(18, (int8, int16, int32, int64, uint8, uint16, uint32, uint64, float32, \
                                float64, decimal32, decimal64, decimal128, rstring, ustring,       \
                                boolean, timestamp, xml))

#define ASSIGN(R, _, T) void assign(const SPL::T& val);

#define LASSIGN(R, _, T) void assign(const SPL::list<SPL::T>& val);

#define SASSIGN(R, _, T) void assign(const SPL::set<SPL::T>& val);

#define CTOR(R, _, T) ExpressionValue(const SPL::T& val);

#define LCTOR(R, _, T) ExpressionValue(const SPL::list<SPL::T>& val);

#define SCTOR(R, _, T) ExpressionValue(const SPL::set<SPL::T>& val);

#define GET(R, _, T) const SPL::T& BOOST_PP_CAT(get_, T)() const;

#define LGET(R, _, T) const SPL::list<SPL::T>& BOOST_PP_CAT(get_list_, T)() const;

#define SGET(R, _, T) const SPL::set<SPL::T>& BOOST_PP_CAT(get_set_, T)() const;

namespace SPL {
//! Hold Expressions at compile time
class RootTyp;
class ConstValueHandle;
class SourceLocation;

class ExpressionValue
{
  public:
    /// Initialization
    ExpressionValue()
      : _type(MetaType::INVALID)
      , _elementType(MetaType::INVALID)
      , _dummy(NULL)
    {}

    /// Ctor - from MetaType
    /// @param type MetaType
    ExpressionValue(const MetaType& type);

    /// Ctors - from values
    BOOST_PP_LIST_FOR_EACH(CTOR, _, EV_BUILTIN_TYPES)
    BOOST_PP_LIST_FOR_EACH(LCTOR, _, EV_BUILTIN_TYPES)
    BOOST_PP_LIST_FOR_EACH(SCTOR, _, EV_BUILTIN_TYPES)

    /// Copy Constructor
    /// @param rhs Source
    /// @return *this
    ExpressionValue(const ExpressionValue& rhs);

    /// Copy Constructor from ConstValueHandle
    /// @param rhs Source
    /// @return *this
    ExpressionValue(const ConstValueHandle& rhs);

    /// Assignment
    /// @param rhs Source
    /// @return *this
    ExpressionValue& operator=(const ExpressionValue& rhs);

    /// Termination - clean up the correct value
    ~ExpressionValue() { cleanup(); }

    /// Assignment - set the value in the expression
    /// @param val    value to be assigned
    BOOST_PP_LIST_FOR_EACH(ASSIGN, _, EV_BUILTIN_TYPES)
    BOOST_PP_LIST_FOR_EACH(LASSIGN, _, EV_BUILTIN_TYPES)
    BOOST_PP_LIST_FOR_EACH(SASSIGN, _, EV_BUILTIN_TYPES)

    /// Get value
    /// @return value that matches the type
    /// asserts if the type doesn't match the Value type
    BOOST_PP_LIST_FOR_EACH(GET, _, EV_BUILTIN_TYPES)
    BOOST_PP_LIST_FOR_EACH(LGET, _, EV_BUILTIN_TYPES)
    BOOST_PP_LIST_FOR_EACH(SGET, _, EV_BUILTIN_TYPES)

    /// Return Expression type
    /// @return the MetaType for the expression
    const MetaType& type() const { return _type; }

    /// Return Expression element type (
    /// @return the MetaType for the expression
    const MetaType& elementType() const { return _elementType; }

    /// Do these 2 Values have the same type?
    /// @param rhs type to check
    /// @return 'true' if the two values have the same type
    bool typesMatch(const ExpressionValue& rhs) const
    {
        return (_type == rhs._type && _elementType == rhs._elementType);
    }

    /// Test for Equality
    /// @param rhs object to be compared
    /// @return 'true' if *this is identical to rhs
    bool operator==(const ExpressionValue& rhs) const;

    /// Test for Inequality
    /// @param rhs object to be compared
    /// @return 'true' if *this is not identical to rhs
    bool operator!=(const ExpressionValue& rhs) const { return !(*this == rhs); }

    /// Compare for Greater Than
    /// @param rhs object to be compared
    /// @return 'true' if *this is greater than rhs
    bool operator>(const ExpressionValue& rhs) const;

    /// Are these 2 Values both lists of the same type and size?
    /// @param rhs type to check
    /// @return 'true' if the two values have the same type
    bool listsMatch(const ExpressionValue& rhs) const;

    /// Are these 2 Values both sets of the same type and size?
    /// @param rhs type to check
    /// @return 'true' if the two values have the same type
    bool setsMatch(const ExpressionValue& rhs) const;

    /// Extract integer value, if possible
    /// @param val returned value
    /// @return 'true' if the expression represents an integer
    bool getIntegerValue(int64_t& val) const;

    /// Extract unsigned integer value, if possible
    /// @param val returned value
    /// @return 'true' if the expression represents an unsigned integer
    bool getIntegerValue(uint64_t& val) const;

    /// Extract float32 value, if possible
    /// @param val returned value
    /// @return 'true' if the expression can be converted to float
    bool getFloatValue(float& val) const;

    /// Extract double value, if possible
    /// @param val value
    /// @return 'true' if the expression can be converted to double
    bool getFloatValue(double& val) const;

    /// Extract long double value, if possible
    /// @param val returned value
    /// @return 'true' if the expression can be converted to long double
    bool getFloatValue(long double& val) const;

    /// Subscript list, if possible
    /// Replace *this with list[i]
    /// @param index  subscript
    /// @return 'true' if subscript is in range
    bool subscript(uint64_t index);

    /// Subscript list, if possible
    /// Replace *this with list[lower:upper-1]
    /// @param lower  lower bound (if present)
    /// @param upper  upper bound (if present)
    /// @return 'true' if subscript is in range
    bool subscript(uint64_t* lower, uint64_t* upper);

    /// Cast a value.  Only integer casts are supported
    /// Replace *this with (type) *this
    /// @param type  RootTyp
    /// @return 'true' if cast was done
    bool cast(const RootTyp& type);

    /// Convert to a string
    /// @param result string result
    /// @param loc Source Location of conversion
    /// @return 'true' if conversion was done
    bool toString(std::string& result, const SourceLocation& loc) const;

    /// Convert from a string
    /// @param val string input
    /// @param loc Source Location of conversion
    /// @return 'true' if conversion was done
    bool fromString(const std::string& val, const SourceLocation& loc);

    /// Convert to a rstring
    /// @param result rstring result
    /// @param loc Source Location of conversion
    /// @return 'true' if conversion was done
    bool toString(rstring& result, const SourceLocation& loc) const;

    /// Convert from a rstring
    /// @param val rstring input
    /// @param loc Source Location of conversion
    /// @return 'true' if conversion was done
    bool fromString(const rstring& val, const SourceLocation& loc);

  private:
    MetaType _type;        // The current value's type
    MetaType _elementType; // If we are a list, the element type
#define UDECL(R, _, T)                                                                             \
    SPL::T* BOOST_PP_CAT(_, T);                                                                    \
    /**/
#define LDECL(R, _, T)                                                                             \
    SPL::list<T>* BOOST_PP_CAT(_list_, T);                                                         \
    /**/
#define SDECL(R, _, T)                                                                             \
    SPL::set<T>* BOOST_PP_CAT(_set_, T);                                                           \
    /**/
    // long list of supported types
    union
    {
        void* _dummy; // to zero nicely
        BOOST_PP_LIST_FOR_EACH(UDECL, _, EV_BUILTIN_TYPES)
        BOOST_PP_LIST_FOR_EACH(LDECL, _, EV_BUILTIN_TYPES)
        BOOST_PP_LIST_FOR_EACH(SDECL, _, EV_BUILTIN_TYPES)
    };
    //
    /// Clean up - delete current data and reset to INVALID
    void cleanup();
};

std::ostream& operator<<(std::ostream& ostr, const ExpressionValue& v);
};

#undef UDECL
#undef LDECL
#undef SDECL
#undef CTOR
#undef LCTOR
#undef SCTOR
#undef ASSIGN
#undef LASSIGN
#undef SASSIGN
#undef GET
#undef LGET
#undef SGET

#endif /* EXPRESSION_VALUE_H */
