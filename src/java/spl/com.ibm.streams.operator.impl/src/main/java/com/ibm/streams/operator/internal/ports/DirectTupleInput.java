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
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.metrics.OperatorMetrics;

/**
 * Class that processes input tuples and punctuation and implements input port finalization and
 * operator port metrics before handing off the tuples to the operator level RuntimeStream.
 *
 * <p>After construction the instance must be registered with the DirectMetricsfactory
 */
public class DirectTupleInput extends DirectTuple {

  /**
   * Create a handler for an operator's input port where the metrics and finalization handling is in
   * Java.
   */
  public static RuntimeStream<Tuple> newDirectTupleInput(
      OperatorAdapter adapter, int portIndex, DirectMetricsFactory operatorMetricsFactory) {

    RuntimeStream<Tuple> handler = adapter.getInputPortHandler(portIndex);
    DirectTupleInput dti = new DirectTupleInput(handler);
    operatorMetricsFactory.addPort(portIndex, dti);
    return dti;
  }

  DirectTupleInput(final RuntimeStream<Tuple> handler) {
    super(
        handler,
        new RuntimeMetric.CounterMetricValue(),
        new RuntimeMetric.CounterMetricValue(),
        new RuntimeMetric.CounterMetricValue());
  }

  RuntimeMetric.MetricValue getMetricValue(OperatorMetrics.InputPortMetric name) {
    switch (name) {
      case nTuplesProcessed:
        return nTuples;
      case nWindowPunctsProcessed:
        return nWindowPuncts;
      case nFinalPunctsProcessed:
        return nFinalPuncts;

        // For this non-queuing implementation queuing metrics
        // are all set to zero as a constant
      case nFinalPunctsQueued:
      case nTuplesQueued:
      case nWindowPunctsQueued:
      case queueSize:
      case nTuplesDropped:
      case maxItemsQueued:
      case recentMaxItemsQueued:
      case recentMaxItemsQueuedInterval:
      case nEnqueueWaits:
        return RuntimeMetric.ZERO_METRIC;
      default:
        throw new UnsupportedOperationException(name.name());
    }
  }
}
