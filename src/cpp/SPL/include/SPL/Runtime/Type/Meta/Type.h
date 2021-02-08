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

#ifndef SPL_RUNTIME_TYPE_META_TYPE_H
#define SPL_RUNTIME_TYPE_META_TYPE_H

/*!
 * \file Type.h \brief Definition of the SPL::Meta::Type class.
 */

// non-templated primitive types
#include <SPL/Runtime/Type/Blob.h>
#include <SPL/Runtime/Type/Complex.h>
#include <SPL/Runtime/Type/Decimal.h>
#include <SPL/Runtime/Type/Integer.h>
#include <SPL/Runtime/Type/Float.h>
#include <SPL/Runtime/Type/String.h>
#include <SPL/Runtime/Type/Timestamp.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <tr1/type_traits>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC

namespace SPL
{

  class BString;
  class List;
  class BList;
  class Map;
  class BMap;
  class Set;
  class BSet;
  class Tuple;
  class Enum;
  class xml;
  class Optional;

  namespace Meta
  {
    /// Class representing top level spl meta types
    class Type
    {
    public:
        enum Value
        {
            INVALID = -1, //!< non-SPL type
            BOOLEAN,      //!< boolean in SPL
            ENUM,         //!< enum{...} in SPL
            INT8,         //!< int8 in SPL
            INT16,        //!< int16 in SPL
            INT32,        //!< int32 in SPL
            INT64,        //!< int64 in SPL
            UINT8,        //!< uint8 in SPL
            UINT16,       //!< uint16 in SPL
            UINT32,       //!< uint32 in SPL
            UINT64,       //!< uint64 in SPL
            FLOAT32,      //!< float32 in SPL
            FLOAT64,      //!< float64 in SPL
            DECIMAL32,    //!< decimal32 in SPL
            DECIMAL64,    //!< decimal64 in SPL
            DECIMAL128,   //!< decimal128 in SPL
            COMPLEX32,    //!< complex32 in SPL
            COMPLEX64,    //!< complex64 in SPL
            TIMESTAMP,    //!< timestamp in SPL
            RSTRING,      //!< rstring in SPL
            BSTRING,      //!< rstring[N] in SPL
            USTRING,      //!< ustring in SPL
            BLOB,         //!< blob in SPL
            LIST,         //!< list<T> in SPL
            BLIST,        //!< list<T>[N] in SPL
            SET,          //!< set<T> in SPL
            BSET,         //!< set<T>[N] in SPL
            MAP,          //!< map<K,V> in SPL
            BMAP,         //!< map<K,V>[N] in SPL
            TUPLE,        //!< tuple<...> in SPL
            XML,          //!< xml or xml<...> in SPL
            OPTIONAL      //!< optional<T> in SPL, where T is any other SPL type
        };

        /// Constructor
        ///
        Type () {}

        /// Constructor
        /// @param v enumeration value
        Type (Value v)
            : value_(v) {}

        /// Get the enumeration value
        /// @return enumeration value
        Type::Value getValue() const
            { return value_; }

        /// Get the string representation
        /// @return string representation
        std::string toString() const;

        /// '=' operator overload for assigning from an enumeration value
        /// @param v enumeration value to assign from
        /// @return a reference to this ibject
        Type & operator=(Value v)
            { value_=v; return *this; }

        /// Cast operator overload for casting to an enumeration value
        /// @return enumeration value
        operator Type::Value () const
            { return value_; }

        /// Get the meta type for a given SPL type (template argument)
        /// @return meta type for a given SPL type
        template <class T>
        static Type typeOf();
    private:
        Value value_;
    };

    inline std::ostream & operator<<(std::ostream & ostr, const Type & type)
        { ostr << type.toString(); return ostr;  }

    inline std::ostream & operator<<(std::ostream & ostr, const Type::Value & type)
        { ostr << Type(type); return ostr;  }


#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class T>
    struct Type_typeOf {
        static const Type::Value type = Type::INVALID;
        static const size_t size;
    };

    template <class X, class Y>
    struct is_base_of {
        const static bool value = std::tr1::is_base_of<X,Y>::value;
    };

    template <class T>
    Type Type::typeOf() {
        if (SPL::Meta::is_base_of<BString, T>::value) {
            return Type::BSTRING;
        } else if (SPL::Meta::is_base_of<BList, T>::value) {
            return Type::BLIST;
        } else if (SPL::Meta::is_base_of<List, T>::value) {
            return Type::LIST;
        } else if (SPL::Meta::is_base_of<BMap, T>::value) {
            return Type::BMAP;
        } else if (SPL::Meta::is_base_of<Map, T>::value) {
            return Type::MAP;
        } else if (SPL::Meta::is_base_of<BSet, T>::value) {
            return Type::BSET;
        } else if (SPL::Meta::is_base_of<Set, T>::value) {
            return Type::SET;
        } else if (SPL::Meta::is_base_of<Tuple, T>::value) {
            return Type::TUPLE;
        } else if (SPL::Meta::is_base_of<Enum, T>::value) {
            return Type::ENUM;
        } else if (SPL::Meta::is_base_of<xml, T>::value) {
            return Type::XML;
        } else if (SPL::Meta::is_base_of<Optional, T>::value) {
            return Type::OPTIONAL;
        } else {
            return Type_typeOf<T>::type;
        }
    }

#define VALUE_HANDLE_TYPE_OF(t_real,t_enum)             \
    template <>                                         \
    struct Type_typeOf<t_real> {                        \
            static const Type::Value type=Type::t_enum; \
            static const size_t size=sizeof(t_real);    \
    };

    VALUE_HANDLE_TYPE_OF(blob,BLOB)
    VALUE_HANDLE_TYPE_OF(float32,FLOAT32)
    VALUE_HANDLE_TYPE_OF(float64,FLOAT64)
    VALUE_HANDLE_TYPE_OF(decimal32,DECIMAL32)
    VALUE_HANDLE_TYPE_OF(decimal64,DECIMAL64)
    VALUE_HANDLE_TYPE_OF(decimal128,DECIMAL128)
    VALUE_HANDLE_TYPE_OF(complex32,COMPLEX32)
    VALUE_HANDLE_TYPE_OF(complex64,COMPLEX64)
    VALUE_HANDLE_TYPE_OF(boolean,BOOLEAN)
    VALUE_HANDLE_TYPE_OF(uint8,UINT8)
    VALUE_HANDLE_TYPE_OF(uint16,UINT16)
    VALUE_HANDLE_TYPE_OF(uint32,UINT32)
    VALUE_HANDLE_TYPE_OF(uint64,UINT64)
    VALUE_HANDLE_TYPE_OF(int8,INT8)
    VALUE_HANDLE_TYPE_OF(int16,INT16)
    VALUE_HANDLE_TYPE_OF(int32,INT32)
    VALUE_HANDLE_TYPE_OF(int64,INT64)
    VALUE_HANDLE_TYPE_OF(rstring,RSTRING)
    VALUE_HANDLE_TYPE_OF(ustring,USTRING)
    VALUE_HANDLE_TYPE_OF(timestamp,TIMESTAMP)
#endif /*DOXYGEN_SKIP_FOR_USERS */
  };
};

POP_DLL_PUBLIC

#endif /* SPL_RUNTIME_TYPE_META_TYPE_H */
