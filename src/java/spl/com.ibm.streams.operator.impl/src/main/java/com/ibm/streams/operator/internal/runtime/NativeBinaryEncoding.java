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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.encoding.BinaryEncoding;
import com.ibm.streams.operator.internal.network.TupleDecoder;
import com.ibm.streams.operator.internal.network.TupleEncoder;
import com.ibm.streams.operator.internal.object.ObjectTupleEncoder;
import com.ibm.streams.operator.internal.object.OpInputTuple;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Exposure of the binary encoding used by the Streams transport and documented in the SPL compiler
 * reference guide. Simply use the encoders and decoders used by the output and input ports.
 */
final class NativeBinaryEncoding implements BinaryEncoding {

  private final TupleDecoder<Tuple> decoder;
  private final TupleEncoder<Tuple> encoder;

  NativeBinaryEncoding(Schema schema) {
    decoder = OpInputTuple.getTupleDecoder(schema, ByteOrder.nativeOrder());
    encoder = new ObjectTupleEncoder(schema);
  }

  @Override
  public long getEncodedSize(Tuple tuple) {
    return encoder.getMaxSerializedSize(tuple);
  }

  @Override
  public void encodeTuple(Tuple tuple, ByteBuffer buffer) {
    encoder.encode(tuple, buffer.order(ByteOrder.nativeOrder()));
  }

  @Override
  public Tuple decodeTuple(ByteBuffer buffer) {
    return decoder.decode(buffer);
  }
}
