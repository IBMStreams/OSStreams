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

import com.ibm.streams.operator.internal.ports.RuntimeStream;
import java.nio.ByteBuffer;

/**
 * Interface to handle stream processing from a serialized form of the stream, e.g. from a network
 * or from a C++ primitive operator.
 *
 * <p>Implementations are thread-safe in that multiple threads may call any method concurrently and
 * the serialized form will be converted correctly and result in concurrent calls to the user's
 * Operator implementation. The Operator implementation must itself be thread-safe.
 */
public interface InputPortDecoder extends RuntimeStream<ByteBuffer> {

  /**
   * Deserialize the tuple and then have it passed onto the Operator implementation. The format of
   * the buffer is
   *
   * <OL>
   *   <LI>N attributes in their serialized form
   * </OL>
   *
   * @param buffer Buffer containing the complete serialized tuple.
   * @throws Exception
   */
  public void processRawTuple(final ByteBuffer buffer) throws Exception;

  /**
   * Process a punctuation mark on this port decoding it from an integer into a valid punctuation
   * mark.
   *
   * @param mark
   * @throws Exception
   */
  public void processPunctuation(int mark) throws Exception;
}
