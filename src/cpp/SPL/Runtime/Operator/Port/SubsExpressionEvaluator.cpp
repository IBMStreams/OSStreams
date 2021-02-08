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

#include <SAM/SAMInterfaceTypes.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyCollection.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyXML.h>
#include <SPL/Runtime/Operator/Port/SubsExpressionEvaluator.h>
#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>

using namespace std;
using namespace SPL;
using namespace xmlns::prod::streams::application;

#define XML_STRING_TO_OBJ(_str, _func, _var)                                                       \
    do {                                                                                           \
        try {                                                                                      \
            stringstream _ts(_str);                                                                \
            _ts.imbue(locale::classic());                                                          \
            _var =                                                                                 \
              _func(_ts, xml_schema::flags::dont_validate | xml_schema::flags::dont_initialize);   \
        } catch (xml_schema::exception & xe) {                                                     \
            std::stringstream ss;                                                                  \
            ss << xe;                                                                              \
            THROW_STRING(SPLRuntimeXMLParsing, SPL_RUNTIME_XML_PARSING_ERROR(_str, ss.str()));     \
        } catch (...) {                                                                            \
            THROW_STRING(SPLRuntimeXMLParsing, SPL_RUNTIME_XML_PARSING_UNKNOWN_ERROR(_str));       \
        }                                                                                          \
    } while (0);

#define MATCH_PRINT(ret) (ret ? "yes" : "no")

const bool SubsExpressionEvaluator::evaluate(const string& expr,
                                             const StreamPropertyCollection& spcol)
{
    return evaluate(*SubscriptionExpression::fromString(expr), spcol);
}

const bool SubsExpressionEvaluator::evaluate(const string& expressionStr,
                                             const string& streamPropertiesTypeXml)
{
    auto_ptr<streamPropertiesType> props;
    XML_STRING_TO_OBJ(streamPropertiesTypeXml, streamProperties, props);
    StreamPropertyCollection spcol;
    fromXML(*props, spcol);

    return evaluate(*SubscriptionExpression::fromString(expressionStr), spcol);
}

const bool SubsExpressionEvaluator::evaluate(const SubscriptionExpression& expr,
                                             const StreamPropertyCollection& spcol)
{
    bool ret = false;

    if (spcol.size() != 0) {

        switch (expr.getKind()) {
            case SubscriptionExpression::AndClause:
                ret = true;
                for (uint32_t scindex = 0; scindex < expr.getSize(); ++scindex) {
                    if (!evaluate(expr.getSubclause(scindex), spcol)) {
                        ret = false;
                        break;
                    }
                }
                break;

            case SubscriptionExpression::OrClause:
                for (uint32_t scindex = 0; scindex < expr.getSize(); ++scindex) {
                    if (evaluate(expr.getSubclause(scindex), spcol)) {
                        ret = true;
                        break;
                    }
                }
                break;

            case SubscriptionExpression::Predicate:
                ret = evaluatePredicate(expr, spcol);
                break;

            default:
                ret = false;
                break;
        }
    }
    APPTRC(L_DEBUG,
           "Match= " << MATCH_PRINT(ret) << " when evaluating expression [" << expr
                     << "] with properties [" << spcol << "]",
           "SPL");
    return ret;
}

const StreamProperty& SubsExpressionEvaluator::findProperty(const string& name,
                                                            const StreamPropertyCollection& spcol)
{
    for (std::vector<StreamProperty>::const_iterator iter = spcol.begin(); iter != spcol.end();
         ++iter) {
        if (iter->getName() == name) {
            return *iter;
        }
    }
    THROW_STRING(SPLRuntimeStreamPropertyNotFound, SPL_RUNTIME_STREAM_PROPERTY_NOT_FOUND(name));
}

const bool SubsExpressionEvaluator::evaluatePredicate(const SubscriptionExpression& expr,
                                                      const StreamPropertyCollection& spcol)
{
    bool ret = false;
    try {
        const string& name = expr.getSymbol();
        const SubscriptionExpression::Literal& lit = expr.getLiteral();
        const StreamProperty& prop = findProperty(name, spcol);

        switch (lit.getKind()) {

            case SubscriptionExpression::Literal::RString:
                ret = evaluate(expr, prop, lit.getRStringValue());
                break;
            case SubscriptionExpression::Literal::Int64:
                ret = evaluateNumeric(expr, prop, lit.getInt64Value());
                break;
            case SubscriptionExpression::Literal::Float64:
                ret = evaluateNumeric(expr, prop, lit.getFloat64Value());
                break;
            default:
                ret = false;
                break;
        }
    } catch (const SPLRuntimeStreamPropertyNotFoundException& e) {
        ret = false;
    } catch (...) {
        throw;
    }
    APPTRC(L_DEBUG,
           "Match= " << MATCH_PRINT(ret) << " when evaluating predicate [" << expr
                     << "] with properties [" << spcol << "]",
           "SPL");
    return ret;
}

#define COMPARE_STRING_VALUE(comparisonOp)                                                         \
    do {                                                                                           \
        if (expr.hasSubscript()) {                                                                 \
            if (prop.getType().isCollection()) {                                                   \
                const list<rstring>& lst = prop;                                                   \
                if (expr.getSubscript() >= 0 && lst.size() > expr.getSubscript()) {                \
                    ret = (lst.at(expr.getSubscript()) comparisonOp val);                          \
                }                                                                                  \
            }                                                                                      \
        } else {                                                                                   \
            if (!prop.getType().isCollection()) {                                                  \
                ret = (static_cast<rstring>(prop) comparisonOp val);                               \
            }                                                                                      \
        }                                                                                          \
    } while (0);

const bool SubsExpressionEvaluator::evaluate(const SubscriptionExpression& expr,
                                             const StreamProperty& prop,
                                             const rstring& val)
{

    if (!prop.getType().isRString() &&
        !(prop.getType().isCollection() &&
          prop.getType().as<Meta::ListType>().getElementType().isRString())) {
        APPTRC(L_DEBUG,
               "Match= " << false << " when evaluating rstring predicate [" << expr
                         << "] with property [" << prop << "]",
               "SPL");
        return false;
    }

    if (expr.hasModuloDivisor()) {
        APPTRC(L_DEBUG,
               "Match= " << false << " when evaluating rstring predicate [" << expr
                         << "] with modulo expn",
               "SPL");
        return false;
    }

    bool ret = false;
    try {
        switch (expr.getOp()) {

            case SubscriptionExpression::LessThan:
                COMPARE_STRING_VALUE(<);
                break;
            case SubscriptionExpression::LessEqual:
                COMPARE_STRING_VALUE(<=);
                break;
            case SubscriptionExpression::GreaterThan:
                COMPARE_STRING_VALUE(>);
                break;
            case SubscriptionExpression::GreaterEqual:
                COMPARE_STRING_VALUE(>=);
                break;
            case SubscriptionExpression::Equal:
                COMPARE_STRING_VALUE(==);
                break;
            case SubscriptionExpression::NotEqual:
                COMPARE_STRING_VALUE(!=);
                break;
            case SubscriptionExpression::In: {
                if (prop.getType().isCollection()) {
                    const list<rstring>& lst = prop;
                    for (uint32_t i = 0; i < lst.size(); ++i) {
                        if (lst.at(i) == val) {
                            ret = true;
                            break;
                        }
                    }
                }
                break;
            }
            default:
                ret = false;
                break;
        }
    } catch (SPLRuntimeTypeMismatchException& e) {
        ret = false;
    }
    APPTRC(L_DEBUG,
           "Match= " << MATCH_PRINT(ret) << " when evaluating rstring predicate [" << expr
                     << "] with property [" << prop << "]",
           "SPL");
    return ret;
}

#define COMPARE_NUMERIC_VALUE(comparisonOp)                                                        \
    do {                                                                                           \
        if (prop.getType().isCollection()) {                                                       \
            if (!expr.hasSubscript()) {                                                            \
                ret = false;                                                                       \
            } else if (prop.getType().as<Meta::ListType>().getElementType().isInt64()) {           \
                const list<int64>& lst = prop;                                                     \
                if (expr.getSubscript() >= 0 && lst.size() > expr.getSubscript()) {                \
                    int64 toCompare = lst.at(expr.getSubscript());                                 \
                    if (expr.hasModuloDivisor())                                                   \
                        doMod(toCompare, expr.getModuloDivisor());                                 \
                    ret = (toCompare comparisonOp val);                                            \
                }                                                                                  \
            } else if (prop.getType().as<Meta::ListType>().getElementType().isFloat64()) {         \
                const list<float64>& lst = prop;                                                   \
                if (expr.getSubscript() >= 0 && lst.size() > expr.getSubscript()) {                \
                    ret = (lst.at(expr.getSubscript()) comparisonOp val);                          \
                }                                                                                  \
            }                                                                                      \
        } else {                                                                                   \
            if (expr.hasSubscript()) {                                                             \
                ret = false;                                                                       \
            } else if (prop.getType().isInt64()) {                                                 \
                int64 toCompare = prop;                                                            \
                if (expr.hasModuloDivisor())                                                       \
                    doMod(toCompare, expr.getModuloDivisor());                                     \
                ret = (toCompare comparisonOp val);                                                \
            } else if (prop.getType().isFloat64()) {                                               \
                const float64& toCompare = prop;                                                   \
                ret = (toCompare comparisonOp val);                                                \
            }                                                                                      \
        }                                                                                          \
    } while (0);

static inline void doMod(SPL::int64& val, SPL::int64 divisor)
{
    val %= divisor;
}

static inline void doMod(double& val, SPL::int64 divisor) {}

template<typename T>
const bool SubsExpressionEvaluator::evaluateNumeric(const SubscriptionExpression& expr,
                                                    const StreamProperty& prop,
                                                    T val)
{
    bool ret = false;

    if (expr.hasModuloDivisor()) {
        if (!prop.getType().isInt64()) {
            APPTRC(L_DEBUG,
                   "Match= " << false << " when evaluating modulo predicate [" << expr
                             << "] with property [" << prop << "]",
                   "SPL");
            return false;
        }
    }

    try {
        switch (expr.getOp()) {
            case SubscriptionExpression::LessThan:
                COMPARE_NUMERIC_VALUE(<);
                break;
            case SubscriptionExpression::LessEqual:
                COMPARE_NUMERIC_VALUE(<=);
                break;
            case SubscriptionExpression::GreaterThan:
                COMPARE_NUMERIC_VALUE(>);
                break;
            case SubscriptionExpression::GreaterEqual:
                COMPARE_NUMERIC_VALUE(>=);
                break;
            case SubscriptionExpression::Equal:
                COMPARE_NUMERIC_VALUE(==);
                break;
            case SubscriptionExpression::NotEqual:
                COMPARE_NUMERIC_VALUE(!=);
                break;
            case SubscriptionExpression::In: {
                if (prop.getType().isCollection()) {
                    if (prop.getType().as<Meta::ListType>().getElementType().isInt64()) {
                        const list<int64>& lst = prop;
                        for (uint32_t i = 0; i < lst.size(); ++i) {
                            if (lst.at(i) == val) {
                                ret = true;
                                break;
                            }
                        }
                    } else if (prop.getType().as<Meta::ListType>().getElementType().isFloat64()) {
                        const list<float64>& lst = prop;
                        for (uint32_t i = 0; i < lst.size(); ++i) {
                            if (lst.at(i) == val) {
                                ret = true;
                                break;
                            }
                        }
                    }
                }
                break;
            }
            default:
                ret = false;
                break;
        }
    } catch (const SPLRuntimeTypeMismatchException& e) {
        ret = false;
    }
    APPTRC(L_DEBUG,
           "Match= " << MATCH_PRINT(ret) << " when evaluating numeric predicate [" << expr
                     << "] with property [" << prop << "]",
           "SPL");
    return ret;
}
