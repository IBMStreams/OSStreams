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

import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.meta.MapType;
import java.math.BigDecimal;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

@SuppressWarnings("unchecked")
class MapSpl extends UtilCollectionsSpl<Map> implements MapType {

  private final ImplementationType kType;
  private final ImplementationType vType;
  private final TypeEncoding mapEncoding;
  private final String splName;

  static ImplementationType map(ImplementationType keyType, ImplementationType valueType) {
    return new MapSpl(MetaType.MAP, keyType, valueType, new MapEncoding(keyType, valueType));
  }

  static ImplementationType boundMap(
      ImplementationType keyType, ImplementationType valueType, int boundedLength) {

    BoundInfo bound = new BoundInfo(boundedLength);
    return new MapBoundedSpl(keyType, valueType, bound);
  }

  MapSpl(
      MetaType metaType,
      ImplementationType keyType,
      ImplementationType valueType,
      TypeEncoding mapEncoding) {
    super(metaType, Collections.EMPTY_MAP);
    this.kType = keyType;
    this.vType = valueType;
    this.splName = "map<" + keyType.getLanguageType() + "," + valueType.getLanguageType() + ">";
    this.mapEncoding = mapEncoding;
  }

  @Override
  public int hashCode() {
    return super.hashCode() + (17 * getKeyType().hashCode()) + (59 * getValueType().hashCode());
  }

  /** Non-bounded types are equal if they have the same element type. */
  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    MapType mt = (MapType) other;
    return getKeyType().equals(mt.getKeyType()) && getValueType().equals(mt.getValueType());
  }

  @Override
  public final TypeEncoding getEncoding() {
    return mapEncoding;
  }

  @Override
  public String getLanguageType() {
    return splName;
  }

  /** Objects are returned as instances of the Map interface. */
  public Class<?> getObjectType() {
    return Map.class;
  }

  @Override
  final Map _randomValue(Random rand) {
    int n = rand.nextInt(getMaxRandomLength());
    Map map = newMap(n);
    outer:
    for (int i = 0; i < n; i++) {
      final Object key = kType.convertToCollectionValue(kType._randomValue(rand));
      switch (kType.getValueMetaType()) {
        case DECIMAL32:
        case DECIMAL64:
        case DECIMAL128:
          if (!map.isEmpty()) {
            BigDecimal dk = (BigDecimal) key;
            for (BigDecimal k : ((Map<BigDecimal, ?>) map).keySet()) {
              if (dk == null || k == null)
                continue; // if both are null, allow overwrite of existing entry
              if (dk.compareTo(k) == 0) continue outer;
            }
            // continue;
          }
          break;
        default:
          break;
      }
      map.put(key, vType.convertToCollectionValue(vType._randomValue(rand)));
    }
    return map;
  }

  final Map newMap(int capacity) {
    return new HashMap(capacity);
  }

  @Override
  public Map convertFromObject(Object value) {
    if (value instanceof Map) return convertFrom((Map<?, ?>) value);
    throw invalidConversion(value.getClass().getName());
  }

  @Override
  public boolean isImmutable() {
    return false;
  }

  @Override
  Map safeCopy(Map map) {
    if (map.isEmpty()) return defaultValue;

    boolean immutableK = kType.isImmutable();
    boolean immutableV = vType.isImmutable();
    if (immutableK && immutableV) return Collections.unmodifiableMap(map);

    // This is a map with mutable (composite) objects.
    Map safe = newMap(map.size());
    for (Object k : map.keySet()) {
      Object v = map.get(k);
      if (!immutableK) k = kType.safeCollectionCopy(k);
      if (!immutableV) v = vType.safeCollectionCopy(v);
      safe.put(k, v);
    }
    return Collections.unmodifiableMap(safe);
  }

  @Override
  public Type getElementType() {
    return getValueType();
  }

  @Override
  public Type getKeyType() {
    return kType;
  }

  @Override
  public Type getValueType() {
    return vType;
  }

  /**
   * Return a read-only copy. The underlying values will be immutable or private to the tuple for a
   * Tuple. For an OutputTuple this returns an object that reflects the content that the user
   * originally supplied, thus it can change if the original is changed.
   */
  @Override
  Map convertToMap(Map value) {
    return Collections.unmodifiableMap(value);
  }

  /**
   * Accept a Map as a value. The incoming value is validated and accepted without modification once
   * validated.
   */
  @Override
  public Map convertFrom(final Map<?, ?> map) {
    if (map.isEmpty()) return defaultValue;

    Map validatedMap = newMap(map.size());

    try {

      for (Object k : map.keySet()) {
        if (k == null && !kType.getMetaType().isOptional()) throw new NullPointerException();
        Object vk = kType.validateAsCollectionElement(k);
        Object v = map.get(k);
        if (v == null && !vType.getMetaType().isOptional()) throw new NullPointerException();
        Object vv = vType.validateAsCollectionElement(v);
        validatedMap.put(vk, vv);
      }
      return validatedMap;

    } catch (UnsupportedOperationException e) {
      throw (RuntimeException) invalidConversion("java.util.Map").initCause(e);
    } catch (ClassCastException e) {
      throw (RuntimeException) invalidConversion("java.util.Map").initCause(e);
    } catch (NullPointerException e) {
      throw (RuntimeException) invalidConversion("java.util.Map").initCause(e);
    }
  }
}
