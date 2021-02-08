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

import static com.ibm.streams.operator.internal.network.TupleSerialization.skip;

import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.internal.network.TupleSerialization.BLen;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.meta.BoundedType;
import com.ibm.streams.operator.meta.CollectionType;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Random;

/**
 * Format for List of booleans type. Number of items (N) as a four byte unsigned value followed by N
 * booleans (as bytes).
 */
final class ListBoolean extends ImplementationType<boolean[]>
    implements CollectionType, BoundedType {

  private final ImplementationType<?> elementType;
  private final TypeEncoding encoding;
  private final BoundInfo bound;
  private final String splName;

  private static final boolean[] EMPTY = new boolean[0];

  ListBoolean(ImplementationType<?> elementType) {
    this(MetaType.LIST, elementType, BoundInfo.UNBOUNDED);
  }

  ListBoolean(ImplementationType<?> elementType, int boundedLength) {
    this(MetaType.BLIST, elementType, new BoundInfo(boundedLength));
  }

  ListBoolean(MetaType metaType, ImplementationType<?> elementType, BoundInfo bound) {
    super(metaType, EMPTY);
    this.elementType = elementType;
    this.splName = bound.getSplName("list<" + elementType.getLanguageType() + ">");
    this.bound = bound;
    this.encoding =
        metaType == MetaType.LIST ? new BooleanArrayEncoding() : new BoundedBooleanArrayEncoding();
  }

  @Override
  public final int hashCode() {
    return super.hashCode() + (17 * getElementType().hashCode()) + (31 * bound.hashCode());
  }

  @Override
  public final boolean equals(Object other) {
    if (!super.equals(other)) return false;

    CollectionType ct = (CollectionType) other;
    if (!getElementType().equals(ct.getElementType())) return false;

    return bound.equals(((ListBoolean) other).bound);
  }

  @Override
  public TypeEncoding getEncoding() {
    return encoding;
  }

  @Override
  public Class<?> getAsCompositeElementType() {
    return List.class;
  }

  @Override
  public Type getElementType() {
    return elementType;
  }

  @Override
  public String getLanguageType() {
    return splName;
  }

  @Override
  public final int getBoundedSize() {
    return bound.getBoundedLength();
  }

  @Override
  boolean[] _randomValue(Random rand) {
    final boolean[] array =
        new boolean[rand.nextInt(getMetaType() == MetaType.LIST ? 20 : bound.getBoundedLength())];
    for (int i = 0; i < array.length; i++) {
      array[i] = rand.nextBoolean();
    }
    return array;
  }

  @Override
  public boolean isImmutable() {
    return false;
  }

  @Override
  boolean[] safeCopy(boolean[] value) {
    return value.clone();
  }

  /** Since we return an array any view of it must have a copy. */
  @Override
  final boolean[] safeView(boolean[] value) {
    return value.clone();
  }

  @Override
  public String getString(Object value) {
    if (value instanceof List) return value.toString();
    return Arrays.toString((boolean[]) value);
  }

  @Override
  public final List<?> convertToList(final boolean[] value) {
    if (value.length == 0) return Collections.emptyList();

    final List<Boolean> list = new ArrayList<Boolean>(value.length);
    for (boolean b : value) list.add(b);
    return Collections.unmodifiableList(list);
  }

  @Override
  public final boolean[] convertFrom(final List<?> list) {
    if (list.isEmpty()) return EMPTY;

    // Bounded lists drop elements.
    int copyCount = bound.copyCount(list.size());

    try {
      final boolean[] array = new boolean[copyCount];
      for (int i = 0; i < array.length; i++) array[i] = (Boolean) list.get(i);
      return array;
    } catch (NullPointerException e) {
      throw (RuntimeException) invalidConversion("java.util.List").initCause(e);
    } catch (ClassCastException e) {
      throw (RuntimeException) invalidConversion("java.util.List").initCause(e);
    }
  }

  @Override
  public final boolean[] convertFromObject(final Object value) {
    return convertFromObjectCopy(value);
  }

  @Override
  public final boolean[] convertFromObjectCopy(final Object value) {
    boolean[] bv = super.convertFromObject(value);
    int copyCount = bound.copyCount(bv.length);
    if (copyCount != bv.length) {
      boolean[] subset = new boolean[copyCount];
      System.arraycopy(bv, 0, subset, 0, copyCount);
      return subset;
    }
    return safeCopy(bv);
  }

  /**
   * Validate this type when it is an element in a collection type (Set, Map, List). This element
   * must be a List in a collection.
   *
   * @param element
   * @return
   */
  protected final Object validateAsCollectionElement(Object element) {
    return ListSpl.validateAsCollectionElementList(elementType, element);
  }

  @Override
  protected final Object convertToCollectionValue(boolean[] value) {
    return convertToList(value);
  }

  @Override
  protected boolean[] convertFromCollectionValue(Object value) {
    return convertFrom((List<?>) value);
  }

  @SuppressWarnings("unchecked")
  @Override
  Object safeCollectionCopy(Object list) {
    return new ArrayList<Boolean>((List<Boolean>) list);
  }

  class BooleanArrayEncoding implements TypeEncoding {
    private final BLen blen;

    BooleanArrayEncoding() {
      this.blen = bound.getSizeType();
    }

    @Override
    public Object readValue(final ByteBuffer bytes) {
      return readList(bytes);
    }

    boolean[] readList(final ByteBuffer bytes) {

      int n = blen.getLength(bytes);
      boolean[] value = new boolean[n];
      for (int i = 0; i < n; i++) value[i] = BooleanSpl.readBoolean(bytes);
      return value;
    }

    @Override
    public long getMaxSerializedSize(Object value) {
      long maxLength = blen.getSerializedSize();
      if (value instanceof Collection) maxLength += ((Collection<?>) value).size();
      else maxLength += ((boolean[]) value).length;
      return maxLength;
    }

    @Override
    public final void writeValue(final ByteBuffer bytes, final Object value) {
      final boolean[] array = (boolean[]) value;
      writeList(bytes, array);
    }

    void writeList(ByteBuffer bytes, boolean[] array) {
      blen.putLength(bytes, array.length);
      for (boolean b : array) BooleanSpl.writeBoolean(bytes, b);
    }

    @Override
    public int getSerializedSize() {
      return VARIABLE_LENGTH;
    }
  }

  class BoundedBooleanArrayEncoding extends BooleanArrayEncoding {

    private int fixedSize;

    BoundedBooleanArrayEncoding() {
      fixedSize = bound.getBoundedLength() + bound.getSizeType().getSerializedSize();
    }

    @Override
    public int getSerializedSize() {
      return fixedSize;
    }

    @Override
    public long getMaxSerializedSize(Object value) {
      return getSerializedSize();
    }

    @Override
    void writeList(ByteBuffer bytes, boolean[] array) {
      super.writeList(bytes, array);
      skip(bytes, bound.getBoundedLength() - array.length);
    }

    @Override
    boolean[] readList(ByteBuffer bytes) {
      boolean[] array = super.readList(bytes);
      skip(bytes, bound.getBoundedLength() - array.length);
      return array;
    }
  }
}
