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
import java.nio.IntBuffer;
import java.util.AbstractList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
 * Format for list<int32> which handles the value as a int[] instead of a List object. Format:
 * Number of items (N) as followed by N int
 */
class ListInt32 extends AbstractBufferList<IntBuffer, int[]> {

  static final class IntegerList extends AbstractList<Integer> implements Serializable {
    /** */
    private static final long serialVersionUID = 6978097660376419631L;

    private final int[] value;

    IntegerList(int[] value) {
      this.value = value;
    }

    @Override
    public Integer get(int index) {
      return value[index];
    }

    @Override
    public int size() {
      return value.length;
    }

    int[] getValue(final int copyCount) {
      if (copyCount == value.length) return value;
      int[] sublist = new int[copyCount];
      System.arraycopy(value, 0, sublist, 0, copyCount);
      return sublist;
    }

    void putInternal(ByteBuffer bytes) {
      for (int i : value) bytes.putInt(i);
    }

    IntBuffer asIntBuffer() {
      return IntBuffer.wrap(value).asReadOnlyBuffer();
    }
  }

  private static final int[] EMPTY = new int[0];

  ListInt32(ImplementationType<?> elementType) {
    super(EMPTY, elementType);
  }

  ListInt32(ImplementationType<?> elementType, int boundedLength) {
    super(EMPTY, elementType, boundedLength);
  }

  /** Integers are four bytes. */
  @Override
  int elementByteSize() {
    return 4;
  }

  @Override
  int[] newList(int n) {
    return new int[n];
  }

  @Override
  int[] cloneList(int[] list) {
    return list.clone();
  }

  @Override
  void randomList(Random rand, int[] list) {
    for (int i = 0; i < list.length; i++) list[i] = rand.nextInt();
  }

  @Override
  public String getString(Object value) {
    // When in a composite the object is a List
    // and getString will be called for the
    // natural string representation of that composite.
    if (value instanceof List) return value.toString();

    return Arrays.toString((int[]) value);
  }

  @Override
  IntBuffer readList(IntBuffer buffer, int[] list) {
    return buffer.get(list);
  }

  @Override
  IntBuffer writeList(IntBuffer buffer, int[] list) {
    return buffer.put(list);
  }

  @Override
  IntBuffer typedBuffer(ByteBuffer bytes) {
    return bytes.asIntBuffer();
  }

  @Override
  IntBuffer getTypedReadOnlyBuffer(int[] list) {
    return IntBuffer.wrap(list).asReadOnlyBuffer();
  }

  /**
   * Use a specific implementation of AbstractList that wraps the primitive int array. Such
   * implementations of AbstractList are read-only.
   */
  @Override
  public final List<?> convertToList(final int[] value) {
    return new IntegerList(value);
  }

  @Override
  int[] convertListToArray(List<?> list, int copyCount) {
    if (list instanceof IntegerList) {
      return cloneList(((IntegerList) list).getValue(copyCount));
    }
    return genericListToArray(list, copyCount);
  }

  @Override
  Object safeCollectionCopy(Object list) {
    if (list instanceof IntegerList) return list;
    return ListUint32.convertListToIntegerList((List<?>) list);
  }
}
