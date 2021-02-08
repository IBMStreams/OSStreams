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

#ifndef PROBE_POINT_EXPRESSION_H
#define PROBE_POINT_EXPRESSION_H

#include <SPL/Debugger/BaseProbePointExpression.h>

namespace SPL {
class Tuple;
namespace Meta {
class TupleType;
};
class Expression;

class ProbePointExpression : public BaseProbePointExpression
{
  public:
    /// construct a probe point expression
    /// @param exprAsString probe point expression as a string
    /// @param tupleType tuple type for the probe point
    /// @param context Definition of extra symbols available for reference
    ProbePointExpression(std::string const& exprAsString,
                         Meta::TupleType const& tupleType,
                         ContextDefinition const& context);

    /// Destructor
    virtual ~ProbePointExpression();

    /// evaluate the probe expression using the tuple values
    /// @param value Tuple (NULL for punctuation)
    /// @param context Definition of extra symbols available for use
    /// @returns 'true' if the probe expression folded to true
    virtual bool evaluate(Tuple const& value, ContextEvaluation const& context);

    /// Is the expression valid
    /// @returns 'true' if the probe expression is evaluatable
    bool valid() const { return isEvaluatable_; }

  private:
    Expression* createProbeExpression(std::string const& exprAsString,
                                      Meta::TupleType const& tupleType,
                                      ContextDefinition const& context);
    std::auto_ptr<Expression> probeExpn_;

    bool isEvaluatable_;
};
};

/// External interface for creating a problem point
/// Suitable for invoking via dlopen/dlsym
/// NOTE:  If you change this interface, you MUST update Debugger/ProbePointExpressionPredicate.cpp
/// @param exprAsString expression to be 'compiled'
/// @param tupleType the tuple to be referenced in the expression
/// @param context Definition of extra symbols available for reference
/// @return a newly created ProbePointExpression
extern "C" SPL::BaseProbePointExpression* createProbePoint(
  std::string const& exprAsString,
  SPL::Meta::TupleType const& tupleType,
  SPL::BaseProbePointExpression::ContextDefinition const& context);

#endif /* PROBE_POINT_EXPRESSION_H */
