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

#ifndef SPL_GENERICS_UNIFIER_H
#define SPL_GENERICS_UNIFIER_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <string>

namespace SPL {
class RootTyp;
class BoundsFormal;
class CallExpressionSym;
class FunctionHeadSym;
class TypeBound;
class TypeFormal;

/// Unifies two generic entities by matching their free generic types
/// and bounds, and encapsulates the result of the unification, which is
/// either a failure or a substitution for those free types and bounds.
/// This is an adaptation of a standard unification algorithm which is
/// described, for example, in the Russell/Norvig AI textbook.
class DLL_PUBLIC GenericsUnifier
{
  public:
    /// Unify two types.
    static GenericsUnifier& unify(RootTyp const&, RootTyp const&);

    /// Unify a call (which has concrete actual parameter types) against
    /// a function declaration (which potentially has generic type and
    /// bounds parameters).
    static GenericsUnifier& unify(CallExpressionSym const&, FunctionHeadSym const&);

    /// Unify two function declarations.
    static GenericsUnifier& unify(FunctionHeadSym const&, FunctionHeadSym const&);

    /// Destructor.
    virtual ~GenericsUnifier() {}

    /// Find out what a bounds-formal got unified to.
    virtual TypeBound const& get(BoundsFormal const&) const = 0;

    /// Find out what a type-formal got unified to.
    virtual RootTyp const& get(TypeFormal const&) const = 0;

    /// Find out whether a bounds-formal was constrained at all.
    virtual bool has(BoundsFormal const&) const = 0;

    /// Find out whether a type-formal was constrained at all.
    virtual bool has(TypeFormal const&) const = 0;

    /// Find out whether unification failed, because the two entities
    /// were incompatible.
    virtual bool isFailure() const = 0;

    /// Apply the substitution from this unification to a type.
    virtual RootTyp const& substitute(RootTyp const&) const;
};

/// Check whether a type belongs to a certain category. The chapter on
/// types in the language specification shows a tree diagram where the
/// type categories are inner nodes, and concrete types are leaves.
/// @param type concrete type, e.g., int32
/// @param constraint category of types, e.g., numeric
bool satisfiesConstraint(RootTyp const& type,
                         std::string const& constraint,
                         bool useUnderlyingType = true) DLL_PUBLIC;

/// Check whether unification between these two types succeeds, i.e.,
/// results in a unifier for which isFailure() is false.
bool unifies(RootTyp const&, RootTyp const&) DLL_PUBLIC;
};

#endif /*SPL_GENERICS_UNIFIER_H*/
