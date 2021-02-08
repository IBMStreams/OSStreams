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
import com.ibm.streams.operator.internal.runtime.AbstractTuple;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.internal.types.ImplementationType;
import com.ibm.streams.operator.internal.types.OptionalSpl;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.operator.types.XML;
import java.math.BigDecimal;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

/** Generic read-only tuple that represents all attribute values as Java objects. */
abstract class ObjectInputTuple extends AbstractTuple {

  private static final long serialVersionUID = 5754612734045501062L;
  protected final Object[] values;

  public ObjectInputTuple(Schema schema, Object[] values) {
    super(schema);
    this.values = values;
  }

  /** Get the type that defines the behaviour of the attribute at the given attribute index. */
  protected final ImplementationType<?> getType(final int attributeIndex) {
    try {
      return getSchema().getImplementationTypes()[attributeIndex];
    } catch (ArrayIndexOutOfBoundsException e) {
      throw attributeIndexOutOfRange(attributeIndex, e);
    }
  }

  @Override
  public final Object getObject(final int attributeIndex) {
    return getType(attributeIndex).getObject(values[attributeIndex]);
  }

  @Override
  public final <T> T getObject(final int attributeIndex, final Class<T> type) {
    T value = type.cast(getObject(attributeIndex));
    if (value == null)
      type.cast(((OptionalSpl) getType(attributeIndex)).getValueType().getDefaultValue());
    return value;
  }

  @Override
  public final boolean getBoolean(final int attributeIndex) {
    return getType(attributeIndex).getBoolean(values[attributeIndex]);
  }

  @Override
  public final byte getByte(final int attributeIndex) {
    return getType(attributeIndex).getByte(values[attributeIndex]);
  }

  @Override
  public final short getShort(final int attributeIndex) {
    return getType(attributeIndex).getShort(values[attributeIndex]);
  }

  @Override
  public final int getInt(final int attributeIndex) {
    return getType(attributeIndex).getInt(values[attributeIndex]);
  }

  @Override
  public final long getLong(final int attributeIndex) {
    return getType(attributeIndex).getLong(values[attributeIndex]);
  }

  @Override
  public final float getFloat(final int attributeIndex) {
    return getType(attributeIndex).getFloat(values[attributeIndex]);
  }

  @Override
  public final double getDouble(final int attributeIndex) {
    return getType(attributeIndex).getDouble(values[attributeIndex]);
  }

  @Override
  public final BigDecimal getBigDecimal(final int attributeIndex) {
    return getType(attributeIndex).getBigDecimal(values[attributeIndex]);
  }

  @Override
  public final String getString(final int attributeIndex) {
    return getType(attributeIndex).getString(values[attributeIndex]);
  }

  @Override
  public final Timestamp getTimestamp(final int attributeIndex) {
    return getType(attributeIndex).getTimestamp(values[attributeIndex]);
  }

  @Override
  public final Buffer getBuffer(final int attributeIndex) {
    return getType(attributeIndex).getBuffer(values[attributeIndex]);
  }

  @Override
  public final <T extends Buffer> T getBuffer(int attributeIndex, Class<T> type) {
    return type.cast(getBuffer(attributeIndex));
  }

  @Override
  public final List<?> getList(final int attributeIndex) {
    return getType(attributeIndex).getList(values[attributeIndex]);
  }

  @Override
  public final Set<?> getSet(final int attributeIndex) {
    return getType(attributeIndex).getSet(values[attributeIndex]);
  }

  @Override
  public final Map<?, ?> getMap(final int attributeIndex) {
    return getType(attributeIndex).getMap(values[attributeIndex]);
  }

  @SuppressWarnings("unchecked")
  @Override
  public final <T> Optional<T> getOptional(final int attributeIndex, final Class<T> valueType) {
    T value = valueType.cast(getObject(attributeIndex));
    if (value == null)
      valueType.cast(((OptionalSpl) getType(attributeIndex)).getValueType().getDefaultValue());
    return (Optional<T>) getType(attributeIndex).getOptional(value);
  }

  @Override
  public final Tuple getTuple(final int attributeIndex) {
    return getType(attributeIndex).getTuple(values[attributeIndex]);
  }

  @Override
  public final Blob getBlob(final int attributeIndex) {
    return getType(attributeIndex).getBlob(values[attributeIndex]);
  }

  @Override
  public final XML getXML(final int attributeIndex) {
    return getType(attributeIndex).getXML(values[attributeIndex]);
  }

  @Override
  public final <E extends Enum<E>> E getEnum(Class<E> enumType, int attributeIndex) {
    return getType(attributeIndex).getEnum(enumType, values[attributeIndex]);
  }

  long getMaxSerializedSize(ObjectTupleEncoder encoder) {
    return encoder.getMaxSerializedSize(values);
  }

  void encode(ObjectTupleEncoder encoder, ByteBuffer rawTuple) {
    encoder.encode(values, rawTuple);
  }

  @Override
  public int hashCode() {
    // TODO
    return 88;
  }

  @Override
  public boolean equals(Object other) {
    if (other == this) return true;
    if (!(other instanceof ObjectInputTuple)) return false;
    ObjectInputTuple ot = (ObjectInputTuple) other;

    // Allow non-null optional attribute values to equal non-optional values
    if (!getSchema().equalsOrOptional(ot.getSchema())) return false;

    for (int ai = 0; ai < values.length; ai++) {
      ImplementationType<?> valueType = getType(ai);
      ImplementationType<?> ovt = ot.getType(ai);
      if (valueType.getMetaType().isOptional()) {
        if (values[ai] == null) {
          if (!ovt.getMetaType().isOptional() || ot.values[ai] != null) return false;
          continue;
        }
        valueType = (ImplementationType<?>) ((OptionalSpl) valueType).getValueType();
      }
      if (ovt.getMetaType().isOptional() && ot.values[ai] == null) return false;
      switch (valueType.getMetaType()) {
        case DECIMAL32:
        case DECIMAL64:
        case DECIMAL128:
          if (((BigDecimal) values[ai]).compareTo((BigDecimal) ot.values[ai]) != 0) return false;
          continue;
          // Some lists as objects are represented by arrays which do not
          // compare using equals, but do when converted to List objects.
        case LIST:
        case BLIST:
          if (!valueType.getList(values[ai]).equals(valueType.getList(ot.values[ai]))) return false;
          break;
        default:
          if (!values[ai].equals(ot.values[ai])) return false;
          break;
      }
    }

    return true;
  }
}
