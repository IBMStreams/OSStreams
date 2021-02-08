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

/**
 * Interface to encode and decode Tuples into a character encoding. Instances of {@code
 * CharacterEncoding} are thread safe.
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
public interface CharacterEncoding {

  /**
   * Encode {@code tuple} into {@code buffer} using this encoding.
   *
   * @param tuple Tuple to be encoded.
   * @param buffer Buffer to hold encoded tuple.
   */
  public void encodeTuple(Tuple tuple, StringBuilder buffer);

  /**
   * Encode a tuple as a {@code String} using this encoding.
   *
   * @param tuple Tuple to be encoded as a String in this encoding.
   * @return {@code tuple} encoded as a {@code String} using this encoding.
   */
  public String encodeTuple(Tuple tuple);

  /**
   * Decode a encoded tuple from {@code buffer} and return it as an immutable {@link Tuple}.
   *
   * @param sequence Characters containing encoded tuple.
   * @return Tuple instance decoded from {@code buffer}.
   */
  // public Tuple decodeTuple(CharSequence sequence);
}
