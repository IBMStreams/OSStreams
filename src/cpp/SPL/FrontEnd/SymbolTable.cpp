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

#include <SPL/FrontEnd/SymbolTable.h>

#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SysOut.h>

#include <boost/filesystem/path.hpp>
#include <cassert>
#include <fstream>
#include <iostream>

namespace SPL {
template class Singleton<SymbolTable>;
// explicitly instantiate the singleton here

bool symIsAttribute(SymbolTableEntry const& sym)
{
    if (sym.is<ErrorDummySym>() || sym.is<AttributeAccessSym>() || sym.is<AttributeDeclSym>()) {
        return true;
    }
    if (sym.is<IndirectSym>()) {
        return symIsAttribute(sym.as<IndirectSym>().firstSym());
    }
    return false;
}

static SymbolTableEntry& findRootSymbol(SymbolTableEntry& sym)
{
    if (sym.is<ExpressionSym>()) {
        AstNode& ast = sym.ast();
        if (AST::subscriptExpr::is(ast)) {
            SymbolTableEntry& base = astSymbol(AST::subscriptExpr::expr(ast));
            return findRootSymbol(base);
        }
    }
    // Anything I don't know about
    return sym;
}

bool symIsMutable(SymbolTableEntry const& sym, std::vector<AstNode*>& loopNestingContext)
{
    if (sym.is<UnwrapReferenceSym>()) {
        return symIsMutable(sym.as<UnwrapReferenceSym>().getLhsSymbol(), loopNestingContext);
    }
    if (sym.is<AttributeAccessSym>()) {
        return symIsMutable(sym.as<AttributeAccessSym>().base(), loopNestingContext);
    }
    if (sym.is<ErrorDummySym>()) {
        return true;
    }
    if (sym.is<CallExpressionSym>()) {
        return false;
    }
    for (int i = 0, n = loopNestingContext.size(); i < n; i++) {
        AstNode& loopAst = *loopNestingContext.at(i);
        if (AST::forStmt::is(loopAst)) {
            SymbolTableEntry& collSym =
              findRootSymbol(astSymbol(AST::forInit::expr(AST::forStmt::forInit(loopAst))));
            if (&sym == &collSym) {
                return false;
            }
        }
    }
    if (sym.is<FunctionFormalSym>()) {
        return sym.as<FunctionFormalSym>().isMutable();
    }
    if (sym.is<HostPoolSym>()) {
        return true; // because initialization looks like assignment
    }
    if (sym.is<VariableSym>()) {
        return sym.as<VariableSym>().isMutable();
    }
    if (sym.is<IndirectSym>()) {
        IndirectSym const& ind = sym.as<IndirectSym>();
        if (ind.isAmbiguous()) {
            return true;
        }
        assert(NULL != ind.base());
        return symIsMutable(*ind.base(), loopNestingContext);
    }
    if (sym.is<EnumValueSym>()) {
        return false; // enums can't be assigned to
    }
    if (sym.is<CompositeInputPortSym>() || sym.is<PortAliasSym>() || sym.is<StreamSym>()) {
        return true; // legality of mutating an incoming tuple checked by IRValidator
    }

    // If symbol hasAst returns false then we are returning the symbol as mutable
    // which is not quite correct. However, returning false would lead to a compile error so
    // this is a workaround for now till we figure out a better approach.
    if (sym.hasAst()) {
        AstNode& ast = sym.ast();
        if (AST::subscriptExpr::is(ast)) {
            if (AST::subscriptSlice::is(AST::subscriptExpr::subscript(ast))) {
                return false;
            }
            SymbolTableEntry& base = astSymbol(AST::subscriptExpr::expr(ast));
            switch (base.type().getMetaType()) {
                case MetaType::RSTRING:
                case MetaType::BSTRING:
                case MetaType::USTRING:
                    return false;
                default:
                    return symIsMutable(base, loopNestingContext);
            }
        }
    } else {
        return true;
    }
    return false;
}

bool symIsStream(SymbolTableEntry const& sym)
{
    return sym.is<CompositeInputPortSym>() || sym.is<PortAliasSym>() || sym.is<StreamSym>();
}

bool symIsTupleType(SymbolTableEntry const& sym)
{
    if (symIsStream(sym) || sym.is<TupleAttribSym>() || sym.is<TupleExtendSym>()) {
        return true;
    }
    if (sym.is<DefTypeSym>()) {
        return MetaType::UNKNOWN == sym.type().getMetaType() ||
               MetaType::TUPLE == sym.type().getMetaType();
    }
    if (sym.is<CompositeFormalSym>()) {
        return AST::typeT::is(
                 AST::expressionMode::name(AST::compositeFormal::expressionMode(sym.ast()))) &&
               (MetaType::UNKNOWN == sym.type().getMetaType() ||
                MetaType::TUPLE == sym.type().getMetaType());
    }
    if (sym.is<AttributeAccessSym>()) {
        return symIsTupleType(sym.as<AttributeAccessSym>().attrib());
    }
    return false;
}

bool symIsType(SymbolTableEntry const& sym)
{
    if (symIsStream(sym)) {
        return true;
    }
    if (sym.is<EnumTypeSym>() || sym.is<ListTypeSym>() || sym.is<MapTypeSym>() ||
        sym.is<RStringTypeSym>() || sym.is<SetTypeSym>() || sym.is<SimpleTypeSym>() ||
        sym.is<TupleAttribSym>() || sym.is<TupleExtendSym>()) {
        return true;
    }
    if (sym.is<DefTypeSym>() || sym.is<TypeFormalSym>()) {
        return true;
    }
    if (sym.is<CompositeFormalSym>()) {
        return AST::typeT::is(
          AST::expressionMode::name(AST::compositeFormal::expressionMode(sym.ast())));
    }
    if (sym.is<AttributeAccessSym>()) {
        return symIsType(sym.as<AttributeAccessSym>().attrib());
    }
    return false;
}

bool symIsValue(SymbolTableEntry const& sym)
{
    if (sym.is<UnwrapReferenceSym>()) {
        return symIsValue(sym.as<UnwrapReferenceSym>().getLhsSymbol());
    }
    if (sym.is<AttributeDeclSym>() || sym.is<AttributeFromTypeSym>() || sym.is<BoundsFormalSym>() ||
        sym.is<CallExpressionSym>() || sym.is<EnumValueSym>() || sym.is<ErrorDummySym>() ||
        sym.is<ExpressionSym>() || sym.is<FormalConfigSym>() || sym.is<FunctionFormalSym>() ||
        sym.is<HostPoolSym>() || sym.is<OtherSym>() || sym.is<TupleLiteralSym>() ||
        sym.is<VariableSym>()) {
        return true;
    }
    if (sym.is<DefTypeSym>() || sym.is<EnumTypeSym>() || sym.is<ListTypeSym>() ||
        sym.is<MapTypeSym>() || sym.is<OpInvokeSym>() || sym.is<OperatorSym>() ||
        sym.is<OverloadedFunctionsSym>() || sym.is<RStringTypeSym>() || sym.is<SetTypeSym>() ||
        sym.is<SimpleTypeSym>() || sym.is<TupleAttribSym>() || sym.is<TupleExtendSym>() ||
        sym.is<TypeFormalSym>()) {
        return false;
    }
    if (sym.is<CompositeFormalSym>()) {
        AstNode& expressionMode = sym.as<CompositeFormalSym>().expressionMode();
        return AST::attributeT::is(expressionMode) || AST::expressionT::is(expressionMode);
    }
    if (sym.is<AttributeAccessSym>()) {
        return symIsValue(sym.as<AttributeAccessSym>().attrib());
    }
    if (sym.is<IndirectSym>()) {
        return symIsValue(sym.as<IndirectSym>().firstSym());
    }
    if (symIsStream(sym)) {
        for (Scope* scope = &SymbolTable::instance().currentScope(); scope != NULL;
             scope = scope->parentScope()) {
            SymbolTableEntry const* holder = scope->holder();
            if (NULL != holder) {
                bool isFound = false, isInput = false;
                if (holder->is<OpInvokeOutputSym>() || holder->is<OpInvokeActualSym>() ||
                    holder->is<OpInvokeWindowSym>() || holder->is<ActualConfigSym>()) {
                    isFound =
                      findStreamInOpInvoke(sym.name(), scope->parentScope()->holder()->ast(), NULL,
                                           &isInput, NULL, NULL);
                    if (holder->is<ActualConfigSym>() && holder->name() == "streamViewability") {
                        return isFound && !isInput;
                    }
                    return isFound && isInput;
                } else if (holder->is<OnTupleLogicSym>()) {
                    bool isFound2 = false, isInput2 = false;
                    int portIdx = -1, portIdx2 = -1;
                    isFound =
                      findStreamInOpInvoke(sym.name(), scope->parentScope()->holder()->ast(), NULL,
                                           &isInput, &portIdx, NULL);
                    isFound2 =
                      findStreamInOpInvoke(holder->name(), scope->parentScope()->holder()->ast(),
                                           NULL, &isInput2, &portIdx2, NULL);
                    return (!isFound2 || (isFound && (!isInput || portIdx == portIdx2)));
                }
#if 0
                                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                                        else if (holder->is<OnPunctLogicSym>() || holder->is<OnProcessLogicSym>() || holder->is<OnWatermarkLogicSym>()) {
#else
                else if (holder->is<OnPunctLogicSym>() || holder->is<OnProcessLogicSym>()) {
#endif
                isFound = findStreamInOpInvoke(sym.name(), scope->parentScope()->holder()->ast(),
                                               NULL, &isInput, NULL, NULL);
                return isFound && !isInput;
            }
        }
    }
    return false;
}
std::cerr << "should never get here: symIsValue called on " << sym << std::endl;
assert(false);
return false;
}

static void ensureIsOperatorOrEnum(ParserContext const& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameLookup);
    SymbolTableEntry& sym = astSymbol(ast);
    bool isOp = (sym.is<OperatorSym>() ||
                 (sym.is<CompositeFormalSym>() &&
                  AST::operatorT::is(sym.as<CompositeFormalSym>().expressionMode())));
    bool isEnum = (sym.is<DefTypeSym>() && sym.type().getMetaType().getValue() == MetaType::ENUM);
    if (!sym.is<ErrorDummySym>() && !isOp && !isEnum) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        resetAstSymbol(
          AstPool::instance(), ast,
          *new ErrorDummySym(loc, ast, sym.name(), &SymbolTable::instance().currentScope()));
        SysOut::errorln(SPL_FRONT_END_OPERATOR_OR_ENUM_NAME_EXPECTED(sym.name()),
                        SourceLocationFactory::instance().create(pCtx, ast));
    }
}

static void ensureIsOperator(ParserContext const& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameLookup);
    SymbolTableEntry& sym = astSymbol(ast);
    bool isOp = (sym.is<OperatorSym>() ||
                 (sym.is<CompositeFormalSym>() &&
                  AST::operatorT::is(sym.as<CompositeFormalSym>().expressionMode())));
    if (!sym.is<ErrorDummySym>() && !isOp) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        resetAstSymbol(
          AstPool::instance(), ast,
          *new ErrorDummySym(loc, ast, sym.name(), &SymbolTable::instance().currentScope()));
        SysOut::errorln(SPL_FRONT_END_OPERATOR_NAME_EXPECTED(sym.name()),
                        SourceLocationFactory::instance().create(pCtx, ast));
    }
}

static void ensureIsStream(ParserContext const& pCtx, AstNode& ast)
{
    SymbolTableEntry& sym = astSymbol(ast);
    if (!sym.is<ErrorDummySym>() && !symIsStream(sym)) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        resetAstSymbol(
          AstPool::instance(), ast,
          *new ErrorDummySym(loc, ast, sym.name(), &SymbolTable::instance().currentScope()));
        SysOut::errorln(SPL_FRONT_END_STREAM_NAME_EXPECTED(sym.name()),
                        SourceLocationFactory::instance().create(pCtx, ast));
    }
}

static void ensureIsType(ParserContext const& pCtx, AstNode& ast)
{
    SymbolTableEntry& sym = astSymbol(ast);
    if (!sym.is<ErrorDummySym>() && !symIsType(sym)) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        resetAstSymbol(
          AstPool::instance(), ast,
          *new ErrorDummySym(loc, ast, sym.name(), &SymbolTable::instance().currentScope()));
        SysOut::errorln(SPL_FRONT_END_TYPE_NAME_EXPECTED(sym.name()), loc);
    }
}

bool isInSubscriptionOrFilterExpression(Scope& currentScope)
{
    if (currentScope.holder() != NULL && currentScope.holder()->is<OpInvokeActualSym>()) {
        OpInvokeActualSym const& actual = currentScope.holder()->as<OpInvokeActualSym>();
        std::string label = actual.name();
        if (actual.formal() != NULL && actual.formal()->is<PrimitiveFormalSym>() &&
            (label == "subscription" || label == "filter")) {
            PrimitiveFormalSym& pf = actual.formal()->as<PrimitiveFormalSym>();
            std::string nameSpaceName = pf.op().parentScope()->parentScope()->holder()->name();
            if (nameSpaceName == "spl.adapter" && pf.op().name() == "Import") {
                if (label == "subscription" || label == "filter") {
                    return true;
                }
            }
        }
    }
    return false;
}

static void ensureIsValue(ParserContext const& pCtx, AstNode& ast)
{
    SymbolTableEntry& sym = astSymbol(ast);
    if (!sym.is<ErrorDummySym>() && !symIsValue(sym)) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        resetAstSymbol(
          AstPool::instance(), ast,
          *new ErrorDummySym(loc, ast, astText(ast), &SymbolTable::instance().currentScope()));
        if (!isInSubscriptionOrFilterExpression(SymbolTable::instance().currentScope())) {
            SysOut::errorln(SPL_FRONT_END_VALUE_NAME_EXPECTED(sym.name()),
                            SourceLocationFactory::instance().create(pCtx, ast));
        }
    }
}

// SymbolTable -----------------------------------------------------
SymbolTable::SymbolTable()
  : Singleton<SymbolTable>(*this)
  , _commonCompositeDefScope(NULL)
  , _commonWindowScope(NULL)
  , _configLabelScope(NULL)
  , _currentScope(new Scope(NULL, NULL))
  , _excursionStack()
  , _logicScopeCurrentPunct(NULL)
  ,
#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            _logicScopeCurrentWatermark(NULL),
#endif
  _logicScopeSubmit(NULL)
  , _topLevelScope(_currentScope)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("GlobalScope(" << (void*)_topLevelScope << ")\n");
    SPLDBG("Top scope is " << (void*)_currentScope, Debug::TraceScopeStack);
    initCommonCompositeDefScope();
    initCommonWindowScope();
    initConfigLabelScope();
    initLogicScopes();
    initIntrinsicScope();
}

void SymbolTable::checkVisibilityComposite(ParserContext const& pCtx,
                                           AstNode& ast,
                                           CompositeDefSym& oper)
{
    if (!oper.isPublic()) {
        std::string tgtNamespace = currentNamespace().name();
        std::string srcNamespace =
          oper.parentScope()->parentScope()->holder()->as<NamespaceSym>().name();
        if (srcNamespace != tgtNamespace) {
            SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
            SysOut::errorln(
              SPL_FRONT_END_COMPOSITE_NOT_VISIBLE(oper.name(), srcNamespace, tgtNamespace), loc);
        }
    }
}

CompilationUnitSym const* SymbolTable::compilationUnit() const
{
    Scope const* scope = compilationUnitScope();
    if (NULL == scope) {
        return NULL;
    }
    return &scope->holder()->as<CompilationUnitSym>();
}

Scope* SymbolTable::compilationUnitScope() const
{
    Scope* top = _topLevelScope;
    for (Scope* s = _currentScope; s != top; s = s->parentScope()) {
        const SymbolTableEntry* entry = s->holder();
        if (NULL != entry && entry->is<CompilationUnitSym>()) {
            assert(top == s->parentScope()->parentScope());
            return s;
        }
    }
    return NULL;
}

Scope& SymbolTable::configScope(std::string const& label) const
{
    Scope* result = _configLabelScope->get(label).heldScope();
    assert(NULL != result);
    return *result;
}

CompositeDefSym const& SymbolTable::currentCompositeDef() const
{
    for (Scope *s = _currentScope, *top = _topLevelScope; s != top; s = s->parentScope()) {
        SymbolTableEntry const* sym = s->holder();
        if (NULL != sym && sym->is<CompositeDefSym>()) {
            return sym->as<CompositeDefSym>();
        }
    }
    assert(false);
    return *((CompositeDefSym*)NULL);
}

CompositeInstanceSym const& SymbolTable::currentCompositeInstance() const
{
    for (Scope *s = _currentScope, *top = _topLevelScope; s != top; s = s->parentScope()) {
        SymbolTableEntry const* sym = s->holder();
        if (NULL != sym && sym->is<CompositeInstanceSym>()) {
            return sym->as<CompositeInstanceSym>();
        }
    }
    assert(false);
    return *((CompositeInstanceSym*)NULL);
}

NamespaceSym& SymbolTable::currentNamespace() const
{
    for (Scope *s = _currentScope, *top = _topLevelScope; s != top; s = s->parentScope()) {
        SymbolTableEntry* sym = s->holder();
        if (NULL != sym && sym->is<NamespaceSym>()) {
            return sym->as<NamespaceSym>();
        }
    }
    assert(false);
    return *((NamespaceSym*)NULL);
}

OpInvokeSym const& SymbolTable::currentOpInvoke() const
{
    for (Scope *s = _currentScope, *top = _topLevelScope; s != top; s = s->parentScope()) {
        SymbolTableEntry const* sym = s->holder();
        if (NULL != sym && sym->is<OpInvokeSym>()) {
            return sym->as<OpInvokeSym>();
        }
    }
    assert(false);
    return *((OpInvokeSym*)NULL);
}

CompilationUnitSym& SymbolTable::defCompilationUnit(ParserContext const& pCtx, AstNode* ast)
{
    assert(_topLevelScope == _currentScope->parentScope());
    std::string name = pCtx.fileName();
    if (_currentScope->has(name)) {
        return _currentScope->get(name).as<CompilationUnitSym>();
    }
    SourceLocation const& loc =
      SourceLocationFactory::instance().create(name, StringSourceLocation::NOSTRING, 1, 0);
    return _currentScope->def(*new CompilationUnitSym(loc, _currentScope, ast))
      .as<CompilationUnitSym>();
}

void SymbolTable::defDefType(ParserContext const& pCtx, AstNode& ast)
{
    _currentScope->def(
      *new DefTypeSym(SourceLocationFactory::instance().create(pCtx, ast), ast, _currentScope));
}

FunctionFormalSym& SymbolTable::defFunctionFormal(ParserContext const& pCtx, AstNode& ast)
{
    SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
    FunctionFormalSym& sym = *new FunctionFormalSym(loc, _currentScope, ast);
    if (sym.hasName()) {
        _currentScope->def(sym);
    }
    return sym;
}

NamespaceSym& SymbolTable::defNamespace(std::string name)
{
    assert(_currentScope == _topLevelScope);
    if (_currentScope->has(name)) {
        return _currentScope->get(name).as<NamespaceSym>();
    }
    return _currentScope->def(*new NamespaceSym(name, _currentScope)).as<NamespaceSym>();
}

void SymbolTable::defStream(ParserContext const& pCtx, AstNode& ast)
{
    std::string const localStreamName = astText(AST::opOutput::id(ast));
    std::string const fullStreamName = currentCompositeInstance().fullStreamName(localStreamName);
    std::string const instanceName = currentCompositeInstance().fullName();
    Scope* scope = _currentScope->parentScope();
    assert(NULL != scope);
    scope->def(*new StreamSym(
      SourceLocationFactory::instance().create(pCtx, ast), ast, fullStreamName,
      instanceName.empty() ? localStreamName : instanceName + '.' + localStreamName));
}

void SymbolTable::defPortAlias(ParserContext const& pCtx, AstNode& ast)
{
    _currentScope->def(*new PortAliasSym(SourceLocationFactory::instance().create(pCtx, ast), ast));
}

void SymbolTable::defVariable(ParserContext const& pCtx, AstNode& defAst, AstNode& idAst)
{
    assert(NULL != _currentScope);
    _currentScope->def(*new VariableSym(SourceLocationFactory::instance().create(pCtx, idAst),
                                        defAst, idAst, *_currentScope));
}

Scope& SymbolTable::excursionBegin(Scope& excursionNew)
{
    assert(NULL != _currentScope);
    Scope& excursionOld = *_currentScope;
    _currentScope = &excursionNew;
    SPLDBG("Excursion start: scope set to " << (void*)_currentScope, Debug::TraceScopeStack);
    MODEL("SetCurrentScope(" << (void*)_currentScope << ")\n");
    _excursionStack.push_back(&excursionOld);
    _excursionStack.push_back(&excursionNew);
    return excursionOld;
}

void SymbolTable::excursionEnd(Scope& excursionOld)
{
    assert(_currentScope == _excursionStack.back());
    _excursionStack.pop_back();
    _currentScope = &excursionOld;
    SPLDBG("Excursion end: scope set to " << (void*)_currentScope, Debug::TraceScopeStack);
    MODEL("SetCurrentScope(" << (void*)_currentScope << ")\n");
    assert(_currentScope == _excursionStack.back());
    _excursionStack.pop_back();
}

FunctionHeadSym* SymbolTable::function() const
{
    Scope const* scope = functionScope();
    FunctionHeadSym* result = (NULL == scope) ? NULL : &scope->holder()->as<FunctionHeadSym>();
    assert((NULL == scope) == (NULL == result));
    return result;
}

Scope* SymbolTable::functionScope() const
{
    for (Scope* s = _currentScope; s != _topLevelScope; s = s->parentScope()) {
        const SymbolTableEntry* sym = s->holder();
        if (NULL != sym && sym->is<FunctionHeadSym>()) {
            return s;
        }
    }
    return NULL;
}

void SymbolTable::initCommonCompositeDefScope()
{
    SourceLocation const& loc = SourceLocation::invalidSourceLocation;
    OtherScopeHolderSym& sym = *new OtherScopeHolderSym(loc, NULL, _currentScope);
    push(NULL, sym);
    _commonCompositeDefScope = _currentScope;
    // INTRINSIC: Value specified for 'name' at job submission, error if missing. See Section
    // 'Submission-Time Values' in Compiler Usage Reference.
    FunctionHeadSym::analyzeIntrinsicFunction(
      _commonCompositeDefScope, "rstring getSubmissionTimeValue(rstring name)", false);
    // INTRINSIC: Value specified for 'name' at job submission, 'default' if missing. See Section
    // 'Submission-Time Values' in Compiler Usage Reference.
    FunctionHeadSym::analyzeIntrinsicFunction(
      _commonCompositeDefScope, "rstring getSubmissionTimeValue(rstring name, rstring default)",
      false);
    // INTRINSIC: List of values specified for 'name' at job submission, error if missing. See
    // Section 'Submission-Time Values' in Compiler Usage Reference.
    FunctionHeadSym::analyzeIntrinsicFunction(
      _commonCompositeDefScope, "list<rstring> getSubmissionTimeListValue(rstring name)", false);
    // INTRINSIC: List of values specified for 'name' at job submission, 'default' if missing. See
    // Section 'Submission-Time Values' in Compiler Usage Reference.
    FunctionHeadSym::analyzeIntrinsicFunction(
      _commonCompositeDefScope,
      "list<rstring> getSubmissionTimeListValue(rstring name, list<rstring> default)", false);
    // INTRINSIC: Name of composite operator instance containing the call. See Section 'Compiler
    // Intrinsic Functions' in Compiler Usage Reference.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonCompositeDefScope,
                                              "rstring getThisCompositeInstanceName()", false);

    FunctionHeadSym::analyzeIntrinsicFunction(_commonCompositeDefScope, "rstring byChannel()",
                                              false);
    FunctionHeadSym::analyzeIntrinsicFunction(_commonCompositeDefScope, "rstring byReplica()",
                                              false);
    pop(sym);
}

void SymbolTable::initCommonWindowScope()
{
    SourceLocation const& loc = SourceLocation::invalidSourceLocation;
    OtherScopeHolderSym& sym = *new OtherScopeHolderSym(loc, NULL, _currentScope);
    push(NULL, sym);
    _commonWindowScope = _currentScope;

    EnumTyp::Values values;
    values.push_back("sliding");
    values.push_back("tumbling");
    values.push_back("timeInterval");
    values.push_back("partitioned");
    const EnumTyp& eType = TypeFactory::instance().enumT(values);
    addEnumConstantsToScope(*_commonWindowScope, eType);

    // INTRINSIC: Count-based window for 'n' tuples. See Section 'Window Clause' in Language
    // Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void count(int32 n)", false);
    // INTRINSIC: Count-based window for 'n' tuples. See Section 'Window Clause' in Language
    // Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void count(uint32 n)", false);
    // INTRINSIC: Time-based window for tuples arriving over a period of 'n' seconds. See Section
    // 'Window Clause' in Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void time(int32 n)", false);
    // INTRINSIC: Time-based window for tuples arriving over a period of 'n' seconds. See Section
    // 'Window Clause' in Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void time(uint32 n)", false);
    // INTRINSIC: Time-based window for tuples arriving over a period of 'n' seconds. See Section
    // 'Window Clause' in Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void time(float64 n)", false);
    // INTRINSIC: Attribute-delta based window for tuples whose 'a' attribute differs by at most
    // 'n'. See Section 'Window Clause' in Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope,
                                              "void delta(timestamp a, float64 n)", false);
    // INTRINSIC: Attribute-delta based window for tuples whose 'a' attribute differs by at most
    // 'n'. See Section 'Window Clause' in Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope,
                                              "<numeric T> void delta(T a, T n)", false);
    // INTRINSIC: Punctuation-based window. See Section 'Window Clause' in Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void punct()", false);
    // INTRINSIC: Partition eviction if more than 'n' tuples. See Section 'Window Clause' in
    // Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void tupleCount(int32 n)",
                                              false);
    // INTRINSIC: Partition eviction if more than 'n' tuples. See Section 'Window Clause' in
    // Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void tupleCount(uint32 n)",
                                              false);
    // INTRINSIC: Partition eviction if more than 'n' partitions. See Section 'Window Clause' in
    // Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void partitionCount(int32 n)",
                                              false);
    // INTRINSIC: Partition eviction if more than 'n' partitions. See Section 'Window Clause' in
    // Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void partitionCount(uint32 n)",
                                              false);
    // INTRINSIC: Partition eviction if a partition is older than 'age'. See Section 'Window Clause'
    // in Language Specification.
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void partitionAge(float64 age)",
                                              false);
    // INTRINSIC: define the interval duration for an event-time window
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope,
                                              "void intervalDuration(float64 n)", false);
    // INTRINSIC: define the creation period for an event-time window
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void creationPeriod(float64 n)",
                                              false);
    // INTRINSIC: define the discard age for an event-time window
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void discardAge(float64 n)",
                                              false);
    // INTRINSIC: define the interval offset for an event-time window
    FunctionHeadSym::analyzeIntrinsicFunction(_commonWindowScope, "void intervalOffset(float64 n)",
                                              false);
    pop(sym);
}

void SymbolTable::initConfigLabelScope()
{
    assert(NULL == _configLabelScope);
    _configLabelScope = new Scope(_currentScope, NULL);
    // configs on operator invocation or operator definition
    {
        FormalConfigSym& cfg =
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "checkpoint"))
            .as<FormalConfigSym>();
        cfg.heldScope()->def(*new OtherSym(SourceLocation::invalidSourceLocation, "operatorDriven",
                                           cfg.heldScope(), NULL,
                                           &TypeFactory::instance().primitiveT(MetaType::BOOLEAN)));
        push(NULL, cfg);
        // INTRINSIC: Interval 'seconds' at which operator saves its state for fault recovery. See
        // Section 'checkpoint' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(_currentScope, "void periodic(float64 seconds)",
                                                  false);
        pop(cfg);
    }
    _configLabelScope->def(*new FormalConfigSym(_currentScope, "defaultPoolSize"))
      .as<FormalConfigSym>();
    {
        FormalConfigSym& cfg =
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "hostPool"))
            .as<FormalConfigSym>();
        push(NULL, cfg);

        // RTC 822 createPool functions
        // INTRINSIC: List of host names for given size, tags, and sharing requirements. See Section
        // 'hostPool' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _currentScope,
          "<tuple T> list<rstring> createPool(T sizeAndTags, enum{Shared,Exclusive} sharing)",
          false);
        // INTRINSIC: List of host names for given sharing requirements. See Section 'hostPool' in
        // Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _currentScope, "list<rstring> createPool(enum{Shared,Exclusive} sharing)", false);
        pop(cfg);
    }
    {
        const char* s[] = { "TCP", NULL };
        initScope(
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "inputTransport")).heldScope(),
          s);
    }
    {
        const char* s[] = { "error", "info", "debug", "trace", NULL };
        initScope(
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "logLevel")).heldScope(), s);
    }
    {
        const char* s[] = { "off", "error", "warn", "info", "debug", "trace", NULL };
        initScope(
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "tracing")).heldScope(), s);
    }
    {
        const char* s[] = { "TCP", NULL };
        initScope(_configLabelScope->def(*new FormalConfigSym(_currentScope, "outputTransport"))
                    .heldScope(),
                  s);
    }
    {
        const char* s[] = { "hostIsolation", "partitionIsolation", NULL };
        FormalConfigSym& cfg =
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "placement"))
            .as<FormalConfigSym>();
        initScope(cfg.heldScope(), s);
        push(NULL, cfg);
        // INTRINSIC: Place operator on host for given 'addressOrName'. See Section 'placement :
        // host' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(_currentScope, "void host(rstring addressOrName)",
                                                  false);
        // INTRINSIC: Place operator on one of the hosts for given 'addressesOrNames'. See Section
        // 'placement : host' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _currentScope, "void host(list<rstring> addressesOrNames)", false);
        // INTRINSIC: Place all operators with same 'id' on same host. See Section 'placement :
        // hostColocation' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(_currentScope, "void hostColocation(rstring id)",
                                                  false);
        // INTRINSIC: Place all operators with same 'id' on different hosts. See Section 'placement
        // : hostExlocation' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(_currentScope, "void hostExlocation(rstring id)",
                                                  false);
        // INTRINSIC: Place all operators with same 'id' in same partition. See Section 'placement :
        // partitionColocation' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(_currentScope,
                                                  "void partitionColocation(rstring id)", false);
        // INTRINSIC: Place all operators with same 'id' in different partitions. See Section
        // 'placement : partitionExlocation' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(_currentScope,
                                                  "void partitionExlocation(rstring id)", false);

        pop(cfg);
    }
    _configLabelScope->def(*new FormalConfigSym(_currentScope, "relocatable"));
    _configLabelScope->def(*new FormalConfigSym(_currentScope, "restartable"));
    _configLabelScope->def(*new FormalConfigSym(_currentScope, "applicationScope"));
    {
        FormalConfigSym& cfg =
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "threadedPort"))
            .as<FormalConfigSym>();
        push(NULL, cfg);
        // INTRINSIC: Keep tuples arriving at 'port' on a queue with given 'congestionPolicy'. See
        // Section 'threadedPort' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _currentScope,
          "<tuple T> void queue(T port, enum {Wait, DropFirst, DropLast} congestionPolicy)", false);
        // INTRINSIC: Keep tuples arriving at 'port' on a queue with given 'congestionPolicy' and
        // 'size'. See Section 'threadedPort' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _currentScope,
          "<tuple T> void queue(T port, enum {Wait, DropFirst, DropLast} congestionPolicy, int32 "
          "size)",
          false);
        pop(cfg);
    }
    {
        const char* s[] = { "none", "nrgdb", "gdb", "sdb", "valgrind", "console", NULL };
        initScope(
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "wrapper")).heldScope(), s);
    }
    {
        FormalConfigSym& cfg =
          _configLabelScope->def(*new FormalConfigSym(_currentScope, "streamViewability"))
            .as<FormalConfigSym>();
        push(NULL, cfg);
        // INTRINSIC: Control stream visibility for data visualization. See Section
        // 'streamViewability' in Config Reference.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _currentScope, "<tuple T> void viewable(T outputPort, boolean isViewable)", false);
        pop(cfg);
    }
}

void SymbolTable::initLogicScopes()
{
    {
        OtherScopeHolderSym& scopeHolderSym =
          *new OtherScopeHolderSym(SourceLocation::invalidSourceLocation, NULL, _currentScope);
        push(NULL, scopeHolderSym);
        _logicScopeCurrentPunct = _currentScope;
        // INTRINSIC: Punctuation that triggered the 'onPunct' clause. See Section 'Logic Clause' in
        // Language Specification.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _logicScopeCurrentPunct, "enum{WindowMarker,FinalMarker} currentPunct()", false);
        pop(scopeHolderSym);
    }
#if 0
                // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                {
			OtherScopeHolderSym & scopeHolderSym = *new OtherScopeHolderSym(SourceLocation::invalidSourceLocation, NULL,
					_currentScope);
			push(NULL, scopeHolderSym);
                        _logicScopeCurrentWatermark = _currentScope;
			//INTRINSIC: Punctuation that triggered the 'onWatermark' clause. See Section 'Logic Clause' in Language Specification.
			FunctionHeadSym::analyzeIntrinsicFunction(_logicScopeCurrentWatermark,
                                        "timestamp currentWatermark()", false);
			pop(scopeHolderSym);
                }
#endif
    {
        OtherScopeHolderSym& scopeHolderSym =
          *new OtherScopeHolderSym(SourceLocation::invalidSourceLocation, NULL, _currentScope);
        push(NULL, scopeHolderSym);
        _logicScopeSubmit = _currentScope;
        // INTRINSIC: Submit 'punctuation' on 'port'. See Section 'Logic Clause' in Language
        // Specification.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _logicScopeSubmit,
          "<tuple T> void submit(enum{WindowMarker,FinalMarker} punctuation, T port)", false);
        FunctionHeadSym::analyzeIntrinsicFunction(
          _logicScopeSubmit,
          "<tuple T> void submit(enum{WindowMarker,FinalMarker} punctuation, uint32 port)", false);
        // INTRINSIC: Submit 'tupleValue' on 'port'. See Section 'Logic Clause' in Language
        // Specification.
        FunctionHeadSym::analyzeIntrinsicFunction(
          _logicScopeSubmit, "<tuple T> void submit(T tupleValue, T port)", false);
        FunctionHeadSym::analyzeIntrinsicFunction(
          _logicScopeSubmit, "<tuple T> void submit(T tupleValue, uint32 port)", false);
#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        // INTRINSIC: submit 'watermark' on 'port'.
			FunctionHeadSym::analyzeIntrinsicFunction(_logicScopeSubmit, "void submit(timestamp watermark, uint32 port)", false);
			FunctionHeadSym::analyzeIntrinsicFunction(_logicScopeSubmit, "<tuple T> void submit(timestamp watermark, T port)", false);
			// INTRINSIC: submit 'watermark' on all ports.
			FunctionHeadSym::analyzeIntrinsicFunction(_logicScopeSubmit, "void submit(timestamp watermark)", false);
#endif
#if 0
			//INTRINSIC: getEventTime for a tuple
			FunctionHeadSym::analyzeIntrinsicFunction(_logicScopeSubmit, "<tuple T> timestamp getEventTime(T tupleValue)", false);
#endif
        pop(scopeHolderSym);
    }
}

void SymbolTable::initIntrinsicScope()
{
    {
        NamespaceSym& intrinsicNamespaceSym =
          *new NamespaceSym("<default_namespace>", _currentScope);
        push(NULL, intrinsicNamespaceSym);
        OtherScopeHolderSym& scopeHolderSym = *new OtherScopeHolderSym(
          SourceLocation::invalidSourceLocation, NULL, intrinsicNamespaceSym.heldScope());
        push(NULL, scopeHolderSym);
        _intrinsicScope = scopeHolderSym.heldScope();
        // INTRINSIC: assignPayload used in expansion of assignFrom function
        FunctionHeadSym::analyzeHiddenFunction(
          _intrinsicScope, "<tuple T1, tuple T2> void assignPayload(T1 t1, T2 t2)", false, "SPL");
        _getToolkitDirectorySym = &FunctionHeadSym::analyzeIntrinsicFunction(
          _intrinsicScope, "rstring getToolkitDirectory(rstring toolkitName)", false);
        pop(scopeHolderSym);
        pop(intrinsicNamespaceSym);
    }
}

void SymbolTable::initScope(Scope* scope, const char** names)
{
    for (int i = 0; NULL != names[i]; i++) {
        std::string name = names[i];
        scope->def(*new OtherSym(SourceLocation::invalidSourceLocation, name, scope, NULL, NULL));
    }
}

bool SymbolTable::isTypeId(AstNode& ast) const
{
    return _currentScope->get(astText(ast)).is<DefTypeSym>();
}

void SymbolTable::pop(AstNode& ast)
{
    pop(astSymbol(ast));
}

void SymbolTable::pop(SymbolTableEntry& sym)
{
    if (sym.is<EnumTypeSym>() || sym.is<TupleAttribSym>() || sym.is<TupleExtendSym>() ||
        sym.is<TupleLiteralSym>()) {
        excursionEnd(*_excursionStack.at(_excursionStack.size() - 2));
    } else {
        if (sym.heldScope() != _currentScope) {
            std::cerr << "sym " << sym << ", scope " << _currentScope << std::endl;
        }
        assert(sym.heldScope() == _currentScope);
        _currentScope = _currentScope->parentScope();
        SPLDBG("Scope pop: set to " << (void*)_currentScope, Debug::TraceScopeStack);
        MODEL("SetCurrentScope(" << (void*)_currentScope << ")\n");
    }
    assert(NULL != _currentScope);
}

void SymbolTable::push(AstNode* astNode, SymbolTableEntry& sym)
{
    if (NULL != astNode) {
        resetAstSymbol(AstPool::instance(), *astNode, sym);
    }
    if (sym.is<EnumTypeSym>() || sym.is<TupleAttribSym>() || sym.is<TupleExtendSym>() ||
        sym.is<TupleLiteralSym>()) {
        excursionBegin(*sym.heldScope());
    } else {
        Scope* oldScope = _currentScope;
        _currentScope = sym.heldScope();
        SPLDBG("Scope push: set to " << (void*)_currentScope, Debug::TraceScopeStack);
        MODEL("SetCurrentScope(" << (void*)_currentScope << ")\n");
        if (oldScope != _currentScope->parentScope()) {
            std::cerr << "oldScope " << oldScope << ", parentScope " << _currentScope->parentScope()
                      << std::endl;
        }
        assert(oldScope == _currentScope->parentScope());
    }
    assert(NULL != _currentScope);
}

CompilationUnitSym& SymbolTable::pushCompilationUnit(ParserContext const& pCtx, AstNode& ast)
{
    CompilationUnitSym& sym = defCompilationUnit(pCtx, &ast);
    push(&ast, sym);
    return sym;
}

CompilationUnitSym& SymbolTable::pushCompilationUnit(ParserContext const& pCtx)
{
    AstNode* astNode = NULL;
    CompilationUnitSym& sym = defCompilationUnit(pCtx, astNode);
    push(astNode, sym);
    return sym;
}

FunctionHeadSym& SymbolTable::pushFunction(ParserContext const& pCtx, AstNode& ast)
{
    SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
    FunctionHeadSym& proto = *new FunctionHeadSym(loc, ast, _currentScope);
    // it's too early now to add the proto to the overloaded function group,
    // must wait until after type analysis to check for overlap
    push(&ast, proto);
    return proto;
}

NamespaceSym& SymbolTable::pushNamespace(std::string name)
{
    NamespaceSym& sym = defNamespace(name);
    push(NULL, sym);
    return sym;
}

NamespaceSym& SymbolTable::pushNamespace(AstNode& ast)
{
    NamespaceSym& sym = defNamespace(NamespaceSym::name(ast));
    push(&ast, sym);
    return sym;
}

OtherScopeHolderSym& SymbolTable::pushNew(ParserContext const& pCtx, AstNode& ast)
{
    OtherScopeHolderSym& sym = *new OtherScopeHolderSym(
      SourceLocationFactory::instance().create(pCtx, ast), &ast, _currentScope);
    push(&ast, sym);
    return sym;
}

void SymbolTable::pushOld(AstNode& ast)
{
    push(&ast, astSymbol(ast));
}

void SymbolTable::pushOpInvoke(ParserContext const& pCtx, AstNode& ast)
{
    push(&ast,
         *new OpInvokeSym(SourceLocationFactory::instance().create(pCtx, ast), ast, _currentScope));
}

void SymbolTable::pushTupleAttrib(ParserContext const& pCtx, AstNode& ast)
{
    push(&ast, *new TupleAttribSym(SourceLocationFactory::instance().create(pCtx, ast), ast,
                                   _currentScope));
}

Scope& SymbolTable::topLevelScope() const
{
    assert(NULL != _topLevelScope);
    return *_topLevelScope;
}

void SymbolTable::useConfig(ParserContext const& pCtx, AstNode& ast)
{
    Scope* oldScope = _currentScope;
    _currentScope = _configLabelScope;
    SPLDBG("Config scope set to " << (void*)_currentScope, Debug::TraceScopeStack);
    MODEL("SetCurrentScope(" << (void*)_currentScope << ")\n");
    useSimpleId(pCtx, ast, false);
    _currentScope = oldScope;
    SPLDBG("Config scope reset to " << (void*)_currentScope, Debug::TraceScopeStack);
    MODEL("SetCurrentScope(" << (void*)_currentScope << ")\n");
}

SymbolTableEntry* SymbolTable::useInNamespace(std::string const& nameSpaceName,
                                              std::string const& simpleName)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Checking for name " << simpleName << " in namespace "
                               << nameSpaceName,
           Debug::TraceNameLookup);
    SymbolTableEntry* result = useInNamespaceFromSymbolTable(nameSpaceName, simpleName);
    if (NULL == result) {
        PathSearch& pathSearch = PathSearch::instance();
        bool isDefaultNS = SymbolTableEntry::isDefaultNameSpace(nameSpaceName);
        PathSearch::NonFunctionResult const* opResult =
          pathSearch.findNonFunction(isDefaultNS ? "" : nameSpaceName, simpleName);
        if (NULL != opResult) {
            Scope& oldScope = excursionBegin(topLevelScope());
            boost::filesystem::path fileName(opResult->getPath());
            if (opResult->isPrimitiveOperator()) {
                fileName /= simpleName + ".xml";
                NamespaceSym& nsSym = pushNamespace(nameSpaceName);
                std::string fullName =
                  isDefaultNS ? simpleName : (nameSpaceName + "::" + simpleName);
                Operator::OperatorModel const* opModel =
                  pathSearch.findOperatorModel(fullName, NULL);
                assert(NULL != opModel);
                ParserContext pCtx(fileName.string());
                CompilationUnitSym& cuSym = pushCompilationUnit(pCtx);
                result = new PrimitiveOperatorSym(*opModel, simpleName, _currentScope,
                                                  opResult->getToolkitIndex());
                _currentScope->def(*result);
                pop(cuSym);
                pop(nsSym);
            } else {
                FrontEndDriver::instance().newCompilationUnit(fileName.string(),
                                                              opResult->getToolkitIndex());
                result = useInNamespaceFromSymbolTable(nameSpaceName, simpleName);
            }
            excursionEnd(oldScope);
        }
    }
    if (NULL != result) {
        SPLDBG(__PRETTY_FUNCTION__ << ":" << simpleName << " found in namespace " << nameSpaceName,
               Debug::TraceNameLookup);
    }
    return result;
}

SymbolTableEntry* SymbolTable::useInNamespaceFromSymbolTable(std::string const& nameSpaceName,
                                                             std::string const& simpleName)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Checking for name " << simpleName << " in namespace  "
                               << nameSpaceName,
           Debug::TraceNameLookup);
    if (!_topLevelScope->has(nameSpaceName)) {
        return NULL;
    }
    SymbolTableEntry& nameSpace = _topLevelScope->get(nameSpaceName);
    Scope* nameSpaceScope = nameSpace.as<NamespaceSym>().heldScope();
    assert(NULL != nameSpaceScope);
    for (int i = 0, n = nameSpaceScope->size(); i < n; i++) {
        SymbolTableEntry& symInNameSpace = nameSpaceScope->get(i);
        if (symInNameSpace.is<CompilationUnitSym>()) {
            CompilationUnitSym& compUnit = symInNameSpace.as<CompilationUnitSym>();
            if (compUnit.heldScope()->has(simpleName)) {
                SPLDBG(__PRETTY_FUNCTION__ << ":" << simpleName << " found in namespace "
                                           << nameSpaceName,
                       Debug::TraceNameLookup);
                return &compUnit.heldScope()->get(simpleName);
            }
        }
    }
    return NULL;
}

SymbolTableEntry* checkParentForShadowingStateVariable(SymbolTableEntry& result, Scope& scope)
{
    const std::string& name = result.name();
    if (scope.has(name)) {
        SymbolTableEntry& sym = scope.get(name);
        if (sym.is<VariableSym>()) {
            return &sym;
        }
    }
    SymbolTableEntry* holder = scope.holder();
    // Don't look outside of the current operator invocation for a shadow
    if (NULL != holder && holder->is<OpInvokeSym>()) {
        return NULL;
    }
    if (NULL != scope.parentScope()) {
        return checkParentForShadowingStateVariable(result, *scope.parentScope());
    }

    return NULL;
}

SymbolTableEntry* SymbolTable::useInner(ParserContext const& pCtx,
                                        AstNode& ast,
                                        bool checkForShadow)
{
    std::string name = astText(ast);
    // the following steps are equivalent to the steps in the constructor of CallExpressionSym
    // (1) lookup in current scope or enclosing scopes in symbol table
    for (Scope* s = _currentScope; NULL != s; s = s->parentScope()) {
        SPLDBG(__PRETTY_FUNCTION__ << ": Checking for " << name << " in scope " << (void*)s,
               Debug::TraceNameLookup);
        if (s->has(name)) {
            SymbolTableEntry& result = s->get(name);
            // We need to check that, if we have found an indirect sym, that there is no state
            // variable with the same name Walk up the scopes until we hit the OpInvoke scope and
            // see if we have the same name
            bool emitError = true;
            CompilerConfiguration const& config = CompilerConfiguration::instance();
            if (config.isSet(CompilerConfiguration::StateVarScopeBehavior)) {
                if ("old" == config.getStringValue(CompilerConfiguration::StateVarScopeBehavior)) {
                    checkForShadow = false;
                } else {
                    emitError = false;
                }
            }

            if (checkForShadow && result.is<IndirectSym>() && NULL != s->parentScope()) {
                SymbolTableEntry* stateSym =
                  checkParentForShadowingStateVariable(result, *s->parentScope());
                if (NULL != stateSym) {
                    if (emitError) {
                        SourceLocation const& loc =
                          SourceLocationFactory::instance().create(pCtx, ast);
                        SysOut::errorln(SPL_FRONT_END_STATE_VAR_SHADOWS_ATTRIBUTE(name), loc);
                    }
                    return stateSym;
                }
            }
            if (!result.is<NamespaceSym>()) {
                SPLDBG(__PRETTY_FUNCTION__ << ": Name " << name << "(" << (void*)&result
                                           << ") found in scope " << (void*)s,
                       Debug::TraceNameLookup);
                return &result;
            }
            break;
        }
    }
    CompilationUnitSym const* cu = compilationUnit();
    // (2) lookup across namespaces based on named use directives
    if (NULL != cu) {
        SPLDBG(__PRETTY_FUNCTION__ << ": Checking for " << name << " in compilationUnit namedUses",
               Debug::TraceNameLookup);
        CompilationUnitSym::String2String::const_iterator i = cu->namedUses().find(name);
        if (cu->namedUses().end() != i) {
            SymbolTableEntry* sym = useInNamespace(i->second, name);
            if (NULL != sym) {
                SPLDBG(__PRETTY_FUNCTION__ << ": Name " << name << " found in namedUses",
                       Debug::TraceNameLookup);
                return sym;
            }
        }
        // It is possible that the name being sought is an enumerator.  Check all the named uses to
        // see if any represent an enumeration and if so, is this name a member of that enumeration?
        for (i = cu->namedUses().begin(); i != cu->namedUses().end(); ++i) {
            SymbolTableEntry* sym = useInNamespace(i->second, i->first);
            if (NULL != sym) {
                if (sym->is<DefTypeSym>()) {
                    DefTypeSym& typeDef = sym->as<DefTypeSym>();
                    AstNode& td = typeDef.ast();
                    AstNode& tail = AST::typeDef::typeDefTail(td);
                    if (AST::enumType::is(tail)) {
                        SymbolTableEntry& eSym = astSymbol(tail);
                        assert(eSym.is<EnumTypeSym>());
                        EnumTypeSym& enumSym = eSym.as<EnumTypeSym>();
                        if (enumSym.heldScope()->has(name)) {
                            SymbolTableEntry& evSym = enumSym.heldScope()->get(name);
                            SPLDBG(__PRETTY_FUNCTION__ << ": Name " << name
                                                       << " found in namedUses by evaluating enums",
                                   Debug::TraceNameLookup);
                            return &evSym;
                        }
                    }
                }
            }
        }
    }
    // (3) lookup in sibling compilation units for current namespace
    if (NULL != cu) {
        SymbolTableEntry* sym = useInNamespace(cu->parentScope()->holder()->name(), name);
        if (NULL != sym) {
            SPLDBG(__PRETTY_FUNCTION__ << ": Name " << name << "(" << (void*)sym
                                       << ") found in sibling compilation",
                   Debug::TraceNameLookup);
            return sym;
        }
    }

    // (4) lookup across namespaces based on wildcard use directives and standard namespaces
    SPLDBG(__PRETTY_FUNCTION__ << ": Checking for " << name << " across namespaces",
           Debug::TraceNameLookup);
    CompilationUnitSym::StringSet wu;
    if (NULL != cu) {
        wu.insert(cu->wildcardUses().begin(), cu->wildcardUses().end());
    }
    wu.insert(CompilationUnitSym::implicitUses().begin(), CompilationUnitSym::implicitUses().end());
    std::vector<std::string> found;
    SymbolTableEntry* sym = NULL;
    for (CompilationUnitSym::StringSet::const_iterator i = wu.begin(), n = wu.end(); i != n; i++) {
        SymbolTableEntry* s = useInNamespace(*i, name);
        if (NULL != s) {
            found.push_back(*i);
            if (NULL == sym) {
                sym = s;
            }
        }
    }
    if (2 <= found.size()) {
        std::ostringstream os;
        for (int i = 0, n = found.size(); i < n; i++) {
            if (0 != i) {
                os << ", ";
            }
            os << '\'' << found.at(i) << '\'';
        }
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        SysOut::errorln(SPL_FRONT_END_AMBIGUOUS_ID_NAMESPACES(name, os.str()), loc);
    }
    if (NULL != sym) {
        SPLDBG(__PRETTY_FUNCTION__ << ": Name " << name << "(" << (void*)sym << ") found",
               Debug::TraceNameLookup);
    }
    return sym;
}

void SymbolTable::useOpActualName(ParserContext const& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameLookup);
    SymbolTableEntry* formal = NULL;
    for (Scope* s = _currentScope; s != NULL; s = s->parentScope()) {
        SymbolTableEntry* holder = const_cast<SymbolTableEntry*>(s->holder());
        if (holder->is<OpInvokeActualSym>()) {
            formal = holder->as<OpInvokeActualSym>().formal();
            break;
        }
        if (holder->is<CompositeFormalSym>()) {
            formal = holder;
            break;
        }
    }
    AstNode& nameAst = AST::opActualName::name(ast);
    if (NULL != formal && formal->is<CompositeFormalSym>() &&
        AST::functionT::is(formal->as<CompositeFormalSym>().expressionMode())) {
        if (AST::idT::is(nameAst) || AST::qualifierExpr::is(nameAst)) {
            setAstSymbol(AstPool::instance(), nameAst,
                         *formal); // hint for constructor of CallExpressionSym to resolve in scope
                                   // of invocation
        } else {
            SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, nameAst);
            SysOut::errorln(SPL_FRONT_END_EXPRESSION_MODE_VIOLATION(formal->name(), "function"),
                            loc);
            resetAstSymbol(AstPool::instance(), nameAst,
                           *new ErrorDummySym(loc, ast, formal->name(), _currentScope));
        }
    } else {
        if (AST::idT::is(nameAst)) {
            useSimpleId(pCtx, nameAst, true);
        } else if (AST::qualifierExpr::is(nameAst)) {
            useQualifiedId(pCtx, nameAst);
        } else {
            assert(AST::attributeExpr::is(nameAst));
            AstNode& baseAst = AST::attributeExpr::base(nameAst);
            if (AST::idT::is(baseAst)) {
                useSimpleId(pCtx, baseAst, true);
            } else {
                useQualifiedOperatorOrEnum(pCtx, baseAst);
            }
        }
    }
}

void SymbolTable::useParam(ParserContext const& pCtx, AstNode& actualAst)
{
    OperatorSym* opEntry = currentOpInvoke().op();
    if (NULL == opEntry) {
        return;
    }
    std::string actualName = astText(actualAst);
    std::string formalName = std::string("$") + actualName;
    Scope* scope = opEntry->paramsScope();
    assert(NULL != scope);
    if (!scope->has(formalName)) {
        PrimitiveOperatorSym* prim =
          opEntry->is<PrimitiveOperatorSym>() ? &opEntry->as<PrimitiveOperatorSym>() : NULL;
        if (NULL == prim || !prim->model().getParameters().allowArbitraryParameters()) {
            SysOut::errorln(SPL_FRONT_END_UNKNOWN_OPERATOR_PARAMETER(actualName, opEntry->name()),
                            SourceLocationFactory::instance().create(pCtx, actualAst));
        }
    } else {
        SymbolTableEntry& formal = scope->get(formalName);
        astSymbol(actualAst).as<OpInvokeActualSym>().setFormal(&formal);
    }
}

void SymbolTable::useQualifiedId(ParserContext const& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameLookup);
    std::ostringstream os;
    prettyPrint(os, AST::qualifierExpr::namespaceName(ast));
    std::string nameSpaceName = os.str();
    std::string simpleName = astText(AST::qualifierExpr::id(ast));
    SymbolTableEntry* sym = useInNamespace(nameSpaceName, simpleName);
    if (NULL == sym) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        SysOut::errorln(SPL_FRONT_END_UNKNOWN_IDENTIFIER((nameSpaceName + "::" + simpleName)), loc);
        sym = new ErrorDummySym(loc, ast, simpleName, _currentScope);
    }
    if (sym->is<CompositeDefSym>()) {
        checkVisibilityComposite(pCtx, ast, sym->as<CompositeDefSym>());
    }
    setAstSymbol(AstPool::instance(), ast, *sym);
}

void SymbolTable::useQualifiedOperatorOrEnum(ParserContext const& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameLookup);
    useQualifiedId(pCtx, ast);
    ensureIsOperatorOrEnum(pCtx, ast);
}

void SymbolTable::useQualifiedOperator(ParserContext const& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameLookup);
    useQualifiedId(pCtx, ast);
    ensureIsOperator(pCtx, ast);
}

void SymbolTable::useQualifiedType(ParserContext const& pCtx, AstNode& ast)
{
    useQualifiedId(pCtx, ast);
    ensureIsType(pCtx, ast);
}

void SymbolTable::useSimpleId(ParserContext const& pCtx, AstNode& ast, bool checkForShadow)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameLookup);
    SymbolTableEntry* sym = useInner(pCtx, ast, checkForShadow);
    if (NULL == sym) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        if (!isInSubscriptionOrFilterExpression(currentScope())) {
            SysOut::errorln(SPL_FRONT_END_UNKNOWN_IDENTIFIER(astText(ast)), loc);
        }
        sym = new ErrorDummySym(loc, ast, astText(ast), _currentScope);
    }
    if (sym->is<IndirectSym>()) {
        IndirectSym& indirect = sym->as<IndirectSym>();
        if (indirect.isAmbiguous()) {
            SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
            SysOut::errorln(
              SPL_FRONT_END_AMBIGUOUS_ID_PORTSENUMS(astText(ast), indirect.toString()), loc);
        }
        // Check that if we are replacing an indirect that it has the same type
        SymbolTableEntry* oldSym = astSymbolPtr(ast);
        if (oldSym && oldSym->is<IndirectSym>()) {
            IndirectSym& oldIndirect = oldSym->as<IndirectSym>();
            SymbolTableEntry* oldBase = oldIndirect.base();
            SymbolTableEntry* newBase = indirect.base();
            if (NULL != oldBase && NULL != newBase) {
                const RootTyp& oldType = oldBase->type();
                const RootTyp& newType = newBase->type();
                if (oldType != newType) {
                    SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
                    std::ostringstream os1, os2;
                    os1 << newType;
                    os2 << oldType;
                    SysOut::errorln(SPL_FRONT_END_STATE_INCORRECT_ATTRIBUTE_TYPE(
                                      indirect.name(), os2.str(), os1.str()),
                                    loc);
                }
            }
        }
    }
    if (sym->is<CompositeDefSym>()) {
        checkVisibilityComposite(pCtx, ast, sym->as<CompositeDefSym>());
    }
    resetAstSymbol(AstPool::instance(), ast, *sym);
}

void SymbolTable::useSimpleIdIfDollar(ParserContext const& pCtx, AstNode& ast)
{
    std::string name = astText(ast);
    if ('$' != name.at(0)) {
        return;
    }
    SymbolTableEntry* sym = NULL;
    for (Scope* s = _currentScope; NULL != s; s = s->parentScope()) {
        if (s->has(name)) {
            sym = &s->get(name);
            break;
        }
    }
    if (NULL == sym) {
        SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
        SysOut::errorln(SPL_FRONT_END_UNKNOWN_IDENTIFIER(name), loc);
        sym = new ErrorDummySym(loc, ast, name, _currentScope);
    }
    resetAstSymbol(AstPool::instance(), ast, *sym);
}

void SymbolTable::checkForMissingSymbol(ParserContext const& pCtx,
                                        AstNode& ast,
                                        SymbolTableEntry& errorSym)
{
    // See if we know about this operator somewhere
    const std::string& name = errorSym.name();
    std::tr1::unordered_set<std::string> wildCardUses;
    CompilationUnitSym const* cu = compilationUnit();
    if (NULL != cu) {
        wildCardUses.insert(cu->wildcardUses().begin(), cu->wildcardUses().end());
    }
    wildCardUses.insert(CompilationUnitSym::implicitUses().begin(),
                        CompilationUnitSym::implicitUses().end());
    PathSearch& pathSearch = PathSearch::instance();
    const std::tr1::unordered_set<std::string>& allToolkitNamespaces =
      pathSearch.toolkitNamespaces();
    std::tr1::unordered_set<std::string> foundInNamespace;
    for (std::tr1::unordered_set<std::string>::const_iterator it = allToolkitNamespaces.begin();
         it != allToolkitNamespaces.end(); it++) {
        if (it->empty()) {
            continue; // skip default namespace
        }
        if (wildCardUses.count(*it)) {
            continue; // Already looked here!
        }
        if (pathSearch.findNonFunction(*it, name)) {
            foundInNamespace.insert(*it);
        }
    }
    if (foundInNamespace.empty()) {
        return;
    }

    SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, ast);
    for (std::tr1::unordered_set<std::string>::const_iterator it = foundInNamespace.begin();
         it != foundInNamespace.end(); it++) {
        SysOut::detailsErrorln(SPL_FRONT_END_CALL_MISSING_USE_NAMESPACE(name, *it), loc);
    }
}

void SymbolTable::useSimpleOperatorId(ParserContext const& pCtx, AstNode& ast)
{
    useSimpleId(pCtx, ast, false);
    SymbolTableEntry& opSym = astSymbol(ast);
    if (opSym.is<ErrorDummySym>()) {
        checkForMissingSymbol(pCtx, ast, opSym);
    } else {
        ensureIsOperator(pCtx, ast);
    }
}

void SymbolTable::useSimpleStreamId(ParserContext const& pCtx, AstNode& ast)
{
    useSimpleId(pCtx, ast, false);
    ensureIsStream(pCtx, ast);
}

void SymbolTable::useSimpleTypeId(ParserContext const& pCtx, AstNode& ast)
{
    useSimpleId(pCtx, ast, false);
    ensureIsType(pCtx, ast);
}

void SymbolTable::useSimpleValueId(ParserContext const& pCtx, AstNode& ast)
{
    useSimpleId(pCtx, ast, true);
    ensureIsValue(pCtx, ast);
}
}
;
