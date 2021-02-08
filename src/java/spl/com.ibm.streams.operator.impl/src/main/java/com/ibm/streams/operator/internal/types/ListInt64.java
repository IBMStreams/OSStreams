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

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.LongBuffer;
import java.util.AbstractList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
 * Format for list<int64> which handles the value as a long[] instead of a List object. Format:
 * Number of items (N) as followed by N longs
 */
final class ListInt64 extends AbstractBufferList<LongBuffer, long[]> {

  static final class LongList extends AbstractList<Long> implements Serializable {
    /** */
    private static final long serialVersionUID = -9070599414023750919L;

    private final long[] value;

    LongList(long[] value) {
      this.value = value;
    }

    @Override
    public Long get(int index) {
      return value[index];
    }

    @Override
    public int size() {
      return value.length;
    }

    long[] getValue(final int copyCount) {
      if (copyCount == value.length) return value;
      long[] sublist = new long[copyCount];
      System.arraycopy(value, 0, sublist, 0, copyCount);
      return sublist;
    }

    void putInternal(final ByteBuffer bytes) {
      for (long v : value) bytes.putLong(v);
    }

    LongBuffer asLongBuffer() {
      return LongBuffer.wrap(value).asReadOnlyBuffer();
    }
  }

  private static final long[] EMPTY = new long[0];

  ListInt64(ImplementationType<?> elementType) {
    super(EMPTY, elementType);
  }

  ListInt64(ImplementationType<?> elementType, int boundedLength) {
    super(EMPTY, elementType, boundedLength);
  }

  @Override
  int elementByteSize() {
    return 8;
  }

  @Override
  long[] newList(int n) {
    return new long[n];
  }

  @Override
  long[] cloneList(long[] list) {
    return list.clone();
  }

  @Override
  void randomList(final Random rand, final long[] list) {
    for (int i = 0; i < list.length; i++) {
      list[i] = rand.nextLong();
    }
  }

  @Override
  public String getString(Object value) {
    // When in a composite the object is a List
    // and getString will be called for the
    // natural string representation of that composite.
    if (value instanceof List) return value.toString();

    return Arrays.toString((long[]) value);
  }

  @Override
  LongBuffer readList(LongBuffer buffer, long[] list) {
    return buffer.get(list);
  }

  @Override
  LongBuffer writeList(LongBuffer buffer, long[] list) {
    return buffer.put(list);
  }

  @Override
  LongBuffer typedBuffer(ByteBuffer bytes) {
    return bytes.asLongBuffer();
  }

  @Override
  LongBuffer getTypedReadOnlyBuffer(long[] list) {
    return LongBuffer.wrap(list).asReadOnlyBuffer();
  }

  /**
   * Use a specific implementation of AbstractList that wraps the primitive long array. Such
   * implementations of AbstractList are read-only.
   */
  @Override
  public final List<?> convertToList(final long[] value) {
    return new LongList(value);
  }

  @Override
  long[] convertListToArray(List<?> list, int copyCount) {
    if (list instanceof LongList) {
      return cloneList(((LongList) list).getValue(copyCount));
    }
    return genericListToArray(list, copyCount);
  }

  @Override
  Object safeCollectionCopy(Object list) {
    if (list instanceof LongList) return list;
    return ListUint64.convertListToLongList((List<?>) list);
  }
}
