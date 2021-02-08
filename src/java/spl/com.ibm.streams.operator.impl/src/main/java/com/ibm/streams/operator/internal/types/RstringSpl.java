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

import com.ibm.streams.operator.internal.network.TupleSerialization.BLen;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.types.RString;
import java.nio.ByteBuffer;

class RstringSpl extends RawStringSpl {

  RstringSpl() {
    super(MetaType.RSTRING);
  }

  @Override
  final int getMaxRandomLength() {
    return 5;
  }

  @Override
  public final int getSerializedSize() {
    return TypeEncoding.VARIABLE_LENGTH;
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    return ((RString) value).getLength() + getBLen().getSerializedSize();
  }

  @Override
  BLen getBLen() {
    return BLen.UNBOUNDED;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return readRString(bytes);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    writeRString(bytes, (RString) value);
  }
}
