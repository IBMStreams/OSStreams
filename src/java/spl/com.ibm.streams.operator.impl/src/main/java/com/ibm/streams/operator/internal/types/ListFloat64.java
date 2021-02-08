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
import java.nio.DoubleBuffer;
import java.util.AbstractList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

/**
 * Format for list<float64> which handles the value as a double[] instead of a List object. Format:
 * Number of items (N) as followed by N doubles
 */
final class ListFloat64 extends AbstractBufferList<DoubleBuffer, double[]> {

  static final class DoubleList extends AbstractList<Double> implements Serializable {
    /** */
    private static final long serialVersionUID = -2326872562107556079L;

    private final double[] value;

    DoubleList(double[] value) {
      this.value = value;
    }

    @Override
    public Double get(int index) {
      return value[index];
    }

    @Override
    public int size() {
      return value.length;
    }

    double[] getValue(final int copyCount) {
      if (copyCount == value.length) return value;
      double[] sublist = new double[copyCount];
      System.arraycopy(value, 0, sublist, 0, copyCount);
      return sublist;
    }
  }

  private static final double[] EMPTY = new double[0];

  ListFloat64(ImplementationType<?> elementType) {
    super(EMPTY, elementType);
  }

  ListFloat64(ImplementationType<?> elementType, int boundedLength) {
    super(EMPTY, elementType, boundedLength);
  }

  /** Doubles are eight bytes. */
  @Override
  int elementByteSize() {
    return 8;
  }

  @Override
  double[] newList(int n) {
    return new double[n];
  }

  @Override
  double[] cloneList(double[] list) {
    return list.clone();
  }

  @Override
  void randomList(Random rand, double[] list) {
    for (int i = 0; i < list.length; i++) list[i] = rand.nextDouble();
  }

  @Override
  public String getString(Object value) {
    // When in a composite the object is a List
    // and getString will be called for the
    // natural string representation of that composite.
    if (value instanceof List) return value.toString();

    return Arrays.toString((double[]) value);
  }

  @Override
  DoubleBuffer readList(DoubleBuffer buffer, double[] list) {
    return buffer.get(list);
  }

  @Override
  DoubleBuffer writeList(DoubleBuffer buffer, double[] list) {
    return buffer.put(list);
  }

  @Override
  DoubleBuffer typedBuffer(ByteBuffer bytes) {
    return bytes.asDoubleBuffer();
  }

  @Override
  DoubleBuffer getTypedReadOnlyBuffer(double[] list) {
    return DoubleBuffer.wrap(list).asReadOnlyBuffer();
  }
  /**
   * Use a specific implementation of AbstractList that wraps the primitive double array. Such
   * implementations of AbstractList are read-only.
   */
  @Override
  public final List<?> convertToList(final double[] value) {
    return new DoubleList(value);
  }

  @Override
  protected double[] convertListToArray(List<?> list, int copyCount) {
    if (list instanceof DoubleList) {
      return cloneList(((DoubleList) list).getValue(copyCount));
    }
    return genericListToArray(list, copyCount);
  }

  @Override
  Object safeCollectionCopy(Object list) {
    if (list instanceof DoubleList) return list;
    List<?> l = (List<?>) list;
    return new DoubleList(genericListToArray(l, l.size()));
  }
}
