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

import com.ibm.streams.flow.declare.InputPortDeclaration;
import com.ibm.streams.flow.declare.OperatorGraph;
import com.ibm.streams.flow.declare.OperatorInvocation;
import com.ibm.streams.flow.declare.OutputPortDeclaration;
import com.ibm.streams.flow.handlers.StreamHandler;
import com.ibm.streams.flow.javaprimitives.JavaTestableGraph;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.object.ObjectOutputTupleFactory;
import com.ibm.streams.operator.internal.ports.HandlerToRuntimeStream;
import com.ibm.streams.operator.internal.ports.OperatorOutputPort;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.management.OperatorManagement;
import java.lang.management.ManagementFactory;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Future;
import javax.management.JMX;
import javax.management.MalformedObjectNameException;

/**
 * Testable graph that exposes the incremental setup of operators through initialize and
 * allPortsReady, as well as the ability to connect to unconnected streams.
 */
class GraphTester extends ExecutableGraphBase<JavaTestableGraph> implements JavaTestableGraph {

  private final Map<InputPortDeclaration, StreamingOutput<OutputTuple>> inputTesters =
      new HashMap<InputPortDeclaration, StreamingOutput<OutputTuple>>();

  GraphTester(OperatorGraph graph) throws Exception {
    super((OpGraph) graph);
  }

  public GraphTester(OperatorInvocation<?> invocation) throws Exception {
    this(((OpInvocation<?>) invocation).opGraph());
  }

  @Override
  public void registerStreamHandler(OutputPortDeclaration iport, StreamHandler<Tuple> handler) {
    OpOutput port = (OpOutput) iport;
    port.checkNotConnected();
    final OpExec op = getOperator(port.op().getOperatorIndex());
    op.addHandlerToOutputPort(port.getPortNumber(), handler);
  }

  /** Gets the tester used to input tuples directly to an input port. */
  @Override
  public synchronized StreamingOutput<OutputTuple> getInputTester(InputPortDeclaration iport) {
    OpInput port = (OpInput) iport;
    port.checkNotConnected();

    if (inputTesters.containsKey(iport)) return inputTesters.get(iport);

    final int portIndex = port.getPortNumber();

    final OpExec op = getOperator(port.op().getOperatorIndex());

    /*
    final StreamingOutput<OutputTuple> tester = new DirectOutputPort(
            portIndex,
            ExportType.NOT_EXPORTED,
            port.getName(),
            port.getStreamSchema(),
            op.getInputHandler(portIndex));
            *
            */
    final StreamingOutput<OutputTuple> tester =
        new OperatorOutputPort<OutputTuple>(
            portIndex,
            port.getName(),
            port.getStreamSchema(),
            new HandlerToRuntimeStream<Tuple>(op.getInputHandler(portIndex)),
            ObjectOutputTupleFactory.getTupleFactory((Schema) port.getStreamSchema()));

    inputTesters.put(iport, tester);
    return tester;
  }

  @Override
  public Future<JavaTestableGraph> initialize() {
    return super.initialize();
  }

  @Override
  public Future<JavaTestableGraph> allPortsReady() {
    return super.allPortsReady();
  }

  @Override
  public Future<JavaTestableGraph> completion() {
    return super.completion();
  }

  @Override
  public JavaTestableGraph waitForCompletion() throws Exception {
    return waitForFuture(completion());
  }

  @Override
  public Future<JavaTestableGraph> shutdown() {
    return super.shutdown();
  }

  @Override
  JavaTestableGraph getGraph() {
    return this;
  }

  @Override
  public OperatorContextMXBean getOperatorContextMBean(
      OperatorInvocation<? extends Operator> operator) {
    return getOperatorContextMBean(operator.getName());
  }

  @Override
  public OperatorContextMXBean getOperatorContextMBean(String name) {
    try {
      return JMX.newMXBeanProxy(
          ManagementFactory.getPlatformMBeanServer(),
          OperatorManagement.getName(OperatorManagement.Type.OPERATORCONTEXT, name),
          OperatorContextMXBean.class,
          true);
    } catch (MalformedObjectNameException e) {
      // can't happen.
      throw new RuntimeException(e);
    }
  }

  @Override
  public <T extends Operator> T getOperatorInstance(final OperatorInvocation<T> operator) {

    OpInvocation<T> oi = (OpInvocation<T>) operator;
    OpExec opExec = getOperator(oi.getOperatorIndex());

    Operator running = opExec.getAdapter().getOperatorInstance();
    return operator.getOperatorClass().cast(running);
  }
}
