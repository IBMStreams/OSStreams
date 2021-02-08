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
import com.ibm.streams.operator.meta.CollectionType;
import java.math.BigDecimal;
import java.util.Collection;
import java.util.Random;
import java.util.Set;

/**
 * Generic SPL list & set types that are not special cased as lists of primitive types or lists of
 * Strings. Value is maintained as a read-only Collection object when read from a serialized tuple.
 * Format is standard list format of 32-bit signed element count N followed by N elements. All the
 * serialization related methods implement the non-bounded collection serialization. Sub-classes
 * that are bounded types have serialization methods that utilize the bound serialization methods
 * provided here.
 */
@SuppressWarnings("unchecked")
abstract class CollectionSpl<T extends Collection> extends UtilCollectionsSpl<T>
    implements CollectionType {

  private final ImplementationType elementType;
  private final String splName;
  private final TypeEncoding collectionEncoding;

  CollectionSpl(
      MetaType metaType,
      TypeEncoding collectionEncoding,
      ImplementationType elementType,
      T defaultValue) {
    super(metaType, defaultValue);
    this.elementType = elementType;
    this.splName = (metaType.isList() ? "list<" : "set<") + elementType.getLanguageType() + ">";
    this.collectionEncoding = collectionEncoding;
  }

  @Override
  public int hashCode() {
    return super.hashCode() + (17 * getElementType().hashCode());
  }

  /** Non-bounded types are equal if they have the same element type. */
  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    CollectionType ct = (CollectionType) other;
    return getElementType().equals(ct.getElementType());
  }

  @Override
  public String getLanguageType() {
    return splName;
  }

  @Override
  public final Type getElementType() {
    return elementType;
  }

  @Override
  public final TypeEncoding getEncoding() {
    return collectionEncoding;
  }

  @Override
  public boolean isImmutable() {
    return false;
  }

  /**
   * Not only must the collection be safe objects so that modification of the it cannot change the
   * value of a Tuple but in additional each element must also be.
   */
  @Override
  T safeCopy(T collection) {
    if (collection.isEmpty()) return defaultValue;
    if (elementType.isImmutable()) {
      // This is a collection of immutable objects,
      // so just need to make it read only.
      T copyCollection = newCollection(collection.size());
      copyCollection.addAll(collection);
      collection = copyCollection;
    } else {
      // This is a collection of mutable (composite) objects.
      T safe = newCollection(collection.size());
      for (Object element : collection) {
        safe.add(elementType.safeCollectionCopy(element));
      }
      collection = safe;
    }
    return getReadOnly(collection);
  }

  T safeView(T internalValue) {
    return getReadOnly(internalValue);
  }

  /**
   * Generate a collection with zero or more values. If the type is a set of decimal types then
   * don't include values that are equivalent but are not equal according to equals().
   */
  @Override
  T _randomValue(Random rand) {
    int n = rand.nextInt(getMaxRandomLength());
    T c = newCollection(n);
    outer:
    for (int i = 0; i < n; i++) {
      final Object value = elementType.convertToCollectionValue(elementType.randomValue(rand));
      switch (elementType.getValueMetaType()) {
        case DECIMAL32:
        case DECIMAL64:
        case DECIMAL128:
          BigDecimal dv = (BigDecimal) value;
          if (!c.isEmpty() && c instanceof Set) {
            for (BigDecimal v : (Set<BigDecimal>) c) {
              if (v == null || dv == null)
                continue; // if both are null, allow overwrite of existing entry
              if (v.compareTo(dv) == 0) continue outer;
            }
          }
          break;
        default:
          break;
      }
      c.add(value);
    }
    return c;
  }

  abstract T newCollection(int size);

  abstract T getReadOnly(T collection);

  /**
   * Validate a collection by ensuring every element type is an instance of the element type as
   * required by the element type.
   *
   * <p>For a bounded list the passed in collection will already have been truncated if the source
   * list was too big.
   */
  void validateCollection(final T collection) {
    if (collection.isEmpty()) return;

    for (Object o : collection) {
      if (o == null && !elementType.getMetaType().isOptional()) throw new NullPointerException();

      elementType.validateAsCollectionElement(o);
    }
  }

  /**
   * Matches the style of format described by AbsractCollection but we use the natural
   * representation of each value rather than its toString.
   */
  @Override
  public final String getString(final Object value) {
    final Collection<?> collection = (Collection<?>) value;
    boolean seenValue = false;
    StringBuilder sb = new StringBuilder();
    sb.append('[');
    for (Object element : collection) {
      if (seenValue) sb.append(", ");
      sb.append(elementType.getString(element));
      seenValue = true;
    }
    sb.append(']');

    return sb.toString();
  }
}
