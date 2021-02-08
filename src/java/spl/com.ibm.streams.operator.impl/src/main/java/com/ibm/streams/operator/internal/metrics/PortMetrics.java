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

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumMap;
import java.util.List;

/**
 * Holder for port metrics, for PE or operator metrics.
 *
 * @param <S>
 * @param <I>
 * @param <O>
 */
class PortMetrics<S extends ReadOnlyMetric, I extends Enum<I>, O extends Enum<O>> {

  private final PortMetricsFactory<S, I, O> factory;
  private final List<EnumMap<I, S>> inputs;
  private final List<EnumMap<O, S>> outputs;

  PortMetrics(
      PortMetricsFactory<S, I, O> factory,
      int inputPortCount,
      Class<I> inputNameClass,
      int outputPortCount,
      Class<O> outputNameClass) {
    this.factory = factory;
    inputs = getPortList(inputPortCount, inputNameClass);
    outputs = getPortList(outputPortCount, outputNameClass);
  }

  private static <E extends Enum<E>, S> List<EnumMap<E, S>> getPortList(
      int size, Class<E> enumClass) {
    if (size == 0) return Collections.emptyList();

    List<EnumMap<E, S>> list = new ArrayList<EnumMap<E, S>>(size);
    for (int i = 0; i < size; i++) list.add(new EnumMap<E, S>(enumClass));

    return list;
  }

  public synchronized ReadOnlyMetric getInputPortMetric(int port, I name) {
    if (name == null) throw new NullPointerException();

    S metric = inputs.get(port).get(name);
    if (metric == null) {
      metric = factory.getInputMetric(port, name);
      inputs.get(port).put(name, metric);
    }
    return metric;
  }

  public synchronized ReadOnlyMetric getOutputPortMetric(int port, O name) {
    if (name == null) throw new NullPointerException();

    S metric = outputs.get(port).get(name);
    if (metric == null) {
      metric = factory.getOutputMetric(port, name);
      outputs.get(port).put(name, metric);
    }
    return metric;
  }
}
