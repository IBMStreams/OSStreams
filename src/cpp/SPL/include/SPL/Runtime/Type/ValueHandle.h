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

#ifndef SPL_RUNTIME_TYPE_VALUE_HANDLE_H
#define SPL_RUNTIME_TYPE_VALUE_HANDLE_H

/*!
 * \file ValueHandle.h \brief Definitions of the SPL::ValueHandle and
 * SPL::ConstValueHandle classes.
 */

#include <SPL/Runtime/Type/Meta/Type.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <iostream>
#include <typeinfo>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

#define DECL_VALUE_HANDLE_READ(type)  \
    operator type const &() const;    \

#define DECL_VALUE_HANDLE_WRITE(type) \
    operator type &();                \

PUSH_DLL_PUBLIC
namespace SPL
{
    class BString;
    class List;
    class BList;
    class Set;
    class BSet;
    class Map;
    class BMap;
    class Tuple;
    class Enum;
    class Optional;
    class ValueHandle;
    class VirtualByteBuffer;
    class NativeByteBuffer;
    class NetworkByteBuffer;

    /// \brief Class for reflective manipulation of const values of spl types.
    ///
    /// This class provides a handle to a value which is of %SPL type. See
    /// ValueHandle for additional details.
    class ConstValueHandle
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        ConstValueHandle(void const * value, Meta::Type type, const std::type_info * tpid)
            : value_(value), type_(type), tpid_(tpid) {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Construct a constant value handle from a value of %SPL type
        /// @param value value of %SPL type to be used to create a handle
        /// @throws SPLRuntimeTypeMismatchException if the value is not of %SPL type
        template <class T>
        ConstValueHandle(T const & value);

        /// Copy constructor
        /// @param o other value handle
        explicit ConstValueHandle(ValueHandle const & o);

        /// Copy constructor
        /// @param o other value handle
        ConstValueHandle(ConstValueHandle & o)
            : value_(o.value_), type_(o.type_), tpid_(o.tpid_) {}

        /// Copy constructor
        /// @param o other value handle
        ConstValueHandle(ConstValueHandle const & o)
            : value_(o.value_), type_(o.type_), tpid_(o.tpid_) {}

        /// Default constructor (creates a constant value handle with invalid type)
        ///
        ConstValueHandle()
            : value_(NULL), type_(Meta::Type::INVALID), tpid_(NULL) {}

        /// Convert the value to a constant reference of type blob
        /// @return constant reference of type blob
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type blob
        DECL_VALUE_HANDLE_READ(blob)

        /// Convert the value to a constant reference of type float32
        /// @return constant reference of type float32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type float32
        DECL_VALUE_HANDLE_READ(float32)

        /// Convert the value to a constant reference of type float64
        /// @return constant reference of type float64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type float64
        DECL_VALUE_HANDLE_READ(float64)

        /// Convert the value to a constant reference of type decimal32
        /// @return constant reference of type decimal32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal32
        DECL_VALUE_HANDLE_READ(decimal32)

        /// Convert the value to a constant reference of type decimal64
        /// @return constant reference of type decimal64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal64
        DECL_VALUE_HANDLE_READ(decimal64)

        /// Convert the value to a constant reference of type decimal128
        /// @return constant reference of type decimal128
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal128
        DECL_VALUE_HANDLE_READ(decimal128)

        /// Convert the value to a constant reference of type complex32
        /// @return constant reference of type complex32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type complex32
        DECL_VALUE_HANDLE_READ(complex32)

        /// Convert the value to a constant reference of type complex64
        /// @return constant reference of type complex64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type complex64
        DECL_VALUE_HANDLE_READ(complex64)

        /// Convert the value to a constant reference of type boolean
        /// @return constant reference of type boolean
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type boolean
        DECL_VALUE_HANDLE_READ(boolean)

        /// Convert the value to a constant reference of type uint8
        /// @return constant reference of type uint8
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint8
        DECL_VALUE_HANDLE_READ(uint8)

        /// Convert the value to a constant reference of type uint16
        /// @return constant reference of type uint16
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint16
        DECL_VALUE_HANDLE_READ(uint16)

        /// Convert the value to a constant reference of type uint32
        /// @return constant reference of type uint32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint32
        DECL_VALUE_HANDLE_READ(uint32)

        /// Convert the value to a constant reference of type uint64
        /// @return constant reference of type uint64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint64
        DECL_VALUE_HANDLE_READ(uint64)

        /// Convert the value to a constant reference of type int8
        /// @return constant reference of type int8
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int8
        DECL_VALUE_HANDLE_READ(int8)

        /// Convert the value to a constant reference of type int16
        /// @return constant reference of type int16
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int16
        DECL_VALUE_HANDLE_READ(int16)

        /// Convert the value to a constant reference of type int32
        /// @return constant reference of type int32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int32
        DECL_VALUE_HANDLE_READ(int32)

        /// Convert the value to a constant reference of type int64
        /// @return constant reference of type int64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int64
        DECL_VALUE_HANDLE_READ(int64)

        /// Convert the value to a constant reference of type rstring
        /// @return constant reference of type rstring
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type rstring
        DECL_VALUE_HANDLE_READ(rstring)

        /// Convert the value to a constant reference of type ustring
        /// @return constant reference of type ustring
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type ustring
        DECL_VALUE_HANDLE_READ(ustring)

        /// Convert the value to a constant reference of type timestamp
        /// @return constant reference of type timestamp
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type timestamp
        DECL_VALUE_HANDLE_READ(timestamp)

        /// Convert the value to a constant reference of type BString
        /// @return constant reference of type BString
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BString
        DECL_VALUE_HANDLE_READ(BString)

        /// Convert the value to a constant reference of type List
        /// @return constant reference of type List
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type List
        DECL_VALUE_HANDLE_READ(List)

        /// Convert the value to a constant reference of type BList
        /// @return constant reference of type BList
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BList
        DECL_VALUE_HANDLE_READ(BList)

        /// Convert the value to a constant reference of type Set
        /// @return constant reference of type Set
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Set
        DECL_VALUE_HANDLE_READ(Set)

        /// Convert the value to a constant reference of type BSet
        /// @return constant reference of type BSet
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BSet
        DECL_VALUE_HANDLE_READ(BSet)

        /// Convert the value to a constant reference of type Map
        /// @return constant reference of type Map
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Map
        DECL_VALUE_HANDLE_READ(Map)

        /// Convert the value to a constant reference of type BMap
        /// @return constant reference of type BMap
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BMap
        DECL_VALUE_HANDLE_READ(BMap)

        /// Convert the value to a constant reference of type Tuple
        /// @return constant reference of type Tuple
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Tuple
        DECL_VALUE_HANDLE_READ(Tuple)

        /// Convert the value to a constant reference of type Enum
        /// @return constant reference of type Enum
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Enum
        DECL_VALUE_HANDLE_READ(Enum)

        /// Convert the value to a constant reference of type xml
        /// @return constant reference of type xml
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type xml
        DECL_VALUE_HANDLE_READ(xml)

        /// Convert the value to a constant reference of type Optional
        /// @return constant reference of type Optional
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Optional
        DECL_VALUE_HANDLE_READ(Optional)

        /// Create a new value of this type (dynamically allocated, see deleteValue)
        /// @return a new value of this type
        ValueHandle newValue() const;

        /// Delete the value contained
        /// Note: should be used only if the value is dynamically allocated
        void deleteValue() const;

        /// Get the value pointer
        /// @return value pointer
        void const * getValuePointer() const
        {
            return value_;
        }

        /// Get the hash code
        /// @return hash code
        size_t hashCode() const;

        /// Get the serialized size of the value
        /// @return serialized size of the value
        size_t getSerializedSize() const;

        /// Get the meta type of the value
        /// @return meta type of the value
        Meta::Type getMetaType() const
        {
            return type_;
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        const std::type_info & getTypeId() const
        {
            return *tpid_;
        }
        void serialize(VirtualByteBuffer & buf) const;
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Get the string representation of the value
        /// @return string representation of the value
        std::string toString() const;

        /// Check if the value is equal to another one.
        /// Equals does a deep comparison of all values (i.e for lists, sets, maps)
        /// @param o value handle to compare
        /// @return true if equal, false otherwise
        bool equals(const ConstValueHandle & o) const;

        /// Serialize to a character stream
        /// @param ostr stream to serialize into
        void serialize(std::ostream & ostr) const;

        /// Serialize to a character stream, with required precision for
        /// floating points
        /// @param ostr stream to serialize into
        void serializeWithPrecision(std::ostream & ostr) const;

        /// Serialize into a native byte buffer
        /// @param buf buffer to serialize into
        void serialize(NativeByteBuffer & buf) const;

        /// Serialize into a network byte buffer
        /// @param buf buffer to serialize into
        void serialize(NetworkByteBuffer & buf) const;

    private:
        void const * value_;
        Meta::Type::Value type_;
        const std::type_info * tpid_;
        friend class ValueHandle;
    };

    /*! \brief Class for reflective manipulation of values of %SPL types.

    This class provides a handle to a value which is of %SPL
    type. Operations on a value through a ValueHandle are only valid during
    the lifetime of the value. ValueHandle does not perform any implicit
    memory management, and is merely a reference to a value. An example
    illustrating its use is given as follows:

    \code
         list<int32> actual = ...;
         ValueHandle handle = actual;
         assert(handle.getMetaType()==Meta::Type::LIST);
         List & interf = handle;
         ValueHandle elemHandle = interf.getElement(0);
         assert(elemHandle.getMetaType()==Meta::Type::INT32);
         int32 & elemActual = elemHandle;
         elemActual = 5; // modifies the actual list
    \endcode

    A ValueHandle can be used to create a dynamically allocated copy of the
    referenced value, via its \link ValueHandle::newValue newValue \endlink
    member function. The newly allocated value can be manipulated using the
    ValueHandle object returned from the \link ValueHandle::newValue
    newValue \endlink function call, and its memory can be reclaimed via
    \link ValueHandle::deleteValue deleteValue \endlink. An example is given
    as follows:

    \code
         ValueHandle orgHandle = ...;
         ValueHandle cpyHandle = orgHandle.newValue();
         // manipulate the new value via cpyHandle;
         cpyHandle.deleteValue(); // delete iff allocated via newValue
    \endcode
    */
    class ValueHandle
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        ValueHandle(void * value, Meta::Type type, const std::type_info * tpid)
            : value_(value), type_(type), tpid_(tpid) {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Construct a value handle from a value of %SPL type
        /// @param value value of %SPL type to be used to create a handle
        /// @throws SPLRuntimeTypeMismatchException if the value is not of %SPL type
        template <class T>
        ValueHandle(T & value);

        /// Copy constructor
        /// @param o other value handle
        ValueHandle(const ValueHandle & o)
            : value_(o.value_), type_(o.type_), tpid_(o.tpid_) {}

        /// Copy constructor
        /// @param o other value handle
        ValueHandle(ValueHandle & o)
            : value_(o.value_), type_(o.type_), tpid_(o.tpid_) {}

        /// Convert to a constant value handle
        ///
        operator ConstValueHandle() const
        {
            return ConstValueHandle(value_,type_,tpid_);
        }

        /// Convert the value to a constant reference of type blob
        /// @return constant reference of type blob
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type blob
        DECL_VALUE_HANDLE_READ(blob)

        /// Convert the value to a constant reference of type float32
        /// @return constant reference of type float32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type float32
        DECL_VALUE_HANDLE_READ(float32)

        /// Convert the value to a constant reference of type float64
        /// @return constant reference of type float64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type float64
        DECL_VALUE_HANDLE_READ(float64)

        /// Convert the value to a constant reference of type decimal32
        /// @return constant reference of type decimal32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal32
        DECL_VALUE_HANDLE_READ(decimal32)

        /// Convert the value to a constant reference of type decimal64
        /// @return constant reference of type decimal64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal64
        DECL_VALUE_HANDLE_READ(decimal64)

        /// Convert the value to a constant reference of type decimal128
        /// @return constant reference of type decimal128
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal128
        DECL_VALUE_HANDLE_READ(decimal128)

        /// Convert the value to a constant reference of type complex32
        /// @return constant reference of type complex32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type complex32
        DECL_VALUE_HANDLE_READ(complex32)

        /// Convert the value to a constant reference of type complex64
        /// @return constant reference of type complex64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type complex64
        DECL_VALUE_HANDLE_READ(complex64)

        /// Convert the value to a constant reference of type boolean
        /// @return constant reference of type boolean
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type boolean
        DECL_VALUE_HANDLE_READ(boolean)

        /// Convert the value to a constant reference of type uint8
        /// @return constant reference of type uint8
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint8
        DECL_VALUE_HANDLE_READ(uint8)

        /// Convert the value to a constant reference of type uint16
        /// @return constant reference of type uint16
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint16
        DECL_VALUE_HANDLE_READ(uint16)

        /// Convert the value to a constant reference of type uint32
        /// @return constant reference of type uint32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint32
        DECL_VALUE_HANDLE_READ(uint32)

        /// Convert the value to a constant reference of type uint64
        /// @return constant reference of type uint64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint64
        DECL_VALUE_HANDLE_READ(uint64)

        /// Convert the value to a constant reference of type int8
        /// @return constant reference of type int8
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int8
        DECL_VALUE_HANDLE_READ(int8)

        /// Convert the value to a constant reference of type int16
        /// @return constant reference of type int16
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int16
        DECL_VALUE_HANDLE_READ(int16)

        /// Convert the value to a constant reference of type int32
        /// @return constant reference of type int32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int32
        DECL_VALUE_HANDLE_READ(int32)

        /// Convert the value to a constant reference of type int64
        /// @return constant reference of type int64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int64
        DECL_VALUE_HANDLE_READ(int64)

        /// Convert the value to a constant reference of type rstring
        /// @return constant reference of type rstring
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type rstring
        DECL_VALUE_HANDLE_READ(rstring)

        /// Convert the value to a constant reference of type ustring
        /// @return constant reference of type ustring
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type ustring
        DECL_VALUE_HANDLE_READ(ustring)

        /// Convert the value to a constant reference of type timestamp
        /// @return constant reference of type timestamp
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type timestamp
        DECL_VALUE_HANDLE_READ(timestamp)

        /// Convert the value to a constant reference of type BString
        /// @return constant reference of type BString
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BString
        DECL_VALUE_HANDLE_READ(BString)

        /// Convert the value to a constant reference of type List
        /// @return constant reference of type List
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type List
        DECL_VALUE_HANDLE_READ(List)

        /// Convert the value to a constant reference of type BList
        /// @return constant reference of type BList
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BList
        DECL_VALUE_HANDLE_READ(BList)

        /// Convert the value to a constant reference of type Set
        /// @return constant reference of type Set
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Set
        DECL_VALUE_HANDLE_READ(Set)

        /// Convert the value to a constant reference of type BSet
        /// @return constant reference of type BSet
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BSet
        DECL_VALUE_HANDLE_READ(BSet)

        /// Convert the value to a constant reference of type Map
        /// @return constant reference of type Map
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Map
        DECL_VALUE_HANDLE_READ(Map)

        /// Convert the value to a constant reference of type BMap
        /// @return constant reference of type BMap
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BMap
        DECL_VALUE_HANDLE_READ(BMap)

        /// Convert the value to a constant reference of type Tuple
        /// @return constant reference of type Tuple
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Tuple
        DECL_VALUE_HANDLE_READ(Tuple)

        /// Convert the value to a constant reference of type Enum
        /// @return constant reference of type Enum
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Enum
        DECL_VALUE_HANDLE_READ(Enum)

        /// Convert the value to a constant reference of type xml
        /// @return constant reference of type xml
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type xml
        DECL_VALUE_HANDLE_READ(xml)

        /// Convert the value to a constant reference of type Optional
        /// @return constant reference of type Optional
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Optional
        DECL_VALUE_HANDLE_READ(Optional)

        /// Convert the value to a reference of type blob
        /// @return reference of type blob
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type blob
        DECL_VALUE_HANDLE_WRITE(blob)

        /// Convert the value to a reference of type float32
        /// @return reference of type float32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type float32
        DECL_VALUE_HANDLE_WRITE(float32)

        /// Convert the value to a reference of type float64
        /// @return reference of type float64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type float64
        DECL_VALUE_HANDLE_WRITE(float64)

        /// Convert the value to a reference of type decimal32
        /// @return reference of type decimal32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal32
        DECL_VALUE_HANDLE_WRITE(decimal32)

        /// Convert the value to a reference of type decimal64
        /// @return reference of type decimal64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal64
        DECL_VALUE_HANDLE_WRITE(decimal64)

        /// Convert the value to a reference of type decimal128
        /// @return reference of type decimal128
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type decimal128
        DECL_VALUE_HANDLE_WRITE(decimal128)

        /// Convert the value to a reference of type complex32
        /// @return reference of type complex32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type complex32
        DECL_VALUE_HANDLE_WRITE(complex32)

        /// Convert the value to a reference of type complex64
        /// @return reference of type complex64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type complex64
        DECL_VALUE_HANDLE_WRITE(complex64)

        /// Convert the value to a reference of type boolean
        /// @return reference of type boolean
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type boolean
        DECL_VALUE_HANDLE_WRITE(boolean)

        /// Convert the value to a reference of type uint8
        /// @return reference of type uint8
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint8
        DECL_VALUE_HANDLE_WRITE(uint8)

        /// Convert the value to a reference of type uint16
        /// @return reference of type uint16
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint16
        DECL_VALUE_HANDLE_WRITE(uint16)

        /// Convert the value to a reference of type uint32
        /// @return reference of type uint32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint32
        DECL_VALUE_HANDLE_WRITE(uint32)

        /// Convert the value to a reference of type uint64
        /// @return reference of type uint64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type uint64
        DECL_VALUE_HANDLE_WRITE(uint64)

        /// Convert the value to a reference of type int8
        /// @return reference of type int8
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int8
        DECL_VALUE_HANDLE_WRITE(int8)

        /// Convert the value to a reference of type int16
        /// @return reference of type int16
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int16
        DECL_VALUE_HANDLE_WRITE(int16)

        /// Convert the value to a reference of type int32
        /// @return reference of type int32
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int32
        DECL_VALUE_HANDLE_WRITE(int32)

        /// Convert the value to a reference of type int64
        /// @return reference of type int64
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type int64
        DECL_VALUE_HANDLE_WRITE(int64)

        /// Convert the value to a reference of type rstring
        /// @return reference of type rstring
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type rstring
        DECL_VALUE_HANDLE_WRITE(rstring)

        /// Convert the value to a reference of type ustring
        /// @return reference of type ustring
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type ustring
        DECL_VALUE_HANDLE_WRITE(ustring)

        /// Convert the value to a reference of type timestamp
        /// @return reference of type timestamp
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type timestamp
        DECL_VALUE_HANDLE_WRITE(timestamp)

        /// Convert the value to a reference of type BString
        /// @return reference of type BString
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BString
        DECL_VALUE_HANDLE_WRITE(BString)

        /// Convert the value to a reference of type List
        /// @return reference of type List
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type List
        DECL_VALUE_HANDLE_WRITE(List)

        /// Convert the value to a reference of type BList
        /// @return reference of type BList
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BList
        DECL_VALUE_HANDLE_WRITE(BList)

        /// Convert the value to a reference of type Set
        /// @return reference of type Set
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Set
        DECL_VALUE_HANDLE_WRITE(Set)

        /// Convert the value to a reference of type BSet
        /// @return reference of type BSet
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BSet
        DECL_VALUE_HANDLE_WRITE(BSet)

        /// Convert the value to a reference of type Map
        /// @return reference of type Map
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Map
        DECL_VALUE_HANDLE_WRITE(Map)

        /// Convert the value to a reference of type BMap
        /// @return reference of type BMap
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type BMap
        DECL_VALUE_HANDLE_WRITE(BMap)

        /// Convert the value to a reference of type Tuple
        /// @return reference of type Tuple
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Tuple
        DECL_VALUE_HANDLE_WRITE(Tuple)

        /// Convert the value to a reference of type Enum
        /// @return reference of type Enum
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Enum
        DECL_VALUE_HANDLE_WRITE(Enum)

        /// Convert the value to a reference of type xml
        /// @return reference of type xml
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type xml
        DECL_VALUE_HANDLE_WRITE(xml)

        /// Convert the value to a reference of type Optional
        /// @return reference of type Optional
        /// @throws SPLRuntimeTypeMismatchException if the value is not of type Optional
        DECL_VALUE_HANDLE_WRITE(Optional)

        /// Create a new value of this type (dynamically allocated, see deleteValue)
        /// @return a new value of this type
        ValueHandle newValue() const;

        /// Delete the value contained
        /// Note: should be used only if the value is dynamically allocated
        void deleteValue() const;

        /// Get the serialized size of the value
        /// @return serialized size of the value
        size_t getSerializedSize() const;

        /// Get the meta type of the value
        /// @return meta type of the value
        Meta::Type getMetaType() const
        {
            return type_;
        }

        /// Get the value pointer
        /// @return value pointer
        void * getValuePointer()
        {
            return value_;
        }

        /// Get the value pointer
        /// @return value pointer
        const void * getValuePointer() const
        {
            return value_;
        }

        /// Get the hash code
        /// @return hash code
        size_t hashCode() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
        const std::type_info & getTypeId() const
        {
            return *tpid_;
        }
        void serialize(VirtualByteBuffer & buf) const;
        void deserialize(VirtualByteBuffer & buf);
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Get the string representation of the value
        /// @return string representation of the value
        std::string toString() const;

        /// Check if the value is equal to another one
        /// Equals does a deep comparison of all values (i.e for lists, sets, maps)
        /// @param o value handle to compare
        /// @return true if equal, false otherwise
        bool equals(const ConstValueHandle & o) const;

        /// Assign the value from another value handle if types match
        /// @param o value handle to assign from
        /// Return true if the types match, false otherwise
        bool assignFrom(const ConstValueHandle & o);

        /// Swap the value with another value handle if types match
        /// @param o value handle to swap with
        /// Return true if the types match, false otherwise
        bool swapWith(ValueHandle & o);

        /// Serialize to a character stream
        /// @param ostr stream to serialize into
        void serialize(std::ostream & ostr) const;

        /// Serialize to a character stream, with required precision for
        /// floating points
        /// @param ostr stream to serialize into
        void serializeWithPrecision(std::ostream & ostr) const;

        /// Deserialize from a character stream
        /// @param istr character stream to deserialize from
        void deserialize(std::istream & istr);

        /// Deserialize skipping suffix from a character stream
        /// @param istr character stream to deserialize from
        void deserializeWithSuffix(std::istream & istr);

        /// Deserialize with Inf and Nan, optionally skipping suffix from a character stream
        /// @param istr character stream to deserialize from
        /// @param withSuffix if 'true', skip suffix
        void deserializeWithNanAndInfs(std::istream & istr, bool withSuffix = false);

        /// Serialize into a native byte buffer
        /// @param buf buffer to serialize into
        void serialize(NativeByteBuffer & buf) const;

        /// Serialize into a network byte buffer
        /// @param buf buffer to serialize into
        void serialize(NetworkByteBuffer & buf) const;

        /// Deserialize from a native byte buffer
        /// @param buf buffer to deserialize from
        void deserialize(NativeByteBuffer & buf);

        /// Deserialize from a network byte buffer
        /// @param buf buffer to deserialize from
        void deserialize(NetworkByteBuffer & buf);

    private:
        void * value_;
        Meta::Type::Value type_;
        const std::type_info * tpid_;
        friend class ConstValueHandle;
    };

    template <class T>
    ValueHandle::ValueHandle(T & value)
        : value_(&value), type_(Meta::Type::typeOf<T>()), tpid_(&typeid(value))
    {
        if(type_==Meta::Type::INVALID) {
            SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_VALUEHANDLE_FAILURE("ValueHandle", typeid(T).name()), SPL_OPER_DBG);
        }
    }

    template <class T>
    ConstValueHandle::ConstValueHandle(T const & value)
        : value_(&value), type_(Meta::Type::typeOf<T>()), tpid_(&typeid(value))
    {
        if(type_==Meta::Type::INVALID) {
            SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_VALUEHANDLE_FAILURE("ConstValueHandle", typeid(T).name()), SPL_OPER_DBG);
        }
    }

    template <class T>
    size_t getSerializedSize(T const & x)
    {
        return ConstValueHandle(x).getSerializedSize();
    }
};

#ifndef DOXYGEN_SKIP_FOR_USERS
#define DECL_INTEGER_TYPE_CONSTRUCTOR_SPECIALIZATION(type) \
    template<> inline type::type(ValueHandle const & h) : val_(*(const type*) h.getValuePointer()) {} \
    template<> inline type const & type::operator=(ValueHandle const & h) { val_ = *(const type*) h.getValuePointer(); return *this; } \
    template<> inline type::type(ConstValueHandle const & h) : val_(*(type*) h.getValuePointer()) {} \
    template<> inline type const & type::operator=(ConstValueHandle const & h) { val_ = *(type*) h.getValuePointer(); return *this; } \

namespace SPL {
  DECL_INTEGER_TYPE_CONSTRUCTOR_SPECIALIZATION(int8)
  DECL_INTEGER_TYPE_CONSTRUCTOR_SPECIALIZATION(uint8)
  DECL_INTEGER_TYPE_CONSTRUCTOR_SPECIALIZATION(boolean)
}

#undef DECL_INTEGER_TYPE_CONSTRUCTOR_SPECIALIZATION
#endif

#undef DECL_VALUE_HANDLE_READ
#undef DECL_VALUE_HANDLE_WRITE
#undef SPL_COMMENT
#undef SPL_SLASH

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_VALUE_HANDLE_H */
