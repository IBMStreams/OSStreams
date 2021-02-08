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

import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.network.DeserializingStream;
import com.ibm.streams.operator.internal.network.InputPortDecoder;
import com.ibm.streams.operator.internal.network.TupleDecoder;
import com.ibm.streams.operator.internal.object.OpInputTuple;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.internal.types.Types;
import java.nio.ByteOrder;

/**
 * Data class that allows the incremental addition of information specific to a set of ports (input
 * or output). The required ordering is setNumberOfPorts(), then for each port setPort() followed by
 * setAttribute() for each attribute within a port. This order is not explicitly enforced by the
 * class but is implicitly enforced by various exceptions due to un-initialized variables.
 */
public final class PortsSetup {

  /** Stream name ordered as in the application. */
  private String[] portNames;

  /**
   * Ordinals for the type of the port for the StreamingInput.InputType and
   * StreamingOutput.OutputType.
   */
  private int[] typeOrdinals;

  /** Array of attribute names ordered as in the application source. */
  private String[][] attributeNames;

  /** Array of attribute SPADE types ordered as in the application source. */
  private String[][] attributeTypes;

  /** Set the number of ports, must be called before setting any details of a specific port. */
  public void setNumberOfPorts(int portCount) {
    portNames = new String[portCount];
    typeOrdinals = new int[portCount];
    attributeTypes = new String[portCount][];
    attributeNames = new String[portCount][];
  }

  public void setPort(int port, int typeOrdinal, String name, int attributeCount) {
    portNames[port] = name;
    typeOrdinals[port] = typeOrdinal;
    attributeTypes[port] = new String[attributeCount];
    attributeNames[port] = new String[attributeCount];
  }

  public void setAttribute(int port, int position, String type, String name) {
    attributeTypes[port][position] = type;
    attributeNames[port][position] = name;
  }

  /**
   * Create a set of schema objects for all the streams defined in this object.
   *
   * @return
   */
  public StreamSchema[] getStreamSchemas() {
    return getSchemas();
  }

  /**
   * Return the runtime internal representation of the schemas.
   *
   * @return
   */
  public Schema[] getSchemas() {

    Schema[] schemas = new Schema[portNames.length];

    for (int port = 0; port < schemas.length; port++) {
      schemas[port] = Types.getSchema(attributeTypes[port], attributeNames[port]);
    }

    return schemas;
  }

  public final String[] getPortNames() {
    return portNames;
  }

  /**
   * Set the input port objects in the adapter from my port definitions.
   *
   * @param adapter
   */
  InputPortDecoder[] setInputPorts(final ByteOrder byteOrder, final OperatorAdapter adapter) {
    final Schema[] schemas = getSchemas();

    final InputPortDecoder[] inputs = new InputPortDecoder[schemas.length];

    for (int port = 0; port < schemas.length; port++) {
      Schema schema = schemas[port];

      TupleDecoder<Tuple> decoder = OpInputTuple.getTupleDecoder(schema, byteOrder);

      RuntimeStream<Tuple> handler = adapter.getInputPortHandler(port);

      inputs[port] = new DeserializingStream(handler, decoder);
    }

    return inputs;
  }

  public int[] getPortTypeOrdinals() {
    return typeOrdinals;
  }
}
