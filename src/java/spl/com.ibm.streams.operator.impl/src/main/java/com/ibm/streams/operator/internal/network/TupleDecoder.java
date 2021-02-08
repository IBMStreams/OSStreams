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
 * Interface to decode a tuple from a ByteBuffer. The ByteBuffer is positioned at the start of the
 * data for the first attribute. Note that the ByteBuffer may contain more information than a single
 * tuple, the decode method must only read the information for a single tuple.
 *
 * <p>Decoder is thread-safe for decoding from a single ByteBuffer. That is the decode method may be
 * called by multiple threads with different ByteBuffers but sharing a single ByteBuffer among
 * multiple threads is not supported (e.g. a ByteBuffer that contains multiple tuples can not be
 * decoded in parallel by multiple threads).
 *
 * @param <T> Type of the tuple being decoded.
 */
public interface TupleDecoder<T> {

  /**
   * Decode the serialized form of the tuple and return an Tuple implementation representing its
   * attribute values. The format of the buffer (at this point) is
   *
   * <OL>
   *   <LI>N attributes in their serialized form
   * </OL>
   *
   * Any trailing data after the N attributes is ignored.
   *
   * @param rawTuple ByteBuffer positioned at the first attribute.
   * @return A valid tuple.
   */
  public T decode(ByteBuffer rawTuple);

  public T newTuple(Object[] values);
}
