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

#ifndef SPL_RUNTIME_TYPE_META_BASE_TYPE_IMPL_H
#define SPL_RUNTIME_TYPE_META_BASE_TYPE_IMPL_H

#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/Mutex.h>
#include <boost/shared_ptr.hpp>

namespace SPL {
namespace Meta {
class BaseTypeImpl;

/// \brief Class for instantiating types
class DLL_PUBLIC BaseTypeFactory
{
  public:
    typedef boost::shared_ptr<BaseTypeImpl> P;
    typedef std::tr1::unordered_map<std::string, P> TypeMap;

    /// Construct a meta-type from an spl type
    ///
    template<class T>
    static const BaseTypeImpl& makeTypeFrom()
    {
        T t;
        return makeType(t);
    }

    /*! Construct a meta-type given a value hadle

        Sample usage:
        \code
        Tuple & T = ...
        BaseTypeImpl const & tp = BaseTypeImpl::makeType(T);

        list<map<int,float> > & l = ...
        BaseTypeImpl const & tp = BaseTypeImpl::makeType(l);

        ValueHandle v = ...
        BaseTypeImpl const & tp = BaseTypeImpl::makeType(v);
        \endcode
        @return a base type represeting the type hierarchy
    */
    static const BaseTypeImpl& makeType(const ConstValueHandle& vh);

  private:
    static BaseTypeImpl* constructType(const ConstValueHandle& vh);
    static const BaseTypeImpl& makeTypeRaw(const ConstValueHandle& vh);

    static TypeMap types_;
    static Distillery::Mutex mutex_;
};

/// \brief Base class for all meta-type classes
class BaseTypeImpl : public virtual BaseType
{
  public:
    /// Empty constructor
    BaseTypeImpl() {}

    /// Constructor
    BaseTypeImpl(const std::string& name, Type type)
      : name_(name)
      , type_(type)
    {}

    /// Destructor
    ///
    virtual ~BaseTypeImpl() {}

    /// Get the name of the type
    /// for example tuple<string name,uint32 id>
    /// @return Returns the name of the type
    const std::string& getName() const { return name_; }

    /// Get the meta tpye
    /// @return the meta type
    const Type& getMetaType() const { return type_; }

    /// Check if this is a templated type (has \<...\> as part of its type name in spl)
    ///
    /// This is equivalent to <tt>isCollection() || isTuple()</tt>
    /// @return true if this is a templated type
    bool isTemplated() const { return isCollection() || isTuple(); }

    // Type inspection functions

    /// Check if this type is a primitive type
    ///
    /// This is equivalent to <tt>!isComposite()</tt>
    /// @return true if this is a primitive type, false otherwise
    bool isPrimitive() const { return !isComposite(); }

    /// Check if this type is a composite type
    ///
    /// This is equivalent to <tt>isTuple() || isCollection()</tt>
    /// @return true if this is a composite type, false otherwise
    bool isComposite() const { return isTuple() || isCollection(); }

    /// Check if this type is a collection type
    ///
    /// This is equivalent to <tt>isSet() || isMap() || isList() || isBSet() || isBMap() ||
    /// isBList()</tt>
    /// @return true if this is a collection type, false otherwise
    bool isCollection() const
    {
        return isSet() || isMap() || isList() || isBSet() || isBMap() || isBList();
    }

    /// Check if this type is a bounded type
    ///
    /// This is equivalent to <tt>isBSet() || isBMap() || isBList() || isBString()</tt>
    /// @return true if this is a bounded type, false otherwise
    bool isBounded() const { return isBSet() || isBMap() || isBList() || isBString(); }

    /// Check if this is a string type
    ///
    /// This is equivalent to <tt>isRString() || isBString() || isUString()</tt>
    /// @return true if this is a string type, false otherwise
    bool isString() const { return isRString() || isBString() || isUString(); }

    /// Check if this is an ordered type
    ///
    /// This is equivalent to <tt>isIntegral() || isFloatingpoint() || isEnum() || isString() ||
    /// isTimestamp()</tt>
    /// @return true if this is an ordered type, false otherwise
    bool isOrdered() const
    {
        return isIntegral() || isFloatingpoint() || isEnum() || isString() || isTimestamp();
    }

    /// Check if this is a numeric type
    ///
    /// This is equivalent to <tt>isIntegral() || isFloatingpoint() || isComplex()</tt>
    /// @return true if this is a numeric type, false otherwise
    bool isNumeric() const { return isIntegral() || isFloatingpoint() || isComplex(); }

    /// Check if this is an integral numeric type
    ///
    /// This is equivalent to <tt>isSigned() || isUnsigned()</tt>
    /// @return true if this is an integral numeric type, false otherwise
    bool isIntegral() const { return isSigned() || isUnsigned(); }

    /// Check if this is a floating point type
    ///
    /// This is equivalent to <tt>isFloat() || isDecimal()</tt>
    /// @return true if this is a floating point type, false otherwise
    bool isFloatingpoint() const { return isFloat() || isDecimal(); }

    /// Check if this is a binary float type
    ///
    /// This is equivalent to <tt>isFloat32() || isFloat64()</tt>
    /// @return true if this is a binary float type, false otherwise
    bool isFloat() const { return isFloat32() || isFloat64(); }

    /// Check if this is a decimal float type
    ///
    /// This is equivalent to <tt>isDecimal32() || isDecimal64() || isDecimal128()</tt>
    /// @return true if this is a decimal float type, false otherwise
    bool isDecimal() const { return isDecimal32() || isDecimal64() || isDecimal128(); }

    /// Check if this is a complex type
    ///
    /// This is equivalent to <tt>isComplex32() || isComplex64(</tt>
    /// @return true if this is a complex type, false otherwise
    bool isComplex() const { return isComplex32() || isComplex64(); }

    /// Check if this is a signed integral type
    ///
    /// This is equivalent to <tt>isInt8() || isInt16() || isInt32() || isInt64()</tt>
    /// @return true if this is a signed integral type, false otherwise
    bool isSigned() const { return isInt8() || isInt16() || isInt32() || isInt64(); }

    /// Check if this is an unsigned integral type
    ///
    /// This is equivalent to <tt>isUint8() || isUint16() || isUint32() || isUint64()</tt>
    /// @return true if this is an unsigned integral type, false otherwise
    bool isUnsigned() const { return isUint8() || isUint16() || isUint32() || isUint64(); }

    // Individual types
    /// Check if this is a tuple type
    /// @return true if this is a tuple type, false otherwise
    bool isTuple() const { return type_ == Type::TUPLE; }

    /// Check if this is a list type
    /// @return true if this is a list type, false otherwise
    bool isList() const { return type_ == Type::LIST; }

    /// Check if this is a map type
    /// @return true if this is a map type, false otherwise
    bool isMap() const { return type_ == Type::MAP; }

    /// Check if this is a set type
    /// @return true if this is a set type, false otherwise
    bool isSet() const { return type_ == Type::SET; }

    /// Check if this is a bounded list type
    /// @return true if this is a bounded list type, false otherwise
    bool isBList() const { return type_ == Type::BLIST; }

    /// Check if this is a bounded map type
    /// @return true if this is a bounded map type, false otherwise
    bool isBMap() const { return type_ == Type::BMAP; }

    /// Check if this is a bounded set type
    /// @return true if this is a bounded set type, false otherwise
    bool isBSet() const { return type_ == Type::BSET; }

    /// Check if this is a timestamp type
    /// @return true if this is a timestamp type, false otherwise
    bool isTimestamp() const { return type_ == Type::TIMESTAMP; }

    /// Check if this is a blob type
    /// @return true if this is a blob type, false otherwise
    bool isBlob() const { return type_ == Type::BLOB; }

    /// Check if this is a enum type
    /// @return true if this is a enum type, false otherwise
    bool isEnum() const { return type_ == Type::ENUM; }

    /// Check if this is a boolean type
    /// @return true if this is a boolean type, false otherwise
    bool isBoolean() const { return type_ == Type::BOOLEAN; }

    /// Check if this is a rstring type
    /// @return true if this is a rstring type, false otherwise
    bool isRString() const { return type_ == Type::RSTRING; }

    /// Check if this is a bounded rstring type
    /// @return true if this is a bounded rstring type, false otherwise
    bool isBString() const { return type_ == Type::BSTRING; }

    /// Check if this is a ustring type
    /// @return true if this is a ustring type, false otherwise
    bool isUString() const { return type_ == Type::USTRING; }

    /// Check if this is a int8 type
    /// @return true if this is a int8 type, false otherwise
    bool isInt8() const { return type_ == Type::INT8; }

    /// Check if this is a int16 type
    /// @return true if this is a int16 type, false otherwise
    bool isInt16() const { return type_ == Type::INT16; }

    /// Check if this is a int32 type
    /// @return true if this is a int32 type, false otherwise
    bool isInt32() const { return type_ == Type::INT32; }

    /// Check if this is a int64 type
    /// @return true if this is a int64 type, false otherwise
    bool isInt64() const { return type_ == Type::INT64; }

    /// Check if this is a uint8 type
    /// @return true if this is a uint8 type, false otherwise
    bool isUint8() const { return type_ == Type::UINT8; }

    /// Check if this is a uint16 type
    /// @return true if this is a uint16 type, false otherwise
    bool isUint16() const { return type_ == Type::UINT16; }

    /// Check if this is a uint32 type
    /// @return true if this is a uint32 type, false otherwise
    bool isUint32() const { return type_ == Type::UINT32; }

    /// Check if this is a uint64 type
    /// @return true if this is a uint64 type, false otherwise
    bool isUint64() const { return type_ == Type::UINT64; }

    /// Check if this is a float32 type
    /// @return true if this is a float32 type, false otherwise
    bool isFloat32() const { return type_ == Type::FLOAT32; }

    /// Check if this is a float64 type
    /// @return true if this is a float64 type, false otherwise
    bool isFloat64() const { return type_ == Type::FLOAT64; }

    /// Check if this is a decimal32 type
    /// @return true if this is a decimal32 type, false otherwise
    bool isDecimal32() const { return type_ == Type::DECIMAL32; }

    /// Check if this is a decimal64 type
    /// @return true if this is a decima64 type, false otherwise
    bool isDecimal64() const { return type_ == Type::DECIMAL64; }

    /// Check if this is a decimal128 type
    /// @return true if this is a decima128 type, false otherwise
    bool isDecimal128() const { return type_ == Type::DECIMAL128; }

    /// Check if this is a complex32 type
    /// @return true if this is a complex32 type, false otherwise
    bool isComplex32() const { return type_ == Type::COMPLEX32; }

    /// Check if this is a complex64 type
    /// @return true if this is a complex64 type, false otherwise
    bool isComplex64() const { return type_ == Type::COMPLEX64; }

    /// Check if this is an xml type
    /// @return true if this is an xml type, false otherwise
    bool isXml() const { return type_ == Type::XML; }

    /// Check if this is an xml type
    /// @return true if this is an xml type, false otherwise
    bool isOptional() const { return type_ == Type::OPTIONAL; }

  protected:
    std::string name_;
    Type type_;
};
std::ostream& operator<<(std::ostream&, BaseTypeImpl const&);

/// Meta-type class representing a primitive type
class PrimitiveTypeImpl
  : public virtual PrimitiveType
  , public BaseTypeImpl
{
  public:
    PrimitiveTypeImpl(const std::string& name, const Type& type)
      : BaseTypeImpl(name, type)
    {}
    friend class BaseTypeImpl;
};

/// Meta-type class representing a simple primitive type, where a simple
/// type is one that is fully described by its meta-type enumeration
class SimplePrimitiveTypeImpl
  : public virtual SimplePrimitiveType
  , public PrimitiveTypeImpl
{
  public:
    SimplePrimitiveTypeImpl(Type const&);
    friend class BaseTypeImpl;
};

/// Meta-type class representing an xml primitive type
class XmlTypeImpl
  : public virtual XmlType
  , public BaseTypeImpl
{
  public:
    XmlTypeImpl(xml const&);
    friend class BaseTypeImpl;
    bool hasSchema() const { return !schema_.empty(); }
    std::string getSchema() const { return schema_; }

  private:
    std::string schema_;
    std::string makeName(xml const&);
};

/// Meta-type class representing an enum type
class EnumTypeImpl
  : public virtual EnumType
  , public PrimitiveTypeImpl
{
  public:
    typedef std::vector<std::string> Values;
    EnumTypeImpl(Values*);
    const Values& getValidValues() const { return *values_; }
    uint32_t getNumberOfValues() const { return values_->size(); }
    static std::string makeName(Values*);

  private:
    std::auto_ptr<Values> values_;
    friend class BaseTypeImpl;
};

/// Meta-type class representing a bounded string type
class BStringTypeImpl
  : public virtual BStringType
  , public PrimitiveTypeImpl
{
  public:
    BStringTypeImpl(int32_t bound);
    int32_t getBound() const { return bound_; }
    static std::string makeName(int32 bound);

  private:
    int32_t bound_;
    friend class BaseTypeImpl;
};

/// Meta-type class representing a composite type
class CompositeTypeImpl
  : public virtual CompositeType
  , public BaseTypeImpl
{
  public:
    CompositeTypeImpl(const std::string& name, const Type& type)
      : BaseTypeImpl(name, type)
    {}
    friend class BaseTypeImpl;
};

/// Meta-type class representing a collection type
class CollectionTypeImpl
  : public virtual CollectionType
  , public CompositeTypeImpl
{
  public:
    CollectionTypeImpl(const std::string& name, const Type& type)
      : CompositeTypeImpl(name, type)
    {}
    friend class BaseTypeImpl;
};

/// Meta-type class representing a list type
class ListTypeImpl
  : public virtual ListType
  , public CollectionTypeImpl
{
  public:
    ListTypeImpl(const BaseTypeImpl& etype);
    BaseType const& getElementType() const { return etype_; }
    static std::string makeName(const BaseTypeImpl& etype);

  private:
    const BaseTypeImpl& etype_;
    friend class BaseTypeImpl;
    friend class BListTypeImpl;
};

/// Meta-type class representing an optional type
class OptionalTypeImpl : public BaseTypeImpl
{
  public:
    OptionalTypeImpl(const BaseTypeImpl& utype);
    BaseType const& getUnderlyingType() const { return utype_; }
    static std::string makeName(const BaseTypeImpl& utype);

  private:
    const BaseTypeImpl& utype_;
    friend class BaseTypeImpl;
};

/// Meta-type class representing a set type
class SetTypeImpl
  : public virtual SetType
  , public CollectionTypeImpl
{
  public:
    SetTypeImpl(const BaseTypeImpl& etype);
    BaseType const& getElementType() const { return etype_; }
    static std::string makeName(const BaseTypeImpl& etype);

  private:
    const BaseTypeImpl& etype_;
    friend class BaseTypeImpl;
    friend class BSetTypeImpl;
};

/// Meta-type class representing a map type
class MapTypeImpl
  : public virtual MapType
  , public CollectionTypeImpl
{
  public:
    MapTypeImpl(const BaseTypeImpl& ktype, const BaseTypeImpl& vtype);
    std::pair<const BaseType*, const BaseType*> getElementType() const
    {
        return std::make_pair(&ktype_, &vtype_);
    }
    const BaseType& getKeyType() const { return ktype_; }
    const BaseType& getValueType() const { return vtype_; }
    static std::string makeName(const BaseTypeImpl& k, const BaseTypeImpl& v);

  private:
    const BaseTypeImpl& ktype_;
    const BaseTypeImpl& vtype_;
    friend class BaseTypeImpl;
    friend class BMapTypeImpl;
};

/// Meta-type class representing a bounded collection type
class BCollectionTypeImpl
  : public virtual BCollectionType
  , public CollectionTypeImpl
{
  public:
    BCollectionTypeImpl(const std::string& name, const Type& type, int32_t bound)
      : CollectionTypeImpl(name, type)
      , bound_(bound)
    {}
    int32_t getBound() const { return bound_; }

  private:
    int32_t bound_;
    friend class BaseTypeImpl;
};

/// Meta-type class representing a bounded list type
class BListTypeImpl
  : public virtual BListType
  , public BCollectionTypeImpl
{
  public:
    BListTypeImpl(const BaseTypeImpl& etype, int32_t bound);
    const BaseType& getElementType() const { return etype_; }
    static std::string makeName(const BaseTypeImpl&, int32_t bound);

  private:
    BaseTypeImpl const& etype_;
    friend class BaseTypeImpl;
};

/// Meta-type class representing a bounded set type
class BSetTypeImpl
  : public virtual BSetType
  , public BCollectionTypeImpl
{
  public:
    BSetTypeImpl(const BaseTypeImpl& etype, int32_t bound);
    const BaseType& getElementType() const { return etype_; }
    static std::string makeName(const BaseTypeImpl&, int32_t bound);

  private:
    const BaseTypeImpl& etype_;
    friend class BaseTypeImpl;
};

/// Meta-type class representing a bounded map type
class BMapTypeImpl
  : public virtual BMapType
  , public BCollectionTypeImpl
{
  public:
    BMapTypeImpl(const BaseTypeImpl& ktype, const BaseTypeImpl& vtype, int32_t bound);
    std::pair<const BaseType*, const BaseType*> getElementType() const
    {
        return std::make_pair(&ktype_, &vtype_);
    }
    const BaseType& getKeyType() const { return ktype_; }
    const BaseType& getValueType() const { return vtype_; }
    static std::string makeName(const BaseTypeImpl&, const BaseTypeImpl&, int32_t bound);

  private:
    const BaseTypeImpl& ktype_;
    const BaseTypeImpl& vtype_;
    friend class BaseTypeImpl;
};

/// Meta-type class representing a tuple type
class TupleTypeImpl
  : public virtual TupleType
  , public CompositeTypeImpl
{
  public:
    typedef std::vector<std::pair<std::string, BaseTypeImpl const*> > Attributes;
    typedef std::tr1::unordered_map<std::string, uint32_t> Name2Index;
    TupleTypeImpl(Name2Index*, Attributes*);
    uint32_t getNumberOfAttributes() const { return attrs_->size(); }
    const std::string& getAttributeName(uint32_t index) const
    {
        return attrs_->operator[](index).first;
    }
    const Name2Index& getAttributeNames() const { return *name2index_; }
    const BaseType& getAttributeType(const std::string& attrb) const
    {
        return getAttributeType(name2index_->operator[](attrb));
    }
    const BaseType& getAttributeType(uint32_t index) const
    {
        return *(attrs_->operator[](index).second);
    }
    static std::string makeName(Attributes*);

  private:
    std::auto_ptr<Name2Index> name2index_;
    std::auto_ptr<Attributes> attrs_;
    friend class BaseTypeImpl;
};
};
};

#endif /* SPL_RUNTIME_TYPE_META_BASE_TYPE_IMPL_H */
