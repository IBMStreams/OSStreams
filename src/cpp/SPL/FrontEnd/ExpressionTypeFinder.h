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

#ifndef SPL_EXPRESSION_TYPE_FINDER_H
#define SPL_EXPRESSION_TYPE_FINDER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

namespace SPL {
/// Visitor that finds types bottom-up for expressions and reports type
/// errors, if any. Decorates intermediate expression AST nodes with
/// ValueExpressionSym where appropriate.
class ExpressionTypeFinder : public AstVisitorPushPop
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(ParserContext&, AstNode&);
    static bool explicitCastAllowed(RootTyp const& fromType, RootTyp const& toType);
    static RootTyp const& elementType(RootTyp const& type);
    static RootTyp const& collectionSubst(RootTyp const& collType, RootTyp const& elemType);
    static bool isElementOf(RootTyp const& elemType, RootTyp const& collType);
    static bool isMappableContainer(RootTyp const& type);

  private:
    std::vector<AstNode*> _loopNestingContext;
    SymbolTable& _symTab;

    // The output port being processed when visiting children of the output clause
    int _outputPort;
    bool _inLogicOnClause;
    bool _inViewAttributeList;
    bool _inViewFilter;

    ExpressionTypeFinder(ParserContext&);
    void ensureResolved(AstNode& ast);
    SourceLocation const& newLoc(AstNode&);
    void setValueExprType(AstNode& ast, RootTyp const& type);

  protected:
    /// If this stage isn't yet done for 'ast', mark it as done, and then
    /// call the superclass generic visit() method to actually do this stage.
    virtual void visit(AstNode& ast);

    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitNamespaceName(AstNode&);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitCompositeBody(AstNode&);
    virtual void visitCompositeFormal(AstNode&);
    virtual void visitOpActualExprs(AstNode&);
    virtual void visitOpActualName(AstNode&);
    virtual void visitConfigItem(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOutputClause(AstNode&);
    virtual void visitOpInvoke(AstNode&);
    virtual void visitOpInvokeHead(AstNode&);
    virtual void visitOpInvokeWindow(AstNode&);
    virtual void visitOpInvokeActual(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);

#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        // logic onTuple/onPunct/onWatermark can appear in a logic clause
#else
    // logic onTuple/onPunct can appear in a logic clause
#endif
    virtual void visitOnProcessLogic(AstNode&);
    virtual void visitOnTupleLogic(AstNode&);
    virtual void visitOnPunctLogic(AstNode&);
#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        virtual void visitOnWatermarkLogic(AstNode &);
#endif

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDecl(AstNode&);
    virtual void visitLocalDeclItem(AstNode&);
    virtual void visitExprStmt(AstNode&);
    virtual void visitIfStmt(AstNode&);
    virtual void visitForStmt(AstNode&);
    virtual void visitWhileStmt(AstNode&);
    virtual void visitBreakStmt(AstNode&);
    virtual void visitContinueStmt(AstNode&);
    virtual void visitReturnStmt(AstNode&);

    // ----- Expressions can appear in many places in the grammar.
    virtual void visitAttributeExpr(AstNode&);
    virtual void visitCastExpr(AstNode&);
    virtual void visitCallExpr(AstNode&);
    virtual void visitConditionalExpr(AstNode&);
    virtual void visitInfixExpr(AstNode&);
    virtual void visitPostfixExpr(AstNode&);
    virtual void visitPrefixExpr(AstNode&);
    virtual void visitSubscriptExpr(AstNode&);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitIsPresentExpr(AstNode& ast);

    // ----- Literals are the highest-precedence expressions denoting values.
    virtual void visitListLiteral(AstNode&);
    virtual void visitTupleLiteral(AstNode&);
    virtual void visitAttributeAssign(AstNode&);
    virtual void visitMapLiteral(AstNode&);
    virtual void visitSetLiteral(AstNode&);
    virtual void visitXMLLiteral(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitTypeName(AstNode&);
    virtual void visitTypeDims(AstNode&);

    // annotations
    virtual void visitAnnotationT(AstNode&);
};
}

#endif /* SPL_EXPRESSION_TYPE_FINDER_H */
