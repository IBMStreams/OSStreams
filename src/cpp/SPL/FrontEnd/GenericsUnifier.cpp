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

#include <SPL/FrontEnd/GenericsUnifier.h>

#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/Debug.h>
#include <UTILS/HashMapHelpers.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

static bool isVariable(RootTyp const& type)
{
    return type.is<TypeFormal>();
}

static bool isVariable(TypeBound const& bound)
{
    return bound.isVariable();
}

static bool occursAnywhereIn(BoundsFormal const& var, TypeBound const& x)
{
    return x.isVariable() && &var == &x.toVar();
}

static bool occursAnywhereIn(TypeFormal const& var, RootTyp const& x)
{
    switch (x.getMetaType()) {
        case MetaType::UNKNOWN:
        case MetaType::EMPTYCURLY:
        case MetaType::BOOLEAN:
        case MetaType::ENUM:
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
        case MetaType::TIMESTAMP:
        case MetaType::RSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
        case MetaType::XML:
            return false;
        case MetaType::TYPEFORMAL:
            return var == x;
        case MetaType::BSTRING:
            return false;
        case MetaType::LIST:
            return occursAnywhereIn(var, x.as<ListTyp>().getElementType());
        case MetaType::BLIST:
            return occursAnywhereIn(var, x.as<BListTyp>().getElementType());
        case MetaType::SET:
            return occursAnywhereIn(var, x.as<SetTyp>().getElementType());
        case MetaType::BSET:
            return occursAnywhereIn(var, x.as<BSetTyp>().getElementType());
        case MetaType::MAP:
            return occursAnywhereIn(var, x.as<MapTyp>().getKeyType()) ||
                   occursAnywhereIn(var, x.as<MapTyp>().getValueType());
        case MetaType::BMAP:
            return occursAnywhereIn(var, x.as<BMapTyp>().getKeyType()) ||
                   occursAnywhereIn(var, x.as<BMapTyp>().getValueType());
        case MetaType::TUPLE: {
            for (int i = 0, n = x.as<TupleTyp>().getNumberOfAttributes(); i < n; i++) {
                if (occursAnywhereIn(var, x.as<TupleTyp>().getAttributeType(i))) {
                    return true;
                }
            }
            return false;
        }
        case MetaType::OPTIONAL:
            return occursAnywhereIn(var, x.as<OptionalTyp>().getUnderlyingType());

        case MetaType::NUL:
            // nulls won't actually appear, but optionals are good enough for this purpose
            return var.isOptional();

        default:
            break;
    }
    cerr << "unexpected type " << x << endl;
    assert(!"this statement should never be reached");
    return false;
}

// -----------------------------------------------------------------
class UnionFindConstraint
{
    typedef unordered_set<string> StringSet;
    typedef unordered_map<string, StringSet> ConstraintMap;
    static ConstraintMap CONSTRAINT_MAP;
    StringSet _possibleValues;
    UnionFindConstraint* _representative;
    static void ensureInitialized()
    {
        ConstraintMap& m = CONSTRAINT_MAP;
        if (!m.empty()) {
            return;
        }
        m["integral"].insert("int8");
        m["integral"].insert("int16");
        m["integral"].insert("int32");
        m["integral"].insert("int64");
        m["integral"].insert("uint8");
        m["integral"].insert("uint16");
        m["integral"].insert("uint32");
        m["integral"].insert("uint64");
        m["integral"].insert("int8");
        m["float"].insert("float32");
        m["float"].insert("float64");
        m["decimal"].insert("decimal32");
        m["decimal"].insert("decimal64");
        m["decimal"].insert("decimal128");
        m["floatingpoint"].insert(m["float"].begin(), m["float"].end());
        m["floatingpoint"].insert(m["decimal"].begin(), m["decimal"].end());
        m["complex"].insert("complex32");
        m["complex"].insert("complex64");
        m["numeric"].insert(m["integral"].begin(), m["integral"].end());
        m["numeric"].insert(m["floatingpoint"].begin(), m["floatingpoint"].end());
        m["numeric"].insert(m["complex"].begin(), m["complex"].end());
        m["string"].insert("rstring");
        m["string"].insert("ustring");
        m["string"].insert("bstring");
        m["andorable"].insert(m["integral"].begin(), m["integral"].end());
        m["andorable"].insert("boolean");
        m["subtractable"].insert(m["numeric"].begin(), m["numeric"].end());
        m["subtractable"].insert("timestamp");
        m["addable"].insert(m["subtractable"].begin(), m["subtractable"].end());
        m["addable"].insert(m["string"].begin(), m["string"].end());
        m["ordered"].insert("boolean");
        m["ordered"].insert("enum");
        m["ordered"].insert(m["integral"].begin(), m["integral"].end());
        m["ordered"].insert(m["floatingpoint"].begin(), m["floatingpoint"].end());
        m["ordered"].insert("timestamp");
        m["ordered"].insert(m["string"].begin(), m["string"].end());
        m["ordered"].insert("blob");
        m["primitive"].insert(m["ordered"].begin(), m["ordered"].end());
        m["primitive"].insert(m["complex"].begin(), m["complex"].end());
        m["list"].insert("list");
        m["list"].insert("blist");
        m["map"].insert("map");
        m["map"].insert("bmap");
        m["set"].insert("set");
        m["set"].insert("bset");
        m["enum"].insert("enum");
        m["tuple"].insert("tuple");
        m["optional"].insert("optional");
        m["collection"].insert(m["list"].begin(), m["list"].end());
        m["collection"].insert(m["map"].begin(), m["map"].end());
        m["collection"].insert(m["set"].begin(), m["set"].end());
        m["composite"].insert(m["collection"].begin(), m["collection"].end());
        m["composite"].insert("tuple");
        m["composite"].insert("optional");
        m["equatable"].insert(m["primitive"].begin(), m["primitive"].end());
        m["equatable"].insert(m["composite"].begin(), m["composite"].end());
        // xml types are not equatable
        m["xml"].insert("xml");
        m["primitive"].insert("xml");
        m["any"].insert(m["primitive"].begin(), m["primitive"].end());
        m["any"].insert(m["composite"].begin(), m["composite"].end());
        m["any"].insert(m["optional"].begin(), m["optional"].end());
    }
    UnionFindConstraint* compressPath()
    {
        if (this == _representative) {
            return this;
        }
        assert(NULL != _representative);
        _representative = _representative->compressPath();
        return _representative;
    }
    static ostream& printStringSet(ostream& os, StringSet& s)
    {
        os << '{';
        bool first = true;
        for (StringSet::const_iterator i = s.begin(); i != s.end(); i++) {
            if (first) {
                first = false;
            } else {
                os << ", ";
            }
            os << *i;
        }
        return os << '}';
    }

  public:
    UnionFindConstraint(string const& constraint)
    {
        ensureInitialized();
        ConstraintMap::const_iterator i = CONSTRAINT_MAP.find(constraint);
        assert(i != CONSTRAINT_MAP.end());
        _possibleValues.insert(i->second.begin(), i->second.end());
        _representative = this;
    }
    bool match(UnionFindConstraint& other)
    {
        UnionFindConstraint* rep1 = this->compressPath();
        UnionFindConstraint* rep2 = other.compressPath();
        assert(NULL != rep1 && NULL != rep2);
        StringSet &pv1 = rep1->_possibleValues, &pv2 = rep2->_possibleValues;
        StringSet intersection;
        for (StringSet::const_iterator i = pv1.begin(); i != pv1.end(); i++) {
            if (pv2.end() != pv2.find(*i)) {
                intersection.insert(*i);
            }
        }
        rep1->_possibleValues = intersection;
        rep2->_representative = rep1;
        return !intersection.empty();
    }
    bool match(RootTyp const& type)
    {
        assert(!isVariable(type));
        StringSet& pv = compressPath()->_possibleValues;
        string const& typeName = type.getMetaType().toString();
        if (pv.end() == pv.find(typeName)) {
            pv.clear();
        } else {
            pv.clear();
            pv.insert(typeName);
        }
        return !pv.empty();
    }
    ostream& print(ostream& os) { return printStringSet(os, compressPath()->_possibleValues); }
    static bool satisfies(RootTyp const& type, string const& constraint, bool useUnderlyingType)
    {
        if (type.getMetaType() == MetaType::UNKNOWN) {
            return true;
        }
        string meta = ((useUnderlyingType && type.is<OptionalTyp>())
                         ? type.as<OptionalTyp>().getUnderlyingType().getMetaType()
                         : type.getMetaType())
                        .toString();

        ensureInitialized();
        ConstraintMap::const_iterator i = CONSTRAINT_MAP.find(constraint);
        if (i == CONSTRAINT_MAP.end()) {
            return meta == constraint;
        } else {
            return i->second.end() != i->second.find(meta);
        }
    }
};

UnionFindConstraint::ConstraintMap UnionFindConstraint::CONSTRAINT_MAP;

bool SPL::satisfiesConstraint(RootTyp const& type, string const& constraint, bool useUnderlyingType)
{
    bool result = UnionFindConstraint::satisfies(type, constraint, useUnderlyingType);
    if (false) {
        cerr << "satisfiesConstraint(" << type << ", " << constraint << ") -> " << result << endl;
    }
    return result;
}

// -----------------------------------------------------------------
class GenericsUnifierImpl : public GenericsUnifier
{
    typedef unordered_map<BoundsFormal const*,
                          TypeBound const*,
                          BoundsFormal::Hash,
                          BoundsFormal::Equal>
      BoundsMap;
    typedef unordered_map<TypeFormal const*, RootTyp const*, TypeFormal::Hash, TypeFormal::Equal>
      TypeMap;
    BoundsMap _boundsMap;
    bool _isFailure;
    TypeMap _typeMap;

  public:
    GenericsUnifierImpl()
      : _isFailure(false)
    {}
    GenericsUnifierImpl* fail()
    {
        _isFailure = true;
        return this;
    }
    GenericsUnifierImpl* add(BoundsFormal const* const var, TypeBound const* const x)
    {
        _boundsMap[var] = x;
        return this;
    }
    GenericsUnifierImpl* add(const TypeFormal* const var, const RootTyp* const x)
    {
        SPLDBG(__PRETTY_FUNCTION__ << ": adding var " << *var << " to map for type " << *x,
               Debug::TraceTypeUnification);
        _typeMap[var] = x;
        return this;
    }
    GenericsUnifierImpl* checkAllConstraints();
    TypeBound const& get(BoundsFormal const& b) const
    {
        TypeBound const* bb = _boundsMap.find(&b)->second;
        assert(NULL != bb);
        if (isVariable(*bb) && has(bb->toVar())) {
            return get(bb->toVar());
        }
        return *bb;
    }
    RootTyp const& get(TypeFormal const& t) const
    {
        RootTyp const* tt = _typeMap.find(&t)->second;
        assert(NULL != tt);
        SPLDBG(__PRETTY_FUNCTION__ << ": getting the typeMap for " << t << " which is " << *tt,
               Debug::TraceTypeUnification);
        return (tt->is<TypeFormal>() && has(tt->as<TypeFormal>())) ? get(tt->as<TypeFormal>())
                                                                   : *tt;
    }
    bool has(BoundsFormal const& b) const { return _boundsMap.end() != _boundsMap.find(&b); }
    bool has(TypeFormal const& t) const
    {
        SPLDBG(__PRETTY_FUNCTION__ << ": checking if the typeMap has a mapping for " << t,
               Debug::TraceTypeUnification);
        return _typeMap.end() != _typeMap.find(&t);
    }
    bool isFailure() const { return _isFailure; }
    GenericsUnifierImpl* unifyBounds(TypeBound const&, TypeBound const&);
    GenericsUnifierImpl* unifyBoundsFormal(TypeBound const&, TypeBound const&);
    GenericsUnifierImpl* unifyCall(CallExpressionSym const&, FunctionHeadSym const&);
    GenericsUnifierImpl* unifyFunctions(FunctionHeadSym const&, FunctionHeadSym const&);
    GenericsUnifierImpl* unifyTypes(RootTyp const&,
                                    RootTyp const&,
                                    bool = false,
                                    bool = false,
                                    bool = false,
                                    bool = false);
    GenericsUnifierImpl* unifyTypeFormal(TypeFormal const&, RootTyp const&);
    GenericsUnifierImpl* unifyTypeFormal(RootTyp const&,
                                         TypeFormal const&,
                                         bool = false,
                                         bool = false);
};

// -----------------------------------------------------------------
GenericsUnifierImpl* GenericsUnifierImpl::checkAllConstraints()
{
    if (_isFailure) {
        return fail();
    }
    typedef unordered_map<TypeFormal const*, UnionFindConstraint*, TypeFormal::Hash,
                          TypeFormal::Equal>
      UnionFindConstraints;
    UnionFindConstraints ufcs;
    for (TypeMap::iterator i = _typeMap.begin(); i != _typeMap.end(); i++) {
        TypeFormal const* var = i->first;
        ufcs[var] = new UnionFindConstraint(var->getConstraint());
    }
    for (TypeMap::iterator i = _typeMap.begin(); i != _typeMap.end(); i++) {
        TypeFormal const* var = i->first;
        RootTyp const& type = get(*var);
        bool satisfiable;
        if (isVariable(type)) {
            TypeFormal const& var2 = type.as<TypeFormal>();
            if (ufcs.end() == ufcs.find(&var2)) {
                ufcs[&var2] = new UnionFindConstraint(var2.getConstraint());
            }
            satisfiable = ufcs[var]->match(*ufcs[&var2]);
        } else {
            satisfiable = ufcs[var]->match(type);
        }
        if (!satisfiable) {
            fail();
        }
    }
    for (UnionFindConstraints::iterator i = ufcs.begin(); i != ufcs.end(); i++) {
        delete ufcs[i->first];
    }
    return this;
}

GenericsUnifierImpl* GenericsUnifierImpl::unifyBounds(TypeBound const& x, TypeBound const& y)
{
    if (_isFailure) {
        return fail();
    }
    if (x.isVariable() && y.isVariable() && &x.toVar() == &y.toVar()) {
        return this;
    }
    if (!x.isVariable() && !y.isVariable() && x.toInt() == y.toInt()) {
        return this;
    }
    if (isVariable(x)) {
        return unifyBoundsFormal(x, y);
    }
    if (isVariable(y)) {
        return unifyBoundsFormal(y, x);
    }
    if (x.toInt() == y.toInt()) {
        return this;
    }
    return fail();
}

GenericsUnifierImpl* GenericsUnifierImpl::unifyBoundsFormal(TypeBound const& var,
                                                            TypeBound const& x)
{
    assert(isVariable(var));
    if (has(var.toVar())) {
        return unifyBounds(get(var.toVar()), x);
    }
    if (isVariable(x) && has(x.toVar())) {
        return unifyBounds(var, get(x.toVar()));
    }
    if (occursAnywhereIn(var.toVar(), x)) {
        return fail();
    }
    return add(&var.toVar(), &x);
}

GenericsUnifierImpl* GenericsUnifierImpl::unifyCall(CallExpressionSym const& x,
                                                    FunctionHeadSym const& y)
{
    if (_isFailure) {
        return fail();
    }
    if (x.name() != y.name()) {
        return fail();
    }
    CallExpressionSym::ActualTypes const& fx = x.actualTypes();
    FunctionHeadSym::Formals const& fy = y.formals();
    if (fx.size() != fy.size()) {
        return fail();
    }

    for (int i = 0, n = fx.size(); i < n; i++) {
        unifyTypes(*fx.at(i), fy.at(i)->type(), true, true, true, x.name() == "submit");
        if (_isFailure) {
            return fail();
        }
    }
    return this;
}

GenericsUnifierImpl* GenericsUnifierImpl::unifyFunctions(FunctionHeadSym const& x,
                                                         FunctionHeadSym const& y)
{
    if (false) {
        cerr << "GenericsUnifierImpl::unifyFunctions(" << x.sourceLocation() << ", "
             << y.sourceLocation() << ")" << endl;
    }
    if (_isFailure) {
        return fail();
    }
    if (&x == &y) {
        return this;
    }
    if (x.name() != y.name()) {
        return fail();
    }
    FunctionHeadSym::Formals const& fx = x.formals();
    FunctionHeadSym::Formals const& fy = y.formals();
    if (fx.size() != fy.size()) {
        return fail();
    }
    for (int i = 0, n = fx.size(); i < n; i++) {
        unifyTypes(fx.at(i)->type(), fy.at(i)->type());
    }
    return this;
}

GenericsUnifierImpl* GenericsUnifierImpl::unifyTypes(RootTyp const& x,
                                                     RootTyp const& y,
                                                     bool unifyOptionalAndNonOptional,
                                                     bool unifyOptionalsLeftToRight,
                                                     bool isCall,
                                                     bool isSubmit)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": unifying type " << x << " with type " << y,
           Debug::TraceTypeUnification);
    if (_isFailure) {
        return fail();
    }
    if (x == y) {
        return this;
    }

    if (x.getMetaType() == MetaType::UNKNOWN || y.getMetaType() == MetaType::UNKNOWN) {
        return this;
    }
    if (isVariable(x)) {
        return unifyTypeFormal(x.as<TypeFormal>(), y);
    }
    if (isVariable(y)) {
        return unifyTypeFormal(x, y.as<TypeFormal>(), isCall, isSubmit);
    }

    // any optional type unifies with the null literal
    if (x.getMetaType() == MetaType::NUL) {
        if (y.is<OptionalTyp>()) {
            return this;
        }
        return fail();
    }
    if (y.getMetaType() == MetaType::NUL) {
        if (x.is<OptionalTyp>()) {
            return this;
        }
        return fail();
    }

    if (x.getMetaType() != y.getMetaType()) {

        if (unifyOptionalAndNonOptional && unifyOptionalsLeftToRight && y.is<OptionalTyp>()) {
            return unifyTypes(x, y.as<OptionalTyp>().getUnderlyingType(),
                              unifyOptionalAndNonOptional, unifyOptionalsLeftToRight);
        }

        else if (unifyOptionalAndNonOptional && !unifyOptionalsLeftToRight && x.is<OptionalTyp>()) {
            return unifyTypes(x.as<OptionalTyp>().getUnderlyingType(), y,
                              unifyOptionalAndNonOptional, unifyOptionalsLeftToRight);
        }

        return fail();
    }

    switch (x.getMetaType()) {
        case MetaType::UNKNOWN:
        case MetaType::BOOLEAN:
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
        case MetaType::TIMESTAMP:
        case MetaType::RSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
            return this;
            // Metatypes are the same, but the type is not the same (schemas(xml) or
            // enumerators(enums) must vary)
        case MetaType::XML:
        case MetaType::ENUM:
            return fail(); // if they were equal we would have returned already
        case MetaType::BSTRING:
            return unifyBounds(x.as<BStringTyp>().getBound(), y.as<BStringTyp>().getBound());
        case MetaType::LIST:
            return unifyTypes(x.as<ListTyp>().getElementType(), y.as<ListTyp>().getElementType(),
                              unifyOptionalAndNonOptional, unifyOptionalsLeftToRight);
        case MetaType::BLIST: {
            BListTyp const& xx = x.as<BListTyp>();
            BListTyp const& yy = y.as<BListTyp>();
            unifyTypes(xx.getElementType(), yy.getElementType(), unifyOptionalAndNonOptional,
                       unifyOptionalsLeftToRight);
            return unifyBounds(xx.getBound(), yy.getBound());
        }
        case MetaType::SET:
            return unifyTypes(x.as<SetTyp>().getElementType(), y.as<SetTyp>().getElementType(),
                              unifyOptionalAndNonOptional, unifyOptionalsLeftToRight);
        case MetaType::BSET: {
            BSetTyp const& xx = x.as<BSetTyp>();
            BSetTyp const& yy = y.as<BSetTyp>();
            unifyTypes(xx.getElementType(), yy.getElementType(), unifyOptionalAndNonOptional,
                       unifyOptionalsLeftToRight);
            return unifyBounds(xx.getBound(), yy.getBound());
        }
        case MetaType::MAP: {
            MapTyp const& xx = x.as<MapTyp>();
            MapTyp const& yy = y.as<MapTyp>();
            unifyTypes(xx.getKeyType(), yy.getKeyType(), unifyOptionalAndNonOptional,
                       unifyOptionalsLeftToRight);
            return unifyTypes(xx.getValueType(), yy.getValueType(), unifyOptionalAndNonOptional,
                              unifyOptionalsLeftToRight);
        }
        case MetaType::BMAP: {
            BMapTyp const& xx = x.as<BMapTyp>();
            BMapTyp const& yy = y.as<BMapTyp>();
            unifyTypes(xx.getKeyType(), yy.getKeyType(), unifyOptionalAndNonOptional,
                       unifyOptionalsLeftToRight);
            unifyTypes(xx.getValueType(), yy.getValueType(), unifyOptionalAndNonOptional,
                       unifyOptionalsLeftToRight);
            return unifyBounds(xx.getBound(), yy.getBound());
        }
        case MetaType::TUPLE: {
            TupleTyp const& xx = x.as<TupleTyp>();
            TupleTyp const& yy = y.as<TupleTyp>();
            if (xx.getNumberOfAttributes() != yy.getNumberOfAttributes()) {
                return fail();
            }
            for (int i = 0, n = xx.getNumberOfAttributes(); i < n; i++) {
                if (xx.getAttributeName(i) != yy.getAttributeName(i)) {
                    return fail();
                }
            }
            for (int i = 0, n = xx.getNumberOfAttributes(); i < n; i++) {
                unifyTypes(xx.getAttributeType(i), yy.getAttributeType(i),
                           unifyOptionalAndNonOptional, unifyOptionalsLeftToRight);
            }
            return this;
        }
        case MetaType::OPTIONAL:
            return unifyTypes(x.as<OptionalTyp>().getUnderlyingType(),
                              y.as<OptionalTyp>().getUnderlyingType(), unifyOptionalAndNonOptional,
                              unifyOptionalsLeftToRight);
        default:
            break;
    }
    cerr << "unexpected type " << x << endl;
    assert(!"this statement should never be reached");
    return NULL;
}

GenericsUnifierImpl* GenericsUnifierImpl::unifyTypeFormal(TypeFormal const& var, RootTyp const& x)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": unifying var " << var << " with type " << x,
           Debug::TraceTypeUnification);
    if (has(var)) {
        return unifyTypes(x, get(var), true, true);
    }
    if (isVariable(x)) {
        TypeFormal const& xVar = x.as<TypeFormal>();
        if (has(xVar)) {
            return unifyTypes(get(xVar), var, true, true);
        }
    }
    if (occursAnywhereIn(var, x)) {
        return fail();
    }
    return add(&var, &x);
}

GenericsUnifierImpl* GenericsUnifierImpl::unifyTypeFormal(RootTyp const& x,
                                                          TypeFormal const& var,
                                                          bool isCall,
                                                          bool isSubmit)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": unifying var " << var << " with type " << x,
           Debug::TraceTypeUnification);

    // Unifying type formals is tricky, because in the context of a function call, we are trying to
    // resolve them before promotions happen.  We might have already set the type of the formal due
    // to a previous actual when that actual might get promoted later.
    // In the case of submit(T, T), we know we don't want to unify the second instance of T
    // (which is the port type) with a previous instance of T because the port type
    // can't actually change.  So, we treat submit(T, T) specially.
    if (has(var)) {

        if (isCall && isSubmit) {
            return unifyTypes(x, get(var), true, false);
        } else {
            return unifyTypes(x, get(var), true, true);
        }
    }

    if (isVariable(x)) {
        TypeFormal const& xVar = x.as<TypeFormal>();
        if (has(xVar)) {
            return unifyTypes(var, get(xVar), true, true);
        }
    }
    if (occursAnywhereIn(var, x)) {
        return fail();
    }
    return add(&var, &x);
}

// -----------------------------------------------------------------
GenericsUnifier& GenericsUnifier::unify(RootTyp const& x, RootTyp const& y)
{
    GenericsUnifierImpl* result = new GenericsUnifierImpl();
    result = result->unifyTypes(x, y);
    assert(NULL != result);
    result = result->checkAllConstraints();
    assert(NULL != result);
    return *result;
}

GenericsUnifier& GenericsUnifier::unify(CallExpressionSym const& x, FunctionHeadSym const& y)
{
    GenericsUnifierImpl* result = new GenericsUnifierImpl();
    result = result->unifyCall(x, y);
    assert(NULL != result);
    result = result->checkAllConstraints();
    assert(NULL != result);
    return *result;
}

GenericsUnifier& GenericsUnifier::unify(FunctionHeadSym const& x, FunctionHeadSym const& y)
{
    GenericsUnifierImpl* result = new GenericsUnifierImpl();
    result = result->unifyFunctions(x, y);
    assert(NULL != result);
    result = result->checkAllConstraints();
    assert(NULL != result);
    return *result;
}

RootTyp const& GenericsUnifier::substitute(RootTyp const& x) const
{
    TypeFactory& tf = TypeFactory::instance();
    switch (x.getMetaType()) {
        case MetaType::UNKNOWN:
        case MetaType::VOID:
        case MetaType::BOOLEAN:
        case MetaType::ENUM:
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
        case MetaType::TIMESTAMP:
        case MetaType::RSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
        case MetaType::XML:
            return x;
        case MetaType::OPTIONAL:
            return tf.optionalT(substitute(x.as<OptionalTyp>().getUnderlyingType()));
        case MetaType::TYPEFORMAL: {
            TypeFormal const& xx = x.as<TypeFormal>();
            return has(xx) ? get(xx) : xx;
        }
        case MetaType::BSTRING: {
            BStringTyp const& xx = x.as<BStringTyp>();
            TypeBound const& b = xx.getBound();
            if (!b.isVariable() || !has(b.toVar())) {
                return xx;
            }
            return tf.rstringT(get(b.toVar()));
        }
        case MetaType::LIST:
            return tf.listT(substitute(x.as<ListTyp>().getElementType()));
        case MetaType::BLIST: {
            BListTyp const& xx = x.as<BListTyp>();
            RootTyp const& e = substitute(xx.getElementType());
            TypeBound const& b = xx.getBound();
            if (!b.isVariable() || !has(b.toVar())) {
                return tf.listT(e, b);
            }
            return tf.listT(e, get(b.toVar()));
        }
        case MetaType::SET:
            return tf.setT(substitute(x.as<SetTyp>().getElementType()));
        case MetaType::BSET: {
            BSetTyp const& xx = x.as<BSetTyp>();
            RootTyp const& e = substitute(xx.getElementType());
            TypeBound const& b = xx.getBound();
            if (!b.isVariable() || !has(b.toVar())) {
                return tf.setT(e, b);
            }
            return tf.setT(e, get(b.toVar()));
        }
        case MetaType::MAP: {
            MapTyp const& xx = x.as<MapTyp>();
            return tf.mapT(substitute(xx.getKeyType()), substitute(xx.getValueType()));
        }
        case MetaType::BMAP: {
            BMapTyp const& xx = x.as<BMapTyp>();
            RootTyp const& k = substitute(xx.getKeyType());
            RootTyp const& v = substitute(xx.getValueType());
            TypeBound const& b = xx.getBound();
            if (!b.isVariable() || !has(b.toVar())) {
                return tf.mapT(k, v, b);
            }
            return tf.mapT(k, v, get(b.toVar()));
        }
        case MetaType::TUPLE: {
            TupleTyp const& xx = x.as<TupleTyp>();
            vector<string> names;
            vector<RootTyp const*> types;
            for (int i = 0, n = xx.getNumberOfAttributes(); i < n; i++) {
                names.push_back(xx.getAttributeName(i));
                types.push_back(&substitute(xx.getAttributeType(i)));
            }
            return tf.tupleT(names, types);
        }
        default:
            break;
    }
    cerr << "unexpected type " << x << endl;
    assert(!"this statement should never be reached");
    return tf.unknownT();
}

// -----------------------------------------------------------------
bool SPL::unifies(RootTyp const& t0, RootTyp const& t1)
{
    GenericsUnifier& gu = GenericsUnifier::unify(t0, t1);
    bool result = !gu.isFailure();
    delete &gu;
    return result;
}
