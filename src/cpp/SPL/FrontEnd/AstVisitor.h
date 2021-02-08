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

#ifndef SPL_AST_VISITOR_H
#define SPL_AST_VISITOR_H

#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Utility/Debug.h>

namespace SPL {
/// Visitor that traverses an SPL AST. For each node, it calls visit(),
/// which dispatches to a more specific visit<NonTerminalName>() method,
/// which in this class only calls allChildrenVisit(), but is overridden
/// in subclasses with analysis-specific behavior.
class AstVisitor
{
  protected:
    /// Call "visit" for all children of the AstNode.
    virtual void allChildrenVisit(AstNode&);

    /// Generic visit method, dispatches to the approriate specialized
    /// visit method based on the type of the AST node.
    virtual void visit(AstNode&);

  public:
    /// Destructor.
    virtual ~AstVisitor() {}

  protected:
    AstVisitor(Debug::DebugFlag df)
      : _debugFlag(df)
      , _indent(0)
    {}

    // Get the Debug flag for the walker
    Debug::DebugFlag debugFlag() const { return _debugFlag; }

    void indent() { _indent += 2; }
    void outdent() { _indent -= 2; }

    // ----- Stand-alone tokens may appear in the AST with 0 children.
    virtual void visitAttributeT(AstNode&);
    virtual void visitBangT(AstNode&);
    virtual void visitOptionalT(AstNode&);
    virtual void visitCompositeT(AstNode&);
    virtual void visitEnumT(AstNode&);
    virtual void visitExpressionT(AstNode&);
    virtual void visitFunctionT(AstNode&);
    virtual void visitListT(AstNode&);
    virtual void visitMapT(AstNode&);
    virtual void visitMinusMinusT(AstNode&);
    virtual void visitMutableT(AstNode&);
    virtual void visitNoneT(AstNode&);
    virtual void visitOperatorT(AstNode&);
    virtual void visitPlusPlusT(AstNode&);
    virtual void visitQMarkT(AstNode&);
    // virtual void visitQMarkQMarkT(AstNode &);
    virtual void visitPublicT(AstNode&);
    virtual void visitSetT(AstNode&);
    virtual void visitStatefulT(AstNode&);
    virtual void visitStaticT(AstNode&);
    virtual void visitTildeT(AstNode&);
    virtual void visitTupleT(AstNode&);
    virtual void visitTypeT(AstNode&);
    virtual void visitUnaryMinusT(AstNode&);

    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitCompilationUnit(AstNode&);
    virtual void visitNamespace(AstNode&);
    virtual void visitUseDirectives(AstNode&);
    virtual void visitUseDirective(AstNode&);
    virtual void visitNamespaceName(AstNode&);
    virtual void visitUseDirectiveTail(AstNode&);
    virtual void visitUseWildcard(AstNode&);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitCompositeDef(AstNode&);
    virtual void visitCompositeHead(AstNode&);
    virtual void visitSplDoc(AstNode&);
    virtual void visitMlComment(AstNode&);
    virtual void visitCompositeIn(AstNode&);
    virtual void visitCompositeOut(AstNode&);
    virtual void visitCompositePort(AstNode&);
    virtual void visitCompositeBody(AstNode&);
    virtual void visitFormalClause(AstNode&);
    virtual void visitTypeClause(AstNode&);
    virtual void visitVarClause(AstNode&);
    virtual void visitGraphClause(AstNode&);
    virtual void visitConfigClause(AstNode&);
    virtual void visitCompositeFormal(AstNode&);
    virtual void visitCompositeFormalInit(AstNode&);
    virtual void visitOpActualExprs(AstNode&);
    virtual void visitOpActualName(AstNode&);
    virtual void visitOpActualType(AstNode&);
    virtual void visitConfigItem(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOpInvoke(AstNode&);
    virtual void visitOpInvokeHead(AstNode&);
    virtual void visitOpOutputs(AstNode&);
    virtual void visitOpOutput(AstNode&);
    virtual void visitStreamType(AstNode&);
    virtual void visitAsAlias(AstNode&);
    virtual void visitOpInputs(AstNode&);
    virtual void visitPortInputs(AstNode&);
    virtual void visitOpInvokeBody(AstNode&);
    virtual void visitLogicClause(AstNode&);
    virtual void visitWindowClause(AstNode&);
    virtual void visitActualClause(AstNode&);
    virtual void visitOutputClause(AstNode&);
    virtual void visitLogicState(AstNode&);
    virtual void visitOnTupleLogic(AstNode&);
    virtual void visitOnPunctLogic(AstNode&);
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        virtual void visitOnWatermarkLogic(AstNode &);
#endif
    virtual void visitOnProcessLogic(AstNode&);
    virtual void visitOpInvokeWindow(AstNode&);
    virtual void visitOpInvokeActual(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);
    virtual void visitFunctionHead(AstNode&);
    virtual void visitFunctionFormal(AstNode&);

    // ----- Function Prototype declarations come from XML files, not regular SPL files.
    virtual void visitFunctionPrototype(AstNode&);
    virtual void visitTypeFormals(AstNode&);
    virtual void visitTypeFormal(AstNode&);
    virtual void visitBoundsFormals(AstNode&);
    virtual void visitProtoFormals(AstNode&);
    virtual void visitProtoFormal(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDecl(AstNode&);
    virtual void visitLocalDeclItem(AstNode&);
    virtual void visitBlockStmt(AstNode&);
    virtual void visitBlockStmtNoScope(AstNode&);
    virtual void visitExprStmt(AstNode&);
    virtual void visitIfStmt(AstNode&);
    virtual void visitElseClause(AstNode&);
    virtual void visitForInit(AstNode&);
    virtual void visitForStmt(AstNode&);
    virtual void visitWhileStmt(AstNode&);
    virtual void visitBreakStmt(AstNode&);
    virtual void visitContinueStmt(AstNode&);
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
    virtual void visitQualifierExpr(AstNode&);
    virtual void visitSubscriptExpr(AstNode&);
    virtual void visitSlice(AstNode&);
    virtual void visitId(AstNode&);
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);
    virtual void visitIsPresentExpr(AstNode& ast);

    // ----- Literals are the highest-precedence expressions denoting values.
    virtual void visitListLiteral(AstNode&);
    virtual void visitCurlyLiteral(AstNode&);
    virtual void visitTupleLiteral(AstNode&);
    virtual void visitAttributeAssign(AstNode&);
    virtual void visitMapLiteral(AstNode&);
    virtual void visitSetLiteral(AstNode&);
    virtual void visitBooleanLiteral(AstNode&);
    virtual void visitStringLiteral(AstNode&);
    virtual void visitXMLLiteral(AstNode&);
    virtual void visitNumericLiteral(AstNode&);
    virtual void visitNullLiteral(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitDefType(AstNode&);
    virtual void visitExpressionMode(AstNode&);
    virtual void visitTypeName(AstNode&);
    virtual void visitEnumType(AstNode&);
    virtual void visitXMLType(AstNode&);
    virtual void visitListType(AstNode&);
    virtual void visitMapType(AstNode&);
    virtual void visitPrimitiveType(AstNode&);
    virtual void visitPrimitiveTypeName(AstNode&);
    virtual void visitRStringType(AstNode&);
    virtual void visitSetType(AstNode&);
    virtual void visitTupleType(AstNode&);
    virtual void visitUStringType(AstNode&);
    virtual void visitVoidType(AstNode&);
    virtual void visitTupleAttrib(AstNode&);
    virtual void visitTupleExtend(AstNode&);
    virtual void visitOptionalType(AstNode&);
    virtual void visitAttributeDecl(AstNode&);
    virtual void visitTypeArgs(AstNode&);
    virtual void visitTypeDims(AstNode&);

    // ----- Modifiers prefix function, variable, and type definitions.
    virtual void visitModifiers(AstNode&);

    // ---- Annotations
    virtual void visitAnnotations(AstNode&);
    virtual void visitAnnotation(AstNode&);
    virtual void visitAnnotationT(AstNode&);

  private:
    Debug::DebugFlag _debugFlag;
    int _indent;
};
}

#endif /* SPL_AST_VISITOR_H */
