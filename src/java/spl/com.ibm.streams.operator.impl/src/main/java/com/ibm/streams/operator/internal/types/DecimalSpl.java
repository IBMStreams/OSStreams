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
import java.util.Arrays;
import java.util.Random;

abstract class DecimalSpl extends StatelessType<BigDecimal> {

  final MathContext context;
  final int maxExponent;
  final int precision;
  protected final int exponentBias;
  private final long randomFactor;
  final int maxEncodedExponent;

  protected final BigDecimal maxPositiveValue;
  protected final BigDecimal minPositiveValue;
  protected final BigDecimal maxNegativeValue;
  protected final BigDecimal minNegativeValue;

  DecimalSpl(MetaType metaType, MathContext context, int precision, int maxExponent) {
    super(metaType, BigDecimal.ZERO);
    this.context = context;
    this.precision = precision;
    this.maxExponent = maxExponent;
    this.exponentBias = maxExponent + precision - 2;
    maxEncodedExponent = 2 * maxExponent - 1;

    StringBuilder sb = new StringBuilder();
    char[] ca = new char[precision + 1];
    Arrays.fill(ca, '9');
    ca[1] = '.';
    sb.append(ca);
    sb.append('E');
    sb.append(maxExponent);

    maxPositiveValue = new BigDecimal(sb.toString());
    maxNegativeValue = maxPositiveValue.negate();

    int minExponent = 1 - maxExponent;
    Arrays.fill(ca, '0');
    ca[1] = '.';
    ca[ca.length - 1] = '1';
    sb.setLength(0);
    sb.append(ca);
    sb.append('E');
    sb.append(minExponent);
    minPositiveValue = new BigDecimal(sb.toString());
    minNegativeValue = minPositiveValue.negate();

    randomFactor = maxPositiveValue.unscaledValue().longValue();
  }

  /**
   * Normalize any incoming value which includes rounding to the given context and throwing an
   * exception if the value is out of range.
   */
  @Override
  public final BigDecimal convertFromObject(Object value) {

    try {
      return normalize((BigDecimal) value);
    } catch (ClassCastException e) {
      throw invalidConversion(value.getClass().getName());
    }
  }

  @Override
  public BigDecimal convertFrom(BigDecimal value) {
    return normalize(value);
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return (BigDecimal) object;
  }

  BigDecimal newBigDecimal(long digits, int exp) {
    BigInteger iv = BigInteger.valueOf(digits);
    return newBigDecimal(false, iv, exp);
  }

  BigDecimal newBigDecimal(boolean negate, BigInteger digits, int exp) {
    BigDecimal value = new BigDecimal(digits, -exp, context);
    if (negate) value = value.negate();
    return normalize(value);
  }

  final BigDecimal normalize(BigDecimal decimal) {
    decimal = decimal.round(context).stripTrailingZeros();

    checkRange(decimal);

    return decimal;
  }

  /** Return a random number in the range +/- 0.precisionE(1-maxE) to 9.precisionE(maxE) */
  @Override
  final BigDecimal _randomValue(Random rand) {

    BigDecimal v = new BigDecimal(_randomDigits(rand), precision - 1, context);

    int exp = rand.nextInt(maxExponent * 2) - maxExponent + 1;
    return normalize(v.scaleByPowerOfTen(exp));
  }

  /**
   * Return a random BigInteger with up to precision digits, the returned value may be negative.
   *
   * @param rand
   * @return
   */
  BigInteger _randomDigits(Random rand) {
    return BigInteger.valueOf(rand.nextLong() % randomFactor);
  }

  /**
   * Check that a decimal value is in the range for this type. The passed in value will have already
   * be normalized using getMathContext().
   *
   * @param decimal
   */
  private final void checkRange(BigDecimal decimal) {

    int sign = decimal.signum();
    if (sign == 1) {

      if (decimal.compareTo(maxPositiveValue) > 0)
        throw outOfRange(decimal + " > " + maxPositiveValue);

      if (decimal.compareTo(minPositiveValue) < 0)
        throw outOfRange(decimal + " < " + minPositiveValue);

    } else if (sign == -1) {
      if (decimal.compareTo(maxNegativeValue) < 0)
        throw outOfRange(decimal + " < " + maxNegativeValue);

      if (decimal.compareTo(minNegativeValue) > 0)
        throw outOfRange(decimal + " > " + minNegativeValue);
    }

    int encodedExponent = (-decimal.scale()) + exponentBias;
    if (encodedExponent < 0) throw outOfRange(decimal.toString());
  }

  private ArithmeticException outOfRange(String why) {
    throw new ArithmeticException(getLanguageType() + ": " + why);
  }

  /**
   * The 5 bit combination field has two formats. A) 11eec B) eeccc where ee is the exponent
   * component and c or ccc is the coefficientMSD value.
   *
   * <p>If the first two bits of the 5 bit combo field are 11, ie. 11xxx then the coefficient most
   * significant digit that the field contains is 8 or 9.
   */
  private static final int COMBO_FIELD_89_BITS = 0x18;

  /**
   * Decode the combo field into the exponentMSB.
   *
   * @param combinationField
   */
  protected static final int getExponentMSB(int combinationField) {
    if ((combinationField & COMBO_FIELD_89_BITS) == COMBO_FIELD_89_BITS) {
      // 11eec
      return (combinationField & 0x06) >> 1;
    } else {
      // eeccc
      return (combinationField & COMBO_FIELD_89_BITS) >> 3;
    }
  }
  /**
   * Decode the combo field into the CoefficientMSD.
   *
   * @param combinationField
   */
  protected static final int getCoefficientMSD(int combinationField) {
    if ((combinationField & COMBO_FIELD_89_BITS) == COMBO_FIELD_89_BITS) {
      // 11eec
      return 0x08 | (combinationField & 0x01);
    } else {
      // eeccc
      return combinationField & 0x07;
    }
  }

  /**
   * Return the 5 bit combination field encoded in a 32 bit integer as the first 5 bits after the
   * sign bit. This the correct location for a decimal32 and can be shifted left 32bits in a long
   * for a decimal64.
   */
  protected static final int encodeCombinationField(
      final int coefficientMSD, final int encodedExponent8bit) {
    int combinationField;
    if (coefficientMSD < 8) {
      // leading two bits of the exponent are shifted into
      // the two bits following the sign bit, equivalent to
      // shifting them right into the lower bits (-6) then shifting
      // them up by three bytes (3x+8) and then 5 bits in the
      // highest byte.
      // highest byte = sEEcccxx

      combinationField = ((encodedExponent8bit & 0xC0) << (-6 + 8 + 8 + 8 + 5));

      // Then the three bits of the coefficientMSD are encoded
      // in the next three bits.
      // highest byte = seeCCCxx
      combinationField |= coefficientMSD << (8 + 8 + 8 + 2);
    } else {
      combinationField = 0x60000000; // Top two bits of the combination bit are 11
      combinationField |= ((encodedExponent8bit & 0xC0) << (-6 + 8 + 8 + 8 + 3));
      if (coefficientMSD == 9) combinationField |= 0x04000000;
    }

    return combinationField;
  }
}
