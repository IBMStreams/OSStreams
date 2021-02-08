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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_HANDLER_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_HANDLER_H

#include <SPL/Runtime/Common/Metric.h>
#include <vector>

namespace SPL {
/// Class for allowing the operator metrics to be handled by an operator's
/// language-specific runtime code.
/// This is used for JavaOp operators that store custom metrics in Java.
class OperatorMetricsHandler
{
  public:
    /// Create a new operator metric for the operator with a given name and initial value
    /// @param name metric name
    /// @param description metric description
    /// @param kind metric kind
    /// @param value metric value
    /// @throws SPLRuntimeInvalidMetricException if the metric already exists
    virtual void createCustomMetric(std::string const& name,
                                    std::string const& description,
                                    Metric::Kind kind,
                                    int64_t value) = 0;

    /// Get the specified custom metric value for the operator
    /// @param name metric name
    /// @return metric value
    /// @throws SPLRuntimeInvalidMetricException if there is no metric with the given name
    virtual int64_t getCustomMetricValue(std::string const& name) = 0;

    /// Set the specified custom metric to the given value for the operator
    /// @param name metric name
    /// @param metric value
    /// @throws SPLRuntimeInvalidMetricException if there is no metric with the given name
    virtual void setCustomMetricValue(std::string const& name, int64_t value) = 0;

    /// Check if the specified custom metric exists for the operator
    /// @param name metric name
    /// @return true if the metric exists, false otherwise
    virtual bool hasCustomMetric(std::string const& name) = 0;

    /// Return all the names of the custom metrics for the operator
    /// @return a list of the names of all the custom metrics defined for this operator
    virtual std::vector<std::string> getCustomMetricNames() = 0;
};
}
#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_HANDLER_H */
