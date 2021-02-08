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
import java.math.BigInteger;
import java.math.MathContext;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;

/**
 * IEEE754 decimal128 type handling using the standard decimal encoding with densely packed decimal
 * digits for tuple serialization (128 bits). Precision is 34
 * (9,999,999,999,999,999,999,999,999,999,999,999) and maximum positive exponent is 6144
 * (9.999999999999999999999999999999999E6144).
 *
 * <p>Format of binary serialized for decimal128 attributes in a tuple (internal to Streams) Logical
 * View: <---------- long e0 in code -----------------------------> <---- long e1 in code
 * ---------------> scccccee(0) eeeeeeee(1) eexxxxxx(2) x(3) x(4) x(5) x(6) x(7) x(8) x(9) x(a) x(b)
 * x(c) x(d) x(e) x(f)
 *
 * <p>Written as two longs using platform encoding for each long to mimic a 128bit value in the
 * platform encoding.
 *
 * <p>Big Endian (e0 followed by e1)
 *
 * <p>scccccee(0) eeeeeeee(1) eexxxxxx(2) x(3) x(4) x(5) x(6) x(7) x(8) x(9) x(a) x(b) x(c) x(d)
 * x(e) x(f)
 *
 * <p>Little Endian (e1 followed by e0)
 *
 * <p>x(f) x(e) x(d) x(c) x(b) x(a) x(9) x(8) x(7) x(6) x(5) x(4) x(3) eexxxxxx(2) eeeeeeee(1)
 * scccccee(0)
 *
 * <p>(n) logical byte position in hex s = sign bit c = combination bits e = exponent continuation x
 * = coefficient continuation (x(n) is 8 bits of coefficient continuation)
 *
 * <p>Value of this type is represented by a BigDecimal using MathContext.DECIMAL128 for rounding.
 */
final class Decimal128 extends DecimalSpl {
  /** Twelve exponent bits will be used from the encoded exponent as the continuation bits. */
  public static final int ENCODED_EXPONENT_BITS = 0xFFF;
  /**
   * The ENCODED_EXPONENT_BITS are shifted into the twelve bits following the sign bit and the 5
   * combination field bits. xxxxxxxx xxxxxxxx xxxxeeee eeeeeeee to scccccee eeeeeeee eexxxxxx
   * xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
   *
   * <p>Which is the same as the 64 bit shift less four (12 to 8 bits).
   */
  private static final int ENCODED_EXPONENT_SHIFT = Decimal64.ENCODED_EXPONENT_SHIFT - 4;

  /** Eight exponent continuation bits. */
  private static final long EXPONENT_CONTINUATION_BITS =
      ((long) ENCODED_EXPONENT_BITS) << ENCODED_EXPONENT_SHIFT;

  private final Decimal128Encoder encoder128;

  Decimal128() {
    super(MetaType.DECIMAL128, MathContext.DECIMAL128, 34, 6144);
    encoder128 =
        ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN
            ? new Decimal128EncoderBigEndian()
            : new Decimal128EncoderLittleEndian();
  }

  /**
   * Simple class to encode a decimal 128 value in endian order as two longs. Big & Little endian
   * implementations that write & read the longs in reverse order to each other.
   */
  private abstract class Decimal128Encoder {
    abstract void write(ByteBuffer bytes, long e0, long e1);

    abstract BigDecimal read(ByteBuffer bytes);
  }

  private final class Decimal128EncoderLittleEndian extends Decimal128Encoder {
    @Override
    void write(ByteBuffer bytes, long e0, long e1) {
      bytes.putLong(e1);
      bytes.putLong(e0);
    }

    @Override
    BigDecimal read(ByteBuffer bytes) {
      final long e1 = bytes.getLong();
      final long e0 = bytes.getLong();
      return decode(e0, e1);
    }
  }

  private final class Decimal128EncoderBigEndian extends Decimal128Encoder {
    @Override
    void write(ByteBuffer bytes, long e0, long e1) {
      bytes.putLong(e0);
      bytes.putLong(e1);
    }

    @Override
    BigDecimal read(ByteBuffer bytes) {
      final long e0 = bytes.getLong();
      final long e1 = bytes.getLong();
      return decode(e0, e1);
    }
  }

  /** Return a random set of digits with precision up to 34 . */
  @Override
  BigInteger _randomDigits(Random rand) {

    int digitCount = rand.nextInt(34) + 1;

    StringBuilder sb = new StringBuilder(digitCount);
    for (int i = 0; i < digitCount; i++) sb.append((char) ('0' + ((char) rand.nextInt(10))));

    BigInteger digits = new BigInteger(sb.toString());

    if (rand.nextBoolean()) digits = digits.negate();

    return digits;
  }

  @Override
  public int getSerializedSize() {
    return 16;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return encoder128.read(bytes);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {

    BigDecimal decimal = normalize((BigDecimal) value);
    boolean negative = decimal.signum() == -1;
    BigInteger digits = decimal.unscaledValue();
    if (negative) digits = digits.negate();

    boolean isZero = digits.compareTo(BigInteger.ZERO) == 0;

    int encodedExponent = (-decimal.scale()) + exponentBias;

    if (encodedExponent > maxEncodedExponent) {
      int delta = (encodedExponent - maxEncodedExponent);
      encodedExponent -= delta;
      for (int i = 0; i < delta; i++) digits = digits.multiply(BigInteger.TEN);
    }

    long e0 = negative ? Decimal64.NEGATIVE_BIT : 0;

    int coefficientMSD = isZero ? 0 : digits.divide(TEN_E33).intValue();

    e0 |= ((long) encodeCombinationField(coefficientMSD, encodedExponent >>> 6)) << (32);

    // Then the next 12 bits are the remainder of the encodedExponent
    // highest two bytes seecccEE EEEEEEEE EE
    e0 |= (((long) (encodedExponent & ENCODED_EXPONENT_BITS)) << ENCODED_EXPONENT_SHIFT);

    long e1;
    if (!isZero) {

      // Remove the coefficientMSD value.
      digits = digits.mod(TEN_E33);

      // 50 top bits (of 110) which is 5 sets of three digits.
      long e50bits = DenselyPackedDecimal.encodeNSets(digits.divide(TEN_E18).longValue(), 0, 5);

      // The bottom 4 bits of e50bits will be in the top 4 bits
      // of e1 and the remaining bits in the lowest bits of e0.
      // The lowest four bits are passed directly to the encoding
      // of the last 5 sets of digits.
      e0 |= (e50bits >>> 4L);

      // then the remaining 18 coefficient digits are encoded in groups of
      // three digits using DPD with 10 bits each.
      e1 =
          DenselyPackedDecimal.encodeNSets(
              digits.mod(TEN_E18).longValue(), (e50bits & 0x0FL) << 60, 6);
    } else {
      e1 = 0L;
    }

    encoder128.write(bytes, e0, e1);
  }

  BigDecimal decode(final long e0, long e1) {

    // For the combination bit and exponent bits the
    // code is almost identical to decimal64. The difference being
    // 12 versus 8 exponent continuation bits6
    boolean negative = (e0 & Decimal64.NEGATIVE_BIT) != 0;

    int combinationField = (int) ((e0 & Decimal64.COMBINATION_BITS) >>> (2 + 8 + 8 + 8 + 32));

    int exponentMSB = getExponentMSB(combinationField);
    int coefficientMSD = getCoefficientMSD(combinationField);

    // The encodedExponent includes a bias
    int exp =
        (exponentMSB << 12)
            + (int) ((e0 & EXPONENT_CONTINUATION_BITS) >>> ENCODED_EXPONENT_SHIFT)
            - exponentBias;

    // Total 110 bits of coefficient continuation split
    // across the two longs.

    // Top 15 digits (50 bits) in the 110 bits
    long _50bits = (e1 >>> 60L) | (e0 << 4L);
    long digts34_19 = DenselyPackedDecimal.decodeNSets((coefficientMSD * TEN_E15_L), _50bits, 5);

    // 60 bits of coefficient continuation
    // six sets of three digits
    long digits18_1 = DenselyPackedDecimal.decodeNSets(0, e1, 6);

    BigInteger bi33_19 = BigInteger.valueOf(digts34_19);
    BigInteger bi18_1 = BigInteger.valueOf(digits18_1);

    BigInteger biVal = bi33_19.multiply(TEN_E18).add(bi18_1);

    return newBigDecimal(negative, biVal, exp);
  }

  private static final BigInteger TEN_E33 = BigInteger.TEN.pow(33);

  private static final BigInteger TEN_E18 = BigInteger.TEN.pow(18);
  private static final long TEN_E15_L = BigInteger.TEN.pow(15).longValue();
}
