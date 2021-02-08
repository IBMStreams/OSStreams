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
import com.ibm.streams.operator.metrics.Metric.Kind;
import com.ibm.streams.operator.metrics.OperatorMetrics.InputPortMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics.OutputPortMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics.SystemMetric;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;

public abstract class RuntimeMetricsFactory
    implements OperatorMetricsFactory<CustomMetric, RuntimeMetric> {

  @Override
  public final CustomMetric newCustomMetric(String name, String description, Kind kind) {
    return new CustomMetric(name, kind, description);
  }

  /**
   * No predefined metrics so justr return null. Each operator maintains a cache of its own metrics.
   */
  @Override
  public final CustomMetric getCustomMetric(String name) {
    return null;
  }

  /**
   * Java runtime knows about all the custom metrics so return null to indicate no more are known
   * about.
   */
  @Override
  public final String[] getCustomMetricNames(int currentCount) {
    return null;
  }

  @Override
  public RuntimeMetric getInputMetric(int port, InputPortMetric name) {
    return newInputPortMetric(port, name, getMetricValue(port, name));
  }

  @Override
  public RuntimeMetric getOutputMetric(int port, OutputPortMetric name) {
    return newOutputPortMetric(port, name, getMetricValue(port, name));
  }

  public RuntimeMetric getOperatorMetric(SystemMetric name) {
    return null;
  }

  protected abstract RuntimeMetric.MetricValue getMetricValue(int port, InputPortMetric name);

  protected abstract RuntimeMetric.MetricValue getMetricValue(int port, OutputPortMetric name);

  public static RuntimeMetric newInputPortMetric(
      int port, InputPortMetric ipm, RuntimeMetric.MetricValue mv) {
    final String name = ipm.name() + port;
    final String description =
        new Message(Key.SPL_RUNTIME_INPUT_PORT_METRIC_NAME, ipm.name(), port).getLocalizedMessage();

    final Metric.Kind kind;
    switch (ipm) {
      case nFinalPunctsProcessed:
      case nTuplesDropped:
      case nTuplesProcessed:
      case nWindowPunctsProcessed:
      case nEnqueueWaits:
        kind = Metric.Kind.COUNTER;
        break;
      case nFinalPunctsQueued:
      case nTuplesQueued:
      case nWindowPunctsQueued:
      case queueSize:
      case maxItemsQueued:
      case recentMaxItemsQueued:
        kind = Metric.Kind.GAUGE;
        break;
      case recentMaxItemsQueuedInterval:
        kind = Metric.Kind.TIME;
        break;
      default:
        throw new UnsupportedOperationException(ipm.name());
    }

    return new RuntimeMetric(mv, name, kind, description);
  }

  public static RuntimeMetric newOutputPortMetric(
      int port, OutputPortMetric opm, RuntimeMetric.MetricValue mv) {
    final String name = opm.name() + port;
    final String description =
        new Message(Key.SPL_RUNTIME_OUTPUT_PORT_METRIC_NAME, opm.name(), port)
            .getLocalizedMessage();

    final Metric.Kind kind;
    switch (opm) {
      case nFinalPunctsSubmitted:
      case nTuplesSubmitted:
      case nWindowPunctsSubmitted:
        kind = Metric.Kind.COUNTER;
        break;
      default:
        throw new UnsupportedOperationException(opm.name());
    }

    return new RuntimeMetric(mv, name, kind, description);
  }
}
