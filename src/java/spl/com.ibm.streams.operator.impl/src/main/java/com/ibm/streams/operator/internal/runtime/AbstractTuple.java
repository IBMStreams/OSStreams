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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.types.ImplementationType;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.operator.types.XML;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.Serializable;
import java.math.BigDecimal;
import java.nio.Buffer;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

/** Abstract Tuple that maintains a schema for the tuple. */
public abstract class AbstractTuple implements Tuple, Serializable {

  private static final long serialVersionUID = -389546255087628049L;

  private final Schema schema;

  protected AbstractTuple(Schema schema) {
    this.schema = schema;
  }

  protected final Schema getSchema() {
    return schema;
  }

  @Override
  public final StreamSchema getStreamSchema() {
    return getSchema();
  }

  protected final int getAttributeIndex(String attributeName) {
    int index = schema.getAttributeIndex(attributeName);
    if (index < 0) throw attributeNameInvalid(attributeName);
    return index;
  }

  @Override
  public final boolean getBoolean(String attributeName) {
    return getBoolean(getAttributeIndex(attributeName));
  }

  @Override
  public final byte getByte(String attributeName) {
    return getByte(getAttributeIndex(attributeName));
  }

  @Override
  public final BigDecimal getBigDecimal(String attributeName) {
    return getBigDecimal(getAttributeIndex(attributeName));
  }

  @Override
  public final double getDouble(String attributeName) {
    return getDouble(getAttributeIndex(attributeName));
  }

  @Override
  public final float getFloat(String attributeName) {
    return getFloat(getAttributeIndex(attributeName));
  }

  @Override
  public final int getInt(String attributeName) {
    return getInt(getAttributeIndex(attributeName));
  }

  @Override
  public final long getLong(String attributeName) {
    return getLong(getAttributeIndex(attributeName));
  }

  @Override
  public final short getShort(String attributeName) {
    return getShort(getAttributeIndex(attributeName));
  }

  @Override
  public final String getString(String attributeName) {
    return getString(getAttributeIndex(attributeName));
  }

  @Override
  public final Timestamp getTimestamp(String attributeName) {
    return getTimestamp(getAttributeIndex(attributeName));
  }

  @Override
  public final Buffer getBuffer(String attributeName) {
    return getBuffer(getAttributeIndex(attributeName));
  }

  @Override
  public final <T extends Buffer> T getBuffer(String attributeName, Class<T> type) {
    return getBuffer(getAttributeIndex(attributeName), type);
  }

  @Override
  public final Blob getBlob(String attributeName) {
    return getBlob(getAttributeIndex(attributeName));
  }

  @Override
  public final Object getObject(String attributeName) {
    return getObject(getAttributeIndex(attributeName));
  }

  @Override
  public final <T> T getObject(final String attributeName, final Class<T> type) {
    return getObject(getAttributeIndex(attributeName), type);
  }

  @Override
  public final List<?> getList(String attributeName) {
    return getList(getAttributeIndex(attributeName));
  }

  @Override
  public final Set<?> getSet(String attributeName) {
    return getSet(getAttributeIndex(attributeName));
  }

  @Override
  public final Map<?, ?> getMap(String attributeName) {
    return getMap(getAttributeIndex(attributeName));
  }

  @Override
  public final <T> Optional<T> getOptional(String attributeName, Class<T> valueType) {
    return getOptional(getAttributeIndex(attributeName), valueType);
  }

  @Override
  public final Tuple getTuple(String attributeName) {
    return getTuple(getAttributeIndex(attributeName));
  }

  @Override
  public final XML getXML(String attributeName) {
    return getXML(getAttributeIndex(attributeName));
  }

  @Override
  public final <E extends Enum<E>> E getEnum(Class<E> enumType, String attributeName) {
    return getEnum(enumType, getAttributeIndex(attributeName));
  }

  protected IllegalArgumentException attributeIndexOutOfRange(
      int attributeIndex, ArrayIndexOutOfBoundsException e) {
    return new IllegalArgumentException(
        new Message(Key.SPL_RUNTIME_ATTR_INDEX_OUT_OF_RANGE, attributeIndex)
            .getLocalizedMessageAndPrefix(),
        e);
  }

  protected IllegalArgumentException attributeNameInvalid(String attributeName) {
    return new IllegalArgumentException(
        new Message(Key.SPL_RUNTIME_ATTR_NOT_EXIST_IN_SCHEMA, attributeName)
            .getLocalizedMessageAndPrefix());
  }

  public String toString() {
    StringBuilder sb = new StringBuilder();
    final ImplementationType<?>[] implTypes = schema.getImplementationTypes();
    sb.append("TUPLE{");
    for (int i = 0; i < schema.getAttributeCount(); i++) {
      if (i != 0) sb.append(",");
      sb.append(implTypes[i].getString(getObject(i)));
    }
    sb.append("}");
    return sb.toString();
  }
}
