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

/**
 * The execution of an SPL directed flow graph that contains Java primitive operators.
 *
 * <H2>Overview </H2>
 *
 * {@link com.ibm.streams.flow.execute.ExecutableGraph} supports execution of an {@link
 * com.ibm.streams.flow.declare.OperatorGraph arbitrary graph} that contains Java primitive
 * operators.
 *
 * <p>The execution environment for the graph simulates a Streams standalone application.
 *
 * <H2>Execution Example</H2>
 *
 * The following code shows a simple example of how an {@link
 * com.ibm.streams.flow.declare.OperatorGraph} is executed after it has been declared using Java
 * primitive testing. <br>
 *
 * <PRE>
 * <CODE>
 * OperatorGraph graph = OperatorGraphFactory.newGraph();
 *
 * // Definition of graph omitted.
 * // See overview for package com.ibm.streams.operator.flow.declare for example
 *
 * // Creation of factory to create ExecutableGraph instances
 * JavaOperatorTester jot = new JavaOperatorTester();
 *
 * // JavaTestableGraph extends ExecutableGraph
 * JavaTestableGraph executableGraph = jot.executable(graph);
 *
 * // Execute the graph and wait for it to complete
 * executableGraph.executeToCompletion();
 * </CODE>
 * </PRE>
 *
 * <H2>Punctuation Final Mark Handling</H2>
 *
 * For source operators that have no input ports, a {@link
 * com.ibm.streams.operator.StreamingData.Punctuation#FINAL_MARKER final mark} is submitted to each
 * output port once the operator completes. An operator completes once its {@link
 * com.ibm.streams.operator.OperatorContext#getThreadFactory() threads} have completed and all
 * {@link com.ibm.streams.operator.OperatorContext#getScheduledExecutorService() scheduled activity}
 * has been completed.
 *
 * <p>For operators with input ports and output ports a {@link
 * com.ibm.streams.operator.StreamingData.Punctuation#FINAL_MARKER final mark} is submitted to each
 * output port once all of its input ports have processed a final mark.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
package com.ibm.streams.flow.execute;
