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

package com.ibm.streams.operator.metrics;

/**
 * A metric represents a measurement of an element in either an operator or a processing element. A
 * metric's value is an signed 64-bit integer value, represented as a primitive long in the Java
 * Operator API.
 *
 * @see com.ibm.streams.operator.metrics.OperatorMetrics
 * @see com.ibm.streams.operator.metrics.PEMetrics
 */
public interface Metric {

  /**
   * Kind of metric. The kind of the metric only indicates the behavior of value, it does not impose
   * any semantics on the value. The kind is typically used by tooling applications.
   */
  public enum Kind {

    /*
     * Implementation note. The order of the enum matches the order
     * in the C++ code.
     */

    /** A gauge metric observes a value that is continuously variable with time. */
    GAUGE,

    /** A counter metric observes a value that represents a count of an occurrence. */
    COUNTER,

    /**
     * A time metric represents a point in time. The representation of time and units are specific
     * to a metric and defined by the application.
     */
    TIME
  }

  /**
   * Get the name of this metric.
   *
   * @return Name of the metric.
   */
  public String getName();

  /**
   * Get the description of this metric.
   *
   * @return Description of the metric.
   */
  public String getDescription();

  /**
   * Get the kind of this metric.
   *
   * @return Kind of the metric.
   */
  public Kind getKind();

  /**
   * Get the value of this metric.
   *
   * @return Value of the metric.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public long getValue();

  /**
   * Set the value of this metric.
   *
   * @param value Value to set this metric to.
   * @exception UnsupportedOperationException Metric is read-only.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public void setValue(long value);

  /**
   * Increment the value of this metric.
   *
   * @param delta Incremental value to add to the metric's value.
   * @exception UnsupportedOperationException Metric is read-only.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public void incrementValue(long delta);

  /**
   * Increment the value of this metric by one.
   *
   * @exception UnsupportedOperationException Metric is read-only.
   * @exception IllegalStateException Operator has been shutdown.
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public void increment();
}
