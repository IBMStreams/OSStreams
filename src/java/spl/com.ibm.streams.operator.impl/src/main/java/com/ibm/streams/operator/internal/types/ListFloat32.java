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
import java.nio.FloatBuffer;
import java.util.AbstractList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
 * Format for list<float32> which handles the value as a float[] instead of a List object. Format:
 * Number of items (N) as followed by N floats
 */
final class ListFloat32 extends AbstractBufferList<FloatBuffer, float[]> {

  static final class FloatList extends AbstractList<Float> implements Serializable {
    /** */
    private static final long serialVersionUID = 695252209686915870L;

    private final float[] value;

    FloatList(float[] value) {
      this.value = value;
    }

    @Override
    public Float get(int index) {
      return value[index];
    }

    @Override
    public int size() {
      return value.length;
    }

    float[] getValue(final int copyCount) {
      if (copyCount == value.length) return value;
      float[] sublist = new float[copyCount];
      System.arraycopy(value, 0, sublist, 0, copyCount);
      return sublist;
    }
  }

  private static final float[] EMPTY = new float[0];

  ListFloat32(ImplementationType<?> elementType) {
    super(EMPTY, elementType);
  }

  ListFloat32(ImplementationType<?> elementType, int boundedLength) {
    super(EMPTY, elementType, boundedLength);
  }

  /** Floats are four bytes. */
  @Override
  int elementByteSize() {
    return 4;
  }

  @Override
  float[] newList(int n) {
    return new float[n];
  }

  @Override
  float[] cloneList(float[] list) {
    return list.clone();
  }

  @Override
  void randomList(Random rand, float[] list) {
    for (int i = 0; i < list.length; i++) list[i] = rand.nextFloat();
  }

  @Override
  FloatBuffer readList(FloatBuffer buffer, float[] list) {
    return buffer.get(list);
  }

  @Override
  FloatBuffer writeList(FloatBuffer buffer, float[] list) {
    return buffer.put(list);
  }

  @Override
  FloatBuffer typedBuffer(ByteBuffer bytes) {
    return bytes.asFloatBuffer();
  }

  @Override
  FloatBuffer getTypedReadOnlyBuffer(float[] list) {
    return FloatBuffer.wrap(list).asReadOnlyBuffer();
  }
  /**
   * Use a specific implementation of AbstractList that wraps the primitive float array. Such
   * implementations of AbstractList are read-only.
   */
  @Override
  public final List<?> convertToList(final float[] value) {
    return new ListFloat32.FloatList(value);
  }

  @Override
  protected float[] convertListToArray(List<?> list, int copyCount) {
    if (list instanceof ListFloat32.FloatList) {
      FloatList fl = (FloatList) list;
      return cloneList(fl.getValue(copyCount));
    }
    return genericListToArray(list, copyCount);
  }

  @Override
  Object safeCollectionCopy(Object list) {
    if (list instanceof FloatList) return list;
    List<?> l = (List<?>) list;
    return new FloatList(genericListToArray(l, l.size()));
  }

  @Override
  public String getString(Object value) {
    // When in a composite the object is a List
    // and getString will be called for the
    // natural string representation of that composite.
    if (value instanceof List) return value.toString();

    return Arrays.toString((float[]) value);
  }
}
