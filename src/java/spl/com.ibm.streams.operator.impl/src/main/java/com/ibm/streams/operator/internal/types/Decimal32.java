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
 * IEEE754 decimal32 type handling using the standard decimal encoding with densely packed decimal
 * digits for tuple serialization (32 bits). Precision is 7 (9,999,999) and maximum positive
 * exponent is 96 (9.999999E96).
 *
 * <p>Format of binary serialized for decimal32 attributes in a tuple (internal to Streams) Written
 * as a single integer, with byte swapping according to platform.
 *
 * <p>Logical View & Big Endian :
 *
 * <p>scccccee(0) eeeexxxx(1) x(2) x(3)
 *
 * <p>Little Endian
 *
 * <p>x(3) x(2) eeeexxxx(1) scccccee(0)
 *
 * <p>(n) logical byte position in hex
 *
 * <p>s = sign bit c = combination bits e = exponent continuation x = coefficient continuation (x(n)
 * is 8 bits of coefficient continuation)
 *
 * <p>Value of this type is represented by a BigDecimal using MathContext.DECIMAL32 for rounding.
 */
final class Decimal32 extends DecimalSpl {

  /*
   * Bit masks for a 4 byte decimal32 using Densely packed decimal
   * with the four bytes represented as a int.
   */

  /** Negative bit is the first. */
  static final int NEGATIVE_BIT = 0x80000000;

  /** Five combination bits. */
  static final int COMBINATION_BITS = 0x7C000000;

  /** Six exponent bits will be used from the encoded exponent as the continuation bits. */
  private static final int ENCODED_EXPONENT_BITS = 0x3F;

  /**
   * The ENCODED_EXPONENT_BITS are shifted into the six bits following the sign bit and the 5
   * combination field bits. xxxxxxxx xxxxxxxx xxxxxxxx xxeeeeee to scccccee eeeexxxx xxxxxxxx
   * xxxxxxxx
   */
  static final int ENCODED_EXPONENT_SHIFT = 8 + 8 + 4;

  /** Six exponent continuation bits. */
  private static final int EXPONENT_CONTINUATION_BITS =
      ENCODED_EXPONENT_BITS << ENCODED_EXPONENT_SHIFT;

  private static final int COEFFICIENT_MUTIPLIER = 1000000;

  Decimal32() {
    super(MetaType.DECIMAL32, MathContext.DECIMAL32, 7, 96);
  }

  @Override
  public int getSerializedSize() {
    return 4;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return decode(bytes.getInt());
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    bytes.putInt(encode((BigDecimal) value));
  }

  private BigDecimal decode(final int enc) {

    final int combinationField = (enc & COMBINATION_BITS) >> (2 + 8 + 8 + 8);

    int exponentMSB = getExponentMSB(combinationField);
    int coefficientMSD = getCoefficientMSD(combinationField);

    // The encodedExponent includes a bias
    int exp =
        (exponentMSB << 6) + ((enc & EXPONENT_CONTINUATION_BITS) >> (4 + 8 + 8)) - exponentBias;

    // Two sets (20 bits) of coefficient continuation
    int digits = DenselyPackedDecimal.decode2Sets(coefficientMSD * COEFFICIENT_MUTIPLIER, enc);

    // Cheaper to negate the integer digits that negate
    // a BigDecimal, hence pass false to newBigDecimal.
    // Since digits has a precision of 7 there is no concern
    // over changing the absolute value on negation
    if ((enc & NEGATIVE_BIT) != 0) digits *= -1;

    return newBigDecimal(digits, exp);
  }

  private int encode(BigDecimal decimal) {

    decimal = normalize(decimal);
    boolean negative = decimal.signum() == -1;
    int digits = decimal.unscaledValue().intValue();
    if (negative) digits *= -1;

    int encodedExponent = (-decimal.scale()) + exponentBias;

    // It's possible that a value has an an exponent value (scale) that cannot be
    // represented in the serialized format. This occurs when the unscaled
    // value has less digits than the maximum and the exponent is too large.
    // e.g. 3.251E95 is 3251 scale -92 which is exponent 92 and encoded exponent 193
    // which is beyond the range of 3x2^6 (192). Thus we need to reduce the exponent
    // and multiply the digits by the same factor of 10.
    if (encodedExponent > maxEncodedExponent) {

      // Proof that the following multiplication can never overflow:
      //
      // Assertion: Overall value is within range of a decimal32 at this point
      //            (enforced by normalize)
      // Assertion: The unscaled value has no trailing zeros
      //            (enforced by normalize)
      // Assertion: The unscaled value has maximum precision of 7
      //            (enforced by normalize using MathContext.DECIMAL32)
      //
      // Both BigDecimal and decimal32 represent the value as an unscaled
      // integer (max 7 digits at this point) and a exponent.
      //
      // Since the overall value must be representable as a decimal32
      // then there is some ten-power-based multiple of the unscaled value that
      // does represent the overall value along with a reduced valid encoded
      // exponent (namely encodedExponent -= delta;
      // (that does
      //
      // Since the maximum value of an unscaled integer in decimal 9,999,999
      // then that must be the maximum result of any multiplication and hence
      // no overflow.
      // e.g. with a value of 99 the maximum delta can be is 5 leading to
      // 9,900,000

      int delta = (encodedExponent - maxEncodedExponent);
      encodedExponent -= delta;

      for (int i = 0; i < delta; i++) digits *= 10;
    }

    int enc = negative ? NEGATIVE_BIT : 0;

    int coefficientMSD = digits / COEFFICIENT_MUTIPLIER;

    enc |= encodeCombinationField(coefficientMSD, encodedExponent);

    // Then the next six bits are the remainder of the encodedExponent
    // highest two bytes seecccEE EEEExxxx
    enc |= ((encodedExponent & ENCODED_EXPONENT_BITS) << ENCODED_EXPONENT_SHIFT);

    // then the remaining coefficient digits are encoded in groups of three
    // digits using DPD with 10 bits each.
    enc |=
        DenselyPackedDecimal.pack(digits / 1000)
            << DenselyPackedDecimal.COEFFICIENT_COMBINATION_SHIFT;
    enc |= DenselyPackedDecimal.pack(digits);

    return enc;
  }
}
