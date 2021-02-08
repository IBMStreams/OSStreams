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

import com.ibm.streams.operator.internal.types.ListInt8.ByteList;
import java.nio.Buffer;
import java.nio.ByteBuffer;
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
 * <p>This class (Streams 3.1, RTC 16549) changes the default internal type to be the immutable
 * ListInt8.ByteList which simply wraps a byte[] and thus performs much better. However for
 * backwards compatibility and the fact a user can pass in any List<Byte> the class still (through
 * ListUint8Encoding) handles any List<Byte>, e.g. tuples serialized prior to Streams 3.1 will
 * contain ArrayList or any List<Byte> supplied by the user.
 */
class ListUint8 extends ListUintXX {

  ListUint8(@SuppressWarnings("rawtypes") ImplementationType elementType) {
    super(new ListUint8Encoding(), elementType);
  }

  @Override
  List<?> _randomValue(Random rand) {
    return _randomValue(rand, getMaxRandomLength());
  }

  /**
   * This random matches the previous behaviour (Streams 2.0-Streams 3.0) but uses ByteList. While
   * rand.nextBytes() might be more efficient it changes the behaviour if a previous client was
   * using a fixed seed for testing.
   */
  static ByteList _randomValue(Random rand, int maxLength) {
    int n = rand.nextInt(maxLength);
    final byte[] data = new byte[n];
    for (int i = 0; i < data.length; i++) data[i] = (byte) rand.nextInt();
    return new ByteList(data);
  }

  @Override
  boolean isImmutableList(List<?> list) {
    return list instanceof ByteList;
  }

  @Override
  List<?> newImmutableList(List<?> list) {
    return convertListToByteList(list);
  }

  public Buffer getBuffer(Object value) {
    return convertToByteBuffer(value);
  }

  static List<Byte> convertListToByteList(List<?> list) {
    return new ByteList(convertListToByteArray(list));
  }

  static byte[] convertListToByteArray(Object value) {
    @SuppressWarnings("unchecked")
    List<Byte> list = (List<Byte>) value;
    byte[] data = new byte[list.size()];
    for (int i = 0; i < data.length; i++) data[i] = list.get(i);
    return data;
  }

  static ByteBuffer convertToByteBuffer(Object value) {
    if (value instanceof ByteList) return ((ByteList) value).asByteBuffer();

    return ByteBuffer.wrap(convertListToByteArray(value)).asReadOnlyBuffer();
  }
}
