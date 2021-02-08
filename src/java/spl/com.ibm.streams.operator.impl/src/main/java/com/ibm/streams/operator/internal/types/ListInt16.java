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
import java.nio.ShortBuffer;
import java.util.AbstractList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
 * Format for list<int16> which handles the value as a short[] instead of a List object. Format:
 * Number of items (N) as followed by N shorts
 */
final class ListInt16 extends AbstractBufferList<ShortBuffer, short[]> {

  static final class ShortList extends AbstractList<Short> implements Serializable {
    /** */
    private static final long serialVersionUID = -6820049979962388229L;

    private final short[] value;

    ShortList(short[] value) {
      this.value = value;
    }

    @Override
    public Short get(int index) {
      return value[index];
    }

    @Override
    public int size() {
      return value.length;
    }

    short[] getValue(final int copyCount) {
      if (copyCount == value.length) return value;
      short[] sublist = new short[copyCount];
      System.arraycopy(value, 0, sublist, 0, copyCount);
      return sublist;
    }

    void putInternal(ByteBuffer bytes) {
      for (short v : value) bytes.putShort(v);
    }

    ShortBuffer asShortBuffer() {
      return ShortBuffer.wrap(value).asReadOnlyBuffer();
    }
  }

  private static final short[] EMPTY = new short[0];

  ListInt16(ImplementationType<?> elementType) {
    super(EMPTY, elementType);
  }

  ListInt16(ImplementationType<?> elementType, int boundedLength) {
    super(EMPTY, elementType, boundedLength);
  }

  /** Shorts are two bytes. */
  @Override
  int elementByteSize() {
    return 2;
  }

  @Override
  short[] newList(int n) {
    return new short[n];
  }

  @Override
  short[] cloneList(short[] list) {
    return list.clone();
  }

  @Override
  ShortBuffer readList(ShortBuffer buffer, short[] list) {
    return buffer.get(list);
  }

  @Override
  ShortBuffer writeList(ShortBuffer buffer, short[] list) {
    return buffer.put(list);
  }

  @Override
  ShortBuffer typedBuffer(ByteBuffer bytes) {
    return bytes.asShortBuffer();
  }

  @Override
  void randomList(final Random rand, final short[] list) {
    for (int i = 0; i < list.length; i++) list[i] = (short) rand.nextInt();
  }

  @Override
  public String getString(Object value) {
    // When in a composite the object is a List
    // and getString will be called for the
    // natural string representation of that composite.
    if (value instanceof List) return value.toString();

    return Arrays.toString((short[]) value);
  }

  @Override
  ShortBuffer getTypedReadOnlyBuffer(short[] list) {
    return ShortBuffer.wrap(list).asReadOnlyBuffer();
  }
  /**
   * Use a specific implementation of AbstractList that wraps the primitive long array. Such
   * implementations of AbstractList are read-only.
   */
  @Override
  public final List<?> convertToList(final short[] value) {
    return new ShortList(value);
  }

  @Override
  short[] convertListToArray(List<?> list, int copyCount) {
    if (list instanceof ShortList) {
      return cloneList(((ShortList) list).getValue(copyCount));
    }
    return genericListToArray(list, copyCount);
  }

  @Override
  Object safeCollectionCopy(Object list) {
    if (list instanceof ShortList) return list;
    return ListUint16.convertListToShortList((List<?>) list);
  }
}
