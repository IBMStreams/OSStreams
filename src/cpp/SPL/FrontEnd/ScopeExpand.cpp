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

#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/ScopeExpander.h> // Include this header first
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>

namespace SPL {

ScopeExpander::ScopeExpander(ParserContext& pContext)
  : AstVisitorPushPop(pContext, Debug::TraceScopeExpander)
  , _opInvoke(NULL)
{}

void ScopeExpander::run(ParserContext& pContext, AstNode& ast)
{
    ScopeExpander expander(pContext);
    MODEL("PhaseStart(ScopeExpander, " << (void*)&ast << ")\n");
    expander.visit(ast);
    MODEL("PhaseEnd(ScopeExpander, " << (void*)&ast << ")\n");
}

void ScopeExpander::visitCompositeDef(AstNode& ast)
{
    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::CompositeDefSym>().expandParamScope();
    }

    _symTab.pushOld(ast);
    AstVisitor::visitCompositeDef(ast);
    _symTab.pop(ast);
}

void ScopeExpander::visitConfigItem(AstNode& ast)
{
    visit(AST::config::id(ast));
    _symTab.pushOld(ast);
    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::ActualConfigSym>().expand();
    }
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        visit(AST::config::expr(ast, i));
    }
    _symTab.pop(ast);
}

void ScopeExpander::visitOpInvoke(AstNode& ast)
{
    assert(NULL == _opInvoke);
    _opInvoke = &ast;

    _symTab.pushOld(ast);

    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::OpInvokeSym>().expand();
    }

    AstVisitor::visitOpInvoke(ast);
    _symTab.pop(ast);

    _opInvoke = NULL;
}

void ScopeExpander::visitOnProcessLogic(AstNode& ast)
{
    _symTab.pushOld(ast);

    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::OnProcessLogicSym>().expand();
    }

    visit(AST::opInvokeLogicOnProcess::stmt(ast));
    _symTab.pop(ast);
}

void ScopeExpander::visitOnTupleLogic(AstNode& ast)
{
    visit(AST::opInvokeLogicOnTuple::id(ast));
    _symTab.pushOld(ast);

    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::OnTupleLogicSym>().expand();
    }

    visit(AST::opInvokeLogicOnTuple::stmt(ast));
    _symTab.pop(ast);
}

void ScopeExpander::visitOnPunctLogic(AstNode& ast)
{
    visit(AST::opInvokeLogicOnPunct::id(ast));
    _symTab.pushOld(ast);

    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::OnPunctLogicSym>().expand();
    }

    visit(AST::opInvokeLogicOnPunct::stmt(ast));
    _symTab.pop(ast);
}

#if 0
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
void ScopeExpander::visitOnWatermarkLogic(AstNode & ast)
{
    visit(AST::opInvokeLogicOnWatermark::id(ast));
    _symTab.pushOld(ast);

    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::OnWatermarkLogicSym>().expand();
    }

    visit(AST::opInvokeLogicOnWatermark::stmt(ast));
    _symTab.pop(ast);
}
#endif

void ScopeExpander::visitOpInvokeWindow(AstNode& ast)
{
    visit(AST::opInvokeWindow::id(ast));
    _symTab.pushOld(ast);

    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::OpInvokeWindowSym>().expand();
    }

    for (int i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        visit(AST::opInvokeWindow::expr(ast, i));
    }
    _symTab.pop(ast);
}

void ScopeExpander::visitOpInvokeActual(AstNode& ast)
{
    visit(AST::opInvokeActual::id(ast));
    _symTab.pushOld(ast);

    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        assert(NULL != _opInvoke);
        astSymbol(ast).as<SPL::OpInvokeActualSym>().expand(*_opInvoke);
    }

    visit(AST::opInvokeActual::opActual(ast));
    _symTab.pop(ast);
}

void ScopeExpander::visitOpInvokeOutput(AstNode& ast)
{
    visit(AST::opInvokeOutput::id(ast));
    _symTab.pushOld(ast);

    OpInvokeOutputSym& sym = astSymbol(ast).as<OpInvokeOutputSym>();
    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        assert(NULL != _opInvoke);
        sym.expand(*_opInvoke);
    }

    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        visit(AST::infixExpr::lhs(exprAst));
        sym.switchRight();
        visit(AST::infixExpr::rhs(exprAst));
        sym.switchLeft();
    }
    _symTab.pop(ast);
}

void ScopeExpander::visitTupleAttrib(AstNode& ast)
{
    _symTab.pushOld(ast);
    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::TupleAttribSym>().expand();
    }
    AstVisitor::visitTupleAttrib(ast);
    _symTab.pop(ast);
}

void ScopeExpander::visitTupleExtend(AstNode& ast)
{
    _symTab.pushOld(ast);
    if (astStage(ast) < FrontEndStage::SCOPE_EXPANDER) {
        astSymbol(ast).as<SPL::TupleExtendSym>().expand();
    }
    AstVisitor::visitTupleExtend(ast);
    _symTab.pop(ast);
}

} // namespace SPL
