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

import static com.ibm.streams.operator.internal.network.TupleSerialization.skip;

import com.ibm.streams.operator.internal.network.TupleSerialization.BLen;
import com.ibm.streams.operator.meta.BoundedType;
import com.ibm.streams.operator.types.RString;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.nio.ByteBuffer;

/**
 * Bounded rstring type.
 *
 * <p>Fixed length format for rstring[N] is:
 *
 * <OL>
 *   <LI>count of used bytes (U)
 *   <LI>U used bytes
 *   <LI>NUL byte ('\0') terminator (can also have embedded NUL)
 *   <LI>N-U bytes
 * </OL>
 *
 * Note that an unbounded rstring (RstringSpl) does not include a NUL terminator.
 *
 * <p>The format of the used bytes depends on N. For N < 256 it is a single byte as an unsigned byte
 * For N < 65536 && > 255 it is two bytes as an unsigned short Otherwise 4 bytes as a signed int
 * (since the maximum value of N is a signed integer). Format of the bounded used count is encoded
 * in the BLen enum.
 */
final class BstringSpl extends RawStringSpl implements BoundedType {

  private final long serializedSize;
  private final int fixedSize;
  private final BoundInfo bound;

  BstringSpl(int boundedLength) {
    super(MetaType.BSTRING);
    bound = new BoundInfo(boundedLength);
    serializedSize = ((long) boundedLength) + bound.getSizeType().getSerializedSize() + 1L;
    fixedSize = serializedSize > Integer.MAX_VALUE ? VARIABLE_LENGTH : (int) serializedSize;
  }

  @Override
  public int hashCode() {
    return super.hashCode() + (17 * getBoundedSize());
  }

  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    BoundedType bt = (BoundedType) other;
    return getBoundedSize() == bt.getBoundedSize();
  }

  @Override
  public final int getSerializedSize() {
    return fixedSize;
  }

  @Override
  public final long getMaxSerializedSize(Object value) {
    return serializedSize;
  }

  @Override
  public String getLanguageType() {
    return bound.getSplName("rstring");
  }

  @Override
  final int getMaxRandomLength() {
    return getBoundedSize();
  }

  /** Convert a String value to a bounded rstring. */
  @Override
  public RString convertFrom(String value) {

    RString rstring = super.convertFrom(value);

    if (false && rstring.getLength() > getBoundedSize())
      throw new StringIndexOutOfBoundsException(
          new Message(
                  Key.SPL_RUNTIME_STRING_LENGTH_TOO_LARGE_FOR_LANGUAGE,
                  getLanguageType(),
                  rstring.getLength())
              .getLocalizedMessageAndPrefix());

    return rstring;
  }

  @Override
  BLen getBLen() {
    return bound.getSizeType();
  }

  /** Read an Rstring value and then skip the C NUL and the remaining bytes that were not used. */
  @Override
  public Object readValue(ByteBuffer bytes) {

    RString value = readRString(bytes);

    skip(bytes, getBoundedSize() + 1 - value.getLength());

    return value;
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    RString rstring = (RString) value;
    writeRString(bytes, rstring);
    bytes.put((byte) '\0');
    if (rstring.getLength() < getBoundedSize()) skip(bytes, getBoundedSize() - rstring.getLength());
  }

  @Override
  public int getBoundedSize() {
    return bound.getBoundedLength();
  }
}
