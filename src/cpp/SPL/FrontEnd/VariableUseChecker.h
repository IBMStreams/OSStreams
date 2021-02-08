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

#ifndef SPL_VARIABLE_USE_CHECKER_H
#define SPL_VARIABLE_USE_CHECKER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

namespace SPL {
class SymbolTableEntry;
class VariableUseCheckerState;

/// Visitor that checks that variables are initialized before their first use.
class VariableUseChecker : public AstVisitorPushPop
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(ParserContext&, AstNode&);

    /// Destructor.
    virtual ~VariableUseChecker();

  private:
    VariableUseCheckerState* _state;

    VariableUseChecker(ParserContext&);

    virtual void enterScope(AstNode& ast);
    virtual void exitScope(AstNode& ast);
    virtual void visitExprDef(AstNode& ast);
    virtual void visitExprUse(AstNode& ast, bool mustBeValue);

  protected:
    /// If this stage isn't yet done for 'ast', mark it as done, and then
    /// call the superclass generic visit() method to actually do this stage.
    virtual void visit(AstNode& ast);

    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitNamespaceName(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOnProcessLogic(AstNode&);
    virtual void visitOnTupleLogic(AstNode&);
    virtual void visitOnPunctLogic(AstNode&);
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        virtual void visitOnWatermarkLogic(AstNode &);
#endif
    virtual void visitOpInvokeOutput(AstNode&);

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);
    virtual void visitFunctionFormal(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDeclItem(AstNode&);
    virtual void visitBlockStmt(AstNode&);
    virtual void visitExprStmt(AstNode&);
    virtual void visitIfStmt(AstNode&);
    virtual void visitForStmt(AstNode&);
    virtual void visitWhileStmt(AstNode&);
    virtual void visitReturnStmt(AstNode&);

    // ----- Expressions can appear in many places in the grammar.
    virtual void visitMapping(AstNode&);
    virtual void visitAttributeExpr(AstNode&);
    virtual void visitCastExpr(AstNode&);
    virtual void visitCallExpr(AstNode&);
    virtual void visitConditionalExpr(AstNode&);
    virtual void visitInfixExpr(AstNode&);
    virtual void visitParenthesizedExpr(AstNode&);
    virtual void visitPostfixExpr(AstNode&);
    virtual void visitPrefixExpr(AstNode&);
    virtual void visitSubscriptExpr(AstNode&);
    virtual void visitSlice(AstNode&);
    virtual void visitIsPresentExpr(AstNode& ast);
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);

    // ----- Literals are the highest-precedence expressions denoting values.
    virtual void visitListLiteral(AstNode&);
    virtual void visitTupleLiteral(AstNode&);
    virtual void visitAttributeAssign(AstNode&);
    virtual void visitSetLiteral(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitTypeDims(AstNode&);
};
};

#endif /* SPL_VARIABLE_USE_CHECKER_H */
