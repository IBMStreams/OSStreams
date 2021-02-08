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
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * Encoding for a bounded map BMAP. Format for a map with a bound of N and U in-use elements:
 * -------- U - int used elements encoded using BLen enums. N slots of <K,V> - N slots of an encoded
 * key followed by an encoded value. N bytes - An in-use array with each byte indicating if the
 * value at the corresponding slot is in-use (non-zero) or not-in-use (0). --------
 *
 * <p>The format corresponds to the C++ implementation of BMAP where a slot is the first location to
 * look for an key with a hash code that mods to the slot position. Since we cannot match C++ hash
 * codes with Java hash codes the Java write (serialize) of a value uses the same format but always
 * writes the used pairs in the first U slots. When the value is returned to C++ it must be
 * normalized to rehash the values and return them to the correct slots.
 */
@SuppressWarnings("unchecked")
final class MapBoundedEncoding extends MapEncoding {

  private final BoundInfo bound;
  private final int fixedLength;

  MapBoundedEncoding(ImplementationType kType, ImplementationType vType, BoundInfo bound) {
    super(kType, vType);

    this.bound = bound;

    if ((kEncoding.getSerializedSize() == VARIABLE_LENGTH)
        || (vEncoding.getSerializedSize() == VARIABLE_LENGTH)) {
      fixedLength = VARIABLE_LENGTH;
    } else {
      long boundedLength = bound.getBoundedLength();
      long totalLength = getBLen().getSerializedSize();
      totalLength += boundedLength; // inUse map
      totalLength += boundedLength * kEncoding.getSerializedSize();
      totalLength += boundedLength * vEncoding.getSerializedSize();
      if (totalLength > Integer.MAX_VALUE)
        // fixed length but too big to represent as our int fixed length
        fixedLength = VARIABLE_LENGTH;
      else fixedLength = (int) totalLength;
    }
  }

  BLen getBLen() {
    return bound.getSizeType();
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    if (fixedLength != VARIABLE_LENGTH) return fixedLength;

    long totalLength = getBLen().getSerializedSize();

    final int boundedLength = bound.getBoundedLength();

    // inUse map
    totalLength += boundedLength;

    final int kLength = kEncoding.getSerializedSize();
    if (kLength != VARIABLE_LENGTH) {
      totalLength += (boundedLength * (long) kLength);
    }
    final int vLength = vEncoding.getSerializedSize();
    if (vLength != VARIABLE_LENGTH) {
      totalLength += (boundedLength * (long) vLength);
    }

    // Fixed size
    if (kLength != VARIABLE_LENGTH && vLength != VARIABLE_LENGTH) return totalLength;

    Map<?, ?> map = (Map<?, ?>) value;

    // key only or both variable length
    if (kLength == VARIABLE_LENGTH) {
      for (Object k : map.keySet()) {
        totalLength += kEncoding.getMaxSerializedSize(k);
        if (vLength == VARIABLE_LENGTH) totalLength += vEncoding.getMaxSerializedSize(map.get(k));
      }

      totalLength +=
          CollectionEncoding.getUnusedSerializedSize(
              kType, kEncoding, (boundedLength - map.size()));

      if (vLength == VARIABLE_LENGTH)
        totalLength +=
            CollectionEncoding.getUnusedSerializedSize(
                vType, vEncoding, (boundedLength - map.size()));

      return totalLength;
    }

    // Value only is variable length
    for (Object v : map.values()) {
      totalLength += vEncoding.getMaxSerializedSize(v);
    }
    totalLength +=
        CollectionEncoding.getUnusedSerializedSize(vType, vEncoding, (boundedLength - map.size()));
    return totalLength;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {

    final int boundLength = bound.getBoundedLength();
    final int inUse = getBLen().getLength(bytes);
    if (inUse == 0) {
      // Need to skip getBound() items and the in-use map.
      for (int i = 0; i < boundLength; i++) {
        kEncoding.readValue(bytes);
        vEncoding.readValue(bytes);
      }
      // Skip the in-use array
      CollectionEncoding.skipInUseArray(bytes, bound);
      return Collections.EMPTY_MAP;
    }

    // Read all the elements
    Object[] keys = new Object[boundLength];
    Object[] values = new Object[boundLength];
    for (int i = 0; i < boundLength; i++) {
      keys[i] = kEncoding.readValue(bytes);
      values[i] = vEncoding.readValue(bytes);
    }

    // Read the in-use map
    Map<Object, Object> map = new HashMap<Object, Object>(inUse);
    for (int i = 0; i < boundLength; i++) {
      if (bytes.get() == 0) continue;
      map.put(kType.convertToCollectionValue(keys[i]), vType.convertToCollectionValue(values[i]));
    }

    return Collections.unmodifiableMap(map);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    Map<?, ?> map = (Map<?, ?>) value;
    super.writeValue(bytes, value);

    final int boundLength = bound.getBoundedLength();
    final int inUse = map.size();
    for (int i = 0; i < boundLength - inUse; i++) {
      kEncoding.writeValue(bytes, kType.defaultValue);
      vEncoding.writeValue(bytes, vType.defaultValue);
    }

    CollectionEncoding.writeJavaUsedMap(bytes, bound, inUse);
  }
}
