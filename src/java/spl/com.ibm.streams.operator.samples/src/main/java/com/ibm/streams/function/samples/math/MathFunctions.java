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

package com.ibm.streams.function.samples.math;

import com.ibm.streams.function.model.Function;
import com.ibm.streams.operator.model.Namespace;
import org.apache.commons.math.util.MathUtils;

/**
 * Sample mathematical SPL Java native functions. Functions that call static methods in {@code
 * java.lang.Math} or {@code org.apache.commons.math.util.MathUtils} and are annotated with {@link
 * Function} to expose them as SPL native functions. <br>
 * The SPL namespace of the functions is defined by the {@link Namespace} annotation applied to this
 * class's package {@link com.ibm.streams.function.samples.math}.
 */
public class MathFunctions {

  /** Only expose the static methods. */
  private MathFunctions() {}

  /**
   * Returns the hyperbolic cosine of a double value.
   *
   * @param v The value to calculate the hyperbolic cosine of.
   * @return The hyperbolic cosine of {@code v}.
   */
  @Function(description = "Returns the hyperbolic cosine of a `float64` value.")
  public static double cosh(double v) {
    return Math.cosh(v);
  }

  /**
   * Returns the hyperbolic sine of a double value.
   *
   * @param v The value to calculate the hyperbolic sine of.
   * @return The hyperbolic sine of {@code v}.
   */
  @Function(description = "Returns the hyperbolic sine of a `float64` value.")
  public static double sinh(double v) {
    return Math.sinh(v);
  }
  /**
   * Returns the hyperbolic tangent of a double value.
   *
   * @param v The value to calculate the hyperbolic tangent of.
   * @return The hyperbolic tangent of {@code v}.
   */
  @Function(description = "Returns the hyperbolic tangent of a `float64` value.")
  public static double tanh(double v) {
    return Math.tanh(v);
  }

  /**
   * Returns <code>&radic;(x<sup>2</sup> + y<sup>2</sup>)</code>.
   *
   * @param x a value
   * @param y a value
   * @return <code>&radic;(x<sup>2</sup> + y<sup>2</sup>)</code>
   */
  @Function(
      name = "hypotenuse",
      description = "Returns the square root of `x` squared plus `y` squared.")
  public static double hypot(double x, double y) {
    return Math.hypot(x, y);
  }

  /**
   * Returns {@code n!} ({@code n} factorial).
   *
   * @param n Value to calculate the factorial of.
   * @return {@code n!}
   */
  @Function(description = "Returns `n!` (`n` factorial).")
  public static long factorial(int n) {
    return MathUtils.factorial(n);
  }
}
