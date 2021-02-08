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

import static com.ibm.streams.operator.internal.logtrace.Setup.opTrace;
import static com.ibm.streams.operator.internal.messages.Messages.illegalArgument;

import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public class OpMetrics<C extends AbstractMetric, S extends ReadOnlyMetric>
    extends PortMetrics<S, OperatorMetrics.InputPortMetric, OperatorMetrics.OutputPortMetric>
    implements OperatorMetrics {

  private final OperatorMetricsFactory<C, S> factory;
  private final OperatorAdapter adapter;
  private final Map<String, Metric> customMetrics;

  public OpMetrics(
      OperatorAdapter adapter,
      OperatorMetricsFactory<C, S> factory,
      int inputPortCount,
      int outputPortCount) {
    super(factory, inputPortCount, InputPortMetric.class, outputPortCount, OutputPortMetric.class);
    this.adapter = adapter;
    this.factory = factory;
    customMetrics = Collections.synchronizedMap(new HashMap<>());
    if (factory != null) {
      String[] names = factory.getCustomMetricNames(0);
      if (names != null) for (String name : names) getCustomMetricFromFactory(name);
    }
  }

  @Override
  public final Metric createCustomMetric(String name, String description, Metric.Kind kind) {
    if (name == null || description == null || kind == null) throw new NullPointerException();
    name = name.trim();
    description = description.trim();
    if (name.isEmpty()) throw illegalArgument(Key.SPL_RUNTIME_CUSTOM_METRIC_EMPTY_NAME);
    synchronized (customMetrics) {
      if (customMetrics.containsKey(name)) {
        // see rtc 41226; this used to throw an exception, now logs a warning
        Object[] params = new Object[1];
        params[0] = name;
        opTrace.log(
            TraceLevel.WARN,
            new Message(Key.SPL_RUNTIME_CUSTOM_METRIC_ALREADY_EXISTS, params)
                .getLocalizedMessageAndPrefix());
        return customMetrics.get(name);
      }
      C metric = factory.newCustomMetric(name, description, kind);
      customMetrics.put(name, metric);
      adapter.getManagement().notify(OperatorContextMXBean.CUSTOM_METRIC_CREATED, name);
      return metric;
    }
  }

  @Override
  public Metric getCustomMetric(String name) {
    if (name == null) throw new NullPointerException();
    Metric metric = customMetrics.get(name);
    if (metric == null) throw illegalArgument(Key.SPL_RUNTIME_CUSTOM_METRIC_NOT_EXISTS, name);
    return metric;
  }

  /**
   * Store in Java a custom metric that was already created for the operator by defining it in the
   * operator model.
   *
   * @param name Name of the metric to get.
   */
  private void getCustomMetricFromFactory(String name) {
    Metric metric = factory.getCustomMetric(name);
    if (metric == null) throw illegalArgument(Key.SPL_RUNTIME_CUSTOM_METRIC_NOT_EXISTS, name);
    customMetrics.put(name, metric);
  }

  /**
   * Return a read-only map (default implementation for if (metric == null) throw
   * illegalArgument(Keys.OM_NOT_EXIST, name); AbstractMap) of the custom metrics.
   */
  @Override
  public Map<String, Metric> getCustomMetrics() {
    return Collections.unmodifiableMap(customMetrics);
  }

  public ReadOnlyMetric getOperatorMetric(OperatorMetrics.SystemMetric name) {
    if (name != OperatorMetrics.SystemMetric.relativeOperatorCost) throw new NullPointerException();
    S metric = factory.getOperatorMetric(name);
    return metric;
  }
}
