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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.operator.internal.metrics.RuntimeMetric;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric.MetricValue;
import com.ibm.streams.operator.internal.metrics.RuntimeMetricsFactory;
import com.ibm.streams.operator.metrics.OperatorMetrics.InputPortMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics.OutputPortMetric;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public class DirectMetricsFactory extends RuntimeMetricsFactory {

  private Map<Integer, DirectTupleInput> inputHandlers =
      Collections.synchronizedMap(new HashMap<Integer, DirectTupleInput>());
  private Map<Integer, DirectTupleOutput> outputHandlers =
      Collections.synchronizedMap(new HashMap<Integer, DirectTupleOutput>());

  void addPort(int port, DirectTupleInput inputPort) {
    inputHandlers.put(port, inputPort);
  }

  void addPort(int port, DirectTupleOutput outputPort) {
    outputHandlers.put(port, outputPort);
  }

  @Override
  protected RuntimeMetric.MetricValue getMetricValue(int port, InputPortMetric name) {
    return inputHandlers.get(port).getMetricValue(name);
  }

  @Override
  protected MetricValue getMetricValue(int port, OutputPortMetric name) {
    return outputHandlers.get(port).getMetricValue(name);
  }
}
