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

#ifndef EXPANDASSIGNFROM_H
#define EXPANDASSIGNFROM_H

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {

//! Operator Graph Class
class DLL_PUBLIC ExpandAssignFrom : public Singleton<ExpandAssignFrom>
{

  public:
    /// Constructor
    /// Initialize state
    ExpandAssignFrom()
      : Singleton<ExpandAssignFrom>(*this)
      , _traceAssignFrom(false)
    {}

    void expandAssignFromInStatement(Statement*& stmt);

  private:
    Expression& createAttributeExpression(Expression& rootAttrExpr,
                                          const std::string& aname,
                                          const RootTyp& atType);
    void expandAssignFrom(const RootTyp& tType,
                          Expression& taExpr,
                          const RootTyp& sType,
                          Expression& saExpr,
                          BlockStatement*& assignStmt,
                          const CallExpression& assignCallExpr);
    void expandAssignFromInExpressionStatement(ExpressionStatement& exprStmt,
                                               BlockStatement*& assignFrom);
    void expandAssignFrom(Statement*& stmt);

    bool _traceAssignFrom; // trace flag
};

};
#endif /* EXPANDASSIGNFROM_H */
