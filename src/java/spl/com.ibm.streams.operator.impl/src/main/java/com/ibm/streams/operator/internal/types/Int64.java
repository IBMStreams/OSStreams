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

/** Long type, a eight-byte integer in network byte order. */
class Int64 extends StatelessType<Long> {

  Int64() {
    super(MetaType.INT64, Long.valueOf(0L));
  }

  Int64(MetaType type) {
    super(type, Long.valueOf(0L));
  }

  @Override
  public final Object readValue(ByteBuffer bytes) {
    return bytes.getLong();
  }

  @Override
  public final void writeValue(ByteBuffer bytes, Object value) {
    bytes.putLong((Long) value);
  }

  @Override
  final Long _randomValue(Random rand) {
    return rand.nextLong();
  }

  @Override
  public final int getSerializedSize() {
    return 8;
  }

  @Override
  public final long getLong(Object value) {
    return ((Long) value).longValue();
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return this.getBigDecimalAsLong(object);
  }

  @Override
  public final Long convertFrom(byte value) {
    return convertFrom((long) value);
  }

  @Override
  public final Long convertFrom(short value) {
    return convertFrom((long) value);
  }

  @Override
  public final Long convertFrom(int value) {
    return convertFrom((long) value);
  }

  @Override
  public final Long convertFrom(long value) {
    return Long.valueOf(value);
  }
}
