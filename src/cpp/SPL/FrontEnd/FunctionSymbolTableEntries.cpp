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

#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>

#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/GenericsUnifier.h>
#include <SPL/FrontEnd/LexicalNameUseFinder.h>
#include <SPL/FrontEnd/NameDefFinder.h>
#include <SPL/FrontEnd/RelativeNameUseFinder.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SysOut.h>

#include <antlr3.h>
#include <sstream>

using namespace std;
using namespace std::tr1;
using namespace SPL;

CallExpressionSym::CallExpressionSym(SourceLocation const& loc,
                                     AstNode& inast,
                                     Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _parentScope(inparentScope)
  , _resolvedTgt(NULL)
  , _returnType(NULL)
  , _submitNeedingCopy(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    AstNode& tgtAst = AST::callExpr::target(inast);
    if (AST::qualifierExpr::is(tgtAst)) {
        _name = astText(AST::qualifierExpr::id(tgtAst));
    } else if (AST::idT::is(tgtAst)) {
        _name = astText(tgtAst);
    } else {
        // definitely not a valid call, create fake name for error message
        ostringstream os;
        printExpr(os, tgtAst);
        _name = os.str();
    }
    MODEL("CallExpression(" << (void*)this << ", " << _name << ", " << (void*)&inast << ")\n");

    setAstSymbol(AstPool::instance(), inast, *this);
    bool anyUnknownActuals = initActualTypes(inast);
    if (anyUnknownActuals) {
        _returnType = &TypeFactory::instance().unknownT();
        return;
    }
    std::vector<FunctionHeadSym*> candidates;
    SymbolTable& symTab = SymbolTable::instance();
    unordered_set<string> wildCardUses;
    if (AST::qualifierExpr::is(tgtAst)) {
        ostringstream os;
        prettyPrint(os, AST::qualifierExpr::namespaceName(tgtAst));
        string nameSpaceName = os.str();
        useFunction(nameSpaceName, inast, candidates);
    } else if (AST::idT::is(tgtAst)) {
        // the following steps are equivalent to the steps in SymbolTable::use()
        // (1) lookup in current scope or enclosing scopes in symbol table
        SymbolTableEntry* idSym = astSymbolPtr(tgtAst);
        Scope* lookupStartScope = &symTab.currentScope();
        if (NULL != idSym && idSym->is<CompositeFormalSym>()) {
            lookupStartScope = idSym->parentScope();
        }
        for (Scope* s = lookupStartScope; NULL != s; s = s->parentScope()) {
            if (s->has(_name) && match(inast, s->get(_name), candidates)) {
                break;
            }
        }
        CompilationUnitSym const* cu = symTab.compilationUnit();
        // (2) lookup based on named use directives
        if (NULL == _resolvedTgt && NULL != cu) {
            CompilationUnitSym::String2String const& nu = cu->namedUses();
            if (nu.end() != nu.find(_name)) {
                useFunction(nu.find(_name)->second, inast, candidates);
            }
        }
        // (3) lookup in sibling compilation units for current namespace
        if (NULL == _resolvedTgt && NULL != cu) {
            useFunction(cu->parentScope()->holder()->name(), inast, candidates);
        }
        // (4) lookup based on wildcard use directives and standard namespaces
        if (NULL == _resolvedTgt) {
            if (NULL != cu) {
                wildCardUses.insert(cu->wildcardUses().begin(), cu->wildcardUses().end());
            }
            wildCardUses.insert(CompilationUnitSym::implicitUses().begin(),
                                CompilationUnitSym::implicitUses().end());
            vector<string> found;
            for (unordered_set<string>::const_iterator i = wildCardUses.begin(),
                                                       n = wildCardUses.end();
                 i != n; i++) {
                FunctionHeadSym* oldResolvedTgt = _resolvedTgt;
                RootTyp const* oldReturnType = _returnType;
                _resolvedTgt = NULL;
                _returnType = NULL;
                useFunction(*i, inast, candidates);
                if (NULL != _resolvedTgt) {
                    found.push_back(*i);
                }
                if (NULL == _resolvedTgt) {
                    _resolvedTgt = oldResolvedTgt;
                    _returnType = oldReturnType;
                }
            }
            if (2 <= found.size()) {
                ostringstream os;
                for (int i = 0, n = found.size(); i < n; i++) {
                    if (0 != i) {
                        os << ", ";
                    }
                    os << '\'' << found.at(i) << '\'';
                }
                SysOut::errorln(SPL_FRONT_END_AMBIGUOUS_CALL(_name, os.str()), loc);
            }
        }
    }
    if (NULL == _resolvedTgt) {
        ostringstream os;
        printBare(os);
        if (0 == candidates.size()) {
            SysOut::errorln(SPL_FRONT_END_UNKNOWN_CALL(os.str()), loc);
            if (AST::idT::is(tgtAst) && !wildCardUses.empty()) {
                // Look through namespaces that we know about that aren't in the 'use' list
                PathSearch& pathSearch = PathSearch::instance();
                const unordered_set<string>& allToolkitNamespaces = pathSearch.toolkitNamespaces();
                unordered_set<string> foundInNamespace;
                for (unordered_set<string>::const_iterator it = allToolkitNamespaces.begin();
                     it != allToolkitNamespaces.end(); it++) {
                    if (it->empty()) {
                        continue; // skip default namespace
                    }
                    if (wildCardUses.count(*it)) {
                        continue; // Already looked here!
                    }
                    _resolvedTgt = NULL;
                    _returnType = NULL;
                    useFunction(*it, inast, candidates);
                    if (NULL != _resolvedTgt) {
                        foundInNamespace.insert(*it);
                    }
                }
                for (unordered_set<string>::const_iterator it = foundInNamespace.begin();
                     it != foundInNamespace.end(); it++) {
                    SysOut::detailsErrorln(SPL_FRONT_END_CALL_MISSING_USE_NAMESPACE(_name, *it),
                                           loc);
                }
            }
        } else {
            SysOut::errorln(SPL_FRONT_END_CALL_NO_MATCH(os.str(), candidates.size()), loc);
            for (int i = 0, n = candidates.size(); i < n; i++) {
                FunctionHeadSym& candidate = *candidates.at(i);
                if (candidate.isIntrinsic()) {
                    SysOut::detailsErrorln(
                      SPL_FRONT_END_CALL_CANDIDATE_INTRINSIC(candidate.toString()),
                      SourceLocation::invalidSourceLocation);
                } else {
                    SysOut::detailsErrorln(SPL_FRONT_END_CALL_CANDIDATE_FUNCTION(
                                             candidate.toString(), candidate.nameSpace()),
                                           candidate.isNativeFunction()
                                             ? SourceLocation::invalidSourceLocation
                                             : candidate.sourceLocation());
                }
            }
        }
        _returnType = &TypeFactory::instance().unknownT();
    } else {
        _resolvedTgt->markUsed();
        if (!_resolvedTgt->isIntrinsic() && !_resolvedTgt->isPublic()) {
            std::string tgtNamespace = _resolvedTgt->nameSpace();
            std::string srcNamespace = symTab.currentNamespace().name();
            if (tgtNamespace != srcNamespace) {
                SysOut::errorln(SPL_FRONT_END_FUNCTION_NOT_VISIBLE(_resolvedTgt->name(),
                                                                   tgtNamespace, srcNamespace),
                                loc);
            }
        }
    }
}

Scope* CallExpressionSym::heldScope() const
{
    return _returnType->is<TupleTyp>()
             ? new Scope(sourceLocation(), _returnType->as<TupleTyp>(), NULL)
             : NULL;
}

bool CallExpressionSym::initActualTypes(AstNode& inast)
{
    bool result = false;
    RootTyp const* const unknownType = &TypeFactory::instance().unknownT();
    int actualCount = AST::callExpr::actualCount(inast);
    for (int i = 0; i < actualCount; i++) {
        AstNode& actualAst = AST::callExpr::actual(inast, i);
        SymbolTableEntry* e = astSymbolPtr(actualAst);
        RootTyp const* t = (NULL == e) ? unknownType : &e->type();
        result |= (t == unknownType);
        _actualTypes.push_back(t);
    }
    return result;
}

bool CallExpressionSym::promotionRequiredToGetFrom(const RootTyp& fromType, const RootTyp& toType)
{

    // note: this method only is intended to be called from match() and assumes a certain amount of
    // checking has already been done on the two types to match function candidates.  As a result, a
    // lot of what normally would be error scenarios such as totally mismatching or unknown types
    // are not checked for. It's assumed that the types are close enough to have generated a
    // function match in the first place.

    if (fromType == toType) {
        return false;
    }

    if (toType.isOptionalOfSameType(fromType)) {
        return true;
    }

    if (toType.getMetaType() == MetaType::TYPEFORMAL) {
        return false;
    }

    if (fromType.is<TupleTyp>() && fromType.is<TupleTyp>()) {
        // recursively check attributes
        const TupleTyp& fromTupleTyp = fromType.as<TupleTyp>();
        const TupleTyp& toTupleTyp = toType.as<TupleTyp>();

        // loop through attributes
        for (unsigned int k = 0; k < fromTupleTyp.getNumberOfAttributes(); k++) {
            if (promotionRequiredToGetFrom(fromTupleTyp.getAttributeType(k),
                                           toTupleTyp.getAttributeType(k))) {
                return true;
            }
        }
        return false;
    }

    if (fromType.is<SetTyp>() && toType.is<SetTyp>()) {
        const SetTyp& fromSetTyp = fromType.as<SetTyp>();
        const SetTyp& toSetTyp = toType.as<SetTyp>();
        return promotionRequiredToGetFrom(fromSetTyp.getElementType(), toSetTyp.getElementType());
    }

    if (fromType.is<ListTyp>() && toType.is<ListTyp>()) {
        const ListTyp& fromListTyp = fromType.as<ListTyp>();
        const ListTyp& toListTyp = toType.as<ListTyp>();
        return promotionRequiredToGetFrom(fromListTyp.getElementType(), toListTyp.getElementType());
    }

    if (fromType.is<MapTyp>() && toType.is<MapTyp>()) {
        const MapTyp& fromMapTyp = fromType.as<MapTyp>();
        const MapTyp& toMapTyp = toType.as<MapTyp>();
        return promotionRequiredToGetFrom(fromMapTyp.getKeyType(), toMapTyp.getKeyType()) ||
               promotionRequiredToGetFrom(fromMapTyp.getValueType(), toMapTyp.getValueType());
    }

    return true;
}

bool CallExpressionSym::match(AstNode& thisAst,
                              SymbolTableEntry& tgtCandidate,
                              std::vector<FunctionHeadSym*>& candidates)
{
    assert(NULL == _resolvedTgt && NULL == _returnType);
    if (tgtCandidate.is<NamespaceSym>()) {
        return false;
    }

    std::vector<RootTyp const*> returnTypes;
    std::vector<FunctionHeadSym*> resolvedTgts;

    if (tgtCandidate.is<OverloadedFunctionsSym>()) {
        OverloadedFunctionsSym::Functions const& functions =
          tgtCandidate.as<OverloadedFunctionsSym>().functions();
        SPLDBG(__PRETTY_FUNCTION__ << ": checking " << functions.size() << " overloaded fucntions",
               Debug::TraceFunctionCallResolution);

        for (int i = 0, n = functions.size(); i < n; i++) {
            FunctionHeadSym* fh = const_cast<FunctionHeadSym*>(functions.at(i));
            SPLDBG(__PRETTY_FUNCTION__ << ": checking " << fh->toString(),
                   Debug::TraceFunctionCallResolution);
            assert(NULL != fh);
            bool triedBefore = false;
            for (int j = 0, p = candidates.size(); !triedBefore && j < p; j++) {
                triedBefore = candidates.at(j) == fh;
            }
            if (!triedBefore) {
                candidates.push_back(fh);
                auto_ptr<GenericsUnifier const> gu(&GenericsUnifier::unify(*this, *fh));
                if (!gu->isFailure()) {
                    // jin - _returnType = const_cast<RootTyp *>(&gu->substitute(fh->returnType()));
                    // jin - _resolvedTgt = fh;
                    // jin - break;
                    returnTypes.push_back(const_cast<RootTyp*>(&gu->substitute(fh->returnType())));
                    resolvedTgts.push_back(fh);
                }
            }
        }
    } else {
        SysOut::errorln(SPL_FRONT_END_INVALID_CALL(tgtCandidate.fullName()), sourceLocation());
    }

    int resolvedInd = 0;
    int resolvedSize = resolvedTgts.size();
    // checks if there is only one function header that matches this function call
    if (resolvedSize > 0) {
        _returnType = returnTypes.at(resolvedInd);
        _resolvedTgt = resolvedTgts.at(resolvedInd);
    }
    if (resolvedSize > 1) {
        // checks if there is an exact match among the matching function headers
        do {
            FunctionHeadSym::Formals const& fy = resolvedTgts.at(resolvedInd)->formals();
            int n = _actualTypes.size();
            int i;
            for (i = 0; i < n; i++) {
                if (*_actualTypes.at(i) != fy.at(i)->type()) {
                    break;
                }
            }
            if (i == n) {
                break;
            } else {
                resolvedInd++;
            }
        } while (resolvedInd < resolvedSize);

        if (resolvedInd == resolvedSize) { // do we have a definitive candidate yet?
            // all candidates are still valid up until this point
            // check to see if there is one and only one that doesn't require a non-optional actual
            // unifying with an optional if so, we prefer it
            int numNonOptionalCandidates = 0;
            int indexOfTheOne = -1;
            int currentResolvedInd = 0;
            do {
                FunctionHeadSym::Formals const& fy = resolvedTgts.at(currentResolvedInd)->formals();
                int n = _actualTypes.size();
                int i;
                for (i = 0; i < n; i++) {
                    if (promotionRequiredToGetFrom(*_actualTypes.at(i), fy.at(i)->type())) {
                        break;
                    }

                    else {
                        numNonOptionalCandidates++;
                        indexOfTheOne = i;
                    }
                }
                if (i == n) {
                    break;
                } else {
                    currentResolvedInd++;
                }
            } while (currentResolvedInd < resolvedSize);

            if (numNonOptionalCandidates == 1) {
                resolvedInd = indexOfTheOne;
            }
        }

        if (resolvedInd == resolvedSize) {
            SysOut::errorln(
              SPL_FRONT_END_CALL_MULTIPLE_MATCHES(tgtCandidate.fullName(), resolvedSize),
              sourceLocation());
            for (int i = 0, n = resolvedSize; i < n; i++) {
                FunctionHeadSym& resolvedTarget = *resolvedTgts.at(i);
                SysOut::detailsErrorln(SPL_FRONT_END_CALL_CANDIDATE_FUNCTION(
                                         resolvedTarget.toString(), resolvedTarget.nameSpace()),
                                       resolvedTarget.sourceLocation());
            }
        } else {
            _returnType = returnTypes.at(resolvedInd);
            _resolvedTgt = resolvedTgts.at(resolvedInd);
        }
    }
    assert((NULL == _resolvedTgt) == (NULL == _returnType));
    if (NULL != _resolvedTgt) {
        SPLDBG(__PRETTY_FUNCTION__ << ": call to " << _name << " resolved to "
                                   << (void*)_resolvedTgt << " return type is " << *_returnType,
               Debug::TraceFunctionCallResolution);
        resetAstSymbol(AstPool::instance(), AST::callExpr::target(thisAst), *_resolvedTgt);
    }
    return NULL != _resolvedTgt;
}

ostream& CallExpressionSym::printBare(ostream& os) const
{
    os << _name << '(';
    for (int i = 0, n = _actualTypes.size(); i < n; i++) {
        if (0 < i) {
            os << ", ";
        }
        os << _actualTypes.at(i);
    }
    return os << ')';
}

ostream& CallExpressionSym::print(ostream& os) const
{
    os << "CallExpressionSym(" << _returnType << ' ';
    printBare(os);
    return os << ')';
}

void CallExpressionSym::useFunction(std::string const& namespaceName,
                                    AstNode& inast,
                                    std::vector<FunctionHeadSym*>& candidates)
{
    SymbolTable& symTab = SymbolTable::instance();
    if (symTab.topLevelScope().has(namespaceName)) {
        NamespaceSym& ns = symTab.topLevelScope().get(namespaceName).as<NamespaceSym>();
        // overloaded functions are kept right in the namespace scope, not nested in compilation
        // units
        std::string simpleName = name();
        if (ns.heldScope()->has(simpleName)) {
            match(inast, ns.heldScope()->get(simpleName), candidates);
        }
    }
    // unlike operators, functions go into the symbol table during toolkit loading,
    // so we don't need to consult PathSearch::findFunction() anymore here
}

// .................................................................
FunctionFormalSym::FunctionFormalSym(SourceLocation const& loc,
                                     Scope* inparentScope,
                                     AstNode& inast)
  : SymbolTableEntry(loc)
  , _heldScope(NULL)
  , _isMutable(false)
  , _name("")
  , _parentScope(inparentScope)
  , _type(NULL)
{

    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    setAstSymbol(AstPool::instance(), inast, *this);
    AstNode& modifiersAst = AST::functionFormal::is(inast) ? AST::functionFormal::modifiers(inast)
                                                           : AST::protoFormal::modifiers(inast);
    for (int i = 0, n = AST::modifiers::modifierCount(modifiersAst); i < n; i++) {
        if (AST::mutableT::is(AST::modifiers::modifier(modifiersAst, i))) {
            _isMutable = true;
        }
    }
    if ((AST::protoFormal::is(inast) && AST::protoFormal::has_id(inast)) ||
        AST::functionFormal::is(inast)) {
        AstNode& idAst = AST::functionFormal::is(inast) ? AST::functionFormal::id(inast)
                                                        : AST::protoFormal::id(inast);
        _name = astText(idAst);
        setAstSymbol(AstPool::instance(), idAst, *this);
        if ('$' == _name.at(0)) {
            SourceLocation const& loc1 = SourceLocationFactory::instance().create(loc, idAst);
            SysOut::errorln(SPL_FRONT_END_LEADING_DOLLAR_ID(name()), loc1);
        }
    }

    MODEL("FunctionFormal(" << (void*)this << ", " << _name << ")\n");
}

Scope* FunctionFormalSym::heldScope() const
{
    return _heldScope = typeToScope(sourceLocation(), type(), _heldScope);
}

string FunctionFormalSym::name() const
{
    assert(hasName());
    return _name;
}

ostream& FunctionFormalSym::print(ostream& os) const
{
    os << "FunctionFormalSym(";
    os << (hasName() ? name() : "<anonymous>");
    return os << ", " << _type << ")";
}

// .................................................................
FunctionHeadSym& FunctionHeadSym::analyzeFunctionFromToolkitModel(string const& nameSpace,
                                                                  string const& source,
                                                                  string const& xmlFileName,
                                                                  string const& splFileName,
                                                                  uint32_t line,
                                                                  uint32_t column,
                                                                  string const& includeFileName,
                                                                  string const& modelBase,
                                                                  bool isNativeFunction,
                                                                  Dependencies const* deps,
                                                                  int32_t builtinNumber,
                                                                  uint32_t toolkitIndex,
                                                                  string const& cppNamespace,
                                                                  string const& cppFunctionName)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": " << cppFunctionName, Debug::TraceSymbolCreation);
    MODEL("StopCollecting()\n");
    SPLAst& sa = SPLAst::functionPrototypeFromString(
      splFileName.empty() ? xmlFileName : splFileName, line, column, source, isNativeFunction);
    SymbolTable& symTab = SymbolTable::instance();
    Scope& oldScope = symTab.excursionBegin(symTab.topLevelScope());
    NamespaceSym& nsSym =
      symTab.pushNamespace(0 == nameSpace.size() ? "<default_namespace>" : nameSpace);
    CompilationUnitSym& cuSym = symTab.pushCompilationUnit(sa.parserContext());
    NameDefFinder::run(sa.parserContext(), sa.ast());
    FunctionHeadSym& proto = astSymbol(sa.ast()).as<FunctionHeadSym>();
    proto._splAst = &sa;
    assert(NULL == proto._returnType && !proto.isIntrinsic());
    proto._builtinNumber = builtinNumber;
    proto._dependencies = deps;
    if (!isNativeFunction) {
        assert(splFileName != "");
    }
    proto._splFileName = splFileName;
    proto._cppNamespace = cppNamespace;
    proto._cppFunctionName = cppFunctionName;
    proto._includeFileName = includeFileName;
    proto._includeIsBuiltin = includeFileName == "SPLFunctions.h" &&
                              modelBase.find("SPL/toolkit/spl/native.function") != string::npos;
    proto._isNativeFunction = isNativeFunction;
    proto._modelBase = modelBase;
    proto._toolkitIndex = toolkitIndex;
    proto._isJavaFunction = (xmlFileName.find("javaFunction.xml") != string::npos);
    symTab.pop(cuSym);
    symTab.pop(nsSym);
    symTab.excursionEnd(oldScope);
    MODEL("StartCollecting()\n");
    return proto;
}

FunctionHeadSym& FunctionHeadSym::analyzeIntrinsicFunction(Scope* scope,
                                                           string const& source,
                                                           bool isNativeFunction)
{
    MODEL("StopCollecting()\n");
    CompilationUnitSym const* cuSym = SymbolTable::instance().compilationUnit();
    string const& fileName = (NULL == cuSym) ? "<intrinsic>" : cuSym->name();
    SPLAst& sa = SPLAst::functionPrototypeFromString(fileName, 0, 0, source, isNativeFunction);
    NameDefFinder::run(sa.parserContext(), sa.ast());
    FunctionHeadSym& proto = astSymbol(sa.ast()).as<FunctionHeadSym>();
    assert(NULL == proto._returnType && proto.isIntrinsic());
    proto._splAst = &sa;
    MODEL("StartCollecting()\n");
    return proto;
}

FunctionHeadSym& FunctionHeadSym::analyzeHiddenFunction(Scope* scope,
                                                        string const& source,
                                                        bool isNativeFunction,
                                                        std::string const& cppNamespace)
{
    MODEL("StopCollecting()\n");
    CompilationUnitSym const* cuSym = SymbolTable::instance().compilationUnit();
    string const& fileName = (NULL == cuSym) ? "<intrinsic>" : cuSym->name();
    SPLAst& sa = SPLAst::functionPrototypeFromString(fileName, 0, 0, source, isNativeFunction);
    NameDefFinder::run(sa.parserContext(), sa.ast());
    FunctionHeadSym& proto = astSymbol(sa.ast()).as<FunctionHeadSym>();
    assert(NULL == proto._returnType && proto.isIntrinsic());
    proto._splAst = &sa;
    proto._cppNamespace = cppNamespace;
    MODEL("StartCollecting()\n");
    return proto;
}

AstNode& FunctionHeadSym::ast() const
{
    assert(NULL != _splAst);
    return _splAst->ast();
}

FunctionHeadSym::FunctionHeadSym(SourceLocation const& loc, AstNode& inast, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _splAst(NULL)
  , _builtinNumber(-1)
  , _dependencies(NULL)
  , _heldScope(new Scope(inparentScope, this))
  , _ir(NULL)
  , _isDef(AST::functionDef::is(inast))
  , _isIntrinsic(!inparentScope->holder()->is<CompilationUnitSym>())
  , _isPublic(false)
  , _isStateful(false)
  , _isUsed(false)
  , _includeIsBuiltin(false)
  , _isNativeFunction(false)
  , _isOutputFunction(false)
  , _makesStatefulCall(false)
  , _isPseudoFunction(false)
  , _isJavaFunction(false)
  , _returnType(NULL)
{
    AstNode& idAst = _isDef ? AST::functionHead::id(AST::functionDef::head(inast))
                            : AST::functionPrototype::id(inast);
    setAstSymbol(AstPool::instance(), inast, *this);
    setAstSymbol(AstPool::instance(), idAst, *this);
    _name = astText(idAst);
    SPLDBG(__PRETTY_FUNCTION__ << ": " << _name, Debug::TraceSymbolCreation);
    MODEL("FunctionHead(" << (void*)this << ", " << (void*)_heldScope << ", " << _name << ")\n");
    if ('$' == _name.at(0)) {
        SourceLocation const& loc1 = SourceLocationFactory::instance().create(loc, idAst);
        SysOut::errorln(SPL_FRONT_END_LEADING_DOLLAR_ID(_name), loc1);
    }
    if (_isDef) {
        _splFileName = loc.fileName();
        // Ensure that the filename has a ".spl" extension
        assert(_splFileName.substr(_splFileName.length() - 4) == ".spl");
    }
    AstNode& modifiersAst = _isDef ? AST::functionHead::modifiers(AST::functionDef::head(inast))
                                   : AST::functionPrototype::modifiers(inast);
    for (int i = 0, n = AST::modifiers::modifierCount(modifiersAst); i < n; i++) {
        AstNode& mod = AST::modifiers::modifier(modifiersAst, i);
        if (AST::publicT::is(mod)) {
            _isPublic = true;
        }
        if (AST::statefulT::is(mod)) {
            _isStateful = true;
        }
    }
    _toolkitIndex = FrontEndDriver::instance().getToolkitIndex();
    joinGroup();
}

void FunctionHeadSym::joinGroup()
{
    assert(NULL != parentScope());
    Scope* groupParentScope = _isIntrinsic ? parentScope() : parentScope()->parentScope();
    assert(NULL != groupParentScope);
    OverloadedFunctionsSym* group = NULL;
    if (groupParentScope->has(_name)) {
        group = &groupParentScope->get(_name).as<OverloadedFunctionsSym>();
    } else {
        group = new OverloadedFunctionsSym(_name, groupParentScope);
        groupParentScope->def(*group);
    }
    group->addFunction(*this);
}

void FunctionHeadSym::initFormalType(AstNode& inast)
{
    FunctionFormalSym& formalSym = astSymbol(inast).as<FunctionFormalSym>();
    AstNode& typeAst = AST::functionFormal::is(inast) ? AST::functionFormal::type(inast)
                                                      : AST::protoFormal::type(inast);
    SymbolTableEntry& typeSym = astSymbol(typeAst);
    formalSym._type = &typeSym.type();
    _formals.push_back(&formalSym);
}

void FunctionHeadSym::initTypes(AstNode& inast)
{
    if (_isDef) {
        AstNode& headAst = AST::functionDef::head(inast);
        for (int i = 0, n = AST::functionHead::functionFormalCount(headAst); i < n; i++) {
            initFormalType(AST::functionHead::functionFormal(headAst, i));
        }
    } else {
        AstNode& typeFormalsAst = AST::functionPrototype::typeFormals(inast);
        for (int i = 0, n = AST::typeFormals::typeFormalCount(typeFormalsAst); i < n; i++) {
            SymbolTableEntry& sym = astSymbol(AST::typeFormals::typeFormal(typeFormalsAst, i));
            _typeFormals.push_back(&sym.type().as<TypeFormal>());
        }
        AstNode& boundsFormalsAst = AST::functionPrototype::boundsFormals(inast);
        for (int i = 0, n = AST::boundsFormals::boundsFormalCount(boundsFormalsAst); i < n; i++) {
            SymbolTableEntry& sym =
              astSymbol(AST::boundsFormals::boundsFormal(boundsFormalsAst, i));
            _boundsFormals.push_back(sym.as<BoundsFormalSym>().bound());
        }
        AstNode& formalsAst = AST::functionPrototype::protoFormals(inast);
        for (int i = 0, n = AST::protoFormals::protoFormalCount(formalsAst); i < n; i++) {
            initFormalType(AST::protoFormals::protoFormal(formalsAst, i));
        }
    }
    AstNode& returnTypeAst = _isDef ? AST::functionHead::type(AST::functionDef::head(inast))
                                    : AST::functionPrototype::type(inast);
    _returnType = &astSymbol(returnTypeAst).type();
}

bool FunctionHeadSym::isProtoFor(FunctionHeadSym const& other) const
{
    if (!_isDef && other._isDef) {
        return toString() == other.toString();
    }
    return false;
}

string FunctionHeadSym::fullName() const
{
    string ns = nameSpace();
    if (isDefaultNameSpace(ns)) {
        return name();
    }
    return ns + "::" + name();
}

string FunctionHeadSym::nameSpace() const
{
    const SymbolTableEntry* holder = parentScope()->parentScope()->holder();
    if (holder) {
        return holder->name();
    }
    return "";
}

bool FunctionHeadSym::overlapsWith(FunctionHeadSym const& other) const
{
    auto_ptr<GenericsUnifier const> gu(&GenericsUnifier::unify(*this, other));
    bool result = !gu->isFailure();
    return result;
}

Scope* FunctionHeadSym::parentScope() const
{
    assert(NULL != _heldScope);
    return _heldScope->parentScope(); // scope of enclosing compilation unit
}

ostream& FunctionHeadSym::print(ostream& os) const
{
    os << "FunctionHeadSym(" << nameSpace() << " :: ";
    printBare(os);
    return os << ")";
}

ostream& FunctionHeadSym::printBare(ostream& os) const
{
    if (0 < _typeFormals.size()) {
        os << '<';
        for (int i = 0, n = _typeFormals.size(); i < n; i++) {
            if (0 < i) {
                os << ", ";
            }
            TypeFormal const* tf = _typeFormals[i];
            os << tf->getConstraint() << ' ' << tf->getIdentifier();
        }
        os << "> ";
    }
    if (0 < _boundsFormals.size()) {
        os << '[';
        for (int i = 0, n = _boundsFormals.size(); i < n; i++) {
            if (0 < i) {
                os << ", ";
            }
            os << _boundsFormals[i]->getIdentifier();
        }
        os << "] ";
    }
    if (isPublic()) {
        os << "public ";
    }
    if (isStateful()) {
        os << "stateful ";
    }
    os << returnTypePtr() << ' ';
    os << name() << '(';
    for (int i = 0, n = _formals.size(); i < n; i++) {
        if (0 < i) {
            os << ", ";
        }
        FunctionFormalSym* ff = _formals[i];
        assert(NULL != ff);
        if (ff->isMutable()) {
            os << "mutable ";
        }
        os << ff->type();
        if (ff->hasName()) {
            os << ' ' << ff->name();
        }
    }
    return os << ')';
}

void FunctionHeadSym::markUsed()
{
    if (!_isUsed) {
        _isUsed = true;
        CompilationState& state = CompilationState::instance();
        if (!_isIntrinsic && !_isPseudoFunction) {
            assert(_toolkitIndex < PathSearch::instance().toolkits().size());
            const Toolkit& tk = *PathSearch::instance().toolkits()[_toolkitIndex];
            state.setToolkitUsed(tk);
        }
        if (NULL != _ir) {
            assert(_isDef);
            if (false) {
                cerr << "UsedSPLFunctions += " << this << endl;
            }
            UsedSPLFunctions::instance().addFunction(*this, *_ir);
        }
    }
}

void FunctionHeadSym::resolvePrototype()
{
    FrontEndDriver& frontEnd = FrontEndDriver::instance();
    assert(!_isDef && NULL != _splAst && NULL == _returnType);
    if (!_isNativeFunction && !_isIntrinsic) {
        frontEnd.newCompilationUnit(_splFileName, _toolkitIndex);
    }
    SymbolTable& symTab = SymbolTable::instance();
    assert(NULL != parentScope());
    Scope& oldScope = symTab.excursionBegin(*parentScope());
    LexicalNameUseFinder::run(_splAst->parserContext(), _splAst->ast());
    frontEnd.finishCompilationUnits(FrontEndStage::LEXICAL_NAME_USE_FINDER);
    RelativeNameUseFinder::run(_splAst->parserContext(), _splAst->ast());
    frontEnd.finishCompilationUnits(FrontEndStage::RELATIVE_NAME_USE_FINDER);
    symTab.excursionEnd(oldScope);
    assert(NULL != _returnType); // make sure that initTypes() was called
}

BlockStatement* FunctionHeadSym::setFunctionBodyIR(BlockStatement* ir)
{
    assert(NULL == _ir && NULL != ir && _isDef);
    _ir = ir;
    if (_isUsed) {
        if (false) {
            cerr << "UsedSPLFunctions += " << this << endl;
        }
        UsedSPLFunctions::instance().addFunction(*this, *ir);
    }
    return _ir;
}

string FunctionHeadSym::toString() const
{
    ostringstream os;
    printBare(os);
    return os.str();
}

// .................................................................
OverloadedFunctionsSym::OverloadedFunctionsSym(string const& inname, Scope* inparentScope)
  : SymbolTableEntry(SourceLocation::invalidSourceLocation)
  , _functions()
  , _name(inname)
  , _parentScope(inparentScope)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": " << inname, Debug::TraceSymbolCreation);
    MODEL("OverloadedFunctions(" << (void*)this << ", " << inname << ")\n");
}

void OverloadedFunctionsSym::addFunction(FunctionHeadSym& fhNew)
{
    assert(_name == fhNew.name());
    _functions.push_back(&fhNew);
}

OverloadedFunctionsSym::Functions const& OverloadedFunctionsSym::functions()
{
    bool foundUnresolvedSPLFunction = false;
    for (int i = 0, n = _functions.size(); i < n; i++) {
        FunctionHeadSym& fun = *_functions.at(i);
        if (!fun.isDef()) {
            if (!fun.isIntrinsic() && !fun.isNativeFunction()) {
                foundUnresolvedSPLFunction = true;
            }
            if (NULL == fun.returnTypePtr()) {
                fun.resolvePrototype();
            }
        }
    }
    if (foundUnresolvedSPLFunction) {
        Functions newFunctions;
        int n = _functions.size();
        std::vector<bool> resolved(n, false);
        for (int i1 = 0; i1 < n; i1++) {
            FunctionHeadSym& fun1 = *_functions.at(i1);
            if (fun1.isDef() || fun1.isIntrinsic() || fun1.isNativeFunction()) {
                newFunctions.push_back(&fun1);
                resolved[i1] = true;
            }
            for (int i2 = i1 + 1; i2 < n; i2++) {
                FunctionHeadSym& fun2 = *_functions.at(i2);
                if (fun1.isDef() && !resolved.at(i2)) {
                    resolved[i2] = fun2.isProtoFor(fun1);
                }
                if (fun2.isDef() && !resolved.at(i1)) {
                    resolved[i1] = fun1.isProtoFor(fun2);
                }
                if (fun1.isDef() && fun2.isDef() && fun1.overlapsWith(fun2)) {
                    SysOut::errorln(SPL_FRONT_END_OVERLAPPING_OVERLOADED_FUNCTION(fun2.name()),
                                    fun2.sourceLocation());
                    SysOut::detailsErrorln(SPL_FRONT_END_PREVIOUS_LOCATION(fun2.name()),
                                           fun1.sourceLocation());
                }
            }
        }
        for (int i = 0; i < n; i++) {
            // We don't see how this can happen at this point
            assert(resolved.at(i));
            // FunctionHeadSym & fun = *_functions.at(i);
            // SysOut::errorln(SPL_FRONT_END_UNRESOLVED_PROTOTYPE(fun.name()),
            // fun.sourceLocation());
        }
        _functions = newFunctions;
    }
    return _functions;
}

ostream& OverloadedFunctionsSym::print(ostream& os) const
{
    os << "OverloadedFunctionsSym(" << name();
    os << ", " << _functions.size() << " functions(";
    for (int i = 0, n = _functions.size(); i < n; i++) {
        if (0 < i) {
            os << ", ";
        }
        os << _functions.at(i);
    }
    return os << "))";
}
