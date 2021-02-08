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

/**
 * Runtime metric that is returned to operators as a read-only metric for ports etc. Delegates the
 * handling of values to its MetricValue interface to ensure that the value cannot be changed by the
 * caller through reflection.
 */
public final class RuntimeMetric extends ReadOnlyMetric {

  private final MetricValue mv;

  public RuntimeMetric(MetricValue mv, String name, Kind kind, String description) {
    super(name, kind, description);
    this.mv = mv;
  }

  @Override
  public final long getValue() {
    return mv.getValue();
  }

  /** Get the value of metric to be wrapped by RuntimeMetric. */
  public static interface MetricValue {
    public long getValue();

    public void bump();

    public void decrement();

    public void setValue(long value);

    public void setMaxValue(long value);
  }

  /** MetricValue for a counter with a single bump() method to increment a value. */
  public static class CounterMetricValue implements MetricValue {
    private long value;

    @Override
    public synchronized long getValue() {
      return value;
    }

    @Override
    public synchronized void bump() {
      value++;
    }

    @Override
    public synchronized void decrement() {
      value--;
    }

    @Override
    public void setValue(long value) {
      throw new UnsupportedOperationException();
    }

    @Override
    public void setMaxValue(long value) {
      throw new UnsupportedOperationException();
    }
  }

  /** MetricValue for a gauge metric whose value can be set to any arbitrary value at any time. */
  public static class GaugeMetricValue implements MetricValue {
    private long value;

    @Override
    public synchronized long getValue() {
      return value;
    }

    @Override
    public synchronized void bump() {
      ++value;
    }

    @Override
    public synchronized void decrement() {
      --value;
    }

    @Override
    public synchronized void setValue(long value) {
      this.value = value;
    }

    @Override
    public synchronized void setMaxValue(long value) {
      if (value > this.value) this.value = value;
    }
  }

  /** MetricValue with a constant value. */
  public static final class ConstantMetricValue implements MetricValue {
    private final long value;

    public ConstantMetricValue(long value) {
      this.value = value;
    }

    @Override
    public long getValue() {
      return value;
    }

    @Override
    public void bump() {
      throw new UnsupportedOperationException();
    }

    @Override
    public void decrement() {
      throw new UnsupportedOperationException();
    }

    @Override
    public void setValue(long value) {
      throw new UnsupportedOperationException();
    }

    @Override
    public void setMaxValue(long value) {
      throw new UnsupportedOperationException();
    }
  }

  /** Constant MetricValue with value zero. */
  public static final MetricValue ZERO_METRIC = new ConstantMetricValue(0l);
}
