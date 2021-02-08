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

/** Short type, a two-byte integer in network byte order. */
class Int16 extends StatelessType<Short> {

  Int16() {
    super(MetaType.INT16, Short.valueOf((short) 0));
  }

  Int16(MetaType type) {
    super(type, Short.valueOf((short) 0));
  }

  @Override
  public final Object readValue(ByteBuffer bytes) {
    return Short.valueOf(bytes.getShort());
  }

  @Override
  public final void writeValue(ByteBuffer bytes, Object value) {
    bytes.putShort((Short) value);
  }

  @Override
  final Short _randomValue(Random rand) {
    return (short) rand.nextInt();
  }

  @Override
  public final int getSerializedSize() {
    return 2;
  }

  @Override
  public int getInt(Object value) {
    return ((Short) value).intValue();
  }

  @Override
  public final short getShort(Object value) {
    return ((Short) value).shortValue();
  }

  @Override
  public long getLong(Object value) {
    return ((Short) value).longValue();
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return this.getBigDecimalAsLong(object);
  }

  @Override
  public final Short convertFrom(byte value) {
    return convertFrom((short) value);
  }

  @Override
  public final Short convertFrom(short value) {
    return Short.valueOf(value);
  }
}
