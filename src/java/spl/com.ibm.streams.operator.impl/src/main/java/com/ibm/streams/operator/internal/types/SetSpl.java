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

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

/**
 * Meta-type for SPL sets . The attributes's value is maintained as a read-only java.util.Set
 * object. Format is standard set format of 32-bit signed element count N followed by N elements.
 */
class SetSpl extends CollectionSpl<Set<?>> {

  static ImplementationType set(ImplementationType<?> elementType) {
    return new SetSpl(MetaType.SET, new SetEncoding(elementType), elementType);
  }

  static ImplementationType boundSet(ImplementationType<?> elementType, int boundedLength) {
    BoundInfo bound = new BoundInfo(boundedLength);
    return new SetBoundedSpl(new SetBoundedEncoding(elementType, bound), elementType, bound);
  }

  SetSpl(MetaType type, SetEncoding setEncoding, ImplementationType<?> elementType) {
    super(type, setEncoding, elementType, Collections.EMPTY_SET);
  }

  /** Objects are returned as instances of the Set interface. */
  public Class<?> getObjectType() {
    return Set.class;
  }

  /**
   * Accept a Set as a value. The incoming set is validated and accepted without modification once
   * validated.
   */
  @Override
  public Set<?> convertFrom(final Set<?> set) {
    try {

      validateCollection(set);
      return set;

    } catch (UnsupportedOperationException e) {
      throw (RuntimeException) invalidConversion("java.util.Set").initCause(e);
    } catch (ClassCastException e) {
      throw (RuntimeException) invalidConversion("java.util.Set").initCause(e);
    } catch (NullPointerException e) {
      throw (RuntimeException) invalidConversion("java.util.Set").initCause(e);
    }
  }

  @Override
  public Set<?> convertFromObject(Object value) {
    if (value instanceof Set) return convertFrom((Set<?>) value);
    throw invalidConversion(value.getClass().getName());
  }

  @Override
  Set<?> newCollection(int size) {
    return new HashSet<Object>(size);
  }

  @Override
  Set<?> getReadOnly(Set<?> collection) {
    return Collections.unmodifiableSet(collection);
  }

  /**
   * Return a read-only copy. The underlying values will be immutable or private to the tuple for a
   * Tuple. For an OutputTuple this returns an object that reflects the content that the user
   * originally supplied, thus it can change if the original is changed.
   */
  @Override
  Set<?> convertToSet(Set<?> value) {
    return Collections.unmodifiableSet(value);
  }
}
