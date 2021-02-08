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

#ifndef SPL_DEBUGGER_ProbePointExpressionPredicate_H
#define SPL_DEBUGGER_ProbePointExpressionPredicate_H

#include <SPL/Debugger/BaseProbePointExpression.h>
#include <SPL/Debugger/ProbePointPredicate.h>
#include <UTILS/Mutex.h>

namespace SPL {
class ProbePointServices;
class ProbePoint;
class Tuple;
class OperatorPort;
namespace Meta {
class TupleType;
}

/*!
 * Each instance of this class represents a predicate
 * established for a debugger probe point.
 * When the probe point is processed, it evaluates its
 * set of predicates to decide whether the probe point's
 * action should be performed.
 *
 * This class represents the base function shared by the
 * various probe point predicate types - e.g. regex match, comparator, etc.
 */
class ProbePointExpressionPredicate : public ProbePointPredicate
{
  public:
    /// Constructor
    /// @param expn             predicate expression
    /// @param tupleType        Associated tuple
    ProbePointExpressionPredicate(const std::string& expn, const Meta::TupleType& tupleType);

    /// Destructor
    virtual ~ProbePointExpressionPredicate();

    /// Evaluate predicate
    /// @param probePointServices       SDB Services
    /// @param probePoint               Associated probe point
    /// @param tuple                    Current tuple to be evaluated against
    /// @param punct                    Current punctuation to be evaluated against
    /// @return true if the predicate evaluates to 'true'
    virtual bool evaluate(ProbePointServices& probePointServices,
                          ProbePoint* probePoint,
                          Tuple* tuple,
                          Punctuation* punct);

    /// Return the predicate expression
    /// @return predicate expression
    virtual const std::string getExpression();

    /// Format predicate nicely
    /// @return expression as a nice string
    virtual const std::string toString();

    /// Is the predicate valid?
    /// Return true if the predicate 'compiled' without problems
    bool valid() const;

  private:
    typedef BaseProbePointExpression* (*createFcnType)(
      std::string const& exprAsString,
      Meta::TupleType const& tupleType,
      BaseProbePointExpression::ContextDefinition const& context);
    static UTILS_NAMESPACE::Mutex _evaluate_mutex;
    static createFcnType _createProbePoint;
    static BaseProbePointExpression::ContextDefinition _probeContext;

    std::string _expn;
    std::auto_ptr<BaseProbePointExpression> _probePoint;
    const OperatorPort* _port;

    void initialize();
};

typedef boost::shared_ptr<ProbePointExpressionPredicate> ProbePointExpressionPredicatePtr;
}

#endif /* SPL_DEBUGGER_ProbePointExpressionPredicate_H */
