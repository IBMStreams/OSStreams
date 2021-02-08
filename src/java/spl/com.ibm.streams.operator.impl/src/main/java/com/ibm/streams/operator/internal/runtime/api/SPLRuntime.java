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

import static com.ibm.streams.operator.internal.logtrace.Setup.rtTrace;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.internal.logtrace.Setup;
import com.ibm.streams.operator.internal.metrics.OperatorMetricsFactory;
import com.ibm.streams.operator.internal.metrics.ReadOnlyMetric;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.pemodel.PEModelStreams;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.state.ConsistentRegionContext.DrainResult;
import com.ibm.streams.operator.types.Timestamp;
import java.util.List;

public class SPLRuntime {

  private static SPLRuntime runtime;

  private final PEModel model;
  private final Adapter[] operators;
  private PE<? extends ReadOnlyMetric> pe;

  /**
   * Load the definitions of the PE (operators, types etc.) from the runtime model (AADL,
   * AugmentedPEType). Clients can use the params argument to pass extra attributes which are not
   * part of the model.
   */
  public static synchronized SPLRuntime load(final String runtimeModel, final String[] params)
      throws Exception {
    return runtime = new SPLRuntime(runtimeModel, params);
  }

  public static synchronized SPLRuntime get() {
    return runtime;
  }

  private SPLRuntime(final String runtimeModel, final String[] params) throws Exception {
    model = new PEModelStreams(runtimeModel, params);
    operators = new Adapter[model.getOperatorCount()];
  }

  public PEModel getModel() {
    return model;
  }

  public synchronized void setPE(PE<? extends ReadOnlyMetric> pe, PEContext peContext) {
    this.pe = pe;
    this.peContext = peContext;
  }

  synchronized PE<? extends ReadOnlyMetric> getPE() {
    return pe;
  }

  synchronized PEContext getPEContext() {
    return peContext;
  }

  public synchronized void registerOperator(Adapter adapter) {
    checkShutdown();
    operators[adapter.getOperatorIndex()] = adapter;
    assert pe == adapter.getExecutionContext().getPE();
  }

  /**
   * When a new Java thread is created through the operator's ThreadFactory, it will call
   * set(OperatorAdapter adapter) on this thread local. This JNI specific version saves the operator
   * adapter and also sets the operator tracker for the thread to be the operator index. This index
   * is used for thread trace/log to ensure the correct operator name.
   */
  private static final ThreadLocal<Adapter> currentAdapter = new ThreadLocal<Adapter>();

  public static Adapter setActive(Adapter adapter) {
    if (adapter == null) {
      currentAdapter.remove();
      return null;
    }
    final Adapter previous = currentAdapter.get();
    currentAdapter.set(adapter);
    adapter.getExecutionContext().setOperatorTracker(adapter);
    return previous;
  }

  /** Return true if a Java operator is currently active. */
  public static boolean isJavaOperatorActive() {
    return currentAdapter.get() != null;
  }

  public static Adapter getActive() {
    Adapter adapter = currentAdapter.get();
    if (adapter != null) return adapter;
    SPLRuntime runtime = SPLRuntime.get();
    if (runtime != null) {
      adapter = runtime.getActiveAdapterFromPE();
      if (adapter != null) return adapter;
    }
    throw new IllegalStateException();
  }

  private PEContext peContext;
  private Thread shutdownThread;
  private boolean shutdown;

  private synchronized void checkShutdown() {
    if (shutdown) throw new IllegalStateException("PE has shutdown");
  }

  private synchronized Adapter getActiveAdapterFromPE() {
    final int operatorIndex = peContext.getCurrentOperator();
    Adapter adapter = operators[operatorIndex];
    if (adapter == null) adapter = createAdapterFromModel(operatorIndex);
    return adapter;
  }

  private Adapter createAdapterFromModel(int operatorIndex) {

    // Create a shutdown thread that drives the shutdown notifications
    // into the Java elements created for a non-Java operator.
    synchronized (this) {
      checkShutdown();
      if (shutdownThread == null) {
        shutdownThread =
            new Thread(
                new Runnable() {

                  @Override
                  public void run() {
                    rtTrace.fine(
                        Thread.currentThread().getName() + " started, waiting for shutdown.");
                    peContext.waitForShutdown();
                    Setup.rtTrace.fine(Thread.currentThread().getName() + " shutdown requested.");
                    synchronized (this) {
                      shutdown = true;
                      shutdownThread = null;
                      for (Adapter adapter : operators) {
                        if (adapter == null) continue;
                        if (adapter.isJavaOp()) continue;
                        try {
                          adapter.getManagement().notify(OperatorContextMXBean.SHUTDOWN);
                          adapter.getManagement().notify(OperatorContextMXBean.SHUTDOWN_COMPLETE);
                        } catch (Throwable e) {
                          rtTrace.log(
                              TraceLevel.DEBUG,
                              "Exception during non-Java operator shutdown of Java context",
                              e);
                        }
                      }
                    }
                    Setup.rtTrace.fine(Thread.currentThread().getName() + " shutdown complete.");
                  }
                },
                "SPL Java Runtime PE shutdown monitor");

        shutdownThread.setDaemon(true);
        shutdownThread.start();
      }
    }

    ExecutionContext ec =
        new ExecutionContext() {

          @Override
          public ProcessingElement getPE() {
            return SPLRuntime.this.getPE();
          }

          @Override
          public OperatorMetricsFactory<?, ?> getMetricsFactory() {
            throw new UnsupportedOperationException();
          }

          @Override
          public List<StreamingOutput<OutputTuple>> getOutputPorts() {
            throw new UnsupportedOperationException();
          }

          @Override
          public void setOperatorTracker(Adapter adapter) {
            ;
          }

          @Override
          public void resetOperatorTracker() {
            ;
          }

          @Override
          public long getConsistentRegionSequenceId() {
            throw new UnsupportedOperationException();
          }

          @Override
          public boolean makeConsistent() {
            throw new UnsupportedOperationException();
          }

          @Override
          public String[] getDomainCredentials() {
            return getPEContext().getDomainCredentials();
          }

          @Override
          public void mustResetConsistentRegion() {
            throw new UnsupportedOperationException();
          }

          @Override
          public int getConsistentRegionResetAttempt() {
            throw new UnsupportedOperationException();
          }

          @Override
          public void resetConsistentRegion() {
            throw new UnsupportedOperationException();
          }

          @Override
          public void setMustRethrowException() {
            throw new UnsupportedOperationException();
          }

          @Override
          public boolean mustRethrowException() {
            throw new UnsupportedOperationException();
          }

          @Override
          public void enableNonBlockingCheckpoint() {
            throw new UnsupportedOperationException();
          }

          @Override
          public DrainResult makeConsistentNonBlocking() {
            throw new UnsupportedOperationException();
          }

          @Override
          public void submitWatermark(Timestamp wm) {
            throw new UnsupportedOperationException();
          }

          @Override
          public boolean submitWatermark(Timestamp wm, int port) {
            throw new UnsupportedOperationException();
          }
        };
    Adapter adapter = ModelOperator.addModelOperator(this, operatorIndex, ec);
    registerOperator(adapter);
    return adapter;
  }
}
