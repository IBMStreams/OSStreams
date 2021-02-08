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

package com.ibm.streams.operator.internal.runtime.api;

/**
 * Generic object that allows incremental setup of an Operator by just maintaining state related to
 * ports and parameters.
 *
 * <p>General flow is to create a OperatorSetup object, and pass in the information related to ports
 * and parameters and then use it to create a new OperatorAdapter using
 * OperatorAdapter.newAdapter().
 *
 * <p>Class is not thread-safe, expected to be used by a single thread during initialization of the
 * operator's environment from the C++ primitive operator JavaOp code.
 */
public final class OperatorSetup {

  /** Intermediate state objects for describing the set of ports. */
  private final PortsSetup inputSet;

  private final PortsSetup outputSet;

  /** Parameters set in the source. */
  private final Parameters parameters = new Parameters();

  /** Name of the operator */
  private final String name;

  private OperatorSetup(String name, PortsSetup inputSet, PortsSetup outputSet) {
    this.name = name;
    this.inputSet = inputSet;
    this.outputSet = outputSet;
  }

  public OperatorSetup(String name) {
    this(name, new PortsSetup(), new PortsSetup());
  }

  public PortsSetup getInputSet() {
    return inputSet;
  }

  public PortsSetup getOutputSet() {
    return outputSet;
  }

  public Parameters getParameters() {
    return parameters;
  }

  public void setNumberOfInputPorts(int inputStreamCount) {
    inputSet.setNumberOfPorts(inputStreamCount);
  }

  public void setInputPort(int port, int typeOrdinal, String name, int attributeCount) {
    inputSet.setPort(port, typeOrdinal, name, attributeCount);
  }

  public void setInputPortAttribute(int port, int position, String type, String name) {
    inputSet.setAttribute(port, position, type, name);
  }

  public void setNumberOfOutputPorts(int outputStreamCount) {
    outputSet.setNumberOfPorts(outputStreamCount);
  }

  public void setOutputPort(int port, int typeOrdinal, String name, int attributeCount) {
    outputSet.setPort(port, typeOrdinal, name, attributeCount);
  }

  public void setOutputPortAttribute(int port, int position, String type, String name) {
    outputSet.setAttribute(port, position, type, name);
  }

  public void setParameter(String parameterName, String value) {
    parameters.addParameterValue(parameterName, value);
  }

  /** @return the name */
  public String getName() {
    return name;
  }

  /**
   * Copy this OperatorSetup to allow multiple execution from the same setup. Cannot reuse setups as
   * the runtime code removes all system parameters.
   */
  public OperatorSetup copy() {
    final OperatorSetup copy = new OperatorSetup(name, inputSet, outputSet);
    copy.getParameters().copy(getParameters().getReadOnlyParams());
    return copy;
  }
}
