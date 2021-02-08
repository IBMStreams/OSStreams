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

import com.ibm.streams.flow.declare.OperatorGraph;
import com.ibm.streams.flow.declare.OperatorGraphFactory;
import com.ibm.streams.flow.declare.OperatorInvocation;
import com.ibm.streams.flow.execute.ExecutableGraphFactory;
import com.ibm.streams.flow.spi.internal.FlowProvider;
import com.ibm.streams.operator.Operator;

/**
 * Utility methods for testing {@link OperatorGraph} instances that contain implementations of
 * {@link Operator}.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public class JavaOperatorTester implements ExecutableGraphFactory<JavaTestableGraph> {

  /**
   * Create a {@link JavaTestableGraph} for a graph of Java primitive operators.
   *
   * @param operatorGraph The declared operator graph.
   * @return {@link JavaTestableGraph testable graph} for {@code operatorGraph}.
   */
  @Override
  public JavaTestableGraph executable(OperatorGraph operatorGraph) throws Exception {
    return FlowProvider.provider.newJavaTestableGraph(operatorGraph);
    // return new GraphTester(operatorGraph);
  }

  /**
   * Create an {@link OperatorInvocation} for a single operator. Allows unit testing of a class that
   * represents a Java primitive.
   *
   * @param <T> Type of the operator class.
   * @param opClass Operator class to be tested.
   * @return OperatorInvocation for a Java primitive that is represented by {@code opClass}
   */
  public <T extends Operator> OperatorInvocation<T> singleOp(Class<T> opClass) {
    return OperatorGraphFactory.newGraph().addOperator(opClass);
  }

  public <T extends Operator> OperatorInvocation<T> singleOp(String name, Class<T> opClass) {
    return OperatorGraphFactory.newGraph().addOperator(name, opClass);
  }

  /**
   * Create a {@link JavaTestableGraph} that contains the operator invocation that is declared by
   * {@code invocation}.
   *
   * @param invocation Invocation of operator
   * @return {@link JavaTestableGraph testable graph} for {@code invocation}.
   * @throws Exception
   * @see #singleOp(Class)
   */
  public JavaTestableGraph tester(OperatorInvocation<? extends Operator> invocation)
      throws Exception {
    return executable(invocation.graph());
  }
}
