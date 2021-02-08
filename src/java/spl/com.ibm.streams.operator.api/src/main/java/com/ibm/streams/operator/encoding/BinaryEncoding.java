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

package com.ibm.streams.operator.encoding;

import com.ibm.streams.operator.Tuple;
import java.nio.ByteBuffer;

/**
 * Interface to encode and decode Tuples into a binary encoding. Instances of {@code BinaryEncoding}
 * are not thread safe. Applications may create an instance for each thread or synchronize access to
 * a {@code BinaryEncoding} instance.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface BinaryEncoding {

  /**
   * Get the maximum number of bytes required to write out a binary encoded version of a tuple using
   * this encoding.
   *
   * @param tuple Tuple to be encoded.
   * @return Maximum number of bytes required to encode {@code tuple}.
   */
  public long getEncodedSize(Tuple tuple);

  /**
   * Encode {@code tuple} into {@code buffer} using this encoding. For {@code tuple} to be
   * successfully encoded {@code buffer.remaining()} must return at least the number of bytes
   * returned by {@link BinaryEncoding#getEncodedSize(Tuple)};
   *
   * @param tuple Tuple to be encoded.
   * @param buffer Buffer to hold encoded tuple.
   */
  public void encodeTuple(Tuple tuple, ByteBuffer buffer);

  /**
   * Decode a encoded tuple from {@code buffer} and return it as an immutable {@link Tuple}.
   *
   * @param buffer Buffer containing encoded tuple.
   * @return Tuple instance decoded from {@code buffer}.
   */
  public Tuple decodeTuple(ByteBuffer buffer);
}
