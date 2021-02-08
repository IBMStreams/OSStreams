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

package com.ibm.streams.operator.internal.object;

import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.meta.TupleType;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.operator.types.XML;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.math.BigDecimal;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

/** Generic read-write tuple that represents all attribute values as Java objects. */
class ObjectOutputTuple extends ObjectInputTuple {

  private static final long serialVersionUID = -2060710509922878302L;

  ObjectOutputTuple(Schema schema) {
    super(schema, schema.newDefaultValues());
  }

  @Override
  public Tuple asReadOnlyTuple() {
    Object[] ro = new Object[values.length];

    for (int i = 0; i < values.length; i++) ro[i] = getType(i).copy(values[i]);

    return new OpInputTuple(getSchema(), ro);
  }

  public final void setObject(int ai, Object value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFromObject(value);
  }

  public final void setList(int ai, List<?> value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setSet(int ai, Set<?> value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setMap(int ai, Map<?, ?> value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setOptional(int ai, Optional<?> value) {
    if (value == null) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setNull(int ai) {
    values[ai] = getType(ai).setNull();
  }

  public final void setBlob(int ai, Blob value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setXML(int ai, XML value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setTuple(int ai, Tuple value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFromObject(value);
  }

  public final void assignTuple(int ai, Tuple tuple) {
    if (tuple == null) throw nullAttribute(ai);
    Type type = getType(ai);
    Type valueType = type.getMetaType().isOptional() ? ((OptionalType) type).getValueType() : type;
    if (valueType.getMetaType() != Type.MetaType.TUPLE)
      throw getType(ai).invalidConversion(Tuple.class.getName());
    setTuple(ai, ((TupleType) valueType).getTupleSchema().getTuple(tuple));
  }

  public <E extends Enum<E>> void setEnum(Class<E> enumType, int ai, E value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(enumType, value);
  }

  public final void setBoolean(final int ai, boolean value) {
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setByte(final int ai, byte value) {
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setShort(final int ai, short value) {
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setInt(final int ai, int value) {
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setLong(final int ai, long value) {
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setFloat(final int ai, float value) {
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setDouble(final int ai, double value) {
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setBigDecimal(final int ai, BigDecimal value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setString(final int ai, String value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setTimestamp(final int ai, Timestamp value) {
    if (value == null && !getType(ai).getMetaType().isOptional()) throw nullAttribute(ai);
    values[ai] = getType(ai).convertFrom(value);
  }

  public final void setBoolean(String attributeName, boolean value) {
    setBoolean(getAttributeIndex(attributeName), value);
  }

  public final void setByte(String attributeName, byte value) {
    setByte(getAttributeIndex(attributeName), value);
  }

  public final void setShort(String attributeName, short value) {
    setShort(getAttributeIndex(attributeName), value);
  }

  public final void setInt(String attributeName, int value) {
    setInt(getAttributeIndex(attributeName), value);
  }

  public final void setLong(String attributeName, long value) {
    setLong(getAttributeIndex(attributeName), value);
  }

  public final void setFloat(String attributeName, float value) {
    setFloat(getAttributeIndex(attributeName), value);
  }

  public final void setDouble(String attributeName, double value) {
    setDouble(getAttributeIndex(attributeName), value);
  }

  public final void setBigDecimal(String attributeName, BigDecimal value) {
    setBigDecimal(getAttributeIndex(attributeName), value);
  }

  public final void setString(String attributeName, String value) {
    setString(getAttributeIndex(attributeName), value);
  }

  public final void setTimestamp(String attributeName, Timestamp value) {
    setTimestamp(getAttributeIndex(attributeName), value);
  }

  public final void setObject(String attributeName, Object value) {
    setObject(getAttributeIndex(attributeName), value);
  }

  public final void setList(String attributeName, List<?> value) {
    setList(getAttributeIndex(attributeName), value);
  }

  public final void setSet(String attributeName, Set<?> value) {
    setSet(getAttributeIndex(attributeName), value);
  }

  public final void setMap(String attributeName, Map<?, ?> value) {
    setMap(getAttributeIndex(attributeName), value);
  }

  public final void setOptional(String attributeName, Optional<?> value) {
    setOptional(getAttributeIndex(attributeName), value);
  }

  public final void setNull(String attributeName) {
    setNull(getAttributeIndex(attributeName));
  }

  public final void setBlob(String attributeName, Blob value) {
    setBlob(getAttributeIndex(attributeName), value);
  }

  public final void setXML(String attributeName, XML value) {
    setXML(getAttributeIndex(attributeName), value);
  }

  public final void setTuple(String attributeName, Tuple value) {
    setTuple(getAttributeIndex(attributeName), value);
  }

  public final void assignTuple(String attributeName, Tuple tuple) {
    assignTuple(getAttributeIndex(attributeName), tuple);
  }

  public <E extends Enum<E>> void setEnum(Class<E> enumType, String attributeName, E value) {
    setEnum(enumType, getAttributeIndex(attributeName), value);
  }

  IllegalArgumentException nullAttribute(int ai) {
    return new IllegalArgumentException(
        new Message(Key.SPL_RUNTIME_NULL_ATTRIBUTE, ai).getLocalizedMessageAndPrefix());
  }
}
