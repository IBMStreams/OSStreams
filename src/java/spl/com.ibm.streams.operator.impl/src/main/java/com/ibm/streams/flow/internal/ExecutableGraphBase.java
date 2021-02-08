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

package com.ibm.streams.flow.internal;

import com.ibm.streams.flow.execute.ExecutableGraph;
import com.ibm.streams.flow.handlers.StreamHandler;
import com.ibm.streams.flow.internal.Connections.Connection;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.logtrace.Setup;
import com.ibm.streams.operator.internal.logtrace.Trace;
import com.ibm.streams.operator.internal.logtrace.TraceHandler;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric;
import com.ibm.streams.operator.internal.runtime.api.PE;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.CompletionService;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

/** An executable graph of Java primitive operators. */
abstract class ExecutableGraphBase<E extends ExecutableGraph<E>> implements ExecutableGraph<E> {

  private final Map<Integer, OpExec> operators = new HashMap<Integer, OpExec>();

  private final PE<RuntimeMetric> pe;

  private Future<E> initializer;
  private Future<E> allPortsReady;
  private Future<E> shutdown;
  private Future<E> complete;

  private final Connections connections = new Connections();

  /**
   * Threads that are used to notify actions on the operator for initialize, allPortsReady and
   * shutdown.
   */
  private final ExecutorService executor = Executors.newCachedThreadPool();

  ExecutableGraphBase(OpGraph graph) throws Exception {

    pe = new PE<RuntimeMetric>(graph.getPEModel(), new StandaloneMetricsFactory());

    Setup.setupForEmbedded();

    for (OpInvocation<?> opInvoke : graph.operators())
      operators.put(opInvoke.getOperatorIndex(), new OpExec(this, opInvoke, pe));
  }

  final OpExec getOperator(int index) {
    return operators.get(index);
  }

  final Connections getConnections() {
    return connections;
  }

  /**
   * Call initialize() for each operator through OpExec which calls OperatorAdapter.initialize()
   * which will call Operator.initialize(). Calls are made in background threads.
   *
   * <p>Order of execution is not defined across the operators.
   */
  synchronized Future<E> initialize() {
    if (initializer != null) return initializer;

    return initializer =
        executor.submit(
            new Callable<E>() {

              @Override
              public E call() throws Exception {
                try {
                  for (OpExec oe : operators.values()) {
                    oe.getAdapter().initialize();
                  }
                } catch (Exception e) {
                  cleanup();
                  throw e;
                }
                return getGraph();
              }
            });
  }

  synchronized Future<E> allPortsReady() {

    if (allPortsReady != null) return allPortsReady;

    verifySuccessfulInitialization();

    return allPortsReady =
        executor.submit(
            new Callable<E>() {

              @Override
              public E call() throws Exception {
                makeConnections();
                try {
                  for (OpExec oe : operators.values()) {
                    oe.getAdapter().allPortsReady();
                  }
                } catch (Exception e) {
                  cleanup();
                  throw e;
                }
                return getGraph();
              }
            });
  }

  synchronized Future<E> shutdown() {

    if (shutdown != null) return shutdown;

    verifySuccessfulInitialization();
    return shutdown =
        executor.submit(
            new Callable<E>() {

              @Override
              public E call() throws Exception {
                try {
                  for (OpExec oe : operators.values()) {
                    oe.getAdapter().shutdown();
                  }
                } finally {
                  cleanup();
                }
                return getGraph();
              }
            });
  }

  /** Wait for each operator to indicate it has no more work to do. */
  synchronized Future<E> completion() {

    if (complete != null) return complete;

    verifySuccessfulAllPortsReady();

    return complete =
        executor.submit(
            new Callable<E>() {

              @Override
              public E call() throws Exception {

                final CompletionService<Object> cs =
                    new ExecutorCompletionService<Object>(executor);

                final int operatorCount = operators.size();
                // Submit all the completers, each will get its own thread
                for (OpExec oe : operators.values()) {
                  final OpExec oef = oe;
                  cs.submit(
                      new Callable<Object>() {

                        @Override
                        public Object call() throws Exception {
                          try {
                            oef.complete();
                          } catch (Throwable e) {
                            cleanup();
                            if (e instanceof Error) throw (Error) e;
                            throw (Exception) e;
                          }
                          return null;
                        }
                      });
                }

                // and then wait for them.
                for (int i = 0; i < operatorCount; i++) {
                  try {
                    cs.take().get();
                  } catch (ExecutionException e) {
                    Throwable t = e.getCause();
                    if (t instanceof Error) throw (Error) t;
                    throw (Exception) t;
                  }
                }

                return getGraph();
              }
            });
  }

  private synchronized void verifySuccessfulInitialization() {
    if (!verifySuccessfulExecution(initializer)) {
      cleanup();
      throw new IllegalStateException(
          new Message(Key.EG_EXECUTABLE_GRAPH_NOT_INITIALIZED, "").getLocalizedMessageAndPrefix());
    }
  }

  private synchronized void verifySuccessfulAllPortsReady() {
    verifySuccessfulInitialization();
    if (!verifySuccessfulExecution(allPortsReady)) {
      cleanup();
      throw new IllegalStateException(
          new Message(Key.EG_EXECUTABLE_GRAPH_NOT_NOTIFIED, "").getLocalizedMessageAndPrefix());
    }
  }

  private static boolean verifySuccessfulExecution(Future<?> future) {
    if (future == null || !future.isDone() || future.isCancelled()) return false;

    try {
      future.get();
      return true;
    } catch (Exception e) {
      return false;
    }
  }

  /**
   * Cleanup after failure by calling cleanup on each OpExec which calls OperatorAdapter.cleanup().
   */
  void cleanup() {
    for (OpExec oe : operators.values()) {
      oe.cleanup();
    }
    executor.shutdown();
  }

  @Override
  public Future<E> execute() {
    return executor.submit(
        new Callable<E>() {

          @Override
          public E call() throws Exception {
            try {
              initialize().get();
              try {
                allPortsReady().get();
                completion().get();
              } catch (InterruptedException ie) {
                // Issue an asynchronous shutdown
                // if initialized and the call
                // is interrupted due to a
                // cancel on the future.
                shutdown();
                return getGraph();
              }
              shutdown().get();
            } catch (ExecutionException e) {
              Throwable t = e.getCause();
              if (t instanceof Error) throw (Error) t;
              if (t instanceof Exception) throw (Exception) t;
            }
            return getGraph();
          }
        });
  }

  @Override
  public E executeToCompletion() throws Exception {
    return waitForFuture(execute());
  }

  protected E waitForFuture(Future<E> future) throws Exception {
    try {
      return future.get();
    } catch (ExecutionException e) {
      throw unwrapExecutionException(e);
    }
  }

  protected final Exception unwrapExecutionException(ExecutionException e) {
    final Throwable t = e.getCause();
    if (t instanceof Error) throw (Error) t;
    return (Exception) t;
  }

  /** Connect all the operators in the graph. */
  private void makeConnections() {

    // Make the actual connections.
    for (Connection conn : getConnections().allConnections()) {
      OpExec outputOp = getOperator(conn.getOutputOperatorIndex());
      OpExec inputOp = getOperator(conn.getInputOperatorIndex());
      StreamHandler<Tuple> handler = inputOp.getInputHandler(conn.getInputPortIndex());
      outputOp.addHandlerToOutputPort(conn.getOutputPortIndex(), intercept(handler));
    }
  }

  /**
   * Allow sub-classes to intercept input by returning a different handler to the one that is
   * connected to the input port. This implementation does not add anything.
   */
  protected StreamHandler<Tuple> intercept(StreamHandler<Tuple> input) {
    return input;
  }

  @Override
  public E setTraceLevel(final TraceLevel level) {
    final Trace traceLevel = TraceHandler.convertLevelToEnum(level);
    final String newLevel = Setup.setTraceLevel(traceLevel);
    pe.traceChange(newLevel);
    return getGraph();
  }

  @Override
  protected void finalize() throws Throwable {
    cleanup();
    super.finalize();
  }

  abstract E getGraph();
}
