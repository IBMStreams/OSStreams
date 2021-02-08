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

#ifndef SPL_RUNTIME_TYPE_BASE_TYPE_H
#define SPL_RUNTIME_TYPE_BASE_TYPE_H

/*!
 * \file BaseType.h \brief Definitions of the SPL::Meta::BaseType class and its
 * derived classes.
 */

/*!
 * \namespace SPL::Meta \brief C++ classes for type introspection
 */

#include <SPL/Runtime/Type/Meta/Type.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <string>
#include <vector>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
  class ConstValueHandle;

  namespace Meta
  {

    /// \brief Class that serves as a base for all meta-type classes
    class BaseType
    {
    public:

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        ///
        virtual ~BaseType() {}
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Construct a meta-type from an spl type
        ///
        /// Sample usage:
        /// \code BaseType const & type = makeTypeFrom<list<int32> >(); \endcode
        template <class T>
        static BaseType const & makeTypeFrom()
        {
            T t; return makeType(t);
        }

        /*! Construct a meta-type given a value handle

        Sample usage:
        \code
        Tuple & T = ...
        BaseType const & tp = BaseType::makeType(T);

        list<map<int,float> > & l = ...
        BaseType const & tp = BaseType::makeType(l);

        ValueHandle v = ...
        BaseType const & tp = BaseType::makeType(v);
        \endcode
        */
        /// @return a base type represeting the type hierarchy
        static BaseType const & makeType(const ConstValueHandle & vh) DLL_PUBLIC;

        /*! Declares the primary template for a const downcaster
        Before the downcaster is used you must verify that the object is of
        the desired type using the \link SPL::Meta::BaseType::as as\endlink
        method

        Sample usage:
        \code
        list<map<int,float> > & l = ...
        BaseType const & tp = BaseType::makeType(l);
        ListType const & ltp = tp.as<ListType const&>();
        \endcode
        */
        template <class T> const T & as() const;

        /*! Declares the primary template for a method to query if the object is one of a specific kind.
        @return Returns true if the object is of the specific kind, otherwise false.

        Sample usage:
        \code
        list<map<int,float> > & l = ...
        BaseType const & tp = BaseType::makeType(l);
        assert(tp.is<ListType const&>());
        \endcode
        */
        template <class T> bool is() const;

        /// Get the name of the type
        /// For example: \c "tuple<string name,uint32 id>"
        /// @return Returns the name of the type
        virtual const std::string & getName() const = 0;

        /// Get the meta type
        /// @return the meta type
        virtual Type const & getMetaType() const = 0;

        /// Check if this is a templated type (has \<...\> as part of its type name in spl)
        ///
        /// This is equivalent to <tt>isCollection() || isTuple()</tt>
        /// @return true if this is a templated type
        virtual bool isTemplated() const = 0;

        // Type inspection functions

        /// Check if this type is a primitive type
        ///
        /// This is equivalent to <tt>!isComposite()</tt>
        /// @return true if this is a primitive type, false otherwise
        virtual bool isPrimitive() const = 0;

        /// Check if this type is a composite type
        ///
        /// This is equivalent to <tt>isTuple() || isCollection()</tt>
        /// @return true if this is a composite type, false otherwise
        virtual bool isComposite() const = 0;

        /// Check if this type is a collection type
        ///
        /// This is equivalent to <tt>isSet() || isMap() || isList() || isBSet() || isBMap() || isBList()</tt>
        /// @return true if this is a collection type, false otherwise
        virtual bool isCollection() const = 0;

        /// Check if this type is a bounded type
        ///
        /// This is equivalent to <tt>isBSet() || isBMap() || isBList() || isBString()</tt>
        /// @return true if this is a bounded type, false otherwise
        virtual bool isBounded() const = 0;

        /// Check if this is a string type
        ///
        /// This is equivalent to <tt>isRString() || isBString() || isUString()</tt>
        /// @return true if this is a string type, false otherwise
        virtual bool isString() const = 0;

        /// Check if this is an ordered type
        ///
        /// This is equivalent to <tt>isIntegral() || isFloatingpoint() || isEnum() || isString() || isTimestamp()</tt>
        /// @return true if this is an ordered type, false otherwise
        virtual bool isOrdered() const = 0;

        /// Check if this is a numeric type
        ///
        /// This is equivalent to <tt>isIntegral() || isFloatingpoint() || isComplex()</tt>
        /// @return true if this is a numeric type, false otherwise
        virtual bool isNumeric() const = 0;

        /// Check if this is an integral numeric type
        ///
        /// This is equivalent to <tt>isSigned() || isUnsigned()</tt>
        /// @return true if this is an integral numeric type, false otherwise
        virtual bool isIntegral() const = 0;

        /// Check if this is a floating point type
        ///
        /// This is equivalent to <tt>isFloat() || isDecimal()</tt>
        /// @return true if this is a floating point type, false otherwise
        virtual bool isFloatingpoint() const = 0;

        /// Check if this is a binary float type
        ///
        /// This is equivalent to <tt>isFloat32() || isFloat64()</tt>
        /// @return true if this is a binary float type, false otherwise
        virtual bool isFloat() const = 0;

        /// Check if this is a decimal float type
        ///
        /// This is equivalent to <tt>isDecimal32() || isDecimal64() || isDecimal128()</tt>
        /// @return true if this is a decimal float type, false otherwise
        virtual bool isDecimal() const = 0;

        /// Check if this is a complex type
        ///
        /// This is equivalent to <tt>isComplex32() || isComplex64()</tt>
        /// @return true if this is a complex type, false otherwise
        virtual bool isComplex() const = 0;

        /// Check if this is a signed integral type
        ///
        /// This is equivalent to <tt>isInt8() || isInt16() || isInt32() || isInt64()</tt>
        /// @return true if this is a signed integral type, false otherwise
        virtual bool isSigned() const = 0;

        /// Check if this is an unsigned integral type
        ///
        /// This is equivalent to <tt>isUint8() || isUint16() || isUint32() || isUint64()</tt>
        /// @return true if this is an unsigned integral type, false otherwise
        virtual bool isUnsigned() const = 0;

        // Individual types
        /// Check if this is a tuple type
        /// @return true if this is a tuple type, false otherwise
        virtual bool isTuple() const = 0;

        /// Check if this is a list type
        /// @return true if this is a list type, false otherwise
        virtual bool isList() const = 0;

        /// Check if this is a map type
        /// @return true if this is a map type, false otherwise
        virtual bool isMap() const = 0;

        /// Check if this is a set type
        /// @return true if this is a set type, false otherwise
        virtual bool isSet() const = 0;

        /// Check if this is a bounded list type
        /// @return true if this is a bounded list type, false otherwise
        virtual bool isBList() const = 0;

        /// Check if this is a bounded map type
        /// @return true if this is a bounded map type, false otherwise
        virtual bool isBMap() const = 0;

        /// Check if this is a bounded set type
        /// @return true if this is a bounded set type, false otherwise
        virtual bool isBSet() const = 0;

        /// Check if this is a timestamp type
        /// @return true if this is a timestamp type, false otherwise
        virtual bool isTimestamp() const  = 0;

        /// Check if this is a blob type
        /// @return true if this is a blob type, false otherwise
        virtual bool isBlob() const = 0;

        /// Check if this is a enum type
        /// @return true if this is a enum type, false otherwise
        virtual bool isEnum() const  = 0;

        /// Check if this is a boolean type
        /// @return true if this is a boolean type, false otherwise
        virtual bool isBoolean() const = 0;

        /// Check if this is a rstring type
        /// @return true if this is a rstring type, false otherwise
        virtual bool isRString() const = 0;

        /// Check if this is a bounded rstring type
        /// @return true if this is a bounded rstring type, false otherwise
        virtual bool isBString() const = 0;

        /// Check if this is a ustring type
        /// @return true if this is a ustring type, false otherwise
        virtual bool isUString() const = 0;

        /// Check if this is a int8 type
        /// @return true if this is a int8 type, false otherwise
        virtual bool isInt8() const = 0;

        /// Check if this is a int16 type
        /// @return true if this is a int16 type, false otherwise
        virtual bool isInt16() const = 0;

        /// Check if this is a int32 type
        /// @return true if this is a int32 type, false otherwise
        virtual bool isInt32() const = 0;

        /// Check if this is a int64 type
        /// @return true if this is a int64 type, false otherwise
        virtual bool isInt64() const = 0;

        /// Check if this is a uint8 type
        /// @return true if this is a uint8 type, false otherwise
        virtual bool isUint8() const = 0;

        /// Check if this is a uint16 type
        /// @return true if this is a uint16 type, false otherwise
        virtual bool isUint16() const = 0;

        /// Check if this is a uint32 type
        /// @return true if this is a uint32 type, false otherwise
        virtual bool isUint32() const = 0;

        /// Check if this is a uint64 type
        /// @return true if this is a uint64 type, false otherwise
        virtual bool isUint64() const = 0;

        /// Check if this is a float32 type
        /// @return true if this is a float32 type, false otherwise
        virtual bool isFloat32() const = 0;

        /// Check if this is a float64 type
        /// @return true if this is a float64 type, false otherwise
        virtual bool isFloat64() const = 0;

        /// Check if this is a decimal32 type
        /// @return true if this is a decimal32 type, false otherwise
        virtual bool isDecimal32() const = 0;

        /// Check if this is a decimal64 type
        /// @return true if this is a decima64 type, false otherwise
        virtual bool isDecimal64() const = 0;

        /// Check if this is a decimal128 type
        /// @return true if this is a decima128 type, false otherwise
        virtual bool isDecimal128() const = 0;

        /// Check if this is a complex32 type
        /// @return true if this is a complex32 type, false otherwise
        virtual bool isComplex32() const = 0;

        /// Check if this is a complex64 type
        /// @return true if this is a complex64 type, false otherwise
        virtual bool isComplex64() const = 0;

        /// Check if this is an xml type
        /// @return true if this is an xml type, false otherwise
        virtual bool isXml() const = 0;

        /// Check if this is an optional type
        /// @return true if this is an optional type, false otherwise
        virtual bool isOptional() const = 0;
    };
    bool operator==(BaseType const& lhs, BaseType const& rhs) DLL_PUBLIC;
    bool operator!=(BaseType const& lhs, BaseType const& rhs) DLL_PUBLIC;
    std::ostream& operator<<(std::ostream&, BaseType const&) DLL_PUBLIC;

#define META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(asType) \
    template<> inline bool BaseType::is<asType>() const \
        { return NULL != dynamic_cast<asType const*>(this); } \
    template<> inline asType const& BaseType::as<asType>() const \
        { assert(NULL != dynamic_cast<asType const*>(this)); return *dynamic_cast<asType const*>(this); }

#define META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(isEnum, asType) \
    template<> inline bool BaseType::is<asType>() const \
        { return Type::isEnum == getMetaType(); } \
    template<> inline asType const& BaseType::as<asType>() const \
        { assert(NULL != dynamic_cast<asType const*>(this)); return *dynamic_cast<asType const*>(this); }

    /// Class that provides a meta-type representation of a primitive type
    class PrimitiveType : public virtual BaseType {};
    META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(PrimitiveType)

    /// \brief Class that provides a meta-type representation of a simple
    /// primitive type.
    ///
    /// A simple primitive type is one that is fully described by its meta-type
    /// enumeration.
    class SimplePrimitiveType : public virtual PrimitiveType {};
    META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(SimplePrimitiveType)

    /// \brief Class that provides a meta-type representation of an xml primitive type.
    ///
    /// xml types may have schemas
    class XmlType : public virtual BaseType {
    public:
        /// Does the xml type have an associated schema?
        /// @return true if there is a schema
        virtual bool hasSchema() const = 0;

        /// The the schema associated with this XML type
        /// @return the schema or an empty string if there is no associated schema
        virtual std::string getSchema() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(XmlType)

    class OptionalType : public virtual BaseType {
    public:
        /// Get the underlying type
        /// @return underlying type
        virtual BaseType const & getUnderlyingType() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(OptionalType)

    /// Class that provides a meta-type representation of an enum type
    class EnumType : public virtual PrimitiveType
    {
    public:
        typedef std::vector<std::string> Values;
        /// Get the valid enumeration values
        /// @return valid enumeration values
        virtual Values const & getValidValues() const = 0;
        /// Get the number of enumeration values
        /// @return number of enumeration values
        virtual uint32_t getNumberOfValues() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(ENUM, EnumType)

    /// Class that provides a meta-type representation of a bounded string type
    class BStringType : public virtual PrimitiveType
    {
    public:
        /// Get the bound size
        /// @return bound size
        virtual int32_t getBound() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(BSTRING, BStringType)

    /// Class that provides a meta-type representation of a composite type
    class CompositeType : public virtual BaseType {};
    META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(CompositeType)

    /// Class that provides a meta-type representation of a collection type
    class CollectionType : public virtual CompositeType {};
    META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(CollectionType)

    /// Class that provides a meta-type representation of a list type
    class ListType : public virtual CollectionType
    {
    public:
        /// Get the element type
        /// @return element type
        virtual BaseType const & getElementType() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(LIST, ListType)

    /// Class that provides a meta-type representation of a set type
    class SetType : public virtual CollectionType
    {
    public:
        /// Get the element type
        /// @return element type
        virtual BaseType const & getElementType() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(SET, SetType)

    /// Class that provides a meta-type representation of a map type
    class MapType : public virtual CollectionType
    {
    public:
        /// Get the element type (key and value types)
        /// @return element type (key and value types)
        virtual std::pair<BaseType const *, BaseType const *> getElementType() const = 0;

        /// Get the key type
        /// @return key type
        virtual BaseType const & getKeyType() const = 0;

        /// Get the value type
        /// @return value type
        virtual BaseType const & getValueType() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(MAP, MapType)

    /// Class that provides a meta-type representation of a bounded collection type
    class BCollectionType : public virtual CollectionType
    {
    public:
        /// Get the bound size
        /// @return bound size
        virtual int32_t getBound() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_DC(BCollectionType)

    /// Class that provides a meta-type representation of a bounded list type
    class BListType : public virtual BCollectionType
    {
    public:
        /// Get the element type
        /// @return element type
        virtual BaseType const & getElementType() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(BLIST, BListType)

    /// Class that provides a meta-type representation of a bounded set type
    class  BSetType : public virtual BCollectionType
    {
    public:
        /// Get the element type
        /// @return element type
        virtual BaseType const & getElementType() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(BSET, BSetType)

    /// Class that provides a meta-type representation of a bounded map type
    class BMapType : public virtual BCollectionType
    {
    public:
        /// Get the element type (key and value types)
        /// @return element type (key and value types)
        virtual std::pair<BaseType const *, BaseType const *> getElementType() const = 0;

        /// Get the key type
        /// @return key type
        virtual BaseType const & getKeyType() const = 0;

        /// Get the value type
        /// @return value type
        virtual BaseType const & getValueType() const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(BMAP, BMapType)

    /// Class that provides a meta-type representation of a tuple type
    class TupleType : public virtual CompositeType
    {
    public:
        typedef std::tr1::unordered_map<std::string,uint32_t> Name2Index;

        /// Get the number of attributes
        /// @return number of attributes
        virtual uint32_t getNumberOfAttributes() const = 0;

        /// Get the attribute name
        /// @param index attribute index
        /// @return attribute name
        virtual std::string const & getAttributeName(uint32_t index) const = 0;

        /// Get the attribute names, with their index mapping
        /// @return list of attribute names, with their index mapping
        virtual Name2Index const & getAttributeNames() const = 0;

        /// Get the type for a given attribute name
        /// @param attrb attribute name
        /// @return type of the attribute
        virtual BaseType const & getAttributeType(const std::string & attrb) const = 0;

        /// Get the type for a given attribute index
        /// @param index attribute index
        /// @return type of the attribute
        virtual BaseType const & getAttributeType(uint32_t index) const = 0;
    };
    META_BASETYPE_IS_AS_SPECIALIZATIONS_MT(TUPLE, TupleType)
  };
};

#endif /* SPL_RUNTIME_TYPE_BASE_TYPE_H */
