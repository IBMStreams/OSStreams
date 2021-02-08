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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.StreamingOutput.ExportType;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.network.SerializingStream;
import com.ibm.streams.operator.internal.network.TupleEncoder;
import com.ibm.streams.operator.internal.network.TupleToConduit;
import com.ibm.streams.operator.internal.object.ObjectOutputTupleFactory;
import com.ibm.streams.operator.internal.object.ObjectTupleEncoder;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.internal.runtime.api.ExecutionContext;
import com.ibm.streams.operator.internal.runtime.api.OutputPortsTransport;

/**
 * StreamingOutput that delivers tuples to a StreamHandler. Can be used in multiple situations. 1)
 * Fused Java to Java connection. Then the first handler needs to be a DirectTupleOutput to perform
 * finalization and metrics. 2) Input port tester. Then the handler is a DirectTupleInput for the
 * input port.
 */
public class PortFactory {

  /**
   * Output port that is completely fused between Java operators and metrics are handled by Java.
   * This returns a standard OperatorOutputPort with a RuntimeStream wrapping handler that ties the
   * metrics to the DirectMetricsFactory.
   */
  public static StreamingOutput<OutputTuple> newDirectJavaPort(
      StreamingData description,
      final RuntimeStream<Tuple> handler,
      final DirectMetricsFactory operatorMetricsFactory) {

    // Handle the metrics and finalization for the port.
    DirectTupleOutput dto =
        DirectTupleOutput.newAllJavaOutput(
            description.getPortNumber(), handler, operatorMetricsFactory);

    return new OperatorOutputPort<OutputTuple>(
        description.getPortNumber(),
        description.getName(),
        description.getStreamSchema(),
        dto,
        ObjectOutputTupleFactory.getTupleFactory((Schema) description.getStreamSchema()));
  }

  /** Get an output port that serializes tuples and sends then to a OutputPortsConduit transport. */
  public static StreamingOutput<OutputTuple> getConduitOutputPort(
      final ExecutionContext ec,
      boolean inConsistentRegion,
      int port,
      ExportType exportType,
      String name,
      Schema schema,
      int pePortNumber,
      OutputPortsTransport transport) {

    // How tuples are serialized.
    final TupleEncoder<Tuple> encoder = new ObjectTupleEncoder(schema);

    // final TupleToConduit<Tuple> handler = new TupleToConduit<Tuple>(
    //        port, encoder, transport);

    RuntimeStream<Tuple> handler =
        new TupleToConduit<Tuple>(
            port, new SerializingStream<Tuple>(transport.getOutputStream(port), encoder));

    if (inConsistentRegion) {
      handler = addConsistentRegionOutputHandler(ec, handler);
    }

    return new OperatorOutputPort<OutputTuple>(
        port,
        exportType,
        name,
        schema,
        pePortNumber,
        handler,
        ObjectOutputTupleFactory.getTupleFactory(schema));

    // return new OutputPortEncoder(
    //        port, typeOrdinal, name, schema, pePortNumber, handler, transport);
  }

  /**
   * Tracks exceptions on a submit for an output stream to ensure that a makeConsistent cannot
   * succeed in the period when an exception is thrown, the permit is released and another thread
   * makes the region consistent, before this exception is seen.
   */
  private static <T> RuntimeStream<T> addConsistentRegionOutputHandler(
      final ExecutionContext ec, RuntimeStream<T> handler) {
    return new RuntimeStreamExceptionHandler<T>(
        new Runnable() {

          @Override
          public void run() {
            ec.mustResetConsistentRegion();
          }
        },
        handler);
  }

  /**
   * Get an output port that switches between serializing tuples to a OutputPortsConduit transport
   * and another handler.
   */
  public static StreamingOutput<OutputTuple> getSwitchableConduitOutputPort(
      final ExecutionContext ec,
      boolean inConsistentRegion,
      int port,
      ExportType exportType,
      String name,
      Schema schema,
      int pePortNumber,
      final RuntimeStreamDynamicForwarder<Tuple> dynamicForwarder,
      OutputPortsTransport transport) {

    // How tuples are serialized.
    final TupleEncoder<Tuple> encoder = new ObjectTupleEncoder(schema);

    // final TupleToConduit<Tuple> handler = new TupleToConduit<Tuple>(
    //        port, encoder, transport);

    final RuntimeStream<Tuple> handler =
        new TupleToConduit<Tuple>(
            port, new SerializingStream<Tuple>(transport.getOutputStream(port), encoder));

    dynamicForwarder.setDestination(handler);

    RuntimeStream<Tuple> finalHandler = dynamicForwarder;
    if (inConsistentRegion) {
      finalHandler = addConsistentRegionOutputHandler(ec, finalHandler);
    }

    return new OperatorOutputPort<OutputTuple>(
        port,
        exportType,
        name,
        schema,
        pePortNumber,
        finalHandler,
        ObjectOutputTupleFactory.getTupleFactory(schema));
  }
}
