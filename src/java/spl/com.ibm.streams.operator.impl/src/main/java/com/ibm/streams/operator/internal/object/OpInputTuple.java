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

package com.ibm.streams.operator.internal.object;

import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.network.LittleEndianTupleDecoder;
import com.ibm.streams.operator.internal.network.TupleDecoder;
import com.ibm.streams.operator.internal.runtime.Schema;
import java.nio.ByteOrder;

/** Tuple object implementation for the com.ibm.streams.operator package. */
public final class OpInputTuple extends ObjectInputTuple {

  private static final long serialVersionUID = -5623172756970857575L;

  public OpInputTuple(Schema schema, Object[] values) {
    super(schema, values);
  }

  @Override
  public Tuple asReadOnlyTuple() {
    return this;
  }

  public static TupleDecoder<Tuple> getTupleDecoder(final Schema schema) {
    return new ObjectTupleDecoder<Tuple>(schema) {

      @Override
      public Tuple newTuple(final Object[] values) {
        return new OpInputTuple(schema, values);
      }
    };
  }

  /**
   * Get a object based tuple encoder that decodes the serialized form into an <code>
   * com.ibm.streams.operator.Tuple
   * </code>
   *
   * @param schema Schema of port.
   * @return Tuple decoder.
   */
  public static TupleDecoder<Tuple> getTupleDecoder(
      final Schema schema, final ByteOrder byteOrder) {
    TupleDecoder<Tuple> decoder = getTupleDecoder(schema);

    if (byteOrder == ByteOrder.LITTLE_ENDIAN)
      decoder = new LittleEndianTupleDecoder<Tuple>(decoder);

    return decoder;
  }
}
