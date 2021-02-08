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

/** Double type. A single 8 byte double in network byte order. */
final class Float64 extends StatelessType<Double> {

  Float64() {
    super(MetaType.FLOAT64, Double.valueOf(0.0));
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return Double.valueOf(bytes.getDouble());
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    bytes.putDouble((Double) value);
  }

  @Override
  Double _randomValue(Random rand) {
    return rand.nextDouble();
  }

  @Override
  public int getSerializedSize() {
    return 8;
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return new BigDecimal(getDouble(object));
  }

  @Override
  public double getDouble(Object value) {
    return ((Double) value).doubleValue();
  }

  @Override
  public Double convertFrom(float value) {
    return convertFrom((double) value);
  }

  @Override
  public Double convertFrom(double value) {
    return Double.valueOf(value);
  }
}
