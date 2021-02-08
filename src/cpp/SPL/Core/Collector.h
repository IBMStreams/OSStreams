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

#ifndef SPL_COLLECTOR_H
#define SPL_COLLECTOR_H

#include <UTILS/HashMapHelpers.h>
#include <Utility/Debug.h>

namespace SPL {
class Expression;
class Statement;
class RootTyp;
class FunctionHeadSym;

//! Helper class to collect type and function information for
//! generating include and deependency information for C++ and Makefile
//! generation
class Collector
{
  public:
    /// Collect named types from a RootTyp
    /// @param result set of RootTyp* of named types referenced.  The set
    /// will be appended to
    /// @param type Type to be examined
    static void collectType(std::tr1::unordered_set<const RootTyp*>& result, const RootTyp& type);

    /// Collect named types from a FunctionHeadSym
    /// @param result set of RootTyp* of named types referenced.  The set
    /// will be appended to
    /// @param fcn FunctionHeadSym to be walked
    static void collectTypes(std::tr1::unordered_set<const RootTyp*>& result,
                             const FunctionHeadSym& fcn);

    /// Collect named types from an Expression
    /// @param types if NON-null, will have named types appended
    /// @param fcns if NON-null, will have referenced functions appended
    /// @param expn Expression to be walked
    /// @param inFcns  if true, walk into references functions
    static void collectTypesAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                         std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                         const Expression& expn,
                                         bool inFcns);

    /// Collect named types from a Statement
    /// @param types if NON-null, will have named types appended
    /// @param fcns if NON-null, will have referenced functions appended
    /// @param stmt Statement to be walked
    /// @param inFcns  if true, walk into references functions
    static void collectTypesAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                         std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                         const Statement& stmt,
                                         bool inFcns);
};

//! class to contain types and functions collected by
/// expressionCollectTypesAndFunctions and StatementCollectTypesAndFunctions
class CollectionContainer
{
  public:
    std::tr1::unordered_set<const RootTyp*>* _types;
    std::tr1::unordered_set<const FunctionHeadSym*>* _fcns;
    bool _inFcns;

    CollectionContainer(std::tr1::unordered_set<const RootTyp*>* types,
                        std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                        bool inFcns)
      : _types(types)
      , _fcns(fcns)
      , _inFcns(inFcns)
    {}

    Debug::DebugFlag debugFlag() const { return Debug::TraceCollection; }
};
};

#endif /* SPL_COLLECTOR_H */
