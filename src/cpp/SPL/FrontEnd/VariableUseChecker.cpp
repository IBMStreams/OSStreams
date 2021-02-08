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

#include <SPL/FrontEnd/VariableUseChecker.h>

#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/HashMapHelpers.h>

namespace SPL {
void VariableUseChecker::run(ParserContext& pCtx, AstNode& ast)
{
    VariableUseChecker visitor(pCtx);
    MODEL("PhaseStart(VariableUseChecker, " << (void*)&ast << ")\n");
    visitor.visit(ast);
    MODEL("PhaseEnd(VariableUseChecker, " << (void*)&ast << ")\n");
}

struct symHash
{
    inline size_t operator()(SymbolTableEntry const* const x) const
    {
        return reinterpret_cast<size_t>(x);
    }
};

struct symEquals
{
    inline bool operator()(SymbolTableEntry const* const x, SymbolTableEntry const* const y) const
    {
        return x == y;
    }
};

class VariableUseCheckerState
{
  public:
    struct Val
    {
        bool _isInitialized;
        bool _isUsed;
        Val()
          : _isInitialized(false)
          , _isUsed(false)
        {}
    };

  private:
    typedef std::tr1::unordered_map<SymbolTableEntry const*, Val, symHash, symEquals> Map;
    Map _map;

  public:
    VariableUseCheckerState() {}
    void add(SymbolTableEntry const& sym)
    {
        assert(!has(sym));
        _map[&sym] = Val();
    }
    VariableUseCheckerState& clone() const
    {
        VariableUseCheckerState& result = *new VariableUseCheckerState();
        for (Map::const_iterator i = _map.begin(); i != _map.end(); i++) {
            result.add(*i->first);
            result.put(*i->first, i->second);
        }
        return result;
    }
    Val const& get(SymbolTableEntry const& sym) const
    {
        assert(has(sym));
        return _map.find(&sym)->second;
    }
    bool has(SymbolTableEntry const& sym) const { return _map.end() != _map.find(&sym); }
    static VariableUseCheckerState& meet(VariableUseCheckerState& x, VariableUseCheckerState& y)
    {
        VariableUseCheckerState& result = x.clone();
        for (Map::iterator i = y._map.begin(); i != y._map.end(); i++) {
            SymbolTableEntry const& sym = *i->first;
            Val val = i->second;
            if (result.has(sym)) {
                val._isInitialized &= result.get(sym)._isInitialized;
                val._isUsed |= result.get(sym)._isUsed;
            } else {
                result.add(sym);
            }
            result.put(sym, val);
        }
        return result;
    }
    void put(SymbolTableEntry const& sym, Val val)
    {
        assert(has(sym));
        _map[&sym] = val;
    }
    std::ostream& print(std::ostream& os) const
    {
        os << "VariableUseCheckerState_" << ((unsigned long long)this) << "(";
        bool isFirst = true;
        for (Map::const_iterator i = _map.begin(); i != _map.end(); i++) {
            if (!isFirst) {
                os << ", ";
            }
            isFirst = false;
            os << i->first->name() << ": " << (i->second._isInitialized ? "I" : "-")
               << (i->second._isUsed ? "U" : "-");
        }
        return os << ")";
    }
    void remove(SymbolTableEntry const& sym)
    {
        int nErased __attribute__((__unused__)) = _map.erase(&sym);
        assert(1 == nErased);
    }
};

std::ostream& operator<<(std::ostream& os, VariableUseCheckerState const& state)
{
    return state.print(os);
}

VariableUseChecker::VariableUseChecker(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceVariableUseChecker)
  , _state(new VariableUseCheckerState())
{}

VariableUseChecker::~VariableUseChecker()
{
    delete _state;
}

void VariableUseChecker::enterScope(AstNode& ast)
{
    Scope* scope = astSymbol(ast).heldScope();
    assert(NULL != scope);
    for (int i = 0, n = scope->size(); i < n; i++) {
        SymbolTableEntry& sym = scope->get(i);
        if (sym.is<AttributeAssignSym>() || sym.is<VariableSym>()) {
            _state->add(sym);
        }
    }
}

void VariableUseChecker::exitScope(AstNode& ast)
{
    Scope* scope = astSymbol(ast).heldScope();
    assert(NULL != scope);
    for (int i = 0, n = scope->size(); i < n; i++) {
        SymbolTableEntry& sym = scope->get(i);
        if (sym.is<AttributeAssignSym>() || sym.is<VariableSym>()) {
            if (_state->has(sym)) {
                VariableUseCheckerState::Val val = _state->get(sym);
                if (!val._isUsed && !sym.is<AttributeAssignSym>()) {
                    SysOut::warnln(SPL_FRONT_END_DEAD_VARIABLE(sym.name()), sym.sourceLocation());
                }
                _state->remove(sym);
            }
        }
    }
}

void VariableUseChecker::visitExprDef(AstNode& ast)
{
    if (AST::idT::is(ast)) {
        SymbolTableEntry& sym = astSymbol(ast);
        if (_state->has(sym)) {
            VariableUseCheckerState::Val val = _state->get(sym);
            val._isInitialized = true;
            _state->put(sym, val);
        }
    } else {
        visit(ast);
    }
}

void VariableUseChecker::visitExprUse(AstNode& ast, bool mustBeValue)
{
    if (AST::idT::is(ast)) {
        SymbolTableEntry* sym = astSymbolPtr(ast);
        if (sym && _state->has(*sym)) {
            VariableUseCheckerState::Val val = _state->get(*sym);
            val._isUsed = true;
            if (!val._isInitialized) {
                SysOut::errorln(SPL_FRONT_END_UNINITIALIZED_VARIABLE(sym->name()),
                                SourceLocationFactory::instance().create(_pCtx, ast));
            }
            _state->put(*sym, val);
        }
    } else if (AST::qualifierExpr::is(ast)) {
        SymbolTableEntry* sym = astSymbolPtr(ast);
        if (sym && mustBeValue && !symIsValue(*sym)) {
            SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
            std::string name = sym->name();
            SysOut::errorln(SPL_FRONT_END_VALUE_NAME_EXPECTED(name), loc);
            resetAstSymbol(AstPool::instance(), ast,
                           *new ErrorDummySym(loc, ast, name, &_symTab.currentScope()));
        }
    } else {
        visit(ast);
    }
}

void VariableUseChecker::visit(AstNode& ast)
{
    if (astStage(ast) < FrontEndStage::VARIABLE_USE_CHECKER) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::VARIABLE_USE_CHECKER);
        AstVisitorPushPop::visit(ast);
    }
}

// ----- The compilation unit is the start symbol of the SPL grammar.
void VariableUseChecker::visitNamespaceName(AstNode& ast)
{
    /* skip (the identifiers in there are not relevant for this checker) */
}

// ----- Streams are defined in a composite operator's graph clause.
void VariableUseChecker::visitOnProcessLogic(AstNode& ast)
{
    enterScope(ast);
    AstVisitorPushPop::visitOnProcessLogic(ast);
    exitScope(ast);
}

void VariableUseChecker::visitOnTupleLogic(AstNode& ast)
{
    enterScope(ast);
    AstVisitorPushPop::visitOnTupleLogic(ast);
    exitScope(ast);
}

void VariableUseChecker::visitOnPunctLogic(AstNode& ast)
{
    enterScope(ast);
    AstVisitorPushPop::visitOnPunctLogic(ast);
    exitScope(ast);
}

#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    void VariableUseChecker::visitOnWatermarkLogic(AstNode & ast)
    {
        enterScope(ast);
        AstVisitorPushPop::visitOnWatermarkLogic(ast);
        exitScope(ast);
    }
#endif

void VariableUseChecker::visitOpInvokeOutput(AstNode& ast)
{
    AstVisitorPushPop::visitOpInvokeOutput(ast);
    _symTab.pushOld(ast);
    OpInvokeOutputSym& sym = astSymbol(ast).as<OpInvokeOutputSym>();
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        sym.switchRight();
        visitExprUse(AST::opInvokeOutput::expr(ast, i), true);
        sym.switchLeft();
    }
    _symTab.pop(ast);
}

// ----- Functions are defined at the top-level in a namespace.
void VariableUseChecker::visitFunctionDef(AstNode& ast)
{
    enterScope(ast);
    AstVisitorPushPop::visitFunctionDef(ast);
    exitScope(ast);
}

void VariableUseChecker::visitFunctionFormal(AstNode& ast)
{
    visitExprDef(AST::functionFormal::id(ast));
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void VariableUseChecker::visitLocalDeclItem(AstNode& ast)
{
    AstNode& initAst = AST::localDeclItem::expr_q(ast);
    AstNode& idAst = AST::localDeclItem::id(ast);
    if (!AST::noneT::is(initAst)) {
        visitExprUse(initAst, true);
        visitExprDef(idAst);
    }
}

void VariableUseChecker::visitBlockStmt(AstNode& ast)
{
    enterScope(ast);
    AstVisitorPushPop::visitBlockStmt(ast);
    exitScope(ast);
}

void VariableUseChecker::visitExprStmt(AstNode& ast)
{
    visitExprUse(AST::exprStmt::expr(ast), true);
}

void VariableUseChecker::visitIfStmt(AstNode& ast)
{
    visitExprUse(AST::ifStmt::expr(ast), true);
    VariableUseCheckerState* elseState = &_state->clone();
    visit(AST::ifStmt::stmt(ast));
    VariableUseCheckerState* thenState = _state;
    if (AST::ifStmt::has_elseClause(ast)) {
        _state = elseState;
        visit(AST::ifStmt::elseClause(ast));
        elseState = _state;
    }
    _state = &VariableUseCheckerState::meet(*thenState, *elseState);
    delete thenState;
    delete elseState;
}

void VariableUseChecker::visitForStmt(AstNode& ast)
{
    AstNode& initAst = AST::forStmt::forInit(ast);
    visit(AST::forInit::modifiers(initAst));
    visit(AST::forInit::type(initAst));
    VariableUseCheckerState* oldState = &_state->clone();
    visitExprUse(AST::forInit::expr(initAst), true);
    _symTab.pushOld(ast);
    enterScope(ast);
    visitExprDef(AST::forInit::id(initAst));
    visit(AST::forStmt::stmt(ast));
    exitScope(ast);
    _symTab.pop(ast);
    VariableUseCheckerState* newState = _state;
    _state = &VariableUseCheckerState::meet(*oldState, *newState);
    delete oldState;
    delete newState;
}

void VariableUseChecker::visitWhileStmt(AstNode& ast)
{
    visitExprUse(AST::whileStmt::expr(ast), true);
    VariableUseCheckerState* oldState = &_state->clone();
    visit(AST::whileStmt::stmt(ast));
    VariableUseCheckerState* newState = _state;
    _state = &VariableUseCheckerState::meet(*oldState, *newState);
    delete oldState;
    delete newState;
}

void VariableUseChecker::visitReturnStmt(AstNode& ast)
{
    if (AST::returnStmt::has_expr(ast)) {
        visitExprUse(AST::returnStmt::expr(ast), true);
    }
}

// ----- Expressions can appear in many places in the grammar.
void VariableUseChecker::visitMapping(AstNode& ast)
{
    visitExprUse(AST::mapping::fromExpr(ast), true);
    visitExprUse(AST::mapping::toExpr(ast), true);
}

void VariableUseChecker::visitAttributeExpr(AstNode& ast)
{
    visitExprUse(AST::attributeExpr::base(ast), false);
}

void VariableUseChecker::visitCastExpr(AstNode& ast)
{
    visit(AST::castExpr::type(ast));
    visitExprUse(AST::castExpr::expr(ast), true);
}

void VariableUseChecker::visitCallExpr(AstNode& ast)
{
    for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
        visitExprUse(AST::callExpr::actual(ast, i), true);
    }
}

void VariableUseChecker::visitConditionalExpr(AstNode& ast)
{
    visitExprUse(AST::conditionalExpr::condExpr(ast), true);
    visitExprUse(AST::conditionalExpr::thenExpr(ast), true);
    visitExprUse(AST::conditionalExpr::elseExpr(ast), true);
}

void VariableUseChecker::visitInfixExpr(AstNode& ast)
{
    if (Expression::eq == astNodeOp(ast)) {
        visitExprUse(AST::infixExpr::rhs(ast), true);
        visitExprDef(AST::infixExpr::lhs(ast));
    } else {
        visitExprUse(AST::infixExpr::lhs(ast), true);
        visitExprUse(AST::infixExpr::rhs(ast), true);
    }
}

void VariableUseChecker::visitParenthesizedExpr(AstNode& ast)
{
    visitExprUse(AST::parenthesizedExpr::expr(ast), true);
}

void VariableUseChecker::visitPostfixExpr(AstNode& ast)
{
    visitExprUse(AST::postfixExpr::expr(ast), true);
}

void VariableUseChecker::visitPrefixExpr(AstNode& ast)
{
    visitExprUse(AST::prefixExpr::expr(ast), true);
}

void VariableUseChecker::visitSubscriptExpr(AstNode& ast)
{
    visitExprUse(AST::subscriptExpr::expr(ast), true);
    visitExprUse(AST::subscriptExpr::subscript(ast), true);
}

void VariableUseChecker::visitSlice(AstNode& ast)
{
    visitExprUse(AST::subscriptSlice::lower(ast), true);
    visitExprUse(AST::subscriptSlice::upper(ast), true);
}

// ----- Literals are the highest-precedence expressions denoting values.
void VariableUseChecker::visitListLiteral(AstNode& ast)
{
    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        visitExprUse(AST::listLiteral::expr(ast, i), true);
    }
}

void VariableUseChecker::visitTupleLiteral(AstNode& ast)
{
    enterScope(ast);
    AstVisitorPushPop::visitTupleLiteral(ast);
    exitScope(ast);
}

void VariableUseChecker::visitAttributeAssign(AstNode& ast)
{
    visitExprUse(AST::attributeAssign::expr(ast), true);
    visitExprDef(AST::attributeAssign::id(ast));
}

void VariableUseChecker::visitSetLiteral(AstNode& ast)
{
    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        visitExprUse(AST::setLiteral::expr(ast, i), true);
    }
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void VariableUseChecker::visitTypeDims(AstNode& ast)
{
    visitExprUse(AST::typeDims::expr(ast), true);
}

void VariableUseChecker::visitIsPresentExpr(AstNode& ast)
{
    visitExprUse(AST::isPresentExpr::lhs(ast), true);
}

void VariableUseChecker::visitUnwrapExpr(AstNode& ast)
{
    visitExprUse(AST::unwrapExpr::lhs(ast), true);
}

void VariableUseChecker::visitUnwrapOrElseExpr(AstNode& ast)
{
    visitExprUse(AST::unwrapOrElseExpr::lhs(ast), true);
    visitExprUse(AST::unwrapOrElseExpr::rhs(ast), true);
}
};
