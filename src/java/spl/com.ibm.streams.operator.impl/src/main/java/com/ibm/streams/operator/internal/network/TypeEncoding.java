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
 * Interface to provide serialization of a type's value to and from a ByteBuffer. TypeEncoding must
 * be thread safe as multiple threads may be concurrently submitting or processing tuples on the
 * same port (and hence same TypeEncoding) object.
 *
 * <p>TypeEncodings with state however are allowed (as long as they are thread-safe), to ensure that
 * encoding and decoding of types is not single-threaded across all schemas. That is each attribute
 * will have its own encoder.
 */
public interface TypeEncoding {

  int VARIABLE_LENGTH = 0;

  /** Read a value as an object for this type from the ByteBuffer. */
  public abstract Object readValue(ByteBuffer bytes);

  /** Return the fixed size of the type when serialized, VARIABLE_SIZE for variable length types. */
  public abstract int getSerializedSize();

  /**
   * Get the maximum number of bytes required to write this specific value. For fixed value types
   * this must return the same value as getSerializedSize().
   */
  public long getMaxSerializedSize(Object value);

  /** Write a value for this type to the ByteBuffer. */
  public abstract void writeValue(ByteBuffer bytes, Object value);
}
