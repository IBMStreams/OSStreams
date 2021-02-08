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

public class Complex64 extends StatelessType<Complex> {

  Complex64() {
    super(MetaType.COMPLEX64, Complex.ZERO);
  }

  /** Defined SPL format is (real value, imaginary value) */
  @Override
  public String getString(Object value) {
    return toString((Complex) value);
  }

  public static String toString(final Complex cv) {
    final StringBuilder sb = new StringBuilder(32);
    sb.append('(');
    sb.append(cv.getReal());
    sb.append(", ");
    sb.append(cv.getImaginary());
    sb.append(')');
    return sb.toString();
  }

  @Override
  Complex _randomValue(Random rand) {
    return new Complex(rand.nextDouble(), rand.nextDouble());
  }

  @Override
  public int getSerializedSize() {
    return 16;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    double r = bytes.getDouble();
    double i = bytes.getDouble();
    return new Complex(r, i);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    Complex complex = (Complex) value;
    bytes.putDouble(complex.getReal());
    bytes.putDouble(complex.getImaginary());
  }
}
