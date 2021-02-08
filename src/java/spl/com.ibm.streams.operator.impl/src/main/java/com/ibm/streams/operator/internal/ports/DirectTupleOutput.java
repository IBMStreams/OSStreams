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

import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics;

/**
 * Class that processes output tuples and punctuation and implements input port finalization and
 * operator port metrics before handing off the tuples to the provided raw StreamHandler.
 *
 * <p>After construction the instance must be registered with the DirectMetricsFactory
 */
class DirectTupleOutput extends DirectTuple {

  /**
   * Get a RuntimeStream that uses the metrics factory to provide the tuples. Used for direct Java
   * to Java ports in the mock framework.
   */
  static DirectTupleOutput newAllJavaOutput(
      int portIndex, RuntimeStream<Tuple> handler, DirectMetricsFactory operatorMetricsFactory) {
    DirectTupleOutput dto = new DirectTupleOutput(handler);
    operatorMetricsFactory.addPort(portIndex, dto);
    return dto;
  }

  /** Counters are provided by this class. */
  private DirectTupleOutput(final RuntimeStream<Tuple> handler) {
    super(
        handler,
        new RuntimeMetric.CounterMetricValue(),
        new RuntimeMetric.CounterMetricValue(),
        new RuntimeMetric.CounterMetricValue());
  }

  RuntimeMetric.MetricValue getMetricValue(OperatorMetrics.OutputPortMetric name) {
    switch (name) {
      case nTuplesSubmitted:
        return nTuples;
      case nWindowPunctsSubmitted:
        return nWindowPuncts;
      case nFinalPunctsSubmitted:
        return nFinalPuncts;
      default:
        throw new UnsupportedOperationException(name.name());
    }
  }
}
