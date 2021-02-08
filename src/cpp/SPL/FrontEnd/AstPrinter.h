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

#ifndef SPL_TREE_PRINTER_H
#define SPL_TREE_PRINTER_H

#include <SPL/FrontEnd/AstVisitor.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <ostream>

namespace SPL {

/// This class will print all or part of an ast tree to a stream
/// This class will replace the printer currently implemented as
/// an antlr tree grammar
class AstPrinter : public AstVisitor
{
  public:
    static void print(PrinterContext& printerContext, AstNode& ast);

  protected:
    virtual void visitCompilationUnit(AstNode& ast);
    virtual void visitNamespace(AstNode& ast);
    virtual void visitNamespaceName(AstNode& ast);
    virtual void visitUseDirectives(AstNode& ast);
    virtual void visitUseDirective(AstNode& ast);
    virtual void visitUseDirectiveTail(AstNode& ast);
    virtual void visitCompositeDef(AstNode& ast);
    virtual void visitCompositeHead(AstNode& ast);
    virtual void visitCompositeIn(AstNode& ast);
    virtual void visitCompositeOut(AstNode& ast);
    virtual void visitCompositePort(AstNode& ast);
    virtual void visitCompositeBody(AstNode& ast);

    virtual void visitStreamType(AstNode& ast);
    virtual void visitModifiers(AstNode& ast);
    virtual void visitAsAlias(AstNode& ast);
    virtual void visitConfigItem(AstNode& ast);

    virtual void visitOpInvoke(AstNode& ast);
    virtual void visitOpInvokeHead(AstNode& ast);
    virtual void visitOpInvokeBody(AstNode& ast);
    virtual void visitOpOutputs(AstNode& ast);
    virtual void visitOpOutput(AstNode& ast);
    virtual void visitOpInputs(AstNode& ast);
    virtual void visitPortInputs(AstNode& ast);
    virtual void visitConfigClause(AstNode& ast) { visitClause(ast, "config"); }
    virtual void visitGraphClause(AstNode& ast) { visitClause(ast, "graph"); }
    virtual void visitActualClause(AstNode& ast) { visitClause(ast, "param"); }
    virtual void visitLogicClause(AstNode& ast) { visitClause(ast, "logic"); }
    virtual void visitWindowClause(AstNode& ast) { visitClause(ast, "window"); }
    virtual void visitOutputClause(AstNode& ast) { visitClause(ast, "output"); }
    virtual void visitFormalClause(AstNode& ast) { visitClause(ast, "param"); }
    virtual void visitTypeClause(AstNode& ast) { visitClause(ast, "type"); }
    virtual void visitVarClause(AstNode& ast) { visitClause(ast, "var"); }
    virtual void visitOpInvokeActual(AstNode& ast);
    virtual void visitOpActualType(AstNode& ast);
    virtual void visitOpActualName(AstNode& ast);
    virtual void visitOpActualExprs(AstNode& ast);
    virtual void visitLogicState(AstNode& ast);
    virtual void visitOnProcessLogic(AstNode& ast);
    virtual void visitOnTupleLogic(AstNode& ast);
    virtual void visitOnPunctLogic(AstNode& ast);
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    virtual void visitOnWatermarkLogic(AstNode & ast);
#endif
    virtual void visitOpInvokeWindow(AstNode& ast);
    virtual void visitOpInvokeOutput(AstNode& ast);
    virtual void visitReturnStmt(AstNode& ast);

    virtual void visitDefType(AstNode& ast);

    virtual void visitCompositeFormal(AstNode& ast);
    virtual void visitCompositeFormalInit(AstNode& ast);
    virtual void visitExpressionMode(AstNode& ast);
    virtual void visitTypeArgs(AstNode& ast);

    virtual void visitTupleAttrib(AstNode& ast);
    virtual void visitTupleExtend(AstNode& ast);
    virtual void visitTypeName(AstNode& ast);
    virtual void visitAttributeExpr(AstNode& ast);
    virtual void visitAttributeDecl(AstNode& ast);

    virtual void visitNoneT(AstNode& ast) {}

    virtual void visitTupleType(AstNode& ast);
    virtual void visitEnumType(AstNode& ast);
    virtual void visitListType(AstNode& ast);
    virtual void visitMapType(AstNode& ast);
    virtual void visitSetType(AstNode& ast);
    virtual void visitXMLType(AstNode& ast);
    virtual void visitVoidType(AstNode& ast);
    virtual void visitStringLiteral(AstNode& ast);
    virtual void visitXMLLiteral(AstNode& ast);
    virtual void visitListLiteral(AstNode& ast);
    virtual void visitSetLiteral(AstNode& ast);
    virtual void visitMapLiteral(AstNode& ast);
    virtual void visitNumericLiteral(AstNode& ast);
    virtual void visitCurlyLiteral(AstNode& ast);
    virtual void visitTupleLiteral(AstNode& ast);
    virtual void visitAttributeAssign(AstNode& ast);
    virtual void visitOptionalType(AstNode& ast);

    virtual void visitMapping(AstNode& ast);

    virtual void visitBooleanLiteral(AstNode& ast);
    virtual void visitPrimitiveType(AstNode& ast);
    virtual void visitPrefixExpr(AstNode& ast);
    virtual void visitInfixExpr(AstNode& ast);
    virtual void visitPostfixExpr(AstNode& ast);
    virtual void visitConditionalExpr(AstNode& ast);
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);
    virtual void visitIsPresentExpr(AstNode& ast);

    virtual void visitTypeDims(AstNode& ast);
    virtual void visitId(AstNode& ast);

    virtual void visitRStringType(AstNode& ast);
    virtual void visitUStringType(AstNode& ast);

    virtual void visitCastExpr(AstNode& ast);
    virtual void visitCallExpr(AstNode& ast);

    virtual void visitFunctionDef(AstNode& ast);
    virtual void visitFunctionHead(AstNode& ast);
    virtual void visitBlockStmtNoScope(AstNode& ast);
    virtual void visitBlockStmt(AstNode& ast);
    virtual void visitIfStmt(AstNode& ast);
    virtual void visitElseClause(AstNode& ast);
    virtual void visitForStmt(AstNode& ast);
    virtual void visitForInit(AstNode& ast);
    virtual void visitWhileStmt(AstNode& ast);
    virtual void visitContinueStmt(AstNode& ast);
    virtual void visitBreakStmt(AstNode& ast);

    virtual void visitExprStmt(AstNode& ast);
    virtual void visitSubscriptExpr(AstNode& ast);
    virtual void visitParenthesizedExpr(AstNode& ast);
    virtual void visitQualifierExpr(AstNode& ast);
    virtual void visitSlice(AstNode& ast);

    virtual void visitLocalDecl(AstNode& ast);
    virtual void visitLocalDeclItem(AstNode& ast);

    virtual void visitFunctionFormal(AstNode& ast);

    virtual void visitUnaryMinusT(AstNode& ast);
    virtual void visitTypeT(AstNode& ast);
    virtual void visitTupleT(AstNode& ast);
    virtual void visitTildeT(AstNode& ast);
    virtual void visitStaticT(AstNode& ast);
    virtual void visitSetT(AstNode& ast);
    virtual void visitPlusPlusT(AstNode& ast);
    virtual void visitQMarkT(AstNode& ast);
    // virtual void visitQMarkQMarkT(AstNode & ast);
    virtual void visitOperatorT(AstNode& ast);
    virtual void visitMinusMinusT(AstNode& ast);
    virtual void visitMapT(AstNode& ast);
    virtual void visitListT(AstNode& ast);
    virtual void visitFunctionT(AstNode& ast);
    virtual void visitExpressionT(AstNode& ast);
    virtual void visitEnumT(AstNode& ast);
    virtual void visitBangT(AstNode& ast);
    virtual void visitAttributeT(AstNode& ast);
    virtual void visitMutableT(AstNode& ast);
    virtual void visitPublicT(AstNode& ast);
    virtual void visitStatefulT(AstNode& ast);
    virtual void visitCompositeT(AstNode& ast);
    virtual void visitFunctionPrototype(AstNode& ast);
    virtual void visitTypeFormal(AstNode& ast);
    virtual void visitTypeFormals(AstNode& ast);
    virtual void visitBoundsFormals(AstNode& ast);
    virtual void visitProtoFormal(AstNode& ast);
    virtual void visitProtoFormals(AstNode& ast);

    virtual void visitAnnotation(AstNode& ast);
    //    virtual void visitAnnotationTail(AstNode & ast);
    //    virtual void visitKeyValuePair(AstNode & ast);

  private:
    AstPrinter(PrinterContext& printerContext);

    void visitListPc(AstNode& ast, int startChildIndex);
    void visitIdPc(AstNode& ast, int startChildIndex, int count);
    void visitEqExprQ(AstNode& ast);
    void visitDefinition(AstNode& ast);
    void visitOpName(AstNode& ast);
    void visitNamespaceNameBody(AstNode& ast);
    void visitNestedStatement(AstNode& ast);
    void visitStandaloneTypeDef(AstNode& ast);
    void visitStmtOrType(AstNode& ast);
    void visitTupleArgs(AstNode& ast);
    void visitTypeNameContents(AstNode& ast);
    void visitClause(AstNode& ast, const std::string& name);
    void print(const std::string& s) const { _printerContext.print(s); }
    void indent() const { _printerContext.indent(); }
    void outdent() const { _printerContext.outdent(); }
    void newLine() const { _printerContext.pln(); }

    PrinterContext& _printerContext;
};

} // namespace SPL

#endif // SPL_TREE_PRINTER_H
