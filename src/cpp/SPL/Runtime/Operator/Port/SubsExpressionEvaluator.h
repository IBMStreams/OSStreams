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

#ifndef SUBSEXPRESSIONEVALUATOR_H_
#define SUBSEXPRESSIONEVALUATOR_H_

#include <SPL/Runtime/Operator/Port/StreamProperty.h>

namespace SPL {
class StreamPropertyCollection;
class SubscriptionExpression;

class SubsExpressionEvaluator
{
  public:
    /// Evaluate the given expression against the specified properties
    /// @param expression SPL expression
    /// @param streamPropertiesTypeXml properties in XML (format applicationMode.xsd -
    /// streamPropertiesType)
    /// @return true if match is successful
    static const bool evaluate(const std::string& expression,
                               const std::string& streamPropertiesTypeXml);

    /// Evaluate the given expression against the specified properties
    /// @param expression SPL expression
    /// @param spcol properties
    /// @return true if match is successful
    static const bool evaluate(const std::string& expression,
                               const StreamPropertyCollection& spcol);

    /// Evaluate the given expression against the specified properties
    /// @param expr SPL expression
    /// @param spcol properties
    /// @return true if match is successful
    static const bool evaluate(const SubscriptionExpression& expr,
                               const StreamPropertyCollection& spcol);

  private:
    /// Evaluate the predicate against the specified properties
    /// @param expr predicate
    /// @param spcol properties
    /// @return true if match is successful
    static const bool evaluatePredicate(const SubscriptionExpression& expr,
                                        const StreamPropertyCollection& spcol);

    /// Find and return the specified property in the given properties list
    /// @param name Name of the property
    /// @param spcol properties
    /// @return stream property associated with the name
    /// @throws SPLRuntimeStreamPropertyNotFoundException
    static const StreamProperty& findProperty(const std::string& name,
                                              const StreamPropertyCollection& spcol);

    /// Evaluate a rstring predicate
    /// @param expr predicate
    /// @param spcol properties
    /// @param val rstring value
    /// @return true if match is successful
    static const bool evaluate(const SubscriptionExpression& expr,
                               const StreamProperty& prop,
                               const rstring& val);

    /// Evaluate a numeric predicate
    /// @param expr predicate
    /// @param spcol properties
    /// @param val numeric value
    /// @return true if match is successful
    template<typename T>
    static const bool evaluateNumeric(const SubscriptionExpression& expr,
                                      const StreamProperty& prop,
                                      T val);
};

};

#endif /* SUBSEXPRESSIONEVALUATOR_H_ */
