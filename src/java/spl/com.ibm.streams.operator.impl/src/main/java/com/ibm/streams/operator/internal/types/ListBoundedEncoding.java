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

import com.ibm.streams.operator.internal.network.TupleSerialization.BLen;
import java.nio.ByteBuffer;
import java.util.List;

/**
 * A bounded list has a format similar to an unbounded list except the encoding of the number of
 * elements and then serialized value is padded by N-U default elements where N is the bound and U
 * the number of used elements in the list.
 */
class ListBoundedEncoding extends ListEncoding {

  private final BoundInfo bound;
  private final int fixedLength;

  ListBoundedEncoding(ImplementationType<?> elementType, BoundInfo bound) {
    super(elementType);
    this.bound = bound;
    this.fixedLength = calculateFixedLength(bound, false);
  }

  BLen getBLen() {
    return bound.getSizeType();
  }

  @Override
  public int getSerializedSize() {
    return fixedLength;
  }

  @SuppressWarnings("unchecked")
  @Override
  public long getMaxSerializedSize(Object value) {
    return getBoundValueSize((List<Object>) value, bound);
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    List<Object> list = readUnboundValue(bytes);
    skipElements(bytes, bound.getBoundedLength() - list.size());
    return list;
  }

  @SuppressWarnings("unchecked")
  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    writeBoundValue(bytes, (List<Object>) value, bound);
  }
}
