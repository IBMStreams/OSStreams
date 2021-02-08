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

package com.ibm.streams.operator.internal.network;

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.ports.actions.WatermarkAction;
import com.ibm.streams.operator.internal.runtime.api.Adapter;
import com.ibm.streams.operator.internal.runtime.api.OutputPortsTransport;
import com.ibm.streams.operator.internal.runtime.api.SPLRuntime;

public class TupleToConduit<T> implements RuntimeStream<T> {

  private final SerializingStream<T> handler;
  private final int portIndex;

  public TupleToConduit(
      final int port, TupleEncoder<T> encoder, final OutputPortsTransport transport) {
    portIndex = port;
    handler = new SerializingStream<T>(transport.getOutputStream(port), encoder);
  }

  public TupleToConduit(final int port, SerializingStream<T> handler) {
    this.portIndex = port;
    this.handler = handler;
  }

  /*

  // @Override
  public void tupleXX(T tuple) throws Exception {
      long bufferSize = encoder.getMaxSerializedSize(tuple);

      // TODO Size error

      final ByteBuffer rawTuple = transport.getByteBuffer(port,
              (int) bufferSize);
      encoder.encode(tuple, rawTuple);
      rawTuple.flip();

      // Ensure downstream code is not associated with the
      // operator submitting the tuple.
      final Adapter oldAdapter = SPLRuntime.setActive(null);
      try {
          transport.submitTuple(port, rawTuple);
      } finally {
          SPLRuntime.setActive(oldAdapter);
      }
  }
  */

  public void tuple(T tuple) throws Exception {
    // Ensure downstream code is not associated with the
    // operator submitting the tuple.
    final Adapter oldAdapter = SPLRuntime.setActive(null);
    try {
      handler.tuple(tuple);
    } finally {
      SPLRuntime.setActive(oldAdapter);
    }
  }

  @Override
  public void mark(Punctuation mark) throws Exception {
    // Ensure downstream code is not associated with the
    // operator submitting the punctuation.
    final Adapter oldAdapter = SPLRuntime.setActive(null);
    try {
      handler.mark(mark);
      // transport.punctuate(port, mark.ordinal());
    } finally {
      SPLRuntime.setActive(oldAdapter);
    }
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    if (action.getType() == StreamAction.Type.WATERMARK) {
      SPLRuntime.getActive()
          .getExecutionContext()
          .submitWatermark(((WatermarkAction) action).getWatermark().getTimestamp(), portIndex);
      return null;
    }
    return handler.action(action);
  }
}
