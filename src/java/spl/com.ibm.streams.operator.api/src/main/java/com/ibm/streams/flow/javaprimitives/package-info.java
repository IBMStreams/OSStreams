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
 * Testing of an SPL directed flow graph that contains Java primitive operators.
 *
 * <H2>Overview </H2>
 *
 * In a test framework, the {@link com.ibm.streams.flow.javaprimitives.JavaTestableGraph} interface
 * supports the execution of an {@link com.ibm.streams.flow.declare.OperatorGraph arbitrary graph}
 * that contains Java primitive operators. In a Java development environment using standard Java
 * techniques, this interface supports unit testing of Java code for classes that implement {@link
 * com.ibm.streams.operator.Operator}.
 *
 * <p>The execution environment for the graph <EM>simulates</EM> a Streams standalone application,
 * with these differences:
 *
 * <UL>
 *   <LI>All Java primitive operators are defined only by the Java class, not by the operator model.
 *   <LI>The external test framework loads classes, and therefore:
 *       <UL>
 *         <LI>All classes must be provided by the external framework. For example, the class path
 *             for executing JUnit tests in Apache Ant must include all the required classes.
 *         <LI>Classes are be shared across operators. In a Streams compiled SPL application, each
 *             operator has its own class loader that loads the classes defined by its operator
 *             model.
 *       </UL>
 * </UL>
 *
 * <H2>Testing Example </H2>
 *
 * The following code is a simple example that shows how an {@link
 * com.ibm.streams.flow.declare.OperatorGraph} is tested after it has been declared. <br>
 *
 * <PRE>
 * <CODE>
 * OperatorGraph graph = OperatorGraphFactory.newGraph();
 *
 * // Definition of graph omitted.
 * // See overview for package com.ibm.streams.operator.flow.declare for example
 *
 * // Create the testable version of the graph
 * JavaTestableGraph testableGraph = new JavaOperatorTester().executable(graph);
 *
 * // Execute the initialization of operators within graph.
 * testableGraph.initialize().get();
 * </CODE>
 * </PRE>
 *
 * <H2>Single Operator Java Class Testing </H2>
 *
 * {@link com.ibm.streams.flow.javaprimitives.JavaOperatorTester} contains utility methods that
 * simplify testing for a single class that implements {@link com.ibm.streams.operator.Operator}.
 * Test code can create an {@link com.ibm.streams.flow.declare.OperatorInvocation} directly using
 * {@link com.ibm.streams.flow.javaprimitives.JavaOperatorTester#singleOp(Class)} and a {@link
 * com.ibm.streams.flow.javaprimitives.JavaTestableGraph} for it using {@link
 * com.ibm.streams.flow.javaprimitives.JavaOperatorTester#tester(OperatorInvocation)}.
 *
 * <H3>Example </H3>
 *
 * The following sample code represents a single operator unit test in the JUnit style for the
 * sample {@code RandomBeacon} operator. The test is a unit test that verifies that the number of
 * tuples submitted by the operator matches the value set for the corresponding {@code iterations}
 * parameter. <CODE>
 * <PRE>
 * Test
 * public void testIterations() throws Exception {
 *
 * final int expectedCount = 100;
 *
 * JavaOperatorTester jot = new JavaOperatorTester();
 *
 * // Set the invocation context for the single operator
 * // including setting the iterations parameter
 * OperatorInvocation&lt;RandomBeacon&gt; op = jot.singleOp(RandomBeacon.class);
 * OutputPortDeclaration output = op.addOutput("tuple&lt;int32 a&gt;");
 * op.paramInt("iterations", expectedCount);
 *
 * // Create a graph for testing
 * TestableGraph tester = jot.tester(op);
 *
 * // Attach a StreamHandler that counts tuples and punctuation marks to the output port
 * StreamCounter&lt;Tuple&gt; counter = new StreamCounter&lt;Tuple&gt;();
 * tester.registerStreamHandler(output, counter);
 *
 * // Execute the graph
 * tester.executeToCompletion();
 *
 * // Verify the counter see the correct number of tuples and punctuation marks.
 * assertEquals(expectedCount, counter.getTupleCount());
 * assertEquals(1, counter.getMarkCount(Punctuation.WINDOW_MARKER));
 * assertEquals(1, counter.getMarkCount(Punctuation.FINAL_MARKER));
 * }
 * </CODE>
 * </PRE>
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
package com.ibm.streams.flow.javaprimitives;

import com.ibm.streams.flow.declare.OperatorInvocation;
