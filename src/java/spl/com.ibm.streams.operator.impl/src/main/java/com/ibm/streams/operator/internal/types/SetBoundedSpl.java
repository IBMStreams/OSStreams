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

import com.ibm.streams.operator.meta.BoundedType;
import java.util.Set;

/** Bounded set (set<T>[N]). */
class SetBoundedSpl extends SetSpl implements BoundedType {

  private final BoundInfo bound;

  SetBoundedSpl(
      SetBoundedEncoding setEncoding, ImplementationType<?> elementType, BoundInfo bound) {
    super(MetaType.BSET, setEncoding, elementType);
    this.bound = bound;
  }

  @Override
  public int hashCode() {
    return super.hashCode() + (31 * getBoundedSize());
  }

  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    return bound.equals(((SetBoundedSpl) other).bound);
  }

  @Override
  int getMaxRandomLength() {
    return getBoundedSize();
  }
  /**
   * If the size of the incoming set exceeds the bound then create a subset of the set. Note that
   * the dropped elements are arbitrary since order is not defined for a set.
   */
  @SuppressWarnings("unchecked")
  public Set<?> convertFrom(Set<?> set) {
    final int copyCount = bound.copyCount(set.size());
    if (copyCount != set.size()) {
      Set subset = newCollection(copyCount);

      for (Object e : set) {
        subset.add(e);
        if (subset.size() == copyCount) break;
      }
      set = subset;
    }
    return super.convertFrom(set);
  }

  /**
   * Cannot re-use the super-class non-bounded code as when we read the elements we need to keep
   * their position so we can know if they are used or not once the in-use map is read. The
   * non-bounded code directly adds the elements into the set and hence loses positional
   * information. We need to read into an array and then see which are required.
   */
  @Override
  public int getBoundedSize() {
    return bound.getBoundedLength();
  }

  @Override
  public String getLanguageType() {
    return bound.getSplName(super.getLanguageType());
  }
}
