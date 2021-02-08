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

import java.math.BigDecimal;
import java.nio.ByteBuffer;
import java.util.Random;

/** SPL int8 type - signed integer. */
class Int8 extends StatelessType<Byte> {

  Int8() {
    super(MetaType.INT8, Byte.valueOf((byte) 0));
  }

  Int8(MetaType meta) {
    super(meta, Byte.valueOf((byte) 0));
  }

  @Override
  public final Object readValue(ByteBuffer bytes) {
    return Byte.valueOf(bytes.get());
  }

  @Override
  public final void writeValue(ByteBuffer bytes, Object value) {
    bytes.put(((Byte) value).byteValue());
  }

  @Override
  final Byte _randomValue(Random rand) {
    return Byte.valueOf((byte) rand.nextInt());
  }

  @Override
  public final int getSerializedSize() {
    return 1;
  }

  @Override
  public final byte getByte(Object value) {
    return ((Byte) value).byteValue();
  }

  @Override
  public short getShort(Object value) {
    return ((Byte) value).shortValue();
  }

  @Override
  public int getInt(Object value) {
    return ((Byte) value).intValue();
  }

  @Override
  public long getLong(Object value) {
    return ((Byte) value).longValue();
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return this.getBigDecimalAsLong(object);
  }

  @Override
  public final Byte convertFrom(byte value) {
    return Byte.valueOf(value);
  }
}
