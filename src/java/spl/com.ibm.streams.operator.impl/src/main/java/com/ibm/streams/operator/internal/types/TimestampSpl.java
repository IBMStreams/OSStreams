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

import com.ibm.streams.operator.types.Timestamp;
import java.nio.ByteBuffer;
import java.util.Random;
import java.util.concurrent.TimeUnit;

/** SPL timestamp. Format is long (seconds), int (nanoseconds) and int (machineId) */
final class TimestampSpl extends StatelessType<Timestamp> {

  private static final int NS_S = (int) TimeUnit.NANOSECONDS.convert(1, TimeUnit.SECONDS);

  TimestampSpl() {
    super(MetaType.TIMESTAMP, new Timestamp(0L, 0));
  }

  /** Format is long (seconds), int (nanoseconds) and int (machineId) */
  @Override
  public int getSerializedSize() {
    return 8 + 4 + 4;
  }

  @Override
  public Timestamp getTimestamp(Object value) {
    return (Timestamp) value;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return new Timestamp(bytes.getLong(), bytes.getInt(), bytes.getInt());
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    Timestamp ts = (Timestamp) value;
    bytes.putLong(ts.getSeconds());
    bytes.putInt(ts.getNanoseconds());
    bytes.putInt(ts.getMachineId());
  }

  @Override
  public Timestamp convertFrom(Timestamp value) {
    return value;
  }

  @Override
  Timestamp _randomValue(Random rand) {
    return new Timestamp(rand.nextLong(), rand.nextInt(NS_S), rand.nextInt());
  }
}
