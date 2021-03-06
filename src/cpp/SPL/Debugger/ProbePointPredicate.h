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

#ifndef SPL_DEBUGGER_ProbePointPredicate_H
#define SPL_DEBUGGER_ProbePointPredicate_H

#include <SPL/Runtime/Operator/Port/OperatorPort.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <boost/shared_ptr.hpp>
#include <string>

namespace SPL {

class ProbePointServices;
class ProbePoint;
class Tuple;

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
class ProbePointPredicate
{
  public:
    /// Destructor
    virtual ~ProbePointPredicate() {}

    /// Evaluate one predicate for a given Tuple or Punctuation
    /// @param probePointServices Probe Point Services
    /// @param probePoint Probe point being evaluated
    /// @param tuple Current Tuple, or NULL if punct != NULL
    /// @param punct Current Punctuation, or NULL if tuple != NULL
    virtual bool evaluate(ProbePointServices& probePointServices,
                          ProbePoint* probePoint,
                          Tuple* tuple,
                          Punctuation* punct) = 0;

    /// Return the predicate expression
    /// @return predicate expression
    virtual const std::string getExpression() = 0;

    /// Format predicate nicely
    /// @return expression as a nice string
    virtual const std::string toString() = 0;
};

typedef boost::shared_ptr<ProbePointPredicate> ProbePointPredicatePtr;

}

#endif /* SPL_DEBUGGER_ProbePointPredicate_H */
