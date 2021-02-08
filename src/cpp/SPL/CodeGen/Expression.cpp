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

#include <SPL/CodeGen/Expression.h>

#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/META.h>
#include <SPL/Core/VisitorSpecializations.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

namespace bf = boost::filesystem;

#include <ostream>

using namespace std;
using namespace std::tr1;
using namespace SPL;

#define LINSERT(R, _, T)                                                                           \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        const SPL::list<T>& l = result.BOOST_PP_CAT(get_list_, T)();                               \
        uint32_t num = l.size();                                                                   \
        for (uint32_t i = 0; i < num; i++)                                                         \
            lit->addListElement(*new Literal(elemType, new ExpressionValue(l[i])));                \
    } break;

#define SINSERT(R, _, T)                                                                           \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        const SPL::set<T>& s = result.BOOST_PP_CAT(get_set_, T)();                                 \
        SPL::set<T>::const_iterator it;                                                            \
        for (it = s.begin(); it != s.end(); it++)                                                  \
            lit->addSetElement(*new Literal(elemType, new ExpressionValue(*it)));                  \
    } break;

static Expression* buildExpression(const RootTyp& type,
                                   const SourceLocation& location,
                                   const ExpressionValue& result,
                                   bool dontSimplify = false)
{
    Literal* lit;
    if (result.type() == MetaType::LIST) {
        const ListTyp& lt = static_cast<const ListTyp&>(type);
        const RootTyp& elemType = lt.getElementType();
        lit = new Literal(type, NULL);
        switch (result.elementType()) {
            BOOST_PP_LIST_FOR_EACH(LINSERT, _, EV_BUILTIN_TYPES)
            default:;
        }
    } else if (result.type() == MetaType::SET) {
        const SetTyp& st = static_cast<const SetTyp&>(type);
        const RootTyp& elemType = st.getElementType();
        lit = new Literal(type, NULL);
        switch (result.elementType()) {
            BOOST_PP_LIST_FOR_EACH(SINSERT, _, EV_BUILTIN_TYPES)
            default:;
        }
    } else {
        lit = new Literal(type, new ExpressionValue(result));
    }
    LiteralExpression* l = new LiteralExpression(type, location, *lit);
    if (dontSimplify) {
        l->setDontSimplify();
    }
    return l;
}

const char* Expression::opDescription[numOps] = {
    "<INVALID>", "++",  "--", "!",           "~",      "-",      "*",   "/",   "%",   "+",
    "-",         "&",   "^",  "|",           "&&",     "||",     "<",   "<=",  ">",   ">=",
    "!=",        "==",  "<<", ">>",          "in",     "=",      "*=",  "/=",  "%=",  "+=",
    "-=",        "&=",  "^=", "|=",          "<<=",    ">>=",    ".*",  "./",  ".%",  ".+",
    ".-",        ".&",  ".^", ".|",          ".<",     ".<=",    ".>",  ".>=", ".!=", ".==",
    ".<<",       ".>>", ".",  "<subscript>", "<call>", "<cast>", "<?:>"
};

Expression::~Expression() {}

bool Expression::equal(const Expression& rhs) const
{
    return _kind == rhs._kind && _type == rhs._type && equal(rhs);
}

bool Expression::operator==(const Expression& rhs) const
{
    return Expression::equal(rhs);
}

string Expression::toString() const
{
    ostringstream s;
    print(s, *this);
    return s.str();
}

ostream& SPL::operator<<(ostream& ostr, const Expression& v)
{
    return v.print(ostr, v);
}

ostream& SPL::operator<<(ostream& ostr, Expression::Kind v)
{
    const char* str = "";
    switch (v) {
        case Expression::Attribute:
            str = "AttributeExpression";
            break;
        case Expression::Binary:
            str = "BinaryExpression";
            break;
        case Expression::Call:
            str = "CallExpression";
            break;
        case Expression::Cast:
            str = "CastExpression";
            break;
        case Expression::Conditional:
            str = "ConditionalExpression";
            break;
        case Expression::Enum:
            str = "EnumExpression";
            break;
        case Expression::Lit:
            str = "LitExpression";
            break;
        case Expression::LiteralSymbol:
            str = "LiteralSymbolExpression";
            break;
        case Expression::Postfix:
            str = "PostfixExpression";
            break;
        case Expression::Prefix:
            str = "PrefixExpression";
            break;
        case Expression::StreamHistorySymbol:
            str = "StreamHistorySymbolExpression";
            break;
        case Expression::StreamSymbol:
            str = "StreamSymbolExpression";
            break;
        case Expression::Subscript:
            str = "SubscriptExpression";
            break;
        case Expression::Symbol:
            str = "SymbolExpression";
            break;
        case Expression::CustomLiteral:
            str = "CustomLiteralExpression";
            break;
        case Expression::Unary:
            str = "UnaryExpression";
            break;
        case Expression::Nary:
            str = "NaryExpression";
            break;
        case Expression::Unwrap:
            str = "UnwrapExpression";
            break;
        case Expression::UnwrapOrElse:
            str = "UnWrapOrElseExpression";
            break;
        case Expression::IsPresent:
            str = "IsPresentExpression";
            break;
        default:
            SPLDBG("Invalid expression kind = " << v, Debug::TraceOperatorVerify);
            assert(!"Invalid expression kind");
            break;
    }
    return ostr << str;
}

Expression* Expression::simplify(Expression* e, ExpressionEvaluator& eval)
{
    if (!e || e->_dontSimplify) {
        return e;
    }
    Expression* simple = e->simplify(eval);
    if (simple != e) {
        // Note that the following delete caused problems when another copy of an AST still had AST nodes with pointers to expressions that were deleted as a side effect of this delete
        //        delete e;
        return simple;
    }
    // unchanged
    return e;
}

Expression* Expression::replaceLits(Expression* e, LiteralReplacer& lit, bool onlySTP)
{
    if (!e) {
        return NULL;
    }
    Expression* rewritten = e->replaceLits(lit, onlySTP);
    if (rewritten != e) {
        delete e;
        return rewritten;
    }

    // unchanged
    return e;
}

namespace SPL {
template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   Expression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind(), context.debugFlag());

    if (context._types) {
        Collector::collectType(*context._types, expr.getType());
    }

    return DepthFirstVisitor<CollectionContainer>::visitExpression(context, expr);
}

template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   CallExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind(), context.debugFlag());

    if (context._fcns) {
        // Ignore pseudo functions
        const FunctionHeadSym& fcn = expr.getFcn();
        BlockStatement* ir = fcn.functionBodyIR();
        if (!ir) {
            ir = FrontEndDriver::instance().getUsedSPLFunctions().findBlockStatement(fcn);
        }
        if (!fcn.isIntrinsic()) {
            if (context._fcns->find(&fcn) == context._fcns->end()) {
                context._fcns->insert(&fcn);
                // We need to walk the function as well to get nested types & fcns
                if (context._inFcns && !fcn.isNativeFunction() && ir) {
                    DepthFirstVisitor<CollectionContainer>::visit(context, *ir);
                }
            }
        }
    }

    return DepthFirstVisitor<CollectionContainer>::visitCallExpression(context, expr);
}

template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   LiteralExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind(), context.debugFlag());

    expr.getValue().collectTypeAndFunctions(context._types, context._fcns, context._inFcns);
    return DepthFirstVisitor<CollectionContainer>::visitLiteralExpression(context, expr);
}
};

void Expression::collectTypeAndFunctions(unordered_set<const RootTyp*>* types,
                                         unordered_set<const FunctionHeadSym*>* fcns,
                                         bool inFcns) const
{
    CollectionContainer collectInfo(types, fcns, inFcns);
    DepthFirstVisitor<CollectionContainer> visitor;
    visitor.visit(collectInfo, *this);
}

string Expression::remapId(SymbolType type, const string& id)
{
    switch (type) {
        case State:
            return "state$" + id;
        case Local:
            return "id$" + id;
        case Other:
        case Attr:
            break;
    }
    return Utility::remapId(id);
}

/*
 * Literal Expression
 */
LiteralExpression::LiteralExpression(const RootTyp& type, const SourceLocation& l, Literal& lit)
  : Expression(Lit, type, l)
  , _value(&lit)
{
    MODEL("LiteralExpression(" << (void*)this << ")\n");
}

LiteralExpression::~LiteralExpression()
{
    delete _value;
}

bool LiteralExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const LiteralExpression& lRhs = static_cast<const LiteralExpression&>(rhs);
    return getType() == rhs.getType() && *_value == *lRhs._value;
}

const Literal& LiteralExpression::getValue() const
{
    return *_value;
}

Literal& LiteralExpression::getValue()
{
    return *_value;
}

bool LiteralExpression::hasSideEffects(void) const
{
    return _value->hasSideEffects();
}

bool LiteralExpression::readsState(void) const
{
    return _value->readsState();
}

ostream& LiteralExpression::print(ostream& ostr, const Expression& root) const
{
    return ostr << *_value;
}

LiteralExpression& LiteralExpression::clone() const
{
    return *new LiteralExpression(getType(), getLocation(), _value->clone());
}

void LiteralExpression::generate(ostream& s,
                                 uint32_t depth,
                                 GenerationContext context,
                                 bool genTemplate) const
{
    if (!_replFcnName.empty()) {
        assert(_value->compileTimeEvaluatable());
        s << _replFcnName << "()";
    } else {
        _value->generate(s, false, genTemplate);
    }
}

Expression* LiteralExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    // The base case for replacement.
    if (onlySTP) {
        return this; // don't actually replace literals
    }
    // Is this something we can work with?
    if (!_value->compileTimeEvaluatable()) {
        Literal* l = _value->replaceLits(lit);
        if (l != _value) {
            delete _value;
            _value = l;
        }
        return this;
    }

    // Can't handle empty tuple literals here
    if (_value->emptyTupleLiteral()) {
        return this;
    }

    // We can do something....
    uint32_t litIndex;
    uint32_t idIndex = lit.addLiteral(*_value, _location, litIndex);
    return new LiteralSymbolExpression(_type, _location, idIndex, litIndex);
}

bool LiteralExpression::getExpressionValue(ExpressionValue& result) const
{
    switch (_value->getKind()) {
        case Literal::Primitive:
            // easy case;
            result = _value->primitive();
            return true;
        case Literal::List:
        case Literal::Set:
            break;
        default:
            return false;
    }

    if (!_value->compileTimeEvaluatable()) {
        return false;
    }

    if (_value->getKind() == Literal::List) {
        // we have a list.  Is it a list of primitives?
        const RootTyp& list_type = _value->getType();
        if (list_type.getMetaType() != MetaType::LIST) {
            return false;
        }

#define LEXTRACT(R, _, T)                                                                          \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        SPL::list<T> l;                                                                            \
        uint32_t num = _value->numElements();                                                      \
        for (uint32_t i = 0; i < num; i++)                                                         \
            l.push_back(_value->listSetElement(i).primitive().BOOST_PP_CAT(get_, T)());            \
        result = l;                                                                                \
    } break;

        const ListTyp& lt = static_cast<const ListTyp&>(list_type);
        switch (lt.getElementType().getMetaType()) {
            BOOST_PP_LIST_FOR_EACH(LEXTRACT, _, EV_BUILTIN_TYPES)
            default:
                return false;
        }
    } else {
        // we have a set.  Is it a set of primitives?
        const RootTyp& set_type = _value->getType();
        if (set_type.getMetaType() != MetaType::SET) {
            return false;
        }

#define SEXTRACT(R, _, T)                                                                          \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        SPL::set<T> l;                                                                             \
        uint32_t num = _value->numElements();                                                      \
        for (uint32_t i = 0; i < num; i++)                                                         \
            l.insert(_value->listSetElement(i).primitive().BOOST_PP_CAT(get_, T)());               \
        result = l;                                                                                \
    } break;

        const SetTyp& st = static_cast<const SetTyp&>(set_type);
        switch (st.getElementType().getMetaType()) {
            BOOST_PP_LIST_FOR_EACH(SEXTRACT, _, EV_BUILTIN_TYPES)
            default:
                return false;
        }
    }
    return true;
};

/*
 * Symbol Expression
 */
SymbolExpression::SymbolExpression(const RootTyp& type,
                                   const SourceLocation& l,
                                   const string& id,
                                   SymbolType symbolType,
                                   int32_t builtinNum)
  : Expression(Symbol, type, l)
  , _id(id)
  , _builtinNum(builtinNum)
  , _symbolType(symbolType)
{
    MODEL("SymbolExpression(" << (void*)this << ", " << id << ", " << (int)symbolType << ")\n");
}

SymbolExpression::SymbolExpression(Kind k,
                                   const RootTyp& type,
                                   const SourceLocation& l,
                                   const string& id,
                                   int32_t builtinNum)
  : Expression(k, type, l)
  , _id(id)
  , _builtinNum(builtinNum)
  , _symbolType(Other)
{}

bool SymbolExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const SymbolExpression& syRhs = static_cast<const SymbolExpression&>(rhs);

    return _id == syRhs._id && _builtinNum == syRhs._builtinNum && _symbolType == syRhs._symbolType;
}

void SymbolExpression::generate(ostream& s,
                                uint32_t depth,
                                GenerationContext context,
                                bool genTemplate) const
{
    s << remapId(_symbolType, _id);
}

ostream& SymbolExpression::print(ostream& ostr, const Expression& root) const
{
    return ostr << _id;
}

SymbolExpression& SymbolExpression::clone() const
{
    return *new SymbolExpression(getType(), getLocation(), _id, _symbolType, _builtinNum);
}

/*
 * Custom Literal Expression
 */
CustomLiteralExpression::CustomLiteralExpression(const RootTyp& type,
                                                 const SourceLocation& l,
                                                 const string& id,
                                                 const string& enumName)
  : SymbolExpression(CustomLiteral, type, l, id)
  , _enumName(enumName)
{
    MODEL("CustomLiteralExpression(" << (void*)this << ", " << id << ")\n");
}

bool CustomLiteralExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const CustomLiteralExpression& syRhs = static_cast<const CustomLiteralExpression&>(rhs);

    return _enumName == syRhs._enumName && this->SymbolExpression::equal(rhs);
}

CustomLiteralExpression& CustomLiteralExpression::clone() const
{
    return *new CustomLiteralExpression(getType(), getLocation(), _id, _enumName);
}

void CustomLiteralExpression::generate(ostream& s,
                                       uint32_t depth,
                                       GenerationContext context,
                                       bool genTemplate) const
{
    TypeGenerator const& tg = TypeGenerator::instance();
    s << tg.getCppType(getType()) << "::" << _id;
}

/*
 * Stream Symbol Expression
 */
StreamSymbolExpression::StreamSymbolExpression(const RootTyp& type,
                                               const SourceLocation& l,
                                               const string& id,
                                               int32_t port)
  : SymbolExpression(StreamSymbol, type, l, id)
  , _port(port)
{
    MODEL("StreamSymbolExpression(" << (void*)this << ", " << id << ", " << port << ")\n");
}

StreamSymbolExpression::StreamSymbolExpression(Kind k,
                                               const RootTyp& type,
                                               const SourceLocation& l,
                                               const string& id,
                                               int32_t port)
  : SymbolExpression(k, type, l, id)
  , _port(port)
{}

bool StreamSymbolExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const StreamSymbolExpression& syRhs = static_cast<const StreamSymbolExpression&>(rhs);

    return _port == syRhs._port && this->SymbolExpression::equal(rhs);
}

void StreamSymbolExpression::generate(ostream& s,
                                      uint32_t depth,
                                      GenerationContext context,
                                      bool genTemplate) const
{
    s << "iport$" << _port;
}

ostream& StreamSymbolExpression::print(ostream& ostr, const Expression& root) const
{
    return ostr << _id;
}

StreamSymbolExpression& StreamSymbolExpression::clone() const
{
    return *new StreamSymbolExpression(getType(), getLocation(), _id, _port);
}

/*
 * Stream History Symbol Expression
 */
StreamHistorySymbolExpression::StreamHistorySymbolExpression(const RootTyp& type,
                                                             const SourceLocation& l,
                                                             const string& id,
                                                             int32_t port,
                                                             int32_t past_depth)
  : StreamSymbolExpression(StreamHistorySymbol, type, l, id, port)
  , _past_depth(past_depth)
{
    MODEL("StreamHistorySymbolExpression(" << (void*)this << ", " << id << ", " << port << ", "
                                           << past_depth << ")\n");
}

bool StreamHistorySymbolExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const StreamHistorySymbolExpression& syRhs =
      static_cast<const StreamHistorySymbolExpression&>(rhs);

    return _past_depth == syRhs._past_depth && this->StreamSymbolExpression::equal(rhs);
}

void StreamHistorySymbolExpression::generate(ostream& s,
                                             uint32_t depth,
                                             GenerationContext context,
                                             bool genTemplate) const
{
    s << "iport$" << _port << "History[" << _past_depth << ']';
}

ostream& StreamHistorySymbolExpression::print(ostream& ostr, const Expression& root) const
{
    return ostr << _id << '[' << _past_depth << ']';
}

StreamHistorySymbolExpression& StreamHistorySymbolExpression::clone() const
{
    return *new StreamHistorySymbolExpression(getType(), getLocation(), _id, _port, _past_depth);
}

/*
 * Literal Symbol Expression (literal converted to symbol)
 */
static string genLitId(uint32_t idIndex)
{
    ostringstream s;
    s << "lit$" << idIndex;
    return s.str();
}

LiteralSymbolExpression::LiteralSymbolExpression(const RootTyp& type,
                                                 const SourceLocation& l,
                                                 uint32_t idIndex,
                                                 uint32_t litIndex)
  : SymbolExpression(LiteralSymbol, type, l, genLitId(idIndex))
  , _idIndex(idIndex)
  , _litIndex(litIndex)
{
    MODEL("LiteralSymbolExpression(" << (void*)this << ", " << idIndex << ", " << litIndex
                                     << ")\n");
}

LiteralSymbolExpression& LiteralSymbolExpression::clone() const
{
    return *new LiteralSymbolExpression(getType(), getLocation(), _idIndex, _litIndex);
}

/*
 * Enum Symbol Expression (named enum value)
 */
EnumExpression::EnumExpression(const RootTyp& type, const SourceLocation& l, const string& id)
  : Expression(Enum, type, l)
  , _id(id)
{
    MODEL("EnumExpression(" << (void*)this << ", " << id << ")\n");
    // Compute the value of the literal
    assert(type.getMetaType() == MetaType::ENUM);
    const EnumTyp& e = static_cast<const EnumTyp&>(type);
    uint32_t numValues = e.getNumberOfValues();
    const vector<string>& values = e.getValidValues();

    _value = NULL;
    for (uint32_t i = 0; i < numValues; i++) {
        if (values[i] == id) {
            SPL::uint32 v = i;
            RootTyp const& tp = TypeFactory::instance().primitiveT(MetaType::UINT32);
            ExpressionValue* ev = new ExpressionValue(v);
            _value = new Literal(tp, ev, id);
            break;
        }
    }
    assert(_value);
}

EnumExpression::~EnumExpression()
{
    delete _value;
}

EnumExpression& EnumExpression::clone() const
{
    return *new EnumExpression(getType(), getLocation(), _id);
}

bool EnumExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const EnumExpression& eRhs = static_cast<const EnumExpression&>(rhs);
    return getType() == rhs.getType() && _id == eRhs._id;
}

ostream& EnumExpression::print(ostream& ostr, const Expression& root) const
{
    return ostr << _id;
}

void EnumExpression::generate(ostream& s,
                              uint32_t depth,
                              GenerationContext context,
                              bool genTemplate) const
{
    TypeGenerator const& tg = TypeGenerator::instance();
    s << tg.getCppType(getType()) << "::" << _id;
}

bool EnumExpression::getExpressionValue(ExpressionValue& result) const
{
    result = _value->primitive();
    return true;
}

/*
 * Binary Expression
 */
BinaryExpression::BinaryExpression(const RootTyp& type,
                                   const SourceLocation& l,
                                   Op op,
                                   Expression& lhs,
                                   Expression& rhs)
  : Expression(Binary, type, l)
  , _lhs(&lhs)
  , _rhs(&rhs)
  , _op(op)
{
    MODEL("BinaryExpression(" << (void*)this << ", " << (void*)&lhs << ", " << (int)op << ", "
                              << (void*)&rhs << ")\n");
}

BinaryExpression::BinaryExpression(Kind k,
                                   const RootTyp& type,
                                   const SourceLocation& l,
                                   Op op,
                                   Expression& lhs,
                                   Expression& rhs)
  : Expression(k, type, l)
  , _lhs(&lhs)
  , _rhs(&rhs)
  , _op(op)
{}

BinaryExpression::~BinaryExpression()
{
    delete _lhs;
    delete _rhs;
}

bool BinaryExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const BinaryExpression& binRhs = static_cast<const BinaryExpression&>(rhs);

    return _op == binRhs._op && getType() == rhs.getType() && *_lhs == *binRhs._lhs &&
           *_rhs == *binRhs._rhs;
}

bool BinaryExpression::readsState() const
{
    if (_rhs->readsState()) {
        return true;
    }

    switch (_op) {
        case eq:
        case starEq:
        case slashEq:
        case modEq:
        case plusEq:
        case minusEq:
        case ampEq:
        case hatEq:
        case barEq:
        case lShiftEq:
        case rShiftEq:
            return false;
        default:;
    }

    if (_lhs->readsState()) {
        return true;
    }

    return false;
}

bool BinaryExpression::hasSideEffects(void) const
{
    if (_lhs->hasSideEffects() || _rhs->hasSideEffects()) {
        return true;
    }

    switch (_op) {
        case eq:
        case starEq:
        case slashEq:
        case modEq:
        case plusEq:
        case minusEq:
        case ampEq:
        case hatEq:
        case barEq:
        case lShiftEq:
        case rShiftEq:
            return true;
        default:;
    }

    return false;
}

ostream& BinaryExpression::print(ostream& ostr, const Expression& root) const
{
    bool parens = true;
    switch (_op) {
        case eq:
        case starEq:
        case slashEq:
        case modEq:
        case plusEq:
        case minusEq:
        case ampEq:
        case hatEq:
        case barEq:
        case lShiftEq:
        case rShiftEq:
            parens = false;
            break;
        default:;
    }
    if (parens && this != &root) {
        ostr << '(';
        _lhs->print(ostr, root);
        ostr << ' ' << opDescription[_op] << ' ';
        _rhs->print(ostr, root);
        ostr << ')';
    } else {
        _lhs->print(ostr, root);
        ostr << ' ' << opDescription[_op] << ' ';
        _rhs->print(ostr, root);
    }
    return ostr;
}

BinaryExpression& BinaryExpression::clone() const
{
    return *new BinaryExpression(getType(), getLocation(), _op, _lhs->clone(), _rhs->clone());
}

string BinaryExpression::dotOperator(Op op)
{
    switch (op) {
        case dotPlus:
            return "dotPlus";
        case dotMinus:
            return "dotMinus";
        case dotStar:
            return "dotTimes";
        case dotSlash:
            return "dotDivides";
        case dotMod:
            return "dotModulus";
        case dotLShift:
            return "dotLshift";
        case dotRShift:
            return "dotRshift";
        case dotAmp:
            return "dotAnd";
        case dotBar:
            return "dotOr";
        case dotHat:
            return "dotXor";
        case dotEqEq:
            return "dotEqual";
        case dotNeq:
            return "dotNotEqual";
        case dotGreater:
            return "dotGreaterThan";
        case dotGeq:
            return "dotGreaterEqual";
        case dotLess:
            return "dotLessThan";
        case dotLeq:
            return "dotLessEqual";
        default:
            assert(false);
            break;
    }
    return "";
}

string BinaryExpression::opEqualOperator(Op op)
{
    switch (op) {
        case plusEq:
            return "assignPlus";
        case minusEq:
            return "assignMinus";
        case starEq:
            return "assignTimes";
        case slashEq:
            return "assignDivides";
        case modEq:
            return "assignModulus";
        case lShiftEq:
            return "assignLshift";
        case rShiftEq:
            return "assignRshift";
        case ampEq:
            return "assignAnd";
        case barEq:
            return "assignOr";
        case hatEq:
            return "assignXor";
        default:
            assert(false);
            break;
    }
    return "";
}

Expression::Op BinaryExpression::mapOperator() const
{
    MetaType lmt = _lhs->getType().getMetaType();
    MetaType rmt = _rhs->getType().getMetaType();

    bool needsDot = false;
    switch (lmt) {
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::MAP:
        case MetaType::BMAP:
            needsDot = true;
            break;
        default:
            break;
    }
    switch (rmt) {
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::MAP:
        case MetaType::BMAP:
            needsDot = true;
            break;
        default:
            break;
    }
    switch (_op) {
        case plus:
            if (needsDot) {
                return dotPlus;
            }
            break;
        case minus:
            if (needsDot) {
                return dotMinus;
            }
            break;
        case star:
            if (needsDot) {
                return dotStar;
            }
            break;
        case slash:
            if (needsDot) {
                return dotSlash;
            }
            break;
        case mod:
            if (needsDot) {
                return dotMod;
            }
            break;
        case lShift:
            if (needsDot) {
                return dotLShift;
            }
            break;
        case rShift:
            if (needsDot) {
                return dotRShift;
            }
            break;
        case amp:
            if (needsDot) {
                return dotAmp;
            }
            break;
        case bar:
            if (needsDot) {
                return dotBar;
            }
            break;
        case hat:
            if (needsDot) {
                return dotHat;
            }
            break;
        case greater:
            if (needsDot) {
                return dotGreater;
            }
            break;
        case geq:
            if (needsDot) {
                return dotGeq;
            }
            break;
        case less:
            if (needsDot) {
                return dotLess;
            }
            break;
        case leq:
            if (needsDot) {
                return dotLeq;
            }
            break;
        case eqEq:
            if (needsDot && lmt != rmt) {
                return dotEqEq;
            }
            break;
        case neq:
            if (needsDot && lmt != rmt) {
                return dotNeq;
            }
            break;
        default:
            break;
    }
    return _op;
}

void BinaryExpression::generate(ostream& s,
                                uint32_t depth,
                                GenerationContext context,
                                bool genTemplate) const
{
    bool parens = true;
    Op op = mapOperator();
    switch (op) {
        case eq:
        case starEq:
        case slashEq:
        case modEq:
        case plusEq:
        case minusEq:
        case ampEq:
        case hatEq:
        case barEq:
        case lShiftEq:
        case rShiftEq:
        case in:
            parens = false;
            break;
        default:;
    }
    switch (op) {
        case in:
            switch (_rhs->getType().getMetaType()) {
                case MetaType::LIST:
                case MetaType::BLIST:
                case MetaType::SET:
                case MetaType::BSET:
                case MetaType::MAP:
                case MetaType::BMAP:
                    s << "::SPL::Functions::Collections::has(";
                    if (genTemplate) {
                        s << "{Rhs}";
                    } else {
                        _rhs->generate(s, depth, General, genTemplate);
                    }
                    s << ", ";
                    if (genTemplate) {
                        s << "{Lhs}";
                    } else {
                        _lhs->generate(s, depth, General, genTemplate);
                    }
                    s << ")";
                    break;
                default:
                    assert(FALSE);
                    break;
            }
            break;
        case dotPlus:
        case dotMinus:
        case dotStar:
        case dotSlash:
        case dotMod:
        case dotLShift:
        case dotRShift:
        case dotAmp:
        case dotBar:
        case dotHat:
        case dotEqEq:
        case dotNeq:
        case dotGreater:
        case dotGeq:
        case dotLess:
        case dotLeq:
            s << "::SPL::" << dotOperator(op) << '(';
            if (genTemplate) {
                s << "{Lhs}";
            } else {
                _lhs->generate(s, depth, General, genTemplate);
            }
            s << ", ";
            if (genTemplate) {
                s << "{Rhs}";
            } else {
                _rhs->generate(s, depth, General, genTemplate);
            }
            s << ')';
            break;

        case starEq:
        case slashEq:
        case modEq:
        case plusEq:
        case minusEq:
        case ampEq:
        case hatEq:
        case barEq:
        case lShiftEq:
        case rShiftEq:
            switch (_lhs->getType().getMetaType()) {
                case MetaType::LIST:
                case MetaType::BLIST:
                case MetaType::MAP:
                case MetaType::BMAP:
                    s << "::SPL::" << opEqualOperator(op) << '(';
                    if (genTemplate) {
                        s << "{Lhs}";
                    } else {
                        _lhs->generate(s, depth, General, genTemplate);
                    }
                    s << ", ";
                    if (genTemplate) {
                        s << "{Rhs}";
                    } else {
                        _rhs->generate(s, depth, General, genTemplate);
                    }
                    s << ')';
                    return;
                default:
                    break;
            }
            goto default_bin;

        default:
            switch (_type.getMetaType()) {
                case MetaType::INT8:
                case MetaType::UINT8:
                case MetaType::INT16:
                case MetaType::UINT16:
                case MetaType::INT32:
                case MetaType::UINT32:
                case MetaType::INT64:
                case MetaType::UINT64:
                    // Need to prevent C type promotions
                    s << TypeGenerator::instance().getCppType(_type);
                    parens = true;
                case MetaType::RSTRING:
                case MetaType::USTRING:
                // FALLTHROUGH
                default:
                default_bin:
                    if (parens) {
                        s << '(';
                    }
                    if (genTemplate) {
                        s << "{Lhs}";
                    } else {
                        _lhs->generate(s, depth, op == eq ? AssignmentLHS : General, genTemplate);
                    }
                    s << ' ' << opDescription[op] << ' ';
                    if (genTemplate) {
                        s << "{Rhs}";
                    } else {
                        _rhs->generate(s, depth, General, genTemplate);
                    }
                    if (parens) {
                        s << ')';
                    }
                    break;
            }
    }
}

Expression* BinaryExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    // simplify children
    _rhs = Expression::replaceLits(_rhs, lit, onlySTP);
    _lhs = Expression::replaceLits(_lhs, lit, onlySTP);
    return this;
}

static bool isInteger(MetaType t)
{
    switch (t) {
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
            return true;
        default:;
    }
    return false;
}

static bool isIntString(MetaType t)
{
    switch (t) {
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::RSTRING:
        case MetaType::USTRING:
            return true;
        default:;
    }
    return false;
}

Expression* BinaryExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children
    _lhs = Expression::simplify(_lhs, eval);

    ExpressionValue l, r;
    // Is this a shortcut operator?
    if (_op == barBar) {
        if (_lhs->getExpressionValue(l)) {
            if (l.type() != MetaType::BOOLEAN) {
                return this; // error condition
            }
            if (l.get_boolean()) {
                // We know it is true.  Stop here
                return buildExpression(_type, _location, l);
            }
            // The result is the RHS
            Expression* r1 = _rhs;
            _rhs = NULL;
            return Expression::simplify(r1, eval);
        }
    } else if (_op == ampAmp) {
        if (_lhs->getExpressionValue(l)) {
            if (l.type() != MetaType::BOOLEAN) {
                return this; // error condition
            }
            if (!l.get_boolean()) {
                // We know it is false.  Stop here
                return buildExpression(_type, _location, l);
            }
            // The result is the RHS
            Expression* r1 = _rhs;
            _rhs = NULL;
            return Expression::simplify(r1, eval);
        }
    }

    _rhs = Expression::simplify(_rhs, eval);

    // Try to simplify
    if (_lhs->dontSimplify() || _rhs->dontSimplify()) {
        return this; // We shouldn't do any more.
    }

    ExpressionEvaluator::Operation op;
    switch (_op) {
        case dotPlus:
        case plus:
            op = ExpressionEvaluator::Add;
            break;
        case dotMinus:
        case minus:
            op = ExpressionEvaluator::Sub;
            break;
        case dotStar:
        case star:
            op = ExpressionEvaluator::Mul;
            break;
        case dotSlash:
        case slash:
            op = ExpressionEvaluator::Div;
            break;
        case dotMod:
        case mod:
            op = ExpressionEvaluator::Mod;
            break;
        case dotLShift:
        case lShift:
            op = ExpressionEvaluator::LeftShift;
            break;
        case dotRShift:
        case rShift:
            op = ExpressionEvaluator::RightShift;
            break;
        case dotAmp:
        case amp:
            op = ExpressionEvaluator::And;
            break;
        case dotBar:
        case bar:
            op = ExpressionEvaluator::Or;
            break;
        case dotHat:
        case hat:
            op = ExpressionEvaluator::Xor;
            break;
        case in:
            op = ExpressionEvaluator::In;
            break;
        case eqEq:
            op = ExpressionEvaluator::Equal;
            break;
        case dotEqEq:
            op = ExpressionEvaluator::DotEqual;
            break;
        case neq:
            op = ExpressionEvaluator::NotEqual;
            break;
        case dotNeq:
            op = ExpressionEvaluator::DotNotEqual;
            break;
        case dotGreater:
        case greater:
            op = ExpressionEvaluator::GreaterThan;
            break;
        case dotGeq:
        case geq:
            op = ExpressionEvaluator::GreaterEqual;
            break;
        case dotLess:
        case less:
            op = ExpressionEvaluator::LessThan;
            break;
        case dotLeq:
        case leq:
            op = ExpressionEvaluator::LessEqual;
            break;
        default:
            return this; // give up!
    }

    bool lValue = _lhs->getExpressionValue(l);
    bool rValue = _rhs->getExpressionValue(r);

    // Look at the possibilities
    if (!lValue && !rValue) {
        return this; // neither simplified
    }

    if (lValue && !rValue) {
        // lhs is a literal, rhs is not.   What can we do?
        int64_t iValue;
        double dValue;
        if (isInteger(l.type()) && l.getIntegerValue(iValue) && r.type() != MetaType::LIST) {
            if (iValue == 0) {
                if (_op == plus || _op == hat) {
                    return &_rhs->clone(); // 0 + x -> x; 0 ^ x -> x
                } else if ((_op == amp || _op == lShift || _op == rShift) &&
                           !_rhs->hasSideEffects()) {
                    return buildExpression(_type, _location, l); // 0 & x -> 0
                }
            } else if (iValue == 1) {
                if (_op == star) {
                    return &_rhs->clone(); // 1 * x -> x
                }
            }
        } else if (l.getFloatValue(dValue)) {
            if (dValue == 0.0) {
                if (_op == plus) {
                    return &_rhs->clone(); // 0 + x -> x
                }
            }
        }
        return this; // can't do anything
    }

    if (!lValue && rValue) {
        // lhs is not a literal, rhs is a literal.   What can we do?
        int64_t iValue;
        double dValue;
        if (isInteger(r.type()) && r.getIntegerValue(iValue) && l.type() != MetaType::LIST) {
            if (iValue == 0) {
                if (_op == plus || _op == hat) {
                    return &_lhs->clone(); // x + 0 -> x; x ^ 0 -> x
                } else if (_op == lShift || _op == rShift) {
                    return &_lhs->clone(); // x >> 0 -> x; x << 0 -> x
                } else if (_op == amp && !_lhs->hasSideEffects()) {
                    return buildExpression(_type, _location, r); // x & 0 -> 0
                }
            } else if (iValue == 1) {
                if (_op == star) {
                    return &_lhs->clone(); // x * 1 -> x
                }
            }
        } else if (r.getFloatValue(dValue)) {
            if (dValue == 0.0) {
                if (_op == plus || _op == minus) {
                    return &_lhs->clone(); // x +/- 0 -> x
                }
            }
        }
        // See if we can play around with  (a OP lit) OP lit
        if ((_op == plus || _op == star || _op == amp || _op == bar || _op == hat) &&
            isIntString(r.type()) && _lhs->getKind() == Binary && !eval.dontCatch()) {
            const BinaryExpression& binLhs = static_cast<const BinaryExpression&>(*_lhs);
            ExpressionValue lr;
            bool lrValue = binLhs.getRhs().getExpressionValue(lr);
            if (lrValue && lr.type() == r.type() && _op == binLhs.getOp()) {
                // RHS of binary op is also a literal
                try {
                    ExpressionValue result;
                    if (eval.evaluateOp(op, lr, r, result)) {
                        Expression* lit = buildExpression(_type, _location, result);
                        return new BinaryExpression(_type, _location, _op, binLhs.getLhs().clone(),
                                                    *lit);
                    }
                } catch (...) {
                    // Didn't work, ignore
                }
            }
        }
        return this; // can't do anything
    }

    // They are BOTH literals
    if (eval.dontCatch()) {
        ExpressionValue result;
        if (eval.evaluateOp(op, l, r, result)) {
            return buildExpression(_type, _location, result);
        }
    } else {
        try {
            ExpressionValue result;
            if (eval.evaluateOp(op, l, r, result)) {
                return buildExpression(_type, _location, result);
            }
        } catch (Distillery::DistilleryException& excp) {
            SysOut::errorln(SPL_CORE_EVALUATION_EXCEPTION_MSG(excp.getExplanation(), toString()),
                            _location);
        } catch (...) {
            // We got an exception - tell the user
            SysOut::errorln(SPL_CORE_EVALUATION_EXCEPTION(toString()), _location);
        }
    }

    // Can't do anything
    return this;
}

/*
 * AttributeExpression
 */
ostream& AttributeExpression::print(ostream& ostr, const Expression& root) const
{
    _lhs->print(ostr, root);
    ostr << '.';
    return _rhs->print(ostr, root);
}

AttributeExpression& AttributeExpression::clone() const
{
    return *new AttributeExpression(getType(), getLocation(), _lhs->clone(), _rhs->clone());
}

void AttributeExpression::generate(ostream& s,
                                   uint32_t depth,
                                   GenerationContext context,
                                   bool genTemplate) const
{
    _lhs->generate(s, depth, General, genTemplate);
    assert(_rhs->getKind() == Symbol);
    s << ".get_";
    _rhs->generate(s, depth, context);
    s << "()";
}

Expression* AttributeExpression::simplify(ExpressionEvaluator& eval)
{
    return BinaryExpression::simplify(eval);
}

/*
 * SubscriptExpression
 */
SubscriptExpression::SubscriptExpression(const RootTyp& type,
                                         const SourceLocation& l,
                                         Expression& lhs,
                                         bool isSlice,
                                         Expression* inlower,
                                         Expression* inupper)
  : Expression(Subscript, type, l)
  , _lhs(&lhs)
  , _lower(inlower)
  , _upper(inupper)
  , _isSlice(isSlice)
{
    MODEL("SubscriptExpression(" << (void*)this << ")");
    assert(isSlice || inlower);
}

SubscriptExpression::~SubscriptExpression()
{
    delete _lhs;
    delete _lower;
    delete _upper;
}

bool SubscriptExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const SubscriptExpression& sRhs = static_cast<const SubscriptExpression&>(rhs);

    if (_isSlice != sRhs._isSlice) {
        return false;
    }
    if (getType() != rhs.getType() || *_lhs != *sRhs._lhs) {
        return false;
    }
    if (_lower) {
        if (!sRhs._lower || *_lower != *sRhs._lower) {
            return false;
        }
    } else if (sRhs._lower) {
        return false;
    }

    if (_upper) {
        if (!sRhs._upper || *_upper != *sRhs._upper) {
            return false;
        }
    } else if (sRhs._upper) {
        return false;
    }
    return true;
}

bool SubscriptExpression::readsState() const
{
    if (_lhs->readsState() || (_lower && _lower->readsState()) ||
        (_upper && _upper->readsState())) {
        return true;
    }
    return false;
}

bool SubscriptExpression::hasSideEffects(void) const
{
    if (_lhs->hasSideEffects() || (_lower && _lower->hasSideEffects()) ||
        (_upper && _upper->hasSideEffects())) {
        return true;
    }
    return false;
}

void SubscriptExpression::generate(ostream& s,
                                   uint32_t depth,
                                   GenerationContext context,
                                   bool genTemplate) const
{
    MetaType metaType = _lhs->getType().getMetaType();
    if (!_isSlice) {
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _lhs->generate(s, depth, General, genTemplate);
        }
        if (context == AssignmentLHS && (metaType == MetaType::BMAP || metaType == MetaType::MAP)) {
            s << '[';
            if (genTemplate) {
                s << "{Lower}";
            } else {
                _lower->generate(s, depth, General, genTemplate);
            }
            s << ']';
        } else {
            s << ".at(";
            if (genTemplate) {
                s << "{Lower}";
            } else {
                _lower->generate(s, depth, General, genTemplate);
            }
            s << ')';
        }
        return;
    }

    if (!_lower && !_upper) {
        // the whole array
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _lhs->generate(s, depth, context);
        }
        return;
    }

    // We are a slice.   Figure out what to do
    string prefix;
    switch (metaType) {
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::BLOB:
            prefix = "SPL::Functions::Collections::";
            break;
        case MetaType::RSTRING:
        case MetaType::USTRING:
        case MetaType::BSTRING:
            prefix = "SPL::Functions::String::";
            break;
        default:
            assert(false);
            break;
    }
    if (_lower) {
        if (_upper) {
            s << prefix << "subSlice (";
        } else {
            s << prefix << "subSliceU (";
        }
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _lhs->generate(s, depth, General, genTemplate);
        }
        s << ", ";
        if (genTemplate) {
            s << "{Lower}";
        } else {
            _lower->generate(s, depth, General, genTemplate);
        }
        if (_upper) {
            s << ", ";
            if (genTemplate) {
                s << "{Upper}";
            } else {
                _upper->generate(s, depth, General, genTemplate);
            }
        }
        s << ')';
    } else {
        assert(_upper);
        s << prefix << "subSliceL (";
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _lhs->generate(s, depth, General, genTemplate);
        }
        s << ", ";
        if (genTemplate) {
            s << "{Upper}";
        } else {
            _upper->generate(s, depth, General, genTemplate);
        }
        s << ')';
    }
}

Expression* SubscriptExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    // simplify children
    _lhs = Expression::replaceLits(_lhs, lit, onlySTP);
    _lower = Expression::replaceLits(_lower, lit, onlySTP);
    _upper = Expression::replaceLits(_upper, lit, onlySTP);
    return this;
}

Expression* SubscriptExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children
    _lhs = Expression::simplify(_lhs, eval);
    if (_lower) {
        _lower = Expression::simplify(_lower, eval);
    }
    if (_upper) {
        _upper = Expression::simplify(_upper, eval);
    }

    // simplify myself
    if (_isSlice && _lower == NULL && _upper == NULL) {
        Expression* result = _lhs;
        _lhs = NULL;
        return result; // just grab the whole array
    }

    ExpressionValue a, sl, su;
    if (!_lhs->getExpressionValue(a) || _lhs->dontSimplify() || a.type() != MetaType::LIST) {
        return this; // can't do much with non-literal list
    }

    if (!_isSlice) {
        // Can we simplify the subscript?
        if (!_lower->getExpressionValue(sl)) {
            return this;
        }
        uint64_t i;
        if (!sl.getIntegerValue(i)) {
            return this;
        }
        // Is the value in range?
        if (!a.subscript(i)) {
            SysOut::errorln(SPL_CORE_EVALUATION_SUBSCRIPT_ERROR(_lower->toString(), toString()),
                            _location);
            return this;
        }
        return buildExpression(_type, _location, a);
    } else {
        // Doing a slice
        uint64_t l, u, *pl = NULL, *pu = NULL;
        if (_lower) {
            if (!_lower->getExpressionValue(sl) || !sl.getIntegerValue(l)) {
                return this;
            }
            pl = &l;
        }
        if (_upper) {
            if (!_upper->getExpressionValue(su) || !su.getIntegerValue(u)) {
                return this;
            }
            pu = &u;
        }
        // Is the value in range?
        if (!a.subscript(pl, pu)) {
            string s;
            if (_lower) {
                s += _lower->toString();
            }
            s += ':';
            if (_upper) {
                s += _upper->toString();
            }
            SysOut::errorln(SPL_CORE_EVALUATION_SUBSCRIPT_ERROR(s, toString()), _location);
            return this;
        }
        return buildExpression(_type, _location, a);
    }
}

ostream& SubscriptExpression::print(ostream& ostr, const Expression& root) const
{
    _lhs->print(ostr, root);
    ostr << '[';
    if (_isSlice) {
        if (_lower) {
            _lower->print(ostr, root);
        }
        ostr << ':';
        if (_upper) {
            _upper->print(ostr, root);
        }
    } else {
        _lower->print(ostr, root);
    }
    return ostr << ']';
}

SubscriptExpression& SubscriptExpression::clone() const
{
    Expression* l = _lower ? &_lower->clone() : NULL;
    Expression* u = _upper ? &_upper->clone() : NULL;
    return *new SubscriptExpression(getType(), getLocation(), _lhs->clone(), _isSlice, l, u);
}

// unwrap expression
UnwrapExpression::UnwrapExpression(const RootTyp& type, const SourceLocation& l, Expression& lhs)
  : Expression(Unwrap, type, l)
  , _lhs(&lhs)
{
    MODEL("UnwrapExpression(" << (void*)this << ", " << (void*)&lhs << ")\n");
}

UnwrapExpression::~UnwrapExpression()
{
    delete _lhs;
}

bool UnwrapExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const UnwrapExpression& unwrapRHS = static_cast<const UnwrapExpression&>(rhs);

    return getType() == rhs.getType() && *_lhs == *unwrapRHS._lhs;
}

bool UnwrapExpression::readsState(void) const
{
    if (_lhs->readsState()) {
        return true;
    }
    return false;
}

bool UnwrapExpression::hasSideEffects(void) const
{
    if (_lhs->hasSideEffects()) {
        return true;
    }
    return false;
}

void UnwrapExpression::generate(ostream& s,
                                uint32_t depth,
                                GenerationContext context,
                                bool genTemplate) const
{

    bool isOptional = _lhs->getType().isOptional();

    if (isOptional) {
        s << '(';
    }

    if (genTemplate) {
        s << "{Expression}";
    } else {
        _lhs->generate(s, depth, General, genTemplate);
    }

    if (
      isOptional) { // identity semantics, only call value() if it's an optional, otherwise we've already generated the expression
        s << ").value()";
    }
}

Expression* UnwrapExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    _lhs = Expression::replaceLits(_lhs, lit, onlySTP);
    return this;
}

Expression* UnwrapExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children

    _lhs = Expression::simplify(_lhs, eval);

    // todo: remove the unwrap and act as an identify function if argument is not an optional

    return this;
}

ostream& UnwrapExpression::print(ostream& ostr, const Expression& root) const
{
    if (this != &root) {
        ostr << '(';
    }
    _lhs->print(ostr, root);
    ostr << " ! ";
    if (this != &root) {
        ostr << ')';
    }
    return ostr;
}

UnwrapExpression& UnwrapExpression::clone() const
{
    return *new UnwrapExpression(getType(), getLocation(), _lhs->clone());
}

// UnwrapOrElse expression
UnwrapOrElseExpression::UnwrapOrElseExpression(const RootTyp& type,
                                               const SourceLocation& l,
                                               Expression& lhs,
                                               Expression& rhs)
  : Expression(UnwrapOrElse, type, l)
  , _lhs(&lhs)
  , _rhs(&rhs)
{
    MODEL("UnwrapExpression(" << (void*)this << ", " << (void*)&lhs << ", " << (void*)&rhs
                              << ")\n");
}

UnwrapOrElseExpression::~UnwrapOrElseExpression()
{
    delete _lhs;
    delete _rhs;
}

bool UnwrapOrElseExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const UnwrapOrElseExpression& unwrapRHS = static_cast<const UnwrapOrElseExpression&>(rhs);

    return getType() == rhs.getType() && *_lhs == *unwrapRHS._lhs && *_rhs == *unwrapRHS._rhs;
}

bool UnwrapOrElseExpression::readsState(void) const
{
    if (_lhs->readsState() || _rhs->readsState()) {
        return true;
    }
    return false;
}

bool UnwrapOrElseExpression::hasSideEffects(void) const
{
    if (_lhs->hasSideEffects() || _rhs->hasSideEffects()) {
        return true;
    }
    return false;
}

void UnwrapOrElseExpression::generate(ostream& s,
                                      uint32_t depth,
                                      GenerationContext context,
                                      bool genTemplate) const
{

    bool isOptional = _lhs->getType().isOptional();

    if (isOptional) {
        s << "((";
        if (genTemplate) {
            s << "{Lhs}";
        } else {
            _lhs->generate(s, depth, General, genTemplate);
        }
        s << ").isPresent() ? (";

        if (genTemplate) {
            s << "{Lhs}";
        } else {
            _lhs->generate(s, depth, General, genTemplate);
        }
        s << ").value() : (";
        if (genTemplate) {
            s << "{Rhs}";
        } else {
            _rhs->generate(s, depth, General, genTemplate);
        }
        s << "))";
    }

    else {

        _rhs->generate(s, depth, General, genTemplate); // identity semantics
    }
}

Expression* UnwrapOrElseExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    _lhs = Expression::replaceLits(_lhs, lit, onlySTP);
    _rhs = Expression::replaceLits(_rhs, lit, onlySTP);
    return this;
}

Expression* UnwrapOrElseExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children

    _lhs = Expression::simplify(_lhs, eval);
    _rhs = Expression::simplify(_rhs, eval);

    // todo: remove the unwrap and act as an identify function if argument is not an optional

    return this;
}

ostream& UnwrapOrElseExpression::print(ostream& ostr, const Expression& root) const
{
    if (this != &root) {
        ostr << '(';
    }
    _lhs->print(ostr, root);
    ostr << " ?: ";
    _rhs->print(ostr, root);
    if (this != &root) {
        ostr << ')';
    }
    return ostr;
}

UnwrapOrElseExpression& UnwrapOrElseExpression::clone() const
{
    return *new UnwrapOrElseExpression(getType(), getLocation(), _lhs->clone(), _rhs->clone());
}

IsPresentExpression::IsPresentExpression(const RootTyp& type,
                                         const SourceLocation& l,
                                         Expression& lhs)
  : Expression(IsPresent, type, l)
  , _lhs(&lhs)
{
    MODEL("IsPresentExpression(" << (void*)this << ", " << (void*)&lhs << ")\n");
}

IsPresentExpression::~IsPresentExpression()
{
    delete _lhs;
}

bool IsPresentExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const IsPresentExpression& isPresentRHS = static_cast<const IsPresentExpression&>(rhs);

    return getType() == rhs.getType() && *_lhs == *isPresentRHS._lhs;
}

bool IsPresentExpression::readsState(void) const
{
    if (_lhs->readsState()) {
        return true;
    }
    return false;
}

bool IsPresentExpression::hasSideEffects(void) const
{
    if (_lhs->hasSideEffects()) {
        return true;
    }
    return false;
}

void IsPresentExpression::generate(ostream& s,
                                   uint32_t depth,
                                   GenerationContext context,
                                   bool genTemplate) const
{

    bool isOptional = _lhs->getType().isOptional();

    if (isOptional) {
        s << "SPL::boolean((";
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _lhs->generate(s, depth, General, genTemplate);
        }
        s << ").isPresent())";
    }

    else {

        s << "SPL::boolean(true)"; // if it's not an optional, it's always present
    }
}

Expression* IsPresentExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    _lhs = Expression::replaceLits(_lhs, lit, onlySTP);
    return this;
}

Expression* IsPresentExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children

    _lhs = Expression::simplify(_lhs, eval);

    // todo: remove the unwrap and act as an identify function if argument is not an optional

    return this;
}

ostream& IsPresentExpression::print(ostream& ostr, const Expression& root) const
{
    if (this != &root) {
        ostr << '(';
    }
    _lhs->print(ostr, root);
    ostr << " ? ";
    if (this != &root) {
        ostr << ')';
    }
    return ostr;
}

IsPresentExpression& IsPresentExpression::clone() const
{
    return *new IsPresentExpression(getType(), getLocation(), _lhs->clone());
}

/*
 * Conditional Expression
 */
ConditionalExpression::ConditionalExpression(const RootTyp& type,
                                             const SourceLocation& l,
                                             Expression& condition,
                                             Expression& lhs,
                                             Expression& rhs)
  : BinaryExpression(Conditional, type, l, conditional, lhs, rhs)
  , _cond(&condition)
{
    MODEL("ConditionalExpression(" << (void*)this << ", " << (void*)&condition << ", "
                                   << (void*)&lhs << ", " << (void*)&rhs << ")\n");
}

ConditionalExpression::~ConditionalExpression()
{
    delete _cond;
}

bool ConditionalExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const ConditionalExpression& condRhs = static_cast<const ConditionalExpression&>(rhs);

    return _op == condRhs._op && getType() == rhs.getType() && *_cond == *condRhs._cond &&
           *_lhs == *condRhs._lhs && *_rhs == *condRhs._rhs;
}

bool ConditionalExpression::readsState(void) const
{
    if (_cond->readsState() || this->BinaryExpression::readsState()) {
        return true;
    }
    return false;
}

bool ConditionalExpression::hasSideEffects(void) const
{
    if (_cond->hasSideEffects() || this->BinaryExpression::hasSideEffects()) {
        return true;
    }
    return false;
}

void ConditionalExpression::generate(ostream& s,
                                     uint32_t depth,
                                     GenerationContext context,
                                     bool genTemplate) const
{
    s << '(';
    if (genTemplate) {
        s << "{Condition}";
    } else {
        _cond->generate(s, depth, General, genTemplate);
    }
    s << " ? ";

    // Special case:  one side is boolean with ! and the other isn't.
    // Need to generate casts in this case to force things
    bool forceBoolean = _lhs->getType().getMetaType() == MetaType::BOOLEAN;
    if (forceBoolean) {
        s << "SPL::boolean(";
    }
    if (genTemplate) {
        s << "{Lhs}";
    } else {
        _lhs->generate(s, depth, General, genTemplate);
    }
    if (forceBoolean) {
        s << ')';
    }
    s << " : ";
    if (forceBoolean) {
        s << "SPL::boolean(";
    }
    if (genTemplate) {
        s << "{Rhs}";
    } else {
        _rhs->generate(s, depth, General, genTemplate);
    }
    if (forceBoolean) {
        s << ')';
    }
    s << ')';
}

Expression* ConditionalExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    _cond = Expression::replaceLits(_cond, lit, onlySTP);
    _rhs = Expression::replaceLits(_rhs, lit, onlySTP);
    _lhs = Expression::replaceLits(_lhs, lit, onlySTP);
    return this;
}

Expression* ConditionalExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children
    _cond = Expression::simplify(_cond, eval);
    _lhs = Expression::simplify(_lhs, eval);
    _rhs = Expression::simplify(_rhs, eval);

    // Is the condition constant?
    const Literal* l = _cond->getLiteral();
    if (!l || _cond->dontSimplify()) {
        return this; // not a literal
    }

    // We have a literal.  Is it a useful one?
    if (l->getKind() == Literal::Primitive && l->getType().getMetaType() == MetaType::BOOLEAN) {
        Expression* result;
        if (l->primitive().get_boolean()) {
            result = _lhs; // true;
            _lhs = NULL;
        } else {
            result = _rhs; // true
            _rhs = NULL;
        }
        return result;
    }
    return this;
}

ostream& ConditionalExpression::print(ostream& ostr, const Expression& root) const
{
    if (this != &root) {
        ostr << '(';
    }
    _cond->print(ostr, root);
    ostr << " ? ";
    _lhs->print(ostr, root);
    ostr << " : ";
    _rhs->print(ostr, root);
    if (this != &root) {
        ostr << ')';
    }
    return ostr;
}

ConditionalExpression& ConditionalExpression::clone() const
{
    return *new ConditionalExpression(getType(), getLocation(), _cond->clone(), _lhs->clone(),
                                      _rhs->clone());
}

/*
 * Unary Expression
 */
UnaryExpression::UnaryExpression(const RootTyp& type,
                                 const SourceLocation& l,
                                 Op op,
                                 Expression& expn)
  : Expression(Unary, type, l)
  , _expn(&expn)
  , _op(op)
{
    MODEL("UnaryExpression(" << (void*)this << ", " << (int)op << ", " << (void*)&expn << ")\n");
}

UnaryExpression::UnaryExpression(Kind k,
                                 const RootTyp& type,
                                 const SourceLocation& l,
                                 Op op,
                                 Expression& expn)
  : Expression(k, type, l)
  , _expn(&expn)
  , _op(op)
{}

UnaryExpression::~UnaryExpression()
{
    delete _expn;
}

bool UnaryExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const UnaryExpression& uRhs = static_cast<const UnaryExpression&>(rhs);

    return _op == uRhs._op && getType() == rhs.getType() && *_expn == *uRhs._expn;
}

bool UnaryExpression::readsState(void) const
{
    if (_expn->readsState()) {
        return true;
    }
    return false;
}

bool UnaryExpression::hasSideEffects(void) const
{
    if (_expn->hasSideEffects()) {
        return true;
    }

    return false;
}

void UnaryExpression::generate(ostream& s,
                               uint32_t depth,
                               GenerationContext context,
                               bool genTemplate) const
{
    s << opDescription[_op] << '(';
    if (genTemplate) {
        s << "{Expression}";
    } else {
        _expn->generate(s, depth, General, genTemplate);
    }
    s << ')';
}

Expression* UnaryExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    _expn = Expression::replaceLits(_expn, lit, onlySTP);
    return this;
}

Expression* UnaryExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children
    _expn = Expression::simplify(_expn, eval);

    // simplification
    if (_op == plus) {
        Expression* e = _expn;
        _expn = NULL;
        return e; // + is no-op
    }

    ExpressionValue e;
    if (!_expn->getExpressionValue(e) || _expn->dontSimplify()) {
        return this; // nothing can be done
    }

    // We have a literal.   Let us see if we can simplify it
    if (_op == bang || _op == tilde) {
        ExpressionValue result;
        if (eval.complement(e, result)) {
            return buildExpression(_type, _location, result);
        }
    } else if (_op == unaryMinus) {
        ExpressionValue result;
        if (eval.negate(e, result)) {
            return buildExpression(_type, _location, result);
        }
    }

    // Can't do anything
    return this;
}

ostream& UnaryExpression::print(ostream& ostr, const Expression& root) const
{
    ostr << opDescription[_op] << '(';
    _expn->print(ostr, root);
    return ostr << ')';
}

UnaryExpression& UnaryExpression::clone() const
{
    return *new UnaryExpression(getType(), getLocation(), _op, _expn->clone());
}

/*
 * Prefix Expression
 */
PrefixExpression::PrefixExpression(const RootTyp& type,
                                   const SourceLocation& l,
                                   Op op,
                                   Expression& expn)
  : UnaryExpression(Prefix, type, l, op, expn)
{
    MODEL("PrefixExpression(" << (void*)this << ", " << (int)op << ", " << (void*)&expn << ")\n");
}

ostream& PrefixExpression::print(ostream& ostr, const Expression& root) const
{
    ostr << opDescription[_op];
    _expn->print(ostr, root);
    return ostr;
}

PrefixExpression& PrefixExpression::clone() const
{
    return *new PrefixExpression(getType(), getLocation(), _op, _expn->clone());
}

void PrefixExpression::generate(ostream& s,
                                uint32_t depth,
                                GenerationContext context,
                                bool genTemplate) const
{
    switch (_type.getMetaType()) {
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
            if (_op == plusPlus || _op == minusMinus) {
                // C++ complex doesn't do increment/decrement
                TypeGenerator const& tg = TypeGenerator::instance();
                s << "({ " << tg.getCppType(_expn->getType()) << "& temp$prefix = ";
                if (genTemplate) {
                    s << "{Expression}";
                } else {
                    _expn->generate(s, depth, General, genTemplate);
                }
                s << "; temp$prefix " << ((_op == plusPlus) ? '+' : '-') << "= "
                  << tg.getCppType(_expn->getType()) << "(1, 0); })";
                return;
            }
            break;

        case MetaType::BOOLEAN:
        case MetaType::INT8:
        case MetaType::UINT8:
        case MetaType::INT16:
        case MetaType::UINT16:
        case MetaType::INT32:
        case MetaType::UINT32:
        case MetaType::INT64:
        case MetaType::UINT64:
            if (_op == tilde || _op == unaryMinus || _op == bang) {
                // Need to ensure that the result has the right type
                TypeGenerator const& tg = TypeGenerator::instance();
                s << tg.getCppType(_expn->getType()) << '(' << opDescription[_op];
                if (genTemplate) {
                    s << "{Expression}";
                } else {
                    _expn->generate(s, depth, General, genTemplate);
                }
                s << ')';
                return;
            }
            break;

        default:
            break;
    }

    // Easy case
    s << opDescription[_op];
    if (genTemplate) {
        s << "{Expression}";
    } else {
        _expn->generate(s, depth, General, genTemplate);
    }
}

/*
 * Postfix Expression
 */
PostfixExpression::PostfixExpression(const RootTyp& type,
                                     const SourceLocation& l,
                                     Op op,
                                     Expression& expn)
  : UnaryExpression(Postfix, type, l, op, expn)
{
    MODEL("PostfixExpression(" << (void*)this << ", " << (int)op << ", " << (void*)&expn << ")\n");
}

ostream& PostfixExpression::print(ostream& ostr, const Expression& root) const
{
    _expn->print(ostr, root);
    return ostr << opDescription[_op];
}

void PostfixExpression::generate(ostream& s,
                                 uint32_t depth,
                                 GenerationContext context,
                                 bool genTemplate) const
{
    switch (_type.getMetaType()) {
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
            if (_op == plusPlus || _op == minusMinus) {
                // C++ complex doesn't do increment/decrement
                TypeGenerator const& tg = TypeGenerator::instance();
                const string tName = tg.getCppType(_expn->getType());
                s << "({ " << tName << "& temp$postfix = ";
                if (genTemplate) {
                    s << "{Expression}";
                } else {
                    _expn->generate(s, depth, General, genTemplate);
                }
                s << "; " << tName << " temp$oldVal = temp$postfix; temp$postfix "
                  << ((_op == plusPlus) ? '+' : '-') << "= " << tName << "(1, 0); temp$oldVal;})";
                break;
            }
        // FALLTHROUGH
        default:
            if (genTemplate) {
                s << "{Expression}";
            } else {
                _expn->generate(s, depth, General, genTemplate);
            }
            s << opDescription[_op];
            break;
    }
}

Expression* PostfixExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children
    _expn = Expression::simplify(_expn, eval);

    // perform the operation on myself
    return this;
}

PostfixExpression& PostfixExpression::clone() const
{
    return *new PostfixExpression(getType(), getLocation(), _op, _expn->clone());
}

/*
 * Cast Expression
 */
CastExpression::CastExpression(const RootTyp& type, const SourceLocation& l, Expression& expn)
  : UnaryExpression(Cast, type, l, cast, expn)
{
    MODEL("CastExpression(" << (void*)this << ", " << (void*)&expn << ")\n");
}

static bool stringType(MetaType t)
{
    return t == MetaType::RSTRING || t == MetaType::BSTRING || t == MetaType::USTRING;
}

void CastExpression::generate(ostream& s,
                              uint32_t depth,
                              GenerationContext context,
                              bool genTemplate) const
{
    TypeGenerator const& tg = TypeGenerator::instance();
    MetaType toType = _type.getMetaType();
    MetaType fromType = _expn->getType().getMetaType();

    // Cast to XML type
    if (toType == MetaType::XML) {
        if (fromType == MetaType::XML) {
            s << tg.getCppType(_type) << "(";
            if (genTemplate) {
                s << "{Expression}";
            } else {
                _expn->generate(s, depth, General, genTemplate);
            }
            s << ")";
            return;
        }
        if (MetaType::RSTRING == fromType) {
            s << tg.getCppType(_type) << "(";
            s << "(SPL::rstring)";
            if (genTemplate) {
                s << "{Expression}";
            } else {
                _expn->generate(s, depth, General, genTemplate);
            }
            s << ")";
        } else {
            assert(MetaType::TUPLE == fromType);
            s << tg.getCppType(_type) << "(";
            s << "(const SPL::Tuple &)";
            if (genTemplate) {
                s << "{Expression}";
            } else {
                _expn->generate(s, depth, General, genTemplate);
            }
            s << ")";
        }
        return;
    }

    // Cast from XML type
    if (fromType == MetaType::XML) {

        ostringstream cst;

        switch (toType) {
            case MetaType::RSTRING:
                cst << "::SPL::spl_cast<SPL::rstring, SPL::xml>::cast(";
                break;

            case MetaType::TUPLE:
                cst << "::SPL::spl_cast<";
                cst << tg.getCppType(getType());
                cst << ", SPL::xml>::cast(";
                break;

            case MetaType::OPTIONAL:
                if (_type.isOptionalOfSameType(_expn->getType())) {
                    cst << tg.getCppType(_type) << "(";
                }
                break;

            default:
                assert(!"Invalid to type");
        }

        s << cst.str();
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _expn->generate(s, depth, General, genTemplate);
        }
        s << ")";
        return;
    }

    // Might be a wide tuple -> narrow tuple cast
    if (toType == MetaType::TUPLE && _expn->getType().getMetaType() == MetaType::TUPLE) {
        // Special logic for empty tuple literal casts
        const Literal* lit = _expn->getLiteral();
        if (lit && lit->emptyTupleLiteral()) {
            s << tg.getCppType(getType()) << "().clear()";
            return;
        }
        s << tg.getCppType(getType()) << '(';
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _expn->generate(s, depth, General, genTemplate);
        }
        s << ", false)";
        return;
    }

    // Casts from string?
    if ((fromType == MetaType::RSTRING || fromType == MetaType::BSTRING) &&
        toType == MetaType::BSTRING) {
        s << tg.getCppType(_type) << '(';
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _expn->generate(s, depth, General, genTemplate);
        }
        s << ')';
        return;
    }

    // Cast to/from Enum
    if (fromType == MetaType::ENUM && toType == MetaType::OPTIONAL &&
        _type.as<OptionalTyp>().getUnderlyingType().getMetaType() == MetaType::ENUM) {
        s << tg.getCppType(_type) << '(';
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _expn->generate(s, depth, General, genTemplate);
        }
        s << ")";
        return;
    } else if (fromType == MetaType::ENUM && !stringType(toType)) {
        s << tg.getCppType(_type) << '(';
        if (genTemplate) {
            s << "{Expression}";
        } else {
            _expn->generate(s, depth, General, genTemplate);
        }
        s << ".getIndex())";
        return;
    } else if (toType == MetaType::ENUM) {
        s << tg.getCppType(_type) << '(';
        if (fromType == MetaType::BSTRING || fromType == MetaType::USTRING) {
            s << "::SPL::spl_cast<SPL::rstring," << tg.getCppType(_expn->getType()) << " >::cast(";
            if (genTemplate) {
                s << "{Expression}";
            } else {
                _expn->generate(s, depth, General, genTemplate);
            }
            s << ')';
        } else {
            if (genTemplate) {
                s << "{Expression}";
            } else {
                _expn->generate(s, depth, General, genTemplate);
            }
        }
        s << ')';
        return;
    }

    // All other casts
    s << "::SPL::spl_cast<" << tg.getCppType(_type) << ", " << tg.getCppType(_expn->getType())
      << " >::cast(";
    if (genTemplate) {
        s << "{Expression}";
    } else {
        _expn->generate(s, depth, General, genTemplate);
    }
    s << ')';
}

static uint32_t computeNumLits(uint32_t nLits, MetaType t, const RootTyp& type)
{
    uint32_t n;
    switch (t) {
        case MetaType::BLIST: {
            const BListTyp& bt = static_cast<const BListTyp&>(type);
            n = bt.getBoundSize();
        } break;
        case MetaType::BSET: {
            const BSetTyp& bt = static_cast<const BSetTyp&>(type);
            n = bt.getBoundSize();
        } break;
        case MetaType::BMAP: {
            const BMapTyp& bt = static_cast<const BMapTyp&>(type);
            n = bt.getBoundSize();
        } break;
        default:
            return nLits;
    }
    if (nLits < n) {
        return nLits;
    }
    return n;
}

static Expression* recreateLiteral(MetaType t,
                                   const RootTyp& type,
                                   const Literal& lit,
                                   const SourceLocation& loc)
{
    // Recopy the lit with a new type
    auto_ptr<Literal> l(new Literal(type, NULL));
    uint32_t numLits = computeNumLits(lit.numElements(), t, type);
    for (uint32_t i = 0; i < numLits; i++) {
        switch (t) {
            case MetaType::LIST:
            case MetaType::BLIST:
                l->addListElement(lit.listSetElement(i).clone());
                break;
            case MetaType::SET:
            case MetaType::BSET:
                l->addSetElement(lit.listSetElement(i).clone());
                break;
            case MetaType::MAP:
            case MetaType::BMAP: {
                const Literal::MapLiteral& ml = lit.mapElement(i);
                l->addMapElement(ml._key->clone(), ml._value->clone());
            } break;
            default:
                assert(false);
        }
    }
    return new LiteralExpression(type, loc, *l.release());
}

Expression* CastExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children
    _expn = Expression::simplify(_expn, eval);

    if (_expn->dontSimplify()) {
        return this; // We can't simplify any more... done in castExpr in FrontEnd
    }

    Literal* lit = _expn->getLiteral();
    // Compare the types.  Nothing to do?
    if (&_type == &_expn->getType()) {
        // Keep empty tuple literal casts
        if (lit && lit->emptyTupleLiteral()) {
            return this;
        }

        Expression* e = _expn;
        _expn = NULL;
        return e;
    }

    // If this is a simple cast of a literal list->blist, set->bset, map->bmap, let it through
    if (lit) {
        // Check if we are consistent
        MetaType t = _type.getMetaType();
        const RootTyp& eType = _expn->getType();
        MetaType e = eType.getMetaType();
        // Is this a cast of an empty lit?
        if (lit->getKind() != Literal::Primitive && lit->numElements() == 0 &&
            t != MetaType::BLOB) {
            return recreateLiteral(t, _type, *lit, _expn->getLocation());
        }

        // Check for other common ones
        if (t == MetaType::BLIST && e == MetaType::LIST) {
            // converting a list to a blist.  Is it okay?
            const BListTyp& tt = static_cast<const BListTyp&>(_type);
            const ListTyp& et = static_cast<const ListTyp&>(eType);
            if (&tt.getElementType() == &et.getElementType()) {
                return recreateLiteral(t, _type, *lit, _expn->getLocation());
            }
        } else if (t == MetaType::BSET && e == MetaType::SET) {
            // converting a set to a bset.  Is it okay?
            const BSetTyp& tt = static_cast<const BSetTyp&>(_type);
            const SetTyp& et = static_cast<const SetTyp&>(eType);
            if (&tt.getElementType() == &et.getElementType()) {
                return recreateLiteral(t, _type, *lit, _expn->getLocation());
            }
        } else if (t == MetaType::BMAP && e == MetaType::MAP) {
            // converting a map to a bmap.  Is it okay?
            const BMapTyp& tt = static_cast<const BMapTyp&>(_type);
            const MapTyp& et = static_cast<const MapTyp&>(eType);
            if (&tt.getKeyType() == &et.getKeyType()) {
                return recreateLiteral(t, _type, *lit, _expn->getLocation());
            }
        } else if (t == MetaType::BSTRING && (e == MetaType::RSTRING || e == MetaType::BSTRING)) {

            // recoskie - fix for RTC41407
            // It is possible a cast for a literal will appear in a place other than an
            // initializer, which means the cast cannot be optimized out.
            return this; // do nothing
                         // end recoskie

        } else if (t == MetaType::RSTRING && e == MetaType::BSTRING) {
            return new LiteralExpression(_type, _expn->getLocation(), lit->clone());
        } else if (t == MetaType::XML && e == MetaType::RSTRING) {
            return new LiteralExpression(_type, _expn->getLocation(), lit->clone());
        } else if (t == MetaType::RSTRING) {
            // Let's try converting to a string, and see what happens
            if (lit->getKind() == Literal::Primitive) {
                if (e == MetaType::ENUM) {
                    const std::string* v = lit->enumValueName();
                    if (v) {
                        SPL::rstring result = *v;
                        lit = new Literal(_type, new ExpressionValue(result));
                        return new LiteralExpression(_type, _expn->getLocation(), *lit);
                    }
                }
                const ExpressionValue& ev = lit->primitive();
                SPL::rstring result;
                if (ev.toString(result, _expn->getLocation())) {
                    lit = new Literal(_type, new ExpressionValue(result));
                    return new LiteralExpression(_type, _expn->getLocation(), *lit);
                }
            }
        } else if (e == MetaType::RSTRING) {
            // Let's try converting from a string, and see what happens
            assert(lit->getKind() == Literal::Primitive);
            // Need a new one...
            ExpressionValue ev(t);
            if (ev.type() != MetaType::INVALID) {
                if (ev.fromString(lit->primitive().get_rstring(), _expn->getLocation())) {
                    lit = new Literal(_type, new ExpressionValue(ev));
                    return new LiteralExpression(_type, _expn->getLocation(), *lit);
                }
            }
        }
    }

    // perform the operation on myself
    ExpressionValue e;
    if (!_expn->getExpressionValue(e)) {
        return this; // nothing can be done
    }

    if (e.cast(_type)) {
        return buildExpression(_type, _location, e);
    }

    // unsupported cast
    return this;
}

ostream& CastExpression::print(ostream& ostr, const Expression& root) const
{
    ostr << '(' << &getType() << ")(";
    _expn->print(ostr, root);
    return ostr << ')';
}

CastExpression& CastExpression::clone() const
{
    return *new CastExpression(getType(), getLocation(), _expn->clone());
}

/*
 * Call Expression
 */
CallExpression::CallExpression(const RootTyp& type,
                               const SourceLocation& l,
                               const FunctionHeadSym& fcn,
                               const vector<Expression*>& args,
                               const std::string& compositeName)
  : Expression(Call, type, l)
  , _functionName(fcn.name())
  , _fcn(&fcn)
  , _args(args)
  , _compositeName(compositeName)
  , _submitNeedingCopy(false)
{
    MODEL("CallExpr(" << (void*)this << ", " << _functionName << ")\n");
}

CallExpression::CallExpression(const RootTyp& type,
                               const SourceLocation& l,
                               const std::string& functionName,
                               const vector<Expression*>& args,
                               const std::string& compositeName)
  : Expression(Call, type, l)
  , _functionName(functionName)
  , _fcn(NULL)
  , _args(args)
  , _compositeName(compositeName)
  , _submitNeedingCopy(false)
{
    MODEL("CallExpression(" << (void*)this << ", " << _functionName << ", " << compositeName
                            << ")\n");
}

CallExpression::~CallExpression()
{
    for (uint32_t i = 0; i < _args.size(); i++) {
        delete _args[i];
    }
}

static bool isCurrentPunct(const Expression& e)
{
    if (e.getKind() != Expression::Call) {
        return false;
    }
    const CallExpression& ce = e.as<CallExpression>();
    const FunctionHeadSym& fcn = ce.getFcn();
    if (!fcn.isIntrinsic()) {
        return false;
    }
    return fcn.name() == "currentPunct";
}

void CallExpression::generate(ostream& s,
                              uint32_t depth,
                              GenerationContext context,
                              bool genTemplate) const
{
    assert(NULL != _fcn);
    string fullName;
    if (_fcn->hasCppNamespace()) {
        fullName = _fcn->getCppNamespace() + "::";
    } else {
        string nsName = _fcn->nameSpace();
        if (!SymbolTableEntry::isDefaultNameSpace(nsName)) {
            vector<string> ns;
            Distillery::tokenize(nsName, ns, ".", false);
            for (uint32_t i = 0; i < ns.size(); i++) {
                fullName += ns[i];
                fullName += "::";
            }
        }
    }

    // Special handling of certain spl builtins
    if (_fcn->isIntrinsic()) {
        const string& fName = _fcn->name();
        if (fName == "submit") {
            // Handle enum case, as punctuation is simpler
            if (_args[0]->getType().getMetaType() == MetaType::ENUM) {
                assert(_args.size() == 2);
                s << "submit (";
                if (isCurrentPunct(*_args[0])) {
                    s << "punct";
                } else {
                    s << "static_cast<SPL::Punctuation::Value>(";
                    if (genTemplate) {
                        s << "{arg:0}";
                    } else {
                        _args[0]->generate(s, depth, General, genTemplate);
                    }
                    s << ".getIndex()";
                    s << "+1)";
                }
                // Now the port number
                s << ", ";
                if (genTemplate) {
                    s << "{arg:1}";
                } else {
                    _args[1]->generate(s, depth, General, genTemplate);
                }
                s << ')';
                return;
            }
            // Handle timestamp, submitting watermark punctuation
            if (_args[0]->getType().getMetaType() == MetaType::TIMESTAMP) {
                s << "eventTimeContext_.submit(";
                if (genTemplate) {
                    s << "{arg:0}";
                } else {
                    _args[0]->generate(s, depth, General, genTemplate);
                }
                if (_args.size() == 2) {
                    // Now the port number
                    s << ", ";
                    if (genTemplate) {
                        s << "{arg:1}";
                    } else {
                        _args[1]->generate(s, depth, General, genTemplate);
                    }
                }
                s << ")";
                return;
            }

            // Tuple case
            if (_submitNeedingCopy) {
                assert(_args.size() == 2);
                // First create a temporary copy
                // Need to use 'do {...} while (0)' to handle single statement 'if'
                s << "do { " << TypeGenerator::instance().getCppType(_args[0]->getType())
                  << " temp = ";
                if (genTemplate) {
                    s << "{arg:0}";
                } else {
                    _args[0]->generate(s, depth, General, genTemplate);
                }
                s << "; submit (temp";
            } else {
                s << "submit (";
                if (genTemplate) {
                    s << "{arg:0}";
                } else {
                    _args[0]->generate(s, depth, General, genTemplate);
                }
            }

            // Now the port number
            s << ", ";
            if (genTemplate) {
                s << "{arg:1}";
            } else {
                _args[1]->generate(s, depth, General, genTemplate);
            }
            s << ')';
            if (_submitNeedingCopy) {
                s << "; } while(0)";
            }
            return;
        } else if (fName == "getToolkitDirectory") {
            assert(_args.size() == 1);
            s << "SPL::Functions::Utility::getToolkitDirectory(";
            if (genTemplate) {
                s << "{arg:0}";
            } else {
                _args[0]->generate(s, depth, General, genTemplate);
            }
            s << ")";
            return;
        } else if (fName == "currentPunct") {
            assert(_args.size() == 0);
            // Hardcoded enum type
            s << "SPL::BeJyrNIo3NArPzEvJL_1dNLMpOLTI0dMvMS8yBcACkegr2(punct-1)";
            return;
        } else if (fName == "currentWatermark") {
            s << "wm";
            return;
#if 0
        } else if (fName == "getEventTime") {
            assert(_args.size() == 1);
            s << "EventTime< " << TypeGenerator::instance().getCppType(_args[0]->getType()) << " >().get(";
            if (genTemplate) {
                s << "{arg:0}";
            } else {
                _args[0]->generate(s, depth, General, genTemplate);
            }
            s << ')';
            return;
#endif
        }
    }

    if (_fcn->hasCppFunctionName()) {
        fullName += _fcn->getCppFunctionName();
    } else {
        fullName += _fcn->name();
    }
    if (genTemplate) {
        s << "{fcnName?::" << fullName << "}(";
    } else {
        s << "::" << fullName << '(';
    }
    int numArgs = _args.size();
    const FunctionHeadSym::Formals& formals = _fcn->formals();
    for (int i = 0; i < numArgs; i++) {
        if (_args[i]->getType().getMetaType() == MetaType::BOOLEAN) {
            // add an extra cast to ensure function overload resolution
            // but only add it we aren't passing to a mutable parameter
            assert(formals[i]);
            if (!formals[i]->isMutable()) {
                s << "(SPL::boolean)";
            }
        }
        if (genTemplate) {
            s << "{arg:" << i << '}';
        } else {
            _args[i]->generate(s, depth, General, genTemplate);
        }
        if (i != numArgs - 1) {
            s << ", ";
        }
    }
    s << ')';
}

Expression* CallExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    assert(NULL != _fcn);
    // getSubmissionTimeValue and getSubmissionTimeListValue are specially handled
    if (_fcn->isIntrinsic()) {
        const string& fName = _fcn->name();
        if (fName == "getSubmissionTimeValue") {
            const Literal* l = _args[0]->getLiteral();
            if (!l || l->getKind() != Literal::Primitive ||
                l->getType().getMetaType() != MetaType::RSTRING) {
                SysOut::errorln(SPL_CORE_GET_ARGUMENT_NEEDS_LITERAL("getSubmissionTimeValue",
                                                                    _args[0]->toString()),
                                _args[0]->getLocation());
            }
            if (_args.size() == 1) {
                // no default value
                SPL::rstring val; // empty string
                ExpressionValue* v = new ExpressionValue(val);
                uint32_t litIndex;
                uint32_t idIndex =
                  lit.addArgument(*new Literal(_type, v), _args[0]->getLocation(), "named",
                                  _compositeName, l->primitive().get_rstring(), true, litIndex);
                return new LiteralSymbolExpression(_type, _location, idIndex, litIndex);
            } else {
                assert(_args.size() == 2);
                const Literal* l2 = _args[1]->getLiteral();
                if (!l2 || l2->getKind() != Literal::Primitive ||
                    l2->getType().getMetaType() != MetaType::RSTRING) {
                    SysOut::errorln(SPL_CORE_GET_ARGUMENT_NEEDS_LITERAL("getSubmissionTimeValue",
                                                                        _args[1]->toString()),
                                    _args[1]->getLocation());
                }
                uint32_t litIndex;
                uint32_t idIndex =
                  lit.addArgument(l2->clone(), _args[0]->getLocation(), "named", _compositeName,
                                  l->primitive().get_rstring(), false, litIndex);
                return new LiteralSymbolExpression(_type, _location, idIndex, litIndex);
            }
        } else if (fName == "getSubmissionTimeListValue") {
            const Literal* l = _args[0]->getLiteral();
            if (!l || l->getKind() != Literal::Primitive ||
                l->getType().getMetaType() != MetaType::RSTRING) {
                SysOut::errorln(SPL_CORE_GET_ARGUMENT_NEEDS_LITERAL("getSubmissionTimeListValue",
                                                                    _args[0]->toString()),
                                _args[0]->getLocation());
            }
            if (_args.size() == 1) {
                // no default value
                SPL::rstring val; // empty string
                ExpressionValue* v = new ExpressionValue(val);
                uint32_t litIndex;
                uint32_t idIndex =
                  lit.addArgument(*new Literal(_type, v), _args[0]->getLocation(), "namedList",
                                  _compositeName, l->primitive().get_rstring(), true, litIndex);
                return new LiteralSymbolExpression(_type, _location, idIndex, litIndex);
            } else {
                assert(_args.size() == 2);
                const Literal* l2 = _args[1]->getLiteral();
                if (!l2 || l2->getKind() != Literal::List ||
                    l2->getType().as<ListTyp>().getElementType().getMetaType() !=
                      MetaType::RSTRING) {
                    SysOut::errorln(SPL_CORE_GET_ARGUMENT_NEEDS_LITERAL(
                                      "getSubmissionTimeListValue", _args[1]->toString()),
                                    _args[1]->getLocation());
                }
                uint32_t litIndex;
                uint32_t idIndex =
                  lit.addArgument(l2->clone(), _args[0]->getLocation(), "namedList", _compositeName,
                                  l->primitive().get_rstring(), false, litIndex);
                return new LiteralSymbolExpression(_type, _location, idIndex, litIndex);
            }
        }
    }

    if (_fcn->isNativeFunction()) {
        // To optimize for loops, don't simplify range
        if (_fcn->nameSpace() == "spl.collection" && _fcn->name() == "range") {
            onlySTP = true;
        }
        // To optimize for loops don't simplify keys
        else if (_fcn->nameSpace() == "spl.collection" && _fcn->name() == "keys") {
            onlySTP = true;
        } else if (_fcn->nameSpace() == "spl.utility" && _fcn->name() == "getMainCompositeName") {
            // We need to replace this with the string name of the main composite
            CompilerConfiguration& config = CompilerConfiguration::instance();
            rstring main = config.getStringValue(CompilerConfiguration::MainComposite);
            ExpressionValue* v = new ExpressionValue(main);
            uint32_t litIndex;
            uint32_t idIndex = lit.addLiteral(*new Literal(_type, v), getLocation(), litIndex);
            return new LiteralSymbolExpression(_type, _location, idIndex, litIndex);
        }
    }

    int numArgs = _args.size();
    for (int i = 0; i < numArgs; i++) {
        Expression*& e = _args[i];
        e = Expression::replaceLits(e, lit, onlySTP);
    }
    return this;
}

Expression* CallExpression::simplify(ExpressionEvaluator& eval)
{
    // simplify children
    int numArgs = _args.size();
    for (int i = 0; i < numArgs; i++) {
        Expression*& e = _args[i];
        e = Expression::simplify(e, eval);
    }

    if (NULL == _fcn) {
        _dontSimplify = true;
        return this;
    }

    // Special handling of certain spl builtins
    bool setDontSimplify1 = false;
    if (_fcn->isIntrinsic()) {
        const string& fName = _fcn->name();
        if (fName == "submit") {
            if (_args.size() == 2 && _args[1]->getType().getMetaType() != MetaType::UINT32) {
                uint32_t port =
                  static_cast<uint32_t>(_args[1]->as<StreamSymbolExpression>().getPort());
                ExpressionValue portValue(port);
                const SourceLocation& loc = _args[1]->getLocation();
                delete _args[1];
                _args[1] = buildExpression(TypeFactory::instance().primitiveT(MetaType::UINT32),
                                           loc, portValue);
                _dontSimplify = true;
            }
            return this;
        } else if (fName == "byChannel" || fName == "byReplica") {
            const SourceLocation& loc = _location.mappedSourceLocation();
            if (CompilationState::instance().allowByChannel()) {
                return buildExpression(TypeFactory::instance().primitiveT(MetaType::RSTRING), loc,
                                       ExpressionValue(SPL::rstring(fName + "()")));
            } else {
                SysOut::errorln(SPL_CORE_INTRINSIC_DISALLOWED(fName), loc);
            }
        }
    }

    // try to simplify
    int32_t builtinNum = _fcn->builtinNumber();
    if (builtinNum < 0) {
        return this;
    }

    if (_fcn->isNativeFunction()) {
        if (_fcn->nameSpace() == "spl.utility") {
            const string& fName = _fcn->name();
            const CompilationState& state = CompilationState::instance();
            if (fName == "assert" || fName == "log" || fName == "abort" || fName == "appLog" ||
                fName == "appTrc") {
                // Add the fileName and line number of the call to the call
                const SourceLocation& loc = _location.mappedSourceLocation();
                SPL::rstring fileName = loc.fileName();
                SPL::uint32 line = loc.line();
                TypeFactory& tf = TypeFactory::instance();
                _args.push_back(buildExpression(tf.primitiveT(MetaType::RSTRING), loc,
                                                ExpressionValue(fileName)));
                _args.push_back(
                  buildExpression(tf.primitiveT(MetaType::UINT32), loc, ExpressionValue(line)));
                numArgs += 2;
                setDontSimplify1 = true;
            } else if ((fName == "getChannel" && state.stringizeGetChannel()) ||
                       (fName == "getLocalChannel" && state.stringizeGetLocalChannel()) ||
                       (fName == "getAllChannels" && state.stringizeGetAllChannels()) ||
                       (fName == "getMaxChannels" && state.stringizeGetMaxChannels()) ||
                       (fName == "getLocalMaxChannels" && state.stringizeGetLocalMaxChannels()) ||
                       (fName == "getAllMaxChannels" && state.stringizeGetAllMaxChannels())) {
                TypeFactory& tf = TypeFactory::instance();
                const SourceLocation& loc = _location.mappedSourceLocation();
                string functionName = fName + "()";
                return buildExpression(tf.primitiveT(MetaType::RSTRING), loc,
                                       ExpressionValue(SPL::rstring(functionName)));
            } else if (state.evaluateUDPIntrinsicsAs64BitLiteral() &&
                       (fName == "getChannel" || fName == "getLocalChannel" ||
                        fName == "getMaxChannels" || fName == "getLocalMaxChannels")) {
                TypeFactory& tf = TypeFactory::instance();
                const SourceLocation& loc = _location.mappedSourceLocation();
                return buildExpression(tf.primitiveT(MetaType::INT64), loc,
                                       ExpressionValue((int64_t)0));
            }
        }
    }

    // We have a possible builtin.  Gather the arguments
    vector<ExpressionValue> args;
    args.resize(numArgs);
    for (int i = 0; i < numArgs; i++) {
        if (!_args[i]->getExpressionValue(args[i]) || _args[i]->dontSimplify()) {
            return this;
        }
    }

    if (eval.dontCatch()) {
        ExpressionValue result;
        if (eval.evaluateBuiltin(builtinNum, args, result)) {
            return buildExpression(_type, _location, result, setDontSimplify1);
        }
    } else {
        // All arguments converted
        try {
            ExpressionValue result;
            if (eval.evaluateBuiltin(builtinNum, args, result)) {
                return buildExpression(_type, _location, result, setDontSimplify1);
            }
        } catch (Distillery::DistilleryException& excp) {
            SysOut::errorln(SPL_CORE_EVALUATION_EXCEPTION_MSG(excp.getExplanation(), toString()),
                            _location);
        } catch (...) {
            // We got an exception - tell the user
            SysOut::errorln(SPL_CORE_EVALUATION_EXCEPTION(toString()), _location);
        }
    }
    if (setDontSimplify1) {
        _dontSimplify = true;
    }
    return this;
}

bool CallExpression::readsState(void) const
{
    for (uint32_t i = 0, n = _args.size(); i < n; i++) {
        if (_args[i]->readsState()) {
            return true;
        }
    }

    return false;
}

bool CallExpression::hasSideEffects(void) const
{
    assert(NULL != _fcn);
    // Check the function
    if (_fcn->isStateful()) {
        return true;
    }
    const FunctionHeadSym::Formals formals = _fcn->formals();
    for (uint32_t i = 0; i < formals.size(); i++) {
        if (formals[i]->isMutable()) {
            return true;
        }
    }

    // Now check the args
    for (uint32_t i = 0, n = _args.size(); i < n; i++) {
        if (_args[i]->hasSideEffects()) {
            return true;
        }
    }

    return false;
}

ostream& CallExpression::print(ostream& ostr, const Expression& root) const
{
    ostr << _functionName;
    ostr << '(';
    int numArgs = _args.size();
    for (int i = 0; i < numArgs; i++) {
        _args[i]->print(ostr, *_args[i]);
        if (i != numArgs - 1) {
            ostr << ", ";
        }
    }
    return ostr << ')';
}

CallExpression& CallExpression::clone() const
{
    vector<Expression*> clonedArgs;
    for (uint32_t i = 0; i < _args.size(); i++) {
        clonedArgs.push_back(&_args[i]->clone());
    }
    CallExpression* n;
    if (NULL == _fcn) {
        n = new CallExpression(getType(), getLocation(), _functionName, clonedArgs, _compositeName);
    } else {
        n = new CallExpression(getType(), getLocation(), *_fcn, clonedArgs, _compositeName);
    }
    if (_submitNeedingCopy) {
        n->_submitNeedingCopy = true;
    }
    return *n;
}

bool CallExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const CallExpression& cRhs = static_cast<const CallExpression&>(rhs);

    if (_fcn != cRhs._fcn || _args.size() != cRhs._args.size()) {
        return false;
    }

    for (uint32_t i = 0; i < _args.size(); i++) {
        if (*_args[i] != *cRhs._args[i]) {
            return false;
        }
    }
    return true;
}

/*
 * Nary Expression
 */
NaryExpression::NaryExpression(const RootTyp& type,
                               const SourceLocation& l,
                               Op op,
                               Expression& lhs,
                               Expression& rhs,
                               Expression& res)
  : Expression(Nary, type, l)
  , _op(op)
{
    MODEL("NaryExpression(" << (void*)this << ", " << (int)op << ", " << (void*)&lhs << ", "
                            << (void*)&rhs << ", " << (void*)&res << ")\n");
    _opds.push_back(&lhs);
    _opds.push_back(&rhs);
    _opds.push_back(&res);
}

NaryExpression::NaryExpression(const RootTyp& type,
                               const SourceLocation& l,
                               Op op,
                               const std::vector<Expression*>& opds)
  : Expression(Nary, type, l)
  , _op(op)
{
    _opds = opds;
}

NaryExpression::~NaryExpression()
{
    for (uint32_t i = 0; i < _opds.size(); i++) {
        delete _opds[i];
    }
}

bool NaryExpression::equal(const Expression& rhs) const
{
    assert(getKind() == rhs.getKind());
    const NaryExpression& naryRhs = static_cast<const NaryExpression&>(rhs);

    if (_op != naryRhs._op || getType() != rhs.getType() || _opds.size() != naryRhs._opds.size()) {
        return false;
    }

    for (uint32_t i = 0; i < _opds.size(); i++) {
        if (*_opds[i] != *naryRhs._opds[i]) {
            return false;
        }
    }

    return true;
}

bool NaryExpression::hasSideEffects(void) const
{

    // FIX_ME: always true for now
    return true;

#if 0
    switch(_op) {
        case eq:
        case starEq:
        case slashEq:
        case modEq:
        case plusEq:
        case minusEq:
        case ampEq:
        case hatEq:
        case barEq:
        case lShiftEq:
        case rShiftEq:
            return true;
        default:;
    }

    // Now check the args
    for (uint32_t i = 0, n = _opds.size(); i < n; i++) {
        if(_opds[i]->hasSideEffects())
            return true;
    }

    return false;
#endif
}

ostream& NaryExpression::print(ostream& ostr, const Expression& root) const
{

    ostr << opDescription[_op];
    ostr << '(';
    int numOpds = _opds.size();
    for (int i = 0; i < numOpds; i++) {
        _opds[i]->print(ostr, *_opds[i]);
        if (i != numOpds - 1) {
            ostr << ", ";
        }
    }
    return ostr << ')';
}

NaryExpression& NaryExpression::clone() const
{
    vector<Expression*> clonedOpds;
    for (uint32_t i = 0; i < _opds.size(); i++) {
        clonedOpds.push_back(&_opds[i]->clone());
    }

    return *new NaryExpression(getType(), getLocation(), _op, clonedOpds);
}

string NaryExpression::dotOperator(Op op)
{
    switch (op) {
        case dotPlus:
            return "dotPlus";
        case dotMinus:
            return "dotMinus";
        case dotStar:
            return "dotTimes";
        case dotSlash:
            return "dotDivides";
        case dotMod:
            return "dotModulus";
        case dotLShift:
            return "dotLshift";
        case dotRShift:
            return "dotRshift";
        case dotAmp:
            return "dotAnd";
        case dotBar:
            return "dotOr";
        case dotHat:
            return "dotXor";
        default:
            assert(false);
            break;
    }
    return "";
}

Expression::Op NaryExpression::mapOperator() const
{
    MetaType lmt = getLhs().getType().getMetaType();
    MetaType rmt = getRhs().getType().getMetaType();

    bool needsDot = false;
    switch (lmt) {
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::MAP:
        case MetaType::BMAP:
            needsDot = true;
            break;
        default:
            break;
    }
    switch (rmt) {
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::MAP:
        case MetaType::BMAP:
            needsDot = true;
            break;
        default:
            break;
    }
    switch (_op) {
        case plus:
            if (needsDot) {
                return dotPlus;
            }
            break;
        case minus:
            if (needsDot) {
                return dotMinus;
            }
            break;
        case star:
            if (needsDot) {
                return dotStar;
            }
            break;
        case slash:
            if (needsDot) {
                return dotSlash;
            }
            break;
        case mod:
            if (needsDot) {
                return dotMod;
            }
            break;
        case lShift:
            if (needsDot) {
                return dotLShift;
            }
            break;
        case rShift:
            if (needsDot) {
                return dotRShift;
            }
            break;
        case amp:
            if (needsDot) {
                return dotAmp;
            }
            break;
        case bar:
            if (needsDot) {
                return dotBar;
            }
            break;
        case hat:
            if (needsDot) {
                return dotHat;
            }
            break;
        case greater:
            if (needsDot) {
                return dotGreater;
            }
            break;
        case geq:
            if (needsDot) {
                return dotGeq;
            }
            break;
        case less:
            if (needsDot) {
                return dotLess;
            }
            break;
        case leq:
            if (needsDot) {
                return dotLeq;
            }
            break;
        case eqEq:
            if (needsDot && lmt != rmt) {
                return dotEqEq;
            }
            break;
        case neq:
            if (needsDot && lmt != rmt) {
                return dotNeq;
            }
            break;
        default:
            break;
    }
    return _op;
}

void NaryExpression::generate(ostream& s,
                              uint32_t depth,
                              GenerationContext context,
                              bool genTemplate) const
{
    Op op = mapOperator();

    switch (op) {
        case dotPlus:
        case dotMinus:
        case dotStar:
        case dotSlash:
        case dotMod:
        case dotLShift:
        case dotRShift:
        case dotAmp:
        case dotBar:
        case dotHat:
            s << "::SPL::" << dotOperator(op) << '(';
            if (genTemplate) {
                s << "{Lhs}";
            } else {
                getLhs().generate(s, depth, General, genTemplate);
            }
            s << ", ";
            if (genTemplate) {
                s << "{Rhs}";
            } else {
                getRhs().generate(s, depth, General, genTemplate);
            }
            s << ", ";
            if (genTemplate) {
                s << "{Res}";
            } else {
                getRes().generate(s, depth, General, genTemplate);
            }
            s << ')';
            break;
        default:
            break;
    }
}

Expression* NaryExpression::replaceLits(LiteralReplacer& lit, bool onlySTP)
{
    // simplify children
    int numArgs = _opds.size();
    for (int i = 0; i < numArgs; i++) {
        Expression*& e = _opds[i];
        e = Expression::replaceLits(e, lit, onlySTP);
    }
    return this;
}

Expression* NaryExpression::simplify(ExpressionEvaluator& eval)
{

    // simplify children
    int numArgs = _opds.size();
    for (int i = 0; i < numArgs; i++) {
        Expression*& e = _opds[i];
        e = Expression::simplify(e, eval);
    }

    // FIX_ME do more simplication

    return this;
}

LiteralReplacer::~LiteralReplacer()
{
    uint32_t numLits = _lits.size();
    for (uint32_t i = 0; i < numLits; i++) {
        delete _lits[i];
    }
}

LiteralReplacer::ArgInfo::ArgInfo(const ArgInfo& rhs)
  : _kind(rhs._kind)
  , _compositeName(rhs._compositeName)
  , _required(rhs._required)
{
    if (rhs._name) {
        _name = new string(*rhs._name);
    } else {
        _name = NULL;
    }
}

LiteralReplacer::ArgInfo& LiteralReplacer::ArgInfo::operator=(const ArgInfo& rhs)
{
    _kind = rhs._kind;
    _compositeName = rhs._compositeName;
    delete _name;
    if (rhs._name) {
        _name = new string(*rhs._name);
    } else {
        _name = NULL;
    }
    _required = rhs._required;
    return *this;
}

bool LiteralReplacer::ArgInfo::operator==(const ArgInfo& rhs) const
{
    if (_kind != rhs._kind || _required != rhs._required || _compositeName != rhs._compositeName) {
        return false;
    }
    if (_name == NULL) {
        return rhs._name == NULL;
    }
    if (rhs._name == NULL) {
        return false;
    }
    return *_name == *rhs._name;
}

uint32_t LiteralReplacer::addLiteral(const Literal& lit, const SourceLocation& loc, uint32_t& index)
{
    index = _lits.size();
    _lits.push_back(&lit.clone());
    ArgInfo a("none", "", NULL, false);
    _arguments.push_back(a);
    _sourceLocs.push_back(&loc);
    return _nextId++;
}

uint32_t LiteralReplacer::addArgument(const Literal& lit,
                                      const SourceLocation& loc,
                                      const std::string& kind,
                                      const string& compositeName,
                                      const string& arg,
                                      bool required,
                                      uint32_t& index)
{
    index = _lits.size();
    _lits.push_back(&lit);
    ArgInfo a(kind, compositeName, new string(arg), required);
    _arguments.push_back(a);
    _sourceLocs.push_back(&loc);
    return _nextId++;
}

bool Expression::isEvaluatable(void) const
{
    typedef unordered_set<const RootTyp*> Types;
    typedef unordered_set<const FunctionHeadSym*> Functions;

    Types allTypes;
    Functions allFunctions;

    collectTypeAndFunctions(&allTypes, &allFunctions, false);
    if (allFunctions.size() > 0) {
        for (Functions::const_iterator it = allFunctions.begin(); it != allFunctions.end(); ++it) {
            if ((*it)->builtinNumber() < 0) {
                return false;
            }
        }
    }
    return true;
}

ostream& SPL::operator<<(ostream& ostr, Expression::Op op)
{
    ostr << Expression::opAsString(op);
    return ostr;
}
