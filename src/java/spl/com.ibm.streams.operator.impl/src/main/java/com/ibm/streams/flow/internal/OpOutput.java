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
import com.ibm.streams.flow.declare.OutputPortDeclaration;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.model.OutputPortSet;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

/** Declared output port. */
class OpOutput extends OpPort<OutputPortDeclaration> implements OutputPortDeclaration {

  private final Set<OpInput> connections = new HashSet<OpInput>();
  private final OutputPortSet portSet;

  OpOutput(OpInvocation<?> op, int index, String name, StreamSchema schema, OutputPortSet portSet) {
    super(op, index, name, schema);
    this.portSet = portSet;
  }

  @Override
  public OutputPortSet getPortSet() {
    return portSet;
  }

  /**
   * Add a connection and add the connection info to the internal parameter OP_INTRA_PE_CONNS as two
   * values:
   *
   * <p>[intraPeConnection].outputPortIndex = receiving operator index
   * [intraPeConnection].outputPortIndex = input port index in receiving operator
   *
   * @param input
   */
  synchronized void addConnection(OpInput input) {
    if (!connections.add(input)) return;

    addConnection(input, this);

    final String param = OperatorAdapter.OP_INTRA_PE_CONNS + getPortNumber();

    op().getParameters().addParameterValue(param, Integer.toString(input.op().getOperatorIndex()));
    op().getParameters().addParameterValue(param, Integer.toString(input.getPortNumber()));
  }

  synchronized Collection<OpInput> connections() {
    return Collections.unmodifiableCollection(connections);
  }

  @Override
  public OutputPortDeclaration connect(InputPortDeclaration... inputPorts) {
    for (InputPortDeclaration input : inputPorts) input.connect(this);
    return this;
  }

  void verifyModel() {}
}
