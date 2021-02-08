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

package com.ibm.streams.operator.internal.runtime.api;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.internal.management.MXBeanControl;
import com.ibm.streams.operator.internal.management.NotifyInfo;
import com.ibm.streams.operator.internal.metrics.MetricBean;
import com.ibm.streams.operator.management.MetricMXBean;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.management.OperatorManagement;
import com.ibm.streams.operator.management.OperatorManagement.Type;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import java.util.Hashtable;
import javax.management.MBeanNotificationInfo;
import javax.management.MBeanRegistration;
import javax.management.MBeanServer;
import javax.management.MalformedObjectNameException;
import javax.management.Notification;
import javax.management.ObjectName;

public final class OperatorInfo extends NotifyInfo
    implements OperatorContextMXBean, MBeanRegistration {

  private final Adapter adapter;
  private ObjectName name;

  private boolean allPortsReady;
  private boolean shutdown;

  OperatorInfo(Adapter adapter) {
    this.adapter = adapter;
  }

  private synchronized ObjectName getObjectName() {
    return name;
  }

  private synchronized ObjectName setObjectName(ObjectName name) {
    return this.name = name;
  }

  private final OperatorContext getContext() {
    return adapter.getContext();
  }

  private final OperatorMetrics getMetrics() {
    return getContext().getMetrics();
  }

  @Override
  public MBeanNotificationInfo[] getNotificationInfo() {
    MBeanNotificationInfo lifecycle =
        new MBeanNotificationInfo(
            new String[] {ALL_PORTS_READY, SHUTDOWN, SHUTDOWN_COMPLETE},
            Notification.class.getName(),
            "Operator life cycle notifications");

    MBeanNotificationInfo metrics =
        new MBeanNotificationInfo(
            new String[] {CUSTOM_METRIC_CREATED},
            Notification.class.getName(),
            "Operator metrics notifications");

    return new MBeanNotificationInfo[] {lifecycle, metrics};
  }

  @Override
  public String getName() {
    return adapter.getOperatorName();
  }

  @Override
  public String getImplementationClassName() {
    return adapter.getOperatorClassName();
  }

  @Override
  public long[] getThreadIds() {
    return adapter.getUserThreadFactory().getActiveThreadIds();
  }

  @Override
  public ObjectName registerCustomMetric(String name) {

    final Metric metric = getMetrics().getCustomMetric(name);
    Hashtable<String, String> objectNameProperties = new Hashtable<String, String>();
    MetricMXBean.MetricType.CUSTOM.addType(objectNameProperties);
    return registerMetric(metric, metric.getName(), objectNameProperties);
  }

  @Override
  public void registerInputPortMetrics(int port) {
    registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nTuplesProcessed);
    registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nWindowPunctsProcessed);
    registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nFinalPunctsProcessed);

    // Only register threaded port (queue) metrics if the input port is actually
    // threaded.
    if (getMetrics().getInputPortMetric(port, OperatorMetrics.InputPortMetric.queueSize).getValue()
        != 0) {
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.queueSize);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nTuplesQueued);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nTuplesDropped);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nWindowPunctsQueued);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nFinalPunctsQueued);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.maxItemsQueued);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.recentMaxItemsQueued);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.recentMaxItemsQueuedInterval);
      registerInputPortMetric(port, OperatorMetrics.InputPortMetric.nEnqueueWaits);
    }
  }

  private void registerInputPortMetric(int port, OperatorMetrics.InputPortMetric ipm) {

    final Metric metric = getMetrics().getInputPortMetric(port, ipm);
    Hashtable<String, String> objectNameProperties = new Hashtable<String, String>();
    MetricMXBean.MetricType.OPERATOR_INPUT_PORT.addType(objectNameProperties);
    objectNameProperties.put("port", Integer.toString(port));
    registerMetric(metric, ipm.name(), objectNameProperties);
  }

  @Override
  public void registerOutputPortMetrics(int port) {
    registerOutputPortMetric(port, OperatorMetrics.OutputPortMetric.nTuplesSubmitted);
    registerOutputPortMetric(port, OperatorMetrics.OutputPortMetric.nWindowPunctsSubmitted);
    registerOutputPortMetric(port, OperatorMetrics.OutputPortMetric.nFinalPunctsSubmitted);
  }

  private void registerOutputPortMetric(int port, OperatorMetrics.OutputPortMetric opm) {

    final Metric metric = getMetrics().getOutputPortMetric(port, opm);
    Hashtable<String, String> objectNameProperties = new Hashtable<String, String>();
    MetricMXBean.MetricType.OPERATOR_OUTPUT_PORT.addType(objectNameProperties);
    objectNameProperties.put("port", Integer.toString(port));
    registerMetric(metric, opm.name(), objectNameProperties);
  }

  private ObjectName registerMetric(
      Metric metric, String name, Hashtable<String, String> objectNameProperties) {
    try {
      ObjectName on =
          OperatorManagement.getOperatorSpecificName(
              getName(), OperatorManagement.Type.METRIC, name, objectNameProperties);
      MXBeanControl.platformRegister(new MetricBean(metric), on, true);
      return on;
    } catch (MalformedObjectNameException e) {
      throw new IllegalStateException(e);
    }
  }

  @Override
  public void notify(String type) {
    if (ALL_PORTS_READY.equals(type)) setAllPortsReady();
    else if (SHUTDOWN.equals(type)) setShutdown();

    super.notify(type);
    if (SHUTDOWN_COMPLETE.equals(type)) {
      MXBeanControl.platformUnregister(getObjectName());
    }
  }

  /** Register all the MXBeans for this operator. */
  void registerMXBeans() {
    // The OperatorContextMXBean
    MXBeanControl.platformRegister(this, null, false);
  }

  @Override
  public ObjectName preRegister(MBeanServer server, ObjectName name) throws Exception {
    return setObjectName(
        OperatorManagement.getName(Type.OPERATORCONTEXT, adapter.getOperatorName()));
  }

  @Override
  public void postRegister(Boolean registrationDone) {}

  /** Remove all operator dependent beans. */
  @Override
  public void preDeregister() throws Exception {
    MXBeanControl.unregisterOperatorMXBeans(adapter.getOperatorName());
  }

  @Override
  public void postDeregister() {
    // If this is the last operator then unregister the PE MXBean
    if (MXBeanControl.queryAll(OperatorManagement.Type.OPERATORCONTEXT).isEmpty())
      MXBeanControl.platformUnregister(OperatorManagement.getPEName());
  }

  /** Cleanup by deregistering this. */
  void cleanup() {
    final ObjectName myName = getObjectName();
    if (myName != null) MXBeanControl.platformUnregister(myName);
  }

  private synchronized void setAllPortsReady() {
    allPortsReady = true;
  }

  @Override
  public synchronized boolean isAllPortsReady() {
    return allPortsReady;
  }

  public synchronized void setShutdown() {
    this.shutdown = true;
  }

  @Override
  public synchronized boolean isShutdown() {
    return shutdown;
  }

  @Override
  public int getChannel() {
    return getContext().getChannel();
  }

  @Override
  public int getLocalChannel() {
    return getContext().getLocalChannel();
  }

  @Override
  public int getMaxChannels() {
    return getContext().getMaxChannels();
  }

  @Override
  public int getLocalMaxChannels() {
    return getContext().getLocalMaxChannels();
  }

  @Override
  public String getLogicalName() {
    return getContext().getLogicalName();
  }
}
