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
import java.util.List;
import java.util.Random;

/** Uses ListInt8.ByteList for efficiency. See ListUint8 for details. */
class ListUint16Bounded extends ListUintXXBounded {

  ListUint16Bounded(@SuppressWarnings("rawtypes") ImplementationType elementType, BoundInfo bound) {
    super(new ListUint16BoundedEncoding(bound), elementType, bound);
  }

  @Override
  List<?> _randomValue(Random rand) {
    return ListUint16._randomValue(rand, getMaxRandomLength());
  }

  @Override
  public Buffer getBuffer(Object value) {
    return ListUint16.convertToShortBuffer(value);
  }

  @Override
  boolean isImmutableList(List<?> list) {
    return list instanceof ShortList;
  }

  @Override
  List<?> newImmutableList(List<?> list) {
    return ListUint16.convertListToShortList(list);
  }
}
