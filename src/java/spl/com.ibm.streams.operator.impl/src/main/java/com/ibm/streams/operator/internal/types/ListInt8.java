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
import java.util.AbstractList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
 * Format for list<int8> which handles the value as a byte[] instead of a List object. Format:
 * Number of items (N) as followed by N bytes
 */
final class ListInt8 extends AbstractBufferList<ByteBuffer, byte[]> {

  static final class ByteList extends AbstractList<Byte> implements Serializable {
    /** */
    private static final long serialVersionUID = 6137026139604536253L;

    private final byte[] value;

    ByteList(byte[] value) {
      this.value = value;
    }

    @Override
    public Byte get(int index) {
      return value[index];
    }

    @Override
    public int size() {
      return value.length;
    }

    byte[] getValue(final int copyCount) {
      if (copyCount == value.length) return value;
      byte[] sublist = new byte[copyCount];
      System.arraycopy(value, 0, sublist, 0, copyCount);
      return sublist;
    }

    void putInternal(ByteBuffer bytes) {
      bytes.put(value);
    }

    ByteBuffer asByteBuffer() {
      return ByteBuffer.wrap(value).asReadOnlyBuffer();
    }
  }

  private static final byte[] EMPTY = new byte[0];

  ListInt8(ImplementationType<?> elementType) {
    super(EMPTY, elementType);
  }

  ListInt8(ImplementationType<?> elementType, int boundedLength) {
    super(EMPTY, elementType, boundedLength);
  }

  @Override
  byte[] newList(int n) {
    return new byte[n];
  }

  @Override
  void randomList(Random rand, byte[] list) {
    rand.nextBytes(list);
  }

  @Override
  public String getString(Object value) {
    // When in a composite the object is a List
    // and getString will be called for the
    // natural string representation of that composite.
    if (value instanceof List) return value.toString();
    return Arrays.toString((byte[]) value);
  }

  @Override
  ByteBuffer readList(ByteBuffer buffer, byte[] list) {
    return buffer.get(list);
  }

  @Override
  ByteBuffer writeList(ByteBuffer buffer, byte[] list) {
    return buffer.put(list);
  }

  @Override
  int elementByteSize() {
    return 1;
  }

  /**
   * Here we just return the actual buffer as we can read and write that directly for byte arrays.
   * This means that our adjustPosition must be a no-op to since the read/write will adjust position
   * in the main buffer.
   */
  @Override
  ByteBuffer typedBuffer(ByteBuffer bytes) {
    return bytes;
  }

  /** No-op see typedBuffer(). */
  @Override
  void adjustPosition(ByteBuffer bytes, int n) {}

  @Override
  ByteBuffer getTypedReadOnlyBuffer(byte[] list) {
    return ByteBuffer.wrap(list).asReadOnlyBuffer();
  }

  @Override
  byte[] cloneList(byte[] list) {
    return list.clone();
  }

  /**
   * Use a specific implementation of AbstractList that wraps the primitive byte array. Such
   * implementations of AbstractList are read-only.
   */
  @Override
  public final List<?> convertToList(final byte[] value) {
    return new ByteList(value);
  }

  @Override
  byte[] convertListToArray(List<?> list, int copyCount) {
    if (list instanceof ByteList) {
      return cloneList(((ByteList) list).getValue(copyCount));
    }
    return genericListToArray(list, copyCount);
  }

  @SuppressWarnings("unchecked")
  @Override
  Object safeCollectionCopy(Object list) {
    if (list instanceof ByteList) return list;
    return ListUint8.convertListToByteList((List<? extends Object>) list);
  }
}
