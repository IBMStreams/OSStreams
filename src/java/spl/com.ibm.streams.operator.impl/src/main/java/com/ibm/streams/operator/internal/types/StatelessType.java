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

import com.ibm.streams.operator.internal.network.TypeEncoding;

/**
 * Type with stateless TypeEncoding, which means there is no need to synchronize to ensure
 * thread-safety as required by TypeEndcoding's definition, since no state is used to decode values.
 */
abstract class StatelessType<T> extends ImplementationType<T> implements TypeEncoding {

  StatelessType(MetaType metaType, T defaultValue) {
    super(metaType, defaultValue);
  }

  @Override
  public final TypeEncoding getEncoding() {
    return this;
  }

  /**
   * Get the maximum number of bytes required to write this specific value. This returns
   * getSerializedSize() which works for fixed length types, non-fixed length types must override
   * this method.
   */
  @Override
  public long getMaxSerializedSize(Object value) {
    return this.getSerializedSize();
  }

  /**
   * Assume any value representing the attribute is immutable, sub-classes need to override if this
   * is not the case.
   */
  @Override
  T safeCopy(T value) {
    return value;
  }
}
