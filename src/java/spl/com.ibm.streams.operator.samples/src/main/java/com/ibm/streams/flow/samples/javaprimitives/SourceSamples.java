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

package com.ibm.streams.flow.samples.javaprimitives;

import com.ibm.streams.flow.declare.OperatorGraph;
import com.ibm.streams.flow.declare.OperatorGraphFactory;
import com.ibm.streams.flow.declare.OperatorInvocation;
import com.ibm.streams.flow.declare.OutputPortDeclaration;
import com.ibm.streams.flow.handlers.StreamCounter;
import com.ibm.streams.flow.javaprimitives.JavaOperatorTester;
import com.ibm.streams.flow.javaprimitives.JavaTestableGraph;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.samples.sources.RandomBeacon;
import java.util.Random;

/**
 * Collection of sample tests for source operators using the {@link JavaOperatorTester test
 * framework}. <br>
 * The methods demonstrate creation of a graph and testing of various aspects of that graph, include
 * testing output. A specific test framework (such as JUnit or TestNG) is not used, with instead
 * Java platform assertions used to verify state. Typically it is expected that testing of toolkits
 * would use a testing framework.
 */
public class SourceSamples {

  /**
   * Create and execute a graph containing a single {@link RandomBeacon} operator. Verify it
   * produces the correct number of tuples and punctuation marks.
   *
   * @throws Exception Unexpected exception declaring or executing graph
   * @see RandomBeacon
   */
  public void randomBeacon() throws Exception {

    final int iterations = 100;

    final OperatorGraph graph = OperatorGraphFactory.newGraph();

    // Declare a beacon operator
    OperatorInvocation<RandomBeacon> beacon = graph.addOperator(RandomBeacon.class);
    beacon.setIntParameter("iterations", iterations).setDoubleParameter("period", 0.001);

    // With a single output port
    OutputPortDeclaration beaconOutput = beacon.addOutput("tuple<rstring name>");

    // Graph is now declared, create an executable version
    JavaOperatorTester jot = new JavaOperatorTester();
    JavaTestableGraph tg = jot.executable(graph);

    // Register a handler to count tuples and
    // punctuation marks
    StreamCounter<Tuple> counter = new StreamCounter<Tuple>();
    tg.registerStreamHandler(beaconOutput, counter);

    // Execute the graph. RandomBeacon submits
    // a final marker after it has submitted the required
    // number of tuples (defined by the iterations parameter)
    // which will cause the graph to complete.
    tg.executeToCompletion();

    // Verify the output.
    assert counter.getTupleCount() == iterations;
    assert counter.getMarkCount(Punctuation.WINDOW_MARKER) == 1;
    assert counter.getMarkCount(Punctuation.FINAL_MARKER) == 1;
  }

  /**
   * Create a {@link RandomBeacon} and verify its parameters are set correctly. This demonstrates
   * accessing the reference to instance of the {@link Operator} under test using {@link
   * JavaTestableGraph#getOperatorInstance(OperatorInvocation)}.
   */
  public void randomBeaconParameters() throws Exception {
    final OperatorGraph graph = OperatorGraphFactory.newGraph();

    // Declare a beacon operator
    OperatorInvocation<RandomBeacon> beacon = graph.addOperator(RandomBeacon.class);

    // Add required output port
    beacon.addOutput("tuple<rstring name>");

    Random rand = new Random();
    final long seed = rand.nextLong();
    final long delay = rand.nextInt(100) + 1;
    beacon.setLongParameter("seed", seed);
    beacon.setLongParameter("initDelay", delay);

    // Graph is now declared, create an executable version
    JavaOperatorTester jot = new JavaOperatorTester();
    JavaTestableGraph tg = jot.executable(graph);

    // Initialize the operator to ensure that the setters
    // have been called by the SPL runtime.
    tg.initialize().get();

    // Get the instance of RandomBeacon executing in the graph
    RandomBeacon runningBeacon = tg.getOperatorInstance(beacon);

    // Verify its setter methods were called.
    assert seed == runningBeacon.getSeed();
    assert delay == runningBeacon.getInitialDelay();

    // Cleanup
    tg.shutdown().get();
  }
}
