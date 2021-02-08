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

#include <SPL/FrontEnd/LexicalNameUseFinder.h>

#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SymbolTable.h>

#include <SPL/Utility/Debug.h>

namespace SPL {
void LexicalNameUseFinder::run(ParserContext& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    LexicalNameUseFinder visitor(pCtx);
    MODEL("PhaseStart(LexicalNameUseFinder, " << (void*)&ast << ")\n");
    visitor.visit(ast);
    MODEL("PhaseEnd(LexicalNameUseFinder, " << (void*)&ast << ")\n");
}

LexicalNameUseFinder::LexicalNameUseFinder(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceNameUseFinder)
{}

void LexicalNameUseFinder::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": ast type: " << astType(ast) << ": spelling: " << astText(ast)
                               << " : current stage: " << astStage(ast).toString(),
           Debug::TraceNameUseFinder);
    if (astStage(ast) < FrontEndStage::LEXICAL_NAME_USE_FINDER) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::LEXICAL_NAME_USE_FINDER);
        AstVisitorPushPop::visit(ast);
    }
}

// ----- Composite operators are defined at the top-level in a namespace.
void LexicalNameUseFinder::visitOpActualName(AstNode& ast)
{
    // defer to RelativeNameUseFinder
}

// ----- Streams are defined in a composite operator's graph clause.
void LexicalNameUseFinder::visitOpInvokeHead(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    visit(AST::opInvokeHead::outputs(ast));
    visit(AST::opInvokeHead::alias(ast));
    AstNode& opNameAst = AST::opInvokeHead::opName(ast);
    if (AST::idT::is(opNameAst)) {
        _symTab.useSimpleOperatorId(_pCtx, opNameAst);
    } else {
        _symTab.useQualifiedOperator(_pCtx, opNameAst);
    }
    visit(AST::opInvokeHead::inputs(ast));
}

void LexicalNameUseFinder::visitPortInputs(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    if (AST::portInputs::has_type(ast)) {
        visit(AST::portInputs::type(ast));
    }
    for (int i = 0, n = AST::portInputs::idCount(ast); i < n; i++) {
        _symTab.useSimpleStreamId(_pCtx, AST::portInputs::id(ast, i));
    }
    if (AST::portInputs::has_alias(ast)) {
        visit(AST::portInputs::alias(ast));
    }
}

void LexicalNameUseFinder::visitOnProcessLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    AstVisitorPushPop::visitOnProcessLogic(ast);
}

void LexicalNameUseFinder::visitOnTupleLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    _symTab.useSimpleStreamId(_pCtx, AST::opInvokeLogicOnTuple::id(ast));
    AstVisitorPushPop::visitOnTupleLogic(ast);
}

void LexicalNameUseFinder::visitOnPunctLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    _symTab.useSimpleStreamId(_pCtx, AST::opInvokeLogicOnPunct::id(ast));
    AstVisitorPushPop::visitOnPunctLogic(ast);
}

#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    void LexicalNameUseFinder::visitOnWatermarkLogic(AstNode & ast)
    {
        SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
        _symTab.useSimpleStreamId(_pCtx, AST::opInvokeLogicOnWatermark::id(ast));
        AstVisitorPushPop::visitOnWatermarkLogic(ast);
    }
#endif

void LexicalNameUseFinder::visitOpInvokeWindow(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    _symTab.useSimpleStreamId(_pCtx, AST::opInvokeWindow::id(ast));
    AstVisitorPushPop::visitOpInvokeWindow(ast);
}

void LexicalNameUseFinder::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    _symTab.useSimpleStreamId(_pCtx, AST::opInvokeOutput::id(ast));
    AstVisitorPushPop::visitOpInvokeOutput(ast);
}

// ----- Expressions can appear in many places in the grammar.
void LexicalNameUseFinder::visitCallExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    AstNode& tgtAst = AST::callExpr::target(ast);
    if (AST::idT::is(tgtAst)) {
        _symTab.useSimpleIdIfDollar(_pCtx, tgtAst);
    }
    for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
        visit(AST::callExpr::actual(ast, i));
    }
}

void LexicalNameUseFinder::visitQualifierExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    _symTab.useQualifiedId(_pCtx, ast);
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void LexicalNameUseFinder::visitTypeName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameUseFinder);
    AstNode& contentsAst = AST::typeName::contents(ast);
    if (AST::idT::is(contentsAst)) {
        _symTab.useSimpleTypeId(_pCtx, contentsAst);
    } else if (AST::qualifierExpr::is(contentsAst)) {
        _symTab.useQualifiedType(_pCtx, contentsAst);
    } else {
        assert(AST::attributeExpr::is(contentsAst));
        AstNode& baseAst = AST::attributeExpr::base(contentsAst);
        if (AST::idT::is(baseAst)) {
            _symTab.useSimpleOperatorId(_pCtx, baseAst);
        } else {
            assert(AST::qualifierExpr::is(baseAst));
            _symTab.useQualifiedOperator(_pCtx, baseAst);
        }
    }
    copyAstSymbol(AstPool::instance(), ast, contentsAst);
}
}
