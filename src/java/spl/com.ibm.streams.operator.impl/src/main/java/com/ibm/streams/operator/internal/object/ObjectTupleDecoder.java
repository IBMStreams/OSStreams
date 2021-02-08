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

import com.ibm.streams.operator.internal.network.TupleDecoder;
import com.ibm.streams.operator.internal.network.TupleSerialization;
import com.ibm.streams.operator.internal.runtime.Schema;
import java.nio.ByteBuffer;

/**
 * Generic TupleDecoder that de-serializes all attribute types as objects and works with a object
 * based Tuple implementation. Any payload data in the rawTuple buffer is simply discarded by not
 * reading it.
 */
public abstract class ObjectTupleDecoder<T> extends TupleSerialization implements TupleDecoder<T> {

  ObjectTupleDecoder(Schema schema) {
    super(schema);
  }

  public final T decode(ByteBuffer rawTuple) {

    final Object[] values = new Object[encodings.length];
    for (int i = 0; i < values.length; i++) {
      values[i] = encodings[i].readValue(rawTuple);
    }
    return newTuple(values);
  }
}
