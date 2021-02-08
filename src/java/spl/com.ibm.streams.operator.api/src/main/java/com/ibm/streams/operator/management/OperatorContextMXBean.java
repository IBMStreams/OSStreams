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

package com.ibm.streams.operator.management;

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.management.OperatorManagement.Type;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.metrics.OperatorMetrics.InputPortMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics.OutputPortMetric;
import javax.management.ObjectName;

/**
 * The management bean for a Java operator's context.
 *
 * <p>{@code OperatorMXBean} exposes information about the operator, its context and notifications
 * related to its life cycle. Within the processing element (PE) an {@code OperatorContextMXBean}
 * represents each Java operator executing within the PE. The management bean is automatically
 * registered with the platform MBeanServer before {@link Operator#initialize(OperatorContext)} is
 * called. <br>
 * An operator may unregister its management bean from the platform MBeanServer if required.
 * Unregistering will cause all management beans for the operator to be unregistered (for example
 * any {@link MetricMXBean} that have been registered for the operator. The management bean is
 * unregistered automatically once {@link Operator#shutdown()} completes.
 *
 * <p>The {@code ObjectName} for uniquely identifying the MXBean for an operator within the PE in
 * the platform MBean server is: <br>
 * {@code com.ibm.streams.operator:type=operatorcontext,name=operator_name} <br>
 * where {@code operator_name} is the value returned by {@link OperatorContext#getName()} and {@link
 * OperatorContextMXBean#getName()}
 *
 * <p><B>Notifications</B> <br>
 * {@code OperatorContextMXBean} is a {@code NotificationEmitter} that emits notifications related
 * to the operator's life cycle.
 *
 * <UL>
 *   <LI>{@link OperatorContextMXBean#ALL_PORTS_READY} - Notification the operator's ports are ready
 *       and it is free to submit tuples. The notification's {@code userData} will be null.
 *   <LI>{@link OperatorContextMXBean#SHUTDOWN} - Notification the operator is to be shutdown.
 *   <LI>{@link OperatorContextMXBean#SHUTDOWN_COMPLETE} - Notification the operator has completed
 *       its shutdown. The notification's {@code userData} will be null.
 *   <LI>{@link OperatorContextMXBean#CUSTOM_METRIC_CREATED} - Notification the operator has created
 *       a custom metric. The notification's {@code userData} will be the name of the custom metric.
 * </UL>
 *
 * @see OperatorManagement#STREAMS_OPERATOR_JMX_DOMAIN
 * @see Type#OPERATORCONTEXT
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface OperatorContextMXBean {

  /**
   * Notification the operator's ports are ready and it is free to submit tuples. <br>
   * The notification's {@code userData} will be null. <br>
   * The value of this notification identifier is {@value}.
   */
  public static final String ALL_PORTS_READY = "com.ibm.streams.operator.lifecycle.allPortsReady";

  /**
   * Notification the operator is to be shutdown. <br>
   * The notification's {@code userData} will be null. <br>
   * The value of this notification identifier is {@value}.
   */
  public static final String SHUTDOWN = "com.ibm.streams.operator.lifecycle.shutdown.notify";

  /**
   * Notification the operator has completed its shutdown by returning from its {@link
   * com.ibm.streams.operator.Operator#shutdown()} method. <br>
   * The notification's {@code userData} will be null. <br>
   * The value of this notification identifier is {@value}.
   */
  public static final String SHUTDOWN_COMPLETE =
      "com.ibm.streams.operator.lifecycle.shutdown.complete";

  /**
   * Notification the operator has created a custom metric. <br>
   * The notification's {@code userData} will be the name of the newly created {@link
   * com.ibm.streams.operator.metrics.Metric}. <br>
   * The value of this notification identifier is {@value}.
   *
   * @see com.ibm.streams.operator.metrics.OperatorMetrics#createCustomMetric(String, String,
   *     com.ibm.streams.operator.metrics.Metric.Kind)
   */
  public static final String CUSTOM_METRIC_CREATED = "com.ibm.streams.operator.metric.createCustom";

  /**
   * Get the name of the operator.
   *
   * <p>The operator name is unique across the application. For operators in a parallel region, the
   * name includes the {@link #getChannel() channel} they are in.
   *
   * @return the name of the operator.
   * @see #getLogicalName()
   * @see OperatorContext#getName()
   */
  public String getName();

  /**
   * Get the Java class name implementing the operator
   *
   * @return Class name implementing {@link com.ibm.streams.operator.Operator}.
   */
  public String getImplementationClassName();

  /**
   * Return the current set of thread identifiers for live threads created by this operator's
   * ThreadFactory. The returned identifiers may be used with the platform ThreadMXBean to analyze
   * then operator's background threads. Some threads included in the returned array may have been
   * terminated when this method returns.
   *
   * @see OperatorContext#getThreadFactory()
   */
  public long[] getThreadIds();

  /**
   * Register a {@link MetricMXBean} for a custom metric. The custom metric must already exist.
   *
   * <p>The {@code ObjectName} for uniquely identifying the metric's MXBean in the platform MBean
   * server is: <br>
   * {@code
   * com.ibm.streams.operator:type=metric,metric=custom,name=metric_name,operator=operator_name}
   * <br>
   * where:
   *
   * <UL>
   *   <LI>{@code metric_name} is the name of the metric, passed in as {@code name}
   *   <LI>{@code operator_name} is the value returned by {@link OperatorContext#getName()} and
   *       {@link OperatorContextMXBean#getName()}
   * </UL>
   *
   * @param name Name of the custom metric to have a {@code MetricMXBean} registered in the platform
   *     MBeanServer.
   * @return Object name for the registered {@code MetricMXBean}.
   * @throws IllegalStateException The custom metric does not exist.
   * @see OperatorMetrics#getCustomMetric(String)
   */
  public ObjectName registerCustomMetric(String name);

  /**
   * Register an {@link MetricMXBean} for all valid metrics for the operator input port at index
   * {@code port}. {@code MetricMXBean} for input port queue related metrics are only registered if
   * the input port is queued ({@link InputPortMetric#queueSize} metric is not zero). <br>
   * The {@code ObjectName} for uniquely identifying any input port metric's MXBean in the platform
   * MBean server is: <br>
   * {@code
   * com.ibm.streams.operator:type=metric,metric=operatorInputPort,port=port_index,name=metric_name,operator=operator_name}
   * <br>
   * where:
   *
   * <UL>
   *   <LI>{@code port_index} is the index of the input port (from {@code port})
   *   <LI>{@code metric_name} is the name of the metric from {@link InputPortMetric#name()}
   *   <LI>{@code operator_name} is the value returned by {@link OperatorContext#getName()} and
   *       {@link OperatorContextMXBean#getName()}
   * </UL>
   *
   * @param port Index of input port.
   */
  public void registerInputPortMetrics(int port);

  /**
   * Register an {@link MetricMXBean} for all metrics for the operator output port at index {@code
   * port}. <br>
   * The {@code ObjectName} for uniquely identifying any output port metric's MXBean in the platform
   * MBean server is: <br>
   * {@code
   * com.ibm.streams.operator:type=metric,metric=operatorOutputPort,port=port_index,name=metric_name,operator=operator_name}
   * <br>
   * where:
   *
   * <UL>
   *   <LI>{@code port_index} is the index of the input port (from {@code port})
   *   <LI>{@code metric_name} is the name of the metric from {@link OutputPortMetric#name()}
   *   <LI>{@code operator_name} is the value returned by {@link OperatorContext#getName()} and
   *       {@link OperatorContextMXBean#getName()}
   * </UL>
   *
   * @param port Index of input port.
   */
  public void registerOutputPortMetrics(int port);

  /**
   * Has the operator been notified of port readiness.
   *
   * @return True if the operator been notified of port readiness, false otherwise.
   * @see Operator#allPortsReady()
   * @see #ALL_PORTS_READY Port readiness notification
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public boolean isAllPortsReady();

  /**
   * Has the operator been notified of shutdown..
   *
   * @return True if the operator has been notified of shutdown, false otherwise.
   * @see Operator#shutdown()
   * @see #SHUTDOWN Shutdown notification
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public boolean isShutdown();

  /**
   * Get the index of the parallel channel the operator is in.
   *
   * <p>If the operator is in a parallel region, this method returns a value from 0 to N-1, where N
   * is the {@link #getMaxChannels() number of channels in the parallel region}; otherwise it
   * returns -1.
   *
   * @return the index of the parallel channel this operator is in, if the operator is in a parallel
   *     region, or -1 if the operator is not in a parallel region.
   * @see OperatorContext#getChannel()
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public int getChannel();

  /**
   * Get the index of the parallel channel in the most immediate parallel region the operator is in.
   *
   * <p>If the operator is in a parallel region, this method returns a value from 0 to N-1, where N
   * is the {@link #getMaxChannels() number of channels in the parallel region}; otherwise it
   * returns -1.
   *
   * @return the index of the parallel channel this operator is in, if the operator is in a parallel
   *     region, or -1 if the operator is not in a parallel region.
   * @see OperatorContext#getLocalChannel()
   * @since IBM Streams Version 4.2
   */
  public int getLocalChannel();

  /**
   * Get the total number of parallel channels for the parallel region that the operator is in. If
   * the operator is not in a parallel region, this method returns 0.
   *
   * @return the number of parallel channels for the parallel region that this operator is in, or 0
   *     if the operator is not in a parallel region.
   * @see OperatorContext#getMaxChannels()
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public int getMaxChannels();

  /**
   * Get the total number of parallel channels for the most immediate parallel region that the
   * operator is in. If the operator is not in a parallel region, this method returns 0.
   *
   * @return the number of parallel channels for the parallel region that this operator is in, or 0
   *     if the operator is not in a parallel region.
   * @see OperatorContext#getLocalMaxChannels()
   * @since IBM Streams Version 4.2
   */
  public int getLocalMaxChannels();

  /**
   * Get the logical name of the operator.
   *
   * @return the logical name of the operator.
   * @see OperatorContext#getLogicalName()
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public String getLogicalName();
}
