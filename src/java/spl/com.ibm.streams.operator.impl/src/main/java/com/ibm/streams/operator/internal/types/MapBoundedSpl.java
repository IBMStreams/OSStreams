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
import java.util.Map;

public class MapBoundedSpl extends MapSpl implements BoundedType {

  private final BoundInfo bound;

  MapBoundedSpl(ImplementationType<?> keyType, ImplementationType<?> valueType, BoundInfo bound) {
    super(MetaType.BMAP, keyType, valueType, new MapBoundedEncoding(keyType, valueType, bound));
    this.bound = bound;
  }

  @Override
  public int hashCode() {
    return super.hashCode() + (149 * getBoundedSize());
  }

  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    BoundedType bt = (BoundedType) other;
    return getBoundedSize() == bt.getBoundedSize();
  }

  @Override
  public int getBoundedSize() {
    return bound.getBoundedLength();
  }

  @Override
  public String getLanguageType() {
    return bound.getSplName(super.getLanguageType());
  }

  @Override
  int getMaxRandomLength() {
    return getBoundedSize();
  }

  /**
   * If the size of the incoming map exceeds the bound then create a subset of the map. Note that
   * the dropped elements are arbitrary since order is not defined for a map.
   */
  @SuppressWarnings("unchecked")
  @Override
  public Map convertFrom(Map<?, ?> map) {
    final int copyCount = bound.copyCount(map.size());
    if (copyCount != map.size()) {
      Map subset = newMap(copyCount);
      for (Object key : map.keySet()) {
        subset.put(key, map.get(key));
        if (subset.size() == copyCount) break;
      }
      map = subset;
    }
    return super.convertFrom(map);
  }
}
