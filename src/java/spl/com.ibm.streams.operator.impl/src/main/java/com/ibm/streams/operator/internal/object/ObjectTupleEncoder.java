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
import com.ibm.streams.operator.internal.network.TupleEncoder;
import com.ibm.streams.operator.internal.network.TupleSerialization;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.internal.runtime.Schema;
import java.nio.ByteBuffer;

/**
 * Generic TupleEncoder that obtains all attributes as objects and then serializes them as Streams
 * types.
 */
public final class ObjectTupleEncoder extends TupleSerialization implements TupleEncoder<Tuple> {

  private final long fixedBufferSize;
  private final boolean hasFixedBufferSize;

  public ObjectTupleEncoder(Schema schema) {
    super(schema);

    long obs = 0;
    boolean fixed = true;
    for (TypeEncoding encoding : encodings) {
      long typeSize = encoding.getSerializedSize();
      if (typeSize == TypeEncoding.VARIABLE_LENGTH) fixed = false;
      else obs += typeSize;
    }

    fixedBufferSize = obs;
    hasFixedBufferSize = fixed;
  }

  @Override
  public long getMaxSerializedSize(Tuple tuple) {
    if (hasFixedBufferSize) return fixedBufferSize;

    return ((ObjectInputTuple) tuple).getMaxSerializedSize(this);
  }

  public void encode(Tuple tuple, ByteBuffer rawTuple) {

    ((ObjectInputTuple) tuple).encode(this, rawTuple);
  }

  long getMaxSerializedSize(Object[] values) {

    long size = 0;
    for (int i = 0; i < encodings.length; i++) {

      TypeEncoding encoding = encodings[i];

      long valueSize = encoding.getSerializedSize();
      if (valueSize != TypeEncoding.VARIABLE_LENGTH) size += valueSize;
      else size += encoding.getMaxSerializedSize(values[i]);
    }
    return size;
  }

  void encode(Object[] values, ByteBuffer rawTuple) {

    for (int i = 0; i < encodings.length; i++) {

      encodings[i].writeValue(rawTuple, values[i]);
    }
  }
}
