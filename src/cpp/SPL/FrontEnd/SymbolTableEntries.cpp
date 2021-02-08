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

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/EmptyContainerTypeFinder.h>
#include <SPL/FrontEnd/ExpressionTypeFinder.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/GenericsUnifier.h>
#include <SPL/FrontEnd/IRGenerator.h>
#include <SPL/FrontEnd/LogicSubmitChecker.h>
#include <SPL/FrontEnd/PromotionChecker.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeAnalyzer.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/TypePromoter.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/FrontEnd/VariableUseChecker.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

#include <algorithm>
#include <antlr3.h>
#include <cassert>
#include <fstream>
#include <iostream>

#define DEFAULT_NS_NAME "<default_namespace>"
// #define DEFAULT_NS_NAME ""

using namespace std;
using namespace tr1;
using namespace SPL;

// Given an ast fragment, which should be an r- or u-string literal, extract the value as a string
#if 0
static string computeStringValue(AstNode & ast)
{
  SymbolTableEntry & sym = astSymbol(ast);
  string val = astText(ast);
  val = val.substr(0, 1 + val.find_last_not_of("ru"));
  switch(sym.type().getMetaType()) {
    case MetaType::RSTRING:
    try {
      string v = interpretRStringLiteral(val);
      return v;
    }
    catch(Distillery::DistilleryException const & e) {
      SysOut::errorln(SPL_FRONT_END_INVALID_STRING_LITERAL(e.getExplanation()), sym.sourceLocation());
    }
    break;
    case MetaType::USTRING:
    try {
      ustring u = interpretUStringLiteral(val);
      string v = ustring::toUTF8(u);
      return v;
    }
    catch(Distillery::DistilleryException const & e) {
      SysOut::errorln(SPL_FRONT_END_INVALID_STRING_LITERAL(e.getExplanation()), sym.sourceLocation());
    }
    break;
    default: {
      cerr << "Type is " << sym.type().getMetaType() << endl;
      return val;
    }
  }
  assert(0);
  return "";
}
#endif

static void addAllIndirect(Scope& target, SymbolTableEntry& origin)
{
    // Make sure the origin has been expanded prior to walking its scope
    origin.expand();

    Scope* origScope = origin.heldScope();
    if (NULL == origScope) {
        return;
    }
    for (int i = 0, n = origScope->size(); i < n; i++) {
        SymbolTableEntry& entry = origScope->get(i);
        string name = entry.name();
        if (!target.has(name)) {
            IndirectSym& ind = *new IndirectSym(name);
            ind.addDef(const_cast<SymbolTableEntry&>(origin), entry);
            target.def(ind);
        } else {
            if (target.get(name).is<IndirectSym>()) {
                target.get(name).as<IndirectSym>().addDef(const_cast<SymbolTableEntry&>(origin),
                                                          entry);
            }
        }
    }
}

static void ensureNoLeadingDollarId(string const& name, SourceLocation const& loc)
{
    if ('$' == name.at(0)) {
        SysOut::errorln(SPL_FRONT_END_LEADING_DOLLAR_ID(name), loc);
    }
}

static TypeBound getBound(ParserContext& pCtx, AstNode& ast)
{
    SymbolTableEntry* sym = astSymbolPtr(ast);
    if (NULL != sym && sym->is<BoundsFormalSym>()) {
        return sym->as<BoundsFormalSym>().bound();
    }
    TypeAnalyzer::run(pCtx, ast);
    TypePromoter::run(pCtx, ast);
    PromotionChecker::run(pCtx, ast);
    ExpressionTypeFinder::run(pCtx, ast);
    TypeBound dummy(1);
    // If type analysis did not yield a sym....
    if (NULL == astSymbolPtr(ast)) {
        return dummy;
    }
    RootTyp const& type = astSymbol(ast).type();
    if (MetaType::UNKNOWN == type.getMetaType()) {
        return dummy;
    }
    bool correct = satisfiesConstraint(type, "integral");
    if (correct) {
        EmptyContainerTypeFinder::run(pCtx, ast);
        VariableUseChecker::run(pCtx, ast);
        LogicSubmitChecker::run(pCtx, ast);
        IRGenerator::run(pCtx, ast);
        Expression& ir = astExpression(ast);
        ExpressionEvaluator ee(CompilerConfiguration::instance());
        Expression* simple = Expression::simplify(&ir, ee);
        assert(NULL != simple);
        Literal const* lit = simple->getLiteral();
        correct &= NULL != lit;
        if (correct) {
            ExpressionValue const& ev = lit->primitive();
            int64_t val;
            bool isInteger = ev.getIntegerValue(val);
            correct &= isInteger && 0 < val;
            if (correct) {
                return TypeBound(val);
            }
        }
    }
    assert(!correct);
    SysOut::errorln(SPL_FRONT_END_INVALID_TYPE_BOUND,
                    SourceLocationFactory::instance().create(pCtx, ast));
    return dummy;
}

bool SymbolTableEntry::isDefaultNameSpace(const string& name)
{
    return name == DEFAULT_NS_NAME;
}

// .................................................................
ActualConfigSym::ActualConfigSym(SourceLocation const& loc, AstNode& inast, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(inast)
  , _heldScope(new Scope(inparentScope, this))
  , _isExpanded(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("ActualConfig(" << (void*)this << ", " << (void*)&inast << ")\n");
    assert(this == _heldScope->holder());
    AstNode& labelAst = AST::config::id(inast);
    if (astText(labelAst) == "hostPool") {
        FormalConfigSym& labelSym = astSymbol(labelAst).as<FormalConfigSym>();
        for (int i = 0, n = AST::config::exprCount(inast); i < n; i++) {
            AstNode& assignAst = AST::config::expr(inast, i);
            if (AST::eqT::is(assignAst) && AST::idT::is(AST::infixExpr::lhs(assignAst))) {
                SourceLocation const& poolLoc =
                  SourceLocationFactory::instance().create(loc, assignAst);
                HostPoolSym& sym = *new HostPoolSym(poolLoc, assignAst);
                labelSym.heldScope()->def(sym);
                Scope& placementScope = SymbolTable::instance().configScope("placement");
                if (!placementScope.has(sym.name())) { // avoid redundant error message
                    placementScope.def(sym);
                }
            }
        }
    }
}

void ActualConfigSym::expand()
{
    assert(!_isExpanded);
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    SymbolTableEntry* label = astSymbolPtr(AST::config::id(_astNode));
    Scope* scope = NULL == label ? NULL : label->heldScope();
    if (NULL != scope) {
        _heldScope->addAll(*scope);
    }
}

string ActualConfigSym::name() const
{
    return astText(AST::config::id(_astNode));
}

ostream& ActualConfigSym::print(ostream& os) const
{
    return os << "ActualConfigSym(" << name() << ", scope=" << heldScope()
              << ", astNode=" << _astNode << ")";
}
// .................................................................
AttributeAccessSym::AttributeAccessSym(SourceLocation const& loc, AstNode& astNode)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("AttributeAccess(" << (void*)this << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), _astNode, *this);
}

SymbolTableEntry& AttributeAccessSym::base() const
{
    return astSymbol(AST::attributeExpr::base(_astNode));
}

SymbolTableEntry& AttributeAccessSym::attrib() const
{
    RootTyp const& unknownType = TypeFactory::instance().unknownT();
    AstNode& attrAst = AST::attributeExpr::id(_astNode);
    SPLDBG(__PRETTY_FUNCTION__ << ": Checking for name " << astText(attrAst),
           Debug::TraceNameLookup);
    SymbolTableEntry* attrSym = astSymbolPtr(attrAst);
    if (NULL == attrSym) {
        SymbolTableEntry& baseSym = base();
        if (baseSym.is<OperatorSym>() || baseSym.type() != unknownType) {
            Scope* baseScope = baseSym.is<PrimitiveOperatorSym>()
                                 ? baseSym.as<PrimitiveOperatorSym>().enumsScope()
                                 : baseSym.heldScope();
            if (NULL != baseScope && baseScope->has(name())) {
                SPLDBG(__PRETTY_FUNCTION__ << ": " << astText(attrAst) << " found in "
                                           << baseSym.name(),
                       Debug::TraceNameLookup);
                attrSym = &baseScope->get(name());
            }
        }
        if (NULL == attrSym) {
            SysOut::errorln(SPL_FRONT_END_UNKNOWN_ATTRIBUTE(name()), sourceLocation());
            attrSym = new ErrorDummySym(sourceLocation(), attrAst, name(), NULL);
        }
        setAstSymbol(AstPool::instance(), attrAst, *attrSym);
        if (attrSym->is<DefTypeSym>() && !attrSym->as<DefTypeSym>().isStatic()) {
            SysOut::errorln(SPL_FRONT_END_NONSTATIC_OPERATOR_DOT_TYPE(name()), sourceLocation());
            attrSym = new ErrorDummySym(sourceLocation(), attrAst, name(), NULL);
        }
        if (attrSym->is<VariableSym>() && !attrSym->as<VariableSym>().isStatic()) {
            SysOut::errorln(SPL_FRONT_END_NONSTATIC_OPERATOR_DOT_VAR(name()), sourceLocation());
            attrSym = new ErrorDummySym(sourceLocation(), attrAst, name(), NULL);
        }
    }
    return astSymbol(attrAst);
}

Scope* AttributeAccessSym::heldScope() const
{
    return attrib().heldScope();
}

string AttributeAccessSym::name() const
{
    return astText(AST::attributeExpr::id(_astNode));
}

Scope* AttributeAccessSym::parentScope() const
{
    return astSymbol(AST::attributeExpr::base(_astNode)).heldScope();
}

ostream& AttributeAccessSym::print(ostream& os) const
{
    return os << "AttributeAccessSym(" << _astNode << " @ " << sourceLocation() << ")";
}

RootTyp const& AttributeAccessSym::type() const
{
    return attrib().type();
}

// .................................................................
AttributeAssignSym::AttributeAssignSym(SourceLocation const& loc,
                                       AstNode& astNode,
                                       Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _parentScope(inparentScope)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("AttributeAssign(" << (void*)this << ", " << (void*)&astNode << ")\n");

    setAstSymbol(AstPool::instance(), _astNode, *this);
    setAstSymbol(AstPool::instance(), AST::attributeAssign::id(_astNode), *this);
    string n = name();
    ensureNoLeadingDollarId(n, loc);
    if (Utility::reservedName(n)) {
        SysOut::errorln(SPL_FRONT_END_ATTRIBUTE_CPP_NAME_CLASH(n), loc);
    }
}

string AttributeAssignSym::name() const
{
    return astText(AST::attributeAssign::id(_astNode));
}

ostream& AttributeAssignSym::print(ostream& os) const
{
    return os << "AttributeAssignSym(" << name() << ", ast=" << _astNode << ", type=" << type()
              << ")";
}

RootTyp const& AttributeAssignSym::type() const
{
    return astSymbol(AST::attributeAssign::expr(_astNode)).type();
}

// .................................................................
AttributeDeclSym::AttributeDeclSym(SourceLocation const& loc,
                                   AstNode& astNode,
                                   Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _parentScope(inparentScope)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    setAstSymbol(AstPool::instance(), _astNode, *this);
    AstNode& n = AST::attributeDecl::id(_astNode);
    SourceLocation const& nameLoc = SourceLocationFactory::instance().create(loc, n);
    _name = astText(n);
    ensureNoLeadingDollarId(_name, nameLoc);
    if (Utility::reservedName(_name)) {
        SysOut::errorln(SPL_FRONT_END_ATTRIBUTE_CPP_NAME_CLASH(_name), nameLoc);
    }
    MODEL("AttributeDecl(" << (void*)this << ", " << _name << ", " << (void*)&astNode << ")\n");
}

Scope* AttributeDeclSym::heldScope() const
{
    SymbolTableEntry* e = astSymbolPtr(AST::attributeDecl::type(_astNode));
    return NULL == e ? NULL : e->heldScope();
}

ostream& AttributeDeclSym::print(ostream& os) const
{
    return os << "AttributeDeclSym(" << name() << ", ast=" << _astNode << ", type=" << type()
              << ")";
}

RootTyp const& AttributeDeclSym::type() const
{
    return astSymbol(AST::attributeDecl::type(_astNode)).type();
}

Expression* AttributeDeclSym::genExpression(const SourceLocation& loc) const
{
    return new SymbolExpression(type(), loc, name(), Expression::Attr);
}

// .................................................................
AttributeFromTypeSym::AttributeFromTypeSym(SourceLocation const& loc,
                                           Scope& inparentScope,
                                           string const& inname,
                                           RootTyp const& intype)
  : SymbolTableEntry(loc)
  , _heldScope(typeToScope(loc, intype, NULL))
  , _name(inname)
  , _parentScope(inparentScope)
  , _type(intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("AttributeFromType(" << (void*)this << ", " << inname << ")\n");
}

ostream& AttributeFromTypeSym::print(ostream& os) const
{
    return os << "AttributeFromTypeSym(" << _name << ", type=" << &_type << ")";
}

// .................................................................
BoundsFormalSym::BoundsFormalSym(SourceLocation const& loc, Scope& inparentScope, AstNode& astNode)
  : SymbolTableEntry(loc)
  , _bound(*new BoundsFormal(&inparentScope.holder()->as<FunctionHeadSym>(), astText(astNode)))
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("BoundsFormal(" << (void*)this << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), astNode, *this);
    ensureNoLeadingDollarId(name(), loc);
}

string BoundsFormalSym::name() const
{
    return _bound.getIdentifier();
}

Scope* BoundsFormalSym::parentScope() const
{
    return _bound.getFunction()->heldScope();
}

ostream& BoundsFormalSym::print(ostream& os) const
{
    /// TODO: Fix operator<< for BoundsFormal in Typ.h/cpp
    return os << "BoundsFormalSym(" << &_bound << ")";
}

RootTyp const& BoundsFormalSym::type() const
{
    return TypeFactory::instance().primitiveT("uint32");
}

// .................................................................
CompilationUnitSym::CompilationUnitSym(SourceLocation const& loc,
                                       Scope* inparentScope,
                                       AstNode* astNode)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(inparentScope, this))
  , _namedUses()
  , _wildcardUses()
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    assert(this == _heldScope->holder());
    string nameSpace = inparentScope->holder()->as<NamespaceSym>().name();
    MODEL("CompilationUnit(" << (void*)this << ", " << (void*)_heldScope << ", " << name()
                             << ")\n");
    if (nameSpace == "spl" || 0 == nameSpace.compare(0, 4, "spl.")) {
        if (!PathSearch::instance().isInStandardToolkit(loc)) {
            SysOut::errorln(SPL_FRONT_END_SPL_NAMESPACE_OUTSIDE_STANDARD_TOOLKIT, loc);
        }
    }
}

void CompilationUnitSym::addUseDirective(AstNode& inast)
{
    MODEL("AddUseDirective(" << (void*)this << ", " << (void*)&inast << ")\n");

    ostringstream os;
    prettyPrint(os, AST::useDirective::namespaceName(inast));
    string lhs = os.str();
    if (PathSearch::instance().toolkitNamespaces().count(lhs) == 0) {
        SysOut::warnln(SPL_FRONT_END_UNKNOWN_NAMESPACE_IN_USE(lhs),
                       SourceLocationFactory::instance().create(
                         sourceLocation(), AST::useDirective::namespaceName(inast)));
    }
    AstNode& idOrStar = AST::useDirectiveTail::idOrStar(AST::useDirective::useDirectiveTail(inast));
    if (AST::useWildcardT::is(idOrStar)) {
        _wildcardUses.insert(lhs);
    } else {
        string rhs = astText(idOrStar);
        if (_namedUses.end() != _namedUses.find(rhs)) {
            SysOut::errorln(SPL_FRONT_END_DUPLICATE_USE_DIRECTIVE,
                            SourceLocationFactory::instance().create(sourceLocation(), inast));
        } else {
            _namedUses[rhs] = lhs;
        }
    }
}

CompilationUnitSym::StringSet const& CompilationUnitSym::implicitUses()
{
    static StringSet memoized;
    if (memoized.empty()) { // not yet initialized
        memoized.insert("spl");
        memoized.insert("spl.adapter");
        memoized.insert("spl.collection");
        memoized.insert("spl.control");
        memoized.insert("spl.eventtime");
        memoized.insert("spl.math");
        memoized.insert("spl.relational");
        memoized.insert("spl.string");
        memoized.insert("spl.time");
        memoized.insert("spl.utility");
        memoized.insert("spl.XML");
    }
    return memoized;
}

string CompilationUnitSym::name() const
{
    return sourceLocation().fileName();
}

ostream& CompilationUnitSym::print(ostream& os) const
{
    return os << "CompilationUnitSym(" << name() << ", ast=" << _astNode << ", scope=" << _heldScope
              << ")";
}

// .................................................................
CompositeDefSym::CompositeDefSym(SourceLocation const& loc,
                                 AstNode& astNode,
                                 Scope& inparentScope,
                                 AstNode& ininstanceBodyAst)
  : OperatorSym(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(&inparentScope, this))
  , _instanceBodyAst(ininstanceBodyAst)
  , _ir(NULL)
  , _paramsScope(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("CompositeDef(" << (void*)this << ", " << (void*)_heldScope << ", " << name() << ", "
                          << (void*)&astNode << ", " << (void*)&ininstanceBodyAst << ")\n");
    assert(this == _heldScope->holder());
    ensureNoLeadingDollarId(name(), loc);
    ensureNoDuplicateClauses();
    ensureNoDuplicatePorts();
    _heldScope->addAll(SymbolTable::instance().commonCompositeDefScope());
    setAstSymbol(AstPool::instance(), _astNode, *this);
}

void CompositeDefSym::ensureNoDuplicateClauses()
{
    AstNode& bodyAst = AST::compositeDef::compositeBody(_astNode);
    if (AST::compositeBody::has_config(bodyAst)) {
        AstNode& clauseAst = AST::compositeBody::config(bodyAst);
        unordered_set<string> labels;
        for (int iBranch = 0, nBranches = AST::configClause::configCount(clauseAst);
             iBranch < nBranches; iBranch++) {
            AstNode& branchAst = AST::configClause::config(clauseAst, iBranch);
            string label = astText(AST::config::id(branchAst));
            if (labels.find(label) != labels.end()) {
                SysOut::errorln(
                  SPL_FRONT_END_OPERATOR_DUPLICATE_LABEL(label),
                  SourceLocationFactory::instance().create(sourceLocation(), branchAst));
            } else {
                labels.insert(label);
            }
        }
    }
}

void CompositeDefSym::ensureNoDuplicatePorts()
{
    AstNode& headAst = AST::compositeDef::compositeHead(_astNode);
    StringMap<AstNode*> portNames;
    AstNode& outAst = AST::compositeHead::compositeOut(headAst);
    if (!AST::noneT::is(outAst)) {
        for (int i = 0, n = AST::compositeOut::compositePortCount(outAst); i < n; i++) {
            AstNode& idAst = AST::compositePort::id(AST::compositeOut::compositePort(outAst, i));
            string id = astText(idAst);
            if (portNames.has(id)) {
                SysOut::errorln(SPL_FRONT_END_DUPLICATE_PORT_NAME(id),
                                SourceLocationFactory::instance().create(sourceLocation(), idAst));
            } else {
                portNames.put(id, &idAst);
            }
        }
    }
    AstNode& inAst = AST::compositeHead::compositeIn(headAst);
    if (!AST::noneT::is(inAst)) {
        for (int i = 0, n = AST::compositeIn::compositePortCount(inAst); i < n; i++) {
            AstNode& idAst = AST::compositePort::id(AST::compositeIn::compositePort(inAst, i));
            string id = astText(idAst);
            if (portNames.has(id)) {
                SysOut::errorln(SPL_FRONT_END_DUPLICATE_PORT_NAME(id),
                                SourceLocationFactory::instance().create(sourceLocation(), idAst));
            } else {
                portNames.put(id, &idAst);
            }
        }
    }
}

void CompositeDefSym::expandParamScope()
{
    assert(NULL == _paramsScope);
    _paramsScope = new Scope(heldScope(), NULL);
    for (int i = 0, n = heldScope()->size(); i < n; i++) {
        SymbolTableEntry& formal = heldScope()->get(i);
        if ('$' == formal.name().at(0)) {
            _paramsScope->def(formal);
        }
    }
}

static string concatNamespace(const string& ns, const string& n)
{
    if (ns == DEFAULT_NS_NAME) {
        return n;
    }
    return ns + "::" + n;
}

string CompositeDefSym::fullName() const
{
    Scope* nsScope = parentScope()->parentScope();
    string nsName = nsScope->holder()->as<NamespaceSym>().name();
    return concatNamespace(nsName, name());
}

bool CompositeDefSym::isPublic() const
{
    return isPublic(_astNode);
}

bool CompositeDefSym::isPublic(AstNode& ast)
{
    AstNode& modsAst = AST::compositeHead::modifiers(AST::compositeDef::compositeHead(ast));
    for (int i = 0, n = AST::modifiers::modifierCount(modsAst); i < n; i++) {
        if (AST::publicT::is(AST::modifiers::modifier(modsAst, i))) {
            return true;
        }
    }
    return false;
}

string CompositeDefSym::name() const
{
    return astText(AST::compositeHead::id(AST::compositeDef::compositeHead(_astNode)));
}

ostream& CompositeDefSym::print(ostream& os) const
{
    return os << "CompositeDefSym(" << name() << ", params=" << paramsScope() << ")";
}

CompositeDefinition& CompositeDefSym::setCompositeDefIR(CompositeDefinition& ir) const
{
    assert(NULL == _ir);
    _ir = &ir;
    return ir;
}

// .................................................................
CompositeInstanceSym::CompositeInstanceSym(SourceLocation const& loc,
                                           CompositeDefSym& def,
                                           string const& infullName,
                                           CompositeInstanceSym* inparent)
  : SymbolTableEntry(loc)
  , _def(def)
  , _fullName(infullName)
  , _heldScope(*new Scope(def.heldScope(), this))
  , _ir(NULL)
  , _parent(inparent)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("CompositeInstance(" << (void*)this << ", " << (void*)&_heldScope << ", " << infullName
                               << ", " << (void*)&def << ")\n");
}

void CompositeInstanceSym::fixUpActual(AstNode& parentAst, AstNode& useAst) const
{
    for (int iChild = 0, nChildren = astSize(parentAst); iChild < nChildren; iChild++) {
        AstNode& ast1 = astChild(parentAst, iChild);
        if (AST::idT::is(ast1)) {
            SymbolTableEntry* sym = astSymbolPtr(ast1);
            if (NULL != sym && (sym->is<IndirectSym>() || symIsStream(*sym))) {
                SymbolTableEntry* oldStream =
                  sym->is<IndirectSym>() ? sym->as<IndirectSym>().base() : sym;
                string replaceBy = "";
                for (int iPort = 0, nPorts = _outputPorts.size(); iPort < nPorts; iPort++) {
                    SymbolTableEntry& port = *_outputPorts.get(iPort);
                    if (oldStream == &port) {
                        replaceBy = _outputPorts.getKey(iPort);
                        goto replace;
                    }
                }
                for (int iPort = 0, nPorts = _heldScope.size(); iPort < nPorts; iPort++) {
                    SymbolTableEntry& port = _heldScope.get(iPort);
                    if (port.is<CompositeInputPortSym>()) {
                        AstNode& portInputsAst = port.as<CompositeInputPortSym>().actualAst();
                        for (int iStream = 0, nStreams = AST::portInputs::idCount(portInputsAst);
                             iStream < nStreams; iStream++) {
                            if (astText(AST::portInputs::id(portInputsAst, iStream)) ==
                                oldStream->name()) {
                                replaceBy = port.name();
                                goto replace;
                            }
                        }
                        if (AST::portInputs::has_alias(portInputsAst)) {
                            if (astText(AST::as::id(AST::portInputs::alias(portInputsAst))) ==
                                oldStream->name()) {
                                replaceBy = port.name();
                                goto replace;
                            }
                        }
                    }
                }
            replace:
                if (replaceBy != "") {
                    bool changed = false;
                    for (Scope* s = &SymbolTable::instance().currentScope(); NULL != s;
                         s = s->parentScope()) {
                        if (s->has(replaceBy)) {
                            SymbolTableEntry& newStream = s->get(replaceBy);
                            if (symIsStream(newStream)) {
                                if (sym->is<IndirectSym>()) {
                                    if (newStream.heldScope()->has(sym->name())) {
                                        // keep symbol but reset previous IRGenerator results, to
                                        // force re-analysis
                                        AstNode& newAst = AST::idT::create(sym->name());
                                        setAstSymbol(AstPool::instance(), newAst, *sym);
                                        copyAstPosition(AstPool::instance(), newAst, useAst);
                                        resetAstChild(AstPool::instance(), parentAst, iChild,
                                                      newAst);
                                        changed = true;
                                        break;
                                    }
                                } else {
                                    AstNode& newAst = AST::idT::create(replaceBy);
                                    setAstSymbol(AstPool::instance(), newAst, newStream);
                                    copyAstPosition(AstPool::instance(), newAst, useAst);
                                    resetAstChild(AstPool::instance(), parentAst, iChild, newAst);
                                    changed = true;
                                    break;
                                }
                            }
                        }
                    }
                    // If the parent node is a typename node ensure that the symbol from the ID is
                    // copied over to it also
                    if (changed && AST::typeName::is(parentAst)) {
                        resetAstSymbol(AstPool::instance(), parentAst,
                                       astSymbol(astChild(parentAst, iChild)));
                    }
                }
            }
        } else {
            fixUpActual(ast1, useAst);
        }
    }
}

AstNode& CompositeInstanceSym::actual(string const& formalName, AstNode& useAst) const
{
    AstNode& resultAst = *_actuals.get(formalName);
    // Patch a clone of the tree in case there are other uses of it
    AstNode& clone = astClone(AstPool::instance(), resultAst);
    fixUpActual(clone, useAst);
    SPLDBG("CompositeInstanceSym::actual: Replacing actual " << (void*)&resultAst << " with clone "
                                                             << (void*)&clone,
           Debug::TraceCompositeExpansion);
    return clone;
}

void CompositeInstanceSym::defActual(string const& formalName, AstNode& actualAst)
{
    if (!_actuals.has(formalName)) {
        _actuals.put(formalName, &actualAst);
        MODEL("DefActual(" << (void*)this << ", " << formalName << ", " << (void*)&actualAst
                           << ")\n");
    }
}

void CompositeInstanceSym::defOutputPort(string const& formalName, SymbolTableEntry& actualSym)
{
    if (!_outputPorts.has(formalName)) {
        _outputPorts.put(formalName, &actualSym);
        MODEL("DefOutputPort(" << (void*)this << ", " << formalName << ", " << (void*)&actualSym
                               << ")\n");
    }
}

string CompositeInstanceSym::fullStreamName(string const& localStreamName) const
{
    if (_outputPorts.has(localStreamName)) {
        return _outputPorts.get(localStreamName)->fullName();
    }
    if (0 == _fullName.size()) {
        return localStreamName;
    }
    return _fullName + "." + localStreamName;
}

bool CompositeInstanceSym::hasActual(string const& formalName) const
{
    return _actuals.has(formalName);
}

void CompositeInstanceSym::pushInputPortActualStreamList()
{
    if (_ir) {
        _ir->pushInputPortActualStreamList();
    }
}

void CompositeInstanceSym::addInputPortActualStream(const string& stream)
{
    if (_ir) {
        _ir->addInputPortActualStream(stream);
    }
}

void CompositeInstanceSym::addOutputPortActualStream(const string& stream)
{
    if (_ir) {
        _ir->addOutputPortActualStream(stream);
    }
}

void CompositeInstanceSym::addOutputPortFormalStream(const string& stream)
{
    if (_ir) {
        _ir->addOutputPortFormalStream(stream);
    }
}

void CompositeInstanceSym::addInputPortAlias(const string& alias)
{
    if (_ir) {
        _ir->addInputPortAlias(alias);
    }
}

void CompositeInstanceSym::addOutputPortAlias(const string& alias)
{
    if (_ir) {
        _ir->addOutputPortAlias(alias);
    }
}

void CompositeInstanceSym::addInputPortFormalStream(const string& stream)
{
    if (_ir) {
        _ir->addInputPortFormalStream(stream);
    }
}

ostream& CompositeInstanceSym::print(ostream& os) const
{
    os << "CompositeInstanceSym(operator " << name();
    if (fullName() != "") {
        os << ", instance " << fullName();
    }
    os << ", outputPorts {";
    for (int i = 0, n = _outputPorts.size(); i < n; i++) {
        if (0 < i) {
            os << ", ";
        }
        os << _outputPorts.getKey(i) << ": " << _outputPorts.get(i)->name();
    }
    os << "}, inputPorts {";
    for (int i = 0, n = _heldScope.size(); i < n; i++) {
        if (0 < i) {
            os << ", ";
        }
        SymbolTableEntry& sym = _heldScope.get(i);
        if (sym.is<CompositeInputPortSym>()) {
            os << sym.name() << ": " << sym.as<CompositeInputPortSym>().actualAst();
        }
    }
    os << "}, actuals {";
    for (int i = 0, n = _actuals.size(); i < n; i++) {
        if (0 < i) {
            os << ", ";
        }
        os << _actuals.getKey(i) << ": " << _actuals.get(i);
    }
    return os << "})";
}

CompositeOperatorInstance& CompositeInstanceSym::setCompositeInstanceIR(
  CompositeOperatorInstance& ir) const
{
    assert(NULL == _ir);
    _ir = &ir;
    return ir;
}

// .................................................................
CompositeFormalSym::CompositeFormalSym(SourceLocation const& loc,
                                       AstNode& astNode,
                                       Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(inparentScope, this))
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("CompositeFormal(" << (void*)this << ", " << (void*)_heldScope << ", " << name() << ", "
                             << (void*)&astNode << ")\n");
    assert(this == _heldScope->holder());
    if ('$' != name().at(0)) {
        SysOut::errorln(SPL_FRONT_END_LEADING_DOLLAR_MISSING, loc);
    }
    setAstSymbol(AstPool::instance(), _astNode, *this);
}

AstNode& CompositeFormalSym::expressionMode() const
{
    return AST::expressionMode::name(AST::compositeFormal::expressionMode(_astNode));
}

bool CompositeFormalSym::expressionModeIsAttribute() const
{
    AstNode& exprMode = expressionMode();
    return AST::attributeT::is(exprMode);
}

string CompositeFormalSym::name() const
{
    return astText(AST::compositeFormal::id(_astNode));
}

ostream& CompositeFormalSym::print(ostream& os) const
{
    return os << "CompositeFormalSym(" << name() << ", ast=" << _astNode << ", type=" << type()
              << ")";
}

RootTyp const& CompositeFormalSym::type() const
{
    AstNode& modeAst = AST::compositeFormal::expressionMode(_astNode);
    AstNode& modeNameAst = AST::expressionMode::name(modeAst);
    if (AST::typeT::is(modeNameAst)) {
        AstNode& initAst = AST::compositeFormal::compositeFormalInit(_astNode);
        if (AST::compositeFormalInit::has_opActual(initAst)) {
            AstNode& actualAst = AST::compositeFormalInit::opActual(initAst);
            if (AST::opActualExprs::is(actualAst)) {
                return astSymbol(AST::opActualExprs::expr(actualAst, 0)).type();
            }
            if (AST::opActualName::is(actualAst)) {
                return astSymbol(AST::opActualName::name(actualAst)).type();
            }
            return astSymbol(AST::opActualType::type(actualAst)).type();
        }
    } else if (AST::expressionT::is(modeNameAst)) {
        if (AST::expressionMode::has_typeArgs(modeAst)) {
            SymbolTableEntry& typeSym =
              astSymbol(AST::typeArgs::type(AST::expressionMode::typeArgs(modeAst), 0));
            return typeSym.type();
        }
    }
    return TypeFactory::instance().unknownT();
}

// .................................................................
CompositeInputPortSym::CompositeInputPortSym(SourceLocation const& loc,
                                             AstNode& inactualAst,
                                             AstNode& formalAst,
                                             CompositeInstanceSym& instanceSym)
  : SymbolTableEntry(loc)
  , _actualAst(inactualAst)
  , _formalAst(formalAst)
  , _instanceSym(instanceSym)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("CompositeInputPort(" << (void*)this << ", " << name() << ", " << (void*)&inactualAst
                                << ", " << (void*)&formalAst << ")\n");
    ensureNoLeadingDollarId(name(), loc);
}

Expression* CompositeInputPortSym::genExpression(const SourceLocation& loc) const
{
    // get the stream names
    vector<string> streamNames;
    pushBundle(streamNames);
    assert(!streamNames.empty());
    int portIndex = -1;
    int countFound = findStreamInOpInvoke(name(), SymbolTable::instance().currentOpInvoke().ast(),
                                          NULL, NULL, &portIndex, NULL);
    assert(0 < countFound && -1 != portIndex);
    if (1 < countFound) {
        SysOut::errorln(SPL_FRONT_END_LOCATED_MULTIPLE_PORTS_FOR_IDENTIFIER(name()), loc);
    }
    return new StreamSymbolExpression(type(), loc, streamNames[0], portIndex);
}

Scope* CompositeInputPortSym::heldScope() const
{
    if (AST::compositePort::has_type(_formalAst)) {
        SymbolTableEntry& typeSym =
          astSymbol(AST::streamType::tupleBody(AST::compositePort::type(_formalAst)));
        return typeSym.heldScope();
    }
    for (int i = 0, n = AST::portInputs::idCount(_actualAst); i < n; i++) {
        SymbolTableEntry& streamSym = astSymbol(AST::portInputs::id(_actualAst, i));
        Scope* result = streamSym.heldScope();
        if (NULL != result) {
            return result;
        }
    }
    return NULL;
}

string CompositeInputPortSym::name() const
{
    return astText(AST::compositePort::id(_formalAst));
}

Scope* CompositeInputPortSym::parentScope() const
{
    return _instanceSym.heldScope();
}

ostream& CompositeInputPortSym::print(ostream& os) const
{
    return os << "CompositeInputPortSym(" << name() << ", " << type() << ")";
}

vector<string>& CompositeInputPortSym::pushBundle(vector<string>& destBuffer) const
{
    for (int i = 0, n = AST::portInputs::idCount(_actualAst); i < n; i++) {
        SymbolTableEntry& streamSym = astSymbol(AST::portInputs::id(_actualAst, i));
        if (streamSym.is<StreamSym>()) {
            destBuffer.push_back(streamSym.fullName());
        } else {
            streamSym.as<CompositeInputPortSym>().pushBundle(destBuffer);
        }
    }
    return destBuffer;
}

RootTyp const& CompositeInputPortSym::type() const
{
    RootTyp const& unknown = TypeFactory::instance().unknownT();
    // Walk the input streams looking for a type
    for (int i = 0, n = AST::portInputs::idCount(_actualAst); i < n; i++) {
        SymbolTableEntry& streamSym = astSymbol(AST::portInputs::id(_actualAst, i));
        RootTyp const& result = streamSym.type();
        if (result != unknown) {
            return result;
        }
    }
    return unknown;
}

// .................................................................
DefTypeSym::DefTypeSym(SourceLocation const& loc, AstNode& astNode, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _parentScope(inparentScope)
  , _expansionStatus(notYet)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("DefType(" << (void*)this << ", " << name() << ", " << (void*)&astNode << ")\n");
    ensureNoLeadingDollarId(name(), loc);
    setAstSymbol(AstPool::instance(), _astNode, *this);
    setAstSymbol(AstPool::instance(), AST::typeDef::id(_astNode), *this);
}

SymbolTableEntry& DefTypeSym::expandedTail() const
{
    SymbolTableEntry& result = astSymbol(AST::typeDef::typeDefTail(_astNode));
    if (result.is<TupleAttribSym>()) {
        result.as<TupleAttribSym>().expand();
    } else if (result.is<TupleExtendSym>()) {
        result.as<TupleExtendSym>().expand();
    }
    return result;
}

Scope* DefTypeSym::heldScope() const
{
    if (failure == _expansionStatus) {
        return NULL;
    }
    if (notYet == _expansionStatus) {
        _expansionStatus = inProgress;
        SymbolTableEntry& tail = expandedTail();
        if (this != &tail) {
            _expansionStatus = notYet;
            return tail.heldScope();
        }
    }

    SysOut::errorln(SPL_FRONT_END_CIRCULAR_TYPE_DEFINITION(name()), sourceLocation());
    _expansionStatus = failure;
    return NULL;
}

bool DefTypeSym::isStatic() const
{
    return isStatic(_astNode);
}

bool DefTypeSym::isStatic(AstNode& ast)
{
    AstNode& modsAst = AST::typeDef::modifiers(ast);
    for (int i = 0, n = AST::modifiers::modifierCount(modsAst); i < n; i++) {
        if (AST::staticT::is(AST::modifiers::modifier(modsAst, i))) {
            return true;
        }
    }
    return false;
}

string DefTypeSym::name() const
{
    return astText(AST::typeDef::id(_astNode));
}

ostream& DefTypeSym::print(ostream& os) const
{
    return os << "DefTypeSym(" << name() << ", astNode=" << _astNode << ")";
}

RootTyp const& DefTypeSym::type() const
{
    if (failure == _expansionStatus) {
        return TypeFactory::instance().unknownT();
    }
    if (inProgress == _expansionStatus) {
        SysOut::errorln(SPL_FRONT_END_CIRCULAR_TYPE_DEFINITION(name()), sourceLocation());
        _expansionStatus = failure;
        return TypeFactory::instance().unknownT();
    }

    _expansionStatus = inProgress;
    SymbolTableEntry& sym = expandedTail();
    RootTyp const& t = sym.type();
    _expansionStatus = notYet;
    return t;
}

// .................................................................
EnumTypeSym::EnumTypeSym(SourceLocation const& loc,
                         string const* inname,
                         Scope* inparentScope,
                         EnumTyp const& intype)
  : SymbolTableEntry(loc)
  , _heldScope(new Scope(inparentScope, this))
  , _name(inname)
  , _type(intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("EnumType(" << (void*)this << ", " << (void*)_heldScope << ", "
                      << (inname ? *inname : string("")) << ")\n");
    assert(this == _heldScope->holder());
}

ostream& EnumTypeSym::print(ostream& os) const
{
    return os << "EnumTypeSym(scope=" << heldScope() << ", type=" << type() << ")";
}

EnumTyp const& EnumTypeSym::type(AstNode& ast)
{
    vector<string> nameVec;
    for (int i = 0, n = AST::enumType::idCount(ast); i < n; i++) {
        nameVec.push_back(astText(AST::enumType::id(ast, i)));
    }
    return TypeFactory::instance().enumT(nameVec);
}

// .................................................................
EnumValueSym::EnumValueSym(SourceLocation const& loc,
                           string const& inname,
                           Scope& inparentScope,
                           EnumTypeSym& owningEnum)
  : SymbolTableEntry(loc)
  , _name(inname)
  , _parentScope(inparentScope)
  , _owningEnum(owningEnum)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("EnumValue(" << (void*)this << ", " << inname << ")\n");
    ensureNoLeadingDollarId(_name, loc);
}

Expression* EnumValueSym::genExpression(const SourceLocation& loc) const
{
    SPLDBG("EnumValueSym::genExpression: " << (void*)this, Debug::IRGen);
    if (isCustomLiteral()) {
        return new CustomLiteralExpression(type(), loc, _name, _parentScope.holder()->name());
    }
    return new EnumExpression(type(), loc, _name);
}

bool EnumValueSym::isCustomLiteral() const
{
    return _parentScope.holder()->as<EnumTypeSym>().isCustomLiteral();
}

ostream& EnumValueSym::print(ostream& os) const
{
    return os << "EnumValueSym(" << name() << ", type=" << type() << ")";
}

RootTyp const& EnumValueSym::type() const
{
    return _parentScope.holder()->type();
}

// .................................................................
ErrorDummySym::ErrorDummySym(SourceLocation const& loc,
                             AstNode& inast,
                             string inname,
                             Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(inast)
  , _name(inname)
  , _parentScope(inparentScope)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("ErrorDummy(" << (void*)this << ", " << inname << ")\n");
}

ostream& ErrorDummySym::print(ostream& os) const
{
    return os << "ErrorDummySym(" << _name << ", ast=" << _astNode << ")";
}

RootTyp const& ErrorDummySym::type() const
{
    return TypeFactory::instance().unknownT();
}

// .................................................................
EventTimeAnnotationSym::EventTimeAnnotationSym(SourceLocation const& loc,
                                               AstNode& astIn,
                                               AstNode& attributeIn,
                                               AstNode* resolutionIn,
                                               AstNode* minimumGapIn,
                                               AstNode* lagIn,
                                               Scope& parentScopeIn)
  : SymbolTableEntry(loc)
  , _astNode(astIn)
  , attribute_(attributeIn)
  , resolution_(resolutionIn)
  , minimumGap_(minimumGapIn)
  , lag_(lagIn)
  , heldScope_(new Scope(&parentScopeIn, this))
  , attributeScope_(new Scope(&parentScopeIn, this))
  , resolutionScope_(new Scope(&parentScopeIn, this))
  , mode_(Held)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("EventTimeAnnotationSym(" << (void*)this << ", " << (void*)heldScope_ << ", "
                                    << (void*)&_astNode << ")\n");
}

EventTimeAnnotationSym::~EventTimeAnnotationSym()
{
    delete attributeScope_;
    delete resolutionScope_;
    delete heldScope_;
}

ostream& EventTimeAnnotationSym::print(ostream& os) const
{
    return os << "EventTimeAnnotationSym()";
}

// .................................................................
ExpressionSym::ExpressionSym(SourceLocation const& loc, AstNode& astNode, RootTyp const& intype)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(typeToScope(loc, intype, NULL))
  , _type(&intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("Expression(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&astNode
                        << ")\n");
}

ostream& ExpressionSym::print(ostream& os) const
{
    return os << "ExpressionSym(ast=" << _astNode << ", type=" << _type << ")";
}

// .................................................................
FormalConfigSym::FormalConfigSym(Scope* inparentScope, const string& inname)
  : SymbolTableEntry(SourceLocation::invalidSourceLocation)
  , _heldScope(new Scope(inparentScope, this))
  , _name(inname)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("FormalConfig(" << (void*)this << ", " << (void*)_heldScope << ", " << inname << ")\n");
    assert(this == _heldScope->holder());
}

ostream& FormalConfigSym::print(ostream& os) const
{
    return os << "FormalConfigSym(" << name() << ", scope=" << heldScope() << ")";
}

// .................................................................
HostPoolSym::HostPoolSym(SourceLocation const& loc, AstNode& inast)
  : SymbolTableEntry(loc)
  , _astNode(inast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("HostPool(" << (void*)this << ", " << (void*)&inast << ")\n");
    assert(AST::eqT::is(inast));
    AST::infixExpr::lhs(inast);
#ifndef NDEBUG
    AstNode& nameAst =
#endif
      AST::infixExpr::lhs(inast);
    assert(AST::idT::is(nameAst));
    ensureNoLeadingDollarId(name(), loc);
}

string HostPoolSym::name() const
{
    return astText(AST::infixExpr::lhs(_astNode));
}

Scope* HostPoolSym::parentScope() const
{
    return &SymbolTable::instance().configScope("hostPool");
}

ostream& HostPoolSym::print(ostream& os) const
{
    return os << "HostPoolSym(" << name() << ", ast=" << _astNode << ")";
}

RootTyp const& HostPoolSym::type() const
{
    return TypeFactory::instance().listT(TypeFactory::instance().rstringT());
}

Expression* HostPoolSym::genExpression(const SourceLocation& loc) const
{
    return new SymbolExpression(type(), sourceLocation(), name(), Expression::Other);
}

// .................................................................
IndirectSym::IndirectSym(const string& inname)
  : SymbolTableEntry(SourceLocation::invalidSourceLocation)
  , _heldScope(NULL)
  , _multiDefMap()
  , _name(inname)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("Indirect(" << (void*)this << ", " << inname << ")\n");
}

void IndirectSym::addDef(SymbolTableEntry& stream, SymbolTableEntry& attr)
{
    MODEL("AddIndirectRef(" << (void*)this << ", " << (void*)&stream << ", " << (void*)&attr
                            << ")\n");
    _multiDefMap[&stream] = &attr;
}

SymbolTableEntry& IndirectSym::firstSym() const
{
    return *_multiDefMap.begin()->second;
}

AstNode& IndirectSym::getFirstAst() const
{
    return firstSym().ast();
}

SymbolTableEntry* IndirectSym::base() const
{
    return _multiDefMap.size() == 1 ? _multiDefMap.begin()->first : NULL;
}

Scope* IndirectSym::heldScope() const
{
    return _heldScope = typeToScope(sourceLocation(), type(), _heldScope);
}

ostream& IndirectSym::print(ostream& os) const
{
    os << "IndirectSym({";
    printBare(os);
    os << '}';
    os << ", type=" << type();
    return os << ")";
}

ostream& IndirectSym::printBare(ostream& os) const
{
    vector<string> keys;
    typedef MultiDefMap::const_iterator I;
    for (I i = _multiDefMap.begin(); i != _multiDefMap.end(); i++) {
        keys.push_back(i->first->name());
    }
    sort(keys.begin(), keys.end());
    for (int i = 0, n = keys.size(); i < n; i++) {
        if (0 != i) {
            os << ", ";
        }
        os << keys.at(i) << '.' << name();
    }
    return os;
}

string IndirectSym::toString() const
{
    ostringstream os;
    printBare(os);
    return os.str();
}

RootTyp const& IndirectSym::type() const
{
    if (isAmbiguous()) {
        return TypeFactory::instance().unknownT();
    }
    return _multiDefMap.begin()->second->type();
}

Expression* IndirectSym::genExpression(const SourceLocation& loc) const
{
    SPLDBG("IndirectSym::genExpression: " << this, Debug::IRGen);

    Expression* lhs = NULL;
    Expression* rhs = NULL;
    typedef MultiDefMap::const_iterator I;
    // Find the (singular) stream that corresponds to the AttributeExpression
    for (I i = _multiDefMap.begin(); i != _multiDefMap.end(); i++) {
        SymbolTableEntry* base1 = i->first;
        if (base1->is<PrimitiveOperatorSym>()) {
            Expression* e = i->second->genExpression(loc);
            if (e) {
                return e;
            }
            continue; // Ignore this case, it goes boom below
        }
        SPLDBG("lhs: base: " << *base1, Debug::IRGen);
        Expression* e = base1->genExpression(loc);
        assert(e);

        // Have we seen this before?
        if (!lhs) {
            lhs = e;
            SymbolTableEntry* r = i->second;
            if (r->is<EnumValueSym>()) {
                return r->genExpression(loc);
            }
            assert(!r->is<ExpressionSym>());
            // Again, need to create it
            Expression* exp = r->genExpression(loc);
            assert(exp);
            rhs = exp;
        } else if (*lhs != *e) { // We have one already... Check to see if they are equivalent
            return NULL;         // Nope: mismatch
        }
    }

    if (!lhs || !rhs) {
        return NULL; // Nothing to do
    }

    // Okay, we have a unique LHS
    return new AttributeExpression(rhs->getType(), loc, *lhs, *rhs);
}

// .................................................................
ListTypeSym::ListTypeSym(SourceLocation const& loc, AstNode& astNode)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("ListType(" << (void*)this << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), astNode, *this);
}

string ListTypeSym::name() const
{
    return type().getName();
}

ostream& ListTypeSym::print(ostream& os) const
{
    return os << "ListTypeSym(" << _astNode << " @ " << sourceLocation() << ")";
}

RootTyp const& ListTypeSym::type() const
{
    if (NULL == _type) {
        TypeFactory& tf = TypeFactory::instance();
        RootTyp const& elemType =
          astSymbol(AST::typeArgs::type(AST::listType::typeArgs(_astNode), 0)).type();
        if (AST::listType::has_typeDims(_astNode)) {
            AstNode& boundAst = AST::typeDims::expr(AST::listType::typeDims(_astNode));
            ParserContext pCtx(sourceLocation().fileName());
            _type = &tf.listT(elemType, getBound(pCtx, boundAst));
        } else {
            _type = &tf.listT(elemType);
        }
    }
    assert(NULL != _type);
    return *_type;
}

// .................................................................
MapTypeSym::MapTypeSym(SourceLocation const& loc, AstNode& astNode)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("MapType(" << (void*)this << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), astNode, *this);
}

string MapTypeSym::name() const
{
    return type().getName();
}

ostream& MapTypeSym::print(ostream& os) const
{
    return os << "MapTypeSym(" << _astNode << " @ " << sourceLocation() << ")";
}

RootTyp const& MapTypeSym::type() const
{
    if (NULL == _type) {
        TypeFactory& tf = TypeFactory::instance();
        RootTyp const& keyType =
          astSymbol(AST::typeArgs::type(AST::mapType::typeArgs(_astNode), 0)).type();
        RootTyp const& valType =
          astSymbol(AST::typeArgs::type(AST::mapType::typeArgs(_astNode), 1)).type();
        if (AST::mapType::has_typeDims(_astNode)) {
            AstNode& boundAst = AST::typeDims::expr(AST::mapType::typeDims(_astNode));
            ParserContext pCtx(sourceLocation().fileName());
            _type = &tf.mapT(keyType, valType, getBound(pCtx, boundAst));
        } else {
            _type = &tf.mapT(keyType, valType);
        }
    }
    assert(NULL != _type);
    return *_type;
}

// .................................................................
PrimitiveOperatorSym::PrimitiveOperatorSym(Model const& inmodel,
                                           const string& inname,
                                           Scope* inparentScope,
                                           uint32_t intoolkitIndex)
  : OperatorSym(SourceLocation::invalidSourceLocation)
  , _ofScopes()
  , _enumsScope(new Scope(inparentScope, this))
  , _heldScope(new Scope(inparentScope, this))
  , _model(inmodel)
  , _name(inname)
  , _paramsScope(new Scope(inparentScope, this))
  , _toolkitIndex(intoolkitIndex)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("PrimitiveOperator(" << (void*)this << ", " << (void*)_heldScope << ", "
                               << (void*)_enumsScope << ", " << (void*)_paramsScope << ", "
                               << inname << ")\n");
    ensureNoLeadingDollarId(inname, sourceLocation());
    ScopeMap scopeMap;
    initOutputFunctions(scopeMap);
    initEnumerations();
    initParameters(scopeMap);
}

Scope* PrimitiveOperatorSym::ofsScope(int portId) const
{
    if (portId >= (int)_ofScopes.size()) {
        portId = (int)_ofScopes.size() - 1;
    }
    return _ofScopes[portId];
}

Scope* PrimitiveOperatorSym::ofsScope(const string& paramName) const
{
    ScopeMap::const_iterator it = _pfScopes.find(paramName);
    if (it != _pfScopes.end()) {
        return it->second;
    }
    return NULL;
}

void PrimitiveOperatorSym::initOutputFunctions(ScopeMap& scopeMap)
{
    initOutputFunctionsScopes(scopeMap);
    typedef vector<SPL::Operator::OutputPortSetPtr> PortVector;
    const PortVector& ports = _model.getOutputPorts().getPortSets();
    for (unsigned i = 0, n = ports.size(); i < n; i++) {
        initOutputFunctionsPort(scopeMap, i, *ports[i]);
    }
    if (_model.getOutputPorts().hasPortOpenSet()) {
        initOutputFunctionsPort(scopeMap, ports.size(), _model.getOutputPorts().getPortOpenSet());
    }
}

void PrimitiveOperatorSym::initOutputFunctionsPort(ScopeMap& scopeMap,
                                                   int portIndex,
                                                   const Port& port)
{
    assert((int)_ofScopes.size() == portIndex);
    if (port.hasOutputFunctions()) {
        string ofType = port.getOutputFunctions().getOutputFunctionType();
        _ofScopes.push_back(scopeMap[ofType]);
    } else {
        _ofScopes.push_back(NULL);
    }
}

void PrimitiveOperatorSym::initOutputFunctionsScopes(ScopeMap& scopeMap)
{
    typedef unordered_map<string, SPL::Operator::CustomOutputPtr> OFMap;
    const OFMap& ofs = _model.getContext().getCustomOutputFunctions();
    SourceLocation const& loc = sourceLocation();
    SymbolTable& symTab = SymbolTable::instance();
    symTab.push(NULL, *this);
    for (OFMap::const_iterator a = ofs.begin(); a != ofs.end(); a++) {
        typedef vector<Operator::CustomOutput::COF> Protos;
        const Protos& protos = a->second->getValues();
        OtherScopeHolderSym& sym = *new OtherScopeHolderSym(loc, NULL, _heldScope);
        symTab.push(NULL, sym);
        for (Protos::const_iterator iOF = protos.begin(); iOF != protos.end(); iOF++) {
            FunctionHeadSym& f =
              FunctionHeadSym::analyzeIntrinsicFunction(&symTab.currentScope(), iOF->proto_, false);
            f.setIsOutputFunction(true);
            f.setIsPseudoFunction(iOF->pseudoFunction_);
        }
        scopeMap[a->first] = &symTab.currentScope();
        symTab.pop(sym);
    }
    symTab.pop(*this);
}

void PrimitiveOperatorSym::initEnumerations()
{
    typedef unordered_map<string, SPL::Operator::EnumerationPtr> EnumMap;
    const EnumMap& enums = _model.getContext().getCustomLiterals();
    for (EnumMap::const_iterator e = enums.begin(); e != enums.end(); e++) {
        vector<string> nameVec = e->second->getOrderedValues();
        EnumTyp const& type1 = TypeFactory::instance().enumT(nameVec);
        EnumTypeSym& typeSym =
          *new EnumTypeSym(SourceLocation::invalidSourceLocation, &e->first, _enumsScope, type1);
        _enumsScope->def(typeSym);
        Scope& currScope = *typeSym.heldScope();
        for (int i = 0, n = nameVec.size(); i < n; i++) {
            EnumValueSym& valSym = *new EnumValueSym(SourceLocation::invalidSourceLocation,
                                                     nameVec.at(i), currScope, typeSym);
            currScope.def(valSym);
            _enumsScope->def(valSym);
        }
    }
}

void PrimitiveOperatorSym::initParameters(const ScopeMap& scopeMap)
{
    typedef unordered_map<string, SPL::Operator::ParameterPtr> ParamMap;
    const ParamMap& prms = _model.getParameters().getNamedParameters();
    for (ParamMap::const_iterator p = prms.begin(); p != prms.end(); p++) {
        _paramsScope->def(
          *new PrimitiveFormalSym(SourceLocation::invalidSourceLocation, *this, *p->second));
        if (p->second->hasCustomOutputFunction()) {
            const string& ofName = p->second->getCustomOutputFunction();
            ScopeMap::const_iterator it = scopeMap.find(ofName);
            if (it != scopeMap.end()) {
                _pfScopes.insert(make_pair(p->first, it->second));
            }
        }
    }
}

ostream& PrimitiveOperatorSym::print(ostream& os) const
{
    return os << "PrimitiveOperatorSym(" << name() << ", enums=" << enumsScope()
              << ", params=" << paramsScope() << ", toolkitIndex=" << toolkitIndex() << ")";
}

RootTyp const& PrimitiveOperatorSym::type() const
{
    return TypeFactory::instance().unknownT();
}

bool PrimitiveOperatorSym::nestedCOFsAllowed(unsigned port) const
{
    const Operator::OutputPort& oPort = _model.getOutputPorts().getPortAt(port);

    if (oPort.allowNestedCustomOutputFunctions()) {
        return true;
    }
    return false;
}

// .................................................................
NamespaceSym::NamespaceSym(const string& inname, Scope* inparentScope)
  : SymbolTableEntry(SourceLocation::invalidSourceLocation)
  , _heldScope(new Scope(inparentScope, this))
  , _name(inname)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("Namespace(" << (void*)this << ", " << (void*)_heldScope << ", " << inname << ")\n");
    if (!inname.empty()) {
        ensureNoLeadingDollarId(inname, sourceLocation());
    }
    assert(this == _heldScope->holder());
}

string NamespaceSym::name(AstNode& astNode)
{
    if (!AST::splNamespace::has_name(astNode)) {
        return DEFAULT_NS_NAME;
    }
    ostringstream os;
    prettyPrint(os, AST::splNamespace::name(astNode));
    return os.str();
}

ostream& NamespaceSym::print(ostream& os) const
{
    return os << "NamespaceSym(" << name() << ", scope=" << heldScope() << ")";
}

// .................................................................
OnPunctLogicSym::OnPunctLogicSym(SourceLocation const& loc, AstNode& astNode, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(inparentScope, this))
  , _isExpanded(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OnPunctLogic(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&astNode
                          << ")\n");
    assert(this == _heldScope->holder());
}

void OnPunctLogicSym::expand()
{
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    OperatorSym* op = parentScope()->holder()->as<OpInvokeSym>().op();
    _heldScope->addAll(SymbolTable::instance().logicScopeCurrentPunct());
    if (NULL != op && op->fullName() == "spl.utility::Custom") {
        _heldScope->addAll(SymbolTable::instance().logicScopeSubmit());
    }
}

string OnPunctLogicSym::name() const
{
    return astText(AST::opInvokeLogicOnPunct::id(_astNode));
}

ostream& OnPunctLogicSym::print(ostream& os) const
{
    return os << "OnPunctLogicSym(" << name() << ", scope=" << heldScope()
              << ", astNode=" << _astNode << ")";
}

#if 0
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
// .................................................................
OnWatermarkLogicSym::OnWatermarkLogicSym(SourceLocation const & loc, AstNode & astNode, Scope * inparentScope) :
    SymbolTableEntry(loc), _astNode(astNode), _heldScope(new Scope(inparentScope, this)), _isExpanded(false) {
  SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
  MODEL("OnWatermarkLogic(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&astNode << ")\n");
  assert(this == _heldScope->holder());
}

void OnWatermarkLogicSym::expand() {
  if (_isExpanded) {
    return;
  }
  _isExpanded = true;
  OperatorSym * op = parentScope()->holder()->as<OpInvokeSym>().op();
  _heldScope->addAll(SymbolTable::instance().logicScopeCurrentWatermark());
  if (NULL != op && op->fullName() == "spl.utility::Custom")
    _heldScope->addAll(SymbolTable::instance().logicScopeSubmit());
}

string OnWatermarkLogicSym::name() const {
  return astText(AST::opInvokeLogicOnWatermark::id(_astNode));
}

ostream & OnWatermarkLogicSym::print(ostream & os) const {
  return os << "OnWatermarkLogicSym(" << name() << ", scope=" << heldScope() << ", astNode=" << _astNode << ")";
}
#endif

// .................................................................
OnTupleLogicSym::OnTupleLogicSym(SourceLocation const& loc, AstNode& astNode, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(inparentScope, this))
  , _isExpanded(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OnTupleLogic(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&astNode
                          << ")\n");
    assert(this == _heldScope->holder());
}

void OnTupleLogicSym::expand()
{
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    OperatorSym* op = parentScope()->holder()->as<OpInvokeSym>().op();
    if (NULL != op && op->fullName() == "spl.utility::Custom") {
        _heldScope->addAll(SymbolTable::instance().logicScopeSubmit());
    }
    SymbolTableEntry* label = astSymbolPtr(AST::opInvokeLogicOnTuple::id(_astNode));
    Scope* scope = NULL == label ? NULL : label->heldScope();
    if (NULL != scope) {
        addAllIndirect(*_heldScope, *label);
    }
}

string OnTupleLogicSym::name() const
{
    return astText(AST::opInvokeLogicOnTuple::id(_astNode));
}

ostream& OnTupleLogicSym::print(ostream& os) const
{
    return os << "OnTupleLogicSym(" << name() << ", scope=" << heldScope()
              << ", astNode=" << _astNode << ")";
}

// .................................................................
OnProcessLogicSym::OnProcessLogicSym(SourceLocation const& loc,
                                     AstNode& astNode,
                                     Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(inparentScope, this))
  , _isExpanded(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OnProcessLogic(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&astNode
                            << ")\n");
    assert(this == _heldScope->holder());
}

void OnProcessLogicSym::expand()
{
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    OperatorSym* op = parentScope()->holder()->as<OpInvokeSym>().op();
    if (NULL != op && op->fullName() == "spl.utility::Custom") {
        _heldScope->addAll(SymbolTable::instance().logicScopeSubmit());
    } else {
        SysOut::errorln(SPL_FRONT_END_ONPROCESS_NOT_CUSTOM, sourceLocation());
    }
}

string OnProcessLogicSym::name() const
{
    return "onProcess";
}

ostream& OnProcessLogicSym::print(ostream& os) const
{
    return os << "OnProcessLogicSym(scope=" << heldScope() << ", astNode=" << _astNode << ")";
}

// .................................................................
OpInvokeActualSym::OpInvokeActualSym(SourceLocation const& loc,
                                     AstNode& astNode,
                                     Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _formal(NULL)
  , _heldScope(new Scope(inparentScope, this))
  , _isExpanded(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OpInvokeActual(" << (void*)this << ", " << (void*)_heldScope << ", " << name() << ", "
                            << (void*)&astNode << ")\n");
    assert(this == _heldScope->holder());
    setAstSymbol(AstPool::instance(), astNode, *this);
    setAstSymbol(AstPool::instance(), AST::opInvokeActual::id(astNode), *this);
}

void OpInvokeActualSym::expand(AstNode& opInvokeAst)
{
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    OpInvokeSym& opInvokeSym = astSymbol(opInvokeAst).as<OpInvokeSym>();

    // Add Custom output functions, if any
    OperatorSym* oper = opInvokeSym.op();
    if (oper && oper->is<PrimitiveOperatorSym>()) {
        const PrimitiveOperatorSym& prim = oper->as<PrimitiveOperatorSym>();
        const Scope* ofScope = prim.ofsScope(name());
        if (ofScope) {
            _heldScope->addAll(*ofScope);
        }
    }
    _heldScope->addAll(opInvokeSym.exprScope());
}

string OpInvokeActualSym::name() const
{
    return astText(AST::opInvokeActual::id(_astNode));
}

ostream& OpInvokeActualSym::print(ostream& os) const
{
    return os << "OpInvokeActualSym(" << _formal << ")";
}

void OpInvokeActualSym::setFormal(SymbolTableEntry* informal)
{
    assert(NULL == _formal && name() == informal->name().substr(1));
    _formal = informal;
}

RootTyp const& OpInvokeActualSym::type() const
{
    AstNode& actualAst = AST::opInvokeActual::opActual(_astNode);
    if (AST::opActualExprs::is(actualAst)) {
        return astSymbol(AST::opActualExprs::expr(actualAst, 0)).type();
    }
    if (AST::opActualName::is(actualAst)) {
        return astSymbol(AST::opActualName::name(actualAst)).type();
    }
    return astSymbol(AST::opActualType::type(actualAst)).type();
}

// .................................................................
OpInvokeOutputSym::OpInvokeOutputSym(SourceLocation const& loc,
                                     AstNode& astNode,
                                     Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _isExpanded(false)
  , _isOnLeft(true)
  , _lhsScope(new Scope(inparentScope, this))
  , _rhsScope(new Scope(inparentScope, this))
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OpInvokeOutput(" << (void*)this << ", " << (void*)_lhsScope << ", " << (void*)_rhsScope
                            << ", " << (void*)&astNode << ")\n");
    ensureNoLeadingDollarId(name(), loc);
}

void OpInvokeOutputSym::expand(AstNode& opInvokeAst)
{
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    AstNode& opInvokeHeadAst = AST::opInvoke::head(opInvokeAst);
    SymbolTableEntry& opSym = astSymbol(AST::opInvokeHead::opName(opInvokeHeadAst));
    // populate lhsScope
    SymbolTableEntry& labelSym = astSymbol(AST::opInvokeOutput::id(_astNode));
    addAllIndirect(*_lhsScope, labelSym);
    // populate rhsScope with attributes from input streams
    OpInvokeSym& opInvokeSym = astSymbol(opInvokeAst).as<OpInvokeSym>();
    _rhsScope->addAll(opInvokeSym.exprScope());
    // populate rhsScope with output functions from operator model
    if (opSym.is<PrimitiveOperatorSym>()) {
        AstNode& outputPortsAst = AST::opInvokeHead::outputs(opInvokeHeadAst);
        int portNumber = -1;
        for (int iPort = 0, nPorts = AST::opOutputs::outputCount(outputPortsAst); iPort < nPorts;
             iPort++) {
            AstNode& portAst = AST::opOutputs::output(outputPortsAst, iPort);
            string name1 = astText(AST::opOutput::id(portAst));
            if (labelSym.name() == name1) {
                portNumber = iPort;
            } else if (AST::opOutput::has_alias(portAst)) {
                string alias = astText(AST::as::id(AST::opOutput::alias(portAst)));
                if (labelSym.name() == alias) {
                    portNumber = iPort;
                }
            }
        }
        if (-1 == portNumber) {
            SysOut::errorln(SPL_FRONT_END_INVALID_PORT_NAME(labelSym.name()), sourceLocation());
        } else {
            Scope* outputFunctions = opSym.as<PrimitiveOperatorSym>().ofsScope(portNumber);
            if (NULL != outputFunctions) {
                _rhsScope->addAll(*outputFunctions);
            }
        }
    }
}

string OpInvokeOutputSym::name() const
{
    return astText(AST::opInvokeOutput::id(_astNode));
}

ostream& OpInvokeOutputSym::print(ostream& os) const
{
    return os << "OpInvokeOutputSym(" << name() << ", lhsScope=" << _lhsScope
              << ", rhsScope=" << _rhsScope << ", astNode=" << _astNode << ")";
}

void OpInvokeOutputSym::switchLeft()
{
    SPL::SymbolTable::instance().pop(_astNode);
    assert(!_isOnLeft);
    _isOnLeft = !_isOnLeft;
    SPL::SymbolTable::instance().pushOld(_astNode);
}

void OpInvokeOutputSym::switchRight()
{
    SPL::SymbolTable::instance().pop(_astNode);
    assert(_isOnLeft);
    _isOnLeft = !_isOnLeft;
    SPL::SymbolTable::instance().pushOld(_astNode);
}

// .................................................................
OpInvokeSym::OpInvokeSym(SourceLocation const& loc, AstNode& astNode, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _exprScope(new Scope(inparentScope, this))
  , _heldScope(new Scope(inparentScope, this))
  , _isExpanded(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OpInvoke(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)_exprScope
                      << ", " << (void*)&astNode << ")\n");

    setAstSymbol(AstPool::instance(), _astNode, *this);
    AstNode& head = AST::opInvoke::head(_astNode);
    AstNode& alias = AST::opInvokeHead::alias(head);
    if (AST::as::is(alias)) {
        setAstSymbol(AstPool::instance(), AST::as::id(alias), *this);
        ensureNoLeadingDollarId(astText(AST::as::id(alias)), loc);
        SymbolTable::instance().currentScope().def(*this);
    }
    assert(this == _heldScope->holder());
    ensureNoDuplicateClauses();
}

static ostream& printPortLabel(ostream& os,
                               AstNode& opInvokeAst,
                               SourceLocation const& opInvokeLoc,
                               AstNode& branchAst,
                               bool expectedIsInput)
{
    string const& streamName = astText(astChild(branchAst, 0));
    int portIndex;
    bool isInput;
    int countFound =
      findStreamInOpInvoke(streamName, opInvokeAst, NULL, &isInput, &portIndex, NULL);
    if (0 == countFound) {
        os << streamName;
        SysOut::errorln(SPL_FRONT_END_CANNOT_LOCATE_PORT_FOR_IDENTIFIER(streamName),
                        SourceLocationFactory::instance().create(opInvokeLoc, branchAst));
    } else {
        if (1 < countFound) {
            SysOut::errorln(SPL_FRONT_END_LOCATED_MULTIPLE_PORTS_FOR_IDENTIFIER(streamName),
                            SourceLocationFactory::instance().create(opInvokeLoc, branchAst));
        }
        if (isInput) {
            if (!expectedIsInput) {
                SysOut::errorln(SPL_FRONT_END_CANNOT_LOCATE_OUTPUT_PORT_FOR_IDENTIFIER(streamName),
                                SourceLocationFactory::instance().create(opInvokeLoc, branchAst));
            }
            AstNode& portAst = AST::opInputs::portInputs(
              AST::opInvokeHead::inputs(AST::opInvoke::head(opInvokeAst)), portIndex);
            for (int i = 0, n = AST::portInputs::idCount(portAst); i < n; i++) {
                if (0 < i) {
                    os << ", ";
                }
                os << astText(AST::portInputs::id(portAst, i));
            }
            if (AST::portInputs::has_alias(portAst)) {
                os << " as " << astText(AST::as::id(AST::portInputs::alias(portAst)));
            }
        } else {
            if (expectedIsInput) {
                SysOut::errorln(SPL_FRONT_END_CANNOT_LOCATE_INPUT_PORT_FOR_IDENTIFIER(streamName),
                                SourceLocationFactory::instance().create(opInvokeLoc, branchAst));
            }
            AstNode& portAst = AST::opOutputs::output(
              AST::opInvokeHead::outputs(AST::opInvoke::head(opInvokeAst)), portIndex);
            os << astText(AST::opOutput::id(portAst));
            if (AST::opOutput::has_alias(portAst)) {
                os << " as " << astText(AST::as::id(AST::opOutput::alias(portAst)));
            }
        }
    }
    return os;
}

void OpInvokeSym::ensureNoDuplicateClauses()
{
    AstNode& bodyAst = AST::opInvoke::body(_astNode);
    for (int iClause = 0, nClauses = astSize(bodyAst); iClause < nClauses; iClause++) {
        AstNode& clauseAst = astChild(bodyAst, iClause);
        bool const isLogic = AST::logic::is(clauseAst);
        bool const isPortLabel =
          !AST::actualClause::is(clauseAst) && !AST::configClause::is(clauseAst);
        unordered_set<string> labels;
        for (int iBranch = 0, nBranches = astSize(clauseAst); iBranch < nBranches; iBranch++) {
            AstNode& branchAst = astChild(clauseAst, iBranch);
            ostringstream os;
            bool isOnTupleOrOnPunct = false;
            if (isLogic) {
                if (AST::opInvokeLogicState::is(branchAst)) {
                    os << "state";
                } else if (AST::opInvokeLogicOnProcess::is(branchAst)) {
                    os << "onProcess";
                } else {
                    os << (AST::opInvokeLogicOnTuple::is(branchAst)
                             ? "onTuple"
                             : AST::opInvokeLogicOnPunct::is(branchAst) ? "onPunct" : "onWatermark")
                       << " ";
                    printPortLabel(os, _astNode, sourceLocation(), branchAst, true);
                    isOnTupleOrOnPunct = true;
                }
            } else if (isPortLabel) {
                printPortLabel(os, _astNode, sourceLocation(), branchAst,
                               AST::window::is(clauseAst));
            } else {
                os << astText(astChild(branchAst, 0));
            }
            string label = os.str();
            if (labels.find(label) != labels.end()) {
                SourceLocation const& loc =
                  SourceLocationFactory::instance().create(sourceLocation(), branchAst);
                if (CompilerConfiguration::instance().getBooleanValue(
                      CompilerConfiguration::SyntaxCheckOnly)) {
                    if (isOnTupleOrOnPunct) {
                        // TODO update this error message
                        SysOut::warnln(SPL_FRONT_END_OPERATOR_DUPLICATE_ONTUPLE_ONPUNCT_WARN(label),
                                       loc);
                    } else {
                        SysOut::warnln(SPL_FRONT_END_OPERATOR_DUPLICATE_LABEL_WARN(label), loc);
                    }
                } else {
                    if (isOnTupleOrOnPunct) {
                        // TODO update this error message
                        SysOut::errorln(SPL_FRONT_END_OPERATOR_DUPLICATE_ONTUPLE_ONPUNCT(label),
                                        loc);
                    } else {
                        SysOut::errorln(SPL_FRONT_END_OPERATOR_DUPLICATE_LABEL(label), loc);
                    }
                }
            } else {
                labels.insert(label);
            }
        }
    }
}

void OpInvokeSym::expand()
{
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    expandAttributes();
    expandEnumerations();
    if (false) {
        cerr << "OpInvokeSym::expand() finish " << this << endl;
    }
}

void OpInvokeSym::expandAttributes()
{
    AstNode& opInputsAst = AST::opInvokeHead::inputs(AST::opInvoke::head(_astNode));
    for (int iPort = 0, nPorts = AST::opInputs::portInputsCount(opInputsAst); iPort < nPorts;
         iPort++) {
        AstNode& portInputsAst = AST::opInputs::portInputs(opInputsAst, iPort);
        AstNode& firstStream = AST::portInputs::id(portInputsAst, 0);
        SymbolTableEntry& sym = astSymbol(firstStream);
        if (!sym.is<OtherSym>()) {
            addAllIndirect(*_exprScope, sym);
        }
    }
}

void OpInvokeSym::expandEnumerations()
{
    OperatorSym* operatorSym = op();
    if (NULL != operatorSym && operatorSym->is<PrimitiveOperatorSym>()) {
        PrimitiveOperatorSym& primitiveOperatorSym = operatorSym->as<PrimitiveOperatorSym>();
        Scope& enumsScope = *primitiveOperatorSym.enumsScope();
        for (int i = 0, n = enumsScope.size(); i < n; i++) {
            SymbolTableEntry& sym = enumsScope.get(i);
            if (sym.is<EnumValueSym>()) {
                string name1 = sym.name();
                if (!_exprScope->has(name1)) {
                    IndirectSym& ind = *new IndirectSym(name1);
                    ind.addDef(primitiveOperatorSym, sym);
                    _exprScope->def(ind);
                } else {
                    _exprScope->get(name1).as<IndirectSym>().addDef(primitiveOperatorSym, sym);
                }
            }
        }
    }
}

string OpInvokeSym::name() const
{
    AstNode& head = AST::opInvoke::head(_astNode);
    AstNode& alias = AST::opInvokeHead::alias(head);
    if (AST::as::is(alias)) {
        return astText(AST::as::id(alias));
    }
    AstNode& outs = AST::opInvokeHead::outputs(head);
    assert(0 < AST::opOutputs::outputCount(outs));
    return astText(AST::opOutput::id(AST::opOutputs::output(outs, 0)));
}

OperatorSym* OpInvokeSym::op() const
{
    SymbolTableEntry& result = astSymbol(AST::opInvokeHead::opName(AST::opInvoke::head(_astNode)));
    if (result.is<OperatorSym>()) {
        return &result.as<OperatorSym>();
    }
    return NULL;
}

ostream& OpInvokeSym::print(ostream& os) const
{
    return os << "OpInvokeSym("
              << "ast=" << _astNode << ", scope=" << heldScope() << ")";
}

RootTyp const& OpInvokeSym::type() const
{
    return TypeFactory::instance().unknownT();
}

bool OpInvokeSym::nestedCOFsAllowed(unsigned port) const
{
    // See if we have a primitive operator and if so, if it allows nested COFs
    OperatorSym* oper = op();
    if (NULL != oper) {
        if (oper->is<PrimitiveOperatorSym>()) {
            return oper->as<PrimitiveOperatorSym>().nestedCOFsAllowed(port);
        }
    }
    return false;
}

// .................................................................
OpInvokeWindowSym::OpInvokeWindowSym(SourceLocation const& loc,
                                     AstNode& astNode,
                                     Scope* inparentScope,
                                     Scope& windowScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(inparentScope, this))
  , _isExpanded(false)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OpInvokeWindow(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&astNode
                            << ")\n");
    assert(this == _heldScope->holder());
    _heldScope->addAll(windowScope);
}

void OpInvokeWindowSym::expand()
{
    if (_isExpanded) {
        return;
    }
    _isExpanded = true;
    SymbolTableEntry* label = astSymbolPtr(AST::opInvokeWindow::id(_astNode));
    Scope* scope = NULL == label ? NULL : label->heldScope();
    if (NULL != scope) {
        addAllIndirect(*_heldScope, *label);
    }
}

string OpInvokeWindowSym::name() const
{
    return astText(AST::opInvokeWindow::id(_astNode));
}

ostream& OpInvokeWindowSym::print(ostream& os) const
{
    return os << "OpInvokeWindowSym(" << name() << ", scope=" << heldScope()
              << ", astNode=" << _astNode << ")";
}

// .................................................................
OptionalTypeSym::OptionalTypeSym(SourceLocation const& loc, AstNode& childType)
  : SymbolTableEntry(loc)
  , _childType(childType)
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OptionalTypeSym(" << (void*)this << ", " << (void*)&childType << ")\n");
}

std::string OptionalTypeSym::name() const
{
    return type().getName();
}

ostream& OptionalTypeSym::print(ostream& os) const
{
    return os << "OptionalTypeSym(" << name() << ")";
}

RootTyp const& OptionalTypeSym::type() const
{
    if (NULL == _type) {
        SymbolTableEntry& typeSym = astSymbol(_childType);
        _type = &TypeFactory::instance().optionalT(typeSym.type());
    }
    return *_type;
}

// .................................................................
OtherScopeHolderSym::OtherScopeHolderSym(SourceLocation const& loc,
                                         AstNode* astNode,
                                         Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _heldScope(new Scope(inparentScope, this))
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("OtherScopeHolder(" << (void*)this << ", " << (void*)_heldScope << ")\n");
    assert(this == _heldScope->holder());
}

ostream& OtherScopeHolderSym::print(ostream& os) const
{
    return os << "OtherScopeHolderSym("
              << "ast=" << _astNode << ", scope=" << heldScope() << ")";
}

// .................................................................
OtherSym::OtherSym(SourceLocation const& loc,
                   const string& inname,
                   Scope* inparentScope,
                   AstNode* astNode,
                   RootTyp const* intype)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _name(inname)
  , _parentScope(inparentScope)
  , _type(NULL == intype ? TypeFactory::instance().unknownT() : *intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("Other(" << (void*)this << ", " << inname << ")\n");
    if (NULL != astNode) {
        setAstSymbol(AstPool::instance(), *astNode, *this);
    }
}

ostream& OtherSym::print(ostream& os) const
{
    return os << "OtherSym(" << name() << ", ast=" << _astNode << ", type=" << _type << ")";
}

Expression* OtherSym::genExpression(const SourceLocation& loc) const
{
    return new SymbolExpression(_type, loc, name(), Expression::Other);
}

// .................................................................
IsPresentReferenceSym::IsPresentReferenceSym(SourceLocation const& loc,
                                             Scope* inparentScope,
                                             AstNode& astNode,
                                             AstNode& lhs,
                                             RootTyp const& intype)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _lhs(lhs)
  , _name("isPresent")
  , _parentScope(inparentScope)
  , _type(intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("IsPresent(" << (void*)this << ", " << (void*)(&lhs) << ", "
                       << ")\n");
    // if(NULL != astNode)
    setAstSymbol(AstPool::instance(), astNode, *this);
}

ostream& IsPresentReferenceSym::print(ostream& os) const
{
    return os << "IsPresentReferenceSym(" << name() << ", ast=" << _astNode << ", type=" << _type
              << ")";
}

// .................................................................
UnwrapReferenceSym::UnwrapReferenceSym(SourceLocation const& loc,
                                       Scope* inparentScope,
                                       AstNode& astNode,
                                       AstNode& lhs,
                                       RootTyp const& intype)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _lhs(lhs)
  , _name("isPresent")
  , _parentScope(inparentScope)
  , _type(intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("UnwrapReference(" << (void*)this << ", " << (void*)(&_lhs) << ", "
                             << ")\n");
    // if(NULL != astNode)
    setAstSymbol(AstPool::instance(), astNode, *this);
}

ostream& UnwrapReferenceSym::print(ostream& os) const
{
    return os << "UnwrapReferenceSym(" << name() << ", ast=" << _astNode << ", type=" << _type
              << ")";
}

SymbolTableEntry& UnwrapReferenceSym::getLhsSymbol() const
{
    return astSymbol(_lhs);
}

// .................................................................

ParallelAnnotationSym::ParallelAnnotationSym(SourceLocation const& loc,
                                             AstNode& astIn,
                                             AstNode& widthIn,
                                             AstNode* hostTagsIn,
                                             const PartitionList& partitionsIn,
                                             AstNode* broadcastsIn,
                                             Scope& parentScopeIn)
  : SymbolTableEntry(loc)
  , _astNode(astIn)
  , _width(widthIn)
  , _hostTags(hostTagsIn)
  , _heldScope(new Scope(&parentScopeIn, this))
  , _partitions(partitionsIn)
  , _broadcasts(broadcastsIn)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("ParallelAnnotation(" << (void*)this << ", " << (void*)&_astNode << ", "
                                << (void*)&widthIn << ", " << _width << ")\n");
#ifndef NDEBUG
    if (!_partitions.empty()) {
        for (PartitionList::const_iterator it = _partitions.begin(); it != _partitions.end();
             ++it) {
            const Partition& s = *it;
            MODEL("AddParallelPartition(" << (void*)this << ", " << (void*)&s.port() << ", "
                                          << (void*)&s.attributes() << ")\n");
        }
    }
#endif
}

ostream& ParallelAnnotationSym::print(ostream& os) const
{
    return os << "ParallelAnnotationSym()";
}

void ParallelAnnotationSym::expand() {}

// .................................................................
PortAliasSym::PortAliasSym(SourceLocation const& loc, AstNode& portAst)
  : SymbolTableEntry(loc)
  , _portAst(portAst)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("PortAlias(" << (void*)this << ", " << (void*)&portAst << ")\n");
    assert(AST::opOutput::is(portAst) || AST::portInputs::is(portAst));
    ensureNoLeadingDollarId(name(), loc);
    setAstSymbol(AstPool::instance(), portAst, *this);
}

AstNode& PortAliasSym::ast() const
{
    return AST::as::id(AST::opOutput::is(_portAst) ? AST::opOutput::alias(_portAst)
                                                   : AST::portInputs::alias(_portAst));
}

Scope* PortAliasSym::heldScope() const
{
    return firstStreamSym().heldScope();
}

SymbolTableEntry& PortAliasSym::firstStreamSym() const
{
    return astSymbol(AST::opOutput::is(_portAst) ? AST::opOutput::id(_portAst)
                                                 : AST::portInputs::id(_portAst, 0));
}

Expression* PortAliasSym::genExpression(SourceLocation const& loc) const
{
    int portIndex = -1;
    int countFound = findStreamInOpInvoke(name(), SymbolTable::instance().currentOpInvoke().ast(),
                                          NULL, NULL, &portIndex, NULL);
    assert(0 < countFound && -1 != portIndex);
    if (1 < countFound) {
        SysOut::errorln(SPL_FRONT_END_LOCATED_MULTIPLE_PORTS_FOR_IDENTIFIER(name()), loc);
    }
    return new StreamSymbolExpression(type(), loc, name(), portIndex);
}

string PortAliasSym::name() const
{
    return astText(ast());
}

ostream& PortAliasSym::print(ostream& os) const
{
    return os << "PortAliasSym(" << name() << ", port=" << _portAst << ", scope=" << heldScope()
              << ")";
}

RootTyp const& PortAliasSym::type() const
{
    return firstStreamSym().type();
}

// .................................................................
PrimitiveFormalSym::PrimitiveFormalSym(SourceLocation const& loc,
                                       PrimitiveOperatorSym& inop,
                                       SPL::Operator::Parameter const& param)
  : SymbolTableEntry(loc)
  , _heldScope(NULL)
  , _op(inop)
  , _param(param)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("PrimitiveFormal(" << (void*)this << ", " << name() << ")\n");
}

int PrimitiveFormalSym::cardinality() const
{
    return _param.hasCardinality() ? _param.getCardinality() : -1;
}

Scope* PrimitiveFormalSym::heldScope() const
{
    return _heldScope = typeToScope(sourceLocation(), type(), _heldScope);
}

string PrimitiveFormalSym::name() const
{
    ostringstream os;
    os << '$' << _param.getName();
    return os.str();
}

ostream& PrimitiveFormalSym::print(ostream& os) const
{
    string nameSpaceName = _op.parentScope()->parentScope()->holder()->name();
    return os << "PrimitiveFormalSym(" << nameSpaceName << "::" << op().name() << "." << name()
              << ", cardinality " << cardinality() << ", type " << type() << ")";
}

RootTyp const& PrimitiveFormalSym::type() const
{
    TypeFactory& tf = TypeFactory::instance();
    if (!_param.hasType()) {
        return tf.unknownT();
    }
    string const& typeName = _param.getType();
    // The call to Utility::splType may cause symbols to be defined in the currrent scope.
    // Create a temporary scope to capture them since we don't really want them for anything
    // but the type check
    // Todo: There is a memory leak here.  This will be fixed later.
    OtherScopeHolderSym& sym =
      *new OtherScopeHolderSym(sourceLocation(), NULL, &SymbolTable::instance().currentScope());
    SymbolTable& symTab = SymbolTable::instance();
    symTab.push(NULL, sym);
    const RootTyp* type1 = Utility::splType(typeName);
    symTab.pop(sym);
    if (type1) {
        return *type1;
    }
    return _op.enumsScope()->get(typeName).as<EnumTypeSym>().type();
}

// .................................................................
SetTypeSym::SetTypeSym(SourceLocation const& loc, AstNode& astNode)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("SetType(" << (void*)this << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), astNode, *this);
}

string SetTypeSym::name() const
{
    return type().getName();
}

ostream& SetTypeSym::print(ostream& os) const
{
    return os << "SetTypeSym(" << _astNode << " @ " << sourceLocation() << ")";
}

RootTyp const& SetTypeSym::type() const
{
    if (NULL == _type) {
        TypeFactory& tf = TypeFactory::instance();
        RootTyp const& elemType =
          astSymbol(AST::typeArgs::type(AST::setType::typeArgs(_astNode), 0)).type();
        if (AST::setType::has_typeDims(_astNode)) {
            AstNode& boundAst = AST::typeDims::expr(AST::setType::typeDims(_astNode));
            ParserContext pCtx(sourceLocation().fileName());
            _type = &tf.setT(elemType, getBound(pCtx, boundAst));
        } else {
            _type = &tf.setT(elemType);
        }
    }
    assert(NULL != _type);
    return *_type;
}

// .................................................................

SimpleAnnotationSym::SimpleAnnotationSym(SourceLocation const& loc,
                                         AstNode& inAstNode,
                                         const string& inname,
                                         Scope& inParentScope)
  : SymbolTableEntry(loc)
  , _astNode(inAstNode)
  , _name(inname)
  , _heldScope(new Scope(&inParentScope, this))
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("SimpleAnnotation(" << (void*)this << ")\n");
}

ostream& SimpleAnnotationSym::print(ostream& os) const
{
    return os << "SimpleAnnotationSym()";
}

// .................................................................
SimpleTypeSym::SimpleTypeSym(SourceLocation const& loc, RootTyp const& intype)
  : SymbolTableEntry(loc)
  , _type(intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("SimpleType(" << (void*)this << ")\n");
}

string SimpleTypeSym::name() const
{
    return _type.getName();
}

ostream& SimpleTypeSym::print(ostream& os) const
{
    return os << "SimpleTypeSym(" << _type << ")";
}

// .................................................................
StreamSym::StreamSym(SourceLocation const& loc,
                     AstNode& astNode,
                     string const& infullName,
                     string const& inlocalName)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _fullName(infullName)
  , _localName(inlocalName)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("Stream(" << (void*)this << ", " << infullName << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), AST::opOutput::id(_astNode), *this);
    ensureNoLeadingDollarId(name(), loc);
}

Expression* StreamSym::genExpression(const SourceLocation& loc) const
{
    int portIndex = -1;
    const string& op = name();
    AstNode& currentOpInvoke = SymbolTable::instance().currentOpInvoke().ast();
    int countFound = findStreamInOpInvoke(op, currentOpInvoke, NULL, NULL, &portIndex, NULL);
    assert(0 < countFound && -1 != portIndex);
    if (1 < countFound) {
        SysOut::errorln(SPL_FRONT_END_LOCATED_MULTIPLE_PORTS_FOR_IDENTIFIER(name()), loc);
    }
    return new StreamSymbolExpression(type(), loc, fullName(), portIndex);
}

Scope* StreamSym::heldScope() const
{
    return tupleBodySym().heldScope();
}

string StreamSym::name() const
{
    return astText(AST::opOutput::id(_astNode));
}

ostream& StreamSym::print(ostream& os) const
{
    return os << "StreamSym(" << name() << ", fullName=" << _fullName
              << ", localName=" << _localName << ", heldScope=" << heldScope() << ")";
}

void StreamSym::expand()
{
    // Make sure the associated tuple body sym has been expanded
    tupleBodySym().expand();
}

SymbolTableEntry& StreamSym::tupleBodySym() const
{
    return astSymbol(AST::streamType::tupleBody(AST::opOutput::streamType(_astNode)));
}

RootTyp const& StreamSym::type() const
{
    return tupleBodySym().type();
}

// .................................................................
RStringTypeSym::RStringTypeSym(SourceLocation const& loc, AstNode& astNode)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("RStringType(" << (void*)this << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), astNode, *this);
}

string RStringTypeSym::name() const
{
    return type().getName();
}

ostream& RStringTypeSym::print(ostream& os) const
{
    return os << "RStringTypeSym(" << _astNode << " @ " << sourceLocation() << ")";
}

RootTyp const& RStringTypeSym::type() const
{
    if (NULL == _type) {
        TypeFactory& tf = TypeFactory::instance();
        if (AST::rstringType::has_typeDims(_astNode)) {
            AstNode& boundAst = AST::typeDims::expr(AST::rstringType::typeDims(_astNode));
            ParserContext pCtx(sourceLocation().fileName());
            _type = &tf.rstringT(getBound(pCtx, boundAst));
        } else {
            _type = &tf.rstringT();
        }
    }
    assert(NULL != _type);
    return *_type;
}

// .................................................................
ThreadingAnnotationSym::ThreadingAnnotationSym(SourceLocation const& loc,
                                               AstNode& inast,
                                               AstNode& modelin,
                                               AstNode* threadsin,
                                               AstNode* elasticin,
                                               Scope& parentScopeIn)
  : SymbolTableEntry(loc)
  , _astNode(inast)
  , _model(modelin)
  , _threads(threadsin)
  , _elastic(elasticin)
  , _heldScope(new Scope(&parentScopeIn, this))
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("ThreadingAnnotationSym(" << (void*)this << ", " << (void*)_heldScope << ", "
                                    << (void*)&_astNode << ")\n");
}

ostream& ThreadingAnnotationSym::print(ostream& os) const
{
    return os << "ThreadingAnnotationSym()";
}

// .................................................................
TupleAttribSym::TupleAttribSym(SourceLocation const& loc, AstNode& astNode, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _expansionStatus(notYet)
  , _heldScope(new Scope(inparentScope, this))
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("TupleAttrib(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&_astNode
                         << ")\n");
    assert(this == _heldScope->holder());
}

void TupleAttribSym::expand()
{
    if (success == _expansionStatus || failure == _expansionStatus) {
        return;
    }
    if (inProgress == _expansionStatus) {
        SysOut::errorln(SPL_FRONT_END_CIRCULAR_TUPLE_DEFINITION, sourceLocation());
        _expansionStatus = failure;
        _type = &TypeFactory::instance().unknownT();
        return;
    }
    _expansionStatus = inProgress;
    for (int i = 0, n = AST::tupleAttrib::attributeDeclCount(_astNode); i < n; i++) {
        AstNode& attributeDeclAst = AST::tupleAttrib::attributeDecl(_astNode, i);
        AstNode& attributeTypeAst = AST::attributeDecl::type(attributeDeclAst);
        SymbolTableEntry& typeSym = astSymbol(attributeTypeAst);
        typeSym.heldScope();
    }
    if (failure == _expansionStatus) {
        return;
    }
    assert(NULL == _type);
    if (0 == _heldScope->size()) {
        _type = &TypeFactory::instance().unknownT();
    } else {
        _type = &TypeFactory::instance().tupleT(*_heldScope);
    }
    _expansionStatus = success;
}

ostream& TupleAttribSym::print(ostream& os) const
{
    os << "TupleAttribSym(" << _astNode << ", type=" << _type << ", scope=";
    switch (_expansionStatus) {
        case notYet:
            os << "not_yet_expanded";
            break;
        case inProgress:
            os << "expansion_in_progress";
            break;
        case failure:
            os << "circular_tuple_type";
            break;
        case success:
            os << heldScope();
            break;
    }
    return os << ")";
}

RootTyp const& TupleAttribSym::type() const
{
    if (NULL == _type) {
        const_cast<TupleAttribSym*>(this)->expand();
    }
    assert(NULL != _type);
    return *_type;
}

// .................................................................
TupleExtendSym::TupleExtendSym(SourceLocation const& loc, AstNode& astNode, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(astNode)
  , _expansionStatus(notYet)
  , _heldScope(new Scope(inparentScope, this))
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("TupleExtend(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&astNode
                         << ")\n");
    assert(this == _heldScope->holder());
}

void TupleExtendSym::expand()
{
    if (success == _expansionStatus || failure == _expansionStatus) {
        return;
    }
    if (inProgress == _expansionStatus) {
        SysOut::errorln(SPL_FRONT_END_CIRCULAR_TUPLE_DEFINITION, sourceLocation());
        _expansionStatus = failure;
        _type = &TypeFactory::instance().unknownT();
        return;
    }
    _expansionStatus = inProgress;
    for (int i = 0, n = AST::tupleExtend::tupleExtendeeCount(_astNode); i < n; i++) {
        AstNode& extendeeAst = AST::tupleExtend::tupleExtendee(_astNode, i);
        SymbolTableEntry& extendeeSym = astSymbol(
          AST::tupleType::is(extendeeAst) ? AST::tupleType::tupleBody(extendeeAst) : extendeeAst);
        if (symIsTupleType(extendeeSym)) {
            RootTyp const& extendeeType = extendeeSym.type();
            if (extendeeType.getMetaType() != MetaType::UNKNOWN) {
                Scope* scope = extendeeSym.heldScope();
                assert(NULL != scope && extendeeType.getMetaType() == MetaType::TUPLE);
                for (int i2 = 0, n2 = scope->size(); i2 < n2; i2++) {
                    SymbolTableEntry& sym2 = scope->get(i2);
                    if (_heldScope->has(sym2.name())) {
                        SymbolTableEntry& sym1 = _heldScope->get(sym2.name());
                        if (sym1.type() != sym2.type()) {
                            SysOut::errorln(SPL_FRONT_END_DUPLICATE_ATTRIBUTE(sym1.name()),
                                            SourceLocationFactory::instance().create(
                                              sourceLocation(), extendeeAst));
                        }
                    }
                }
                _heldScope->addAll(*scope);
            }
        } else {
            SysOut::errorln(
              SPL_FRONT_END_TUPLE_EXTENDEE_NOT_TUPLE,
              SourceLocationFactory::instance().create(sourceLocation(), extendeeAst));
        }
    }
    if (failure == _expansionStatus) {
        return;
    }
    _expansionStatus = success;
    assert(NULL == _type);
    if (0 == _heldScope->size()) {
        _type = &TypeFactory::instance().unknownT();
    } else {
        _type = &TypeFactory::instance().tupleT(*_heldScope);
    }
}

ostream& TupleExtendSym::print(ostream& os) const
{
    os << "TupleExtendSym(" << _astNode << ", type=" << _type << ", scope=";
    switch (_expansionStatus) {
        case notYet:
            os << "not_yet_expanded";
            break;
        case inProgress:
            os << "expansion_in_progress";
            break;
        case failure:
            os << "circular_tuple_type";
            break;
        case success:
            os << heldScope();
            break;
    }
    return os << ")";
}

RootTyp const& TupleExtendSym::type() const
{
    if (NULL == _type) {
        const_cast<TupleExtendSym*>(this)->expand();
    }
    assert(NULL != _type);
    return *_type;
}

// .................................................................
TupleLiteralSym::TupleLiteralSym(SourceLocation const& loc, AstNode& inast, Scope* inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(inast)
  , _heldScope(new Scope(inparentScope, this))
  , _type(NULL)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("TupleLiteral(" << (void*)this << ", " << (void*)_heldScope << ", " << (void*)&inast
                          << ")\n");
    assert(this == _heldScope->holder());
}

TupleLiteralSym::TupleLiteralSym(SourceLocation const& loc,
                                 AstNode& inast,
                                 Scope& inparentScope,
                                 TupleTyp const& intype)
  : SymbolTableEntry(loc)
  , _astNode(inast)
  , _heldScope(new Scope(loc, intype, &inparentScope))
  , _type(&intype)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("TupleLiteral(" << (void*)this << ", " << (void*)_heldScope << ")\n");
}

ostream& TupleLiteralSym::print(ostream& os) const
{
    return os << "TupleLiteralSym("
              << "ast=" << _astNode << ", scope=" << heldScope() << ")";
}

RootTyp const& TupleLiteralSym::type() const
{
    if (NULL == _type) {
        vector<string> names;
        vector<RootTyp const*> types;
        for (int i = 0, n = AST::tupleLiteral::attributeAssignCount(_astNode); i < n; i++) {
            AstNode& child = AST::tupleLiteral::attributeAssign(_astNode, i);
            RootTyp const& type1 = astSymbol(AST::attributeAssign::expr(child)).type();
            names.push_back(astText(AST::attributeAssign::id(child)));
            types.push_back(&type1);
        }
        _type = &TypeFactory::instance().tupleT(names, types);
    }
    return *_type;
}

// .................................................................
TypeFormalSym::TypeFormalSym(SourceLocation const& loc, Scope& inparentScope, AstNode& astNode)
  : SymbolTableEntry(loc)
  , _type(TypeFormalSym::type(inparentScope, astNode))
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("TypeFormal(" << (void*)this << ", " << (void*)&astNode << ")\n");
    setAstSymbol(AstPool::instance(), astNode, *this);
    setAstSymbol(AstPool::instance(), AST::typeFormal::id(astNode), *this);
    ensureNoLeadingDollarId(name(), loc);
}

string TypeFormalSym::name() const
{
    return _type.getIdentifier();
}

Scope* TypeFormalSym::parentScope() const
{
    return _type.getFunction()->heldScope();
}

ostream& TypeFormalSym::print(ostream& os) const
{
    return os << "TypeFormalSym(" << _type.getConstraint() << ' ' << _type.getIdentifier() << ')';
}

RootTyp const& TypeFormalSym::type() const
{
    return _type;
}

TypeFormal const& TypeFormalSym::type(Scope& parentScope, AstNode& astNode)
{
    FunctionHeadSym const& functionHead = parentScope.holder()->as<FunctionHeadSym>();
    string constraint = astText(AST::typeFormal::typeFormalConstraint(astNode));
    string identifier = astText(AST::typeFormal::id(astNode));
    return TypeFactory::instance().typeFormalT(&functionHead, constraint, identifier);
}

// .................................................................
VariableSym::VariableSym(SourceLocation const& loc,
                         AstNode& defAst,
                         AstNode& inidAst,
                         Scope& inparentScope)
  : SymbolTableEntry(loc)
  , _astNode(defAst)
  , _idAst(inidAst)
  , _isMutable(false)
  , _isPublic(false)
  , _isStatic(false)
  , _name(astText(inidAst))
  , _parentScope(inparentScope)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("Variable(" << (void*)this << ", " << name() << ", " << (void*)&defAst << ", "
                      << (void*)&inidAst << ")\n");
    AstNode& modifiersAst = AST::localDecl::is(defAst) ? AST::localDecl::modifiers(defAst)
                                                       : AST::forInit::modifiers(defAst);
    for (int i = 0, n = AST::modifiers::modifierCount(modifiersAst); i < n; i++) {
        AstNode& modAst = AST::modifiers::modifier(modifiersAst, i);
        if (AST::mutableT::is(modAst)) {
            _isMutable = true;
        } else if (AST::publicT::is(modAst)) {
            _isPublic = true;
        } else if (AST::staticT::is(modAst)) {
            _isStatic = true;
        }
    }
    setAstSymbol(AstPool::instance(), inidAst, *this);
    assert(isVarDefAst(defAst));
    ensureNoLeadingDollarId(name(), loc);
    if (!_isMutable) {
        if (AST::localDecl::is(defAst)) {
            for (int i = 0, n = AST::localDecl::localDeclItemCount(defAst); i < n; i++) {
                AstNode& itemAst = AST::localDecl::localDeclItem(defAst, i);
                if (_name == astText(AST::localDeclItem::id(itemAst)) &&
                    AST::noneT::is(AST::localDeclItem::expr_q(itemAst))) {
                    SysOut::errorln(SPL_FRONT_END_MISSING_INITIALIZER, sourceLocation());
                }
            }
        }
    }
}

Scope* VariableSym::heldScope() const
{
    AstNode& typeAst =
      AST::localDecl::is(_astNode) ? AST::localDecl::type(_astNode) : AST::forInit::type(_astNode);
    SymbolTableEntry* e = astSymbolPtr(typeAst);
    return NULL == e ? NULL : e->heldScope();
}

bool VariableSym::isStatic(AstNode& ast)
{
    AstNode& modsAst =
      AST::localDecl::is(ast) ? AST::localDecl::modifiers(ast) : AST::forInit::modifiers(ast);
    for (int i = 0, n = AST::modifiers::modifierCount(modsAst); i < n; i++) {
        if (AST::staticT::is(AST::modifiers::modifier(modsAst, i))) {
            return true;
        }
    }
    return false;
}

ostream& VariableSym::print(ostream& os) const
{
    return os << "VariableSym(" << _name << ", type=" << type() << ")";
}

RootTyp const& VariableSym::type() const
{
    AstNode& typeAst =
      AST::localDecl::is(_astNode) ? AST::localDecl::type(_astNode) : AST::forInit::type(_astNode);
    return astSymbol(typeAst).type();
}

Expression* VariableSym::genExpression(const SourceLocation& loc) const
{
    return new SymbolExpression(type(), loc, _name,
                                _parentScope.holder()->is<OpInvokeSym>() ? Expression::State
                                                                         : Expression::Local);
}

// .................................................................
ViewAnnotationSym::ViewAnnotationSym(SourceLocation const& loc,
                                     AstNode& inast,
                                     AstNode& nameIn,
                                     AstNode& portIn,
                                     AstNode& sampleSizeIn,
                                     AstNode* bufferSizeIn,
                                     AstNode* bufferTimeIn,
                                     AstNode* descriptionIn,
                                     AstNode* attributesIn,
                                     AstNode* filterIn,
                                     AstNode* activateOptionIn,
                                     Scope& inParentScope)
  : SymbolTableEntry(loc)
  , _astNode(inast)
  , _name(nameIn)
  , _port(portIn)
  , _sampleSize(sampleSizeIn)
  , _bufferSize(bufferSizeIn)
  , _bufferTime(bufferTimeIn)
  , _description(descriptionIn)
  , _attributes(attributesIn)
  , _filter(filterIn)
  , _activateOption(activateOptionIn)
  , _heldScope(new Scope(&inParentScope, this))
  , _portScope(new Scope(&inParentScope, this))
  , _attributesScope(new Scope(&inParentScope, this))
  , _mode(Held)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceSymbolCreation);
    MODEL("ViewAnnotation(" << (void*)this << ", " << viewName() << ", " << (void*)&portIn
                            << ")\n");
}

ViewAnnotationSym::~ViewAnnotationSym()
{
    delete _portScope;
    delete _attributesScope;
    delete _heldScope;
}

Scope* ViewAnnotationSym::heldScope() const
{
    if (_mode == Port) {
        return _portScope;
    } else if (_mode == Attribute) {
        return _attributesScope;
    }
    return _heldScope;
}

ostream& ViewAnnotationSym::print(ostream& os) const
{
    return os << "ViewAnnotationSym(" << _name << ", type=" << type() << ")";
}
