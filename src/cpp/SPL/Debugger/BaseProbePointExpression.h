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

#ifndef BASE_PROBE_POINT_EXPN_H
#define BASE_PROBE_POINT_EXPN_H

#include <SPL/Runtime/Type/ValueHandle.h>
#include <UTILS/HashMapHelpers.h>
#include <string>

namespace SPL {
class Tuple;
namespace Meta {
class BaseType;
};
class BaseProbePointExpression
{

  public:
    typedef std::tr1::unordered_map<std::string, const Meta::BaseType*> ContextDefinition;
    typedef std::tr1::unordered_map<std::string, ConstValueHandle> ContextEvaluation;

    /// Destructor
    virtual ~BaseProbePointExpression() {}

    /// evaluate the probe expression using the tuple values
    /// @param value Tuple
    /// @param context Extra information available to expression
    /// @returns 'true' if the probe expression folded to true
    virtual bool evaluate(Tuple const& value, ContextEvaluation const& context) = 0;

    /// Is the Expression valid?
    /// @returns 'true' if the probe expression is valid
    virtual bool valid() const = 0;
};
};
#endif // BASE_PROBE_POINT_EXPN_H
