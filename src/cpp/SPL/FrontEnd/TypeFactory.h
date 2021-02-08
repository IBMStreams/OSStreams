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

#ifndef FRONTENT_TYPE_FACTORY_H
#define FRONTENT_TYPE_FACTORY_H

#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>

#include <boost/shared_ptr.hpp>

namespace SPL {
class FunctionHeadSym;
class Scope;

/// Factory for subclasses of RootTyp, making them easy to create while
/// also ensuring uniqueness, which helps with structural equivalence.
class DLL_PUBLIC TypeFactory : public Singleton<TypeFactory>
{
  private:
    typedef std::tr1::unordered_set<RootTyp*, RootTyp::Hash, RootTyp::Equal> TypeSet;
    TypeSet _typeSet;

  public:
    TypeFactory();

    /// Find or create enumeration with the given constants.
    EnumTyp const& enumT(EnumTyp::Values& names);

    /// Find or create an XML type with the given schema
    /// An empty string implies no schema
    XMLTyp const& xmlT(const std::string& schema, uint32_t toolkitIndex_);
    XMLTyp const& xmlT();

    /// Find or create empty curly type.
    EmptyCurlyTyp const& emptyCurlyT();

    /// Find or create list type with given element type.
    ListTyp const& listT(RootTyp const&);
    /// Find or create list type with given element type and bound.
    BListTyp const& listT(RootTyp const&, TypeBound const&);
    /// Find or create list type with given element type and bound.
    BListTyp const& listT(RootTyp const&, int32_t const);
    /// Find or create list type with given element type and bound.
    BListTyp const& listT(RootTyp const&, BoundsFormal const&);

    /// Find or create map type with given key and value type.
    MapTyp const& mapT(RootTyp const&, RootTyp const&);
    /// Find or create map type with given key and value type and bound.
    BMapTyp const& mapT(RootTyp const&, RootTyp const&, TypeBound const&);
    /// Find or create map type with given key and value type and bound.
    BMapTyp const& mapT(RootTyp const&, RootTyp const&, int32_t const);
    /// Find or create map type with given key and value type and bound.
    BMapTyp const& mapT(RootTyp const&, RootTyp const&, BoundsFormal const&);

    /// Find or create numeric type based on given suffixes of literal.
    SimplePrimitiveTyp const& numberT(char typeSuffix, int bitsSuffix);

    /// Metatype for numeric type based on given suffixes of literal.
    MetaType numberMetaType(char typeSuffix, int bitsSuffix);

    /// Find or create primitive type for given metatype.
    SimplePrimitiveTyp const& primitiveT(MetaType const&);
    /// Find or create primitive type for given type name.
    SimplePrimitiveTyp const& primitiveT(std::string const&);

    /// Find or create rstring type.
    SimplePrimitiveTyp const& rstringT();
    /// Find or create rstring type with given bound..
    BStringTyp const& rstringT(TypeBound const&);
    /// Find or create rstring type with given bound..
    BStringTyp const& rstringT(int32_t const);
    /// Find or create rstring type with given bound..
    BStringTyp const& rstringT(BoundsFormal const&);

    /// Find or create set type with given element type.
    SetTyp const& setT(RootTyp const&);
    /// Find or create set type with given element type and bound.
    BSetTyp const& setT(RootTyp const&, TypeBound const&);
    /// Find or create set type with given element type and bound.
    BSetTyp const& setT(RootTyp const&, int32_t const);
    /// Find or create set type with given element type and bound.
    BSetTyp const& setT(RootTyp const&, BoundsFormal const&);

    /// Find or create tuple type with given attribute names and types.
    TupleTyp const& tupleT(TupleTyp::Attributes& attributes);
    /// Find or create tuple type with given attribute names and types.
    TupleTyp const& tupleT(std::vector<std::string>&, std::vector<RootTyp const*>&);
    /// Find or create tuple type based on scope with attributes.
    TupleTyp const& tupleT(Scope&);

    /// Find or create type-formal for given generic function head, constraint, and identifier.
    TypeFormal const& typeFormalT(FunctionHeadSym const*,
                                  std::string const& constraint,
                                  std::string const& identifier);

    /// Find or create unknown type.
    SimplePrimitiveTyp const& unknownT();

    /// Find or create ustring type.
    SimplePrimitiveTyp const& ustringT();

    /// Find or create void type.
    SimplePrimitiveTyp const& voidT();

    /// Find or create null type.
    SimplePrimitiveTyp const& nullT();

    /// Create the type to represent an option<underlyingType> type
    OptionalTyp const& optionalT(const RootTyp& underlyingType);
};
};

#endif /*FRONTENT_TYPE_FACTORY_H*/
