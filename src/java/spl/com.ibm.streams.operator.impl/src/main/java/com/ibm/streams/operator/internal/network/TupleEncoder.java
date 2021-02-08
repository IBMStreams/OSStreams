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

/**
 * Encode a tuple into its serialized form, data only. Meta-data surrounding the attribute values is
 * handled elsewhere.
 *
 * <p>TupleEncoders are thread-safe to allow multiple threads submit tuples concurrently.
 *
 * @param <T> Type of the tuple being encoded.
 */
public interface TupleEncoder<T> {

  /**
   * For the given tuple return the maximum number of bytes required to serialize it using this
   * encoder. This is used to create the ByteBuffer, the return size is not used as the definitive
   * length of the tuple.
   *
   * @param tuple Tuple to be serialized.
   * @return Number of bytes required to serialize tuple.
   */
  public long getMaxSerializedSize(T tuple);

  /**
   * Encode the tuple's attributes into the ByteBuffer in the required order. Only the attributes
   * are encoded, no meta-information.
   *
   * @param tuple
   * @param rawTuple
   */
  public void encode(T tuple, ByteBuffer rawTuple);
}
