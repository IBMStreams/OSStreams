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
 * Declaration of an SPL directed flow graph of Java primitive operators.
 *
 * <H2>Overview </H2>
 *
 * {@link com.ibm.streams.flow.declare.OperatorGraph} allows for the declaration of an arbitrary
 * graph of Java primitive operators that follows SPL semantics. A Java primitive operator is
 * represented by a reference to a class that implements {@link com.ibm.streams.operator.Operator}.
 *
 * <H2>Testing Java Operators </H2>
 *
 * The creation of the graph and the subsequent execution of it, supports the testing of SPL Java
 * primitive operator code in a Java environment, by using testing frameworks such as JUnit.
 *
 * <p>A graph that can be tested is represented as a {@link
 * com.ibm.streams.flow.javaprimitives.JavaTestableGraph} and can contain disconnected input and
 * output ports. <br>
 * If ports of the graph are all connected then the test program will typically verifies the
 * behavior by testing the affect of the graph on an external system, for example testing that a
 * database sink operator produced the correct output in a database. <br>
 * By using the {@link
 * com.ibm.streams.flow.javaprimitives.JavaTestableGraph#getInputTester(InputPortDeclaration)}
 * method a test program can directly submit tuples to an operator. The returned {@link
 * com.ibm.streams.operator.StreamingOutput} reference represents a test output port that is
 * connected directly to the input port. <br>
 * A test progam can use the disconnected output ports to verify operator or graph output by using a
 * {@link com.ibm.streams.flow.handlers.StreamHandler} registered with {@link
 * com.ibm.streams.flow.javaprimitives.JavaTestableGraph#registerStreamHandler(OutputPortDeclaration,
 * StreamHandler)}.
 *
 * <H2>Example of a Declared Graph</H2>
 *
 * Here's a simple example of a graph that contains two operators, a {@code
 * com.ibm.streams.operator.samples.sources.RandomBeacon} submitting tuples on a single output port
 * to a filter operator that uses the {@code com.ibm.streams.operator.samples.operators.Regex}
 * class. <br>
 *
 * <PRE>
 * <CODE>
 * OperatorGraph graph = OperatorGraphFactory.newGraph();
 *
 * // Declare a beacon operator
 * OperatorInvocation<RandomBeacon> beacon = graph.addOperator(RandomBeacon.class);
 * beacon.setIntParameter("iterations", 100);
 * OutputPortDeclaration beaconOutput = beacon.addOutput("tuple<rstring name>");
 *
 * // Declare the filter operator
 * OperatorInvocation<Regex> filter = graph.addOperator(Regex.class);
 * filter.setStringParameter("patterns",  "^[ABC].*", "^D.*");
 *
 * // Declare an input port connected to to the beacon's output
 * InputPortDeclaration filterInput = filter.addInput(beaconOutput);
 *
 * // Declare the output port for the filter using the same
 * // schema as its input
 * OutputPortDeclaration filterOutput = filter.addOutput(filterInput.getStreamSchema());
 * </CODE>
 * </PRE>
 *
 * <H3>Example using Fluent Interface Technique</H3>
 *
 * Methods in this package usually return a reference that can be used to declare additional
 * invocation state. This Fluent Interface style then allows compact representation of a graph in
 * Java code. <br>
 * <B>Example of connecting two operators</B> <br>
 *
 * <PRE>
 * <CODE>
 * OperatorGraph graph = OperatorGraphFactory.newGraph();
 *
 * // Declare a beacon operator
 * OutputPortDeclaration beaconOutput =
 * graph.addOperator(RandomBeacon.class)
 * .setIntParameter("iterations", 100)
 * .addOutput("tuple<rstring name>");
 *
 * // Declare the filter operator connected to to the beacon's output
 * // and an output port of the same type.
 * OutputPortDeclaration filterOutput =
 * graph.addOperator(Regex.class)
 * .setStringParameter("patterns",  "^[ABC].*", "^D.*")
 * .addInput(beaconOutput)
 * .operator()
 * .addOutput(beaconOutput.getStreamSchema());
 * </CODE>
 * </PRE>
 *
 * <B>Example of declared a windowed input port</B> <br>
 *
 * <PRE>
 * <CODE>
 * OperatorGraph graph = OperatorGraphFactory.newGraph();
 *
 * OperatorInvocation<TumblingSortOperator> op = graph.addOperator(TumblingSortOperator.class);
 *
 * // Here a window declaration is similar to an SPL window declaration
 * op.addInput("tuple<ustring value>").tumbling().evictCount(200);
 * </CODE>
 * </PRE>
 *
 * <H2>Limitations </H2>
 *
 * <UL>
 *   <LI>Only Java primitive operators defined by Java classes that implement {@link
 *       com.ibm.streams.operator.Operator} can be included in the graph.
 *   <LI>SPL dynamic connections are not supported.
 *   <LI>The SPL logic clause is not supported.
 *   <LI>The SPL configuration clause is not supported.
 *   <LI>SPL annotations are not supported, therefore:
 *       <UL>
 *         <LI>Parallel regions are not supported.
 *         <LI>{@link com.ibm.streams.operator.state.ConsistentRegionContext Consistent regions} are
 *             not supported.
 *       </UL>
 *   <LI>{@link com.ibm.streams.operator.state.CheckpointContext Checkpointing} is not supported.
 * </UL>
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
package com.ibm.streams.flow.declare;

import com.ibm.streams.flow.handlers.StreamHandler;
