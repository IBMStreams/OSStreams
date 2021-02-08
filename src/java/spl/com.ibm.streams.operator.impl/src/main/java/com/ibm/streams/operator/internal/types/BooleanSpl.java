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

import java.nio.ByteBuffer;
import java.util.Random;

/**
 * Boolean type. A single byte with a non-zero value representing truth, zero representing false.
 * This class always writes 1 for true.
 */
final class BooleanSpl extends StatelessType<Boolean> {

  BooleanSpl() {
    super(MetaType.BOOLEAN, Boolean.FALSE);
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return Boolean.valueOf(readBoolean(bytes));
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    writeBoolean(bytes, (Boolean) value);
  }

  @Override
  public boolean getBoolean(Object value) {
    return (Boolean) value;
  }

  @Override
  Boolean _randomValue(Random rand) {
    return rand.nextBoolean();
  }

  @Override
  public int getSerializedSize() {
    return 1;
  }

  static void writeBoolean(ByteBuffer bytes, boolean value) {
    bytes.put(value ? (byte) 1 : (byte) 0);
  }

  static boolean readBoolean(ByteBuffer bytes) {
    return bytes.get() != 0;
  }

  @Override
  public Boolean convertFrom(boolean value) {
    return Boolean.valueOf(value);
  }
}
