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

#ifndef SPL_USED_OPERATOR_INVOCATIONS_H
#define SPL_USED_OPERATOR_INVOCATIONS_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>
#include <string>

namespace SPL {
class PrimitiveOperatorInstance;
class OperatorGraph;

//! Remember all the operators used in the user program
class UsedOperatorInvocations : public Singleton<UsedOperatorInvocations>
{
  public:
    /// Constructor
    UsedOperatorInvocations();

    /// Destructor
    /// Delete all operators
    ~UsedOperatorInvocations();

    /// Add an operator.  Take over responsibility for freeing the memory.
    /// @param op    Operator
    void add(PrimitiveOperatorInstance& op);

    typedef std::vector<PrimitiveOperatorInstance*> Operators;

    /// Return all the operators
    /// @return set of Operators
    const Operators& getOperators() const { return _map; }

    /// Return the operator matching a name
    /// @param name     Operator being searched for
    /// @return pointer to PrimitiveOperatorInstance or NULL if not found
    const PrimitiveOperatorInstance* find(const std::string& name) const;

  private:
    // table to hold all the operators
    Operators _map;
    std::tr1::unordered_map<std::string, PrimitiveOperatorInstance*> _lookup;
};
};

#endif /* SPL_USED_OPERATOR_INVOCATIONS_H */
