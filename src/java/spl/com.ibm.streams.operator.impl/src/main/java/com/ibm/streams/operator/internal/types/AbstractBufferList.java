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
import java.lang.reflect.Array;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Random;

/**
 * Generic class to handle types where the reading of the elements is handled by a typed Buffer with
 * fixed size elements, e.g. IntBuffer.
 */
abstract class AbstractBufferList<B extends Buffer, L> extends AbstractBufferType<B, L>
    implements CollectionType, BoundedType {

  private final ImplementationType<?> elementType;
  private final TypeEncoding encoding;
  private final String splName;
  /** Information about the lists bounds, set to BoundInfo.UNBOUNDED for unbounded lists. */
  private final BoundInfo bound;

  /** Unbounded List */
  AbstractBufferList(L defaultValue, ImplementationType<?> elementType) {
    this(MetaType.LIST, defaultValue, elementType, BoundInfo.UNBOUNDED);
  }

  /** Bounded List */
  AbstractBufferList(L defaultValue, ImplementationType<?> elementType, int boundedLength) {
    this(MetaType.BLIST, defaultValue, elementType, new BoundInfo(boundedLength));
  }

  private AbstractBufferList(
      MetaType metaType, L defaultValue, ImplementationType<?> elementType, BoundInfo bound) {
    super(metaType, defaultValue);
    this.elementType = elementType;
    this.bound = bound;
    encoding = metaType == MetaType.LIST ? new ArrayListEncoding() : new ArrayBoundedListEncoding();
    this.splName = bound.getSplName("list<" + elementType.getLanguageType() + ">");
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

    return bound.equals(((AbstractBufferList<?, ?>) other).bound);
  }

  @Override
  public final TypeEncoding getEncoding() {
    return encoding;
  }

  @Override
  public Class<?> getAsCompositeElementType() {
    return List.class;
  }

  @Override
  public final Type getElementType() {
    return elementType;
  }

  @Override
  public final int getBoundedSize() {
    return bound.getBoundedLength();
  }

  @Override
  public String getLanguageType() {
    return splName;
  }

  @Override
  public boolean isImmutable() {
    return false;
  }

  /** How many bytes an element of the specific type will take up in the ByteBuffer. */
  abstract int elementByteSize();

  /**
   * Move the position in the byte buffer forward to account for N elements being written in a typed
   * Buffer object that was a view on the ByteBuffer.
   */
  void adjustPosition(ByteBuffer bytes, int n) {
    if (n == 0) return;
    /*
     * Adjust the position in the byte buffer to account for the elements
     * being read by a different Buffer. Move the position in the ByteBuffer
     * forward by the number of elements multiplied by the byte size of each
     * element, e.g. N4 for an IntegerList.
     */
    skip(bytes, n, elementByteSize());
  }

  /** Get the typed buffer for this type used internally to read and write values. */
  abstract B typedBuffer(ByteBuffer bytes);

  /** Clone the value as an array. */
  @Override
  final L safeCopy(L value) {
    return cloneList(value);
  }

  /** Since we return an array any view of it must have a copy. */
  @Override
  L safeView(L internalValue) {
    return cloneList(internalValue);
  }

  abstract L cloneList(L list);

  abstract L newList(int n);

  abstract B readList(B buffer, L list);

  abstract B writeList(B buffer, L list);

  @Override
  L _randomValue(Random rand) {
    final L list =
        newList(rand.nextInt(getMetaType() == MetaType.LIST ? 20 : bound.getBoundedLength()));
    randomList(rand, list);
    return list;
  }

  /** Populate the list with a set of random values. */
  abstract void randomList(Random rand, L list);

  final Buffer getReadOnlyBuffer(L list) {
    return getTypedReadOnlyBuffer((L) list);
  }

  abstract B getTypedReadOnlyBuffer(L list);

  @SuppressWarnings("unchecked")
  @Override
  public final L convertFrom(final List<?> list) {
    if (list.isEmpty()) return (L) getDefaultValue();

    // Bounded lists drop elements.
    int copyCount = bound.copyCount(list.size());
    return convertListToArray(list, copyCount);
  }

  public final L convertFromObject(final Object value) {
    L lv = super.convertFromObject(value);
    if (bound == BoundInfo.UNBOUNDED) return safeCopy(lv);

    int n = Array.getLength(lv);
    int copyCount = bound.copyCount(n);
    if (copyCount == n) return lv;

    L subset = newList(copyCount);
    System.arraycopy(lv, 0, subset, 0, copyCount);

    return subset;
  }

  /**
   * Generically convert a list to an array, copying up to copyCount elements to match SPL bounded
   * type semantics of dropping excess elements.
   *
   * @param list
   * @param copyCount
   * @return
   */
  @SuppressWarnings("unchecked")
  protected final L genericListToArray(final List<?> list, final int copyCount) {

    try {
      final L array = (L) Array.newInstance(getObjectType().getComponentType(), copyCount);

      for (int i = 0; i < copyCount; i++) {
        Array.set(array, i, list.get(i));
      }
      return array;
    } catch (IllegalArgumentException e) {
      throw (RuntimeException) invalidConversion("java.util.List").initCause(e);
    }
  }

  abstract L convertListToArray(List<?> list, int copyCount);

  /**
   * Validate this type when it is an element in a collection type (Set, Map, List). This element
   * must be a List in a collection.
   *
   * @param element
   */
  final Object validateAsCollectionElement(Object element) {
    return ListSpl.validateAsCollectionElementList(elementType, element);
  }

  /** Return this element as a value that can be stored in a composite. */
  @Override
  final Object convertToCollectionValue(L value) {
    return convertToList(value);
  }

  @Override
  L convertFromCollectionValue(Object value) {
    return convertFrom((List<?>) value);
  }

  class ArrayListEncoding implements TypeEncoding {

    private final BLen blen;

    ArrayListEncoding() {
      this.blen = bound.getSizeType();
    }

    @Override
    public int getSerializedSize() {
      return VARIABLE_LENGTH;
    }

    /**
     * When in a collection the list will be represented by a List object and not an array. When
     * written out from an collection the collection type will convert the List to the non-composite
     * form, but to avoid doing this twice the conversion is not done when ascertaining the
     * serialized size.
     */
    @Override
    public long getMaxSerializedSize(Object value) {

      long elements;
      if (value instanceof List) elements = ((List<?>) value).size();
      else elements = Array.getLength(value);

      return blen.getSerializedSize() + (elements * elementByteSize());
    }

    /**
     * Read the value using the createAndPopulateArray assuming that method uses a different Buffer
     * to bytes (ByteBuffer).
     */
    @Override
    public final Object readValue(ByteBuffer bytes) {
      int n = blen.getLength(bytes);
      L list;
      if (n != 0) {
        list = newList(n);
        readList(typedBuffer(bytes), list);
      } else list = defaultValue;

      adjustBuffer(bytes, n);

      return list;
    }

    void adjustBuffer(ByteBuffer bytes, int n) {
      adjustPosition(bytes, n);
    }

    @SuppressWarnings("unchecked")
    @Override
    public final void writeValue(ByteBuffer bytes, Object value) {

      int elements = Array.getLength(value);

      blen.putLength(bytes, elements);

      if (elements != 0) writeList(typedBuffer(bytes), (L) value);

      adjustBuffer(bytes, elements);
    }
  }

  class ArrayBoundedListEncoding extends ArrayListEncoding {

    private final int fixedLength;

    ArrayBoundedListEncoding() {
      super();
      fixedLength =
          bound.getSizeType().getSerializedSize() + (bound.getBoundedLength() * elementByteSize());
    }

    @Override
    public int getSerializedSize() {
      return fixedLength;
    }

    @Override
    public long getMaxSerializedSize(Object value) {
      return fixedLength;
    }

    @Override
    void adjustBuffer(ByteBuffer bytes, int n) {
      super.adjustBuffer(bytes, n);
      int padElementCount = bound.getBoundedLength() - n;
      if (padElementCount > 0) skip(bytes, padElementCount, elementByteSize());
    }
  }
}
