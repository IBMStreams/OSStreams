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
import org.apache.commons.math.complex.Complex;

public class Complex32 extends StatelessType<Complex> {

  Complex32() {
    super(MetaType.COMPLEX32, Complex.ZERO);
  }

  /**
   * Need range checks on the complex value coming in since the object representation is based upon
   * doubles and complex32 uses floats.
   */
  public Complex convertFromObject(Object value) {

    try {
      Complex complex = (Complex) value;

      checkValue(complex.getReal());
      checkValue(complex.getImaginary());

      return complex;
    } catch (ClassCastException e) {
      throw invalidConversion(value.getClass().getName());
    }
  }

  private static void checkValue(double in) {

    if ((float) in != in) throw new ArithmeticException(Double.toString(in)); // TODO
  }

  /**
   * Defined SPL format is (real value, imaginary value). Since this is a 32bit complex we reduce
   * the real and imaginary values to floats.
   */
  @Override
  public String getString(Object value) {
    return toString((Complex) value);
  }

  public static String toString(final Complex cv) {
    final StringBuilder sb = new StringBuilder(32);
    sb.append('(');
    sb.append((float) cv.getReal());
    sb.append(", ");
    sb.append((float) cv.getImaginary());
    sb.append(')');
    return sb.toString();
  }

  @Override
  Complex _randomValue(Random rand) {
    return new Complex(rand.nextFloat(), rand.nextFloat());
  }

  @Override
  public int getSerializedSize() {
    return 8;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    float r = bytes.getFloat();
    float i = bytes.getFloat();
    return new Complex(r, i);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    Complex complex = (Complex) value;
    bytes.putFloat((float) complex.getReal());
    bytes.putFloat((float) complex.getImaginary());
  }
}
