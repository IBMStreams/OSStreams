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
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/META.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/Runtime/Function/BuiltinSPLFunctions.h>

#include <ostream>

using namespace std;
using namespace std::tr1;
using namespace SPL;

#define BINOP(R, FCN, T)                                                                           \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        result.assign(                                                                             \
          static_cast<SPL::T>(lhs.BOOST_PP_CAT(get_, T)() FCN rhs.BOOST_PP_CAT(get_, T)()));       \
        break;

#define DIVBINOP(R, FCN, T)                                                                        \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        if (rhs.BOOST_PP_CAT(get_, T)() == T(0))                                                   \
            THROW(SPLCompilerDivModByZero, SPL_CORE_DIV_MOD_BY_ZERO);                              \
        result.assign(                                                                             \
          static_cast<SPL::T>(lhs.BOOST_PP_CAT(get_, T)() FCN rhs.BOOST_PP_CAT(get_, T)()));       \
        break;

#define CMPBINOP(R, FCN, T)                                                                        \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        result.assign(                                                                             \
          static_cast<SPL::boolean>(lhs.BOOST_PP_CAT(get_, T)() FCN rhs.BOOST_PP_CAT(get_, T)())); \
        break;

#define UOP(R, FCN, T)                                                                             \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        result.assign(static_cast<SPL::T>(FCN rhs.BOOST_PP_CAT(get_, T)()));                       \
        break;

bool ExpressionEvaluator::evaluateOp(Operation op,
                                     const ExpressionValue& arg1,
                                     const ExpressionValue& arg2,
                                     ExpressionValue& result)
{
    switch (op) {
        case Add:
            return add(arg1, arg2, result);
        case Sub:
            return sub(arg1, arg2, result);
        case Mul:
            return mul(arg1, arg2, result);
        case Div:
            return div(arg1, arg2, result);
        case Mod:
            return mod(arg1, arg2, result);
        case LeftShift:
            return shiftl(arg1, arg2, result);
        case RightShift:
            return shiftr(arg1, arg2, result);
        case Negate:
            return negate(arg1, result);
        case And:
            return doAnd(arg1, arg2, result);
        case Or:
            return doOr(arg1, arg2, result);
        case Xor:
            return doXor(arg1, arg2, result);
        case In:
            return doIn(arg1, arg2, result);
        case Complement:
            return complement(arg1, result);
        case Equal:
            return equal(arg1, arg2, result);
        case NotEqual:
            return notEqual(arg1, arg2, result);
        case LessThan:
            return lessThan(arg1, arg2, result);
        case LessEqual:
            return lessEqual(arg1, arg2, result);
        case GreaterThan:
            return greaterThan(arg1, arg2, result);
        case GreaterEqual:
            return greaterEqual(arg1, arg2, result);
        case DotEqual:
            return dotEqual(arg1, arg2, result);
        case DotNotEqual:
            return dotNotEqual(arg1, arg2, result);
    }
    return false;
}

static string exprMismatch(const string& op)
{
    return string("Evaluation type mismatch in '") + op + '\'';
}

static string exprListMismatch(const string& op)
{
    return string("Evaluation type mismatch in list '") + op + '\'';
}

bool ExpressionEvaluator::add(const ExpressionValue& lhs,
                              const ExpressionValue& rhs,
                              ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("+"));

    if (lhs.type() == MetaType::LIST)
        return addList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, +, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, +, STRING_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, +, TIMESTAMP_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::sub(const ExpressionValue& lhs,
                              const ExpressionValue& rhs,
                              ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("-"));

    if (lhs.type() == MetaType::LIST)
        return subList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, -, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, -, TIMESTAMP_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::mul(const ExpressionValue& lhs,
                              const ExpressionValue& rhs,
                              ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("*"));

    if (lhs.type() == MetaType::LIST)
        return mulList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, *, ARITH_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::div(const ExpressionValue& lhs,
                              const ExpressionValue& rhs,
                              ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("/"));

    if (lhs.type() == MetaType::LIST)
        return divList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(DIVBINOP, /, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, /, ALL_FLOAT_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::mod(const ExpressionValue& lhs,
                              const ExpressionValue& rhs,
                              ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("%"));

    if (lhs.type() == MetaType::LIST)
        return modList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(DIVBINOP, %, INTEGER_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::shiftl(const ExpressionValue& lhs,
                                 const ExpressionValue& rhs,
                                 ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("<<"));

    if (lhs.type() == MetaType::LIST)
        return modList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, <<, INTEGER_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::shiftr(const ExpressionValue& lhs,
                                 const ExpressionValue& rhs,
                                 ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch(">>"));

    if (lhs.type() == MetaType::LIST)
        return modList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, >>, INTEGER_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::negate(const ExpressionValue& rhs, ExpressionValue& result)
{
    if (rhs.type() == MetaType::LIST)
        return negateList(rhs, result);

    switch (rhs.type()) {
        BOOST_PP_LIST_FOR_EACH(UOP, -, ARITH_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::doAnd(const ExpressionValue& lhs,
                                const ExpressionValue& rhs,
                                ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("&"));

    if (lhs.type() == MetaType::LIST)
        return doAndList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, &, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, &, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::doOr(const ExpressionValue& lhs,
                               const ExpressionValue& rhs,
                               ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("|"));

    if (lhs.type() == MetaType::LIST)
        return doOrList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, |, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, |, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::doXor(const ExpressionValue& lhs,
                                const ExpressionValue& rhs,
                                ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("^"));

    if (lhs.type() == MetaType::LIST)
        return doXorList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, ^, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, ^, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::doIn(const ExpressionValue& item,
                               const ExpressionValue& rhs,
                               ExpressionValue& result)
{
    MetaType rType = rhs.type();
    if (!(item.type() == rhs.elementType() && (rType == MetaType::LIST || rType == MetaType::SET)))
        THROW(SPLCompilerInvalidExpression, exprMismatch("in"));

    bool found = false;

#define INOP(R, _, T)                                                                              \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        const SPL::list<SPL::T>& l = rhs.BOOST_PP_CAT(get_list_, T)();                             \
        const SPL::T& it = item.BOOST_PP_CAT(get_, T)();                                           \
        uint32_t num = l.size();                                                                   \
        for (int i = 0; i < num; i++)                                                              \
            if (it == l[i]) {                                                                      \
                found = true;                                                                      \
                break;                                                                             \
            }                                                                                      \
    } break;

    if (rType == MetaType::LIST) {
        switch (item.type()) {
            BOOST_PP_LIST_FOR_EACH(INOP, *, ARITH_TYPES)
            default:
                return false;
        }
    } else {
#undef INOP
#define INOP(R, _, T)                                                                              \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        const SPL::set<SPL::T>& s = rhs.BOOST_PP_CAT(get_set_, T)();                               \
        const SPL::T& it = item.BOOST_PP_CAT(get_, T)();                                           \
        found = s.count(it) > 0;                                                                   \
    } break;
        switch (item.type()) {
            BOOST_PP_LIST_FOR_EACH(INOP, *, ARITH_TYPES)
            default:
                return false;
        }
    }
    result.assign(static_cast<boolean>(found));
    return true;
#undef INOP
}

bool ExpressionEvaluator::complement(const ExpressionValue& rhs, ExpressionValue& result)
{
    if (rhs.type() == MetaType::LIST)
        return complementList(rhs, result);

    switch (rhs.type()) {
        BOOST_PP_LIST_FOR_EACH(UOP, ~, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(UOP, !, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

// Comparisons
bool ExpressionEvaluator::equal(const ExpressionValue& lhs,
                                const ExpressionValue& rhs,
                                ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("=="));

    if (lhs.type() == MetaType::LIST)
        return equalList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, ==, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, ==, STRING_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, ==, MISC_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::notEqual(const ExpressionValue& lhs,
                                   const ExpressionValue& rhs,
                                   ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("!="));

    if (lhs.type() == MetaType::LIST)
        return notEqualList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, !=, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, !=, STRING_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, !=, MISC_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::lessThan(const ExpressionValue& lhs,
                                   const ExpressionValue& rhs,
                                   ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("<"));

    if (lhs.type() == MetaType::LIST)
        return lessThanList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, <, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::lessEqual(const ExpressionValue& lhs,
                                    const ExpressionValue& rhs,
                                    ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch("<="));

    if (lhs.type() == MetaType::LIST)
        return lessEqualList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, <=, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::greaterThan(const ExpressionValue& lhs,
                                      const ExpressionValue& rhs,
                                      ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch(">"));

    if (lhs.type() == MetaType::LIST)
        return greaterThanList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, >, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::greaterEqual(const ExpressionValue& lhs,
                                       const ExpressionValue& rhs,
                                       ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprMismatch(">="));

    if (lhs.type() == MetaType::LIST)
        return greaterEqualList(lhs, rhs, result);

    switch (lhs.type()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, >=, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

#undef BINOP
#undef UOP
#undef CMPBINOP

#define BINOP(R, FCN, T)                                                                           \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        SPL::list<SPL::T> res;                                                                     \
        const SPL::list<SPL::T>& l = lhs.BOOST_PP_CAT(get_list_, T)();                             \
        const SPL::list<SPL::T>& r = rhs.BOOST_PP_CAT(get_list_, T)();                             \
        for (int i = 0; i < l.size(); i++)                                                         \
            res.push_back(static_cast<SPL::T>(l[i] FCN r[i]));                                     \
        result.assign(res);                                                                        \
    } break;

#undef DIVBINOP
#define DIVBINOP(R, FCN, T)                                                                        \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        SPL::list<SPL::T> res;                                                                     \
        const SPL::list<SPL::T>& l = lhs.BOOST_PP_CAT(get_list_, T)();                             \
        const SPL::list<SPL::T>& r = rhs.BOOST_PP_CAT(get_list_, T)();                             \
        for (int i = 0; i < l.size(); i++) {                                                       \
            if (r[i] == T(0))                                                                      \
                THROW(SPLCompilerDivModByZero, SPL_CORE_DIV_MOD_BY_ZERO);                          \
            res.push_back(static_cast<SPL::T>(l[i] FCN r[i]));                                     \
        }                                                                                          \
        result.assign(res);                                                                        \
    } break;

#define CMPBINOP(R, FCN, T)                                                                        \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        SPL::list<SPL::boolean> res;                                                               \
        const SPL::list<SPL::T>& l = lhs.BOOST_PP_CAT(get_list_, T)();                             \
        const SPL::list<SPL::T>& r = rhs.BOOST_PP_CAT(get_list_, T)();                             \
        for (int i = 0; i < l.size(); i++)                                                         \
            res.push_back(static_cast<SPL::boolean>(l[i] FCN r[i]));                               \
        result.assign(res);                                                                        \
    } break;

#define UOP(R, FCN, T)                                                                             \
    case MetaType::BOOST_PP_CAT(META_, T): {                                                       \
        SPL::list<SPL::T> res;                                                                     \
        const SPL::list<SPL::T>& r = rhs.BOOST_PP_CAT(get_list_, T)();                             \
        for (int i = 0; i < r.size(); i++)                                                         \
            res.push_back(static_cast<SPL::T>(FCN r[i]));                                          \
        result.assign(res);                                                                        \
    } break;

bool ExpressionEvaluator::addList(const ExpressionValue& lhs,
                                  const ExpressionValue& rhs,
                                  ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("+"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, +, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, +, STRING_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, +, TIMESTAMP_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::subList(const ExpressionValue& lhs,
                                  const ExpressionValue& rhs,
                                  ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("-"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, -, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, -, TIMESTAMP_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::mulList(const ExpressionValue& lhs,
                                  const ExpressionValue& rhs,
                                  ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("*"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, *, ARITH_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::divList(const ExpressionValue& lhs,
                                  const ExpressionValue& rhs,
                                  ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("/"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(DIVBINOP, /, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, /, ALL_FLOAT_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::modList(const ExpressionValue& lhs,
                                  const ExpressionValue& rhs,
                                  ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("%"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(DIVBINOP, %, INTEGER_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::shiftlList(const ExpressionValue& lhs,
                                     const ExpressionValue& rhs,
                                     ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("<<"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, <<, INTEGER_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::shiftrList(const ExpressionValue& lhs,
                                     const ExpressionValue& rhs,
                                     ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch(">>"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, >>, INTEGER_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::negateList(const ExpressionValue& rhs, ExpressionValue& result)
{
    switch (rhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(UOP, -, ARITH_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::doAndList(const ExpressionValue& lhs,
                                    const ExpressionValue& rhs,
                                    ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("&"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, &, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, &, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::doOrList(const ExpressionValue& lhs,
                                   const ExpressionValue& rhs,
                                   ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("|"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, |, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, |, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::doXorList(const ExpressionValue& lhs,
                                    const ExpressionValue& rhs,
                                    ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("^"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(BINOP, ^, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(BINOP, ^, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::complementList(const ExpressionValue& rhs, ExpressionValue& result)
{
    switch (rhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(UOP, ~, INTEGER_TYPES)
        BOOST_PP_LIST_FOR_EACH(UOP, !, BOOL_TYPES)
        default:
            return false;
    }
    return true;
}

// Comparisons
bool ExpressionEvaluator::dotEqual(const ExpressionValue& lhs,
                                   const ExpressionValue& rhs,
                                   ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("=="));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, ==, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, ==, STRING_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, ==, MISC_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::equalList(const ExpressionValue& lhs,
                                    const ExpressionValue& rhs,
                                    ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        return false;

    result.assign(static_cast<SPL::boolean>(lhs == rhs));
    return true;
}

bool ExpressionEvaluator::dotNotEqual(const ExpressionValue& lhs,
                                      const ExpressionValue& rhs,
                                      ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("!="));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, !=, ARITH_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, !=, STRING_TYPES)
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, !=, MISC_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::notEqualList(const ExpressionValue& lhs,
                                       const ExpressionValue& rhs,
                                       ExpressionValue& result)
{
    if (!lhs.typesMatch(rhs))
        return false;

    result.assign(static_cast<SPL::boolean>(lhs != rhs));
    return true;
}

bool ExpressionEvaluator::lessThanList(const ExpressionValue& lhs,
                                       const ExpressionValue& rhs,
                                       ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("<"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, <, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::lessEqualList(const ExpressionValue& lhs,
                                        const ExpressionValue& rhs,
                                        ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch("<="));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, <=, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::greaterThanList(const ExpressionValue& lhs,
                                          const ExpressionValue& rhs,
                                          ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch(">"));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, >, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

bool ExpressionEvaluator::greaterEqualList(const ExpressionValue& lhs,
                                           const ExpressionValue& rhs,
                                           ExpressionValue& result)
{
    if (!lhs.listsMatch(rhs))
        THROW(SPLCompilerInvalidExpression, exprListMismatch(">="));

    switch (lhs.elementType()) {
        BOOST_PP_LIST_FOR_EACH(CMPBINOP, >=, ORDERED_TYPES)
        default:
            return false;
    }
    return true;
}

#define EVALUATABLE(R, _, T)                                                                       \
    case MetaType::BOOST_PP_CAT(META_, T):                                                         \
        return true;                                                                               \
        break;

bool ExpressionEvaluator::isEvaluatableType(const RootTyp& type)
{
    switch (type.getMetaType()) {
        BOOST_PP_LIST_FOR_EACH(EVALUATABLE, _, EV_BUILTIN_TYPES)

        case MetaType::LIST: {
            const ListTyp* lt = static_cast<const ListTyp*>(&type);
            switch (lt->getElementType().getMetaType()) {
                BOOST_PP_LIST_FOR_EACH(EVALUATABLE, _, EV_BUILTIN_TYPES)
                default:
                    return false;
            }
            break;
        }
        case MetaType::SET: {
            const SetTyp* st = static_cast<const SetTyp*>(&type);
            switch (st->getElementType().getMetaType()) {
                BOOST_PP_LIST_FOR_EACH(EVALUATABLE, _, EV_BUILTIN_TYPES)
                default:
                    return false;
            }
            break;
        }
        default:
            return false;
    }

    return false;
}

bool ExpressionEvaluator::isEvaluatable(const Expression& e)
{
    typedef unordered_set<const RootTyp*> Types;
    typedef unordered_set<const FunctionHeadSym*> Functions;

    Types allTypes;
    Functions allFunctions;

    e.collectTypeAndFunctions(&allTypes, &allFunctions, false);

    for (Types::const_iterator it = allTypes.begin(); it != allTypes.end(); ++it) {
        if (!isEvaluatableType(**it))
            return false;
    }

    // built ins with numbers less than zero are not evaluatable
    // if above test works (for types) then builtin should be evaluatable
    // for that type
    for (Functions::const_iterator it = allFunctions.begin(); it != allFunctions.end(); ++it) {
        int32_t fcnNum = (*it)->builtinNumber();
        if (fcnNum < 0)
            return false;
        if (!isBuiltinEvaluatable(fcnNum))
            return false;
    }

    return true;
}

class SubstituteValueContext
{
  public:
    // TBD: add failure code enum
    enum ExpressionEvaluator::EvaluateFailureCode _reasonCode;
    const ValueTableType& _valueTable;

    SubstituteValueContext(const ValueTableType& valueTable)
      : _valueTable(valueTable)
    {
        _reasonCode = ExpressionEvaluator::Success;
    }

    bool substituteValue(Expression& e, Expression*& r);
};

bool SubstituteValueContext::substituteValue(Expression& expr, Expression*& result)
{
    Expression* this_result; // scratch var for result of operand substitution

    bool done_ = false; // nothing has gone wrong yet

    result = NULL; // assume that there is no result

    switch (expr.getKind()) {
        // n-ary nodes
        case Expression::Call: {
            CallExpression& e = (CallExpression&)expr;
            vector<Expression*>& args = e.getArgs();
            for (uint32_t i = 0; i < args.size(); ++i) {
                if (!done_) {
                    done_ = substituteValue(*args[i], this_result);
                    if (this_result)
                        args[i] = this_result;
                }
            }
        } break;

        // ternary nodes
        case Expression::Subscript: {
            SubscriptExpression& e = (SubscriptExpression&)expr;
            done_ = substituteValue(e.getLhs(), this_result);
            if (this_result)
                e.setLhs(*this_result);
            if (!done_ && e.lower()) {
                done_ = substituteValue(*e.lower(), this_result);
                if (this_result)
                    e.setLower(*this_result);
            }
            if (!done_ && e.upper()) {
                done_ = substituteValue(*e.upper(), this_result);
                if (this_result)
                    e.setUpper(*this_result);
            }
        } break;

        case Expression::Conditional: {
            ConditionalExpression& e = (ConditionalExpression&)expr;
            done_ = substituteValue(e.getCondition(), this_result);
            if (this_result)
                e.setCondition(*this_result);
            if (!done_) {
                done_ = substituteValue(e.getLhs(), this_result);
                if (this_result)
                    e.setLhs(*this_result);
            }
            if (!done_) {
                done_ = substituteValue(e.getRhs(), this_result);
                if (this_result)
                    e.setRhs(*this_result);
            }
        } break;

        // binary nodes
        case Expression::Attribute: {
            AttributeExpression& e = (AttributeExpression&)expr;
            done_ = substituteValue(e.getLhs(), this_result);
            if (this_result)
                e.setLhs(*this_result);
            if (!done_) {
                done_ = substituteValue(e.getRhs(), this_result);
                if (this_result)
                    e.setRhs(*this_result);
            }
        } break;

        case Expression::Binary: {
            BinaryExpression& e = (BinaryExpression&)expr;
            done_ = substituteValue(e.getLhs(), this_result);
            if (this_result)
                e.setLhs(*this_result);
            if (!done_) {
                done_ = substituteValue(e.getRhs(), this_result);
                if (this_result)
                    e.setRhs(*this_result);
            }
        } break;

        // unary nodes
        case Expression::Cast:
        case Expression::Prefix:
        case Expression::Postfix:
        case Expression::Unary: {
            UnaryExpression& e = (UnaryExpression&)expr;
            done_ = substituteValue(e.getExpn(), this_result);
            if (this_result)
                e.setExpn(*this_result);
        } break;

        // leaf nodes
        case Expression::Enum:
        case Expression::Lit:
        case Expression::LiteralSymbol:
        case Expression::StreamHistorySymbol:
        case Expression::StreamSymbol:
        case Expression::CustomLiteral:
            break;

        // symbol nodes are substituted with their literal values
        case Expression::Symbol: {
            SymbolExpression& e = (SymbolExpression&)expr;

            ValueTableType::const_iterator vit = _valueTable.find(e.getId());

            if (vit == _valueTable.end()) {
                // could not find reference in Value Table
                _reasonCode = ExpressionEvaluator::FoundUnknownReference;
                return true; // we're done
            }

            ExpressionValue* value = vit->second;

            bool typesMatch = false;
            // check if ExpressionValue type match Expression type
            if (e.getType().getMetaType() == value->type()) {
                if (value->type() == MetaType::LIST) {
                    // also check element
                    const ListTyp& lt = static_cast<const ListTyp&>(e.getType());
                    if (lt.getElementType().getMetaType() == value->elementType()) {
                        typesMatch = true;
                    }
                } else // must be primitive type
                    typesMatch = true;
            }

            if (!typesMatch) {
                // types don't match
                _reasonCode = ExpressionEvaluator::FoundTypeMismatch;
                return true; // we're done
            }

            // make a copy of the value expression
            value = new ExpressionValue(*value);

            // create Literal from ExpressionValue
            Literal* lit = new Literal(e.getType(), value);

            // create Literal Expression
            LiteralExpression* litExpr = new LiteralExpression(e.getType(), e.getLocation(), *lit);

            // return result to caller
            result = litExpr;
        } break;

        default:
            assert(!"Missing case in switch in ExpressionEvaluator");
    }
    return done_;
}

ExpressionValue* ExpressionEvaluator::evaluate(const Expression& e,
                                               const ValueTableType& valueTable,
                                               enum EvaluateFailureCode& reasonCode)
{
    // clone it first
    auto_ptr<Expression> exp(&e.clone());

    SubstituteValueContext substituteValueContext(valueTable);

    Expression* substituteResult;
    bool substituteFailed;

    // traverse the expression and substiture the values
    substituteFailed = substituteValueContext.substituteValue(*exp, substituteResult);

    if (substituteResult) {
        // top level substitution
        exp.reset(substituteResult);
    }

    if (substituteFailed) {
        reasonCode = substituteValueContext._reasonCode;
        return NULL;
    }

    // simplify (fold) the constant expression
    try {
        exp.reset(Expression::simplify(exp.release(), *this));
    } catch (SPLCompilerException const& e1) {
        reasonCode = ExceptionOccured;
        return NULL;
    }

    assert(exp.get());

    ExpressionValue exprValue;
    if (exp->getExpressionValue(exprValue)) {
        reasonCode = Success;
        return new ExpressionValue(exprValue);
    }

    assert(!"Did not Simplify");
    return NULL;
}

namespace SPL {
ostream& operator<<(ostream& ostr, const ValueTableType& vt)
{
    ostr << endl;
    ostr << "Value Table: " << endl;
    for (ValueTableType::const_iterator it = vt.begin(); it != vt.end(); ++it) {
        const string& name = it->first;
        ExpressionValue* ev = it->second;
        ostr << name << " -> " << *ev << endl;
    }
    return ostr << endl;
}

ostream& operator<<(ostream& ostr, const enum ExpressionEvaluator::EvaluateFailureCode& fc)
{
    switch (fc) {
        case ExpressionEvaluator::Success:
            ostr << "Success";
            break;
        case ExpressionEvaluator::FoundTypeMismatch:
            ostr << "Found Type Mismatch";
            break;
        case ExpressionEvaluator::FoundUnknownReference:
            ostr << "Found UnknownReference";
            break;
        case ExpressionEvaluator::ExceptionOccured:
            ostr << "Exception Occured";
            break;
        default:
            ostr << "** Unknown EvaluateFailureCode **";
    }
    return ostr;
}
}
