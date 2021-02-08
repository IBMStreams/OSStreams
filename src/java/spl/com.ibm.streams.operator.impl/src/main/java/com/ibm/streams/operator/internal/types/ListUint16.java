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

import com.ibm.streams.operator.internal.types.ListInt16.ShortList;
import java.nio.Buffer;
import java.nio.ShortBuffer;
import java.util.List;
import java.util.Random;

/**
 * When lists were introduced in Streams 2.0 the generic approach was to make their object type be
 * List, and this is supported by ListSpl. Specific exclusions for list of primitive and string
 * types that existed in Streams 1.2 (SPADE) were made so their object type continued to be a
 * primitive array (e.g. int[]) or String[].
 *
 * <p>However, performance testing by the SPL performance team typically uses list<uint8> to
 * represent an arbitrary list. testing showed these are much slower that list<int8> as the internal
 * representation ends up being a List<Byte> or specifically an ArrayList<Byte>.
 *
 * <p>This class (Streams 3.1, RTC 16549) changes the default internal type to be ListInt8.ByteList
 * which simply wraps a byte[] and thus performs much better. However for backwards compatibility
 * and the fact a user can pass in any List<Byte> the class still (through ListUint8Encoding)
 * handles any List<Byte>, e.g. tuples serialized prior to Streams 3.1 will contain ArrayList or any
 * List<Byte> supplied by the user.
 */
class ListUint16 extends ListUintXX {

  ListUint16(@SuppressWarnings("rawtypes") ImplementationType elementType) {
    super(new ListUint16Encoding(), elementType);
  }

  @Override
  List<?> _randomValue(Random rand) {
    return _randomValue(rand, getMaxRandomLength());
  }

  /** This random matches the previous behaviour (Streams 2.0-Streams 3.0) but uses ShortList. */
  static ShortList _randomValue(Random rand, int maxLength) {
    int n = rand.nextInt(maxLength);
    short[] data = new short[n];
    for (int i = 0; i < data.length; i++) data[i] = (short) rand.nextInt();
    return new ShortList(data);
  }

  @Override
  public Buffer getBuffer(Object value) {
    return convertToShortBuffer(value);
  }

  @Override
  boolean isImmutableList(List<?> list) {
    return list instanceof ShortList;
  }

  @Override
  List<?> newImmutableList(List<?> list) {
    return convertListToShortList(list);
  }

  static List<Short> convertListToShortList(List<?> list) {
    return new ShortList(convertListToShortArray(list));
  }

  static short[] convertListToShortArray(Object value) {
    @SuppressWarnings("unchecked")
    final List<Short> list = (List<Short>) value;
    final short[] data = new short[list.size()];
    for (int i = 0; i < data.length; i++) data[i] = list.get(i);
    return data;
  }

  static ShortBuffer convertToShortBuffer(Object value) {
    if (value instanceof ShortList) return ((ShortList) value).asShortBuffer();

    return ShortBuffer.wrap(convertListToShortArray(value)).asReadOnlyBuffer();
  }
}
