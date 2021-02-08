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
import java.util.Set;

/**
 * Encoding for a bounded set DSET. Format for a set with a bound of N and U in-use elements:
 * -------- U - int used elements encoded using BLen enums. N slots of E - N slots of an encoded
 * element. N bytes - An in-use array with each byte indicating if the value at the corresponding
 * slot is in-use (non-zero) or not-in-use (0). --------
 *
 * <p>The format corresponds to the C++ implementation of BSET where a slot is the first location to
 * look for an element with a hash code that mods to the slot position. Since we cannot match C++
 * hash codes with Java hash codes the Java write (serialize) of a value uses the same format but
 * always writes the used elements in the first U slots. When the value is returned to C++ it must
 * be normalized to rehash the values and return them to the correct slots.
 */
@SuppressWarnings("unchecked")
public class SetBoundedEncoding extends SetEncoding {

  private final BoundInfo bound;
  private final int fixedLength;

  SetBoundedEncoding(ImplementationType elementType, BoundInfo bound) {
    super(elementType);
    this.bound = bound;
    this.fixedLength = calculateFixedLength(bound, true);
  }

  BLen getBLen() {
    return bound.getSizeType();
  }

  @Override
  public int getSerializedSize() {
    return fixedLength;
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    long totalLength = getBoundValueSize((Set) value, bound);

    // Add in the bytes for the used map
    return totalLength + bound.getBoundedLength();
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    int inUse = writeBoundValue(bytes, (Set) value, bound);
    writeJavaUsedMap(bytes, bound, inUse);
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return readBoundSet(bytes);
  }

  private Set readBoundSet(ByteBuffer bytes) {

    final int inUse = readBoundLength(bytes, bound);
    if (inUse == 0) {
      skipInUseArray(bytes, bound);
      return emptyCollection();
    }

    // Read all the elements
    Object[] all = new Object[bound.getBoundedLength()];
    for (int i = 0; i < all.length; i++) {
      all[i] = elementEncoding.readValue(bytes);
    }

    // Read the in-use map
    Set value = getCollection(inUse);
    for (int i = 0; i < all.length; i++) {
      if (bytes.get() == 0) continue;
      value.add(elementType.convertToCollectionValue(all[i]));
    }

    return getReadOnly(value);
  }
}
