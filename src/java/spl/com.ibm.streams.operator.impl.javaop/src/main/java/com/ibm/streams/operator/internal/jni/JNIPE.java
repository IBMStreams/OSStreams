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

package com.ibm.streams.operator.internal.jni;

import static com.ibm.streams.operator.internal.logtrace.Setup.rtTrace;

import com.ibm.streams.operator.internal.metrics.PEMetricsFactory;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.runtime.api.PE;
import com.ibm.streams.operator.internal.runtime.api.PEContext;
import com.ibm.streams.operator.internal.runtime.api.SPLRuntime;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.PEMetrics.InputPortMetric;
import com.ibm.streams.operator.metrics.PEMetrics.OutputPortMetric;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.math.BigDecimal;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.management.Notification;
import javax.management.NotificationListener;

final class JNIPE extends PE<JNISystemMetric> {
  private static final String[] INFO_SYSTEM_PROPERTIES = {
    "java.vendor",
    "java.home",
    "java.version",
    "java.fullVersion",
    "java.specification.name",
    "java.specification.version",
  };

  private static final Object sync = new Object();

  // Singleton object for the PE we are running in.
  private static JNIPE PE;

  private final PEContext context;

  /** Get the current PE, can return null if no-pe has been created yet */
  static JNIPE getPE() {
    synchronized (sync) {
      return (JNIPE) PE;
    }
  }

  static void createPE(SPLRuntime runtime) {
    synchronized (sync) {
      // TODO: Pointer check for PE metrics
      final JNIPointerState dummy =
          new JNIPointerState() {
            @Override
            public void validPointers() {}
          };

      PE =
          new JNIPE(
              runtime.getModel(),
              new PEMetricsFactory<JNISystemMetric>() {

                @Override
                public JNISystemMetric getInputMetric(int port, InputPortMetric name) {
                  final long metricPtr =
                      JNIBridge.getPortMetric(0L, port, true, false, name.ordinal());
                  return new JNISystemMetric(
                      dummy,
                      metricPtr,
                      JNIBridge.getMetricName(metricPtr),
                      Metric.Kind.values()[JNIBridge.getMetricKind(metricPtr)],
                      JNIBridge.getMetricDescription(metricPtr));
                }

                @Override
                public JNISystemMetric getOutputMetric(int port, OutputPortMetric name) {
                  final long metricPtr =
                      JNIBridge.getPortMetric(0L, port, true, true, name.ordinal());
                  return new JNISystemMetric(
                      dummy,
                      metricPtr,
                      JNIBridge.getMetricName(metricPtr),
                      Metric.Kind.values()[JNIBridge.getMetricKind(metricPtr)],
                      JNIBridge.getMetricDescription(metricPtr));
                }
              });

      runtime.setPE(PE, PE.getPEContext());
    }

    // For Debugging log the key system properties describing
    // which jvm is being run.
    for (String sp : INFO_SYSTEM_PROPERTIES) {
      String spv = System.getProperty(sp);
      if (spv != null) {
        rtTrace.log(TraceLevel.INFO, sp + "=" + spv);
      }
    }

    verifyJavaVersion();
  }

  /**
   * Verify that the JVM selected for this PE is at least Java 7 using the platform API
   * specification level.
   */
  private static void verifyJavaVersion() {
    final String name = System.getProperty("java.specification.name");
    if (name != null) {
      if (name.equals("Java Platform API Specification")) {
        final String version = System.getProperty("java.specification.version");
        try {
          final BigDecimal rv = new BigDecimal(version);
          final BigDecimal minumumVersion = new BigDecimal("1.7");

          if (rv.compareTo(minumumVersion) >= 0) return;

          throw new IllegalStateException(
              new Message(Key.SPL_RUNTIME_UNSUPPORTED_JAVA_SPEC_VERSION, "")
                  .getLocalizedMessageAndPrefix());
        } catch (NumberFormatException e) {
        }
      }
    }
    throw new IllegalStateException(
        new Message(Key.SPL_RUNTIME_UNDETERMINED_JAVA_SPEC_VERSION, "")
            .getLocalizedMessageAndPrefix());
  }

  private JNIPE(PEModel peModel, PEMetricsFactory<JNISystemMetric> factory) {
    super(peModel, factory);
    configuredHostName = (isStandalone()) ? null : peModel.getConfiguredHostName();
    this.context = newContext();
  }

  private PEContext newContext() {
    return new PEContext() {

      @Override
      public int getCurrentOperator() {
        return JNIBridge.getOperatorTracker();
      }

      @Override
      public String[] getDomainCredentials() {
        return JNIBridge.getCredentials();
      }

      @Override
      public void waitForShutdown() {
        JNIBridge.waitForShutdown();
      }

      @Override
      public Map<String, String> getApplicationConfiguration(String name) {
        return JNISPLRuntimeBridge.getApplicationConfiguration(name);
      }
    };
  }

  PEContext getPEContext() {
    return context;
  }

  /*
   * Information related to the operators executing
   * within this PE in the SPL runtime.
   */

  private final Map<Integer, SPLRuntimeOperator> operators =
      Collections.synchronizedMap(new HashMap<Integer, SPLRuntimeOperator>());

  /**
   * Add a runtime operator and then see if we have enough information to determine if any of the
   * ports can be fused.
   */
  void addRuntimeOperator(SPLRuntimeOperator op) {
    synchronized (operators) {
      operators.put(op.getOperatorIndex(), op);
    }
  }

  /**
   * Can return null if we haven't seen the the operator, either because it is not Java, or because
   * it hasn't been initialized yet.
   */
  SPLRuntimeOperator getRuntimeOperator(int operatorIndex) {
    return operators.get(operatorIndex);
  }

  private List<NotificationListener> visualizationListeners = new ArrayList<NotificationListener>();

  /** Stores visualizing state in the current PE */
  private boolean visualizing;

  /**
   * Visualization notifications. These are not full JMX as we don't expose these to user operators.
   */
  synchronized void addVisualizationListener(NotificationListener listener) {
    visualizationListeners.add(listener);
  }

  void visualizationChange(boolean visualizing) {

    final NotificationListener[] listeners;
    synchronized (this) {
      this.visualizing = visualizing;
      listeners =
          visualizationListeners.toArray(new NotificationListener[visualizationListeners.size()]);
    }

    // Simple notification object, just informing the handler is enough.
    final Notification notification =
        new Notification("visualization", this, System.currentTimeMillis());
    for (NotificationListener listener : listeners) listener.handleNotification(notification, null);
  }

  /**
   * Returns whether the current PE is being visualized at any operator port
   *
   * @return true if there is a visualization view on any operator port.
   */
  synchronized boolean isVisualizing() {
    return visualizing;
  }

  private final String configuredHostName;

  @Override
  public InetAddress getConfiguredHost() throws UnknownHostException {
    if (isStandalone()) return super.getConfiguredHost();

    return InetAddress.getByName(configuredHostName);
  }

  /** Connect java operators in cases where they connect to each other without JNI. */
  public void connectOperators() throws Exception {
    for (SPLRuntimeOperator rtOp : operators.values()) {
      rtOp.connect(this);
    }
  }

  public void addView(int operatorIndex, int outputPortIndex, long propertiesHandle) {
    SPLRuntimeOperator operator = operators.get(operatorIndex);
    if (operator != null) {
      operator.addView(outputPortIndex, propertiesHandle);
    }
  }

  public void deleteView(int operatorIndex, int outputPortIndex, long propertiesHandle) {
    SPLRuntimeOperator operator = operators.get(operatorIndex);
    if (operator != null) {
      operator.deleteView(outputPortIndex, propertiesHandle);
    }
  }

  @Override
  public void setTagData(String tagName, Map<String, String> tagValues) {
    JNISPLRuntimeBridge.setTagData(tagName, tagValues);
  }

  @Override
  public Map<String, String> getApplicationConfiguration(String name) {
    return context.getApplicationConfiguration(name);
  }
}
