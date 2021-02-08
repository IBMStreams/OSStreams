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

import java.nio.ByteBuffer;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

@SuppressWarnings("unchecked")
class SetEncoding extends CollectionEncoding<Set<Object>> {

  SetEncoding(ImplementationType elementType) {
    super(elementType);
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    return getUnboundValueSize((Set<Object>) value);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    writeUnboundValue(bytes, (Set<Object>) value);
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return readUnboundValue(bytes);
  }

  @Override
  Set<Object> emptyCollection() {
    return Collections.EMPTY_SET;
  }

  @Override
  Set<Object> getCollection(int n) {
    return new HashSet<Object>(n);
  }

  @Override
  Set<Object> getReadOnly(Set<Object> value) {
    return Collections.unmodifiableSet(value);
  }
}
