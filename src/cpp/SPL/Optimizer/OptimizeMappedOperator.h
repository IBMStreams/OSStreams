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

#ifndef OPIMIZEMAPPEDOPERATOR_H
#define OPIMIZEMAPPEDOPERATOR_H

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {

//! Operator Graph Class
class DLL_PUBLIC OptimizeMappedOperator : public Singleton<OptimizeMappedOperator>
{

  public:
    /// Constructor
    /// Initialize state
    OptimizeMappedOperator()
      : Singleton<OptimizeMappedOperator>(*this)
      , _traceMappedOp(false)
      , _currentLoc(NULL)
      , _tgtId(NULL)
      , _tgtSymExpr(NULL)
      , _newSplit(false)
    {}

    void optimizeMappedOperatorInStatement(Statement*& stmt);

  private:
    bool mappedOperatorBasicPattern(ExpressionStatement& exprStmt, BinaryExpression*& mappedOp);
    Expression* optimizeMappedOperatorInExpressionStatement(ExpressionStatement& exprStmt);
    void optimizeMappedOperator(Statement& stmt);
    bool isLoad(const Expression& opd, const std::string& loadId);
    void splitOperand(Expression& opd, BlockStatement*& splittedStmt);
    bool symbolInExpression(const Expression& opd, const std::string& id);
    void mappedOperatorBlockStatementFromExpressionStatement(ExpressionStatement& exprStmt,
                                                             BlockStatement*& splittedStmt);
    void splitMappedOperator(Statement*& stmt);

    bool _traceMappedOp; // trace flag

    const SourceLocation* _currentLoc; // location of current stmt

    const std::string* _tgtId; // target id of current assign stmt

    const Expression* _tgtSymExpr; // Symbol expression potr for LHS of current assign stmt

    bool _newSplit; // an assign statement with mapped operator was split
};

};
#endif /* OPIMIZEMAPPEDOPERATOR_H */
