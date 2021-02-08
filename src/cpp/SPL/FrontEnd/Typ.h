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

#ifndef SPL_FRONT_END_TYP_H
#define SPL_FRONT_END_TYP_H

#include <SPL/FrontEnd/MetaType.h>
#include <SPL/Runtime/Type/TypeTMPHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <UTILS/Mutex.h>

#include <boost/shared_ptr.hpp>

PUSH_DLL_PUBLIC

namespace SPL {
namespace Meta {
class BaseType;
}

/// Base class for all compile-time type representations. Their names all end in "Typ" without
/// the "e" to distinguish them from the run-time type representations in namespace "Meta".
class RootTyp
{
  public:
    /// Empty constructor
    RootTyp() {}

    /// Constructor
    RootTyp(const std::string& name, MetaType type)
      : name_(name)
      , type_(type)
    {}

    /// Destructor
    ///
    virtual ~RootTyp() {}

    /// Construct from runtime representation
    /// This interface is used by the debugger to convert the runtime meta
    /// type to a compile-time one, so that expression evaluation can be performed
    /// @return compile-time representation
    static RootTyp const& fromRuntimeRepresentation(Meta::BaseType const& btype);

    /// Declares the primary template for a const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    const T& as() const;

    /// Declares the primary template for a non-const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    T& as();

    /// Declares the primary template for a method to query if the object is one of a specific kind.
    /// @return Returns true if the object is of the specific kind, otherwise false.
    template<class T>
    bool is() const;

    /// Get the fixed size of the type
    /// Fixed sized types include scalar types, bstring, blist of scalar
    /// types, bset of scalar types, and bmap of scalar types. For fixed
    /// sized types, the memory layout and the serialized format should
    /// match when native byte order is being used for binary serialization
    /// @return fixed size of the type
    virtual uint64_t getFixedSize() const
    {
        assert(!"cannot happen");
        return 0;
    }

    /// Return an estimate of the size.  A guess for collections
    /// @return size estimate
    virtual uint64_t getSizeEstimate() const = 0;

    /// Get the alignment of the type
    /// @return alignment of the type
    virtual uint8_t getAlignment() const
    {
        assert(!"cannot happen");
        return 0;
    }

    /// Get the name of the type
    /// for example tuple<string name,uint32 id>
    /// @return Returns the name of the type
    const std::string& getName() const { return name_; }

    /// Get the meta type
    /// @return the meta type
    const MetaType& getMetaType() const { return type_; }

    /// Are the two types the same?
    virtual bool equals(RootTyp const& other) const = 0;

    // is this an optional type?
    virtual bool isOptional() const { return false; }

    // returns true if this is an optional type that wraps the given type
    virtual bool isOptionalOfSameType(RootTyp const& otherType) const { return false; }

    /// Hash code for this type.
    virtual size_t hash() const = 0;

    // Returns true if the type is one of the integral types, otherwise false
    virtual bool isIntegralType() const { return false; }

    // Returns true if the type is one of the integral types, otherwise false
    virtual bool isNumericType() const { return false; }

    // Returns true if the metatype is UNKNOWN
    bool isUnknown() const { return type_ == MetaType::UNKNOWN; }

    /// Wrapped equal() member function in a function object for hash tables.
    struct Equal
    {
        bool operator()(RootTyp const* const x, RootTyp const* const y) const
        {
            assert(NULL != x && NULL != y);
            return x->equals(*y);
        }
    };

    /// Wrapped hash() member function in a function object for hash tables.
    struct Hash
    {
        size_t operator()(RootTyp const* const x) const
        {
            assert(NULL != x);
            return x->hash();
        }
    };

  private:
    static RootTyp const& constructType(Meta::BaseType const& btype);

  protected:
    std::string name_;
    MetaType type_;
};
inline bool operator==(RootTyp const& x, RootTyp const& y)
{
    return x.equals(y);
}
inline bool operator!=(RootTyp const& lhs, RootTyp const& rhs)
{
    return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream&, RootTyp const*);
std::ostream& operator<<(std::ostream&, RootTyp const&);

#define TYP_IS_AS_SPECIALIZATIONS_DC(asType)                                                       \
    template<>                                                                                     \
    inline bool RootTyp::is<asType>() const                                                        \
    {                                                                                              \
        return NULL != dynamic_cast<asType const*>(this);                                          \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& RootTyp::as<asType>()                                                           \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType*>(this));                                               \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline asType const& RootTyp::as<asType>() const                                               \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType const*>(this));                                         \
        return *static_cast<asType const*>(this);                                                  \
    }

#define TYP_IS_AS_SPECIALIZATIONS_MT(isEnum, asType)                                               \
    template<>                                                                                     \
    inline bool RootTyp::is<asType>() const                                                        \
    {                                                                                              \
        return MetaType::isEnum == getMetaType();                                                  \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& RootTyp::as<asType>()                                                           \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType*>(this));                                               \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline asType const& RootTyp::as<asType>() const                                               \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType const*>(this));                                         \
        return *static_cast<asType const*>(this);                                                  \
    }

/// Type class representing an empty curly
class EmptyCurlyTyp : public RootTyp
{
  public:
    EmptyCurlyTyp();
    virtual uint64_t getSizeEstimate() const { return 0; }
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;
};
TYP_IS_AS_SPECIALIZATIONS_MT(EMPTYCURLY, EmptyCurlyTyp)

/// Type class representing a primitive type
class PrimitiveTyp : public RootTyp
{
  public:
    PrimitiveTyp(const std::string& name, const MetaType& type)
      : RootTyp(name, type)
    {}

  private:
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_DC(PrimitiveTyp)

/// Type class representing a simple primitive type, where a simple type is one that is fully
/// described by its meta-type enumeration
class SimplePrimitiveTyp : public PrimitiveTyp
{
  public:
    SimplePrimitiveTyp(MetaType const&);
    virtual uint64_t getSizeEstimate() const;
    virtual uint64_t getFixedSize() const;
    virtual uint8_t getAlignment() const;
    virtual bool equals(RootTyp const& other) const { return getMetaType() == other.getMetaType(); }
    virtual size_t hash() const { return 4099 + 3 * getMetaType().getValue(); }
    virtual bool isIntegralType() const;
    virtual bool isNumericType() const;

  private:
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_DC(SimplePrimitiveTyp)

/// Type class representing an enum type
class EnumTyp : public PrimitiveTyp
{
  public:
    typedef std::vector<std::string> Values;
    EnumTyp(Values const& values);
    Values const& getValidValues() const { return _values; }
    uint32_t getNumberOfValues() const { return _values.size(); }
    static std::string makeName(Values const& values);
    virtual uint64_t getSizeEstimate() const;
    virtual uint64_t getFixedSize() const { return sizeof(int); }
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    Values _values;
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(ENUM, EnumTyp)

/// Class representing an XML type
class XMLTyp : public PrimitiveTyp
{
  public:
    XMLTyp(const std::string& sch, uint32_t toolkitIndex_);

    static std::string makeName(const std::string& schema, uint32_t toolkitIndex);
    const std::string& schema() const { return _schema; }
    // Index of the toolkit where type is defined and where we expect to find the schema
    uint32_t toolkitIndex() const { return _toolkitIndex; }
    std::string compileTimeSchema() const;
    std::string runTimeSchema() const;
    virtual uint64_t getSizeEstimate() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    std::string _schema;
    uint32_t _toolkitIndex;
};
TYP_IS_AS_SPECIALIZATIONS_MT(XML, XMLTyp)

class FunctionHeadSym;

//! Formal type parameter for a generic native function. For example, in
//'<any T>[N] void foo(set<T>[N] s)', T is a type formal.
class TypeFormal : public RootTyp
{
  public:
    TypeFormal(FunctionHeadSym const* function,
               std::string const& constraint,
               std::string const& identifier);
    static std::string makeName(const FunctionHeadSym*, const std::string&);
    const FunctionHeadSym* getFunction() const { return function_; }
    const std::string& getConstraint() const { return constraint_; }
    const std::string& getIdentifier() const { return identifier_; }
    virtual uint64_t getSizeEstimate() const { return 0; }
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    const FunctionHeadSym* function_;
    const std::string constraint_;
    const std::string identifier_;
};
TYP_IS_AS_SPECIALIZATIONS_MT(TYPEFORMAL, TypeFormal)

//! Formal bounds parameter for a generic native function. For example, in
//'<any T>[N] void foo(set<T>[N] s)', N is a bounds formal.
class BoundsFormal
{
  public:
    BoundsFormal(const FunctionHeadSym* f, const std::string& i)
      : function_(f)
      , identifier_(i)
    {}
    const FunctionHeadSym* getFunction() const { return function_; }
    const std::string& getIdentifier() const { return identifier_; }
    bool equals(BoundsFormal const& other) const;
    size_t hash() const;
    struct Equal
    {
        bool operator()(BoundsFormal const* const x, BoundsFormal const* const y) const
        {
            assert(NULL != x && NULL != y);
            return x->equals(*y);
        }
    };
    struct Hash
    {
        size_t operator()(BoundsFormal const* const x) const
        {
            assert(NULL != x);
            return x->hash();
        }
    };

  private:
    const FunctionHeadSym* function_;
    const std::string identifier_;
};
std::ostream& operator<<(std::ostream&, BoundsFormal const*);

/// \brief Bound to be used in meta types for bounded-size string, list,
/// set, or map type.
///
/// For example, in string[32], the bound is a concrete value 32.
class TypeBound
{
  public:
    TypeBound(const int32_t b)
      : var_(NULL)
      , val_(b)
    {}
    TypeBound(const BoundsFormal* b)
      : var_(b)
      , val_(-1)
    {}
    int32_t toInt() const { return val_; }
    const BoundsFormal& toVar() const
    {
        assert(NULL != var_);
        return *var_;
    }
    bool isVariable() const { return NULL != var_; }
    bool equals(TypeBound const& other) const;
    size_t hash() const;
    struct Equal
    {
        bool operator()(TypeBound const* const x, TypeBound const* const y) const
        {
            assert(NULL != x && NULL != y);
            return x->equals(*y);
        }
    };
    struct Hash
    {
        size_t operator()(TypeBound const* const x) const
        {
            assert(NULL != x);
            return x->hash();
        }
    };

  private:
    const BoundsFormal* var_;
    const int32_t val_;
};
inline bool operator==(const TypeBound& x, const TypeBound& y)
{
    return x.equals(y);
}
inline bool operator!=(const TypeBound& x, const TypeBound& y)
{
    return !(x == y);
}
std::ostream& operator<<(std::ostream&, const TypeBound&);

/// Type class representing a bounded string type
class BStringTyp : public PrimitiveTyp
{
  public:
    BStringTyp(const TypeBound& bound);
    int32_t getBoundSize() const { return bound_.toInt(); }
    static std::string makeName(const TypeBound& bound);
    virtual uint64_t getSizeEstimate() const;
    virtual uint64_t getFixedSize() const
    {
        return TMP::getSizeTypeSize(bound_.toInt()) + bound_.toInt() + 1;
    }
    const TypeBound& getBound() const { return bound_; }
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    const TypeBound bound_;
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(BSTRING, BStringTyp)

/// Type class representing a composite type
class CompositeTyp : public RootTyp
{
  public:
    CompositeTyp(std::string const& name, MetaType const& type)
      : RootTyp(name, type)
    {}

  private:
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_DC(CompositeTyp)

/// Type class representing a collection type
class CollectionTyp : public CompositeTyp
{
  public:
    CollectionTyp(const std::string& name, const MetaType& type)
      : CompositeTyp(name, type)
    {}

  private:
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_DC(CollectionTyp)

/// Type class representing a list type
class ListTyp : public CollectionTyp
{
  public:
    ListTyp(const RootTyp& etype);
    RootTyp const& getElementType() const { return etype_; }
    static std::string makeName(const RootTyp& etype);
    virtual uint64_t getSizeEstimate() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    const RootTyp& etype_;
    friend class RootTyp;
    friend class BListTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(LIST, ListTyp)

/// Type class representing a set type
class SetTyp : public CollectionTyp
{
  public:
    SetTyp(const RootTyp& etype);
    RootTyp const& getElementType() const { return etype_; }
    static std::string makeName(const RootTyp& etype);
    virtual uint64_t getSizeEstimate() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    const RootTyp& etype_;
    friend class RootTyp;
    friend class BSetTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(SET, SetTyp)

/// Type class representing a map type
class MapTyp : public CollectionTyp
{
  public:
    MapTyp(const RootTyp& ktype, const RootTyp& vtype);
    std::pair<const RootTyp*, const RootTyp*> getElementType() const
    {
        return std::make_pair(&ktype_, &vtype_);
    }
    const RootTyp& getKeyType() const { return ktype_; }
    const RootTyp& getValueType() const { return vtype_; }
    static std::string makeName(const RootTyp& k, const RootTyp& v);
    virtual uint64_t getSizeEstimate() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    const RootTyp& ktype_;
    const RootTyp& vtype_;
    friend class RootTyp;
    friend class BMapTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(MAP, MapTyp)

/// Type class representing a bounded collection type
class BCollectionType : public CollectionTyp
{
  public:
    BCollectionType(const std::string& name, const MetaType& type, const TypeBound& bound)
      : CollectionTyp(name, type)
      , bound_(bound)
    {}
    int32_t getBoundSize() const { return bound_.toInt(); }
    const TypeBound& getBound() const { return bound_; }

  protected:
    const TypeBound bound_;

  private:
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_DC(BCollectionType)

/// Type class representing a bounded list type
class BListTyp : public BCollectionType
{
  public:
    BListTyp(const RootTyp& etype, const TypeBound& bound);
    const RootTyp& getElementType() const { return etype_; }
    static std::string makeName(const RootTyp&, const TypeBound&);
    virtual uint64_t getSizeEstimate() const;
    virtual uint64_t getFixedSize() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    RootTyp const& etype_;
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(BLIST, BListTyp)

/// Type class representing a bounded set type
class BSetTyp : public BCollectionType
{
  public:
    BSetTyp(const RootTyp& etype, const TypeBound& bound);
    const RootTyp& getElementType() const { return etype_; }
    static std::string makeName(const RootTyp&, const TypeBound&);
    virtual uint64_t getSizeEstimate() const;
    virtual uint64_t getFixedSize() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    const RootTyp& etype_;
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(BSET, BSetTyp)

/// Type class representing a bounded map type
class BMapTyp : public BCollectionType
{
  public:
    BMapTyp(const RootTyp& ktype, const RootTyp& vtype, const TypeBound& bound);
    std::pair<const RootTyp*, const RootTyp*> getElementType() const
    {
        return std::make_pair(&ktype_, &vtype_);
    }
    const RootTyp& getKeyType() const { return ktype_; }
    const RootTyp& getValueType() const { return vtype_; }
    static std::string makeName(const RootTyp&, const RootTyp&, const TypeBound&);
    virtual uint64_t getSizeEstimate() const;
    virtual uint64_t getFixedSize() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    const RootTyp& ktype_;
    const RootTyp& vtype_;
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(BMAP, BMapTyp)

/// Type class representing a tuple type
class TupleTyp : public CompositeTyp
{
  public:
    typedef std::tr1::unordered_map<std::string, uint32_t> Name2Index;
    typedef std::vector<std::pair<std::string, RootTyp const*> > Attributes;
    TupleTyp(Name2Index& name2index, Attributes& attributes);
    uint32_t getNumberOfAttributes() const { return _attributes.size(); }
    const std::string& getAttributeName(uint32_t index) const
    {
        return _attributes.at(index).first;
    }
    const Name2Index& getAttributeNames() const { return _name2index; }
    const RootTyp& getAttributeType(std::string const& name) const
    {
        return getAttributeType(_name2index.find(name)->second);
    }
    const RootTyp& getAttributeType(uint32_t index) const
    {
        return *(_attributes.at(index).second);
    }
    static std::string makeName(Attributes&);
    virtual uint64_t getSizeEstimate() const;
    virtual uint64_t getFixedSize() const;
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;

  private:
    Name2Index _name2index;
    Attributes _attributes;
    friend class RootTyp;
};
TYP_IS_AS_SPECIALIZATIONS_MT(TUPLE, TupleTyp)

class OptionalTyp : public RootTyp
{
  public:
    OptionalTyp(const RootTyp& underlyingType);
    virtual uint64_t getFixedSize() const { return underlyingType_.getFixedSize(); }
    virtual uint64_t getSizeEstimate() const { return underlyingType_.getSizeEstimate(); }
    virtual uint8_t getAlignment() const { return underlyingType_.getAlignment(); }
    virtual bool isOptional() const { return true; }
    virtual bool isOptionalOfSameType(RootTyp const& otherType) const
    {
        return getUnderlyingType().equals(otherType);
    }
    virtual bool equals(RootTyp const& other) const;
    virtual size_t hash() const;
    static std::string makeName(const RootTyp& underlyingType);
    const RootTyp& getUnderlyingType() const { return underlyingType_; }

  private:
    const RootTyp& underlyingType_;
};
TYP_IS_AS_SPECIALIZATIONS_MT(OPTIONAL, OptionalTyp)
};
POP_DLL_PUBLIC

#endif /* SPL_FRONT_END_TYP_H */
