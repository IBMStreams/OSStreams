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

package com.ibm.streams.flow.javaprimitives;

import com.ibm.streams.flow.declare.InputPortDeclaration;
import com.ibm.streams.flow.declare.OperatorInvocation;
import com.ibm.streams.flow.declare.OutputPortDeclaration;
import com.ibm.streams.flow.execute.ExecutableGraph;
import com.ibm.streams.flow.handlers.StreamHandler;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.logging.LoggerNames;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import java.util.concurrent.Future;

/**
 * An executable flow graph of Java primitive operators with support for disconnected input and
 * output ports. <br>
 * For the purpose of testing, behavior such as the separation of initialization and ports-ready
 * notification, this class extends {@link ExecutableGraph} to control the execution of the graph.
 * Actions against the graph can be executed asynchronously to allow the testing of the graph and
 * the operators in it during the actions, such as during {@link #initialize() operator
 * initialization}. <br>
 * Test programs can use {@link #getInputTester(InputPortDeclaration)} to submit tuples and
 * punctuation marks directly to the disconnected input ports. <br>
 * Output from the disconnected output ports is verified by using handlers that are attached to the
 * output ports with {@link #registerStreamHandler(OutputPortDeclaration, StreamHandler)}.
 *
 * <p>Java platform logging and Apache Log4j may continue to be used by operators executing within
 * the graph. It is generally assumed that the framework being used has correctly initialized the
 * loggers, by installing required handlers or appenders. If Apache Log4j has not been initialized
 * then when a graph is created then the Apache Log4j root logger will have a {@code
 * ConsoleAppender} pointing to {@code System.err} installed. <br>
 * The dynamic setting of SPL trace level is simulated by calling {@link
 * #setTraceLevel(com.ibm.streams.operator.logging.TraceLevel)}. <br>
 * The SPL log level may simulated by setting the level of the logger named {@link
 * LoggerNames#LOG_FACILITY com.ibm.streams.operator.log} for the logging system used by operators.
 * If both Java platform logging and Apache Log4j are used by operators in the graph, then both
 * loggers should have their level set to the desired level.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface JavaTestableGraph extends ExecutableGraph<JavaTestableGraph> {

  /**
   * Asynchronously initialize all the {@link Operator} invocations in the graph. When the returned
   * {@code Future} completes successfully, all the operators in the graph have been initialized.
   * <br>
   * For each operator, this results in calls to:
   *
   * <OL>
   *   <LI>Runtime {@link ContextCheck context check methods}
   *   <LI>{@link Operator#initialize(com.ibm.streams.operator.OperatorContext)}
   * </OL>
   *
   * The order of initialization is not defined across the operators.
   *
   * <p>After successful initialization the graph is an a running state and is waiting for {@link
   * #allPortsReady()} or {@link #execute()} to be called, which allows tuple processing to start.
   * <br>
   * Once initialized the graph must be shutdown using {@link #shutdown()} to ensure any resources
   * opened by the operator or this graph are closed or shutdown.
   *
   * @return A {@code Future} that completes once all operator invocations have been initialized.
   * @see com.ibm.streams.operator.compile.OperatorContextChecker
   * @see Operator#initialize(com.ibm.streams.operator.OperatorContext)
   */
  public Future<JavaTestableGraph> initialize();

  /**
   * Asynchronously notify all the operator invocations in the graph that the ports are ready, and
   * the submission of tuples can start. When the returned {@code Future} completes successfully,
   * all the operators in the graph have been notified. <br>
   * This call results in a call to {@link Operator#allPortsReady()} for each operator. The order of
   * the port readiness notification is not defined across the operators.
   *
   * @return A {@code Future} that completes after all the operator invocations are notified of port
   *     readiness.
   * @see Operator#allPortsReady()
   */
  public Future<JavaTestableGraph> allPortsReady();

  /**
   * Return a {@code Future} that completes when all the operator invocations in the graph have
   * completed. When an operator without input ports completes (a source operator) {@link
   * Punctuation#FINAL_MARKER final marks} are sent to all the output ports of the operator. <br>
   * Once the graph is complete {@code Future.get()} returns {@code this}.
   *
   * @return A {@code Future} that completes after all the operator invocations have completed.
   */
  public Future<JavaTestableGraph> completion();

  /**
   * Wait for all the operator invocations to complete.
   *
   * @return this
   * @throws Exception
   */
  public JavaTestableGraph waitForCompletion() throws Exception;

  /**
   * Asynchronously notify all the {@link Operator} invocations in the graph of a shutdown request.
   * When the returned {@code Future} completes successfully, all then operators in the graph have
   * been notified. <br>
   * This call results in a call to {@link Operator#shutdown()} for each operator. The order of
   * shutdown is not defined across the operators.
   *
   * @return A {@code Future} that completes after all the operator invocations are shut down.
   * @see Operator#shutdown()
   */
  public Future<JavaTestableGraph> shutdown();

  /**
   * Register a {@link StreamHandler} to a disconnected output port. This call can be made multiple
   * times to connect multiple {@link StreamHandler} instances to a single output port, which allows
   * for the testing of multiple aspects of the stream output. <br>
   * All the tuples that are submitted by the output port are delivered to {@code processor}.
   *
   * @param port Disconnected output port
   * @param handler The handler that receives tuples and punctuation marks from the disconnected
   *     output port.
   * @exception IllegalStateException The output port is connected.
   */
  public void registerStreamHandler(OutputPortDeclaration port, StreamHandler<Tuple> handler);

  /**
   * Obtain a {@link StreamingOutput} that can be used to submit test tuples and punctuation marks
   * to a disconnected input port.
   *
   * @param port Disconnected input port
   * @return StreamingOutput that can be used to submit tuples and punctuation to the input port.
   * @exception IllegalStateException The input port is connected.
   */
  public StreamingOutput<OutputTuple> getInputTester(InputPortDeclaration port);

  /**
   * Return a proxy to the {@link OperatorContextMXBean} for the operator invocation named {@code
   * name}.
   *
   * @param name Name of the operator invocation
   * @return OperatorContextMXBean for the named operator.
   */
  public OperatorContextMXBean getOperatorContextMBean(String name);

  /**
   * Return a proxy to the {@link OperatorContextMXBean} for an operator invocation.
   *
   * @param operator Declaration of the operator invocation
   * @return OperatorContextMXBean for the named operator
   */
  public OperatorContextMXBean getOperatorContextMBean(
      OperatorInvocation<? extends Operator> operator);

  /**
   * Return the reference to the instance of {@code T} executing in this graph for invocation
   * represented by {@code operator}. This may be used by testing code to verify state of the
   * operator
   *
   * @param <T> Class of operator invocation that implements {@link Operator}.
   * @param operator Operator invocation
   * @return Reference to the instance of {@code T} that is running within this graph.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public <T extends Operator> T getOperatorInstance(OperatorInvocation<T> operator);
}
