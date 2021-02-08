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

import com.ibm.streams.flow.declare.InputPortDeclaration;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric.MetricValue;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.metrics.OperatorMetrics;

public class DirectTupleThreadedInput extends DirectTupleInput {

  private static MetricValue queueSizeMetric;
  private static MetricValue tuplesQueuedMetric;
  private static MetricValue windowPunctsQueuedMetric;
  private static MetricValue finalPunctsQueuedMetric;
  private static MetricValue tuplesDroppedMetric;
  private static MetricValue maxItemsQueuedMetric;
  private static MetricValue recentMaxItemsQueuedMetric;
  private static MetricValue recentMaxItemsQueuedIntervalMetric;
  private static MetricValue enqueueWaitsMetric;

  public static RuntimeStream<Tuple> newThreadedDirectTupleInput(
      OperatorAdapter adapter,
      int portIndex,
      DirectMetricsFactory operatorMetricsFactory,
      InputPortDeclaration.CongestionPolicy congestionPolicy,
      int queueSize,
      boolean singleThreadedOnInput) {
    RuntimeStream<Tuple> innerHandler = adapter.getInputPortHandler(portIndex);
    queueSizeMetric = new RuntimeMetric.ConstantMetricValue(queueSize > 0 ? queueSize : 1);
    tuplesQueuedMetric = new RuntimeMetric.GaugeMetricValue();
    windowPunctsQueuedMetric = new RuntimeMetric.GaugeMetricValue();
    finalPunctsQueuedMetric = new RuntimeMetric.GaugeMetricValue();
    tuplesDroppedMetric = new RuntimeMetric.CounterMetricValue();
    maxItemsQueuedMetric = new RuntimeMetric.GaugeMetricValue();
    recentMaxItemsQueuedMetric = new RuntimeMetric.GaugeMetricValue();
    recentMaxItemsQueuedIntervalMetric = new RuntimeMetric.ConstantMetricValue(300 * 1000);
    enqueueWaitsMetric = new RuntimeMetric.CounterMetricValue();
    ThreadedPortHandler threadedHandler =
        new ThreadedPortHandler(
            innerHandler,
            congestionPolicy,
            singleThreadedOnInput,
            adapter,
            queueSizeMetric,
            tuplesQueuedMetric,
            windowPunctsQueuedMetric,
            finalPunctsQueuedMetric,
            tuplesDroppedMetric,
            maxItemsQueuedMetric,
            recentMaxItemsQueuedMetric,
            enqueueWaitsMetric);
    DirectTupleThreadedInput dti = new DirectTupleThreadedInput(threadedHandler);
    operatorMetricsFactory.addPort(portIndex, dti);
    adapter.addThreadedHandler(threadedHandler, portIndex);
    return dti;
  }

  DirectTupleThreadedInput(final RuntimeStream<Tuple> innerHandler) {
    super(innerHandler);
  }

  @Override
  RuntimeMetric.MetricValue getMetricValue(OperatorMetrics.InputPortMetric name) {
    switch (name) {
      case nTuplesProcessed:
        return nTuples;
      case nWindowPunctsProcessed:
        return nWindowPuncts;
      case nFinalPunctsProcessed:
        return nFinalPuncts;

      case nFinalPunctsQueued:
        return finalPunctsQueuedMetric;
      case nTuplesQueued:
        return tuplesQueuedMetric;
      case nWindowPunctsQueued:
        return windowPunctsQueuedMetric;
      case queueSize:
        return queueSizeMetric;
      case nTuplesDropped:
        return tuplesDroppedMetric;
      case maxItemsQueued:
        return maxItemsQueuedMetric;
      case recentMaxItemsQueued:
        return recentMaxItemsQueuedMetric;
      case recentMaxItemsQueuedInterval:
        return recentMaxItemsQueuedIntervalMetric;
      case nEnqueueWaits:
        return enqueueWaitsMetric;

      default:
        throw new UnsupportedOperationException(name.name());
    }
  }
}
