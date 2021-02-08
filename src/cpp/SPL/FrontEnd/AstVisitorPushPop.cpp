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

#include <SPL/FrontEnd/AstVisitorPushPop.h>

#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>

namespace SPL {
AstVisitorPushPop::AstVisitorPushPop(ParserContext& pCtx, Debug::DebugFlag df)
  : AstVisitor(df)
  , _pCtx(pCtx)
  , _symTab(SPL::SymbolTable::instance())
{}

// ----- The compilation unit is the start symbol of the SPL grammar.
void AstVisitorPushPop::visitCompilationUnit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    AstNode& nsAst = AST::compilationUnit::splNamespace(ast);
    visit(nsAst);
    _symTab.pushOld(nsAst);
    _symTab.pushOld(ast);
    visit(AST::compilationUnit::useDirectives(ast));
    for (int i = 0, n = AST::compilationUnit::definitionCount(ast); i < n; i++) {
        visit(AST::compilationUnit::definition(ast, i));
    }
    _symTab.pop(ast);
    _symTab.pop(nsAst);
}

// ----- Composite operators are defined at the top-level in a namespace.
void AstVisitorPushPop::visitCompositeDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitCompositeDef(ast);
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitCompositeFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    visit(AST::compositeFormal::expressionMode(ast));
    visit(AST::compositeFormal::id(ast));
    _symTab.pushOld(ast);
    visit(AST::compositeFormal::compositeFormalInit(ast));
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitConfigItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    visit(AST::config::id(ast));
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        visit(AST::config::expr(ast, i));
    }
    _symTab.pop(ast);
}

// ----- Streams are defined in a composite operator's graph clause.
void AstVisitorPushPop::visitOpInvoke(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitOpInvoke(ast);
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitOnProcessLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    visit(AST::opInvokeLogicOnProcess::stmt(ast));
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitOnTupleLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    visit(AST::opInvokeLogicOnTuple::id(ast));
    _symTab.pushOld(ast);
    visit(AST::opInvokeLogicOnTuple::stmt(ast));
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitOnPunctLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    visit(AST::opInvokeLogicOnPunct::id(ast));
    _symTab.pushOld(ast);
    visit(AST::opInvokeLogicOnPunct::stmt(ast));
    _symTab.pop(ast);
}

#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    void AstVisitorPushPop::visitOnWatermarkLogic(AstNode & ast)
    {
        visit(AST::opInvokeLogicOnWatermark::id(ast));
        _symTab.pushOld(ast);
        visit(AST::opInvokeLogicOnWatermark::stmt(ast));
        _symTab.pop(ast);
    }
#endif

void AstVisitorPushPop::visitOpInvokeWindow(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    visit(AST::opInvokeWindow::id(ast));
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        visit(AST::opInvokeWindow::expr(ast, i));
    }
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitOpInvokeActual(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    visit(AST::opInvokeActual::id(ast));
    _symTab.pushOld(ast);
    visit(AST::opInvokeActual::opActual(ast));
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    visit(AST::opInvokeOutput::id(ast));
    _symTab.pushOld(ast);
    OpInvokeOutputSym& sym = astSymbol(ast).as<OpInvokeOutputSym>();
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        visit(AST::infixExpr::lhs(exprAst));
        sym.switchRight();
        visit(AST::infixExpr::rhs(exprAst));
        sym.switchLeft();
    }
    _symTab.pop(ast);
}

// ----- Functions are defined at the top-level in a namespace.
void AstVisitorPushPop::visitFunctionDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitFunctionDef(ast);
    _symTab.pop(ast);
}

// ----- Function Prototype declarations come from XML files, not regular SPL files.
void AstVisitorPushPop::visitFunctionPrototype(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitFunctionPrototype(ast);
    _symTab.pop(ast);
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void AstVisitorPushPop::visitBlockStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitBlockStmt(ast);
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitForStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstNode& initAst = AST::forStmt::forInit(ast);
    visit(AST::forInit::modifiers(initAst));
    visit(AST::forInit::type(initAst));
    visit(AST::forInit::id(initAst));
    _symTab.pop(ast);
    visit(AST::forInit::expr(initAst));
    _symTab.pushOld(ast);
    visit(AST::forStmt::stmt(ast));
    _symTab.pop(ast);
}

// ----- Literals are the highest-precedence expressions denoting values.
void AstVisitorPushPop::visitTupleLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitTupleLiteral(ast);
    _symTab.pop(ast);
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void AstVisitorPushPop::visitEnumType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitEnumType(ast);
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitTupleAttrib(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitTupleAttrib(ast);
    _symTab.pop(ast);
}

void AstVisitorPushPop::visitTupleExtend(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    AstVisitor::visitTupleExtend(ast);
    _symTab.pop(ast);
}

#define AST_PUSH_POP_DEFAULT(x)                                                                    \
    void AstVisitorPushPop::x(AstNode& ast) { AstVisitor::x(ast); }

AST_PUSH_POP_DEFAULT(visitContinueStmt)
AST_PUSH_POP_DEFAULT(visitTupleT)
AST_PUSH_POP_DEFAULT(visitOpActualExprs)
AST_PUSH_POP_DEFAULT(visitSetType)
AST_PUSH_POP_DEFAULT(visitCompositeBody)
AST_PUSH_POP_DEFAULT(visitSubscriptExpr)
AST_PUSH_POP_DEFAULT(visitPublicT)
AST_PUSH_POP_DEFAULT(visitUnaryMinusT)
AST_PUSH_POP_DEFAULT(visitMutableT)
AST_PUSH_POP_DEFAULT(visitSplDoc)
AST_PUSH_POP_DEFAULT(visitTypeT)
AST_PUSH_POP_DEFAULT(visitAttributeAssign)
AST_PUSH_POP_DEFAULT(visitPrefixExpr)
AST_PUSH_POP_DEFAULT(visitRStringType)
AST_PUSH_POP_DEFAULT(visitOpInputs)
AST_PUSH_POP_DEFAULT(visitExprStmt)
AST_PUSH_POP_DEFAULT(visitStatefulT)
AST_PUSH_POP_DEFAULT(visitCompositeFormalInit)
AST_PUSH_POP_DEFAULT(visitOptionalType)
AST_PUSH_POP_DEFAULT(visitNoneT)
AST_PUSH_POP_DEFAULT(visitPrimitiveTypeName)
AST_PUSH_POP_DEFAULT(visitQMarkT)
AST_PUSH_POP_DEFAULT(visitPostfixExpr)
AST_PUSH_POP_DEFAULT(visitEnumT)
AST_PUSH_POP_DEFAULT(visitBangT)
AST_PUSH_POP_DEFAULT(visitStaticT)
AST_PUSH_POP_DEFAULT(visitAsAlias)
AST_PUSH_POP_DEFAULT(visitVoidType)
AST_PUSH_POP_DEFAULT(visitUStringType)
AST_PUSH_POP_DEFAULT(visitIsPresentExpr)
AST_PUSH_POP_DEFAULT(visitLogicClause)
AST_PUSH_POP_DEFAULT(visitActualClause)
AST_PUSH_POP_DEFAULT(visitNamespaceName)
AST_PUSH_POP_DEFAULT(visitOptionalT)
AST_PUSH_POP_DEFAULT(visitElseClause)
AST_PUSH_POP_DEFAULT(visitModifiers)
AST_PUSH_POP_DEFAULT(visitCompositeHead)
AST_PUSH_POP_DEFAULT(visitCompositePort)
AST_PUSH_POP_DEFAULT(visitOpInvokeBody)
AST_PUSH_POP_DEFAULT(visitQualifierExpr)
AST_PUSH_POP_DEFAULT(visitCurlyLiteral)
AST_PUSH_POP_DEFAULT(visitId)
AST_PUSH_POP_DEFAULT(visitWindowClause)
AST_PUSH_POP_DEFAULT(visitBooleanLiteral)
AST_PUSH_POP_DEFAULT(visitMapLiteral)
AST_PUSH_POP_DEFAULT(visitOpInvokeHead)
AST_PUSH_POP_DEFAULT(visitAnnotationT)
AST_PUSH_POP_DEFAULT(visitAttributeExpr)
AST_PUSH_POP_DEFAULT(visitTypeName)
AST_PUSH_POP_DEFAULT(visitCompositeT)
AST_PUSH_POP_DEFAULT(visitOpActualName)
AST_PUSH_POP_DEFAULT(visitOpOutput)
AST_PUSH_POP_DEFAULT(visitBlockStmtNoScope)
AST_PUSH_POP_DEFAULT(visitOperatorT)
AST_PUSH_POP_DEFAULT(visitCompositeIn)
AST_PUSH_POP_DEFAULT(visitPrimitiveType)
AST_PUSH_POP_DEFAULT(visitListLiteral)
AST_PUSH_POP_DEFAULT(visitStreamType)
AST_PUSH_POP_DEFAULT(visitUseDirectives)
AST_PUSH_POP_DEFAULT(visitTypeFormal)
AST_PUSH_POP_DEFAULT(visitFormalClause)
AST_PUSH_POP_DEFAULT(visitTupleType)
AST_PUSH_POP_DEFAULT(visitCallExpr)
AST_PUSH_POP_DEFAULT(visitAnnotations)
AST_PUSH_POP_DEFAULT(visitExpressionT)
AST_PUSH_POP_DEFAULT(visitIfStmt)
AST_PUSH_POP_DEFAULT(visitSetLiteral)
AST_PUSH_POP_DEFAULT(visitUnwrapOrElseExpr)
AST_PUSH_POP_DEFAULT(visitSlice)
AST_PUSH_POP_DEFAULT(visitConditionalExpr)
AST_PUSH_POP_DEFAULT(visitTildeT)
AST_PUSH_POP_DEFAULT(visitBreakStmt)
AST_PUSH_POP_DEFAULT(visitReturnStmt)
AST_PUSH_POP_DEFAULT(visitListType)
AST_PUSH_POP_DEFAULT(visitMlComment)
AST_PUSH_POP_DEFAULT(visitMapT)
AST_PUSH_POP_DEFAULT(visitExpressionMode)
AST_PUSH_POP_DEFAULT(visitFunctionFormal)
AST_PUSH_POP_DEFAULT(visitOutputClause)
AST_PUSH_POP_DEFAULT(visitXMLType)
AST_PUSH_POP_DEFAULT(visitOpOutputs)
AST_PUSH_POP_DEFAULT(visitNullLiteral)
AST_PUSH_POP_DEFAULT(visitProtoFormals)
AST_PUSH_POP_DEFAULT(visitLocalDeclItem)
AST_PUSH_POP_DEFAULT(visitConfigClause)
AST_PUSH_POP_DEFAULT(visitVarClause)
AST_PUSH_POP_DEFAULT(visitFunctionHead)
AST_PUSH_POP_DEFAULT(visitMapType)
AST_PUSH_POP_DEFAULT(visitTypeFormals)
AST_PUSH_POP_DEFAULT(visitNamespace)
AST_PUSH_POP_DEFAULT(visitStringLiteral)
AST_PUSH_POP_DEFAULT(visitDefType)
AST_PUSH_POP_DEFAULT(visitListT)
AST_PUSH_POP_DEFAULT(visitMapping)
AST_PUSH_POP_DEFAULT(visitLocalDecl)
AST_PUSH_POP_DEFAULT(visitBoundsFormals)
AST_PUSH_POP_DEFAULT(visitProtoFormal)
AST_PUSH_POP_DEFAULT(visitWhileStmt)
AST_PUSH_POP_DEFAULT(visitAnnotation)
AST_PUSH_POP_DEFAULT(visitCompositeOut)
AST_PUSH_POP_DEFAULT(visitOpActualType)
AST_PUSH_POP_DEFAULT(visitPortInputs)
AST_PUSH_POP_DEFAULT(visitXMLLiteral)
AST_PUSH_POP_DEFAULT(visitForInit)
AST_PUSH_POP_DEFAULT(visitPlusPlusT)
AST_PUSH_POP_DEFAULT(visitUseDirective)
AST_PUSH_POP_DEFAULT(visitUseWildcard)
AST_PUSH_POP_DEFAULT(visitUnwrapExpr)
AST_PUSH_POP_DEFAULT(visitMinusMinusT)
AST_PUSH_POP_DEFAULT(visitTypeClause)
AST_PUSH_POP_DEFAULT(visitFunctionT)
AST_PUSH_POP_DEFAULT(visitLogicState)
AST_PUSH_POP_DEFAULT(visitInfixExpr)
AST_PUSH_POP_DEFAULT(visitSetT)
AST_PUSH_POP_DEFAULT(visitAttributeT)
AST_PUSH_POP_DEFAULT(visitTypeDims)
AST_PUSH_POP_DEFAULT(visitTypeArgs)
AST_PUSH_POP_DEFAULT(visitParenthesizedExpr)
AST_PUSH_POP_DEFAULT(visitUseDirectiveTail)
AST_PUSH_POP_DEFAULT(visitGraphClause)
AST_PUSH_POP_DEFAULT(visitNumericLiteral)
AST_PUSH_POP_DEFAULT(visitCastExpr)
AST_PUSH_POP_DEFAULT(visitAttributeDecl)

}
