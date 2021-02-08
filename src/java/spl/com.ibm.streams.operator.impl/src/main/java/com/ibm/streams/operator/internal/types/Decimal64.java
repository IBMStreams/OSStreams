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
import java.math.MathContext;
import java.nio.ByteBuffer;

/**
 * IEEE754 decimal64 type handling using the standard decimal encoding with densely packed decimal
 * digits for tuple serialization (64 bits). Precision is 16 (9,999,999,999,999,999) and maximum
 * positive exponent is 384 (9.999999999999999E384).
 *
 * <p>Format of binary serialized for decimal64 attributes in a tuple (Streams native encoding)
 *
 * <p>Logical View:
 *
 * <p>scccccee(0) eeeeeexx(1) eexxxxxx(2) x(3) x(4) x(5) x(6) x(7)
 *
 * <p>Written as a long using platform's byte order
 *
 * <p>Big Endian
 *
 * <p>scccccee(0) eeeeeexx(1) xxxxxxxx(2) x(4) x(5) x(6) x(7)
 *
 * <p>Little Endian
 *
 * <p>x(7) x(6) x(5) x(4) x(3) xxxxxxxx(2) eeeeeexx(1) scccccee(0)
 *
 * <p>(n) logical byte position in hex s = sign bit c = combination bits e = exponent continuation x
 * = coefficient continuation (x(n) is 8 bits of coefficient continuation)
 *
 * <p>Value of this type is represented by a BigDecimal using MathContext.DECIMAL64 for rounding.
 */
final class Decimal64 extends DecimalSpl {

  /*
   * Bit masks for a 4 byte decimal32 using Densely packed decimal with the
   * four bytes represented as a int.
   */

  /** Negative bit is the first. */
  static final long NEGATIVE_BIT = ((long) Decimal32.NEGATIVE_BIT) << 32;

  /** Five combination bits. */
  static final long COMBINATION_BITS = ((long) Decimal32.COMBINATION_BITS) << 32;

  /** Eight exponent bits will be used from the encoded exponent as the continuation bits. */
  public static final int ENCODED_EXPONENT_BITS = 0xFF;

  /**
   * The ENCODED_EXPONENT_BITS are shifted into the eight bits following the sign bit and the 5
   * combination field bits. xxxxxxxx xxxxxxxx xxxxxxxx xxeeeeee to scccccee eeeexxxx xxxxxxxx
   * xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
   *
   * <p>Which is the same as the 32 bit shift less two (8 to 6 bits) and adding an entire 32 bit in
   * shift (top bytes of a long vs an int).
   */
  static final int ENCODED_EXPONENT_SHIFT = Decimal32.ENCODED_EXPONENT_SHIFT - 2 + 32;

  /** Six exponent continuation bits. */
  private static final long EXPONENT_CONTINUATION_BITS =
      ((long) ENCODED_EXPONENT_BITS) << ENCODED_EXPONENT_SHIFT;

  private static final long COEFFICIENT_MUTIPLIER = 1000000000000000L;

  Decimal64() {
    super(MetaType.DECIMAL64, MathContext.DECIMAL64, 16, 384);
  }

  @Override
  public int getSerializedSize() {
    return 8;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    // return decode(readWordSwappedLong(bytes));
    return decode(bytes.getLong());
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    bytes.putLong(encode((BigDecimal) value));
    // writeWordSwappedLong(bytes, encode((BigDecimal) value));
  }

  BigDecimal decode(final long enc) {

    int combinationField = (int) ((enc & COMBINATION_BITS) >>> (2 + 8 + 8 + 8 + 32));

    int exponentMSB = getExponentMSB(combinationField);
    int coefficientMSD = getCoefficientMSD(combinationField);

    // The encodedExponent includes a bias
    int exp =
        (exponentMSB << 8)
            + (int) ((enc & EXPONENT_CONTINUATION_BITS) >>> ENCODED_EXPONENT_SHIFT)
            - exponentBias;

    // 50 bits of coefficient continuation
    // five sets of three digits
    long digits = DenselyPackedDecimal.decodeNSets(coefficientMSD * COEFFICIENT_MUTIPLIER, enc, 5);

    // Cheaper to negate the long digits that negate
    // a BigDecimal, hence pass false to newBigDecimal.
    // Since digits has a precision of 16 there is no concern
    // over changing the absolute value on negation
    if ((enc & NEGATIVE_BIT) != 0) digits *= -1L;

    return newBigDecimal(digits, exp);
  }

  long encode(BigDecimal decimal) {

    decimal = normalize(decimal);
    boolean negative = decimal.signum() == -1;
    long digits = decimal.unscaledValue().longValue();
    if (negative) digits *= -1L;

    int encodedExponent = (-decimal.scale()) + exponentBias;

    if (encodedExponent > maxEncodedExponent) {
      int delta = (encodedExponent - maxEncodedExponent);
      encodedExponent -= delta;
      for (int i = 0; i < delta; i++) digits *= 10L;
    }

    long enc = negative ? NEGATIVE_BIT : 0;

    int coefficientMSD = (int) (digits / COEFFICIENT_MUTIPLIER);

    enc |= ((long) encodeCombinationField(coefficientMSD, encodedExponent >>> 2)) << (32);

    // Then the next eight bits are the remainder of the encodedExponent
    // highest two bytes seecccEE EEEEEExx
    enc |= (((long) (encodedExponent & ENCODED_EXPONENT_BITS)) << ENCODED_EXPONENT_SHIFT);

    if (digits == 0L) return enc;

    // then the remaining 15 coefficient digits are encoded in groups of
    // three
    // digits using DPD with 10 bits each.
    return DenselyPackedDecimal.encodeNSets(digits, enc, 5);
  }

  /*
  String displayEncoding(BigDecimal val) {
      return displayEncoding(encode(val));
  }

  String displayEncoding(long enc) {
      String desc = "sccccceeeeeeeeTTTTTTTTTTttttttttttTTTTTTTTTTttttttttttTTTTTTTTTT";
      String b = Long.toBinaryString(enc);
      for (int i = b.length(); i < desc.length(); i++) {
          b = "0" + b;
      }
      return desc + "\n" + b;
  }
  */
}
