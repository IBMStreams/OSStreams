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

#include <SAM/SubmissionExpression.h>
#include <SPL/Runtime/Function/SPLCast.h>
#include <SPL/Runtime/Function/SPLFunctions.h>

#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#define BOOST_SPIRIT_THREADSAFE
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/spirit/include/classic.hpp>

#include <stack>

SAM_NAMESPACE_USE

SAM_NAMESPACE_BEGIN

namespace LogicalToPhysical {
namespace SubmissionExpressionEval {

std::ostream& operator<<(std::ostream& ostr, OrOp v)
{
    ostr << "||";
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, AndOp v)
{
    ostr << "&&";
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, RelOp v)
{
    switch (v) {
        case LessThan:
            ostr << "<";
            break;
        case LessEqual:
            ostr << "<=";
            break;
        case GreaterThan:
            ostr << ">";
            break;
        case GreaterEqual:
            ostr << ">=";
            break;
        case Equal:
            ostr << "==";
            break;
        case NotEqual:
            ostr << "!=";
            break;
        case In:
            ostr << "in";
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid RelOp");
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, AddOp v)
{
    switch (v) {
        case Add:
            ostr << "+";
            break;
        case Sub:
            ostr << "-";
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid AddOp");
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, MulOp v)
{
    switch (v) {
        case Mul:
            ostr << "*";
            break;
        case Div:
            ostr << "/";
            break;
        case Mod:
            ostr << "%";
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid MulOp");
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, Type v)
{
    switch (v) {
        case Int32:
            ostr << "int32";
            break;
        case Int64:
            ostr << "int64";
            break;
        case Float64:
            ostr << "float64";
            break;
        case RString:
            ostr << "rstring";
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid Type in operator<<");
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, MakeIndent m)
{
    for (int i = 0; i < m.level; ++i) {
        ostr << "  ";
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Printable& v)
{
    return v.print(ostr);
}

Literal::Literal(const SPL::rstring& value)
  : _type(RString)
  , _rstring(new SPL::rstring(value))
{}

Literal::~Literal()
{
    if (_type == RString)
        delete _rstring;
}

std::ostream& Literal::print(std::ostream& s) const
{
    switch (_type) {
        case Int64:
            s << _int64;
            break;
        case Float64:
            SPL::serializeWithPrecision(s, _float64);
            break;
        case RString:
            s << *_rstring;
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid Type in Literal::print");
    }
    return s;
}

std::ostream& Literal::printTree(std::ostream& s, int level) const
{
    s << MakeIndent(level) << "Literal: " << *this << std::endl;
    return s;
}

inline void Literal::typeCheck(Type t, const char* msg) const
{
    if (_type != t) {
        THROW(LogicalToPhysical, "Internal error: submission kind invalid literal " << msg);
    }
}

SPL::int32 Literal::int32Value() const
{
    typeCheck(Int32, "int32");
    return _int32;
}

SPL::int64 Literal::int64Value() const
{
    typeCheck(Int64, "int64");
    return _int64;
}

SPL::float64 Literal::float64Value() const
{
    typeCheck(Float64, "float64");
    return _float64;
}

const SPL::rstring& Literal::rstringValue() const
{
    typeCheck(RString, "rstring");
    return *_rstring;
}

Intrinsic::Intrinsic(std::string f)
  : _function(f)
{
    // Yes, we could just set it to what it needs to be, but I'm indicating what
    // we will do in the future if we add more intrinsics which are different
    // types. Also, we're lying: it is technically int32, but the front-end
    // lets users fudge on this one.
    if (_function == "getChannel()" || _function == "getMaxChannels()") {
        _type = Int64;
    } else {
        THROW(LogicalToPhysical, "Internal error: unrecognized intrinsic function, " << f);
    }
}

std::ostream& Intrinsic::print(std::ostream& s) const
{
    s << _function;
    return s;
}

std::ostream& Intrinsic::printTree(std::ostream& s, int level) const
{
    s << MakeIndent(level) << "Intrinsic: " << *this << std::endl;
    return s;
}

inline void Value::literalCheck(const char* msg) const
{
    if (_literal.get() == NULL) {
        THROW(LogicalToPhysical,
              "Internal error: " << msg << "Value() called on Value with no Literal");
    }
}

SPL::int32 Value::int32Value() const
{
    literalCheck("int32");
    return _literal->int32Value();
}

SPL::int64 Value::int64Value() const
{
    literalCheck("int64");
    return _literal->int64Value();
}

SPL::float64 Value::float64Value() const
{
    literalCheck("float64");
    return _literal->float64Value();
}

const SPL::rstring& Value::rstringValue() const
{
    literalCheck("rstring");
    return _literal->rstringValue();
}

static SPL::int64 castInt64(const SPL::ConstValueHandle& val)
{
    SPL::int64 num;
    switch (val.getMetaType()) {
        case SPL::Meta::Type::FLOAT32:
            num = SPL::spl_cast<SPL::int64, SPL::float32>::cast(val);
            break;
        case SPL::Meta::Type::FLOAT64:
            num = SPL::spl_cast<SPL::int64, SPL::float64>::cast(val);
            break;
        case SPL::Meta::Type::BOOLEAN:
            num = SPL::spl_cast<SPL::int64, SPL::boolean>::cast(val);
            break;
        case SPL::Meta::Type::UINT8:
            num = SPL::spl_cast<SPL::int64, SPL::uint8>::cast(val);
            break;
        case SPL::Meta::Type::UINT16:
            num = SPL::spl_cast<SPL::int64, SPL::uint16>::cast(val);
            break;
        case SPL::Meta::Type::UINT32:
            num = SPL::spl_cast<SPL::int64, SPL::uint32>::cast(val);
            break;
        case SPL::Meta::Type::UINT64:
            num = SPL::spl_cast<SPL::int64, SPL::uint64>::cast(val);
            break;
        case SPL::Meta::Type::INT8:
            num = SPL::spl_cast<SPL::int64, SPL::int8>::cast(val);
            break;
        case SPL::Meta::Type::INT16:
            num = SPL::spl_cast<SPL::int64, SPL::int16>::cast(val);
            break;
        case SPL::Meta::Type::INT32:
            num = SPL::spl_cast<SPL::int64, SPL::int32>::cast(val);
            break;
        case SPL::Meta::Type::INT64:
            num = val;
            break;
        case SPL::Meta::Type::RSTRING:
            num = SPL::spl_cast<SPL::int64, SPL::rstring>::cast(val);
            break;
        case SPL::Meta::Type::USTRING:
            num = SPL::spl_cast<SPL::int64, SPL::ustring>::cast(val);
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: unrecognized Meta::Type in castInt64");
    }
    return num;
}

static SPL::float64 castFloat64(const SPL::ConstValueHandle& val)
{
    SPL::float64 num;
    switch (val.getMetaType()) {
        case SPL::Meta::Type::FLOAT32:
            num = SPL::spl_cast<SPL::float64, SPL::float32>::cast(val);
            break;
        case SPL::Meta::Type::FLOAT64:
            num = val;
            break;
        case SPL::Meta::Type::BOOLEAN:
            num = SPL::spl_cast<SPL::float64, SPL::boolean>::cast(val);
            break;
        case SPL::Meta::Type::UINT8:
            num = SPL::spl_cast<SPL::float64, SPL::uint8>::cast(val);
            break;
        case SPL::Meta::Type::UINT16:
            num = SPL::spl_cast<SPL::float64, SPL::uint16>::cast(val);
            break;
        case SPL::Meta::Type::UINT32:
            num = SPL::spl_cast<SPL::float64, SPL::uint32>::cast(val);
            break;
        case SPL::Meta::Type::UINT64:
            num = SPL::spl_cast<SPL::float64, SPL::uint64>::cast(val);
            break;
        case SPL::Meta::Type::INT8:
            num = SPL::spl_cast<SPL::float64, SPL::int8>::cast(val);
            break;
        case SPL::Meta::Type::INT16:
            num = SPL::spl_cast<SPL::float64, SPL::int16>::cast(val);
            break;
        case SPL::Meta::Type::INT32:
            num = SPL::spl_cast<SPL::float64, SPL::int32>::cast(val);
            break;
        case SPL::Meta::Type::INT64:
            num = SPL::spl_cast<SPL::float64, SPL::int64>::cast(val);
            break;
        case SPL::Meta::Type::RSTRING:
            num = SPL::spl_cast<SPL::float64, SPL::rstring>::cast(val);
            break;
        case SPL::Meta::Type::USTRING:
            num = SPL::spl_cast<SPL::float64, SPL::ustring>::cast(val);
            break;
        case SPL::Meta::Type::TIMESTAMP:
            num = SPL::spl_cast<SPL::float64, SPL::timestamp>::cast(val);
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: unrecognized Meta::Type in castFloat64");
    }
    return num;
}

bool Value::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_literal.get()) {
        return true;
    } else if (!_intrinsic.get()) {
        THROW(LogicalToPhysical, "Internal error: evaluate() called on empty Value");
    } else if (_intrinsic->function() != sym) {
        return false;
    }

    Literal* literal = NULL;
    switch (_intrinsic->type()) {
        case Int32:
            literal = new Literal(castInt64(val));
            break; // implicit promotion from int32 to int64
        case Int64:
            literal = new Literal(castInt64(val));
            break;
        case Float64:
            literal = new Literal(castFloat64(val));
            break;
        case RString:
            literal = new Literal(val.toString());
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid Type in Value::evaluate");
    }

    _intrinsic.reset();
    _literal.reset(literal);

    return true;
}

Type Value::type() const
{
    if (_literal.get()) {
        return _literal->type();
    } else if (_intrinsic.get()) {
        return _intrinsic->type();
    } else {
        THROW(LogicalToPhysical, "Internal error: type() called on on empty Value");
    }
}

std::ostream& Value::print(std::ostream& s) const
{
    if (_literal.get()) {
        s << *_literal;
    } else if (_intrinsic.get()) {
        s << *_intrinsic;
    }
    return s;
}

std::ostream& Value::printTree(std::ostream& s, int level) const
{
    s << MakeIndent(level) << "Value: " << *this << std::endl;
    if (_literal.get()) {
        _literal->printTree(s, level + 1);
    } else if (_intrinsic.get()) {
        _intrinsic->printTree(s, level + 1);
    }
    return s;
}

std::ostream& Symbol::print(std::ostream& s) const
{
    s << _symbol;
    if (_hasSubscript) {
        s << '[' << _subscript << ']';
    }
    return s;
}

std::ostream& Symbol::printTree(std::ostream& s, int level) const
{
    s << MakeIndent(level) << "Symbol: " << *this << std::endl;
    return s;
}

Type PrimaryExpression::type() const
{
    if (_value.get()) {
        return _value->type();
    } else if (_or.get()) {
        return _or->type();
    } else if (_symbol.get()) {
        THROW(LogicalToPhysical, "Internal error: type() called on untyped symbol");
    } else {
        THROW(LogicalToPhysical, "Internal error: type() called on empty PrimaryExpression");
    }
}

void PrimaryExpression::emptyCheck(const char* k) const
{
    if (_value.get() == NULL && _or.get() == NULL) {
        THROW(LogicalToPhysical,
              "Internal error: " << k << "Value() called on unreducible PrimaryExpression");
    }
}

SPL::int32 PrimaryExpression::int32Value() const
{
    emptyCheck("int32");
    if (_value.get()) {
        return _value->int32Value();
    } else {
        return _or->int32Value();
    }
}

SPL::int64 PrimaryExpression::int64Value() const
{
    emptyCheck("int64");
    if (_value.get()) {
        return _value->int64Value();
    } else {
        return _or->int64Value();
    }
}

SPL::float64 PrimaryExpression::float64Value() const
{
    emptyCheck("float64");
    if (_value.get()) {
        return _value->float64Value();
    } else {
        return _or->float64Value();
    }
}

const SPL::rstring& PrimaryExpression::rstringValue() const
{
    emptyCheck("rstring");
    if (_value.get()) {
        return _value->rstringValue();
    } else {
        return _or->rstringValue();
    }
}

bool PrimaryExpression::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_symbol.get()) {
        return false;
    } else if (_value.get()) {
        return _value->evaluate(sym, val);
    } else if (_or.get()) {
        if (_or->evaluate(sym, val)) {

            Value* v = NULL;
            switch (_or->type()) {
                case Int64:
                    v = new Value(new Literal(_or->int64Value()));
                    break;
                case Float64:
                    v = new Value(new Literal(_or->float64Value()));
                    break;
                case RString:
                    v = new Value(new Literal(_or->rstringValue()));
                    break;
                default:
                    THROW(LogicalToPhysical,
                          "Internal error: invalid Type in PrimaryExpression::evaluate");
            }

            _value.reset(v);
            _or.reset();

            return true;
        } else {
            return false;
        }
    } else {
        THROW(LogicalToPhysical,
              "Internal error: evaluate() called on unevaluable PrimaryExpression");
    }
}

std::ostream& PrimaryExpression::print(std::ostream& s) const
{
    if (_symbol.get()) {
        s << *_symbol;
    } else if (_value.get()) {
        s << *_value;
    } else if (_or.get()) {
        s << "(" << *_or << ")";
    }
    return s;
}

std::ostream& PrimaryExpression::printTree(std::ostream& s, int level) const
{
    s << MakeIndent(level) << "PrimaryExpression: " << *this << std::endl;
    if (_symbol.get()) {
        _symbol->printTree(s, level + 1);
    }
    if (_value.get()) {
        _value->printTree(s, level + 1);
    } else if (_or.get()) {
        _or->printTree(s, level + 1);
    }
    return s;
}

Type CastExpression::type() const
{
    if (_primary.get()) {
        return _primary->type();
    } else if (_cast.get()) {
        return _type;
    } else {
        THROW(LogicalToPhysical, "Internal error: type() called on empty CastExpression");
    }
}

void CastExpression::evaluatedCheck(const char* k) const
{
    if (_cast.get()) {
        THROW(LogicalToPhysical,
              "Internal error: " << k << "Value() called on unevaluated CastExpression");
    } else if (_primary.get() == NULL) {
        THROW(LogicalToPhysical,
              "Internal error: " << k << "Value() called on empty CastExpression");
    }
}

SPL::int32 CastExpression::int32Value() const
{
    evaluatedCheck("int32");
    return _primary->int32Value();
}

SPL::int64 CastExpression::int64Value() const
{
    evaluatedCheck("int64");
    return _primary->int64Value();
}

SPL::float64 CastExpression::float64Value() const
{
    evaluatedCheck("float64");
    return _primary->float64Value();
}

const SPL::rstring& CastExpression::rstringValue() const
{
    evaluatedCheck("rstring");
    return _primary->rstringValue();
}

template<class T>
static Literal* toLiteral(const T& value, Type type)
{
    switch (type) {
        case Int64:
            return new Literal(SPL::spl_cast<SPL::int64, T>::cast(value));
            break;
        case Float64:
            return new Literal(SPL::spl_cast<SPL::float64, T>::cast(value));
            break;
        case RString:
            return new Literal(SPL::spl_cast<SPL::rstring, T>::cast(value));
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid Type in toLiteral");
    }
}

bool CastExpression::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_primary.get()) {
        return _primary->evaluate(sym, val);
    } else if (_cast.get()) {
        if (_cast->evaluate(sym, val)) {

            Literal* lit = NULL;
            switch (_cast->type()) {
                case Int32:
                    lit = toLiteral(_cast->int32Value(), _type);
                    break;
                case Int64:
                    lit = toLiteral(_cast->int64Value(), _type);
                    break;
                case Float64:
                    lit = toLiteral(_cast->float64Value(), _type);
                    break;
                case RString:
                    lit = toLiteral(_cast->rstringValue(), _type);
                    break;
                default:
                    THROW(LogicalToPhysical,
                          "Internal error: invalid Type in CastExpression::evaluate expression");
            }

            _primary.reset(new PrimaryExpression(new Value(lit)));
            _cast.reset();

            return true;
        } else {
            return false;
        }
    } else {
        THROW(LogicalToPhysical, "Internal error: evaluate() called on on empty CastExpression");
    }
}

std::ostream& CastExpression::print(std::ostream& s) const
{
    if (_primary.get()) {
        s << *_primary;
    } else if (_cast.get()) {
        s << "(" << _type << ")" << *_cast;
    }
    return s;
}

std::ostream& CastExpression::printTree(std::ostream& s, int level) const
{
    s << MakeIndent(level) << "CastExpression: " << *this << std::endl;
    if (_primary.get()) {
        _primary->printTree(s, level + 1);
    } else if (_cast.get()) {
        _cast->printTree(s, level + 1);
    }
    return s;
}

SPL::int64 evalInt64MulOp(SPL::int64 lhs, MulOp op, SPL::int64 rhs)
{
    if (rhs == 0 && (op == Div || op == Mod)) {
        THROW(LogicalToPhysical, "Internal error: submission expression divide by zero");
    }

    SPL::int64 res;
    switch (op) {
        case Mul:
            res = lhs * rhs;
            break;
        case Div:
            res = lhs / rhs;
            break;
        case Mod:
            res = lhs % rhs;
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: illegal MulOp in evalInt64MulOp");
    }
    return res;
}

SPL::float64 evalFloat64MulOp(SPL::float64 lhs, MulOp op, SPL::float64 rhs)
{
    SPL::float64 res;
    switch (op) {
        case Mul:
            res = lhs * rhs;
            break;
        case Div:
            res = lhs / rhs;
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: illegal MulOp in evalFloat64MulOp");
    }
    return res;
}

bool MulExpression::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_left.get() == NULL) {
        THROW(LogicalToPhysical, "Internal error: evaluate() called on empty MulExpression");
    }

    bool leftReady = _left->evaluate(sym, val);
    bool rightReady = false;
    if (_right.get()) {
        rightReady = _right->evaluate(sym, val);
    }

    if (leftReady && _right.get() == NULL) {
        return true;
    } else if (leftReady && rightReady) {
        if (_left->type() != _right->type()) {
            THROW(LogicalToPhysical, "Internal error: MulExpression type mismatch in evaluate()");
        }

        CastExpression* ce = NULL;
        switch (_left->type()) {
            case Int64:
                ce = new CastExpression(new PrimaryExpression(new Value(
                  new Literal(evalInt64MulOp(_left->int64Value(), _op, _right->int64Value())))));
                break;
            case Float64:
                ce = new CastExpression(new PrimaryExpression(new Value(new Literal(
                  evalFloat64MulOp(_left->float64Value(), _op, _right->float64Value())))));
                break;
            default:
                THROW(LogicalToPhysical, "Internal error: invalid Type in MulExpression::evaluate");
        }

        _left.reset(ce);
        _right.reset();
        return true;

    } else {
        return false;
    }
}

template<class T>
T evalAddOp(T lhs, AddOp op, T rhs)
{
    T res;
    switch (op) {
        case Add:
            res = lhs + rhs;
            break;
        case Sub:
            res = lhs - rhs;
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: illegal AddOp in evalMathOp");
    }
    return res;
}

template<>
SPL::rstring evalAddOp(SPL::rstring lhs, AddOp op, SPL::rstring rhs)
{
    if (op != Add) {
        THROW(LogicalToPhysical, "Internal error: illegal operator used on rstrings");
    }
    return lhs + rhs;
}

bool AddExpression::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_left.get() == NULL) {
        THROW(LogicalToPhysical, "Internal error: evaluate() called on unevaluated AddExpression");
    }

    bool leftReady = _left->evaluate(sym, val);
    bool rightReady = false;
    if (_right.get()) {
        rightReady = _right->evaluate(sym, val);
    }

    if (leftReady && _right.get() == NULL) {
        return true;
    } else if (leftReady && rightReady) {
        if (_left->type() != _right->type()) {
            THROW(LogicalToPhysical, "Internal error: AddExpression type mismatch in evaluate()");
        }

        MulExpression* me = NULL;
        switch (_left->type()) {
            case Int64:
                me = new MulExpression(new CastExpression(new PrimaryExpression(new Value(
                  new Literal(evalAddOp(_left->int64Value(), _op, _right->int64Value()))))));
                break;
            case Float64:
                me = new MulExpression(new CastExpression(new PrimaryExpression(new Value(
                  new Literal(evalAddOp(_left->float64Value(), _op, _right->float64Value()))))));
                break;
            case RString:
                me = new MulExpression(new CastExpression(new PrimaryExpression(new Value(
                  new Literal(evalAddOp(_left->rstringValue(), _op, _right->rstringValue()))))));
                break;
            default:
                THROW(LogicalToPhysical, "Internal error: invalid Type in AddExpression::evaluate");
        }

        _left.reset(me);
        _right.reset();
        return true;

    } else {
        return false;
    }
}

bool Predicate::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_left.get() == NULL) {
        THROW(LogicalToPhysical, "Internal error: evaluate() called on empty Predicate");
    }

    bool ready = _left->evaluate(sym, val);
    if (_right.get()) {
        ready = _right->evaluate(sym, val) && ready;
    }
    return ready;
}

bool AndExpression::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_left.get() == NULL) {
        THROW(LogicalToPhysical, "Internal error: evaluate() called on empty AndExpression");
    }

    bool ready = _left->evaluate(sym, val);
    if (_right.get()) {
        ready = _right->evaluate(sym, val) && ready;
    }
    return ready;
}

std::string OrExpression::toString() const
{
    std::ostringstream s;
    s.imbue(std::locale::classic());
    print(s);
    return s.str();
}

bool OrExpression::evaluate(const std::string& sym, const SPL::ConstValueHandle& val)
{
    if (_left.get() == NULL) {
        return false;
    }

    bool ready = _left->evaluate(sym, val);
    if (_right.get()) {
        ready = _right->evaluate(sym, val) && ready;
    }
    return ready;
}

template<class T>
void clearStack(std::stack<T*>& s)
{
    while (!s.empty()) {
        delete s.top();
        s.pop();
    }
}

using namespace boost::spirit;
struct ParserState
{
    ParserState()
      : hasPredicate(false)
      , hasPrimaryExpression(false)
      , hasOrRHS(false)
      , hasAndRHS(false)
      , hasPredicateRHS(false)
      , hasAddRHS(false)
      , hasMulRHS(false)
      , hasValue(false)
      , hasIntrinsic(false)
      , hasSubscript(false)
      , hasModulo(false)
      , hasSymbol(false)
    {}

    ~ParserState()
    {
        clearStack(orExpressions);
        clearStack(andExpressions);
        clearStack(predicates);
        clearStack(addExpressions);
        clearStack(mulExpressions);
        clearStack(castExpressions);
    }

    void do_integer(int64_t v) { currInt = v; }

    void do_int(const char* f) { literal.reset(new Literal(currInt)); }

    void do_real(double v) { literal.reset(new Literal(v)); }

    void do_symbolText(const char* f, const char* l) { symbolText.assign(f, l - f); }

    void do_string(const char* f, const char* l)
    {
        literal.reset(new Literal(SPL::interpretRStringLiteral(std::string(f, l))));
    }

    void do_intrinsic(const char* f, const char* l)
    {
        hasIntrinsic = true;
        intrinsicFunction.assign(f, l - f);
    }

    void do_sub(const char* f)
    {
        hasSubscript = true;
        symbolSubscript = currInt;
    }

    void do_modAddExpression(const char* f, const char* l)
    {
        hasModulo = true;
        if (addExpressions.empty()) {
            THROW(LogicalToPhysical, "Internal error: addExpressions empty in do_modAddExpression");
        }
        modAddExpression.reset(addExpressions.top());
        addExpressions.pop();
    }

    void do_relOp(const char* f, const char* l)
    {
        if (*f == '=') {
            relOp = Equal;
        } else if (*f == '!') {
            relOp = NotEqual;
        } else if (*f == '>') {
            if (f[1] == '=') {
                relOp = GreaterEqual;
            } else {
                relOp = GreaterThan;
            }
        } else if (*f == 'i') {
            relOp = In;
        } else {
            if (*f != '<') {
                THROW(LogicalToPhysical, "Internal error: expected '<' in RelOp");
            }
            if (f[1] == '=') {
                relOp = LessEqual;
            } else {
                relOp = LessThan;
            }
        }
    }

    void do_addOp(const char* f, const char* l)
    {
        switch (*f) {
            case '+':
                addOp = Add;
                break;
            case '-':
                addOp = Sub;
                break;
            default:
                THROW(LogicalToPhysical,
                      "Internal error: invalid AddOp in parsing SubmissionExpression");
        }
    }

    void do_mulOp(const char* f, const char* l)
    {
        switch (*f) {
            case '*':
                mulOp = Mul;
                break;
            case '/':
                mulOp = Div;
                break;
            case '%':
                mulOp = Mod;
                break;
            default:
                THROW(LogicalToPhysical,
                      "Internal error: invalid MulOp in parsing SubmissionExpression");
        }
    }

    void do_type(const char* f, const char* l)
    {
        std::string str(f, l);
        if (str == "int64") {
            type = Int64;
        } else if (str == "float64") {
            type = Float64;
        } else if (str == "rstring") {
            type = RString;
        } else {
            THROW(LogicalToPhysical,
                  "Internal error: unrecognized type when parsing SubmissionExpression");
        }
    }

    void do_value(const char* f, const char* l)
    {
        hasValue = true;
        if (hasIntrinsic) {
            value.reset(new Value(new Intrinsic(intrinsicFunction)));
        } else {
            if (literal.get() == NULL) {
                THROW(LogicalToPhysical, "Internal error: literal is NULL");
            }
            value.reset(new Value(literal.release()));
        }
        hasIntrinsic = false;
    }

    void do_symbol(const char* f, const char* l)
    {
        hasSymbol = true;
        if (hasSubscript) {
            symbol.reset(new Symbol(symbolText, symbolSubscript));
        } else {
            symbol.reset(new Symbol(symbolText));
        }
        hasSubscript = false;
    }

    void do_primaryExpression(const char* f, const char* l)
    {
        hasPrimaryExpression = true;
        if (hasSymbol) {
            if (symbol.get() == NULL) {
                THROW(LogicalToPhysical, "Internal error: symbol is NULL");
            }
            primaryExpression.reset(new PrimaryExpression(symbol.release()));
        } else if (hasValue) {
            if (value.get() == NULL) {
                THROW(LogicalToPhysical, "Internal error: value is NULL");
            }
            primaryExpression.reset(new PrimaryExpression(value.release()));
        } else {
            if (orExpressions.empty()) {
                THROW(LogicalToPhysical, "Internal error: orExpressions empty");
            }
            primaryExpression.reset(new PrimaryExpression(orExpressions.top()));
            orExpressions.pop();
        }
        hasSymbol = false;
        hasValue = false;
    }

    void do_castExpression(const char* f, const char* l)
    {
        if (hasPrimaryExpression) {
            if (primaryExpression.get() == NULL) {
                THROW(LogicalToPhysical, "Internal error: primaryExpression is NULL");
            }
            castExpressions.push(new CastExpression(primaryExpression.release()));
        } else {
            if (castExpressions.empty()) {
                THROW(LogicalToPhysical,
                      "Internal error: castExpressions empty in do_castExpression");
            }
            CastExpression* ce = new CastExpression(type, castExpressions.top());
            castExpressions.pop();
            castExpressions.push(ce);
        }
        hasPrimaryExpression = false;
    }

    void do_mulRHS(const char*, const char*) { hasMulRHS = true; }

    void do_mulExpression(const char* f, const char* l)
    {
        if (castExpressions.empty()) {
            THROW(LogicalToPhysical, "Internal error: castExpressions empty in do_mulExpression");
        }
        CastExpression* ce = castExpressions.top();
        castExpressions.pop();

        if (hasMulRHS) {
            if (castExpressions.empty()) {
                THROW(LogicalToPhysical, "Internal error: castExpressions empty but saw MulOp");
            }
            mulExpressions.push(new MulExpression(castExpressions.top(), mulOp, ce));
            castExpressions.pop();
        } else {
            mulExpressions.push(new MulExpression(ce));
        }
        hasMulRHS = false;
    }

    void do_addRHS(const char*, const char*) { hasAddRHS = true; }

    void do_addExpression(const char* f, const char* l)
    {
        if (mulExpressions.empty()) {
            THROW(LogicalToPhysical, "Internal error: mulExpressions empty");
        }
        MulExpression* me = mulExpressions.top();
        mulExpressions.pop();

        if (hasAddRHS) {
            if (mulExpressions.empty()) {
                THROW(LogicalToPhysical, "Internal error: mulExpressions empty but saw AddOp");
            }
            addExpressions.push(new AddExpression(mulExpressions.top(), addOp, me));
            mulExpressions.pop();
        } else {
            addExpressions.push(new AddExpression(me));
        }
        hasAddRHS = false;
    }

    void do_predicateRHS(const char*, const char*) { hasPredicateRHS = true; }

    void do_predicate(const char* f, const char* l)
    {
        hasPredicate = true;

        if (addExpressions.empty()) {
            THROW(LogicalToPhysical, "Internal error: addExpressions empty in Relation");
        }
        AddExpression* ae = addExpressions.top();
        addExpressions.pop();

        if (hasPredicateRHS) {
            if (addExpressions.empty()) {
                THROW(LogicalToPhysical, "Internal error: addExpressions empty but saw RelOp");
            }
            predicates.push(new Predicate(addExpressions.top(), relOp, ae));
            addExpressions.pop();
        } else {
            predicates.push(new Predicate(ae));
        }
        hasPredicateRHS = false;
    }

    void do_andRHS(const char*, const char*) { hasAndRHS = true; }

    void do_andExpression(const char* f, const char* l)
    {
        if (predicates.empty()) {
            THROW(LogicalToPhysical, "Internal error: predicates is empty");
        }
        Predicate* pred = predicates.top();
        predicates.pop();

        if (hasAndRHS) {
            if (predicates.empty()) {
                THROW(LogicalToPhysical, "Internal error: predicates is empty");
            }
            andExpressions.push(new AndExpression(predicates.top(), pred));
            predicates.pop();
        } else {
            andExpressions.push(new AndExpression(pred));
        }
        hasAndRHS = false;
    }

    void do_orRHS(const char*, const char*) { hasOrRHS = true; }

    void do_orExpression(const char* f, const char* l)
    {
        if (andExpressions.empty()) {
            THROW(LogicalToPhysical, "Internal error: andExpressions is empty");
        }
        AndExpression* ae = andExpressions.top();
        andExpressions.pop();

        if (hasOrRHS) {
            if (andExpressions.empty()) {
                THROW(LogicalToPhysical, "Internal error: andExpressions is empty");
            }
            orExpressions.push(new OrExpression(andExpressions.top(), ae));
            andExpressions.pop();
        } else {
            orExpressions.push(new OrExpression(ae));
        }
        hasOrRHS = false;
    }

    std::stack<OrExpression*> orExpressions;
    std::stack<AndExpression*> andExpressions;
    std::stack<Predicate*> predicates;
    std::stack<AddExpression*> addExpressions;
    std::stack<MulExpression*> mulExpressions;
    std::stack<CastExpression*> castExpressions;

    std::auto_ptr<AddExpression> modAddExpression;
    std::auto_ptr<PrimaryExpression> primaryExpression;
    std::auto_ptr<Symbol> symbol;
    std::auto_ptr<Value> value;
    std::auto_ptr<Literal> literal;

    RelOp relOp;
    AddOp addOp;
    MulOp mulOp;
    Type type;

    std::string symbolText;
    int64_t symbolSubscript;

    int64_t currInt;
    std::string intrinsicFunction;

    bool hasPredicate;
    bool hasPrimaryExpression;
    bool hasOrRHS;
    bool hasAndRHS;
    bool hasPredicateRHS;
    bool hasAddRHS;
    bool hasMulRHS;
    bool hasValue;
    bool hasIntrinsic;
    bool hasSubscript;
    bool hasModulo;
    bool hasSymbol;
};

boost::spirit::int_parser<long long> const longlong_p = boost::spirit::int_parser<long long>();

#define BIND1(x) boost::bind(&ParserState::x, &const_cast<OrParser&>(self).state, _1)
#define BIND2(x) boost::bind(&ParserState::x, &const_cast<OrParser&>(self).state, _1, _2)

struct OrParser : public grammar<OrParser>
{
    template<typename ScannerT>
    struct definition
    {
        definition(OrParser const& self)
        {
            orExpression =
              (andExpression >> !("||" >> andExpression[BIND2(do_orRHS)]))[BIND2(do_orExpression)];

            andExpression =
              (predicate >> !("&&" >> predicate[BIND2(do_andRHS)]))[BIND2(do_andExpression)];

            predicate = (addExpression >>
                         !(relOp >> addExpression[BIND2(do_predicateRHS)]))[BIND2(do_predicate)];

            addExpression = (mulExpression >>
                             !(addOp >> mulExpression[BIND2(do_addRHS)]))[BIND2(do_addExpression)];

            mulExpression = (castExpression >>
                             !(mulOp >> castExpression[BIND2(do_mulRHS)]))[BIND2(do_mulExpression)];

            castExpression =
              (primaryExpression | '(' >> type >> ')' >> castExpression)[BIND2(do_castExpression)];

            primaryExpression =
              (value | symbol | '(' >> orExpression >> ')')[BIND2(do_primaryExpression)];

            value = (intrinsic | literal)[BIND2(do_value)];

            symbol = (symbolText >> !('[' >> integer[BIND1(do_sub)] >> ']'))[BIND2(do_symbol)];

            intrinsic = (str_p("getChannel()") | "getMaxChannels()")[BIND2(do_intrinsic)];

            type = (str_p("int64") | "float64" | "rstring")[BIND2(do_type)];

            symbolText = (lexeme_d[alphaUnderscore_p >> *alnumUnderscore_p] - intrinsic -
                          type)[BIND2(do_symbolText)];

            literal = strict_real_p[BIND1(do_real)] | integer[BIND1(do_int)] |
                      confix_p('"', *c_escape_ch_p, '"')[BIND2(do_string)];

            integer = (lexeme_d[longlong_p[BIND1(do_integer)] >> !ch_p('l')]);

            relOp = (str_p("<=") | '<' | ">=" | '>' | "==" | "!=" | "in")[BIND2(do_relOp)];

            addOp = (ch_p('+') | '-')[BIND2(do_addOp)];

            mulOp = (ch_p('*') | '/' | '%')[BIND2(do_mulOp)];

            alphaUnderscore_p = alpha_p | ch_p('_');

            alnumUnderscore_p = alnum_p | ch_p('_');
        }
        rule<ScannerT> const& start() const { return orExpression; }
        rule<ScannerT> orExpression, andExpression, predicate, symbol, symbolText, addExpression,
          mulExpression, castExpression, primaryExpression, value, intrinsic, intrinsicFunction,
          type, literal, integer, relOp, addOp, mulOp, alphaUnderscore_p, alnumUnderscore_p;
    };
    ParserState state;
};

std::auto_ptr<OrExpression> OrExpression::fromString(const std::string& str)
{
    std::string s = boost::trim_copy(str);
    if (s.empty()) {
        return std::auto_ptr<OrExpression>(new OrExpression());
    }

    OrParser parser;
    parse_info<> info = parse(s.c_str(), parser, space_p);
    OrExpression* result = NULL;
    if (info.full) {
        if (parser.state.orExpressions.empty()) {
            THROW(LogicalToPhysical, "Internal error: parse succeeded, but orExpressions empty");
        }
        result = parser.state.orExpressions.top();
        parser.state.orExpressions.pop();
    } else {
        THROW(LogicalToPhysical, "Internal error: OrExpression parse failure");
    }
    return std::auto_ptr<OrExpression>(result);
}

} // namespace SubmissionExpressionEval
} // namespace LogicalToPhysical

SAM_NAMESPACE_END
