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
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.Random;

abstract class RawStringSpl extends AbstractBufferType<ByteBuffer, RString>
    implements TypeEncoding {
  RawStringSpl(MetaType type) {
    super(type, new RString(new byte[0]));
  }

  @Override
  public final String getString(Object value) {
    return ((RString) value).getString();
  }

  @Override
  public RString convertFrom(String value) {
    return new RString(value);
  }

  @Override
  final Buffer getReadOnlyBuffer(RString value) {
    return value.getByteBuffer();
  }

  @Override
  final RString _randomValue(Random rand) {
    final byte[] rstring = new byte[rand.nextInt(getMaxRandomLength())];
    for (int i = 0; i < rstring.length; i++) rstring[i] = (byte) ('A' + rand.nextInt(26));
    return new RString(rstring);
  }

  @Override
  public final TypeEncoding getEncoding() {
    return this;
  }

  @Override
  final RString safeCopy(RString internalValue) {
    return internalValue;
  }

  abstract int getMaxRandomLength();

  abstract BLen getBLen();

  RString readRString(ByteBuffer bytes) {
    final int length = getBLen().getLength(bytes);
    if (length == 0) return defaultValue;
    return new RString(bytes, length);
  }

  void writeRString(ByteBuffer bytes, RString rstring) {
    final int length = rstring.getLength();
    getBLen().putLength(bytes, length);
    if (length != 0) rstring.put(bytes);
  }
}
