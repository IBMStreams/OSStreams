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

import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.operator.types.XML;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.math.BigDecimal;
import java.nio.Buffer;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Random;
import java.util.Set;

/**
 * Abstract type that contains implementations of all the methods to get a Java specific type from
 * the attribute's value, all throwing a conversion error. An ImplementationType is thread-safe.
 *
 * @param T Represents the type used to represent an attribute's value in the tuple. Used mainly for
 *     compile type type checking with sub-classes, the external api continues to deal with generic
 *     Objects.
 */
public abstract class ImplementationType<T> implements Type {

  private final MetaType metaType;

  /** Default value (initial value of an attribute) as its object type. */
  protected final T defaultValue;

  ImplementationType(MetaType metaType, T defaultValue) {
    this.metaType = metaType;
    this.defaultValue = defaultValue;
  }

  @Override
  public int hashCode() {
    return getMetaType().hashCode();
  }

  /** This type is equal to another type if they have the same base type. */
  @Override
  public boolean equals(Object other) {
    if (!(other instanceof ImplementationType)) return false;

    ImplementationType<?> ot = (ImplementationType<?>) other;

    return this.getMetaType().equals(ot.getMetaType());
  }

  /**
   * Allow this type to match another type if the types are the same, or the underlying type of an
   * optional type is the same type. This implementation is overridden for optional and tuple types.
   */
  public boolean equalsOrOptional(Object other) {
    if (!(other instanceof ImplementationType)) return false;

    ImplementationType<?> ot = (ImplementationType<?>) other;
    Type ovt = (ot.getMetaType().isOptional() ? ((OptionalType) ot).getValueType() : ot);

    return this.equals(ovt);
  }

  /** Return an encoding object for this type. */
  public abstract TypeEncoding getEncoding();

  /**
   * Return an value that will be handed back to the application ensuring it is a safe copy so that
   * the original value remains unchanged. That is an immutable object is returned or a copy.
   *
   * @param value the object type as stored in the Tuple's object representation. This will be of
   *     type T but the use of T is for internal use only. External code sees T as generic objects.
   * @return A copy that is a safe copy of the value. typically it is the same type as T but some
   *     types may have an internal representation that is different to the external.
   */
  @SuppressWarnings("unchecked")
  public final Object getObject(Object internalValue) {
    return safeView((T) internalValue);
  }

  /**
   * Return an object with the same value as the passed in value. If T is immutable (e.g. Integer)
   * then the same reference can be returned. If T is mutable then a new object must be returned
   * that contains the same value, e.g. a clone an an int[] or new collection with the same contents
   * as the old.
   */
  abstract T safeCopy(T internalValue);

  /**
   * Return an object with the same value as the passed in value. If T is immutable (e.g. Integer)
   * then the same reference can be returned. For collection values a read-only view of the
   * underlying collection is returned. For SPL lists that are internally represented as arrays a
   * copy of the array is returned.
   *
   * <p>This implementation assumes the value is immutable and thus returns internalValue.
   */
  T safeView(T internalValue) {
    return internalValue;
  }

  /**
   * Copy an object of this type returning an immutable copy. This is the non-generic method for
   * external code. It just calls safeCopy.
   */
  @SuppressWarnings("unchecked")
  public final Object copy(Object v) {
    return safeCopy((T) v);
  }

  /**
   * Return the value for this type that corresponds to an uninitialized value, e.g. zero for
   * numeric types, null for optional types.
   *
   * @return
   */
  public final Object getDefaultValue() {
    return defaultValue;
  }

  /**
   * Return my object type used for getObject(). This implementation returns the class of the
   * default value for non-optional types. For an optional type the default value is null.
   */
  public Class<?> getObjectType() {
    return getDefaultValue().getClass();
  }

  public Class<?> getAsCompositeElementType() {
    return getObjectType();
  }

  /**
   * By default defer to the MetaType, composite, optional and bounded types need to override this.
   */
  @Override
  public String getLanguageType() {
    return getMetaType().getLanguageType();
  }

  @Override
  public final Object randomValue(Random rand) {
    return _randomValue(rand);
  }

  abstract T _randomValue(Random rand);

  /**
   * Return true if the Object value for this type is always immutable. Except for composite types,
   * this is true in general.
   */
  public boolean isImmutable() {
    return true;
  }

  public boolean getBoolean(Object value) {
    throw invalidConversion("boolean");
  }

  public byte getByte(Object value) {
    throw invalidConversion("byte");
  }

  public short getShort(Object value) {
    throw invalidConversion("short");
  }

  public int getInt(Object value) {
    throw invalidConversion("int");
  }

  public long getLong(Object value) {
    throw invalidConversion("long");
  }

  public float getFloat(Object value) {
    throw invalidConversion("float");
  }

  public double getDouble(Object value) {
    throw invalidConversion("double");
  }

  public BigDecimal getBigDecimal(Object object) {
    throw invalidConversion("java.math.BigDecimal");
  }

  public Tuple getTuple(Object object) {
    throw invalidConversion("com.ibm.streams.operator.Tuple");
  }

  /**
   * Get a BigDecimal from a Number value as a long.
   *
   * @param value
   * @return
   */
  final BigDecimal getBigDecimalAsLong(Object value) {
    long lv = getLong(value);
    if (lv == 0L) return BigDecimal.ZERO;
    return new BigDecimal(lv);
  }

  /**
   * Most types are convertible to a String, types that are not must explicitly throw an
   * invalidConversion() error.
   */
  public String getString(Object value) {
    return value.toString();
  }

  @Override
  public final String toString(Object value) {
    return getString(convertFromObject(value));
  }

  public Timestamp getTimestamp(Object value) {
    throw invalidConversion("com.ibm.streams.operator.types.Timestamp");
  }

  public Blob getBlob(Object value) {
    throw invalidConversion("com.ibm.streams.operator.types.Blob");
  }

  public Buffer getBuffer(Object value) {
    throw invalidConversion("java.nio.Buffer");
  }

  public XML getXML(Object value) {
    throw invalidConversion("com.ibm.streams.operator.types.XML");
  }
  /** Convert the attribute/type's value to a Java enumeration of the specific type. */
  public <E extends Enum<E>> E getEnum(Class<E> enumType, Object value) {
    throw invalidConversion(enumType.getName());
  }

  /**
   * Return a read-only List representing the values including the fact that the elements of the
   * list must also be safe copies, so that modifying them does not modify the Tuple.
   *
   * @param value
   * @return
   */
  @SuppressWarnings("unchecked")
  public final List<?> getList(Object value) {
    return convertToList((T) value);
  }

  /**
   * Return a read-only Set representing the values including the fact that the elements of the set
   * must also be safe copies, so that modifying them does not modify the Tuple.
   *
   * @param value
   * @return
   */
  @SuppressWarnings("unchecked")
  public final Set<?> getSet(Object value) {
    return Collections.unmodifiableSet(convertToSet((T) value));
  }

  /**
   * Return a read-only Map representing the values including the fact that the elements of the map
   * must also be safe copies, so that modifying them does not modify the Tuple.
   *
   * @param value
   * @return
   */
  @SuppressWarnings("unchecked")
  public final Map<?, ?> getMap(Object value) {
    return Collections.unmodifiableMap(convertToMap((T) value));
  }

  /**
   * Return a read-only Optional containing the value including the fact that the contents of the
   * optional must also be a safe copy, so that modifying them does not modify the Tuple.
   *
   * @param value
   * @return
   */
  @SuppressWarnings("unchecked")
  public final Optional<Object> getOptional(Object value) {
    if (value == null) {
      return Optional.empty();
    }
    return Optional.of(safeView((T) value));
  }

  /**
   * Convert to a list. Contents of the list must be safe copies so that modifying the elements
   * obtained from the List do not modify the Tuple. List must be read-only.
   */
  public List<?> convertToList(T value) {
    throw invalidConversion("java.util.List");
  }

  /**
   * Convert to a set. Contents of the set must be safe copies so that modifying the elements
   * obtained from the Set do not modify the Tuple. List must be read-only.
   */
  Set<?> convertToSet(T value) {
    throw invalidConversion("java.util.Set");
  }

  /**
   * Convert to a map. Contents of the map must be safe copies so that modifying the elements
   * obtained from the Map do not modify the Tuple. Map may be modifiable, it will be returned as
   * read-only by getMap().
   */
  Map<?, ?> convertToMap(T value) {
    throw invalidConversion("java.util.Map");
  }

  public T convertFrom(boolean value) {
    throw invalidConversion("boolean");
  }

  public T convertFrom(byte value) {
    throw invalidConversion("byte");
  }

  public T convertFrom(short value) {
    throw invalidConversion("short");
  }

  public T convertFrom(int value) {
    throw invalidConversion("int");
  }

  public T convertFrom(long value) {
    throw invalidConversion("long");
  }

  public T convertFrom(float value) {
    throw invalidConversion("float");
  }

  public T convertFrom(double value) {
    throw invalidConversion("double");
  }

  public T convertFrom(BigDecimal value) {
    throw invalidConversion("java.math.BigDecimal");
  }

  public T convertFrom(String value) {
    throw invalidConversion("String");
  }

  public T convertFrom(Timestamp value) {
    throw invalidConversion("com.ibm.streams.operator.types.Timestamp");
  }

  @SuppressWarnings("unchecked")
  public T convertFromObject(Object value) {
    if (this.getObjectType().isInstance(value)) return (T) value;
    throw invalidConversion(value.getClass().getName());
  }

  /**
   * Convert from an object value to this type and return a copy so that changes to value do not
   * modify the returned object. If value is immutable then the value can be returned.
   *
   * @param value
   * @return
   */
  public T convertFromObjectCopy(Object value) {
    return safeCopy(convertFromObject(value));
  }

  /**
   * Validate this type when it is an element in a composite type (Set, Map, List). This
   * implementation uses convertFromObject. Throws an exception if element type is not a valid
   * instance for this value.
   *
   * @param element
   */
  Object validateAsCollectionElement(Object element) {
    return convertFromObject(element);
  }

  /**
   * Convert an object representation of this type into one that can be stored in a collection. For
   * most types this is the same object where C is the same as T.
   */
  Object convertToCollectionValue(T value) {
    return safeCopy(value);
  }

  /**
   * Return a safe copy of an item in a collection. This implementation just uses safeCopy(). Types
   * that use a different type as collection values need to override this method.
   *
   * @param value
   * @return
   */
  @SuppressWarnings("unchecked")
  Object safeCollectionCopy(Object value) {
    return safeCopy((T) value);
  }

  /**
   * Convert a representation of this type that can be stored in a collection into the object
   * representation for this type. For most types this is the same object.
   */
  T convertFromCollectionValue(Object value) {
    return convertFromObject(value);
  }

  public T convertFrom(List<?> list) {
    throw invalidConversion("java.util.List");
  }

  public T convertFrom(Set<?> set) {
    throw invalidConversion("java.util.Set");
  }

  public T convertFrom(Map<?, ?> map) {
    throw invalidConversion("java.util.Map");
  }

  public T convertFrom(Optional<?> optional) {
    throw invalidConversion("java.util.Optional");
  }

  public T setNull() {
    throw invalidConversion("java.util.Optional");
  }

  public T convertFrom(Blob blob) {
    throw invalidConversion("com.ibm.streams.operator.types.Blob");
  }

  public T convertFrom(XML xml) {
    throw invalidConversion("com.ibm.streams.operator.types.XML");
  }

  /**
   * Convert to an Java enumeration value of the specific class to an value of this attribute/type.
   */
  public <E extends Enum<E>> T convertFrom(Class<E> enumType, E value) {
    throw invalidConversion(enumType.getName());
  }

  public RuntimeException invalidConversion(String javaType) {
    return new UnsupportedOperationException(
        new Message(
                Key.SPL_RUNTIME_INVALID_CONVERSION, this.getLanguageType(), getMetaType(), javaType)
            .getLocalizedMessageAndPrefix());
  }

  public final MetaType getMetaType() {
    return metaType;
  }

  /**
   * Return the MetaType for a non-null Object value for this type. For optional types this is the
   * MetaType for the optional value. This is defined for all types as a convenience, to avoid
   * needing to check explicitly for an optional type.
   */
  public MetaType getValueMetaType() {
    return metaType;
  }

  @Override
  public final String toString() {
    return getMetaType() + ":" + getLanguageType();
  }
}
