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

import com.ibm.streams.operator.internal.network.TupleSerialization;
import java.nio.ByteBuffer;
import java.util.List;

/** Uses ListInt8.ByteList for efficiency. See ListUint8 for details. */
class ListUint8BoundedEncoding extends ListUint8Encoding {

  private final BoundInfo bound;
  private final int fixedLength;

  ListUint8BoundedEncoding(BoundInfo bound) {
    super(bound.getSizeType());
    this.bound = bound;
    fixedLength = getBLen().getSerializedSize() + bound.getBoundedLength();
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    List<Byte> list = readList(bytes);
    int unused = bound.getBoundedLength() - list.size();
    if (unused != 0) TupleSerialization.skip(bytes, unused);
    return list;
  }

  @Override
  public int getSerializedSize() {
    return fixedLength;
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    return fixedLength;
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    final int n = writeList(bytes, value);
    final int unused = bound.getBoundedLength() - n;
    for (int i = 0; i < unused; i++) {
      bytes.put((byte) 0);
    }
  }
}
