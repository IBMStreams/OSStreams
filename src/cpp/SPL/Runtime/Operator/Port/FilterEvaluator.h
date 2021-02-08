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

#ifndef FILTER_EVALUATOR_H
#define FILTER_EVALUATOR_H

/*!
 * \file FilterEvaluator.h \brief Class to evaluate Filter Expressions when exporting
 */
#include <functional>
#include <typeinfo>

#include <SPL/Runtime/Operator/Port/FilterExpression.h>
#include <SPL/Runtime/Type/Meta/Type.h>
#include <SPL/Runtime/Type/SPLType.h>

namespace SPL {
namespace Filter {

// Helper to access the values of FilterExpression::Literal variables.
// Note that no casting is performed.  If, for example,
// getLiteralValue<int64>(foo) is called, foo must contain an int64
// value, or an exception will thrown.
template<typename T>
T const& getLiteralValue(FilterExpression::Literal const& value)
{
    return value.getValue<T>();
}

/// AppliedRelativeOp applies a relative operation (a value comparison)
/// between two values, one of type LeftType and one of type
/// RightType, and produces a result of type bool.
template<typename LeftType, typename RightType = LeftType>
struct AppliedRelativeOp
{
    inline static bool apply(FilterExpression::Op op, LeftType const& lhs, RightType const& rhs)
    {
        switch (op) {
            case FilterExpression::Equal:
                return eq(lhs, rhs);
            case FilterExpression::NotEqual:
                return neq(lhs, rhs);
            case FilterExpression::LessThan:
                return lt(lhs, rhs);
            case FilterExpression::LessEqual:
                return lte(lhs, rhs);
            case FilterExpression::GreaterThan:
                return gt(lhs, rhs);
            case FilterExpression::GreaterEqual:
                return gte(lhs, rhs);
            default:; // fallthrough
        }
        return false;
    }

    static inline bool eq(LeftType const& lhs, RightType const& rhs) { return lhs == rhs; }
    static inline bool neq(LeftType const& lhs, RightType const& rhs) { return lhs != rhs; }
    static inline bool lt(LeftType const& lhs, RightType const& rhs) { return lhs < rhs; }
    static inline bool lte(LeftType const& lhs, RightType const& rhs) { return lhs <= rhs; };
    static inline bool gt(LeftType const& lhs, RightType const& rhs) { return lhs > rhs; }
    static inline bool gte(LeftType const& lhs, RightType const& rhs) { return lhs >= rhs; }
};

/// AppliedArithmeticOp applies a unary or binary arithmetic operation.
/// For binary operations, the operation operates on a value of type
/// LeftType and a value of type RightType, producing a result of type
/// ResultType.  For a unary operation, it operates on a value of type
/// LeftType and produces a result of type ResultType.  In either case,
/// the return value will be a Literal containing a value of type
/// ResultType.
template<typename ResultType, typename LeftType = ResultType, typename RightType = LeftType>
struct AppliedArithmeticOp
{
    inline static FilterExpression::Literal apply(FilterExpression::ArithmeticOp op,
                                                  LeftType const& lhs,
                                                  RightType const& rhs)
    {
        switch (op) {
            case FilterExpression::Mod:
                return mod(lhs, rhs);
            case FilterExpression::BitOr:
                return bitOr(lhs, rhs);
            case FilterExpression::BitAnd:
                return bitAnd(lhs, rhs);
            case FilterExpression::BitXor:
                return bitXor(lhs, rhs);
            case FilterExpression::LeftShift:
                return leftShift(lhs, rhs);
            case FilterExpression::RightShift:
                return rightShift(lhs, rhs);

            case FilterExpression::Add:
                return add(lhs, rhs);
            case FilterExpression::Sub:
                return sub(lhs, rhs);
            case FilterExpression::Mul:
                return mul(lhs, rhs);
            case FilterExpression::Div:
                return div(lhs, rhs);

            // Unary operation: ignore rhs
            case FilterExpression::BNot:
                return bitNot(lhs);
            default:
                break;
        }
        THROW_STRING(SPLRuntimeFilter, "Unsupported Operation"); // TODO better handling
    }

    // Bitwise binary arithmetic operations.  These may be used with
    // integer values only
    static inline FilterExpression::Literal bitOr(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs | rhs));
    }
    static inline FilterExpression::Literal bitAnd(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs & rhs));
    }
    static inline FilterExpression::Literal bitXor(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs ^ rhs));
    }
    static inline FilterExpression::Literal leftShift(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs << rhs));
    }
    static inline FilterExpression::Literal rightShift(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs >> rhs));
    }

    // Binary arithmetic operations.  May be used with integer and
    // floating point values
    static inline FilterExpression::Literal add(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs + rhs));
    }
    static inline FilterExpression::Literal sub(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs - rhs));
    }
    static inline FilterExpression::Literal mod(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs % rhs));
    }
    static inline FilterExpression::Literal mul(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs * rhs));
    }
    static inline FilterExpression::Literal div(LeftType const& lhs, RightType const& rhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(lhs / rhs));
    }

    // Unary bitwise operation.  May be used with integer values only
    static inline FilterExpression::Literal bitNot(LeftType const& lhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(~lhs));
    }

    // Unary operation.  May be used with any type.
    static inline FilterExpression::Literal hash(LeftType const& lhs)
    {
        return FilterExpression::Literal(static_cast<ResultType>(std::tr1::hash<LeftType>()(lhs)));
    }
};

/// Class that represents a filter evaluation clause for filter expressions
class FilterClause
{
  public:
    /// Dtor
    virtual ~FilterClause() {}

    /// Evaluate this clause
    /// @param tuple Tuple used to evaluate the expression
    /// @return true if the expression evaluates to true, else false
    bool evaluate(const Tuple& tuple) const
    {
        FilterClauseContext context(tuple);
        evaluate(context);
        return context.result.getBooleanValue();
    }

    /// Create a FilterClause that represents the filter expression
    /// @param expn filter expression to be converted
    /// @return A FilterClause that can be used to evaluate the FilterExpression. The caller
    /// is responsible for deleting the memory
    static FilterClause& create(FilterExpression& expn);

  protected:
    // FilterClauseContext is the context in which a filter clause
    // is being evaluated.  This includes the tuple from which
    // it resolves symbol values, the intermediate or final value,
    // and whether the value has been completedly determined or not.
    struct FilterClauseContext
    {
        // The tuple against which to resolve symbols.
        Tuple const& tuple;
        // The intermediate or final result of the evaluation.
        // Each clause may reference the current value, operate
        // on it, and then write the new result.  For intermediate
        // values, the value may have any type, but the type
        // of the final value must be boolean.
        FilterExpression::Literal result;

        // Some operators may cause the filter evaluation to
        // "short circuit".  An example is the by convention,
        // attempting to reference a value outside the range of a
        // list does not result in an error, but results in the
        // filter clause evaluating to false.  Intermediate
        // filter clauses may set isComplete to true to indicate
        // that the result is complete, and no further operations
        // should be performed by other tuple clauses.
        bool isComplete;

        FilterClauseContext(Tuple const& t)
          : tuple(t)
          , result()
          , isComplete(false)
        {}
    };

  public:
    // Even though this is public, it is intended for internal use
    // by FilterClause implementations only.  External users
    // should use the other evaluate method above.
    virtual void evaluate(FilterClauseContext& context) const = 0;
};

/// Dummy 'true' expression for placeholder
class TrueClause : public FilterClause
{
  public:
    virtual void evaluate(FilterClauseContext& context) const { context.result = true; }
};

/// A ValueClause is a clause that extracts the value of an
/// attribute from a tuple.
template<typename T>
class ValueClause : public FilterClause
{
  public:
    ValueClause(int32 attrIndex)
      : _attrIndex(attrIndex)
    {}

    void evaluate(FilterClauseContext& context) const
    {
        context.isComplete = false;
        ConstValueHandle vh = context.tuple.getAttributeValue(_attrIndex);
        const T& value = static_cast<const T&>(vh);
        context.result = FilterExpression::Literal(value);
    }

  private:
    int32 _attrIndex;
};

/// A SubscriptedValueClause is a clause that extracts the value of an
/// attribute from a tuple.  This value must be a list.  It then
/// extracts a member of the list by numeric index.  If the specified
/// index is out of bounds of the list, the return is false, and any
/// containing clauses are expected to return false as well.
template<typename T>
class SubscriptedValueClause : public FilterClause
{
  public:
    SubscriptedValueClause(uint32_t attrIndex, int64_t subscript)
      : _attrIndex(attrIndex)
      , _subscript(subscript)
    {}

    virtual void evaluate(FilterClauseContext& context) const
    {
        context.isComplete = false;
        ConstValueHandle vh = context.tuple.getAttributeValue(_attrIndex);
        const List& lv = (const List&)vh;
        ConstValueHandle evh;
        if (lv.getSize() > _subscript) {
            evh = lv.getElement(_subscript);
        } else {
            // subscript is out of range.
            context.isComplete = true;
            context.result = false;
            return;
        }
        const T& tupleValue = static_cast<const T&>(evh);
        context.result = FilterExpression::Literal(tupleValue);
    }

  private:
    int32 _attrIndex;
    int64_t _subscript;
};

/// An ArithmeticValueClause is a clause that first evaluates an inner
/// clause, and then applies an arithmetic operation on the result
/// of the inner clause.
template<typename ResultType, typename LeftType = ResultType, typename RightType = LeftType>
class ArithmeticValueClause : public FilterClause
{
  public:
    ArithmeticValueClause(FilterExpression::ArithmeticOp arithmeticOperator,
                          FilterClause* valueClause,
                          FilterExpression::Literal const& arithmeticOperand)
      : _arithmeticOperator(arithmeticOperator)
      , _arithmeticOperand(getLiteralValue<RightType>(arithmeticOperand))
      , _valueClause(valueClause)
    {}
    virtual ~ArithmeticValueClause() { delete _valueClause; }

    virtual void evaluate(FilterClauseContext& context) const
    {
        context.isComplete = false;
        _valueClause->evaluate(context);
        if (context.isComplete) {
            return;
        }

        LeftType lhs = getLiteralValue<LeftType>(context.result);

        FilterExpression::Literal result =
          AppliedArithmeticOp<LeftType, RightType, ResultType>::apply(_arithmeticOperator, lhs,
                                                                      _arithmeticOperand);

        context.result = result;
    }

  private:
    FilterExpression::ArithmeticOp _arithmeticOperator;
    RightType _arithmeticOperand;
    FilterClause* _valueClause;
};

/// A PredicateClause is a clause that evaluates an inner clause,
/// and then applies a conditional operation against the result
/// of the inner clause.
template<typename T>
class PredicateClause : public FilterClause
{
  public:
    PredicateClause(FilterExpression::Op op, FilterClause* valueClause, const T& rhs)
      : _rhs(rhs)
      , _op(op)
      , _valueClause(valueClause)
    {}
    virtual ~PredicateClause() { delete _valueClause; }

    virtual void evaluate(FilterClauseContext& context) const
    {
        context.isComplete = false;
        // There should be exactly one subclause, who evaluates
        // to a literal of type T
        _valueClause->evaluate(context);

        if (context.isComplete) {
            return;
        }

        FilterExpression::Literal result =
          AppliedRelativeOp<T>::apply(_op, getLiteralValue<T>(context.result), _rhs);

        context.result = result;
    }

  private:
    T _rhs;
    FilterExpression::Op _op;
    FilterClause* _valueClause; // TODO smart pointer.
};

/// Class that represents a In predicate to be evaluated
template<class T>
class InPredicateClause : public FilterClause
{
  public:
    /// Constructor
    /// @param attrIndex attribute number in the tuple
    /// @param value value to be checked to see if it is in the list
    InPredicateClause(uint32_t attrIndex, const T& value)
      : _attrIndex(attrIndex)
      , _value(value)
    {}

    /// Evaluate this clause
    /// @param tuple Tuple used to evaluate the expression
    /// @return true if the expression evaluates to true, else false
    virtual void evaluate(FilterClauseContext& context) const
    {
        context.isComplete = false;
        ConstValueHandle vh = context.tuple.getAttributeValue(_attrIndex);
        const List& lv = (const List&)vh;
        const SPL::list<T>& tupleValue = (const SPL::list<T>&)lv;
        context.result =
          std::find(tupleValue.begin(), tupleValue.end(), _value) != tupleValue.end();
    }

  private:
    int32 _attrIndex;
    T _value;
};

/// Class the represents an AND clause
class AndClause : public FilterClause
{
  public:
    /// Constructor
    /// @param attrIndex attribute number in the tuple
    /// @param clauses sub clauses to be evaluated
    AndClause(std::vector<FilterClause*> clauses)
      : _clauses(clauses)
    {}

    /// Destructor
    ~AndClause();

    /// Evaluate this clause
    /// @param tuple Tuple used to evaluate the expression
    /// @return true if the expression evaluates to true, else false

    virtual void evaluate(FilterClauseContext& context) const;

  private:
    std::vector<FilterClause*> _clauses;
};

/// Class the represents an OR clause
class OrClause : public FilterClause
{
  public:
    /// Constructor
    /// @param clauses sub clauses to be evaluated
    OrClause(std::vector<FilterClause*> clauses)
      : _clauses(clauses)
    {}

    /// Destructor
    ~OrClause();

    /// Evaluate this clause
    /// @param tuple Tuple used to evaluate the expression
    /// @return true if the expression evaluates to true, else false
    void evaluate(FilterClauseContext& context) const;

  private:
    std::vector<FilterClause*> _clauses;
};

/// Class the represents a NOT clause
class NotClause : public FilterClause
{
  public:
    /// Constructor
    /// @param clauses sub clause to be evaluated
    NotClause(FilterClause* clause)
      : _clause(clause)
    {}

    /// Destructor
    ~NotClause();

    /// Evaluate this clause
    /// @param tuple Tuple used to evaluate the expression
    /// @return true if the expression evaluates to true, else false
    virtual void evaluate(FilterClauseContext& context) const;

  private:
    FilterClause* _clause;
};
};
};

#endif /* FILTER_EVALUATOR_H */
