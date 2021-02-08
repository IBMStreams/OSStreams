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

import com.ibm.streams.flow.handlers.StreamDiscarder;
import com.ibm.streams.flow.handlers.StreamHandler;
import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.metrics.OperatorMetricsFactory;
import com.ibm.streams.operator.internal.ports.DirectMetricsFactory;
import com.ibm.streams.operator.internal.ports.DirectTupleInput;
import com.ibm.streams.operator.internal.ports.DirectTupleThreadedInput;
import com.ibm.streams.operator.internal.ports.HandlerToRuntimeStream;
import com.ibm.streams.operator.internal.ports.PortFactory;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.RuntimeStreamDynamicForwarder;
import com.ibm.streams.operator.internal.ports.StreamCollection;
import com.ibm.streams.operator.internal.runtime.api.Adapter;
import com.ibm.streams.operator.internal.runtime.api.ExecutionContext;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.OperatorSetup;
import com.ibm.streams.operator.internal.runtime.api.PE;
import com.ibm.streams.operator.state.ConsistentRegionContext.DrainResult;
import com.ibm.streams.operator.types.Timestamp;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

class OpExec implements ExecutionContext {

  /** */
  private final ProcessingElement pe;

  private final ExecutableGraphBase<?> execGraph;
  private final OpInvocation<? extends Operator> opInvoke;
  private final OperatorAdapter adapter;
  private final FinalTracker finalTracker;
  // RTC22706 private final Logging oldLogger;
  private final DirectMetricsFactory operatorMetricsFactory = new DirectMetricsFactory();

  /** handlers that send tuples to the operator including handling metrics and finalization. */
  private final List<StreamHandler<Tuple>> inputHandlers;

  private final List<RuntimeStreamDynamicForwarder<Tuple>> outputForwarders;
  private final List<StreamingOutput<OutputTuple>> outputPorts;

  OperatorAdapter getAdapter() {
    return adapter;
  }

  OpExec(ExecutableGraphBase<?> execGraph, OpInvocation<?> opInvoke, PE<?> pe) throws Exception {
    this.execGraph = execGraph;
    this.opInvoke = opInvoke.setup();
    this.pe = pe;
    addPortsForRuntime();

    outputForwarders = createOutputForwarders();

    registerConnections();

    outputPorts = createOutputPorts(operatorMetricsFactory);

    final FinalTracker ft;
    if (!opInvoke.getInputPorts().isEmpty() && !outputPorts.isEmpty())
      ft = new FinalTracker(opInvoke.getInputPorts().size(), outputPorts);
    else ft = null;
    finalTracker = ft;

    adapter =
        OperatorAdapter.newAdapter(
            opInvoke.getOperatorClass(),
            getSetup().copy(),
            this, // as an ExecutionContext
            null // no checkpoint support
            );

    inputHandlers = createInputHandlers(operatorMetricsFactory);
  }

  /**
   * Create the DirectTupleInput handlers that provide the full port support in Java.
   *
   * @return
   */
  private List<StreamHandler<Tuple>> createInputHandlers(
      final DirectMetricsFactory operatorMetricsFactory) {
    final List<OpInput> inputs = opInvoke.getInputPorts();
    if (inputs.isEmpty()) return Collections.emptyList();

    final List<StreamHandler<Tuple>> handlers = new ArrayList<StreamHandler<Tuple>>(inputs.size());

    for (OpInput input : inputs) {

      StreamHandler<Tuple> handler = null;

      if (input.isThreaded()) {
        handler =
            DirectTupleThreadedInput.newThreadedDirectTupleInput(
                getAdapter(),
                input.getPortNumber(),
                operatorMetricsFactory,
                input.getCongestionPolicy(),
                input.getQueueSize(),
                input.isSingleThreadedOnInput());
      } else {
        handler =
            DirectTupleInput.newDirectTupleInput(
                getAdapter(), input.getPortNumber(), operatorMetricsFactory);
      }

      // Final mark handling
      if (finalTracker != null) handler = finalTracker.finalWatcher(handler);

      // Fan-in handling.
      final int fanInCount = input.getFanInCount();
      if (fanInCount > 1) {
        handler = new FanInHandler<Tuple>(handler, fanInCount);
      }
      handlers.add(handler);
    }

    return Collections.unmodifiableList(handlers);
  }

  StreamHandler<Tuple> getInputHandler(int portIndex) {
    return inputHandlers.get(portIndex);
  }

  RuntimeStreamDynamicForwarder<Tuple> getOutputForwarder(int portIndex) {
    return outputForwarders.get(portIndex);
  }

  /** Add a handler to an output port. */
  synchronized void addHandlerToOutputPort(int portIndex, StreamHandler<Tuple> handler) {

    final RuntimeStreamDynamicForwarder<Tuple> forwarder = getOutputForwarder(portIndex);
    final StreamCollection<Tuple> hc;
    if (forwarder.getDestination() instanceof StreamDiscarder) {
      hc = new StreamCollection<Tuple>(new ArrayList<RuntimeStream<Tuple>>());
      forwarder.setDestination(hc);
    } else {
      hc = (StreamCollection<Tuple>) forwarder.getDestination();
    }
    hc.getHandlers().add(new HandlerToRuntimeStream<Tuple>(handler));
  }

  /** Create a list of dynamic forwarders that will allow the outputs to be redirected later. */
  private List<RuntimeStreamDynamicForwarder<Tuple>> createOutputForwarders() {
    final List<OpOutput> outputs = opInvoke.getOutputPorts();
    if (outputs.isEmpty()) return Collections.emptyList();
    final List<RuntimeStreamDynamicForwarder<Tuple>> forwarders =
        new ArrayList<RuntimeStreamDynamicForwarder<Tuple>>(outputs.size());
    for (int i = 0; i < outputs.size(); i++)
      forwarders.add(new RuntimeStreamDynamicForwarder<Tuple>());

    return Collections.unmodifiableList(forwarders);
  }

  /** Register the connections for my output ports with graphs Connections object. */
  private void registerConnections() {
    final int outputPortCount = opInvoke.getOutputPorts().size();
    if (outputPortCount == 0) return;
    for (int port = 0; port < outputPortCount; port++) {
      String param = OperatorAdapter.OP_INTRA_PE_CONNS + port;
      execGraph
          .getConnections()
          .addConnections(
              opInvoke.getOperatorIndex(),
              port,
              getSetup().getParameters().getParameterValues(param));
    }
  }

  /**
   * Create the output ports to be seen by the user's operator. Initially they are set to discard
   * through the output forwarders.
   */
  private List<StreamingOutput<OutputTuple>> createOutputPorts(
      final DirectMetricsFactory operatorMetricsFactory) {
    final List<OpOutput> outputs = opInvoke.getOutputPorts();
    if (outputs.isEmpty()) return Collections.emptyList();

    final List<StreamingOutput<OutputTuple>> ports =
        new ArrayList<StreamingOutput<OutputTuple>>(outputs.size());
    for (OpOutput output : outputs) {
      ports.add(
          PortFactory.newDirectJavaPort(
              output, getOutputForwarder(output.getPortNumber()), operatorMetricsFactory));
    }

    return Collections.unmodifiableList(ports);
  }

  private OperatorSetup getSetup() {
    return opInvoke.getSetup();
  }

  private void addPortsForRuntime() {

    final OperatorSetup setup = getSetup();

    final List<OpInput> inputPorts = opInvoke.getInputPorts();

    getSetup().setNumberOfInputPorts(inputPorts.size());
    for (OpInput input : inputPorts) {
      final StreamSchema schema = input.getStreamSchema();
      final int port = input.getPortNumber();
      setup.setInputPort(port, 0, input.getName(), schema.getAttributeCount());
      for (Attribute attr : schema) {
        setup.setInputPortAttribute(
            port, attr.getIndex(), attr.getType().getLanguageType(), attr.getName());
      }
    }

    List<OpOutput> outputPorts = opInvoke.getOutputPorts();
    setup.setNumberOfOutputPorts(outputPorts.size());
    for (OpOutput output : outputPorts) {
      final StreamSchema schema = output.getStreamSchema();
      final int port = output.getPortNumber();
      setup.setOutputPort(port, 0, output.getName(), schema.getAttributeCount());
      for (Attribute attr : schema) {
        setup.setOutputPortAttribute(
            port, attr.getIndex(), attr.getType().getLanguageType(), attr.getName());
      }
    }
  }

  void cleanup() {
    getAdapter().cleanup();
  }

  /**
   * Complete the operator and implement operator finalization if it has no input ports.
   *
   * <p>From the toolkit reference:
   *
   * <p>For source operators, which do not have input ports, the final punctuations are sent on
   * output ports, automatically by the runtime, once all operator threads complete their execution.
   */
  void complete() throws Throwable {
    getAdapter().complete();

    // Source operator, no input ports.
    if (inputHandlers.isEmpty()) {
      for (StreamingOutput<OutputTuple> output : outputPorts) {
        output.punctuate(Punctuation.FINAL_MARKER);
      }
    }
  }

  @Override
  public ProcessingElement getPE() {
    return pe;
  }

  @Override
  public OperatorMetricsFactory<?, ?> getMetricsFactory() {
    return operatorMetricsFactory;
  }

  @Override
  public List<StreamingOutput<OutputTuple>> getOutputPorts() {
    return outputPorts;
  }

  @Override
  public void setOperatorTracker(Adapter context) {
    // At the moment nothing to do.
  }

  @Override
  public void resetOperatorTracker() {
    // At the moment nothing to do.
  }

  @Override
  public long getConsistentRegionSequenceId() {
    throw new UnsupportedOperationException();
  }

  @Override
  public int getConsistentRegionResetAttempt() {
    throw new UnsupportedOperationException();
  }

  @Override
  public boolean makeConsistent() {
    throw new UnsupportedOperationException();
  }

  @Override
  public void mustResetConsistentRegion() {
    throw new UnsupportedOperationException();
  }

  @Override
  public String[] getDomainCredentials() {
    throw new UnsupportedOperationException();
  }

  @Override
  public void resetConsistentRegion() throws java.io.IOException {
    throw new UnsupportedOperationException();
  }

  @Override
  public void setMustRethrowException() {
    throw new UnsupportedOperationException();
  }

  @Override
  public boolean mustRethrowException() {
    throw new UnsupportedOperationException();
  }

  @Override
  public void enableNonBlockingCheckpoint() {
    throw new UnsupportedOperationException();
  }

  @Override
  public DrainResult makeConsistentNonBlocking() {
    throw new UnsupportedOperationException();
  }

  @Override
  public void submitWatermark(Timestamp ts) {
    throw new UnsupportedOperationException();
  }

  @Override
  public boolean submitWatermark(Timestamp ts, int port) {
    throw new UnsupportedOperationException();
  }
}
