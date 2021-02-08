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

import static com.ibm.streams.operator.internal.network.TupleSerialization.skip;

import com.ibm.streams.operator.internal.network.TupleSerialization.BLen;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import java.nio.ByteBuffer;
import java.util.Collection;

@SuppressWarnings("unchecked")
abstract class CollectionEncoding<C extends Collection> implements TypeEncoding {

  final ImplementationType elementType;
  final TypeEncoding elementEncoding;

  CollectionEncoding(ImplementationType elementType) {
    this.elementType = elementType;
    this.elementEncoding = elementType.getEncoding();
  }

  BLen getBLen() {
    return BLen.UNBOUNDED;
  }

  @Override
  public int getSerializedSize() {
    return VARIABLE_LENGTH;
  }

  /*
   * Methods to handle unbound collections.
   */

  final long getUnboundValueSize(C collection) {
    long totalLength = getBLen().getSerializedSize();

    final int elementLength = elementEncoding.getSerializedSize();
    if (elementLength == TypeEncoding.VARIABLE_LENGTH) {
      for (Object element : collection)
        totalLength += elementEncoding.getMaxSerializedSize(element);
    } else {
      totalLength += (collection.size() * (long) elementLength);
    }
    return totalLength;
  }

  final void writeUnboundValue(ByteBuffer bytes, C collection) {
    getBLen().putLength(bytes, collection.size());
    for (Object element : collection)
      elementEncoding.writeValue(bytes, elementType.convertFromCollectionValue(element));
  }

  C readUnboundValue(ByteBuffer bytes) {
    final int n = getBLen().getLength(bytes);
    if (n == 0) return emptyCollection();

    C value = getCollection(n);
    for (int i = 0; i < n; i++)
      value.add(elementType.convertToCollectionValue(elementEncoding.readValue(bytes)));
    return getReadOnly(value);
  }

  /*
   * Methods for bound collection types.
   */

  /**
   * Calculate the serialized length of the value of a bounded collection including the default
   * values.
   *
   * @param collection
   * @param bi
   * @return
   */
  final long getBoundValueSize(C collection, BoundInfo bound) {

    final int boundedLength = bound.getBoundedLength();
    long totalLength = bound.getSizeType().getSerializedSize();

    final int elementLength = elementEncoding.getSerializedSize();
    if (elementLength == TypeEncoding.VARIABLE_LENGTH) {
      for (Object element : collection)
        totalLength += elementEncoding.getMaxSerializedSize(element);

      if (collection.size() < boundedLength) {
        totalLength +=
            getUnusedSerializedSize(
                elementType, elementEncoding, (boundedLength - collection.size()));
      }
    } else {
      totalLength += (boundedLength * (long) elementLength);
    }

    return totalLength;
  }

  /*
   * followed by the corresponding in-use map showing the first (used)
   * elements are in-use. The JavaOp will need to re-hash it to make it match
   * the C++ layout.
   */
  /** Write out using the non-bounded code and then add in (bound-used) number of default values. */
  final int writeBoundValue(ByteBuffer bytes, C collection, BoundInfo bound) {

    writeUnboundValue(bytes, collection);

    final int unused = bound.getBoundedLength() - collection.size();
    for (int i = 0; i < unused; i++) {
      elementEncoding.writeValue(bytes, elementType.defaultValue);
    }

    return collection.size();
  }

  /**
   * Cannot re-use the non-bounded code as when we read the elements we need to keep their position
   * so we can know if they are used or not once the in-use map is read. The non-bounded code
   * directly adds the elements into the set and hence loses positional information. We need to read
   * into an array and then see which are required.
   */
  final int readBoundLength(ByteBuffer bytes, BoundInfo bound) {

    final int inUse = bound.getSizeType().getLength(bytes);
    if (inUse == 0) {
      skipElements(bytes, bound.getBoundedLength());
    }
    return inUse;
  }

  final int calculateFixedLength(BoundInfo bound, boolean usageArray) {
    int fixedLength;
    if (elementEncoding.getSerializedSize() == VARIABLE_LENGTH) {
      fixedLength = VARIABLE_LENGTH;
    } else {
      long totalLength = getBLen().getSerializedSize();
      if (usageArray) totalLength += bound.getBoundedLength(); // in-use array
      totalLength += ((long) bound.getBoundedLength()) * elementEncoding.getSerializedSize();
      if (totalLength > Integer.MAX_VALUE) fixedLength = VARIABLE_LENGTH;
      else fixedLength = (int) totalLength;
    }
    return fixedLength;
  }

  abstract C getReadOnly(C value);

  abstract C getCollection(int n);

  abstract C emptyCollection();

  /**
   * When serializing BMAP and BSET Java writes all the used values, then a set of empty/default
   * values up to the bound and then a used map with the first used positions set to one and the
   * remaining set to 0. Before this is processed by C++ the tuple must be normalized
   */
  static void writeJavaUsedMap(ByteBuffer bytes, BoundInfo bound, int inUse) {
    for (int i = 0; i < bound.getBoundedLength(); i++) {
      bytes.put((byte) (i < inUse ? 1 : 0));
    }
  }

  static long getUnusedSerializedSize(
      ImplementationType<?> type, TypeEncoding encoding, int unusedCount) {

    return unusedCount * encoding.getMaxSerializedSize(type.defaultValue);
  }

  void skipElements(ByteBuffer bytes, int count) {
    int elementSize = elementEncoding.getSerializedSize();
    if (elementSize != VARIABLE_LENGTH) {
      skip(bytes, count, elementSize);
      return;
    }

    for (int i = 0; i < count; i++) elementEncoding.readValue(bytes);
  }

  /**
   * Skip the in-use array for a bounded collection, used when the number of used elements is zero,
   * thus no need to read the array.
   */
  static void skipInUseArray(ByteBuffer bytes, BoundInfo bound) {
    skip(bytes, bound.getBoundedLength());
  }
}
