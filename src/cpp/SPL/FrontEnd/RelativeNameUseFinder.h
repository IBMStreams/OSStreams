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

#ifndef SPL_RELATIVE_NAME_USE_FINDER_H
#define SPL_RELATIVE_NAME_USE_FINDER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

namespace SPL {
/// Visitor that resolves names that have to be resolved after
/// ScopeExpander but before ExpressionTypeFinder, and also initializes
/// function and prototype types.
class RelativeNameUseFinder : public AstVisitorPushPop
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(ParserContext&, AstNode&);

  private:
    bool _pushTupleLiteralSym;

    RelativeNameUseFinder(ParserContext&);

    virtual void ensureResolved(AstNode& ast);
    virtual void visitExprUse(AstNode& ast);

  protected:
    /// If this stage isn't yet done for 'ast', mark it as done, and then
    /// call the superclass generic visit() method to actually do this stage.
    virtual void visit(AstNode& ast);

    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitNamespaceName(AstNode&);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitOpActualExprs(AstNode&);
    virtual void visitOpActualName(AstNode&);
    virtual void visitConfigItem(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOpInvokeWindow(AstNode&);
    virtual void visitOpInvokeActual(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);

    // ----- Function Prototype declarations come from XML files, not regular SPL files.
    virtual void visitFunctionPrototype(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDeclItem(AstNode&);
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
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);
    virtual void visitIsPresentExpr(AstNode& ast);

    // ----- Literals are the highest-precedence expressions denoting values.
    virtual void visitListLiteral(AstNode&);
    virtual void visitAttributeAssign(AstNode&);
    virtual void visitSetLiteral(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitListType(AstNode&);
    virtual void visitMapType(AstNode&);
    virtual void visitRStringType(AstNode&);
    virtual void visitSetType(AstNode&);
    virtual void visitTupleLiteral(AstNode&);
    virtual void visitTupleAttrib(AstNode&);
    virtual void visitTupleExtend(AstNode&);
    virtual void visitTypeDims(AstNode&);

    // annotations
    virtual void visitAnnotationT(AstNode&);
};
}

#endif /* SPL_RELATIVE_NAME_USE_FINDER_H */
