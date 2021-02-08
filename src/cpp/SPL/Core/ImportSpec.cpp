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

#include <SPL/Core/ImportSpec.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/Param.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Runtime/Operator/Port/FilterExpression.h>
#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

#include <assert.h>

using namespace SPL;
using namespace std;
using namespace tr1;

static bool canCast(const MetaType actualType, const MetaType desiredType)
{
    if (actualType == desiredType) {
        return true;
    }
    switch (actualType) {
        // TODO: int casts to float?
        // TODO: how to deal with over/underflow?
        case MetaType::INT8:
            return desiredType == MetaType::INT16 || desiredType == MetaType::INT32 ||
                   desiredType == MetaType::INT64 || MetaType::UINT64;
        case MetaType::INT16:
            return desiredType == MetaType::INT32 || desiredType == MetaType::INT64;
        case MetaType::INT32:
            return desiredType == MetaType::INT64;
        case MetaType::UINT8:
            return desiredType == MetaType::UINT16 || desiredType == MetaType::UINT32 ||
                   desiredType == MetaType::UINT64 || desiredType == MetaType::UINT64 ||
                   desiredType == MetaType::INT64;
        case MetaType::UINT16:
            return desiredType == MetaType::UINT32 || desiredType == MetaType::UINT64 ||
                   desiredType == MetaType::INT64;
        case MetaType::UINT32:
            return desiredType == MetaType::UINT64 || desiredType == MetaType::INT64;
        case MetaType::UINT64:
            return desiredType == MetaType::INT64;
        case MetaType::FLOAT32:
            return desiredType == MetaType::FLOAT64;
        default
          : /* fallthrough */
            ;
    }
    return false;
}

static bool isIntegral(const MetaType type)
{
    switch (type) {
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
            return true;
        default:
            return false;
    }
}

ImportSpec::ImportSpec(const string& appScope,
                       const string& appName,
                       const PrimitiveOperatorInstance& operatorIR)
  : _filterExpn(NULL)
  , _appScope(appScope)
  , _appName(appName)
  , _isNameBased(false)
  , _operatorIR(operatorIR)
  , _ee(new ExpressionEvaluator(CompilerConfiguration::instance()))
{
    _expn = NULL;
}

ImportSpec::ImportSpec(const string& appScope,
                       const string& appName,
                       const Expression& expn,
                       const PrimitiveOperatorInstance& operatorIR)
  : _filterExpn(NULL)
  , _appScope(appScope)
  , _appName(appName)
  , _isNameBased(false)
  , _operatorIR(operatorIR)
  , _ee(new ExpressionEvaluator(CompilerConfiguration::instance()))
{
    _expn = &expn.clone();
    checkSubscriptionExpression(*_expn, General);
    // Now hold on the the simplified version for ADL generation
    _expn = Expression::simplify(const_cast<Expression*>(_expn), *_ee);
}

ImportSpec::ImportSpec(const string& appScope,
                       const string& appName,
                       const Expression& expn,
                       const string& str,
                       const PrimitiveOperatorInstance& operatorIR)
  : _filterExpn(NULL)
  , _appScope(appScope)
  , _appName(appName)
  , _isNameBased(false)
  , _operatorIR(operatorIR)
  , _ee(new ExpressionEvaluator(CompilerConfiguration::instance()))
{
    _expn = convertFromString(str, expn);
    // Now hold on the the simplified version for ADL generation
    if (_expn) {
        _expn = Expression::simplify(const_cast<Expression*>(_expn), *_ee);
    }
}

ImportSpec::ImportSpec(const string& appScope,
                       const string& appName,
                       const string& streamId,
                       const PrimitiveOperatorInstance& operatorIR)
  : _expn(NULL)
  , _filterExpn(NULL)
  , _streamName(streamId)
  , _appScope(appScope)
  , _appName(appName)
  , _isNameBased(true)
  , _operatorIR(operatorIR)
  , _ee(new ExpressionEvaluator(CompilerConfiguration::instance()))
{}

ImportSpec::~ImportSpec()
{
    if (!_isNameBased) {
        delete _expn;
    }
    delete _filterExpn;
}

ostream& ImportSpec::print(ostream& s) const
{
    if (_isNameBased) {
        s << "Named import: " << _streamName << ", application : \"" << _appName << "\"";
    } else {
        s << "XPath import: ";
        if (_expn) {
            _expn->print(s, *_expn);
        }
        s << '\n';
    }
    if (!_appScope.empty()) {
        s << ", applicationScope : \"" << _appScope << "\"\n";
    } else {
        s << '\n';
    }
    if (_filterExpn) {
        s << ", filter: " << *_filterExpn << '\n';
    }
    return s;
}

const ImportSpec& ImportSpec::addFilter(ImportSpec& is,
                                        const string* filterString,
                                        const Expression* filter,
                                        const PrimitiveOperatorInstance& operatorIR)
{
    if (filterString) {
        is.addFilterExpression(*filterString, *filter, operatorIR);
    } else if (filter) {
        is.addFilterExpression(*filter, operatorIR);
    }
    return is;
}

void ImportSpec::addFilterExpression(const Expression& filterExpn,
                                     const PrimitiveOperatorInstance& operatorIR)
{
    _filterExpn = &filterExpn.clone();
    checkFilterExpression(*_filterExpn, General, operatorIR);
    // Now hold on the the simplified version for ADL generation
    _filterExpn = Expression::simplify(const_cast<Expression*>(_filterExpn), *_ee);
}

void ImportSpec::addFilterExpression(const string& str,
                                     const Expression& filterExpn,
                                     const PrimitiveOperatorInstance& operatorIR)
{
    _filterExpn = convertFilterFromString(str, filterExpn, operatorIR);
    // Now hold on the the simplified version for ADL generation
    if (_filterExpn) {
        _filterExpn = Expression::simplify(const_cast<Expression*>(_filterExpn), *_ee);
    }
}

const ImportSpec* ImportSpec::createFromImportOper(PrimitiveOperatorInstance const& importOp)
{
    const SourceLocation& loc = importOp.getLocation();
    const vector<Param*>& params = importOp.getParams();

    // Walk through the parameter list, checking for correctness
    uint32_t i, numParams = params.size();
    string appScope;
    string appName;
    bool appNameSpecified = false;
    bool subscriptionSpecified = false;
    const Expression* subscription = NULL;
    const Expression* filter = NULL;
    string subscriptionString;
    bool subscriptionStringExists = false;
    string filterString;
    bool filterStringExists = false;
    const string* filterStringPtr = NULL;
    SourceLocation appLoc;
    string streamId;
    bool streamIdSpecified = false;

    string singleString;

    for (i = 0; i < numParams; i++) {
        const Param& p = *params[i];
        const string& id = p.getName();

        if (id == "subscription") {
            subscriptionSpecified = true;
            subscription = p.getSingleExpression(id);
            subscriptionStringExists = p.getSingleString(id, subscriptionString);
            if (subscription && subscription->getType().getMetaType() == MetaType::RSTRING) {
                if (!subscriptionStringExists) {
                    SysOut::errorln(
                      SPL_CORE_IMPORT_SUBSCRIPTION_RUNTIME_STRING(id, subscription->toString()),
                      loc);
                    subscription = NULL;
                    break;
                }
            }
            if (!subscription) {
                SysOut::errorln(SPL_CORE_SINGLE_EXPRESSION_EXPECTED_IN_PARAMETER(id), loc);
            }
        } else if (id == "filter") {
            filter = p.getSingleExpression(id);
            filterStringExists = p.getSingleString(id, filterString);
            if (filterStringExists) {
                filterStringPtr = &filterString;
            } else {
                filterStringPtr = NULL;
            }
            if (filter && filter->getType().getMetaType() == MetaType::RSTRING) {
                if (!filterStringExists) {
                    SysOut::errorln(
                      SPL_CORE_IMPORT_SUBSCRIPTION_RUNTIME_STRING(id, filter->toString()), loc);
                    filterStringPtr = NULL;
                    filter = NULL;
                    break;
                }
            }
        } else if (id == "applicationName") {
            appNameSpecified = true;
            if (p.getSingleString(id, singleString)) {
                appName = Distillery::trim(singleString, " \t");
                static Distillery::RegEx appNameRE(
                  "^([_[:alpha:]][_[:alnum:]]*(\\.[_[:alpha:]][_[:alnum:]]*)*::)?"
                  "[_[:alpha:]][_[:alnum:]]*(\\.[_[:alpha:]][_[:alnum:]]*)*$");
                if (!appNameRE.match(appName)) {
                    SysOut::errorln(SPL_CORE_IMPORT_APP_NAME_INVALID_SYNTAX(appName), loc);
                }
            } else {
                // something went wrong when getting the single string param
                const Expression* e1 = p.getSingleExpression(id);
                // was it the number of params or the type of the param ?
                if (e1) {
                    SysOut::errorln(
                      SPL_CORE_SINGLE_STRING_LITERAL_EXPECTED_IN_PARAMETER(id, e1->toString()),
                      loc);
                } else {
                    SysOut::errorln(SPL_CORE_SINGLE_EXPRESSION_EXPECTED_IN_PARAMETER(id), loc);
                }
            }
        } else if (id == "applicationScope") {
            if (p.getSingleString(id, singleString))
                appScope = Distillery::trim(singleString, " \t");
            else {
                // something went wrong when getting the single string param
                const Expression* e1 = p.getSingleExpression(id);
                // was it the number of params or the type of the param ?
                if (e1) {
                    SysOut::errorln(
                      SPL_CORE_SINGLE_STRING_LITERAL_EXPECTED_IN_PARAMETER(id, e1->toString()),
                      loc);
                } else {
                    SysOut::errorln(SPL_CORE_SINGLE_EXPRESSION_EXPECTED_IN_PARAMETER(id), loc);
                }
            }
        } else if (id == "streamId") {
            streamIdSpecified = true;
            CompilationState& state = CompilationState::instance();
            state.push();
            state.setStringizeGetChannel();
            state.setStringizeGetMaxChannels();
            state.setStringizeGetLocalChannel();
            state.setStringizeGetLocalMaxChannels();
            bool brc = p.getSingleString(id, singleString);
            state.pop();
            if (brc) {
                streamId = Distillery::trim(singleString, " \t");
            } else {
                // something went wrong when getting the single string param
                const Expression* e1 = p.getSingleExpression(id);
                // was it the number of params or the type of the param ?
                if (e1) {
                    SysOut::errorln(
                      SPL_CORE_SINGLE_STRING_LITERAL_EXPECTED_IN_PARAMETER(id, e1->toString()),
                      loc);
                } else {
                    SysOut::errorln(SPL_CORE_SINGLE_EXPRESSION_EXPECTED_IN_PARAMETER(id), loc);
                }
            }
        }
    }

    if (streamIdSpecified) {
        if (subscriptionSpecified) {
            SysOut::errorln(SPL_CORE_STREAM_ID_AND_SUBSCRIPTION_ON_IMPORT, loc);
            return NULL;
        }
        if (!appNameSpecified) {
            SysOut::errorln(SPL_CORE_NAME_BASED_IMPORT_MISSING_APPLICATION, loc);
            return NULL;
        }
        const ImportSpec& spec = addFilter(*new ImportSpec(appScope, appName, streamId, importOp),
                                           filterStringPtr, filter, importOp);
        return &spec;
    }

    // no stream id was specified so we expect a subscription

    // If there is no subscription, and no streamID, we have an empty subscription
    if (appNameSpecified) {
        SysOut::warnln(SPL_CORE_SUBSCRIPTION_BASED_IMPORT_IGNORING_APPLICATION, loc);
    }

    if (!subscriptionSpecified) {
        const ImportSpec& spec = addFilter(*new ImportSpec(appScope, appName, importOp),
                                           filterStringPtr, filter, importOp);
        return &spec;
    }

    // Was something wrong with the subscription expression?
    if (!subscription) {
        return NULL;
    }

    if (subscriptionStringExists) {
        const ImportSpec& spec =
          addFilter(*new ImportSpec(appScope, appName, *subscription, subscriptionString, importOp),
                    filterStringPtr, filter, importOp);
        return &spec;
    }

    const ImportSpec& spec = addFilter(*new ImportSpec(appScope, appName, *subscription, importOp),
                                       filterStringPtr, filter, importOp);
    return &spec;
}

auto_ptr<const Expression> ImportSpec::simplify(const Expression& e) const
{
    return auto_ptr<const Expression>(Expression::simplify(&e.clone(), *_ee));
}

void ImportSpec::checkSubscriptionExpression(const Expression& e, Context context) const
{
    auto_ptr<const Expression> lhs;
    switch (context) {
        case SymbolMod:
            // Support Symbol % Const
            switch (e.getKind()) {
                case Expression::Symbol:
                case Expression::Subscript:
                    checkSubscriptionExpression(e, Symbol);
                    return;

                case Expression::Binary: {
                    const BinaryExpression& be = static_cast<const BinaryExpression&>(e);
                    const Expression& lhs2 = be.getLhs();
                    const Expression& rhs = be.getRhs();

                    checkSubscriptionExpression(lhs2, Symbol);
                    if (be.getOp() == Expression::mod) {
                        CompilationState& state = CompilationState::instance();
                        state.push();
                        state.setEvaluateUDPIntrinsicsAs64BitLiteral();
                        auto_ptr<const Expression> r(simplify(rhs));
                        state.pop();

                        const Expression* expr = r.get();
                        if (expr->getKind() != Expression::Lit) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_LITERAL(e.toString()),
                              e.getLocation());
                            return;
                        }

                        const SPL::Literal* l = expr->getLiteral();
                        if (l->getKind() != Literal::Primitive) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_INT64_LITERAL(e.toString()),
                              e.getLocation());
                            return;
                        }
                        const ExpressionValue& ev = l->primitive();
                        switch (ev.type()) {
                            case MetaType::INT8:
                            case MetaType::INT16:
                            case MetaType::INT32:
                            case MetaType::INT64:
                            case MetaType::UINT8:
                            case MetaType::UINT16:
                            case MetaType::UINT32:
                                break;
                            default:
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_INT64_LITERAL(e.toString()),
                                  e.getLocation());
                        }
                    }
                }
                    return;
                default:
                    break;
            }
            invalidExpn(e);
            return;
        case Symbol:
            // Support symbol[INT] or symbol
            switch (e.getKind()) {
                case Expression::Symbol:
                    break;
                case Expression::Subscript:
                    // check symbol & subscript for validity
                    {
                        const SubscriptExpression& se = static_cast<const SubscriptExpression&>(e);
                        lhs = simplify(se.getLhs());
                        if (lhs->getKind() != Expression::Symbol) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_SYMBOL(se.getLhs().toString()),
                              se.getLhs().getLocation());
                            return;
                        }
                        // we don't support slices
                        if (se.getSlice()) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_SYMBOL(e.toString()),
                              e.getLocation());
                            return;
                        }
                        // subscript better be a literal
                        const Expression& subscript = *se.lower();
                        auto_ptr<const Expression> rhs = simplify(subscript);
                        if (rhs->getKind() != Expression::Lit) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_LITERAL(subscript.toString()),
                              subscript.getLocation());
                            return;
                        }
                        // And it better be an integer literal
                        const SPL::Literal* l = rhs->getLiteral();
                        if (l->getKind() != Literal::Primitive) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_SUBSCRIPT_LITERAL_TYPE(
                                subscript.toString()),
                              subscript.getLocation());
                            return;
                        }
                        const ExpressionValue& ev = l->primitive();
                        switch (ev.type()) {
                            case MetaType::INT8:
                            case MetaType::INT16:
                            case MetaType::INT32:
                            case MetaType::INT64:
                            case MetaType::UINT8:
                            case MetaType::UINT16:
                            case MetaType::UINT32:
                            case MetaType::UINT64:
                                break;
                            default:
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_SUBSCRIPT_LITERAL_TYPE(
                                    subscript.toString()),
                                  subscript.getLocation());
                                return;
                        }
                    }
                    break;
                default:
                    SysOut::errorln(SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_SYMBOL(e.toString()),
                                    e.getLocation());
                    return;
            }
            break;
        case Literal: {
            // Only allowed are strings, float64, integer
            CompilationState& state = CompilationState::instance();
            state.push();
            state.setEvaluateUDPIntrinsicsAs64BitLiteral();
            lhs = simplify(e);
            state.pop();

            const Expression* expr = lhs.get();
            if (expr->getKind() != Expression::Lit) {
                SysOut::errorln(SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_LITERAL(e.toString()),
                                e.getLocation());
                return;
            }

            const SPL::Literal* l = expr->getLiteral();
            if (l->getKind() != Literal::Primitive) {
                SysOut::errorln(SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_LITERAL_TYPE(e.toString()),
                                e.getLocation());
                return;
            }
            const ExpressionValue& ev = l->primitive();
            switch (ev.type()) {
                case MetaType::RSTRING:
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
                    break;
                default:
                    SysOut::errorln(SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_LITERAL_TYPE(e.toString()),
                                    e.getLocation());
            }
        } break;
        case General: {
            if (e.getKind() != Expression::Binary) {
                invalidExpn(e);
                return;
            }

            const BinaryExpression& be = static_cast<const BinaryExpression&>(e);
            const Expression& lhs2 = be.getLhs();
            const Expression& rhs = be.getRhs();

            switch (be.getOp()) {
                case Expression::in:
                    checkSubscriptionExpression(rhs, Symbol);
                    checkSubscriptionExpression(lhs2, Literal);
                    break;
                case Expression::ampAmp:
                case Expression::barBar:
                    checkSubscriptionExpression(lhs2, General);
                    checkSubscriptionExpression(rhs, General);
                    break;
                case Expression::less:
                case Expression::leq:
                case Expression::eqEq:
                case Expression::greater:
                case Expression::geq:
                case Expression::neq:
                    checkSubscriptionExpression(lhs2, SymbolMod);
                    checkSubscriptionExpression(rhs, Literal);
                    break;
                default:
                    invalidExpn(e);
                    return;
            }
        } break;
        case SymbolIn:
            break;
    }
}

MetaType ImportSpec::checkFilterExpression(const Expression& e,
                                           Context context,
                                           const PrimitiveOperatorInstance& operatorIR) const
{
    auto_ptr<const Expression> lhs;
    switch (context) {
        case SymbolMod:
            // Support Symbol % Const
            switch (e.getKind()) {
                case Expression::Symbol:
                case Expression::Subscript:
                    return checkFilterExpression(e, Symbol, operatorIR);

                case Expression::Prefix: {
                    const PrefixExpression& pe = static_cast<const PrefixExpression&>(e);
                    const Expression& rhs = pe.getExpn(); // actually rhs;

                    MetaType mt = checkFilterExpression(rhs, SymbolMod, operatorIR);
                    if (mt == MetaType::INVALID) {
                        return mt;
                    }

                    if (pe.getOp() == Expression::tilde) {
                        if (!isIntegral(mt)) {
                            return MetaType::INVALID;
                        }
                        return mt;
                    }

                    return MetaType::INVALID;
                }

                case Expression::Binary: {
                    const BinaryExpression& be = static_cast<const BinaryExpression&>(e);
                    const Expression& lhs2 = be.getLhs();
                    const Expression& rhs = be.getRhs();

                    MetaType mt = checkFilterExpression(lhs2, SymbolMod, operatorIR);
                    if (mt == MetaType::INVALID) {
                        return mt;
                    }

                    Expression::Op const& binOp = be.getOp();
                    if (binOp == Expression::mod || binOp == Expression::star ||
                        binOp == Expression::slash || binOp == Expression::plus ||
                        binOp == Expression::minus || binOp == Expression::amp ||
                        binOp == Expression::bar || binOp == Expression::hat ||
                        binOp == Expression::lShift || binOp == Expression::rShift) {

                        // Support a call to getChannel() or getMaxChannels
                        CompilationState& state = CompilationState::instance();
                        state.push();
                        state.setEvaluateUDPIntrinsicsAs64BitLiteral();
                        auto_ptr<const Expression> r(simplify(rhs));
                        state.pop();

                        const Expression* expr = r.get();
                        if (expr->getKind() != Expression::Lit) {
                            SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_LITERAL(e.toString()),
                                            e.getLocation());
                            return MetaType::INVALID;
                        }

                        const SPL::Literal* l = expr->getLiteral();
                        if (l->getKind() != Literal::Primitive) {
                            // TODO review message
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_FILTER_INT64_LITERAL(e.toString()),
                              e.getLocation());
                            return MetaType::INVALID;
                        }
                        const ExpressionValue& ev = l->primitive();
                        switch (ev.type()) {
                            case MetaType::INT8:
                            case MetaType::INT16:
                            case MetaType::INT32:
                            case MetaType::INT64:
                            case MetaType::UINT8:
                            case MetaType::UINT16:
                            case MetaType::UINT32:
                            case MetaType::UINT64:
                                break;
                            default:
                                // TODO review message
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_FILTER_INT64_LITERAL(expr->toString()),
                                  expr->getLocation());
                        }

                        // Symbol needs to be integer
                        if (!isIntegral(mt)) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_FILTER_MOD_SYMBOL(lhs2.toString()),
                              lhs2.getLocation());
                            return MetaType::INVALID;
                        }
                    }
                }
                    return MetaType::INT64;
                default:
                    break;
            }
            invalidFilterExpn(e);
            return MetaType::INVALID;
        case Symbol:
            // Support symbol[INT] or symbol
            switch (e.getKind()) {
                case Expression::Symbol: {
                    MetaType mt = getMetaType(e, operatorIR);
                    switch (mt) {
                        case MetaType::INT64:
                        case MetaType::INT32:
                        case MetaType::INT16:
                        case MetaType::INT8:
                        case MetaType::UINT64:
                        case MetaType::UINT32:
                        case MetaType::UINT16:
                        case MetaType::UINT8:
                        case MetaType::FLOAT64:
                        case MetaType::FLOAT32:
                        case MetaType::RSTRING:
                        case MetaType::BOOLEAN:
                            return mt;
                        case MetaType::INVALID: // error case
                            break;
                        default:;
                    }
                    SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SYMBOL_TYPE(e.toString()),
                                    e.getLocation());
                    return MetaType::INVALID;
                } break;
                case Expression::Subscript:
                    // check symbol & subscript for validity
                    {
                        const SubscriptExpression& se = static_cast<const SubscriptExpression&>(e);
                        lhs = simplify(se.getLhs());
                        if (lhs->getKind() != Expression::Symbol) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_FILTER_SYMBOL(se.getLhs().toString()),
                              se.getLhs().getLocation());
                            return MetaType::INVALID;
                        }
                        // Use check above
                        MetaType mt = getElementMetaType(*lhs, operatorIR);
                        switch (mt) {
                            case MetaType::INT64:
                            case MetaType::INT32:
                            case MetaType::INT16:
                            case MetaType::INT8:
                            case MetaType::UINT64:
                            case MetaType::UINT32:
                            case MetaType::UINT16:
                            case MetaType::UINT8:
                            case MetaType::FLOAT64:
                            case MetaType::FLOAT32:
                            case MetaType::RSTRING:
                            case MetaType::BOOLEAN:
                                break;
                            case MetaType::INVALID: // error case
                                return mt;
                            default:
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_FILTER_SUBSCRIPT_SYMBOL_TYPE(
                                    e.toString()),
                                  e.getLocation());
                                return MetaType::INVALID;
                        }

                        // we don't support slices
                        if (se.getSlice()) {
                            SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SYMBOL(e.toString()),
                                            e.getLocation());
                            return MetaType::INVALID;
                        }
                        // subscript better be a literal
                        const Expression& subscript = *se.lower();
                        auto_ptr<const Expression> rhs = simplify(subscript);
                        if (rhs->getKind() != Expression::Lit) {
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_FILTER_LITERAL(subscript.toString()),
                              subscript.getLocation());
                            return MetaType::INVALID;
                        }
                        // And it better be an integer literal
                        const SPL::Literal* l = rhs->getLiteral();
                        if (l->getKind() != Literal::Primitive) {
                            SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SUBSCRIPT_LITERAL_TYPE(
                                              subscript.toString()),
                                            subscript.getLocation());
                            return MetaType::INVALID;
                        }
                        const ExpressionValue& ev = l->primitive();
                        switch (ev.type()) {
                            case MetaType::INT8:
                            case MetaType::INT16:
                            case MetaType::INT32:
                            case MetaType::INT64:
                            case MetaType::UINT8:
                            case MetaType::UINT16:
                            case MetaType::UINT32:
                            case MetaType::UINT64:
                                return mt;
                            default:
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_FILTER_SUBSCRIPT_LITERAL_TYPE(
                                    subscript.toString()),
                                  subscript.getLocation());
                                return MetaType::INVALID;
                        }
                    }
                    break;
                default:
                    SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SYMBOL(e.toString()),
                                    e.getLocation());
                    return MetaType::INVALID;
            }
            break;
        case SymbolIn:
            // Support symbol
            switch (e.getKind()) {
                case Expression::Symbol:
                    return getMetaType(e, operatorIR);
                default:
                    SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SYMBOL(e.toString()),
                                    e.getLocation());
                    return MetaType::INVALID;
            }
            break;
        case Literal: {
            // Only allowed are strings, float64, integer, and function calls getMaxChannels, getChannel
            CompilationState& state = CompilationState::instance();
            state.push();
            state.setEvaluateUDPIntrinsicsAs64BitLiteral();
            lhs = simplify(e);
            state.pop();

            const Expression* expr = lhs.get();
            if (expr->getKind() != Expression::Lit) {
                SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_LITERAL(e.toString()),
                                e.getLocation());
                return MetaType::INVALID;
            }

            const SPL::Literal* l = expr->getLiteral();
            if (l->getKind() != Literal::Primitive) {
                SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_LITERAL_TYPE(e.toString()),
                                e.getLocation());
                return MetaType::INVALID;
            }
            const ExpressionValue& ev = l->primitive();
            switch (ev.type()) {
                case MetaType::RSTRING:
                    return MetaType::RSTRING;
                case MetaType::INT8:
                case MetaType::INT16:
                case MetaType::INT32:
                case MetaType::INT64:
                case MetaType::UINT8:
                case MetaType::UINT16:
                case MetaType::UINT32:
                case MetaType::UINT64:
                    return MetaType::INT64;
                case MetaType::FLOAT32:
                case MetaType::FLOAT64:
                    return MetaType::FLOAT64;
                case MetaType::BOOLEAN:
                    return MetaType::BOOLEAN;
                default:
                    SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_LITERAL_TYPE(e.toString()),
                                    e.getLocation());
                    return MetaType::INVALID;
            }
        } break;
        case General: {
            switch (e.getKind()) {
                case Expression::Binary: {
                    const BinaryExpression& be = static_cast<const BinaryExpression&>(e);
                    const Expression& lhs2 = be.getLhs();
                    const Expression& rhs = be.getRhs();

                    MetaType rmt, lmt;
                    switch (be.getOp()) {
                        case Expression::in:
                            rmt = checkFilterExpression(rhs, SymbolIn, operatorIR);
                            if (rmt != MetaType::INVALID && rmt != MetaType::LIST) {
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_FILTER_NOT_LIST(rhs.toString()),
                                  e.getLocation());
                                rmt = MetaType::INVALID;
                            } else {
                                rmt = getElementMetaType(rhs, operatorIR);
                            }
                            lmt = checkFilterExpression(lhs2, Literal, operatorIR);
                            if (rmt == MetaType::INVALID || lmt == MetaType::INVALID) {
                                return MetaType::INVALID;
                            }
                            if (rmt != lmt && !(canCast(rmt, lmt))) {
                                SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_MISMATCH_TYPE(
                                                  "in", lhs2.toString(), rhs.toString()),
                                                e.getLocation());
                                return MetaType::INVALID;
                            }
                            return MetaType::BOOLEAN;
                        case Expression::ampAmp:
                        case Expression::barBar:
                            rmt = checkFilterExpression(lhs2, General, operatorIR);
                            lmt = checkFilterExpression(rhs, General, operatorIR);
                            if (rmt == MetaType::INVALID || lmt == MetaType::INVALID) {
                                return MetaType::INVALID;
                            }
                            if (rmt != MetaType::BOOLEAN) {
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_FILTER_NEED_BOOL(
                                    Expression::opAsString(be.getOp()), rmt.toString()),
                                  rhs.getLocation());
                                return MetaType::INVALID;
                            }
                            if (lmt != MetaType::BOOLEAN) {
                                SysOut::errorln(
                                  SPL_CORE_IMPORT_INVALID_FILTER_NEED_BOOL(
                                    Expression::opAsString(be.getOp()), lmt.toString()),
                                  lhs2.getLocation());
                                return MetaType::INVALID;
                            }
                            return MetaType::BOOLEAN;

                        case Expression::less:
                        case Expression::leq:
                        case Expression::eqEq:
                        case Expression::greater:
                        case Expression::geq:
                        case Expression::neq:
                            lmt = checkFilterExpression(lhs2, SymbolMod, operatorIR);
                            rmt = checkFilterExpression(rhs, Literal, operatorIR);
                            if (rmt == MetaType::INVALID || lmt == MetaType::INVALID)
                                return MetaType::INVALID;
                            if (rmt == MetaType::INVALID || lmt == MetaType::INVALID) {
                                return MetaType::INVALID;
                            }

                            if (rmt != lmt && !canCast(lmt, rmt)) {
                                SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_MISMATCH_TYPE(
                                                  Expression::opAsString(be.getOp()),
                                                  lhs2.toString(), rhs.toString()),
                                                e.getLocation());
                                return MetaType::INVALID;
                            }

                            // Boolean literals can be used with only
                            // == and != opeators.  There is no such thing
                            // as x < true, even if x is boolean.
                            if (rmt == MetaType::BOOLEAN) {
                                if ((be.getOp() != Expression::eqEq) &&
                                    (be.getOp() != Expression::neq)) {
                                    SysOut::errorln(SPL_CORE_IMPORT_INVALID_BOOLEAN_OPERATOR(
                                                      Expression::opAsString(be.getOp())),
                                                    e.getLocation());
                                    return MetaType::INVALID;
                                }
                                // else fall through to return BOOLEAN
                            }
                            return MetaType::BOOLEAN;
                        default:
                            invalidFilterExpn(e);
                            return MetaType::INVALID;
                    }

                    break; // unreachable?
                }
                case Expression::Prefix: {
                    const PrefixExpression& pe = static_cast<const PrefixExpression&>(e);
                    const Expression& expn = pe.getExpn();
                    lhs = simplify(expn); // really rhs

                    switch (lhs->getKind()) {

                        case Expression::Subscript:

                            // We are good if the op is bang and the expression type
                            // is boolean
                            if (pe.getOp() == Expression::bang) {

                                MetaType type = checkFilterExpression(*lhs, Symbol, operatorIR);
                                if (type != MetaType::BOOLEAN) {
                                    return MetaType::INVALID;
                                }
                                return MetaType::BOOLEAN;
                            }

                            // not bang
                            return MetaType::INVALID;

                        case Expression::Symbol:
                            // We are good if the op is bang and the expression type
                            // is boolean
                            if (pe.getOp() == Expression::bang) {
                                MetaType type = getElementMetaType(*lhs, operatorIR);
                                if (type != MetaType::BOOLEAN) {
                                    SysOut::errorln(
                                      SPL_CORE_IMPORT_INVALID_FILTER_NEED_BOOL(
                                        Expression::opAsString(pe.getOp()), type.toString()),
                                      e.getLocation());
                                    return MetaType::INVALID;
                                }

                                return MetaType::BOOLEAN;
                            }

                            // We are good if the op is ~ and the expression type
                            // is integral
                            if (pe.getOp() == Expression::tilde) {
                                MetaType type = getElementMetaType(*lhs, operatorIR);
                                if (!isIntegral(type)) {
                                    return MetaType::INVALID;
                                }
                                return type;
                            }
                            // not bang or tilde
                            return MetaType::INVALID;

                        default:
                            return MetaType::INVALID;
                    }
                }
                case Expression::Symbol: {
                    // We only allow this only for boolean symbols
                    MetaType type = getElementMetaType(e, operatorIR);
                    if (type == MetaType::BOOLEAN) {
                        return MetaType::BOOLEAN;
                    }
                    SysOut::errorln(
                      SPL_CORE_IMPORT_INVALID_FILTER_NEED_BOOL(e.toString(), type.toString()),
                      e.getLocation());
                    return MetaType::INVALID;
                }
                case Expression::Subscript: {
                    // We allow this only for boolean symbols
                    const SubscriptExpression& se = static_cast<const SubscriptExpression&>(e);
                    lhs = simplify(se.getLhs());
                    if (lhs->getKind() != Expression::Symbol) {
                        SysOut::errorln(
                          SPL_CORE_IMPORT_INVALID_FILTER_SYMBOL(se.getLhs().toString()),
                          se.getLhs().getLocation());
                        return MetaType::INVALID;
                    }
                    // Use check above
                    MetaType mt = getElementMetaType(*lhs, operatorIR);
                    switch (mt) {
                        case MetaType::BOOLEAN:
                            break;
                        case MetaType::INVALID: // error case
                            return mt;
                        default:
                            SysOut::errorln(
                              SPL_CORE_IMPORT_INVALID_FILTER_SUBSCRIPT_SYMBOL_TYPE(e.toString()),
                              e.getLocation());
                            return MetaType::INVALID;
                    }

                    // we don't support slices
                    if (se.getSlice()) {
                        SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SYMBOL(e.toString()),
                                        e.getLocation());
                        return MetaType::INVALID;
                    }
                    // subscript better be a literal
                    const Expression& subscript = *se.lower();
                    auto_ptr<const Expression> rhs = simplify(subscript);
                    if (rhs->getKind() != Expression::Lit) {
                        SysOut::errorln(
                          SPL_CORE_IMPORT_INVALID_FILTER_LITERAL(subscript.toString()),
                          subscript.getLocation());
                        return MetaType::INVALID;
                    }
                    // And it better be an integer literal
                    const SPL::Literal* l = rhs->getLiteral();
                    if (l->getKind() != Literal::Primitive) {
                        SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SUBSCRIPT_LITERAL_TYPE(
                                          subscript.toString()),
                                        subscript.getLocation());
                        return MetaType::INVALID;
                    }
                    const ExpressionValue& ev = l->primitive();
                    switch (ev.type()) {
                        case MetaType::INT8:
                        case MetaType::INT16:
                        case MetaType::INT32:
                        case MetaType::INT64:
                        case MetaType::UINT8:
                        case MetaType::UINT16:
                        case MetaType::UINT32:
                        case MetaType::UINT64:
                            return mt;
                        default:
                            SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_SUBSCRIPT_LITERAL_TYPE(
                                              subscript.toString()),
                                            subscript.getLocation());
                            return MetaType::INVALID;
                    }

                    break;
                }
                default:
                    invalidFilterExpn(e);
                    break;
            }

        } break;
    }
    return MetaType::INVALID;
}

MetaType ImportSpec::getMetaType(const Expression& expn,
                                 const PrimitiveOperatorInstance& operatorIR) const
{
    // Check that the symbol exists in the output tuple, and that it has a valid type
    // float64/integer/rstring
    const PrimitiveOperatorInstance::Port& oPort = operatorIR.getOutputPort(0);
    const TupleTyp* oTuple = oPort.getTuple();
    assert(oTuple);
    const SymbolExpression& sym = expn.as<SymbolExpression>();
    const TupleTyp::Name2Index& tupleNames = oTuple->getAttributeNames();
    TupleTyp::Name2Index::const_iterator it = tupleNames.find(sym.getId());
    if (it == tupleNames.end()) {
        // Not in output tuple
        SysOut::errorln(SPL_CORE_IMPORT_FILTER_SYMBOL_NOT_IN_OUTPUT(sym.getId()),
                        expn.getLocation());
        return MetaType::INVALID;
    }
    // return the meta-type of the symbol
    return oTuple->getAttributeType(it->second).getMetaType();
}

MetaType ImportSpec::getElementMetaType(const Expression& expn,
                                        const PrimitiveOperatorInstance& operatorIR) const
{
    // Check that the symbol exists in the output tuple, and that it has a valid type
    // list<float64/integer/rstring>
    const PrimitiveOperatorInstance::Port& oPort = operatorIR.getOutputPort(0);
    const TupleTyp* oTuple = oPort.getTuple();
    assert(oTuple);
    const SymbolExpression& sym = expn.as<SymbolExpression>();
    const TupleTyp::Name2Index& tupleNames = oTuple->getAttributeNames();
    TupleTyp::Name2Index::const_iterator it = tupleNames.find(sym.getId());
    if (it == tupleNames.end()) {
        // Not in output tuple
        SysOut::errorln(SPL_CORE_IMPORT_FILTER_SYMBOL_NOT_IN_OUTPUT(sym.getId()),
                        expn.getLocation());
        return MetaType::INVALID;
    }
    // return the meta-type of the element type
    const RootTyp& rt = oTuple->getAttributeType(it->second);
    MetaType mt = rt.getMetaType();
    if (mt != MetaType::LIST) {
        return mt;
    }
    // It IS a list!
    const ListTyp& lt = rt.as<ListTyp>();
    return lt.getElementType().getMetaType();
}

void ImportSpec::invalidExpn(const Expression& e) const
{
    // Is this the whole expression?
    if (NULL == _expn || &e == _expn) {
        SysOut::errorln(SPL_CORE_IMPORT_INVALID_SUBSCRIPTION_EXPN(e.toString()), e.getLocation());
    } else {
        SysOut::errorln(
          SPL_CORE_IMPORT_INVALID_SIMPLIFIED_SUBSCRIPTION_EXPN(e.toString(), _expn->toString()),
          e.getLocation());
    }
}

void ImportSpec::invalidFilterExpn(const Expression& e) const
{
    // Is this the whole expression?
    if (NULL == _filterExpn || &e == _filterExpn) {
        SysOut::errorln(SPL_CORE_IMPORT_INVALID_FILTER_EXPN(e.toString()), e.getLocation());
    } else {
        SysOut::errorln(
          SPL_CORE_IMPORT_INVALID_SIMPLIFIED_FILTER_EXPN(e.toString(), _filterExpn->toString()),
          e.getLocation());
    }
}

static Expression& convertOne(SubscriptionExpression& s, const SourceLocation& loc);

static Expression& convertOne(SubscriptionExpression& clause,
                              const SourceLocation& loc,
                              Expression::Op op)
{
    // We know that we have at least 2 subclauses
    const RootTyp& t = TypeFactory::instance().primitiveT(MetaType::INT32);
    Expression* e = new BinaryExpression(t, loc, op, convertOne(*clause.getSubclause(0), loc),
                                         convertOne(*clause.getSubclause(1), loc));
    for (uint32_t i = 2, n = clause.getSize(); i < n; i++) {
        e = new BinaryExpression(t, loc, op, *e, convertOne(*clause.getSubclause(i), loc));
    }
    return *e;
}

static Expression& convertOneClause(SubscriptionExpression& clause,
                                    const SourceLocation& loc,
                                    Expression::Op op)
{
    uint32_t size = clause.getSize();
    if (size == 1) {
        return convertOne(*clause.getSubclause(0), loc);
    }

    return convertOne(clause, loc, op);
}

static Expression& convertPredicate(SubscriptionExpression& s, const SourceLocation& loc)
{
    // This is a predicate
    SubscriptionExpression::Op subscriptionOp = s.getOp();
    const SubscriptionExpression::Literal& lit = s.getLiteral();
    const string& sym = s.getSymbol();
    bool hasSubscript = s.hasSubscript();
    int64 subscript;
    if (hasSubscript) {
        subscript = s.getSubscript();
    }
    bool hasModuloDivisor = s.hasModuloDivisor();
    int64 moduloDivisor;
    if (hasModuloDivisor) {
        moduloDivisor = s.getModuloDivisor();
    }

    Expression::Op op = Expression::eqEq;
    switch (subscriptionOp) {
        case SubscriptionExpression::LessThan:
            op = Expression::less;
            break;
        case SubscriptionExpression::LessEqual:
            op = Expression::leq;
            break;
        case SubscriptionExpression::GreaterThan:
            op = Expression::greater;
            break;
        case SubscriptionExpression::GreaterEqual:
            op = Expression::geq;
            break;
        case SubscriptionExpression::Equal:
            op = Expression::eqEq;
            break;
        case SubscriptionExpression::NotEqual:
            op = Expression::neq;
            break;
        case SubscriptionExpression::In:
            op = Expression::in;
            break;
    }

    const RootTyp& t = TypeFactory::instance().primitiveT(MetaType::INT64);

    // sym
    Expression* e = new SymbolExpression(t, loc, sym, Expression::Other);

    // sym[int64]
    if (hasSubscript) {
        ExpressionValue* ev = new ExpressionValue(subscript);
        Literal* l = new Literal(t, ev);
        e = new SubscriptExpression(t, loc, *e, false, new LiteralExpression(t, loc, *l), NULL);
    }

    // sym % int64
    // TODO
    if (hasModuloDivisor) {
        ExpressionValue* ev = new ExpressionValue(moduloDivisor);
        Literal* l = new Literal(t, ev);
        e = new BinaryExpression(t, loc, Expression::mod, *e, *new LiteralExpression(t, loc, *l));
    }

    // Fix the literal
    Literal* l = NULL;
    switch (lit.getKind()) {
        case SubscriptionExpression::Literal::RString: {
            rstring str = lit.getRStringValue();
            l = new Literal(t, new ExpressionValue(str));
            break;
        }
        case SubscriptionExpression::Literal::Int64: {
            int64 i = lit.getInt64Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case SubscriptionExpression::Literal::Float64: {
            float64 f = lit.getFloat64Value();
            l = new Literal(t, new ExpressionValue(f));
            break;
        }
    }

    // Now the operation
    if (op == Expression::in) {
        e = new BinaryExpression(t, loc, op, *new LiteralExpression(t, loc, *l), *e);
    } else {
        e = new BinaryExpression(t, loc, op, *e, *new LiteralExpression(t, loc, *l));
    }

    // All done!
    return *e;
}

static Expression& convertOne(SubscriptionExpression& s, const SourceLocation& loc)
{
    switch (s.getKind()) {
        case SubscriptionExpression::AndClause:
            return convertOneClause(s, loc, Expression::ampAmp);
            break;
        case SubscriptionExpression::OrClause:
            return convertOneClause(s, loc, Expression::barBar);
            break;
        case SubscriptionExpression::Predicate:
            break;
    }
    return convertPredicate(s, loc);
}

Expression* ImportSpec::convertFromString(const string& expn, const Expression& e)
{
    SubscriptionExpressionPtr s = SubscriptionExpression::fromString(expn);
    const SourceLocation& loc = e.getLocation();
    if (s.get() == NULL) {
        SysOut::errorln(SPL_CORE_IMPORT_SUBSCRIPTION_INVALID(e.toString()), loc);
        return NULL;
    }

    if (s->getKind() == SubscriptionExpression::OrClause && s->getSize() == 0) {
        // there was nothing to convert
        SysOut::errorln(SPL_CORE_IMPORT_SUBSCRIPTION_INVALID(e.toString()), loc);
        return NULL;
    }

    // It is a valid expression.   Now convert to an Expression.
    return &convertOne(*s, loc);
}

static Expression& convertOne(FilterExpression& s, const SourceLocation& loc);

static Expression& convertOne(FilterExpression& clause,
                              const SourceLocation& loc,
                              Expression::Op op)
{
    // We know that we have at least 2 subclauses
    const RootTyp& t = TypeFactory::instance().primitiveT(MetaType::INT32);
    Expression* e = new BinaryExpression(t, loc, op, convertOne(*clause.getSubclause(0), loc),
                                         convertOne(*clause.getSubclause(1), loc));
    for (uint32_t i = 2, n = clause.getSize(); i < n; i++) {
        e = new BinaryExpression(t, loc, op, *e, convertOne(*clause.getSubclause(i), loc));
    }
    return *e;
}

static Expression& convertOneClause(FilterExpression& clause,
                                    const SourceLocation& loc,
                                    Expression::Op op)
{
    uint32_t size = clause.getSize();
    if (size == 1) {
        return convertOne(*clause.getSubclause(0), loc);
    }

    return convertOne(clause, loc, op);
}

static Expression& convertPredicate(FilterExpression& s, const SourceLocation& loc)
{
    // This is a predicate
    FilterExpression::Op filterOp = s.getOp();
    const FilterExpression::Literal& lit = s.getLiteral();
    const string& sym = s.getSymbol();
    bool hasSubscript = s.hasSubscript();
    int64 subscript;
    if (hasSubscript) {
        subscript = s.getSubscript();
    }
    /*
    bool hasModuloDivisor = s.hasModuloDivisor();
    int64 moduloDivisor;
    if (hasModuloDivisor) {
        moduloDivisor = s.getModuloDivisor().getInt64Value();
    }
    */
    size_t arithmeticOpCount = s.getArithmeticOperationsCount();

    Expression::Op op = Expression::eqEq;
    switch (filterOp) {
        case FilterExpression::LessThan:
            op = Expression::less;
            break;
        case FilterExpression::LessEqual:
            op = Expression::leq;
            break;
        case FilterExpression::GreaterThan:
            op = Expression::greater;
            break;
        case FilterExpression::GreaterEqual:
            op = Expression::geq;
            break;
        case FilterExpression::Equal:
            op = Expression::eqEq;
            break;
        case FilterExpression::NotEqual:
            op = Expression::neq;
            break;
        case FilterExpression::In:
            op = Expression::in;
            break;

        // We shouldn't get here
        // We are going to treat this expression as equivant to == true.
        case FilterExpression::None:
            op = Expression::eqEq;
            break;
    }

    const RootTyp& t = TypeFactory::instance().primitiveT(MetaType::INT64);

    // sym
    Expression* e = new SymbolExpression(t, loc, sym, Expression::Other);

    // sym[int64]
    if (hasSubscript) {
        ExpressionValue* ev = new ExpressionValue(subscript);
        Literal* l = new Literal(t, ev);
        e = new SubscriptExpression(t, loc, *e, false, new LiteralExpression(t, loc, *l), NULL);
    }

    for (size_t opIndex = 0; opIndex < arithmeticOpCount; ++opIndex) {
        // Build the whole tree
        FilterExpression::ArithmeticOperation const& arithmeticOperation =
          s.getArithmeticOperation(opIndex);
        Expression::Op arithOp = Expression::INVALID;
        switch (arithmeticOperation.getOperator()) {
            // Binary operations
            case FilterExpression::Mod:
                arithOp = Expression::mod;
                break;
            case FilterExpression::BitOr:
                arithOp = Expression::bar;
                break;
            case FilterExpression::BitAnd:
                arithOp = Expression::amp;
                break;
            case FilterExpression::BitXor:
                arithOp = Expression::hat;
                break;
            case FilterExpression::LeftShift:
                arithOp = Expression::lShift;
                break;
            case FilterExpression::RightShift:
                arithOp = Expression::rShift;
                break;
            case FilterExpression::Add:
                arithOp = Expression::plus;
                break;
            case FilterExpression::Sub:
                arithOp = Expression::minus;
                break;
            case FilterExpression::Mul:
                arithOp = Expression::star;
                break;
            case FilterExpression::Div:
                arithOp = Expression::slash;
                break;

            // Unary
            case FilterExpression::BNot:
                arithOp = Expression::tilde;
                break;

            // noop (should not happen)
            case FilterExpression::NoArithmeticOp:
                arithOp = Expression::INVALID;
                break;
                // TODO Handle unary operations correctly.
        }
        ExpressionValue* ev = new ExpressionValue(arithmeticOperation.getOperand());
        Literal* l = new Literal(t, ev);
        e = new BinaryExpression(t, loc, arithOp, *e, *new LiteralExpression(t, loc, *l));
    }

    // Fix the literal
    Literal* l = NULL;
    switch (lit.getKind()) {
        case FilterExpression::Literal::RString: {
            rstring str = lit.getRStringValue();
            l = new Literal(t, new ExpressionValue(str));
            break;
        }
        case FilterExpression::Literal::Int64: {
            int64 i = lit.getInt64Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::Int32: {
            int32 i = lit.getInt32Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::Int16: {
            int32 i = lit.getInt16Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::Int8: {
            int32 i = lit.getInt8Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::UInt64: {
            uint32 i = lit.getUInt64Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::UInt32: {
            uint32 i = lit.getUInt32Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::UInt16: {
            uint16 i = lit.getUInt16Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::UInt8: {
            uint8 i = lit.getUInt8Value();
            l = new Literal(t, new ExpressionValue(i));
            break;
        }
        case FilterExpression::Literal::Float64: {
            float64 f = lit.getFloat64Value();
            l = new Literal(t, new ExpressionValue(f));
            break;
        }
        case FilterExpression::Literal::Float32: {
            float32 f = lit.getFloat32Value();
            l = new Literal(t, new ExpressionValue(f));
            break;
        }
        case FilterExpression::Literal::Boolean: {
            boolean b = lit.getBooleanValue();
            l = new Literal(t, new ExpressionValue(b));
            break;
        }
    }

    // Now the operation
    if (op == Expression::in) {
        e = new BinaryExpression(t, loc, op, *new LiteralExpression(t, loc, *l), *e);
    } else {
        e = new BinaryExpression(t, loc, op, *e, *new LiteralExpression(t, loc, *l));
    }

    // All done!
    return *e;
}

static Expression& convertNotClause(FilterExpression& clause,
                                    const SourceLocation& loc,
                                    Expression::Op op)
{
    Expression& subExpression = convertOne(*clause.getSubclause(0), loc);
    const RootTyp& type = TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
    Expression* e = new PrefixExpression(type, loc, op, subExpression);
    return *e;
}

static Expression& convertOne(FilterExpression& s, const SourceLocation& loc)
{
    switch (s.getKind()) {
        case FilterExpression::AndClause:
            return convertOneClause(s, loc, Expression::ampAmp);
        case FilterExpression::OrClause:
            return convertOneClause(s, loc, Expression::barBar);
        case FilterExpression::NotClause:
            return convertNotClause(s, loc, Expression::bang);
        case FilterExpression::Predicate:
            break;
    }
    return convertPredicate(s, loc);
}

struct TupleTypValidator : FilterExpression::NameValidator
{
    TupleTypValidator(const TupleTyp& tuple)
      : _tuple(tuple)
      , _nameMap(tuple.getAttributeNames())
    {}

    virtual Meta::Type validate(const string& name, bool isList, uint32_t& index) const;

  private:
    const TupleTyp& _tuple;
    const std::tr1::unordered_map<string, uint32_t>& _nameMap;
};

Meta::Type TupleTypValidator::validate(const string& name, bool isList, uint32_t& index) const
{
    TupleTyp::Name2Index::const_iterator it = _nameMap.find(name);
    if (it == _nameMap.end()) {
        return Meta::Type::INVALID; // no such attribute
    }

    index = it->second;
    const RootTyp& rt = _tuple.getAttributeType(index);
    MetaType mt = rt.getMetaType();
    if (isList) {
        if (mt != MetaType::LIST) {
            return Meta::Type::INVALID;
        }
        const ListTyp& lt = rt.as<ListTyp>();
        mt = lt.getElementType().getMetaType();
    }
    switch (mt) {
        case MetaType::INT64:
            return Meta::Type::INT64;
        case MetaType::RSTRING:
            return Meta::Type::RSTRING;
        case MetaType::FLOAT64:
            return Meta::Type::FLOAT64;
        default:;
    }
    return Meta::Type::INVALID;
}

Expression* ImportSpec::convertFilterFromString(const string& str,
                                                const Expression& filterExpn,
                                                const PrimitiveOperatorInstance& operatorIR)
{
    const PrimitiveOperatorInstance::Port& oPort = operatorIR.getOutputPort(0);
    const TupleTyp* oTuple = oPort.getTuple();
    assert(oTuple);
    TupleTypValidator validator(*oTuple);
    FilterExpressionPtr f = FilterExpression::fromString(validator, str);
    const SourceLocation& loc = filterExpn.getLocation();
    if (f.get() == NULL) {
        SysOut::errorln(SPL_CORE_IMPORT_FILTER_INVALID(filterExpn.toString()), loc);
        return NULL;
    }

    if (f->getKind() == FilterExpression::OrClause && f->getSize() == 0) {
        // there was nothing to convert
        SysOut::errorln(SPL_CORE_IMPORT_FILTER_INVALID(filterExpn.toString()), loc);
        return NULL;
    }

    // It is a valid expression.   Now convert to an Expression.
    return &convertOne(*f, loc);
}
