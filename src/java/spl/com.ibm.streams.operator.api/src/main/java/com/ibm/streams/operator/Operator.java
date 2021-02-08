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

package com.ibm.streams.operator;

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.management.OperatorLifeCycle;
import java.util.Map;

/**
 * Operator for processing and producing streams of tuples. An {@code Operator} is an event
 * processor for {@link Tuple tuples} and {@link StreamingData.Punctuation punctuation marks} that
 * arrives on its input ports. Incoming tuples result in a call to {@link
 * Operator#process(StreamingInput, Tuple)} and punctuation marks a call to {@link
 * Operator#processPunctuation(StreamingInput, StreamingData.Punctuation)}. These calls are invoked
 * by the SPL runtime. Operator implementations must be thread-safe as different threads may invoke
 * the operator's methods and tuples and punctuation marks may arrive concurrently, even on the same
 * input port. <br>
 * An implementation of {@code Operator} becomes a <em>Java primitive operator</em> when it is
 * annotated with {@link com.ibm.streams.operator.model.PrimitiveOperator}. A Java primitive
 * operator becomes part of an SPL toolkit and thus can be called from an SPL application. The
 * {@link com.ibm.streams.operator.model} package contains a number of annotations that allow an
 * implementation of {@code Operator} to fully describe itself as a Java primitive operator.
 *
 * <p>An {@code Operator} has an associated {@link OperatorContext} that describes its execution
 * environment, including references to objects representing its input ports ({@link
 * com.ibm.streams.operator.StreamingInput StreamingInput}) and output ports ({@link
 * com.ibm.streams.operator.StreamingOutput StreamingOutput}).
 *
 * <p>An {@code Operator} is initialized by the SPL runtime invoking {@link
 * Operator#initialize(OperatorContext)} passing the <code>OperatorContext</code> for the specific
 * SPL operator invocation. Once initialized the <code>
 * Operator</code> is notified that its ports are ready by the SPL runtime invoking {@link
 * Operator#allPortsReady()}. <br>
 * When the processing element(PE) that hosts the operator is being shutdown, the SPL runtime will
 * invoke {@link Operator#shutdown()}, which may take place while tuple and punctuation processing
 * functions are active. The <code>
 * Operator</code>'s implementation of its <code>shutdown</code> method should complete any
 * asynchronous activity and release any resources.
 *
 * <p>Tuples are submitted to output ports using submit methods on <code>StreamingOutput</code>,
 * such as {@link StreamingOutput#submit(Tuple)}. Output ports are punctuated using {@link
 * StreamingOutput#punctuate(StreamingData.Punctuation)}.
 *
 * <p>An operator performs asynchronous actions using tasks with a {@code cheduledExecutorService}
 * obtained from {@link OperatorContext#getScheduledExecutorService()} or threads created from the
 * {@code ThreadFactory} obtained from {@link OperatorContext#getThreadFactory() } . Source
 * operators typically initiate asynchronous actions in their implementation of {@link
 * Operator#allPortsReady()} to produce data and submit it as tuples on their output ports.
 *
 * <p>Operators with a windowed input port specify a {@link
 * com.ibm.streams.operator.window.StreamWindowListener StreamWindowListener} using {@link
 * com.ibm.streams.operator.window.StreamWindow#registerListener(com.ibm.streams.operator.window.StreamWindowListener,
 * boolean) StreamWindow.registerListener} to handle window events. If the window is partitioned the
 * operator specifies a {@link com.ibm.streams.operator.window.StreamWindowPartitioner
 * StreamWindowPartitioner} using {@link
 * com.ibm.streams.operator.window.StreamWindow#registerPartitioner(com.ibm.streams.operator.window.StreamWindowPartitioner)
 * StreamWindow.registerPartitioner}.
 *
 * <p>A number of abstract and concrete <A HREF="../../../../../samples/index.html"
 * target="_blank">sample operators</A> are provided by Streams.
 */
public interface Operator {

  /**
   * Initialize this operator. Called once before any tuples are processed.
   *
   * @param context OperatorContext for this operator.
   * @throws Exception Operator failure, will cause the enclosing PE to terminate.
   */
  public void initialize(OperatorContext context) throws Exception;

  /**
   * Called once initialization is complete and all input and output ports are connected and ready
   * to process and submit tuples. Operators that submit tuples independently of incoming tuples,
   * e.g. source type operators, must wait for this method to be called before submitting any
   * tuples. <br>
   * Since this is a notification that all input and output ports are ready, it is possible that
   * {@code process} and {@code processPunctuation} may be called before {@code allPortsReady}, due
   * to thread scheduling.
   *
   * <p>Implementations must not block and must return control to the caller.
   *
   * <p>This operator's {@link OperatorContextMXBean} will also issue a {@link
   * OperatorContextMXBean#ALL_PORTS_READY notification} when all ports are ready. <br>
   * Code may register actions to be invoked at all ports ready using the utility method {@link
   * OperatorLifeCycle#addAllPortsReadyAction(OperatorContext,
   * javax.management.NotificationListener) addAllPortsReadyAction} or by adding notification
   * listeners to this operator's {@link OperatorContextMXBean}.
   *
   * @throws Exception Operator failure, will cause the enclosing PE to terminate.
   * @see OperatorContextMXBean
   * @see OperatorLifeCycle#addAllPortsReadyAction(OperatorContext,
   *     javax.management.NotificationListener)
   */
  public void allPortsReady() throws Exception;

  /**
   * Process an incoming tuple that arrived on the specified port.
   *
   * @param port Port the tuple is arriving on.
   * @param tuple Object representing the incoming tuple.
   * @throws Exception Operator failure, will cause the enclosing PE to terminate.
   */
  public void process(StreamingInput<Tuple> port, Tuple tuple) throws Exception;

  /**
   * Process an incoming punctuation mark on the specified port.
   *
   * @param port Port punctuation mark arrived on
   * @param mark value of mark
   * @throws Exception Operator failure, will cause the enclosing PE to terminate.
   */
  public void processPunctuation(StreamingInput<Tuple> port, Punctuation mark) throws Exception;

  /**
   * Instruct this operator to shutdown. Called by the SPL runtime when the processing element (PE)
   * hosting the operator is being shutdown. Implementations should complete or terminate any
   * asynchronous activity and release any resources, such as closing open files.
   *
   * <p>Prior to this call, the SPL runtime initiates an orderly shutdown on the <code>Operator
   * </code>'s <code>
   * ThreadFactory</code> and <code>ScheduledExecutorService</code> so that no new threads or tasks
   * can be created. <br>
   * Once this method returns the SPL Runtime completes the shutdown of the <code>Operator</code>'s
   * <code>ThreadFactory
   * </code> (calling <code>Thread.interrupt</code> on any of its active threads) and <code>
   * ScheduledExecutorService
   * </code> (calling <code>ExecutorService.shutdownNow()</code>).
   *
   * <p>This operator's {@link OperatorContextMXBean} will also issue a {@link
   * OperatorContextMXBean#SHUTDOWN shutdown notification} and a {@link
   * OperatorContextMXBean#SHUTDOWN_COMPLETE shutdown complete notification}. <br>
   * Code may register actions to be invoked at {@link OperatorContextMXBean#SHUTDOWN shutdown}
   * using the utility method {@link OperatorLifeCycle#addShutdownAction(OperatorContext,
   * javax.management.NotificationListener) addShutdownAction} or by adding notification listeners
   * to this operator's {@link OperatorContextMXBean}.
   *
   * @throws Exception Error while shutting down.
   * @see OperatorContext#getThreadFactory()
   * @see OperatorContext#getScheduledExecutorService()
   * @see OperatorContextMXBean
   * @see OperatorLifeCycle#addShutdownAction(OperatorContext,
   *     javax.management.NotificationListener)
   * @see OperatorLifeCycle#closeOnShutdown(OperatorContext, java.io.Closeable)
   */
  public void shutdown() throws Exception;

  /**
   * Tag names for use with {@link Operator#setTagData(String, Map)}.
   *
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public static interface TagNames {
    public static final String OperatorIGC = "OperatorIGC";
  }

  /**
   * Set tag data for the operator.
   *
   * <p>This default implementation throws {@code UnsupportedOperationException}.
   *
   * @deprecated Operators are not required to implement this method as setting tags is a Streams
   *     runtime function. Operators may set tag data using {@link
   *     ProcessingElement#setTagData(String, Map)} or {@link AbstractOperator#setTagData(String,
   *     Map)} if they are a sub-class of {@link AbstractOperator}.
   * @see ProcessingElement#setTagData(String, Map)
   * @see AbstractOperator#setTagData(String, Map)
   * @since InfoSphere&reg; Streams Version 4.1
   */
  @Deprecated
  default void setTagData(String tagName, Map<String, String> tagValue) {
    throw new UnsupportedOperationException("Operator.setTagData()");
  }
}
