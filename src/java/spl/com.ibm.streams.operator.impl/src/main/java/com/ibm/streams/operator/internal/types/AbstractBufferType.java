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

package com.ibm.streams.operator.internal.types;

import java.nio.Buffer;

/**
 * Generic class to handle List types where the reading of the elements is handled by a typed Buffer
 * with fixed size elements, e.g. IntBuffer.
 */
abstract class AbstractBufferType<B extends Buffer, T> extends ImplementationType<T> {

  AbstractBufferType(MetaType metaType, T defaultValue) {
    super(metaType, defaultValue);
  }

  @Override
  public String getString(Object value) {
    throw invalidConversion("String");
  }

  /** Return a read only view of the Type's value through the Tuple api. */
  @SuppressWarnings("unchecked")
  public final Buffer getBuffer(Object value) {
    return getReadOnlyBuffer((T) value);
  }

  /** Return a read only view of the Type's value. */
  abstract Buffer getReadOnlyBuffer(T value);
}
