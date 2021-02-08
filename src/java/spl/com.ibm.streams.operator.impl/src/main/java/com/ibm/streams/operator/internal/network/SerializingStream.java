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
import java.nio.ByteBuffer;

/** RuntimeStream that encodes the tuple and delivers it onto a RuntimeByteStream */
public class SerializingStream<T> implements RuntimeStream<T> {

  private final OutputByteStream handler;
  private final TupleEncoder<T> encoder;

  public SerializingStream(OutputByteStream handler, TupleEncoder<T> encoder) {
    this.handler = handler;
    this.encoder = encoder;
  }

  @Override
  public void tuple(T tuple) throws Exception {
    long bufferSize = encoder.getMaxSerializedSize(tuple);

    // TODO Size error

    final ByteBuffer rawTuple = handler.getByteBuffer((int) bufferSize);
    encoder.encode(tuple, rawTuple);
    rawTuple.flip();

    handler.tuple(rawTuple);
  }

  @Override
  public void mark(Punctuation mark) throws Exception {
    handler.mark(mark);
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    return handler.action(action);
  }
}
