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

import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.internal.network.TupleDecoder;
import com.ibm.streams.operator.internal.network.TupleEncoder;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.internal.object.ObjectTupleEncoder;
import com.ibm.streams.operator.internal.object.OpInputTuple;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.meta.TupleType;
import java.nio.ByteBuffer;
import java.util.Random;

final class TupleSpl extends ImplementationType<Tuple> implements TupleType {

  private final TupleEncoder<Tuple> encoder;
  private final TupleDecoder<Tuple> decoder;
  private final String splName;
  private final Schema schema;

  static TupleSpl newTupleSpl(String splName, Schema schema) {
    return new TupleSpl(splName, schema, schema.getTuple());
  }

  private TupleSpl(String splName, Schema schema, Tuple defaultValue) {
    this(
        splName,
        schema,
        new ObjectTupleEncoder(schema),
        OpInputTuple.getTupleDecoder(schema),
        defaultValue);
  }

  TupleSpl(
      String splName,
      Schema schema,
      TupleEncoder<Tuple> encoder,
      TupleDecoder<Tuple> decoder,
      Tuple defaultValue) {
    super(MetaType.TUPLE, defaultValue);
    this.splName = splName;
    this.schema = schema;
    this.encoder = encoder;
    this.decoder = decoder;
  }

  @Override
  public int hashCode() {
    return schema.hashCode();
  }

  /** Non-bounded types are equal if they have the same element type. */
  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    TupleType tt = (TupleType) other;
    return getTupleSchema().equals(tt.getTupleSchema());
  }

  /**
   * Allow this tuple type to match another type if the other type is a tuple or an optional tuple,
   * and they have matching attribute names and types in the exact same order, where the type of an
   * attribute is allowed to match if it has the same type, or if it is an optional type with the
   * same underlying type.
   */
  @Override
  public boolean equalsOrOptional(Object other) {
    if (!(other instanceof ImplementationType)) return false;

    ImplementationType<?> ot = (ImplementationType<?>) other;
    Type ovt = (ot.getMetaType().isOptional() ? ((OptionalType) ot).getValueType() : ot);
    if (!(ovt instanceof TupleType)) return false;

    TupleType tt = (TupleType) ovt;
    return ((Schema) getTupleSchema()).equalsOrOptional(tt.getTupleSchema());
  }

  @Override
  public String getLanguageType() {
    return splName;
  }

  /** Objects are returned as instances of the Map interface. */
  public Class<?> getObjectType() {
    return Tuple.class;
  }

  @Override
  public Tuple getTuple(Object object) {
    return (Tuple) object;
  }

  /** Validate the schema of the incoming Tuple matches. */
  @Override
  public final Tuple convertFromObject(Object value) {

    try {
      Tuple tuple = (Tuple) value;
      if (!tuple.getStreamSchema().equals(schema))
        throw invalidConversion(Tuple.class.getName() + ":tuple<" + tuple.getStreamSchema() + ">");
      return tuple.asReadOnlyTuple();
    } catch (ClassCastException e) {
      throw invalidConversion(value.getClass().getName());
    }
  }

  @Override
  Tuple _randomValue(Random rand) {
    Object[] values = new Object[getTupleSchema().getAttributeCount()];
    for (int ai = 0; ai < values.length; ai++) {
      values[ai] = schema.getImplementationTypes()[ai].randomValue(rand);
    }
    return getTupleSchema().getTuple(values);
  }

  @Override
  public TypeEncoding getEncoding() {
    return new TypeEncoding() {

      @Override
      public long getMaxSerializedSize(Object value) {
        return encoder.getMaxSerializedSize((Tuple) value);
      }

      @Override
      public int getSerializedSize() {
        return VARIABLE_LENGTH;
      }

      @Override
      public Object readValue(ByteBuffer bytes) {
        return decoder.decode(bytes);
      }

      @Override
      public void writeValue(ByteBuffer bytes, Object value) {
        encoder.encode((Tuple) value, bytes);
      }
    };
  }

  @Override
  Tuple safeCopy(Tuple internalValue) {
    return internalValue.asReadOnlyTuple();
  }

  @Override
  public StreamSchema getTupleSchema() {
    return schema;
  }
}
