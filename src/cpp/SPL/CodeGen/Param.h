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

#ifndef SPL_CODE_GEN_PARAM_H
#define SPL_CODE_GEN_PARAM_H

#include <SPL/Utility/ModelDumper.h>
#include <UTILS/HashMapHelpers.h>
#include <assert.h>
#include <string>
#include <vector>

namespace SPL {
class RootTyp;
class Expression;
class FunctionHeadSym;
class SourceLocation;

//! Parameters for the operator.  These may be types or expressions
class Param
{
  public:
    /// Constructor: id/expression list pair
    /// @param id        identifier
    /// @param loc       Source Location
    /// @param expnList  Expression List
    /// @post            Param owns the memory for the expnList (must be NEW'ed)
    Param(const std::string& id,
          const SourceLocation& loc,
          std::vector<const Expression*>& expnList)
      : _id(id)
      , _loc(loc)
      , _expnList(&expnList)
    {
        MODEL("Param(" << (void*)this << ", " << id << ")\n");
        for (int i = 0, size = expnList.size(); i < size; ++i) {
            MODEL("AddExpression(" << (void*)this << ", " << (void*)expnList[i] << ")\n");
        }
    }

    /// Destructor
    ~Param();

    /// Return the parameter name
    /// @return the parametername
    const std::string& getName() const { return _id; }

    /// Get the source location
    /// @return Returns the source location of the parameter
    const SourceLocation& getLocation() const { return _loc; }

    /// Return the parameter expression
    /// @return the expression
    const std::vector<const Expression*>& getExpressionList() const { return *_expnList; }

    /// Get a single expression; issue msg if not single expression
    /// @param paramName   name of parameter
    /// @return the expression or NULL when invalid
    const Expression* getSingleExpression(const std::string& paramName) const;

    /// Get a single string; issue msg if not single string exp
    /// @param paramName   name of parameter
    /// @return the string ir NULL when invalid
    bool getSingleString(const std::string& paramName, std::string& singleString) const;

    /// Print Stream
    /// @param  s stream to print to
    /// @return s
    std::ostream& print(std::ostream& s) const;

    /// Collect Type and Function Information
    /// @param types   if non-NULL, named types will be added
    /// @param fcns    if non-NULL, referenced functions will be added
    /// @param inFcns  if true, walk into references functions
    void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                 std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                 bool inFcns) const;

  private:
    /// Copy constructor - don't do this
    Param(const Param& rhs);

    std::string _id;
    const SourceLocation& _loc;
    std::vector<const Expression*>* _expnList;
    mutable std::tr1::unordered_set<std::string> _simplifiedStrings;
};
}
#endif // SPL_CODE_GEN_PARAM_H
