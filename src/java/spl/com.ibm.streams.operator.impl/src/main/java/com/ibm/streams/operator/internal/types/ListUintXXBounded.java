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
import java.util.List;

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
 * lists which simply wraps a primitive array. and thus performs much better. However for backwards
 * compatibility and the fact a user can pass in any List the class still (through its encoding)
 * handles any List, e.g. tuples serialized prior to Streams 3.1 will contain ArrayList or any
 * List<?> supplied by the user.
 */
abstract class ListUintXXBounded extends ListBoundedSpl {

  ListUintXXBounded(
      TypeEncoding encoding,
      @SuppressWarnings("rawtypes") ImplementationType elementType,
      BoundInfo bound) {
    super(encoding, elementType, bound);
  }

  abstract boolean isImmutableList(List<?> list);

  abstract List<?> newImmutableList(List<?> list);

  /** Fast validation if the collection is a ByteList. */
  @Override
  final void validateCollection(List<?> collection) {
    if (isImmutableList(collection)) return;
    super.validateCollection(collection);
  }

  /** User view of the list. */
  @Override
  public final List<?> convertToList(List<?> value) {
    if (isImmutableList(value)) return value;
    return super.convertToList(value);
  }

  /** Copy into an immutable list if not already an immutable list. */
  @Override
  final List<?> safeCopy(List<?> collection) {
    if (isImmutableList(collection)) return collection;

    return newImmutableList(collection);
  }

  @Override
  final List<?> safeView(List<?> internalValue) {
    if (isImmutableList(internalValue)) return internalValue;
    return getReadOnly(internalValue);
  }
}
