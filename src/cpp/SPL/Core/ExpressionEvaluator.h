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

#ifndef EXPRESSION_EVALUATION_H
#define EXPRESSION_EVALUATION_H

#include <SPL/Core/ExpressionValue.h>

namespace SPL {
class CompilerConfiguration;
class Expression;

typedef std::tr1::unordered_map<std::string, ExpressionValue*> ValueTableType;

std::ostream& operator<<(std::ostream& ostr, const ValueTableType& vt);

//! Evaluate Expressions at compile time, if possible
class ExpressionEvaluator
{
  public:
    //! What is the type of the name
    enum Operation
    {
        Add,         //!< +   (concat for strings)
        Sub,         //!< -
        Mul,         //!< *
        Div,         //!< /
        Mod,         //!< %
        LeftShift,   //!< <<
        RightShift,  //!< >>
        Negate,      //!< Unary -
        And,         //!< &
        Or,          //!< |
        Xor,         //!< ^
        In,          //!< in (list/set/map inclusion)
        Complement,  //!< ~ (complement for boolean)
        Equal,       //!< ==
        DotEqual,    //!< .==
        NotEqual,    //!< !=
        DotNotEqual, //!< .!=
        LessThan,    //!< <
        LessEqual,   //!< <=
        GreaterThan, //!< >
        GreaterEqual //!< >=
    };

    enum EvaluateFailureCode
    {
        Success,
        FoundTypeMismatch,
        FoundUnknownReference,
        ExceptionOccured,
        numEvaluateFailureCode
    };

    /// Initialization
    /// No work done here
    ExpressionEvaluator(CompilerConfiguration& config, bool indontCatch = false)
      : _config(config)
      , _dontCatch(indontCatch)
    {}

    /// (Possibly) Evaluate an operation
    /// @param op Operation to be performed
    /// @param arg1 left hand (or only) argument
    /// @param arg2 rhs argument (for binary op)
    /// @param result Result of the operation
    /// @returns 'true' if the operation could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression SPLCompilerDivModByZero
    bool evaluateOp(Operation op,
                    const ExpressionValue& arg1,
                    const ExpressionValue& arg2,
                    ExpressionValue& result);

    /// (Possibly) Evaluate an expression involving a call to a native
    /// function that is known to the compiler
    /// @param fcnNum Function number to invoke
    /// @param args   Vector of inputs to a function call
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool evaluateBuiltin(uint32_t fcnNum,
                         const std::vector<ExpressionValue>& args,
                         ExpressionValue& result);

    /// Basic Arithmetic functions:  works on primitive and list<primitive>

    /// Addition   (concat for strings)
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool add(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Subtraction
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool sub(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Multiplication
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool mul(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Division
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression SPLCompilerDivModByZero
    bool div(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Modulus
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression SPLCompilerDivModByZero
    bool mod(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Shift Left
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    bool shiftl(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Shift Right
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    bool shiftr(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Basic Arithmetic functions
    /// @param rhs arg 1
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool negate(const ExpressionValue& rhs, ExpressionValue& result);

    /// And (bitwise, boolean)
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool doAnd(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Or (bitwise, boolean)
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool doOr(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Xor (bitwise, boolean)
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool doXor(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// In (set/map/list inclusion)
    /// @param item Item to be searched for
    /// @param rhs List to be searched
    /// @param result Result of the call (boolean)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool doIn(const ExpressionValue& item, const ExpressionValue& rhs, ExpressionValue& result);

    /// Complement (bitwise, boolean)
    /// @param rhs arg
    /// @param result Result of the call
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool complement(const ExpressionValue& rhs, ExpressionValue& result);

    /// Basic Comparison  functions:  works on primitive and list<primitive>

    /// Equality
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (boolean)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool equal(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Memberwise Equality
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (list<boolean>)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool dotEqual(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Inequality
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (boolean)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool notEqual(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Memberwise Inequality
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (list<boolean>)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool dotNotEqual(const ExpressionValue& lhs,
                     const ExpressionValue& rhs,
                     ExpressionValue& result);

    /// Less Than
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (boolean)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool lessThan(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Less Than or Equal
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (boolean)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool lessEqual(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);

    /// Greater Than
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (boolean)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool greaterThan(const ExpressionValue& lhs,
                     const ExpressionValue& rhs,
                     ExpressionValue& result);

    /// Greater Than or Equal
    /// @param lhs arg 1
    /// @param rhs arg 2
    /// @param result Result of the comparison (boolean)
    /// @returns 'true' if the call could be folded, else 'false'
    /// @throws SPLCompilerInvalidExpression
    bool greaterEqual(const ExpressionValue& lhs,
                      const ExpressionValue& rhs,
                      ExpressionValue& result);

    /// Is Evaluatable Type
    /// @param type RootTyp
    /// @returns 'true' if the evaluator can fold an operation of this type
    static bool isEvaluatableType(const RootTyp& type);

    /// (Possibly) Evaluate an expression involving a call to a native
    /// function that is known to the compiler
    /// @param fcnNum Function number to invoke
    /// @returns 'true' if the call could be folded, else 'false'
    static bool isBuiltinEvaluatable(uint32_t fcnNum);

    /// Is Evaluatable Expression
    /// @param e Expression
    /// @returns 'true' if the evaluator can fold the Expression e
    static bool isEvaluatable(const Expression& e);

    /// evaluate an expression using a table of values
    /// @param expn Expression
    /// @param valueTable ValueTableType
    /// @param resonCode resulting EvaluateFailureCode
    /// @returns value or NULL if failed to evaluate
    ExpressionValue* evaluate(const Expression& expn,
                              const ValueTableType& valueTable,
                              enum EvaluateFailureCode& resonCode);

    /// check to see if we are catching exceptions
    /// @returns true if we are not catching exceptions
    bool dontCatch() const { return _dontCatch; }

  private:
    bool addList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool subList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool mulList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool divList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool modList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool shiftlList(const ExpressionValue& lhs,
                    const ExpressionValue& rhs,
                    ExpressionValue& result);
    bool shiftrList(const ExpressionValue& lhs,
                    const ExpressionValue& rhs,
                    ExpressionValue& result);
    bool negateList(const ExpressionValue& rhs, ExpressionValue& result);
    bool doAndList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool doOrList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool doXorList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool complementList(const ExpressionValue& rhs, ExpressionValue& result);
    bool equalList(const ExpressionValue& lhs, const ExpressionValue& rhs, ExpressionValue& result);
    bool notEqualList(const ExpressionValue& lhs,
                      const ExpressionValue& rhs,
                      ExpressionValue& result);
    bool lessThanList(const ExpressionValue& lhs,
                      const ExpressionValue& rhs,
                      ExpressionValue& result);
    bool lessEqualList(const ExpressionValue& lhs,
                       const ExpressionValue& rhs,
                       ExpressionValue& result);
    bool greaterThanList(const ExpressionValue& lhs,
                         const ExpressionValue& rhs,
                         ExpressionValue& result);
    bool greaterEqualList(const ExpressionValue& lhs,
                          const ExpressionValue& rhs,
                          ExpressionValue& result);
    CompilerConfiguration& _config;
    bool _dontCatch;
};

std::ostream& operator<<(std::ostream& ostr,
                         const enum ExpressionEvaluator::EvaluateFailureCode& fc);
};
#endif /* EXPRESSION_EVALUATION_H */
