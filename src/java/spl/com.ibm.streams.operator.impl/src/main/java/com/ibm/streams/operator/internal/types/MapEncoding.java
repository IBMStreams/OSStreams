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
import com.ibm.streams.operator.internal.network.TypeEncoding;
import java.nio.ByteBuffer;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

@SuppressWarnings("unchecked")
class MapEncoding implements TypeEncoding {

  final ImplementationType kType;
  final ImplementationType vType;
  TypeEncoding kEncoding;
  TypeEncoding vEncoding;

  MapEncoding(ImplementationType kType, ImplementationType vType) {
    this.kType = kType;
    this.vType = vType;
    this.kEncoding = kType.getEncoding();
    this.vEncoding = vType.getEncoding();
  }

  BLen getBLen() {
    return BLen.UNBOUNDED;
  }

  @Override
  public int getSerializedSize() {
    return VARIABLE_LENGTH;
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    Map<?, ?> map = (Map<?, ?>) value;
    long totalLength = getBLen().getSerializedSize();

    if (map.isEmpty()) return totalLength;

    final int kLength = kEncoding.getSerializedSize();
    if (kLength != VARIABLE_LENGTH) {
      totalLength += (map.size() * kLength);
    }
    final int vLength = vEncoding.getSerializedSize();
    if (vLength != VARIABLE_LENGTH) {
      totalLength += (map.size() * vLength);
    }

    // Fixed size
    if (kLength != VARIABLE_LENGTH && vLength != VARIABLE_LENGTH) return totalLength;

    // key only or both variable length
    if (kLength == VARIABLE_LENGTH) {
      for (Object k : map.keySet()) {
        totalLength += kEncoding.getMaxSerializedSize(k);
        if (vLength == VARIABLE_LENGTH) totalLength += vEncoding.getMaxSerializedSize(map.get(k));
      }
      return totalLength;
    }

    // Value only is variable length
    for (Object v : map.values()) {
      totalLength += vEncoding.getMaxSerializedSize(v);
    }
    return totalLength;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    final int n = getBLen().getLength(bytes);
    if (n == 0) return Collections.EMPTY_MAP;

    Map<Object, Object> map = new HashMap<Object, Object>(n);
    for (int i = 0; i < n; i++) {
      Object key = kType.convertToCollectionValue(kEncoding.readValue(bytes));

      Object value = vType.convertToCollectionValue(vEncoding.readValue(bytes));

      map.put(key, value);
    }
    return Collections.unmodifiableMap(map);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    Map<?, ?> map = (Map<?, ?>) value;
    getBLen().putLength(bytes, map.size());
    for (Object key : map.keySet()) {
      kEncoding.writeValue(bytes, kType.convertFromCollectionValue(key));
      Object mv = map.get(key);
      vEncoding.writeValue(bytes, vType.convertFromCollectionValue(mv));
    }
  }
}
