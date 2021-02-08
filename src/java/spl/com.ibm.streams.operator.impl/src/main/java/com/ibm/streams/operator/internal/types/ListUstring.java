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
import java.util.Collections;
import java.util.List;
import java.util.Random;

/**
 * List of Strings type. Number of items (N) as a four byte unsigned value followed by N Strings in
 * the format defined for String.
 */
final class ListUstring extends ImplementationType<String[]>
    implements CollectionType, BoundedType {

  private final ImplementationType<?> stringType;
  private final BoundInfo bound;
  private final String splName;

  private static final String[] EMPTY = new String[0];

  ListUstring(ImplementationType<?> stringType) {
    this(MetaType.LIST, stringType, BoundInfo.UNBOUNDED);
  }

  ListUstring(ImplementationType<?> stringType, int boundedLength) {
    this(MetaType.BLIST, stringType, new BoundInfo(boundedLength));
  }

  private ListUstring(MetaType metaType, ImplementationType<?> stringType, BoundInfo bound) {
    super(metaType, EMPTY);
    this.stringType = stringType;
    this.bound = bound;
    this.splName = bound.getSplName("list<" + stringType.getLanguageType() + ">");
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

    return bound.equals(((ListUstring) other).bound);
  }

  @Override
  public TypeEncoding getEncoding() {
    return getMetaType() == MetaType.LIST
        ? new UstringArrayEncoding()
        : new BoundedUstringArrayEncoding();
  }

  @Override
  String[] _randomValue(Random rand) {
    final String[] array =
        new String[rand.nextInt(getMetaType() == MetaType.LIST ? 20 : bound.getBoundedLength())];
    for (int i = 0; i < array.length; i++) {
      array[i] = Ustring.randomString(rand);
    }
    return array;
  }

  @Override
  public boolean isImmutable() {
    return false;
  }

  @Override
  String[] safeCopy(String[] value) {
    return value.clone();
  }
  /** Since we return an array any view of it must have a copy. */
  @Override
  final String[] safeView(String[] value) {
    return value.clone();
  }

  @Override
  public String getString(Object value) {
    // When in a composite the object is a List
    // and getString will be called for the
    // natural string representation of that composite.
    if (value instanceof List) return value.toString();

    return Arrays.toString((String[]) value);
  }

  @Override
  public final List<?> convertToList(String[] value) {
    return Collections.unmodifiableList(Arrays.asList(value));
  }

  @Override
  public Class<?> getAsCompositeElementType() {
    return List.class;
  }

  @Override
  public Type getElementType() {
    return stringType;
  }

  @Override
  public String getLanguageType() {
    return splName;
  }

  @Override
  public final int getBoundedSize() {
    return bound.getBoundedLength();
  }

  /** Strip out any excess elements. */
  @Override
  public final String[] convertFromObject(final Object value) {
    return convertFromObjectCopy(value);
  }

  @Override
  public final String[] convertFromObjectCopy(final Object value) {
    String[] sv = super.convertFromObject(value);
    int copyCount = bound.copyCount(sv.length);
    if (copyCount != sv.length) {
      String[] subset = new String[copyCount];
      System.arraycopy(sv, 0, subset, 0, copyCount);
      return subset;
    }
    return safeCopy(sv);
  }

  @Override
  public String[] convertFrom(List<?> list) {
    if (list.isEmpty()) return EMPTY;

    int copyCount = bound.copyCount(list.size());
    if (copyCount != list.size()) list = list.subList(0, copyCount);

    try {
      try {
        if (list.contains(null))
          throw (RuntimeException)
              invalidConversion("java.util.List").initCause(new NullPointerException());
      } catch (NullPointerException e) {
        // Indicates the List does not allow null which is ok as
        // SPL does not support null
      }
      return list.toArray(EMPTY);
    } catch (ArrayStoreException e) {
      throw (RuntimeException) invalidConversion("java.util.List").initCause(e);
    }
  }

  /**
   * Validate this type when it is an element in a collection type (Set, Map, List). This element
   * must be a List in a collection.
   *
   * @param element
   */
  @Override
  final Object validateAsCollectionElement(Object element) {
    return ListSpl.validateAsCollectionElementList(stringType, element);
  }

  @Override
  final Object convertToCollectionValue(String[] value) {
    return convertToList(value);
  }

  @Override
  String[] convertFromCollectionValue(Object value) {
    return convertFrom((List<?>) value);
  }

  @SuppressWarnings("unchecked")
  @Override
  Object safeCollectionCopy(Object list) {
    return new ArrayList<String>((List<String>) list);
  }

  class UstringArrayEncoding implements TypeEncoding {

    private final BLen blen;
    final TypeEncoding ustringEncoding;

    UstringArrayEncoding() {
      this.blen = bound.getSizeType();
      this.ustringEncoding = stringType.getEncoding();
    }

    @Override
    public final int getSerializedSize() {
      return VARIABLE_LENGTH;
    }

    @SuppressWarnings("unchecked")
    public long getMaxSerializedSize(Object value) {

      long totalLength = blen.getSerializedSize();

      if (value instanceof List) {

        for (String sv : (List<String>) value)
          totalLength += ustringEncoding.getMaxSerializedSize(sv);

      } else {

        final String[] array = (String[]) value;

        for (String sv : array) totalLength += ustringEncoding.getMaxSerializedSize(sv);
      }

      return totalLength;
    }

    public final void writeValue(ByteBuffer bytes, Object value) {
      writeList(bytes, (String[]) value);
    }

    void writeList(ByteBuffer bytes, String[] list) {
      blen.putLength(bytes, list.length);
      for (String sv : list) ustringEncoding.writeValue(bytes, sv);
    }

    public final Object readValue(ByteBuffer bytes) {
      return readList(bytes);
    }

    String[] readList(ByteBuffer bytes) {
      final int n = blen.getLength(bytes);
      String[] value = new String[n];
      for (int i = 0; i < n; i++) value[i] = (String) ustringEncoding.readValue(bytes);
      return value;
    }
  }

  class BoundedUstringArrayEncoding extends UstringArrayEncoding {

    private final int padElementSize;

    BoundedUstringArrayEncoding() {
      ByteBuffer buf = ByteBuffer.allocate((int) ustringEncoding.getMaxSerializedSize(""));
      ustringEncoding.writeValue(buf, "");
      padElementSize = buf.position();
    }

    public long getMaxSerializedSize(Object value) {
      long totalLength = super.getMaxSerializedSize(value);
      int n;
      if (value instanceof List) n = ((List<?>) value).size();
      else n = ((String[]) value).length;

      int padElementCount = bound.getBoundedLength() - n;
      if (padElementCount > 0) {
        totalLength += (padElementCount * padElementSize);
      }

      return totalLength;
    }

    /** Write the list padded by a number of default elements. */
    void writeList(ByteBuffer bytes, String[] list) {
      super.writeList(bytes, list);
      int padElementCount = bound.getBoundedLength() - list.length;
      for (int i = 0; i < padElementCount; i++) ustringEncoding.writeValue(bytes, "");
    }
    /** Read the list and skip any pad bytes. */
    String[] readList(ByteBuffer bytes) {
      String[] list = super.readList(bytes);
      int padElementCount = bound.getBoundedLength() - list.length;
      if (padElementCount > 0) {
        skip(bytes, padElementCount, padElementSize);
      }
      return list;
    }
  }
}
