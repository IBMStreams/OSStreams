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

package com.ibm.streams.flow.execute;

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.management.ProcessingElementMXBean;
import java.util.concurrent.Future;

/**
 * {@code ExecutableGraph} represents an executable flow graph of Java primitive operators.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface ExecutableGraph<E extends ExecutableGraph<E>> {

  /**
   * Execute the complete graph. The following tasks are performed for each operator in the graph:
   *
   * <OL>
   *   <LI>Calls the {@link ContextCheck context check methods}
   *   <LI>Calls the {@link Operator#initialize(OperatorContext)} method
   *   <LI>Calls the {@link Operator#allPortsReady()} method, which notifies the operator that the
   *       ports are ready and the operator can submit tuples.
   * </OL>
   *
   * <br>
   * The order for the method calls is not defined across the operators. However, the {@link
   * Operator#allPortsReady()} method is called for an operator only after {@link
   * Operator#initialize(OperatorContext)} method is called for all the operators. <br>
   *
   * <p>The graph runs until all the work has been completed by all the operators. A graph is
   * completed when all the operators in the graph have been completed by terminating all the
   * background threads and tasks. A graph is typically completed by source operators that submit
   * {@link Punctuation#FINAL_MARKER final punctuation marks} on all the output ports after those
   * ports submit all of the tuples. When all work is completed, the operators are {@link
   * Operator#shutdown() shutdown} and the returned {@code Future} {@code get()} method returns
   * {@code this}. <br>
   * Some graphs might never complete and therefore run forever. Typically, this occurs for graphs
   * with source operators that read live data sources that never end. <br>
   * A call to the returned {@code Future} {@code cancel(true)} method initiates an asynchronous
   * shutdown of the graph, thereby calling the {@link Operator#shutdown()} method for all the
   * operator invocations in the graph. The order for shutdowns is not defined across the operators.
   *
   * @return this
   */
  public Future<E> execute();

  /**
   * Execute this graph and wait for the completion of the graph. Once the graph completes it will
   * be shutdown prior to this method returning.
   *
   * @return this
   * @throws Exception Exception executing the graph.
   */
  public E executeToCompletion() throws Exception;

  /**
   * Set the trace level for the executing graph. This will cause the {@link
   * ProcessingElementMXBean#TRACE_LEVEL trace level change} notification to be issued.
   *
   * @param level New trace level.
   * @return this
   */
  public E setTraceLevel(TraceLevel level);
}
