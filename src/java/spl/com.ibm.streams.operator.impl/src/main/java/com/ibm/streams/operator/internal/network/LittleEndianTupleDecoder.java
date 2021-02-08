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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Tuple decoder that wraps another decoder but always changes the ByteOrder of the tuple's incoming
 * buffer to LITTLE_ENDIAN. Note that the order of all newly created buffers is BIG_ENDIAN which
 * matches network byte order.
 *
 * @param <T> Tuple type.
 */
public final class LittleEndianTupleDecoder<T> implements TupleDecoder<T> {
  private final TupleDecoder<T> decoder;

  public LittleEndianTupleDecoder(TupleDecoder<T> decoder) {
    this.decoder = decoder;
  }

  /**
   * Decode the buffer into a tuple using the decoder but first change the order of the ByteBuffer
   * to LITTLE_ENDIAN
   */
  @Override
  public T decode(final ByteBuffer rawTuple) {
    return decoder.decode(rawTuple.order(ByteOrder.LITTLE_ENDIAN));
  }

  @Override
  public T newTuple(Object[] values) {
    return decoder.newTuple(values);
  }
}
