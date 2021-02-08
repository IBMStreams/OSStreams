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
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Meta-type for SPL lists that are not special cased as arrays of primitives or Strings. The
 * attributes's value is maintained as a read-only java.util.List object. Format is standard list
 * format of 32-bit signed element count N followed by N elements.
 */
class ListSpl extends CollectionSpl<List<?>> {

  static ImplementationType list(ImplementationType elementType) {
    switch (elementType.getMetaType()) {
      case UINT8:
        return new ListUint8(elementType);
      case UINT16:
        return new ListUint16(elementType);
      case UINT32:
        return new ListUint32(elementType);
      case UINT64:
        return new ListUint64(elementType);
    }
    return new ListSpl(MetaType.LIST, new ListEncoding(elementType), elementType);
  }

  static ImplementationType boundList(ImplementationType elementType, int boundedLength) {

    // See if this is a "primitive" list type.
    switch (elementType.getMetaType()) {
      case BOOLEAN:
        return new ListBoolean(elementType, boundedLength);
      case INT8:
        return new ListInt8(elementType, boundedLength);
      case INT16:
        return new ListInt16(elementType, boundedLength);
      case INT32:
        return new ListInt32(elementType, boundedLength);
      case INT64:
        return new ListInt64(elementType, boundedLength);
      case FLOAT32:
        return new ListFloat32(elementType, boundedLength);
      case FLOAT64:
        return new ListFloat64(elementType, boundedLength);
      case USTRING:
        return new ListUstring(elementType, boundedLength);
    }

    BoundInfo bound = new BoundInfo(boundedLength);
    switch (elementType.getMetaType()) {
      case UINT8:
        return new ListUint8Bounded(elementType, bound);
      case UINT16:
        return new ListUint16Bounded(elementType, bound);
      case UINT32:
        return new ListUint32Bounded(elementType, bound);
      case UINT64:
        return new ListUint64Bounded(elementType, bound);
    }
    return new ListBoundedSpl(new ListBoundedEncoding(elementType, bound), elementType, bound);
  }

  ListSpl(MetaType metaType, TypeEncoding listEncoding, ImplementationType elementType) {
    super(metaType, listEncoding, elementType, Collections.EMPTY_LIST);
  }

  /** Objects are returned as instances of the List interface. */
  public Class<?> getObjectType() {
    return List.class;
  }

  @Override
  public List<?> convertFromObject(Object value) {
    if (value instanceof List) return convertFrom((List<?>) value);
    throw invalidConversion(value.getClass().getName());
  }

  @Override
  protected List<?> newCollection(int size) {
    return new ArrayList<Object>(size);
  }

  @Override
  protected List<Object> getReadOnly(List<?> collection) {
    return Collections.unmodifiableList(collection);
  }

  /**
   * Return a read-only copy. The underlying values will be immutable or private to the tuple for a
   * Tuple. For an OutputTuple this returns an object that reflects the content that the user
   * originally supplied, thus it can change if the original is changed.
   */
  @Override
  public List<?> convertToList(List<?> value) {
    return Collections.unmodifiableList(value);
  }

  /**
   * Validate this type when it is contained in another collection (list, set, map). For a list the
   * type must be a List even though lists of Java primitives are returned as arrays.
   *
   * @return
   */
  Object validateAsCollectionElement(Object element) {

    List<?> list = (List<?>) element;
    validateCollection(list);
    return list;
  }

  /** Accept a List as a value. The incoming list is validated and copied once validated. */
  @Override
  public List<?> convertFrom(final List<?> list) {
    try {

      validateCollection(list);
      return list;

    } catch (UnsupportedOperationException e) {
      throw (RuntimeException) invalidConversion("java.util.List").initCause(e);
    } catch (ClassCastException e) {
      throw (RuntimeException) invalidConversion("java.util.List").initCause(e);
    } catch (NullPointerException e) {
      throw (RuntimeException) invalidConversion("java.util.List").initCause(e);
    }
  }

  @SuppressWarnings({"rawtypes", "unchecked"})
  protected static final Object validateAsCollectionElementList(
      ImplementationType<?> elementType, Object element) {
    List<?> list = (List<?>) element;
    if (list.isEmpty()) return Collections.emptyList();
    List validatedList = new ArrayList<Boolean>(list.size());
    for (Object myElement : list) validatedList.add(elementType.convertFromObject(myElement));
    return Collections.unmodifiableList(validatedList);
  }
}
