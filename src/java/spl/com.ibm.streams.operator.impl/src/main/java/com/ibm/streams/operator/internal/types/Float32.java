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

/** Float type, a single 4 byte double in network byte order. */
final class Float32 extends StatelessType<Float> {

  Float32() {
    super(MetaType.FLOAT32, Float.valueOf(0.0f));
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return Float.valueOf(bytes.getFloat());
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    bytes.putFloat((Float) value);
  }

  @Override
  Float _randomValue(Random rand) {
    return rand.nextFloat();
  }

  @Override
  public int getSerializedSize() {
    return 4;
  }

  @Override
  public float getFloat(Object value) {
    return ((Float) value).floatValue();
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return new BigDecimal(getDouble(object));
  }

  @Override
  public double getDouble(Object value) {
    return ((Float) value).doubleValue();
  }

  @Override
  public Float convertFrom(float value) {
    return Float.valueOf(value);
  }
}
