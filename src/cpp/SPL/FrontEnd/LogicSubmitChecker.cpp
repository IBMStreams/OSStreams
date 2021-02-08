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

#include <SPL/FrontEnd/LogicSubmitChecker.h>

#include <SPL/FrontEnd/Aliasing.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

namespace SPL {
void LogicSubmitChecker::run(ParserContext& pCtx, AstNode& ast)
{
    LogicSubmitChecker visitor(pCtx);
    MODEL("PhaseStart(LogicSubmitChecker, " << (void*)&ast << ")\n");
    visitor.visit(ast);
    MODEL("PhaseEnd(LogicSubmitChecker, " << (void*)&ast << ")\n");
}

AstNode& LogicSubmitChecker::getSubmitActual(AstNode& submitCallAst)
{
    assert(isSubmitTupleCall(submitCallAst));
    return AST::callExpr::actual(submitCallAst, 0);
}

bool LogicSubmitChecker::isSubmitTupleCall(AstNode& ast)
{
    FunctionHeadSym* tgt = astSymbol(ast).as<CallExpressionSym>().resolvedTgt();
    if (NULL == tgt || !tgt->isIntrinsic() || tgt->name() != "submit") {
        return false;
    }
    FunctionFormalSym const& formal = *tgt->formals().at(0);
    return formal.type().getMetaType() == MetaType::TYPEFORMAL; // as opposed to punctuation
}

class LogicSubmitCheckerState
{
  private:
    typedef std::tr1::unordered_set<AstNode*> MySet;
    typedef MySet::const_iterator Iter;
    MySet _submitCalls;

  public:
    LogicSubmitCheckerState() {}
    void add(AstNode& submitCallAst)
    {
        SPLDBG(__PRETTY_FUNCTION__ << ": Adding submitCallAst: " << (void*)&submitCallAst,
               Debug::TraceSubmitLogicVisitor);
        Iter it = _submitCalls.find(&submitCallAst);
        if (it == _submitCalls.end()) {
            _submitCalls.insert(&submitCallAst);
        }
    }
    void clear() { _submitCalls.clear(); }
    LogicSubmitCheckerState& clone() const
    {
        SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSubmitLogicVisitor);
        LogicSubmitCheckerState& result = *new LogicSubmitCheckerState();
        for (Iter it = _submitCalls.begin(), end = _submitCalls.end(); it != end; ++it) {
            AstNode* ast = *it;
            result.add(*ast);
        }
        return result;
    }
    AstNode* getPtr(AstNode& accessedAst) const
    {
        SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSubmitLogicVisitor);
        for (Iter it = _submitCalls.begin(), end = _submitCalls.end(); it != end; ++it) {
            AstNode& submitCallAst = **it;
            AstNode& submittedAst = LogicSubmitChecker::getSubmitActual(submitCallAst);
            if (Aliasing::isAccessTo(accessedAst, submittedAst)) {
                return &submitCallAst;
            }
        }
        return NULL;
    }
    static LogicSubmitCheckerState& meet(LogicSubmitCheckerState& x, LogicSubmitCheckerState& y)
    {
        SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSubmitLogicVisitor);
        LogicSubmitCheckerState& result = x.clone();
        for (Iter it = y._submitCalls.begin(), end = y._submitCalls.end(); it != end; ++it) {
            result.add(**it);
        }
        return result;
    }
    std::ostream& print(std::ostream& os) const
    {
        os << "LogicSubmitCheckerState_" << ((unsigned long long)this) << "(";
        bool isFirst = true;
        for (Iter it = _submitCalls.begin(), end = _submitCalls.end(); it != end; ++it) {
            if (!isFirst) {
                os << ", ";
            }
            isFirst = false;
            os << *it;
        }
        return os << ")";
    }
    void remove(AstNode& submitCallAst)
    {
        SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSubmitLogicVisitor);
        Iter victim = _submitCalls.end();
        for (Iter it = _submitCalls.begin(), end = _submitCalls.end(); it != end; ++it) {
            AstNode* s = *it;
            if (s == &submitCallAst) {
                victim = it;
                break;
            }
        }
        assert(victim != _submitCalls.end());
        _submitCalls.erase(victim);
    }
};

std::ostream& operator<<(std::ostream& os, LogicSubmitCheckerState const& state)
{
    return state.print(os);
}

LogicSubmitChecker::LogicSubmitChecker(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceSubmitLogicVisitor)
  , _state(new LogicSubmitCheckerState())
{}

LogicSubmitChecker::~LogicSubmitChecker()
{
    delete _state;
}

void LogicSubmitChecker::enterScope(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    /* nothing to do here */
}

void LogicSubmitChecker::exitScope(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    Scope* scope = astSymbol(ast).heldScope();
    assert(NULL != scope);
    for (int i = 0, n = scope->size(); i < n; i++) {
        SymbolTableEntry& sym = scope->get(i);
        if (sym.is<VariableSym>()) {
            AstNode& accessedAst = sym.as<VariableSym>().idAst();
            while (true) {
                AstNode* submitCallAst = _state->getPtr(accessedAst);
                if (NULL == submitCallAst) {
                    break;
                }
                _state->remove(*submitCallAst);
            }
        }
    }
}

void LogicSubmitChecker::rewriteSubmitCall(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    assert(isSubmitTupleCall(ast));
    CallExpressionSym& sym = astSymbol(ast).as<CallExpressionSym>();
    sym.setSubmitNeedingCopy();
}

void LogicSubmitChecker::visitExprAccess(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    if (AST::attributeExpr::is(ast) || AST::qualifierExpr::is(ast) || AST::subscriptExpr::is(ast)) {
        while (true) {
            AstNode* submitCallAst = _state->getPtr(ast);
            if (NULL == submitCallAst) {
                break;
            }
            assert(Aliasing::isAccessTo(ast, getSubmitActual(*submitCallAst)));
            SPLDBG(
              __PRETTY_FUNCTION__
                << astSymbol(*submitCallAst).sourceLocation()
                << ": Rewriting submit call to copy its tuple, since the actual is accessed later.",
              Debug::TraceSubmitLogicVisitor);
            SPLDBG(astSymbol(ast).sourceLocation()
                     << ": DETAILS: This is the access causing the submit call to be rewritten.",
                   Debug::TraceSubmitLogicVisitor);
            rewriteSubmitCall(*submitCallAst);
            _state->remove(*submitCallAst);
        }
    } else {
        visit(ast);
    }
}

void LogicSubmitChecker::visitExprDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprAccess(ast);
}

void LogicSubmitChecker::visitExprUse(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprAccess(ast);
}

void LogicSubmitChecker::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    if (astStage(ast) < FrontEndStage::LOGIC_SUBMIT_CHECKER) {
        bool allDone = true;
        for (int i = 0, n = _loopNestingDoneMarks.size(); i < n; i++) {
            allDone &= _loopNestingDoneMarks.at(i);
        }
        if (allDone) {
            setAstStage(AstPool::instance(), ast, FrontEndStage::LOGIC_SUBMIT_CHECKER);
        }
        AstVisitorPushPop::visit(ast);
    }
}

// ----- The compilation unit is the start symbol of the SPL grammar.
void LogicSubmitChecker::visitNamespaceName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    /* skip (the identifiers in there are not relevant for this checker) */
}

// ----- Streams are defined in a composite operator's graph clause.
void LogicSubmitChecker::visitOnProcessLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    enterScope(ast);
    AstVisitorPushPop::visitOnProcessLogic(ast);
    _state->clear();
    exitScope(ast);
}

void LogicSubmitChecker::visitOnTupleLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    enterScope(ast);
    AstVisitorPushPop::visitOnTupleLogic(ast);
    _state->clear();
    exitScope(ast);
}

void LogicSubmitChecker::visitOnPunctLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    enterScope(ast);
    AstVisitorPushPop::visitOnPunctLogic(ast);
    _state->clear();
    exitScope(ast);
}

#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    void LogicSubmitChecker::visitOnWatermarkLogic(AstNode & ast)
    {
        SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast, Debug::TraceSubmitLogicVisitor);
        enterScope(ast);
        AstVisitorPushPop::visitOnWatermarkLogic(ast);
        _state->clear();
        exitScope(ast);
    }
#endif

void LogicSubmitChecker::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    /* skip (can't call submit(tuple, port) from in there) */
}

// ----- Functions are defined at the top-level in a namespace.
void LogicSubmitChecker::visitFunctionDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    /* skip (can't call submit(tuple, port) from in there) */
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void LogicSubmitChecker::visitLocalDeclItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    AstNode& initAst = AST::localDeclItem::expr_q(ast);
    AstNode& idAst = AST::localDeclItem::id(ast);
    if (!AST::noneT::is(initAst)) {
        visitExprUse(initAst);
    }
    visitExprDef(idAst);
}

void LogicSubmitChecker::visitBlockStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    enterScope(ast);
    AstVisitorPushPop::visitBlockStmt(ast);
    exitScope(ast);
}

void LogicSubmitChecker::visitExprStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::exprStmt::expr(ast));
}

void LogicSubmitChecker::visitIfStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::ifStmt::expr(ast));
    LogicSubmitCheckerState* elseState = &_state->clone();
    visit(AST::ifStmt::stmt(ast));
    LogicSubmitCheckerState* thenState = _state;
    if (AST::ifStmt::has_elseClause(ast)) {
        _state = elseState;
        visit(AST::ifStmt::elseClause(ast));
        elseState = _state;
    }
    _state = &LogicSubmitCheckerState::meet(*thenState, *elseState);
    delete thenState;
    delete elseState;
}

void LogicSubmitChecker::visitForStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    _loopNestingContext.push_back(&ast);
    AstNode& initAst = AST::forStmt::forInit(ast);
    visit(AST::forInit::type(initAst));
    LogicSubmitCheckerState* oldState = &_state->clone();
    for (int i = 0; i < 2; i++) {
        _loopNestingDoneMarks.push_back(1 == i);
        // visit body twice, first time to find submit, second time to find errors
        visitExprUse(AST::forInit::expr(initAst));
        _symTab.pushOld(ast);
        enterScope(ast);
        visitExprDef(AST::forInit::id(initAst));
        visit(AST::forStmt::stmt(ast));
        exitScope(ast);
        _symTab.pop(ast);
        _loopNestingDoneMarks.pop_back();
    }
    LogicSubmitCheckerState* newState = _state;
    _state = &LogicSubmitCheckerState::meet(*oldState, *newState);
    delete oldState;
    delete newState;
    assert(&ast == _loopNestingContext.back());
    _loopNestingContext.pop_back();
}

void LogicSubmitChecker::visitWhileStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    _loopNestingContext.push_back(&ast);
    visitExprUse(AST::whileStmt::expr(ast));
    LogicSubmitCheckerState* oldState = &_state->clone();
    for (int i = 0; i < 2; i++) {
        _loopNestingDoneMarks.push_back(1 == i);
        // visit body twice, first time to find submit, second time to find errors
        visit(AST::whileStmt::stmt(ast));
        visit(AST::whileStmt::stmt(ast));
        _loopNestingDoneMarks.pop_back();
    }
    LogicSubmitCheckerState* newState = _state;
    _state = &LogicSubmitCheckerState::meet(*oldState, *newState);
    delete oldState;
    delete newState;
    assert(&ast == _loopNestingContext.back());
    _loopNestingContext.pop_back();
}

void LogicSubmitChecker::visitReturnStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    if (AST::returnStmt::has_expr(ast)) {
        visitExprUse(AST::returnStmt::expr(ast));
    }
}

// ----- Expressions can appear in many places in the grammar.
void LogicSubmitChecker::visitMapping(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::mapping::fromExpr(ast));
    visitExprUse(AST::mapping::toExpr(ast));
}

void LogicSubmitChecker::visitAttributeExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::attributeExpr::base(ast));
}

void LogicSubmitChecker::visitCastExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visit(AST::castExpr::type(ast));
    visitExprUse(AST::castExpr::expr(ast));
}

void LogicSubmitChecker::visitCallExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visit(AST::callExpr::target(ast));
    for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
        visitExprUse(AST::callExpr::actual(ast, i));
    }
    if (isSubmitTupleCall(ast)) {
        CallExpressionSym& tgt = astSymbol(ast).as<CallExpressionSym>();
        if (!tgt.getSubmitNeedingCopy()) {
            SymbolTableEntry& submittedSym = astSymbol(getSubmitActual(ast));
            SPLDBG(__PRETTY_FUNCTION__ << ": Submitting " << submittedSym.name(),
                   Debug::TraceSubmitLogicVisitor);
            if (symIsMutable(submittedSym, _loopNestingContext)) {
                // If the var being submitted is a state var, assume it will be modified
                if (submittedSym.is<VariableSym>()) {
                    Scope* parentScope = submittedSym.parentScope();
                    assert(NULL != parentScope);
                    SymbolTableEntry* holder = parentScope->holder();
                    if (NULL != holder and holder->is<OpInvokeSym>()) {
                        SymbolTableEntry& opInvoke = holder->as<OpInvokeSym>();
                        if (parentScope == opInvoke.heldScope()) {
                            SPLDBG(__PRETTY_FUNCTION__
                                     << astSymbol(ast).sourceLocation() << ": "
                                     << submittedSym.name()
                                     << " is a state variable; rewriting submit call.",
                                   Debug::TraceSubmitLogicVisitor);
                            rewriteSubmitCall(ast);
                            return;
                        }
                    }
                }
                _state->add(ast); // use data flow analysis to find later accesses
            } else {
                SPLDBG(
                  __PRETTY_FUNCTION__
                    << astSymbol(ast).sourceLocation()
                    << ": Rewriting submit call to copy its tuple, since the actual is immutable.",
                  Debug::TraceSubmitLogicVisitor);
                rewriteSubmitCall(ast);
            }
        }
    }
}

void LogicSubmitChecker::visitConditionalExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::conditionalExpr::condExpr(ast));
    visitExprUse(AST::conditionalExpr::thenExpr(ast));
    visitExprUse(AST::conditionalExpr::elseExpr(ast));
}

void LogicSubmitChecker::visitInfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    if (Expression::eq == astNodeOp(ast)) {
        visitExprUse(AST::infixExpr::rhs(ast));
        visitExprDef(AST::infixExpr::lhs(ast));
    } else {
        visitExprUse(AST::infixExpr::lhs(ast));
        visitExprUse(AST::infixExpr::rhs(ast));
    }
}

void LogicSubmitChecker::visitParenthesizedExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::parenthesizedExpr::expr(ast));
}

void LogicSubmitChecker::visitPostfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::postfixExpr::expr(ast));
}

void LogicSubmitChecker::visitPrefixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::prefixExpr::expr(ast));
}

void LogicSubmitChecker::visitSubscriptExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::subscriptExpr::expr(ast));
    visitExprUse(AST::subscriptExpr::subscript(ast));
}

void LogicSubmitChecker::visitSlice(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::subscriptSlice::lower(ast));
    visitExprUse(AST::subscriptSlice::upper(ast));
}

// ----- Literals are the highest-precedence expressions denoting values.
void LogicSubmitChecker::visitListLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        visitExprUse(AST::listLiteral::expr(ast, i));
    }
}

void LogicSubmitChecker::visitTupleLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    enterScope(ast);
    AstVisitorPushPop::visitTupleLiteral(ast);
    exitScope(ast);
}

void LogicSubmitChecker::visitAttributeAssign(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::attributeAssign::expr(ast));
    visitExprDef(AST::attributeAssign::id(ast));
}

void LogicSubmitChecker::visitSetLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        visitExprUse(AST::setLiteral::expr(ast, i));
    }
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void LogicSubmitChecker::visitTypeDims(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast: " << (void*)&ast,
           Debug::TraceSubmitLogicVisitor);
    visitExprUse(AST::typeDims::expr(ast));
}
};
