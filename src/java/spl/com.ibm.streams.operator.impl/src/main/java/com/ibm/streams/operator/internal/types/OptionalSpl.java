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
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.operator.types.XML;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.math.BigDecimal;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Random;
import java.util.Set;

/**
 * The optional type stores an optional value internally as an Object holding the value, if present,
 * or null, if not present.
 *
 * <p>The type of the optional value is the same type as would be used for storing the value of a
 * non-optional value (eg a Java array for an optional list of primitive types), unless it is an
 * element in a collection (eg a list of optional values), when the java.util.List object form is
 * used.
 */
public final class OptionalSpl extends StatelessType<Object> implements OptionalType {

  private final ImplementationType valueType;
  private final TypeEncoding valueEncoding;
  private final String splName;

  static ImplementationType<?> optional(ImplementationType<?> valueType) {
    return new OptionalSpl(valueType);
  }

  private OptionalSpl(ImplementationType<?> valueType) {
    super(MetaType.OPTIONAL, null);
    this.valueType = valueType;
    this.splName = "optional<" + valueType.getLanguageType() + ">";
    this.valueEncoding = valueType.getEncoding();
    if (valueType.getMetaType().isOptional())
      throw new UnsupportedOperationException(
          new Message(Key.SPL_RUNTIME_JAVAOP_INVALID_LANGUAGE_TYPE, splName)
              .getLocalizedMessageAndPrefix());
  }

  @Override
  public int hashCode() {
    return super.hashCode() + (17 * getValueType().hashCode());
  }

  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    OptionalType ot = (OptionalType) other;
    return getValueType().equals(ot.getValueType());
  }

  /**
   * Allow this optional type to match another type if the underlying type of this optional type
   * matches the other type, or the other type is optional and the underlying types match.
   */
  @Override
  public boolean equalsOrOptional(Object other) {
    return ((ImplementationType<?>) this.getValueType()).equalsOrOptional(other);
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    if (value == null) return 1;
    return 1 + valueEncoding.getMaxSerializedSize(value);
  }

  @Override
  public int getSerializedSize() {
    return VARIABLE_LENGTH;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    if (bytes.get() == 0) return null;
    return valueEncoding.readValue(bytes);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    if (value == null) {
      bytes.put((byte) 0);
      return;
    }
    bytes.put((byte) 1);
    valueEncoding.writeValue(bytes, value);
    return;
  }

  @Override
  public String getLanguageType() {
    return splName;
  }

  @Override
  final Object _randomValue(Random rand) {
    if (rand.nextDouble() < 0.1) return null;
    return valueType._randomValue(rand);
  }

  @Override
  public boolean isImmutable() {
    return valueType.isImmutable();
  }

  @SuppressWarnings("unchecked")
  @Override
  Object safeCopy(Object value) {
    if (value == null) return null;
    return valueType.safeCopy(value);
  }

  @Override
  Object safeView(Object internalValue) {
    return safeCopy(internalValue);
  }

  public Type getValueType() {
    return valueType;
  }

  @Override
  public Class<?> getObjectType() {
    return valueType.getObjectType();
  }

  @Override
  public Class<?> getAsCompositeElementType() {
    return valueType.getAsCompositeElementType();
  }

  @Override
  public String getString(Object value) {
    valueType.getString(valueType.getDefaultValue()); // verify conversion is allowed
    return (value == null ? "null" : valueType.getString(value));
  }

  @Override
  public Object convertFrom(boolean value) {
    return valueType.convertFrom(value);
  }

  @Override
  public Object convertFrom(byte value) {
    return valueType.convertFrom(value);
  }

  @Override
  public Object convertFrom(short value) {
    return valueType.convertFrom(value);
  }

  @Override
  public Object convertFrom(int value) {
    return valueType.convertFrom(value);
  }

  @Override
  public Object convertFrom(long value) {
    return valueType.convertFrom(value);
  }

  @Override
  public Object convertFrom(float value) {
    return valueType.convertFrom(value);
  }

  @Override
  public Object convertFrom(double value) {
    return valueType.convertFrom(value);
  }

  @Override
  public Object convertFrom(BigDecimal value) {
    return (value == null ? null : valueType.convertFrom(value));
  }

  @Override
  public Object convertFrom(String value) {
    return (value == null ? null : valueType.convertFrom(value));
  }

  @Override
  public Object convertFrom(Timestamp value) {
    return (value == null ? null : valueType.convertFrom(value));
  }

  @Override
  public Object convertFromObject(Object value) {
    return (value == null ? null : valueType.convertFromObject(value));
  }

  @Override
  Object validateAsCollectionElement(Object element) {
    return (element == null ? null : valueType.validateAsCollectionElement(element));
  }

  @SuppressWarnings("unchecked")
  @Override
  Object convertToCollectionValue(Object value) {
    return (value == null ? null : valueType.convertToCollectionValue(value));
  }

  @Override
  Object safeCollectionCopy(Object value) {
    return (value == null ? null : valueType.safeCollectionCopy(value));
  }

  @Override
  Object convertFromCollectionValue(Object value) {
    return (value == null ? null : valueType.convertFromCollectionValue(value));
  }

  @SuppressWarnings("unchecked")
  @Override
  public Object convertFrom(List<?> list) {
    return (list == null ? null : valueType.convertFrom(list));
  }

  @SuppressWarnings("unchecked")
  @Override
  public Object convertFrom(Set<?> set) {
    return (set == null ? null : valueType.convertFrom(set));
  }

  @SuppressWarnings("unchecked")
  @Override
  public Object convertFrom(Map<?, ?> map) {
    return (map == null ? null : valueType.convertFrom(map));
  }

  /** Accept an optional value wrapped in a Java Optional. */
  @Override
  public Object convertFrom(final Optional<?> optional) {
    if (!optional.isPresent()) return null;
    return valueType.convertFromObject(optional.get());
  }

  @Override
  public Object setNull() {
    return null;
  }

  @Override
  public Object convertFrom(Blob blob) {
    return (blob == null ? null : valueType.convertFrom(blob));
  }

  @Override
  public Object convertFrom(XML xml) {
    return (xml == null ? null : valueType.convertFrom(xml));
  }

  @SuppressWarnings("unchecked")
  @Override
  public <E extends Enum<E>> Object convertFrom(Class<E> enumType, E value) {
    return (value == null ? null : valueType.convertFrom(enumType, value));
  }

  @Override
  public MetaType getValueMetaType() {
    return valueType.getMetaType();
  }
}
