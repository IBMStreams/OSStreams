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

#include <SPL/Runtime/Operator/Port/FilterEvaluator.h>

using namespace std;
using namespace SPL;
using namespace SPL::Filter;

AndClause::~AndClause()
{
    vector<FilterClause*>::const_iterator it;
    for (it = _clauses.begin(); it != _clauses.end(); it++) {
        delete *it;
    }
}

void AndClause::evaluate(FilterClauseContext& context) const
{
    context.isComplete = false;
    vector<FilterClause*>::const_iterator it;
    for (it = _clauses.begin(); it != _clauses.end(); it++) {
        (*it)->evaluate(context);
        if (context.isComplete) {
            return;
        }
        if (!(context.result.getBooleanValue())) {
            context.result = false;
            return;
        }
    }
    context.result = true;
}

OrClause::~OrClause()
{
    vector<FilterClause*>::const_iterator it;
    for (it = _clauses.begin(); it != _clauses.end(); it++) {
        delete *it;
    }
}

void OrClause::evaluate(FilterClauseContext& context) const
{
    context.isComplete = false;
    vector<FilterClause*>::const_iterator it;
    for (it = _clauses.begin(); it != _clauses.end(); it++) {
        (*it)->evaluate(context);
        if (context.isComplete) {
            return;
        }
        if (context.result.getBooleanValue()) {
            context.result = true;
            return;
        }
    }
    context.result = false;
}

NotClause::~NotClause()
{
    delete _clause;
}

void NotClause::evaluate(FilterClauseContext& context) const
{
    context.isComplete = false;
    _clause->evaluate(context);
    if (context.isComplete) {
        return;
    }
    context.result = !context.result.getBooleanValue();
}

namespace {
FilterClause& createSubclauses(FilterExpression& expn, bool isAnd)
{
    uint32_t n = expn.getSize();
    if (n == 0) {
        return *new TrueClause;
    }

    vector<FilterClause*> results;
    results.reserve(n);
    for (uint32_t i = 0; i < n; i++) {
        results.push_back(&FilterClause::create(*expn.getSubclause(i)));
    }

    if (isAnd) {
        return *new AndClause(results);
    }
    return *new OrClause(results);
}

FilterClause& createNotClause(FilterExpression& expn)
{
    return *new NotClause(&FilterClause::create(*expn.getSubclause(0)));
}

template<typename T>
FilterClause& createIntPredicateClause(FilterExpression& expn)
{

    FilterExpression::Literal lit = expn.getLiteral();
    uint32_t index = expn.getIndex();
    FilterExpression::Op op = expn.getOp();
    if (op == FilterExpression::In) {
        return *new InPredicateClause<T>(index, getLiteralValue<T>(lit));
    }

    FilterClause* filterClause = 0;
    if (expn.hasSubscript()) {
        filterClause = new SubscriptedValueClause<T>(index, expn.getSubscript());
    } else {
        filterClause = new ValueClause<T>(index);
    }

    FilterExpression::ArithmeticOperationsVector::size_type mathCount =
      expn.getArithmeticOperationsCount();
    for (FilterExpression::ArithmeticOperationsVector::size_type mathIndex = 0;
         mathIndex < mathCount; ++mathIndex) {
        FilterExpression::ArithmeticOperation const& math = expn.getArithmeticOperation(mathIndex);
        filterClause =
          new ArithmeticValueClause<T>(math.getOperator(), filterClause, math.getOperand());
    }

    if (op != FilterExpression::None) {
        filterClause = new PredicateClause<T>(op, filterClause, getLiteralValue<T>(lit));
    }

    return *filterClause;
}

template<typename T>
FilterClause& createPredicateClause(FilterExpression& expn)
{
    FilterExpression::Literal lit = expn.getLiteral();
    uint32_t index = expn.getIndex();
    FilterExpression::Op op = expn.getOp();
    if (op == FilterExpression::In) {
        return *new InPredicateClause<T>(index, getLiteralValue<T>(lit));
    }

    FilterClause* filterClause = 0;
    if (expn.hasSubscript()) {
        filterClause = new SubscriptedValueClause<T>(index, expn.getSubscript());
    } else {
        filterClause = new ValueClause<T>(index);
    }

    // No arithmetic operators for non-int types.  The operators we support
    // are bitwise and modulus operators, which are not defined for floats,
    // strings, or booleans.

    // TODO support + - * / for floats

    if (op != FilterExpression::None) {
        filterClause = new PredicateClause<T>(op, filterClause, getLiteralValue<T>(lit));
    }

    return *filterClause;
}
}

FilterClause& FilterClause::create(FilterExpression& expn)
{
    switch (expn.getKind()) {
        case FilterExpression::AndClause:
            return createSubclauses(expn, true);
            break;
        case FilterExpression::OrClause:
            return createSubclauses(expn, false);
            break;
        case FilterExpression::NotClause:
            return createNotClause(expn);
        case FilterExpression::Predicate: {

            FilterExpression::Literal lit = expn.getLiteral();
            switch (lit.getKind()) {
                case FilterExpression::Literal::RString:
                    return createPredicateClause<SPL::rstring>(expn);

                case FilterExpression::Literal::Int64:
                    return createIntPredicateClause<SPL::int64>(expn);
                case FilterExpression::Literal::Int32:
                    return createIntPredicateClause<SPL::int32>(expn);
                case FilterExpression::Literal::Int16:
                    return createIntPredicateClause<SPL::int16>(expn);
                case FilterExpression::Literal::Int8:
                    return createIntPredicateClause<SPL::int8>(expn);
                case FilterExpression::Literal::UInt64:
                    return createIntPredicateClause<SPL::uint64>(expn);
                case FilterExpression::Literal::UInt32:
                    return createIntPredicateClause<SPL::uint32>(expn);
                case FilterExpression::Literal::UInt16:
                    return createIntPredicateClause<SPL::uint16>(expn);
                case FilterExpression::Literal::UInt8:
                    return createIntPredicateClause<SPL::uint8>(expn);

                case FilterExpression::Literal::Float64:
                    return createPredicateClause<SPL::float64>(expn);
                case FilterExpression::Literal::Float32:
                    return createPredicateClause<SPL::float32>(expn);
                case FilterExpression::Literal::Boolean:
                    return createPredicateClause<SPL::boolean>(expn);
                default:
                    break;
            }
        }
        default:
            // This should be unreachable
            return *new TrueClause;
    }
}
