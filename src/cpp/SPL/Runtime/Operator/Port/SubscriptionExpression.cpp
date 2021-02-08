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

#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#define BOOST_SPIRIT_THREADSAFE
#ifndef NDEBUG
// Define BOOST_SPIRIT_DEBUG to get extra diagnostics from the parser.
#define BOOST_SPIRIT_DEBUG_OUT std::cerr
#endif
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/spirit/include/classic.hpp>

using namespace std;
using namespace SPL;

boost::spirit::int_parser<long long> const longlong_p = boost::spirit::int_parser<long long>();

SubscriptionExpression::Literal::Literal(const rstring& value)
  : _kind(RString)
  , _rstring(new rstring(value))
{}

SubscriptionExpression::Literal::Literal(const Literal& rhs)
  : _kind(rhs._kind)
{
    switch (_kind) {
        case Int64:
            _int64 = rhs._int64;
            break;
        case Float64:
            _float64 = rhs._float64;
            break;
        case RString:
            _rstring = new rstring(*rhs._rstring);
            break;
    }
}

SubscriptionExpression::Literal& SubscriptionExpression::Literal::operator=(const Literal& rhs)
{
    if (this != &rhs) {
        _kind = rhs._kind;
        switch (_kind) {
            case Int64:
                _int64 = rhs._int64;
                break;
            case Float64:
                _float64 = rhs._float64;
                break;
            case RString:
                _rstring = new rstring(*rhs._rstring);
                break;
        }
    }
    return *this;
}

bool SubscriptionExpression::Literal::operator==(const Literal& rhs) const
{
    if (_kind != rhs._kind) {
        // might be a mismatch on integral FP values
        if (_kind == Int64 && rhs._kind == Float64) {
            return _int64 == rhs._float64;
        }
        if (_kind == Float64 && rhs._kind == Int64) {
            return _float64 == rhs._int64;
        }
        return false;
    }

    switch (_kind) {
        case Int64:
            return _int64 == rhs._int64;
        case Float64:
            return _float64 == rhs._float64;
        case RString:
            return *_rstring == *rhs._rstring;
    }
    return false; // can't get here
}

SubscriptionExpression::Literal::~Literal()
{
    if (_kind == RString) {
        delete _rstring;
    }
}

ostream& SubscriptionExpression::Literal::print(ostream& s) const
{
    switch (_kind) {
        case Int64:
            s << _int64;
            break;
        case Float64:
            SPL::serializeWithPrecision(s, _float64);
            break;
        case RString:
            s << *_rstring;
            break;
    }
    return s;
}

int64 SubscriptionExpression::Literal::getInt64Value() const
{
    if (_kind != Int64) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidLiteral, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_KIND_INVALID("int64"), SPL_FUNC_DBG);
    }
    return _int64;
}

float64 SubscriptionExpression::Literal::getFloat64Value() const
{
    if (_kind != Float64) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidLiteral, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_KIND_INVALID("float64"), SPL_FUNC_DBG);
    }
    return _float64;
}

const rstring& SubscriptionExpression::Literal::getRStringValue() const
{
    if (_kind != RString) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidLiteral, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_KIND_INVALID("rstring"), SPL_FUNC_DBG);
    }
    return *_rstring;
}

SubscriptionExpression::SubscriptionExpression(Kind k)
{
    _kind = k;
    if (k == Predicate) {
        _item = new ItemInfo;
    } else {
        _subClauses = new vector<SubscriptionExpressionPtr>;
    }
}

SubscriptionExpression::~SubscriptionExpression()
{
    if (_kind == Predicate) {
        delete _item;
    } else {
        delete _subClauses;
    }
}

bool SubscriptionExpression::isEmpty() const
{
    // We are true if we are an 'and' or 'or' with no clauses
    if (_kind == Predicate) {
        return false;
    }

    return _subClauses->size() == 0;
}

SubscriptionExpressionPtr SubscriptionExpression::createAndClause(SubscriptionExpressionPtr& lhs,
                                                                  SubscriptionExpressionPtr& rhs)
{
    SubscriptionExpressionPtr p(new SubscriptionExpression(AndClause));
    p->_subClauses->push_back(lhs);
    p->_subClauses->push_back(rhs);
    return p;
}

SubscriptionExpressionPtr SubscriptionExpression::createAndClause(
  SubscriptionExpressionPtr& subClause)
{
    SubscriptionExpressionPtr p(new SubscriptionExpression(AndClause));
    p->_subClauses->push_back(subClause);
    return p;
}

SubscriptionExpressionPtr SubscriptionExpression::createOrClause(
  SubscriptionExpressionPtr& subClause)
{
    SubscriptionExpressionPtr p(new SubscriptionExpression(OrClause));
    p->_subClauses->push_back(subClause);
    return p;
}

SubscriptionExpressionPtr SubscriptionExpression::createOrClause(SubscriptionExpressionPtr& lhs,
                                                                 SubscriptionExpressionPtr& rhs)
{
    SubscriptionExpressionPtr p(new SubscriptionExpression(OrClause));
    p->_subClauses->push_back(lhs);
    p->_subClauses->push_back(rhs);
    return p;
}

SubscriptionExpressionPtr SubscriptionExpression::createPredicate(const string& symbol,
                                                                  Op op,
                                                                  const Literal& lit)
{
    SubscriptionExpressionPtr p(new SubscriptionExpression(Predicate));
    p->_item->_symbol = symbol;
    p->_item->_op = op;
    p->_item->_hasSubscript = false;
    p->_item->_moduloDivisor = 0;
    p->_item->_lit = lit;
    return p;
}

SubscriptionExpressionPtr SubscriptionExpression::createPredicate(const string& symbol,
                                                                  Op op,
                                                                  const Literal& lit,
                                                                  int64 subscript)
{
    if (op == In) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionNoSubscriptWithIn, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_IN_INVALID, SPL_FUNC_DBG);
    }
    SubscriptionExpressionPtr p(new SubscriptionExpression(Predicate));
    p->_item->_symbol = symbol;
    p->_item->_op = op;
    p->_item->_hasSubscript = true;
    p->_item->_subscript = subscript;
    p->_item->_moduloDivisor = 0;
    p->_item->_lit = lit;
    return p;
}

SubscriptionExpressionPtr SubscriptionExpression::createModuloPredicate(const string& symbol,
                                                                        int64 modDivisor,
                                                                        Op op,
                                                                        const Literal& lit)
{
    if (op == In) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionNoSubscriptWithIn, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_IN_INVALID, SPL_FUNC_DBG);
    }
    if (modDivisor == 0) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_MODULUS_DIVISOR_ZERO, SPL_FUNC_DBG);
    }
    SubscriptionExpressionPtr p(new SubscriptionExpression(Predicate));
    p->_item->_symbol = symbol;
    p->_item->_op = op;
    p->_item->_hasSubscript = false;
    p->_item->_moduloDivisor = modDivisor;
    p->_item->_lit = lit;
    return p;
}

SubscriptionExpressionPtr SubscriptionExpression::createModuloPredicate(const string& symbol,
                                                                        int64 modDivisor,
                                                                        Op op,
                                                                        const Literal& lit,
                                                                        int64 subscript)
{
    if (op == In) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionNoSubscriptWithIn, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_IN_INVALID, SPL_FUNC_DBG);
    }
    if (modDivisor == 0) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_MODULUS_DIVISOR_ZERO, SPL_FUNC_DBG);
    }
    SubscriptionExpressionPtr p(new SubscriptionExpression(Predicate));
    p->_item->_symbol = symbol;
    p->_item->_op = op;
    p->_item->_hasSubscript = true;
    p->_item->_subscript = subscript;
    p->_item->_moduloDivisor = modDivisor;
    p->_item->_lit = lit;
    return p;
}

void SubscriptionExpression::addClause(SubscriptionExpressionPtr& subClause)
{
    if (_kind == Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    _subClauses->push_back(subClause);
}

void SubscriptionExpression::removeClause(uint32_t index)
{
    if (_kind == Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    uint32_t n = _subClauses->size();
    if (n < 2) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_REMOVE_LAST(toString()), SPL_FUNC_DBG);
    }
    if (index >= n) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_INVALID_REMOVE(index), SPL_FUNC_DBG);
    }

    // Okay, do it
    _subClauses->erase(_subClauses->begin() + index);
}

const string& SubscriptionExpression::getSymbol() const
{
    if (_kind != Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_symbol;
}

const SubscriptionExpression::Literal& SubscriptionExpression::getLiteral() const
{
    if (_kind != Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_lit;
}

SubscriptionExpression::Op SubscriptionExpression::getOp() const
{
    if (_kind != Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_op;
}

bool SubscriptionExpression::hasSubscript() const
{
    if (_kind != Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_hasSubscript;
}

int64 SubscriptionExpression::getSubscript() const
{
    if (_kind != Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    if (!_item->_hasSubscript) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_INVALID_SUBSCRIPT(toString()), SPL_FUNC_DBG);
    }

    return _item->_subscript;
}

bool SubscriptionExpression::hasModuloDivisor() const
{
    if (_kind != Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_moduloDivisor != 0;
}

int64 SubscriptionExpression::getModuloDivisor() const
{
    if (_kind != Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    if (!_item->_moduloDivisor) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_INVALID_MODULO_DIVISOR(toString()),
                            SPL_FUNC_DBG);
    }

    return _item->_moduloDivisor;
}

uint32_t SubscriptionExpression::getSize() const
{
    if (_kind == Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    return _subClauses->size();
}

SubscriptionExpressionPtr SubscriptionExpression::getSubclause(uint32_t index)
{
    if (_kind == Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    uint32_t n = _subClauses->size();
    if (index >= n) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_INVALID_GETSUBCLAUSE(index), SPL_FUNC_DBG);
    }

    // Okay, do it
    return (*_subClauses)[index];
}

SubscriptionExpression const& SubscriptionExpression::getSubclause(uint32_t index) const
{
    if (_kind == Predicate) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    uint32_t n = _subClauses->size();
    if (index >= n) {
        SPLTRACEMSGANDTHROW(SPLRuntimeSubscriptionInvalidClause, L_ERROR,
                            SPL_RUNTIME_SUBSCRIPTION_INVALID_GETSUBCLAUSE(index), SPL_FUNC_DBG);
    }

    // Okay, do it
    return *((*_subClauses)[index]);
}

ostream& SubscriptionExpression::print(ostream& s) const
{
    switch (_kind) {
        case Predicate:
            if (_item->_op == In) {
                s << _item->_lit << " in " << _item->_symbol;
                if (_item->_hasSubscript) {
                    s << '[' << _item->_subscript << ']';
                }
            } else {
                s << _item->_symbol;
                if (_item->_hasSubscript) {
                    s << '[' << _item->_subscript << ']';
                }
                if (_item->_moduloDivisor) {
                    s << " % " << _item->_moduloDivisor;
                }
                s << ' ' << _item->_op << ' ' << _item->_lit;
            }
            break;
        default: {
            string op = (_kind == AndClause) ? " && " : " || ";
            for (uint32_t i = 0, n = _subClauses->size(); i < n; i++) {
                if (i > 0) {
                    s << op;
                }
                s << '(' << *((*_subClauses)[i]) << ')';
            }
        }
    }
    return s;
}

bool SubscriptionExpression::operator==(const SubscriptionExpression& rhs) const
{
    if (_kind != rhs._kind) {
        // check for some simple equivalences
        if (_kind == Predicate && rhs._subClauses->size() == 1) {
            return *this == *((*rhs._subClauses)[0]);
        }
        if (rhs._kind == Predicate && _subClauses->size() == 1) {
            return *((*_subClauses)[0]) == rhs;
        }

        // no way we could match
        return false;
    }

    switch (_kind) {
        case Predicate:
            if (_item->_op != rhs._item->_op) {
                return false;
            }
            if (_item->_symbol != rhs._item->_symbol) {
                return false;
            }
            if (_item->_hasSubscript != rhs._item->_hasSubscript) {
                return false;
            }
            if (_item->_lit != rhs._item->_lit) {
                return false;
            }
            if (_item->_hasSubscript && _item->_subscript != rhs._item->_subscript) {
                return false;
            }
            if (_item->_moduloDivisor != rhs._item->_moduloDivisor) {
                return false;
            }
            break;
        case AndClause:
        case OrClause:
            if (_subClauses->size() != rhs._subClauses->size()) {
                return false;
            }
            for (uint32_t i = 0, n = _subClauses->size(); i < n; i++) {
                if (*((*_subClauses)[i]) != *((*rhs._subClauses)[i])) {
                    return false;
                }
            }
            break;
    }
    return true;
}

string SubscriptionExpression::toString() const
{
    ostringstream s;
    s.imbue(locale::classic());
    print(s);
    return s.str();
}

ostream& SPL::operator<<(ostream& ostr, const SubscriptionExpression& v)
{
    return v.print(ostr);
}

ostream& SPL::operator<<(ostream& ostr, const SubscriptionExpression::Literal& v)
{
    return v.print(ostr);
}

ostream& SPL::operator<<(ostream& ostr, SubscriptionExpression::Kind v)
{
    switch (v) {
        case SubscriptionExpression::AndClause:
            return ostr << "AndClause";
        case SubscriptionExpression::OrClause:
            return ostr << "OrClause";
        case SubscriptionExpression::Predicate:
            return ostr << "Predicate";
    };
    return ostr;
}

ostream& SPL::operator<<(ostream& ostr, SubscriptionExpression::Op v)
{
    static const char* descriptions[SubscriptionExpression::In + 1] = { "<",  "<=", ">", ">=",
                                                                        "==", "!=", "in" };
    return ostr << descriptions[v];
}

using namespace boost::spirit;
struct ParserState
{
    ParserState()
      : hasSubscript(false)
      , hasModulo(false)
    {}

    void clear()
    {
        expressions.clear();
        hasSubscript = false;
        hasModulo = false;
    }

    void do_symbol(const char* f, const char* l) { currentSymbol.assign(f, l - f); }

    void do_sub(const char* f)
    {
        hasSubscript = true;
        currentSubscript = currentInt;
    }

    void do_mod(const char* f)
    {
        hasModulo = true;
        currentModulo = currentInt;
    }

    void do_integer(int64_t v) { currentInt = v; }

    void do_int(const char* f) { currentLiteral = SubscriptionExpression::Literal(currentInt); }

    void do_real(double v) { currentLiteral = SubscriptionExpression::Literal(v); }

    void do_string(const char* f, const char* l)
    {
        currentLiteral = SubscriptionExpression::Literal(interpretRStringLiteral(string(f, l)));
    }

    void do_op(const char* f, const char* l)
    {
        if (*f == '=') {
            currentOperation = SubscriptionExpression::Equal;
        } else if (*f == '!') {
            currentOperation = SubscriptionExpression::NotEqual;
        } else if (*f == '>') {
            if (f[1] == '=') {
                currentOperation = SubscriptionExpression::GreaterEqual;
            } else {
                currentOperation = SubscriptionExpression::GreaterThan;
            }
        } else {
            assert(*f == '<');
            if (f[1] == '=') {
                currentOperation = SubscriptionExpression::LessEqual;
            } else {
                currentOperation = SubscriptionExpression::LessThan;
            }
        }
    }

    void do_operation(const char* f, const char* l)
    {
        if (hasSubscript) {
            if (hasModulo) {
                expressions.push_back(SubscriptionExpression::createModuloPredicate(
                  currentSymbol, currentModulo, currentOperation, currentLiteral,
                  currentSubscript));
            } else {
                expressions.push_back(SubscriptionExpression::createPredicate(
                  currentSymbol, currentOperation, currentLiteral, currentSubscript));
            }
        } else if (hasModulo) {
            expressions.push_back(SubscriptionExpression::createModuloPredicate(
              currentSymbol, currentModulo, currentOperation, currentLiteral));
        } else {
            expressions.push_back(SubscriptionExpression::createPredicate(
              currentSymbol, currentOperation, currentLiteral));
        }
        hasSubscript = false;
        hasModulo = false;
    }

    void do_in(const char* f, const char* l)
    {
        expressions.push_back(SubscriptionExpression::createPredicate(
          currentSymbol, SubscriptionExpression::In, currentLiteral));
        hasSubscript = false;
        hasModulo = false;
    }

    void do_and(const char*, const char*)
    {
        assert(expressions.size() >= 2);
        SubscriptionExpressionPtr r = expressions.back();
        expressions.pop_back();
        SubscriptionExpressionPtr l = expressions.back();
        expressions.pop_back();
        if (l->getKind() == SubscriptionExpression::AndClause) {
            // We can just add it in to that
            l->addClause(r);
            expressions.push_back(l);
        } else {
            expressions.push_back(SubscriptionExpression::createAndClause(l, r));
        }
    }

    void do_or(const char*, const char*)
    {
        assert(expressions.size() >= 2);
        SubscriptionExpressionPtr r = expressions.back();
        expressions.pop_back();
        SubscriptionExpressionPtr l = expressions.back();
        expressions.pop_back();
        if (l->getKind() == SubscriptionExpression::OrClause) {
            // We can just add it in to that
            l->addClause(r);
            expressions.push_back(l);
        } else {
            expressions.push_back(SubscriptionExpression::createOrClause(l, r));
        }
    }

    vector<SubscriptionExpressionPtr> expressions;
    SubscriptionExpression::Literal currentLiteral;
    string currentSymbol;
    int64_t currentSubscript;
    bool hasSubscript;
    bool hasModulo;
    int64_t currentModulo;
    int64_t currentInt;
    SubscriptionExpression::Op currentOperation;
};

#define BIND1(x) boost::bind(&ParserState::x, &const_cast<subscriptExpn&>(self).state, _1)
#define BIND2(x) boost::bind(&ParserState::x, &const_cast<subscriptExpn&>(self).state, _1, _2)

struct subscriptExpn : public grammar<subscriptExpn>
{
    template<typename ScannerT>
    struct definition
    {
        definition(subscriptExpn const& self)
        {
#ifdef BOOST_SPIRIT_DEBUG
            static int debugFlags =
              (0 != getenv("STREAMS_FILTER_EXPRESSION_DEBUG")) ? BOOST_SPIRIT_DEBUG_FLAGS : 0;
#endif
            BOOST_SPIRIT_DEBUG_TRACE_NODE(expression, debugFlags);
            expression = term >> *(("||" >> term)[BIND2(do_or)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(term, debugFlags);
            term = factor >> *(("&&" >> factor)[BIND2(do_and)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(factor, debugFlags);
            factor = predicate | ('(' >> expression >> ')');

            BOOST_SPIRIT_DEBUG_TRACE_NODE(predicate, debugFlags);
            predicate = (symbolPossibleSub >> op >> literal)[BIND2(do_operation)] |
                        (literal >> "in" >> symbol)[BIND2(do_in)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(symbolPossibleSub, debugFlags);
            symbolPossibleSub = symbol >> !('[' >> integer[BIND1(do_sub)] >> ']') >>
                                  !('%' >> integer[BIND1(do_mod)]) |
                                '(' >> symbol >> !('[' >> integer[BIND1(do_sub)] >> ']') >>
                                  !('%' >> integer[BIND1(do_mod)]) >> ')';

            BOOST_SPIRIT_DEBUG_TRACE_NODE(symbol, debugFlags);
            symbol = (lexeme_d[alphaUnderscore_p >> *alnumUnderscore_p])[BIND2(do_symbol)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(literal, debugFlags);
            literal = strict_real_p[BIND1(do_real)] | integer[BIND1(do_int)] |
                      confix_p('"', *c_escape_ch_p, '"')[BIND2(do_string)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(integer, debugFlags);
            integer = (lexeme_d[longlong_p[BIND1(do_integer)] >> !ch_p('l')]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(op, debugFlags);
            op = (str_p("<=") | '<' | ">=" | '>' | "==" | "!=")[BIND2(do_op)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(alphaUnderscore_p, 0);
            alphaUnderscore_p = alpha_p | ch_p('_');

            BOOST_SPIRIT_DEBUG_TRACE_NODE(alnumUnderscore_p, 0);
            alnumUnderscore_p = alnum_p | ch_p('_');
        }
        rule<ScannerT> const& start() const { return expression; }
        rule<ScannerT> expression, term, predicate, literal, integer, op, symbol, symbolPossibleSub,
          factor, alphaUnderscore_p, alnumUnderscore_p;
    };
    ParserState state;
};

static Distillery::Mutex parserMutex_;
static __thread subscriptExpn* parser_ = NULL;
static vector<boost::shared_ptr<subscriptExpn> > parsers_;
static void allocateParser()
{
#ifdef BOOST_SPIRIT_DEBUG
    static int debugFlags =
      (0 != getenv("STREAMS_FILTER_EXPRESSION_DEBUG")) ? BOOST_SPIRIT_DEBUG_FLAGS : 0;
#endif

    parser_ = new subscriptExpn();
    BOOST_SPIRIT_DEBUG_TRACE_NODE_NAME(*parser_, "subscriptionExpn", debugFlags);
    Distillery::AutoMutex am(parserMutex_);
    parsers_.push_back(boost::shared_ptr<subscriptExpn>(parser_));
}

SubscriptionExpressionPtr SubscriptionExpression::fromString(const string& str)
{
    string s = boost::trim_copy(str);
    if (s.empty()) {
        // This is the empty string; just return an empty 'or' clause
        SubscriptionExpressionPtr p(new SubscriptionExpression(OrClause));
        return p;
    }
    if (!parser_) {
        allocateParser();
    }
    parse_info<> info = parse(s.c_str(), *parser_, space_p);
    SubscriptionExpressionPtr result;
    if (info.full) { // worked
        assert(parser_->state.expressions.size() == 1);
        result = parser_->state.expressions[0];
    }
    parser_->state.clear();
    return result;
}
