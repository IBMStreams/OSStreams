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

import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.meta.BoundedType;
import java.util.List;

/** Bounded set (set<T>[N]). */
class ListBoundedSpl extends ListSpl implements BoundedType {

  private final BoundInfo bound;

  ListBoundedSpl(TypeEncoding encoding, ImplementationType<?> elementType, BoundInfo bound) {
    super(MetaType.BLIST, encoding, elementType);
    this.bound = bound;
  }

  @Override
  public int hashCode() {
    return super.hashCode() + (31 * getBoundedSize());
  }

  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    return bound.equals(((ListBoundedSpl) other).bound);
  }

  @Override
  int getMaxRandomLength() {
    return getBoundedSize();
  }

  @Override
  public List<?> convertFrom(List<?> list) {
    int copyCount = bound.copyCount(list.size());
    if (copyCount != list.size()) {
      list = list.subList(0, copyCount);
    }
    return super.convertFrom(list);
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
