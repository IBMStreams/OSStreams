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

/** Integer type, a four-byte integer in network byte order. */
class Int32 extends StatelessType<Integer> {

  Int32() {
    super(MetaType.INT32, Integer.valueOf(0));
  }

  Int32(MetaType type) {
    super(type, Integer.valueOf(0));
  }

  @Override
  public final Object readValue(ByteBuffer bytes) {
    return bytes.getInt();
  }

  @Override
  public final void writeValue(ByteBuffer bytes, Object value) {
    bytes.putInt((Integer) value);
  }

  @Override
  final Integer _randomValue(Random rand) {
    return rand.nextInt();
  }

  @Override
  public final int getSerializedSize() {
    return 4;
  }

  @Override
  public int getInt(Object value) {
    return ((Integer) value).intValue();
  }

  @Override
  public long getLong(Object value) {
    return ((Integer) value).longValue();
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return this.getBigDecimalAsLong(object);
  }

  @Override
  public final Integer convertFrom(byte value) {
    return convertFrom((int) value);
  }

  @Override
  public final Integer convertFrom(short value) {
    return convertFrom((int) value);
  }

  @Override
  public final Integer convertFrom(int value) {
    return Integer.valueOf(value);
  }
}
