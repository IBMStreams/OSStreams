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
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.OperatorSetup;
import com.ibm.streams.operator.internal.runtime.api.Parameters;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.spl.model.ObjectFactory;
import com.ibm.streams.spl.model.OperatorInstanceModelType;
import com.ibm.streams.spl.model.OutputPortType;
import com.ibm.streams.spl.model.ParameterType;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * Declared invocation of an operator in an OperatorGraph.
 *
 * <p>Re-uses the OperatorSetup and related classes from the declaration of an invocation from the
 * real Streams runtime (via JavaOp).
 *
 * @param <T> Class of the operator.
 */
final class OpInvocation<T extends Operator> implements OperatorInvocation<T> {

  private final OpGraph graph;
  private final int operatorIndex;
  private final Class<T> operatorClass;
  private final OperatorSetup setup;
  private final OpModel model;
  private final OperatorInstanceModelType splModel;

  private final List<OpInput> inputPorts = new ArrayList<OpInput>();

  List<OpInput> getInputPorts() {
    return inputPorts;
  }

  private final List<OpOutput> outputPorts = new ArrayList<OpOutput>();

  List<OpOutput> getOutputPorts() {
    return outputPorts;
  }

  OpInvocation(
      OpGraph graph,
      int index,
      String name,
      Class<T> operatorClass,
      Map<String, List<String>> commonParameters) {
    this.graph = graph;
    this.operatorClass = operatorClass;
    this.model = OpModel.getOpModel(operatorClass);
    this.setup = new OperatorSetup(name);
    this.operatorIndex = index;
    this.splModel = graph.newSPLModel();
    getParameters().copy(commonParameters);
    getParameters()
        .addParameterValue(OperatorAdapter.OP_PE_OPERATOR_INDEX, Integer.toString(index));

    getParameters()
        .addParameterValue(
            OperatorAdapter.OP_PE_OPERATOR_KIND, OpModel.primitiveKind(operatorClass));

    splModel.getContext().setKind(OpModel.primitiveKind(operatorClass));
  }

  @Override
  public final String getName() {
    return setup.getName();
  }

  @Override
  public OperatorGraph graph() {
    return graph;
  }

  final OpGraph opGraph() {
    return graph;
  }

  final int getOperatorIndex() {
    return operatorIndex;
  }

  Parameters getParameters() {
    return setup.getParameters();
  }

  /** Get a parameter value assuming it is always set and a single value. */
  String getParameter(String parameterName) {
    return getParameters().getParameterValue(parameterName);
  }

  @Override
  public synchronized OutputPortDeclaration addOutput(String name, StreamSchema schema) {
    OutputPortSet portSet = getModel().verifyAddOutputPort(outputPorts.size());
    final OpOutput port = new OpOutput(this, outputPorts.size(), name, schema, portSet);
    outputPorts.add(port);
    OutputPortType portModel = graph.getSPLModelFactory().createOutputPortType();
    portModel.setIndex(BigInteger.valueOf(port.getPortNumber()));
    portModel.setTupleSPLType(schema.getLanguageType());
    splModel.getOutputPorts().getOutputPort().add(portModel);
    return port;
  }

  @Override
  public OutputPortDeclaration addOutput(String name, String schema) {
    return addOutput(name, Type.Factory.getTupleType(schema).getTupleSchema());
  }

  public OperatorSetup getSetup() {
    return setup;
  }

  @Override
  public Class<T> getOperatorClass() {
    return operatorClass;
  }

  @Override
  public synchronized InputPortDeclaration addInput(OutputPortDeclaration outputPort) {
    return addInputPort(outputPort.getName(), outputPort.getStreamSchema()).connect(outputPort);
  }

  private InputPortDeclaration addInputPort(String name, StreamSchema schema) {
    InputPortSet portSet = getModel().verifyAddInputPort(getInputPorts().size());
    OpInput ipd = new OpInput(this, getInputPorts().size(), name, schema, portSet);
    getInputPorts().add(ipd);

    return ipd;
  }

  @Override
  public InputPortDeclaration addInput(String name, String schema) {
    return addInput(name, Type.Factory.getTupleType(schema).getTupleSchema());
  }

  @Override
  public InputPortDeclaration addInput(String name, StreamSchema schema) {
    return addInputPort(name, schema);
  }

  /** Unique output port name within the operator. */
  private String outputPortName() {
    return setup.getName() + "_OUT" + outputPorts.size();
  }
  /** Unique input port name within the operator. */
  private String inputPortName() {
    return setup.getName() + "_IN" + inputPorts.size();
  }

  @Override
  public synchronized OutputPortDeclaration addOutput(StreamSchema schema) {
    return addOutput(outputPortName(), schema);
  }

  @Override
  public synchronized OutputPortDeclaration addOutput(String schema) {
    return addOutput(outputPortName(), schema);
  }

  @Override
  public synchronized InputPortDeclaration addInput(String schema) {
    return addInput(inputPortName(), schema);
  }

  @Override
  public synchronized InputPortDeclaration addInput(StreamSchema schema) {
    return addInput(inputPortName(), schema);
  }

  @Override
  public OperatorInvocation<T> setStringParameter(String parameterName, String... values) {
    getParameters().remove(parameterName);
    for (String value : values) getParameters().addParameterValue(parameterName, value);

    // In the SPL model we only have the names of the
    // parameters currently.
    List<ParameterType> params = getSPLModel().getParameters().getParameter();
    boolean seenParam = false;
    for (ParameterType pt : params) {
      if (pt.getName().equals(parameterName)) {
        seenParam = true;
        break;
      }
    }
    if (!seenParam) {
      ParameterType pt = graph.getSPLModelFactory().createParameterType();
      pt.setName(parameterName);
      params.add(pt);
    }
    return this;
  }

  @Override
  public OperatorInvocation<T> setIntParameter(String parameterName, int... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = Integer.toString(values[i]);
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setBooleanParameter(String parameterName, boolean... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = Boolean.toString(values[i]);
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setByteParameter(String parameterName, byte... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = Byte.toString(values[i]);
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setShortParameter(String parameterName, short... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = Short.toString(values[i]);
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setLongParameter(String parameterName, long... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = Long.toString(values[i]);
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setFloatParameter(String parameterName, float... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = Float.toString(values[i]);
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setDoubleParameter(String parameterName, double... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = Double.toString(values[i]);
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setBigDecimalParameter(String parameterName, BigDecimal... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = values[i].toString();
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public OperatorInvocation<T> setAttributeParameter(String parameterName, String... values) {
    return setStringParameter(parameterName, values);
  }

  @Override
  public OperatorInvocation<T> setCustomLiteralParameter(String parameterName, Enum<?>... values) {
    String[] svalues = new String[values.length];
    for (int i = 0; i < values.length; i++) svalues[i] = values[i].name();
    return setStringParameter(parameterName, svalues);
  }

  @Override
  public boolean compileChecks(boolean verbose) throws Exception {
    getModel().verifyCompile(this);
    for (OpInput port : getInputPorts()) port.verifyModel();
    for (OpOutput port : getOutputPorts()) port.verifyModel();

    OpCompile opc = new OpCompile(setup());
    return opc.compileChecks(verbose);
  }

  @Override
  public boolean compileChecks() throws Exception {
    return compileChecks(false);
  }

  OpModel getModel() {
    return model;
  }

  /** Setup this operator invocation prior to compile time checks or execution. */
  OpInvocation<T> setup() {
    // Set up the window parameters correction.
    for (OpInput input : getInputPorts()) {
      input.setWindowParameter(getParameters());
    }
    return this;
  }

  OperatorInstanceModelType getSPLModel() {
    return splModel;
  }

  ObjectFactory getSPLModelFactory() {
    return graph.getSPLModelFactory();
  }
}
