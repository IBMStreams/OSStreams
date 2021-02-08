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

import com.ibm.streams.operator.OperatorContext;

/**
 * PEMetrics provides access to metrics related to an processing element's ports.
 *
 * @see com.ibm.streams.operator.ProcessingElement#getMetrics()
 * @see Metric
 */
public interface PEMetrics {

  /**
   * Get a read-only Metric for a specific processing element input port.
   *
   * @param port Port for the metric.
   * @param name Enumeration representing the name of the metric.
   * @return A read-only Metric.
   * @exception NullPointerException <code>name</code> is null.
   * @exception IllegalStateException Operator has been shutdown and the metric has not been
   *     accessed previously.
   */
  public Metric getInputPortMetric(int port, InputPortMetric name);

  /**
   * Get a read-only Metric for a specific processing element output port.
   *
   * @param port Port for the metric.
   * @param name Enumeration representing the name of the metric.
   * @return A read-only Metric.
   * @exception NullPointerException <code>name</code> is null.
   * @exception IllegalStateException Operator has been shutdown and the metric has not been
   *     accessed previously.
   */
  public Metric getOutputPortMetric(int port, OutputPortMetric name);

  /**
   * Enumeration representing metric names for processing element input ports. Any Metric object
   * returned for one of these names is read only.
   */
  public enum InputPortMetric {
    // IMPLEMENTATION NOTE - Order of enums must match the
    // C++ OperatorMetrics::InputPortMetricName
    /** Number of tuples processed by the port. */
    nTuplesProcessed,
    /** Number of bytes processed by the port. */
    nTupleBytesProcessed,
    /** Number of window punctuation marks processed by the port. */
    nWindowPunctsProcessed,
    /** Number of final punctuation marks processed by the port. */
    nFinalPunctsProcessed;

    /**
     * Convenience method to get the Metric for a specific processing element port.
     *
     * @param context Operator's context.
     * @param port Processing Element port.
     * @return The Metric object for this processing element input port metric.
     * @exception IllegalStateException Operator has been shutdown and the metric has not been
     *     accessed previously.
     */
    public Metric getMetric(OperatorContext context, int port) {
      return context.getPE().getMetrics().getInputPortMetric(port, this);
    }
  }

  /**
   * Enumeration representing metric names for processing element output ports. Any Metric object
   * returned for one of these names is read only.
   */
  public enum OutputPortMetric {
    // IMPLEMENTATION NOTE - Order of enums must match the
    // C++ OperatorMetrics::OutputPortMetricName
    /** Number of tuples submitted by the port. */
    nTuplesSubmitted,
    /** Number of bytes submitted by the port. */
    nTupleBytesSubmitted,
    /** Number of window punctuation marks submitted by the port. */
    nWindowPunctsSubmitted,
    /** Number of final punctuation marks submitted by the port. */
    nFinalPunctsSubmitted,
    /** Number of broken connections that have occurred on the port. */
    nBrokenConnections,
    /** Number of required connections currently connecting on the port. */
    nRequiredConnecting,
    /** Number of optional connections currently connecting on the port. */
    nOptionalConnecting,
    /** Total number of tuples transmitted by the port to all consumers. */
    nTuplesTransmitted,
    /** Total number of bytes of tuples transmitted by the port to all consumers. */
    nTupleBytesTransmitted,
    /** Number of PE input streams connected to the port. */
    nConnections;

    /**
     * Convenience method to get the Metric for a specific processing element port.
     *
     * @param context Operator's context.
     * @param port Processing Element port.
     * @return The Metric object for this processing element output port metric.
     * @exception IllegalStateException Operator has been shutdown and the metric has not been
     *     accessed previously.
     */
    public Metric getMetric(OperatorContext context, int port) {
      return context.getPE().getMetrics().getOutputPortMetric(port, this);
    }
  }
}
