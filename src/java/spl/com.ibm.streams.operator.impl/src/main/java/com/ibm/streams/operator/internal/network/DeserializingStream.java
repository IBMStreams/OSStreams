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
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.StreamAction;
import java.nio.ByteBuffer;

/**
 * Helper class that converts a serialized tuple from a ByteBuffer to its object form and then uses
 * an StreamHandler to process the tuple.
 */
public final class DeserializingStream implements RuntimeStream<ByteBuffer>, InputPortDecoder {

  /**
   * Array of Punctuation marks by ordinals, cached to avoid creating a new array on each punctuate
   * processing.
   */
  private static final Punctuation[] MARKS = Punctuation.values();

  /** Decoder from serialized tuples to Tuple objects. */
  private final TupleDecoder<Tuple> tupleDecoder;

  /** RuntimeStream that is managing the Operator. */
  private final RuntimeStream<Tuple> handler;

  public DeserializingStream(RuntimeStream<Tuple> handler, TupleDecoder<Tuple> decoder) {
    this.handler = handler;
    this.tupleDecoder = decoder;
  }

  @Override
  public void processRawTuple(ByteBuffer buffer) throws Exception {
    tuple(buffer);
  }

  /**
   * Deserialize a tuple buffer whose first byte is the tuple/punctuation marker (value 0) and hand
   * it off to the adapter for the Operator to process. Data following the marker represents the
   * serialized attributes.
   */
  @Override
  public final void tuple(final ByteBuffer buffer) throws Exception {
    handler.tuple(tupleDecoder.decode(buffer));
  }

  /**
   * Convert from an punctuation marker as an integer and and hand it off to the adapter for the
   * Operator to process. Since punctuation is a enum only valid punctuation marks will make it to
   * the Operator. Others will throw an exception. The tuple marker (Punctuation.NON_EXISTENT) will
   * be ignored.
   */
  public final void processPunctuation(int mark) throws Exception {
    Punctuation pm = MARKS[mark];
    if (pm == Punctuation.NON_EXISTENT) return;

    mark(pm);
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
