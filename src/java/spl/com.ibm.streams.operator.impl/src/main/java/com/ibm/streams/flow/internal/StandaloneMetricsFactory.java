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

package com.ibm.streams.flow.internal;

import com.ibm.streams.operator.internal.metrics.PEMetricsFactory;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric;
import com.ibm.streams.operator.metrics.PEMetrics;

/** Can never return metrics since the number of PE input and output ports is zero. */
final class StandaloneMetricsFactory implements PEMetricsFactory<RuntimeMetric> {

  @Override
  public RuntimeMetric getInputMetric(int port, PEMetrics.InputPortMetric name) {
    return null;
  }

  @Override
  public RuntimeMetric getOutputMetric(int port, PEMetrics.OutputPortMetric name) {
    return null;
  }
}
