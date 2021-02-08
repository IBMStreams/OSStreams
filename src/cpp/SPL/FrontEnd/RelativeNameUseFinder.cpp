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

#include <SPL/FrontEnd/RelativeNameUseFinder.h>

#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>

#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

#include <set>
#include <stack>

using namespace std;
using namespace std::tr1;

namespace SPL {
void RelativeNameUseFinder::run(ParserContext& pCtx, AstNode& ast)
{
    RelativeNameUseFinder visitor(pCtx);
    MODEL("PhaseStart(RelativeNameUseFinder, " << (void*)&ast << ")\n");
    visitor.visit(ast);
    MODEL("PhaseEnd(RelativeNameUseFinder, " << (void*)&ast << ")\n");
}

RelativeNameUseFinder::RelativeNameUseFinder(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceRelativeNameUseFinder)
  , _pushTupleLiteralSym(true)
{}

void RelativeNameUseFinder::ensureResolved(AstNode& ast)
{
    astSymbol(ast).type();
}

void RelativeNameUseFinder::visitTupleLiteral(AstNode& ast)
{
    if (_pushTupleLiteralSym) {
        _symTab.pushOld(ast);
    }
    allChildrenVisit(ast);
    if (_pushTupleLiteralSym) {
        _symTab.pop(ast);
    }
}

void RelativeNameUseFinder::visitExprUse(AstNode& ast)
{
    if (AST::idT::is(ast)) {
        _symTab.useSimpleValueId(_pCtx, ast);
    } else {
        visit(ast);
    }
}

void RelativeNameUseFinder::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": ast " << (void*)&ast << " :type: " << astType(ast)
                               << ": spelling: " << astText(ast)
                               << " : current stage: " << astStage(ast).toString(),
           Debug::TraceRelativeNameUseFinder);
    if (astStage(ast) < FrontEndStage::RELATIVE_NAME_USE_FINDER) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::RELATIVE_NAME_USE_FINDER);
        AstVisitorPushPop::visit(ast);
    }
}

// ----- The compilation unit is the start symbol of the SPL grammar.
void RelativeNameUseFinder::visitNamespaceName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": namespace is " << astText(AST::namespaceName::body(ast)),
           Debug::TraceRelativeNameUseFinder);
    // skip
}

// ----- Composite operators are defined at the top-level in a namespace.
void RelativeNameUseFinder::visitOpActualExprs(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    for (int i = 0, n = AST::opActualExprs::exprCount(ast); i < n; i++) {
        visitExprUse(AST::opActualExprs::expr(ast, i));
    }
}

void RelativeNameUseFinder::visitOpActualName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    _symTab.useOpActualName(_pCtx, ast);
}

void RelativeNameUseFinder::visitConfigItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visit(AST::config::id(ast));
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        visitExprUse(AST::config::expr(ast, i));
    }
    _symTab.pop(ast);
}

// ----- Streams are defined in a composite operator's graph clause.
void RelativeNameUseFinder::visitOpInvokeWindow(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visit(AST::opInvokeWindow::id(ast));
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        visitExprUse(AST::opInvokeWindow::expr(ast, i));
    }
    _symTab.pop(ast);
}

void RelativeNameUseFinder::visitOpInvokeActual(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    _symTab.useParam(_pCtx, AST::opInvokeActual::id(ast));
    _symTab.pushOld(ast);
    visit(AST::opInvokeActual::opActual(ast));
    _symTab.pop(ast);
}

void RelativeNameUseFinder::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visit(AST::opInvokeOutput::id(ast));
    _symTab.pushOld(ast);
    OpInvokeOutputSym& sym = astSymbol(ast).as<OpInvokeOutputSym>();
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        _symTab.useSimpleId(_pCtx, AST::infixExpr::lhs(exprAst), false);
        sym.switchRight();
        visitExprUse(AST::infixExpr::rhs(exprAst));
        sym.switchLeft();
    }
    _symTab.pop(ast);
}

// ----- Functions are defined at the top-level in a namespace.
void RelativeNameUseFinder::visitFunctionDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    astSymbol(ast).as<FunctionHeadSym>().initTypes(ast);
    AstVisitorPushPop::visitFunctionDef(ast);
}

// ----- Function Prototype declarations come from XML files, not regular SPL files.
void RelativeNameUseFinder::visitFunctionPrototype(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitFunctionPrototype(ast);
    astSymbol(ast).as<FunctionHeadSym>().initTypes(ast);
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void RelativeNameUseFinder::visitLocalDeclItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::localDeclItem::expr_q(ast));
}

void RelativeNameUseFinder::visitExprStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::exprStmt::expr(ast));
}

void RelativeNameUseFinder::visitIfStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::ifStmt::expr(ast));
    visit(AST::ifStmt::stmt(ast));
    if (AST::ifStmt::has_elseClause(ast)) {
        visit(AST::ifStmt::elseClause(ast));
    }
}

void RelativeNameUseFinder::visitForStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstNode& initAst = AST::forStmt::forInit(ast);
    visit(AST::forInit::modifiers(initAst));
    visit(AST::forInit::type(initAst));
    visitExprUse(AST::forInit::expr(initAst));
    _symTab.pushOld(ast);
    visit(AST::forInit::id(initAst));
    visit(AST::forStmt::stmt(ast));
    _symTab.pop(ast);
}

void RelativeNameUseFinder::visitWhileStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::whileStmt::expr(ast));
    visit(AST::whileStmt::stmt(ast));
}

void RelativeNameUseFinder::visitReturnStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    if (AST::returnStmt::has_expr(ast)) {
        visitExprUse(AST::returnStmt::expr(ast));
    }
}

// ----- Expressions can appear in many places in the grammar.
void RelativeNameUseFinder::visitMapping(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::mapping::fromExpr(ast));
    visitExprUse(AST::mapping::toExpr(ast));
}

void RelativeNameUseFinder::visitAttributeExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstNode& baseAst = AST::attributeExpr::base(ast);
    if (AST::idT::is(baseAst)) {
        _symTab.useSimpleId(_pCtx, baseAst, false);
    } else {
        visit(baseAst);
    }
    visit(AST::attributeExpr::id(ast));
}

void RelativeNameUseFinder::visitCastExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visit(AST::castExpr::type(ast));
    visitExprUse(AST::castExpr::expr(ast));
}

void RelativeNameUseFinder::visitCallExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstNode& tgtAst = AST::callExpr::target(ast);
    if (!AST::idT::is(tgtAst) && !AST::qualifierExpr::is(tgtAst)) {
        visitExprUse(tgtAst);
    }
    bool isOutputPortIntrinsic = AST::idT::is(tgtAst) && astText(tgtAst) == "outputPort";
    for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
        AstNode& actualAst = AST::callExpr::actual(ast, i);
        if (isOutputPortIntrinsic && AST::idT::is(actualAst)) {
            _symTab.useSimpleStreamId(_pCtx, actualAst);
        } else {
            visitExprUse(actualAst);
        }
    }
}

void RelativeNameUseFinder::visitConditionalExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::conditionalExpr::condExpr(ast));
    visitExprUse(AST::conditionalExpr::thenExpr(ast));
    visitExprUse(AST::conditionalExpr::elseExpr(ast));
}

void RelativeNameUseFinder::visitInfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::infixExpr::lhs(ast));
    visitExprUse(AST::infixExpr::rhs(ast));
}

void RelativeNameUseFinder::visitParenthesizedExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::parenthesizedExpr::expr(ast));
}

void RelativeNameUseFinder::visitPostfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::postfixExpr::expr(ast));
}

void RelativeNameUseFinder::visitPrefixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::prefixExpr::expr(ast));
}

void RelativeNameUseFinder::visitSubscriptExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::subscriptExpr::expr(ast));
    visitExprUse(AST::subscriptExpr::subscript(ast));
}

void RelativeNameUseFinder::visitSlice(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::subscriptSlice::lower(ast));
    visitExprUse(AST::subscriptSlice::upper(ast));
}

void RelativeNameUseFinder::visitUnwrapExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::unwrapExpr::lhs(ast));
}

void RelativeNameUseFinder::visitUnwrapOrElseExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::unwrapOrElseExpr::lhs(ast));
    visitExprUse(AST::unwrapOrElseExpr::rhs(ast));
}

void RelativeNameUseFinder::visitIsPresentExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visitExprUse(AST::isPresentExpr::lhs(ast));
}

// ----- Literals are the highest-precedence expressions denoting values.
void RelativeNameUseFinder::visitListLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        visitExprUse(AST::listLiteral::expr(ast, i));
    }
}

void RelativeNameUseFinder::visitAttributeAssign(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    visit(AST::attributeAssign::id(ast));
    visitExprUse(AST::attributeAssign::expr(ast));
}

void RelativeNameUseFinder::visitSetLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        visitExprUse(AST::setLiteral::expr(ast, i));
    }
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void RelativeNameUseFinder::visitListType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitListType(ast);
    ensureResolved(ast);
}

void RelativeNameUseFinder::visitMapType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitMapType(ast);
    ensureResolved(ast);
}

void RelativeNameUseFinder::visitRStringType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitRStringType(ast);
    ensureResolved(ast);
}

void RelativeNameUseFinder::visitSetType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitSetType(ast);
    ensureResolved(ast);
}

void RelativeNameUseFinder::visitTupleAttrib(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitTupleAttrib(ast);
    ensureResolved(ast);
}

void RelativeNameUseFinder::visitTupleExtend(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitTupleExtend(ast);
    ensureResolved(ast);
}

void RelativeNameUseFinder::visitTypeDims(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    AstVisitorPushPop::visitTypeDims(ast);
    visitExprUse(AST::typeDims::expr(ast));
}

static EnumTypeSym& createEnumTypeSym(const string& typeName,
                                      EnumTyp::Values& values,
                                      const SourceLocation& loc,
                                      Scope* parentScope)
{
    EnumTyp const& eType = TypeFactory::instance().enumT(values);
    string* enumTypeName = new string(typeName); // TODO is this leaked?
    EnumTypeSym* etSym = new EnumTypeSym(loc, enumTypeName, parentScope,
                                         eType); // TODO also this looks like it might be leaked.
    Scope* enumScope = etSym->heldScope();
    for (EnumTyp::Values::const_iterator it = values.begin(), itEnd = values.end(); it != itEnd;
         ++it) {
        const string& str = *it;
        EnumValueSym* evSym = new EnumValueSym(loc, str, *enumScope, *etSym);
        enumScope->def(*evSym);
    }
    return *etSym;
}

StreamSym* findStreamSymbol(AstNode& stream)
{
    SymbolTableEntry& sSym = astSymbol(stream);
    if (sSym.is<StreamSym>()) {
        return &sSym.as<StreamSym>();
    } else if (sSym.is<CompositeInputPortSym>()) {
        // We have a reference to an input port on a composite.  Get its instance input ports
        const CompositeInputPortSym& cSym = sSym.as<CompositeInputPortSym>();
        AstNode& actual = cSym.actualAst();
        if (AST::portInputs::is(actual)) {
            // Get the child at 0 as we just need to get to a stream symbol
            AstNode& child = AST::portInputs::id(actual, 0);
            return findStreamSymbol(child);
        }
    }
    return NULL;
}

// We need to do some name lookup.  This is not the normal name lookup because we don't want to look
// up any scopes.  We want to ensure that the port or port alias specified is a port for the
// specified operator Also the attributes specified for attributes must be elements of the type of
// that port
void RelativeNameUseFinder::visitAnnotationT(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceRelativeNameUseFinder);
    SymbolTableEntry& sym = astSymbol(ast);
    if (sym.is<SimpleAnnotationSym>()) {
        SimpleAnnotationSym& aSym = sym.as<SimpleAnnotationSym>();
        const SourceLocation& loc = aSym.sourceLocation();
        if (aSym.name() == "consistent") {
            Scope* annotScope = aSym.heldScope();
            // We need to implicitly declare the enum for trigger here
            EnumTyp::Values values;
            values.push_back("periodic");
            values.push_back("operatorDriven");
            EnumTypeSym& etSym = createEnumTypeSym("@triggerType@", values, loc, aSym.heldScope());
            annotScope->def(etSym);
            annotScope->addAll(*etSym.heldScope());
        } else if (aSym.name() == "catch") {
            Scope* annotScope = aSym.heldScope();
            // We need to implicitly declare the enum for exception here
            EnumTyp::Values values;
            values.push_back("streams");
            values.push_back("std");
            values.push_back("all");
            values.push_back("none");
            EnumTypeSym& etSym =
              createEnumTypeSym("@exceptionType@", values, loc, aSym.heldScope());
            annotScope->def(etSym);
            annotScope->addAll(*etSym.heldScope());
        }
        _symTab.push(&ast, sym);
        const std::vector<std::pair<AstNode*, AstNode*> >& keyValuePairs = aSym.keyValuePairs();
        for (std::vector<std::pair<AstNode*, AstNode*> >::const_iterator it = keyValuePairs.begin();
             it != keyValuePairs.end(); ++it) {
            AstNode& value = *it->second;
            visitExprUse(value);
        }
        _symTab.pop(ast);
    } else if (sym.is<ThreadingAnnotationSym>()) {
        ThreadingAnnotationSym& tSym = sym.as<ThreadingAnnotationSym>();
        const SourceLocation& loc = tSym.sourceLocation();
        Scope* annotScope = tSym.heldScope();
        EnumTyp::Values values;
        values.push_back("dedicated");
        values.push_back("automatic");
        values.push_back("manual");
        values.push_back("dynamic");
        EnumTypeSym& etSym = createEnumTypeSym("@modelType@", values, loc, tSym.heldScope());
        annotScope->def(etSym);
        annotScope->addAll(*etSym.heldScope());
        _symTab.push(&ast, sym);
        AstNode& model = tSym.model();
        visitExprUse(model);
        _symTab.pop(ast);
    } else if (sym.is<EventTimeAnnotationSym>()) {
        EventTimeAnnotationSym& etSym = sym.as<EventTimeAnnotationSym>();
        SourceLocation const& loc = etSym.sourceLocation();

        AstNode* resolution = etSym.resolution();
        if (resolution) {

            etSym.setResolutionScopeMode();
            Scope* annotScope = etSym.heldScope();
            // We need to implicitly declare the enum for resolution here
            EnumTyp::Values resolutionValues;
            resolutionValues.push_back("Milliseconds");
            resolutionValues.push_back("Microseconds");
            resolutionValues.push_back("Nanoseconds");
            EnumTypeSym& resolutionSym =
              createEnumTypeSym("@resolutionType@", resolutionValues, loc, annotScope);
            annotScope->def(resolutionSym);
            annotScope->addAll(*resolutionSym.heldScope());
            _symTab.push(&ast, sym);
            visitExprUse(*resolution);
            _symTab.pop(ast);
            etSym.resetScopeMode();
        }

        AstNode* minimumGap = etSym.minimumGap();
        if (minimumGap) {
            visitExprUse(*minimumGap);
        }
        AstNode* lag = etSym.lag();
        if (lag) {
            visitExprUse(*lag);
        }

        // Find all the tuple types in all the outputs, and
        // add them to the scope
        etSym.setAttributeScopeMode();

        const OpInvokeSym& opInvoke = _symTab.currentOpInvoke();
        AstNode& invoke = opInvoke.ast();
        assert(AST::opInvoke::is(invoke));
        AstNode& head = AST::opInvoke::head(invoke);
        AstNode& opOutputs = AST::opInvokeHead::outputs(head);
        assert(AST::opOutputs::is(opOutputs));
        for (int i = 0; i < astSize(opOutputs); ++i) {
            AstNode& opOutput = astChild(opOutputs, i);
            AstNode& streamType = AST::opOutput::streamType(opOutput);
            AstNode& tupleBody = AST::streamType::tupleBody(streamType);
            SymbolTableEntry& tSym = astSymbol(tupleBody);
            Scope* streamScope = tSym.heldScope();
            etSym.heldScope()->addAll(*streamScope);
        }
        AstNode& attribute = etSym.eventTimeAttribute();
        _symTab.push(&ast, etSym);
        visitExprUse(attribute);
        _symTab.pop(etSym);
        etSym.resetScopeMode();

    } else if (sym.is<ViewAnnotationSym>()) {
        ViewAnnotationSym& vSym = sym.as<ViewAnnotationSym>();
        const SourceLocation& loc = vSym.sourceLocation();
        tr1::unordered_map<string, AstNode*> portMap;
        {
            Scope& portScope = vSym.portScope();
            // Create an enum for all the output port names
            const OpInvokeSym& opInvoke = _symTab.currentOpInvoke();
            AstNode& invoke = opInvoke.ast();
            assert(AST::opInvoke::is(invoke));
            AstNode& head = AST::opInvoke::head(invoke);
            AstNode& opOutputs = AST::opInvokeHead::outputs(head);
            assert(AST::opOutputs::is(opOutputs));
            EnumTyp::Values portNames;
            for (int i = 0; i < astSize(opOutputs); ++i) {
                AstNode& opOutput = astChild(opOutputs, i);
                AstNode& outId = AST::opOutput::id(opOutput);
                SymbolTableEntry& outSym = astSymbol(outId);
                const string& outName = outSym.name();
                portNames.push_back(outName);
                portMap.insert(make_pair(outName, &opOutput));
                if (AST::opOutput::has_alias(opOutput)) {
                    AstNode& as = AST::opOutput::alias(opOutput);
                    AstNode& alias = AST::as::id(as);
                    string aliasName = astText(alias);
                    portNames.push_back(aliasName);
                    portMap.insert(make_pair(aliasName, &opOutput));
                }
            }
            EnumTypeSym& etSym = createEnumTypeSym("@portType@", portNames, loc, vSym.heldScope());
            portScope.def(etSym);
            portScope.addAll(*etSym.heldScope());
        }

        AstNode& name = vSym.viewName();
        visitExprUse(name);
        AstNode& port = vSym.port();
        {
            vSym.setPortScopeMode();
            _symTab.push(&ast, vSym);
            visitExprUse(port);
            _symTab.pop(vSym);
            vSym.resetScopeMode();
        }
        {
            Scope& attributesScope = vSym.attributesScope();
            string portName = astText(port);
            tr1::unordered_map<string, AstNode*>::const_iterator it1 = portMap.find(portName);
            if (it1 != portMap.end()) {
                AstNode* opOutput = it1->second;
                AstNode& streamType = AST::opOutput::streamType(*opOutput);
                AstNode& tupleBody = AST::streamType::tupleBody(streamType);
                SymbolTableEntry& tSym = astSymbol(tupleBody);
                Scope* heldScope = tSym.heldScope();
                attributesScope.addAll(*heldScope);
            }
        }
        AstNode& sampleSize = vSym.sampleSize();
        visitExprUse(sampleSize);
        AstNode* bufferSize = vSym.bufferSize();
        if (NULL != bufferSize) {
            visitExprUse(*bufferSize);
        }
        AstNode* bufferTime = vSym.bufferTime();
        if (NULL != bufferTime) {
            visitExprUse(*bufferTime);
        }
        AstNode* description = vSym.description();
        if (NULL != description) {
            visitExprUse(*description);
        }
        AstNode* attributes = vSym.attributes();
        if (NULL != attributes) {
            vSym.setAttributesScopeMode();
            _symTab.push(&ast, vSym);
            visitExprUse(*attributes);
            _symTab.pop(vSym);
            vSym.resetScopeMode();
        }
        AstNode* filter = vSym.filter();
        if (NULL != filter) {
            vSym.setAttributesScopeMode();
            _symTab.push(&ast, vSym);
            _pushTupleLiteralSym = false;
            visitExprUse(*filter);
            _pushTupleLiteralSym = true;
            _symTab.pop(vSym);
            vSym.resetScopeMode();
        }
        AstNode* activateOption = vSym.activateOption();
        if (NULL != activateOption) {
            Scope& heldScope = *vSym.heldScope();

            // Create an enum for the activateOption attribute
            EnumTyp::Values values;
            values.push_back("automatic");
            values.push_back("firstAccess");
            EnumTypeSym& etSym =
              createEnumTypeSym("@activateOptionType@", values, loc, vSym.heldScope());
            heldScope.def(etSym);
            heldScope.addAll(*etSym.heldScope());

            _symTab.push(&ast, vSym);
            visitExprUse(*activateOption);
            _symTab.pop(vSym);
        }
    } else if (sym.is<ParallelAnnotationSym>()) {
        ParallelAnnotationSym& pSym = sym.as<ParallelAnnotationSym>();
        const ParallelAnnotationSym::PartitionList& partitions = pSym.partitions();
        for (ParallelAnnotationSym::PartitionList::const_iterator it = partitions.begin();
             it != partitions.end(); ++it) {
            const ParallelAnnotationSym::Partition& partition = *it;
            AstNode& port = partition.port();
            assert(AST::idT::is(port));
            string portName = astText(port);

            const OpInvokeSym& opInvoke = _symTab.currentOpInvoke();
            AstNode& invoke = opInvoke.ast();
            assert(AST::opInvoke::is(invoke));
            AstNode& head = AST::opInvoke::head(invoke);
            AstNode& opInputs = AST::opInvokeHead::inputs(head);
            StreamSym* streamSym = NULL;
            PortAliasSym* portAliasSym = NULL;
            for (int i = 0; i < astSize(opInputs); ++i) {
                AstNode& inputPort = astChild(opInputs, i);
                assert(AST::portInputs::is(inputPort));
                for (int j = 0, n = AST::portInputs::idCount(inputPort); j < n; ++j) {
                    AstNode& stream = AST::portInputs::id(inputPort, j);
                    assert(AST::idT::is(stream));
                    if (portName == astText(stream)) {
                        streamSym = findStreamSymbol(stream);
                        if (NULL != streamSym) {
                            setAstSymbol(AstPool::instance(), port, *streamSym);
                        }
                    }
                }
                if (NULL == streamSym && AST::portInputs::has_alias(inputPort)) {
                    AstNode& alias = AST::portInputs::alias(inputPort);
                    AstNode& aliasName = AST::as::id(alias);
                    if (astText(aliasName) == portName) {
                        SymbolTableEntry& aSym = astSymbol(inputPort);
                        portAliasSym = &aSym.as<PortAliasSym>();
                        setAstSymbol(AstPool::instance(), port, *portAliasSym);
                        AstNode& stream = AST::portInputs::id(inputPort, 0);
                        assert(AST::idT::is(stream));
                        streamSym = findStreamSymbol(stream);
                        break;
                    }
                }
            }
            if (NULL == streamSym && NULL == portAliasSym) {
                const SourceLocation& loc = SourceLocationFactory::instance().create(_pCtx, port);
                SysOut::errorln(SPL_FRONT_END_PARALLEL_INVALID_PORT(portName), loc);
                setAstSymbol(AstPool::instance(), port,
                             *new ErrorDummySym(loc, port, astText(port), &_symTab.currentScope()));
                return;
            }
            // We found a stream sym.  Now, if we have attributes, make sure they are valid
            AstNode& attributes = partition.attributes();
            if (AST::listLiteral::is(attributes)) {
                Scope* streamScope = streamSym->heldScope();
                Scope& heldScope = *pSym.heldScope();
                assert(NULL != streamScope);
                heldScope.addAll(*streamScope);
                _symTab.push(&ast, pSym);
                for (int i = 0, n = AST::listLiteral::exprCount(attributes); i < n; ++i) {
                    AstNode& child = AST::listLiteral::expr(attributes, i);
                    visitExprUse(child);
                }
                _symTab.pop(pSym);
            } else {
                const SourceLocation& loc =
                  SourceLocationFactory::instance().create(_pCtx, attributes);
                SysOut::errorln(SPL_FRONT_END_PARALLEL_INVALID_ATTRIBUTE_TYPE, loc);
            }
        } // end of PartitionList iterator
        AstNode* broadcasts = pSym.broadcasts();
        if (broadcasts && AST::listLiteral::is(*broadcasts)) {
            for (int k = 0, m = AST::listLiteral::exprCount(*broadcasts); k < m; ++k) {
                AstNode& port = AST::listLiteral::expr(*broadcasts, k);
                string portName = astText(port);

                const OpInvokeSym& opInvoke = _symTab.currentOpInvoke();
                AstNode& invoke = opInvoke.ast();
                assert(AST::opInvoke::is(invoke));
                AstNode& head = AST::opInvoke::head(invoke);
                AstNode& opInputs = AST::opInvokeHead::inputs(head);
                StreamSym* streamSym = NULL;
                PortAliasSym* portAliasSym = NULL;
                for (int i = 0; i < astSize(opInputs); ++i) {
                    AstNode& inputPort = astChild(opInputs, i);
                    assert(AST::portInputs::is(inputPort));
                    for (int j = 0, n = AST::portInputs::idCount(inputPort); j < n; ++j) {
                        AstNode& stream = AST::portInputs::id(inputPort, j);
                        assert(AST::idT::is(stream));
                        if (portName == astText(stream)) {
                            streamSym = findStreamSymbol(stream);
                            if (NULL != streamSym) {
                                setAstSymbol(AstPool::instance(), port, *streamSym);
                            }
                        }
                    }
                    if (NULL == streamSym && AST::portInputs::has_alias(inputPort)) {
                        AstNode& alias = AST::portInputs::alias(inputPort);
                        AstNode& aliasName = AST::as::id(alias);
                        if (astText(aliasName) == portName) {
                            SymbolTableEntry& aSym = astSymbol(inputPort);
                            portAliasSym = &aSym.as<PortAliasSym>();
                            setAstSymbol(AstPool::instance(), port, *portAliasSym);
                            AstNode& stream = AST::portInputs::id(inputPort, 0);
                            assert(AST::idT::is(stream));
                            streamSym = findStreamSymbol(stream);
                            break;
                        }
                    }
                }
                if (NULL == streamSym && NULL == portAliasSym) {
                    const SourceLocation& loc =
                      SourceLocationFactory::instance().create(_pCtx, port);
                    SysOut::errorln(SPL_FRONT_END_PARALLEL_INVALID_PORT(portName), loc);
                    setAstSymbol(
                      AstPool::instance(), port,
                      *new ErrorDummySym(loc, port, astText(port), &_symTab.currentScope()));
                    return;
                }
            }
        }
    }
}
}
