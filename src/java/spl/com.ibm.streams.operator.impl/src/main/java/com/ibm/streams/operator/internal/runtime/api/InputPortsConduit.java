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

import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.eventtime.Watermark;
import com.ibm.streams.operator.internal.network.DeserializingStream;
import com.ibm.streams.operator.internal.network.InputPortDecoder;
import com.ibm.streams.operator.internal.network.TupleDecoder;
import com.ibm.streams.operator.internal.object.OpInputTuple;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.ports.actions.WatermarkAction;
import com.ibm.streams.operator.internal.runtime.Schema;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.List;

/**
 * Conduit between a mechanism that manages ports by port number and receives tuples on them in
 * serialized form using a ByteBuffer. This class provides the mapping between the port number and
 * the port objects for the corresponding OperatorAdapater.
 */
public final class InputPortsConduit {

  private final InputPortDecoder[] inputs;

  public InputPortsConduit(ByteOrder byteOrder, OperatorAdapter adapter) {
    inputs = getInputPortDecoders(byteOrder, adapter);
  }

  /**
   * Set the input port objects in the adapter from my port definitions.
   *
   * @param adapter
   */
  private InputPortDecoder[] getInputPortDecoders(
      final ByteOrder byteOrder, final OperatorAdapter adapter) {
    List<StreamingInput<Tuple>> ports = adapter.getContext().getStreamingInputs();

    final InputPortDecoder[] inputs = new InputPortDecoder[ports.size()];

    for (StreamingInput<Tuple> stream : ports) {
      int port = stream.getPortNumber();
      Schema schema = (Schema) stream.getStreamSchema();

      TupleDecoder<Tuple> decoder = OpInputTuple.getTupleDecoder(schema, byteOrder);

      RuntimeStream<Tuple> handler = adapter.getInputPortHandler(port);

      inputs[port] = new DeserializingStream(handler, decoder);
    }

    return inputs;
  }

  /**
   * Process a serialized tuple. The format of the buffer is
   *
   * <OL>
   *   <LI>N attributes in their serialized form
   * </OL>
   *
   * <p>This format matches the tuple format expected by OutputPortsConduit.
   *
   * @param port Port Number
   * @param rawTuple Serialized tuple positioned at first byte of tuple data.
   * @throws Exception Error processing tuple
   */
  public void processRawTuple(final int port, final ByteBuffer rawTuple) throws Exception {
    inputs[port].processRawTuple(rawTuple);
  }

  /**
   * Process an incoming punctuation.
   *
   * @param port Port Number
   * @param mark The punctuation
   * @throws Exception Error processing the punctuation
   */
  public void processPunctuation(int port, int mark) throws Exception {
    inputs[port].processPunctuation(mark);
  }

  /**
   * Process an incoming watermark.
   *
   * @param port Port Number
   * @param mark The watermark
   * @throws Exception Error processing the watermark
   * @since IBM Streams Version 4.3
   */
  public void processWatermark(int port, Watermark wm) throws Exception {
    action(port, new WatermarkAction(wm));
  }

  public void action(int port, StreamAction<?> action) throws Exception {
    inputs[port].action(action);
  }
}
