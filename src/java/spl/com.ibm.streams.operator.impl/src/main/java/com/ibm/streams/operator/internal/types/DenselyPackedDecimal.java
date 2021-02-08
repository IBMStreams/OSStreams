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

public class DenselyPackedDecimal {

  static final int COEFFICIENT_COMBINATION_BITS = 0x000003FF;
  static final int COEFFICIENT_COMBINATION_SHIFT = 10;

  /*
   * 10 Bits are laid out as abcdefXghi where X is one of the format
   * indicators.
   */

  // Bitmasks to get the specific bit out.
  private static final int A = 0x200;
  private static final int B = 0x100;
  private static final int C = 0x80;
  private static final int D = 0x40;
  private static final int E = 0x20;
  private static final int F = 0x10;
  private static final int G = 0x04;
  private static final int H = 0x02;
  private static final int I = 0x01;

  private static final int X = 0x08;

  // Shifts to get C,F as the 2^0 bit
  private static final int C_S = 7;
  private static final int F_S = 4;

  // Shifts to get B,E as the 2^1 bit
  private static final int B1_S = C_S;
  private static final int E1_S = F_S;

  private static final int ABC = A | B | C;
  private static final int DEF = D | E | F;
  private static final int GHI = G | H | I;
  private static final int AB = A | B;
  private static final int DE = D | E;

  private static int[] digitPackLookup = new int[1000];

  static {
    for (int z = 0; z <= (A | B | C | D | E | F | G | H | I | X); z++) {

      if ((z & (D | E | X | G | H)) == (D | E | X | G | H)) {
        if ((z & (A | B)) != 0) continue;
      }
      int digits = unpack(z);
      digitPackLookup[digits] = z;
    }
  }

  static int pack(long digits) {
    return digitPackLookup[(int) (digits % 1000L)];
  }

  static int pack(int digits) {
    return digitPackLookup[digits % 1000];
  }

  static int unpack(int bits) {
    int d0, d1, d2;
    switch (bits & (X | G | H)) {
      default: // abcdef0ghi
        d2 = (bits & ABC) >> C_S;
        d1 = (bits & DEF) >> F_S;
        d0 = bits & GHI;
        break;
      case 8: // abcdef100i
        d2 = (bits & ABC) >> C_S;
        d1 = (bits & DEF) >> F_S;
        d0 = 0x08 | (bits & I);
        break;
      case 10: // abcdef101i
        d2 = (bits & ABC) >> C_S;
        d1 = 0x08 | ((bits & F) >> F_S); // 100f
        d0 = ((bits & DE) >> E1_S) | (bits & I);
        break;
      case 12: // abcdef110i
        d2 = 0x08 | ((bits & C) >> 7); // 100c
        d1 = (bits & DEF) >> F_S;
        d0 = ((bits & AB) >> B1_S) | (bits & I);
        break;
      case 14: // abcdef111i
        switch (bits & 0x60) {
          case 0x40: // abc10f111i
            d2 = (bits & ABC) >> C_S;
            d1 = 0x08 | ((bits & F) >> F_S);
            d0 = 0x08 | (bits & I);
            break;
          case 0x20: // abc01f111i
            d2 = 0x08 | ((bits & C) >> C_S); // 100c
            d1 = ((bits & AB) >> B1_S) | ((bits & F) >> F_S); // 0abf
            d0 = 0x08 | (bits & I);
            break;
          case 0x00: // abc00f111i
            d2 = 0x08 | ((bits & C) >> C_S);
            d1 = 0x08 | ((bits & F) >> F_S);
            d0 = ((bits & AB) >> B1_S) | (bits & I);
            break;
          default: // // xxc11f111i
            d2 = 0x08 | ((bits & C) >> C_S); // 100c
            d1 = 0x08 | ((bits & F) >> F_S); // 100f
            d0 = 0x08 | (bits & I);
            break;
        }
        break;
    }

    return (d2 * 100) + (d1 * 10) + d0;
  }

  /**
   * Decode N sets of three digits, each set is 10 bits comprising three densely packed digits.
   *
   * @param digits Initial value of digits
   * @param enc Encoding
   * @param n Number of sets to decode.
   * @return Numeric value of the digits.
   */
  static long decodeNSets(long digits, long enc, final int n) {

    long multiplier = 1L;
    for (int i = 0; i < n; i++) {

      int cabc =
          DenselyPackedDecimal.unpack(
              (int) (enc & DenselyPackedDecimal.COEFFICIENT_COMBINATION_BITS));

      digits += (cabc * multiplier);

      enc >>>= DenselyPackedDecimal.COEFFICIENT_COMBINATION_SHIFT;
      multiplier *= 1000L;
    }

    return digits;
  }

  /** Decode 2 sets, using int arithmetic. */
  static int decode2Sets(int digits, long enc) {

    int multiplier = 1;
    for (int i = 0; i < 2; i++) {

      int cabc =
          DenselyPackedDecimal.unpack(
              (int) (enc & DenselyPackedDecimal.COEFFICIENT_COMBINATION_BITS));

      digits += (cabc * multiplier);

      enc >>>= DenselyPackedDecimal.COEFFICIENT_COMBINATION_SHIFT;
      multiplier *= 1000;
    }

    return digits;
  }

  static long encodeNSets(long digits, long enc, final int n) {
    for (int i = 0; i < n; i++) {
      if (digits == 0L) break;

      enc |=
          ((long) DenselyPackedDecimal.pack(digits))
              << (i * DenselyPackedDecimal.COEFFICIENT_COMBINATION_SHIFT);

      digits /= 1000L;
    }
    return enc;
  }
}
