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

package com.ibm.streams.operator.internal.metrics;

import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.OperatorMetrics;

/** Interface between the SPL Java operator runtime and the mechanism for operator metrics. */
public interface OperatorMetricsFactory<C extends AbstractMetric, S extends ReadOnlyMetric>
    extends PortMetricsFactory<
        S, OperatorMetrics.InputPortMetric, OperatorMetrics.OutputPortMetric> {

  C newCustomMetric(String name, String description, Metric.Kind kind);

  C getCustomMetric(String name);

  /**
   * Get the list of the names of the current custom metrics. Returns null if the count of the
   * current metrics is equal to the passed in count, that is no metrics have been added or the Java
   * side knows about all custom metrics.
   *
   * @param currentCount Current count of the custom metrics as seen by Java.
   * @return The list of names or null.
   */
  String[] getCustomMetricNames(int currentCount);

  S getOperatorMetric(OperatorMetrics.SystemMetric name);
}
