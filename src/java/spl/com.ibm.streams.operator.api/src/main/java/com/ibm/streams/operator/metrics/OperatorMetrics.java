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
import java.util.Map;

/**
 * OperatorMetrics provides access to metrics related to an operator and its ports. An operator may
 * access read-only information about its ports and define custom metrics that will be available for
 * external monitoring of the application.
 *
 * @see Metric
 */
public interface OperatorMetrics {

  /**
   * Create a custom metric specific to this operator.
   * The initial value of the metric is zero. If Metric <code>name</code> already exists, does nothing
   * starting with IBM Streams v4.3.0 and throws an <code>IllegalArgumentException<code> for versions prior to 4.3.0.
   * @param name Name of the metric, the actual value used for the metric's name is trimmed using <code>trim()</code>.
   * @param description Description of the metric.
   * @param kind Kind of the metric
   * @return The Metric object for the custom metric.
   * @exception IllegalArgumentException Metric <code>name</code> already exists (prior to IBM Streams v4.3.0).
   * @exception IllegalArgumentException Empty String is passed for <code>name</code>.
   * @exception NullPointerException An argument (<code>name</code>,
   * <code>description</code> or <code>kind</code>) is null.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public Metric createCustomMetric(String name, String description, Metric.Kind kind);

  /**
   * Return a custom metric either previously created by createCustomMetric or specified in the
   * operator model.
   *
   * @param name Name of the metric, the actual value used for the metric's name is trimmed using
   *     <code>trim()</code>.
   * @return The Metric object for the custom metric.
   * @exception IllegalArgumentException Metric <code>name</code> does not exist.
   * @exception NullPointerException <code>name</code> is null.
   * @see OperatorMetrics#createCustomMetric(String, String,
   *     com.ibm.streams.operator.metrics.Metric.Kind)
   * @exception IllegalStateException Operator has been shutdown and the metric has not been
   *     accessed previously.
   */
  public Metric getCustomMetric(String name);

  /**
   * Returns a read-only view of the operator's custom metrics. The map's key is a custom metric's
   * name and its value is the corresponding Metric object. Includes all custom metrics including
   * those defined by the operator model and those defined dynamically using SPL or {@link
   * OperatorMetrics#createCustomMetric(String, String,
   * com.ibm.streams.operator.metrics.Metric.Kind)} The returned map is a view of the current state
   * of the operator's custom metrics and its contents can change if custom metrics are being
   * created dynamically.
   *
   * @return A read-only map of the operator's custom metrics.
   * @since InfoSphere&reg; Streams Version 2.0.0.3
   */
  public Map<String, Metric> getCustomMetrics();

  /**
   * Get a read-only Metric for a specific operator input port.
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
   * Get a read-only Metric for a specific operator output port.
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
   * Get a read-only system Metric of this operator.
   *
   * @param name Enumeration representing the name of the metric.
   * @return A read-only Metric.
   * @exception NullPointerException <code>name</code> is null.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public Metric getOperatorMetric(SystemMetric name);

  /**
   * Enumeration representing metric names for operator input ports. Any Metric object returned for
   * one of these names is read only.
   */
  public enum InputPortMetric {
    // IMPLEMENTATION NOTE - Order of enums must match the
    // C++ OperatorMetrics::InputPortMetricName
    /** Number of tuples processed by the port. */
    nTuplesProcessed,
    /** Number of tuples dropped by the port. */
    nTuplesDropped,
    /** Number of tuples currently queued by the port. */
    nTuplesQueued,
    /**
     * Number of {@link com.ibm.streams.operator.StreamingData.Punctuation#WINDOW_MARKER
     * WINDOW_MARKER} punctuation marks processed by the port.
     */
    nWindowPunctsProcessed,
    /**
     * Number of {@link com.ibm.streams.operator.StreamingData.Punctuation#FINAL_MARKER
     * FINAL_MARKER} punctuation marks processed by the port.
     */
    nFinalPunctsProcessed,
    /**
     * Number of {@link com.ibm.streams.operator.StreamingData.Punctuation#WINDOW_MARKER
     * WINDOW_MARKER} punctuation marks currently queued by the port.
     */
    nWindowPunctsQueued,
    /**
     * Number of {@link com.ibm.streams.operator.StreamingData.Punctuation#FINAL_MARKER
     * FINAL_MARKER} punctuation marks currently queued by the port.
     */
    nFinalPunctsQueued,
    /**
     * Size of the queue for a threaded port, or 0 if the port is not threaded.
     *
     * @since InfoSphere&reg; Streams Version 2.0.0.3
     */
    queueSize,
    /**
     * Largest number of items queued.
     *
     * @since IBM&reg; Streams Version 4.2.3.0
     */
    maxItemsQueued,
    /**
     * Recent largest number of items queued.
     *
     * @since IBM&reg; Streams Version 4.2.3.0
     */
    recentMaxItemsQueued,
    /**
     * Interval to determine the recent largest number of items queued (milliseconds).
     *
     * @since IBM&reg; Streams Version 4.2.3.0
     */
    recentMaxItemsQueuedInterval,
    /**
     * Number of waits due to a full queue.
     *
     * @since IBM&reg; Streams Version 4.2.3.0
     */
    nEnqueueWaits;

    /**
     * Convenience method to get the metric for a specific operator port.
     *
     * @param context Operator's context.
     * @param port Operator port.
     * @return The Metric object for this operator input port metric.
     * @exception IllegalStateException Operator has been shutdown and the metric has not been
     *     accessed previously.
     */
    public Metric getMetric(OperatorContext context, int port) {
      return context.getMetrics().getInputPortMetric(port, this);
    }
  }

  /**
   * Enumeration representing metric names for operator output ports. Any Metric object returned for
   * one of these names is read only.
   */
  public enum OutputPortMetric {
    // IMPLEMENTATION NOTE - Order of enums must match the
    // C++ OperatorMetrics::OutputPortMetricName

    /** Number of tuples submitted on an output port. */
    nTuplesSubmitted,
    /**
     * Number of window punctuation marks submitted on an output port.
     *
     * @see com.ibm.streams.operator.StreamingData.Punctuation#WINDOW_MARKER
     */
    nWindowPunctsSubmitted,
    /**
     * Number of final punctuation marks submitted on an output port.
     *
     * @see com.ibm.streams.operator.StreamingData.Punctuation#FINAL_MARKER
     */
    nFinalPunctsSubmitted;

    /**
     * Convenience method to get the Metric for a specific operator port.
     *
     * @param context Operator's context.
     * @param port Operator port.
     * @return The Metric object for this operator output port metric.
     * @exception IllegalStateException Operator has been shutdown and the metric has not been
     *     accessed previously.
     */
    public Metric getMetric(OperatorContext context, int port) {
      return context.getMetrics().getOutputPortMetric(port, this);
    }
  }

  /**
   * Enumeration representing system metric names for the operator. Any Metric object returned for
   * one of these names is read only.
   */
  public enum SystemMetric {
    /** Relative cost of the operator within PE scope */
    relativeOperatorCost;

    /**
     * Convenience method to get the Metric for a specific operator port.
     *
     * @param context Operator's context.
     * @return The Metric object for this operator system metric.
     * @exception IllegalStateException Operator has been shutdown.
     */
    public Metric getMetric(OperatorContext context) {
      return context.getMetrics().getOperatorMetric(this);
    }
  }
}
