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

#ifndef SPL_DEBUGGER_APPLICATION_MODEL_H
#define SPL_DEBUGGER_APPLICATION_MODEL_H

#include <UTILS/HashMapHelpers.h>
#include <string>

namespace SPL {
class OperatorImpl;
class PEImpl;

/*!
 * This class represents a model of the SPL application.
 * The model provides access to application's objects -
 * operators, ports, streams - and their properties - name,
 * schemas, etc.
 *
 * The ApplicationModel is used by the debugger to provide
 * application inforamtion to the user, and to validate the
 * user's input.
 */
class ApplicationModel
{
  public:
    /// Constructo
    /// @param peHandle Processing Element
    ApplicationModel(PEImpl& peHandle);

    /// Return the names of the operators in this application
    /// @param names output set of operator names
    void getOperatorNames(std::tr1::unordered_set<std::string>& names) const;

    /// Return the operators in this application
    /// @param ops map of operator names to Operator ptr
    void getOperators(std::tr1::unordered_map<std::string, OperatorImpl*>& ops) const;

    /// Given an operator name return the operator's properties, or if
    /// the operator with that name is not defined, return NULL.
    /// @param opName Operator being searched for
    /// @return matching Operator, or NULL
    OperatorImpl* getOperator(const std::string& opName) const;

  private:
    PEImpl& _peHandle;
};
}; // namespace SPL

#endif /* SPL_DEBUGGER_APPLICATION_MODEL_H */
