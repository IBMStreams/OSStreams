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

package com.ibm.streams.function.model;

import com.ibm.streams.operator.model.Namespace;
import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declare an SPL Java native function. A method annotated with {@code @Function} allows the method
 * to be called as a Java native function from SPL. The annotated method must be {@code public} and
 * {@code static}. The method may use classes from other libraries specified with the {@link
 * com.ibm.streams.toolkit.model.ToolkitLibraries} annotation.
 *
 * <p>A Java method annotated with {@code @Function} is automatically created as an SPL native
 * function during toolkit indexing. The resulting function that can be called from SPL:
 *
 * <UL>
 *   <LI>Has an SPL namespace defined according to {@link #namespace()}.
 *   <LI>Is a public function.
 *   <LI>Has an SPL name defined according to {@link #name()}.
 *   <LI>Has SPL parameter and return types according to the supported mapping.
 *   <LI>Has immutable parameters.
 *   <LI>Is stateful if {@link #stateful()} is true.
 * </UL>
 *
 * <p>Supported mapping of Java types to SPL types for native functions is:
 *
 * <TABLE border="1" summary="This table describes the mapping between Java method
 * parameter and return types and SPL types for SPL Java native functions">
 * <CAPTION><B><I>SPL Java native function type mapping</I></B></CAPTION>
 * <TR>
 * <TH scope="col">Java method parameter or return type</TH>
 * <TH scope="col">SPL Types</TH>
 * <TH scope="col">Notes</TH>
 * </TR>
 * <TR>
 * <TD>{@code void}</TD>
 * <TD>{@code void}</TD>
 * <TD>Return type only.</TD>
 * </TR>
 * <TR>
 * <TD>{@code boolean}</TD>
 * <TD>{@code boolean}</TD>
 * <TD></TD>
 * </TR>
 * <TR>
 * <TD>{@code byte}</TD>
 * <TD>{@code int8}, {@code uint8}</TD>
 * <TD rowspan="4">When the method has integral Java primitive parameter types
 * or arrays of integral primitive parameter types, then
 * multiple SPL functions are generated to support signed and unsigned integral SPL types.
 * Each resulting function prototype defines that all integral Java primitive types are mapped to either all
 * signed or all unsigned SPL types. For example all {@code int} and {@code long} parameters and
 * return type must map to {@code int32} and {@code int64}, respectively, or to {@code uint32} and {@code uint64}, respectively, and similarly for the corresponding list types, and a mix of
 * {@code int32} and {@code uint32} expressions is not allowed in a single
 * SPL Java function invocation.
 * When a Java integral primitive type is used only as the return type for
 * the method, and there are no integral Java primitive parameter types, then the SPL function prototype supports returning only
 * the signed SPL integral. For example a method that returns a {@code short}
 * and takes no arguments has a single SPL function prototype returning an {@code int16}.
 * When an unsigned integral SPL type is passed to, or returned from,
 * an SPL Java native function, it is mapped to a signed Java type,
 * and so the result might not be correct if a value exceeds the
 * maximum positive value supported by the signed Java type.
 * </TD>
 * </TR>
 * </TR>
 * <TR>
 * <TD>{@code short}</TD>
 * <TD>{@code int16}, {@code uint16}</TD>
 * </TR>
 * <TR>
 * <TD>{@code int}</TD>
 * <TD>{@code int32}, {@code uint32}</TD>
 * </TR>
 * <TR>
 * <TD>{@code long}</TD>
 * <TD>{@code int64}, {@code uint64}</TD>
 * </TR>
 * <TR>
 * <TD>{@code float}</TD>
 * <TD>{@code float32}</TD>
 * <TD></TD>
 * </TR>
 * <TR>
 * <TD>{@code double}</TD>
 * <TD>{@code float64}</TD>
 * <TD></TD>
 * </TR>
 * <TR>
 * <TD>{@code String}</TD>
 * <TD>{@code rstring}, {@code rstring[n]},
 * {@code ustring}</TD>
 * <TD>When the function has {@code String} or {@code String[]} parameters the prototype for the SPL function uses a single generic {@code <string T>}
 * to represent all {@code String} and {@code String[]} parameters and the return type if it is of type {@code String} or {@code String[]}.
 * If the method has a return type
 * of {@code String} or {@code String[]} without any {@code String} or {@code String[]} parameters then the SPL function has a return type of {@code rstring}, or {@code list<rstring>} (and {@code list<rstring>[m]} if there are other array parameters), respectively.</TD>
 * </TR>
 * <TR>
 * <TD>{@code boolean[]}</TD>
 * <TD>{@code list<boolean>}, {@code list<boolean>[m]}</TD>
 * <TD></TD>
 * </TR>
 * <TR>
 * <TD>{@code byte[]}</TD>
 * <TD>{@code list<int8>}, {@code list<int8>[m]}, {@code list<uint8>}, {@code list<uint8>[m]}</TD>
 * <TD rowspan="4">See above note for integral primitive types to see how
 * signed and unsigned SPL types are supported.</TD>
 * </TR>
 * <TR>
 * <TD>{@code short[]}</TD>
 * <TD>{@code list<int16>}, {@code list<int16>[m]}, {@code list<uint16>}, {@code list<uint16>[m]}</TD>
 * </TR>
 * <TR>
 * <TD>{@code int[]}</TD>
 * <TD>{@code list<int32>}, {@code list<int32>[m]}, {@code list<uint32>}, {@code list<uint32>[m]}</TD>
 * </TR>
 * <TR>
 * <TD>{@code long[]}</TD>
 * <TD>{@code list<int64>}, {@code list<int64>[m]}, {@code list<uint64>}, {@code list<uint64>[m]}</TD>
 * </TR>
 * <TR>
 * <TD>{@code float[]}</TD>
 * <TD>{@code list<float32>}, {@code list<float32>[m]}</TD>
 * <TD></TD>
 * </TR>
 * <TR>
 * <TD>{@code double[]}</TD>
 * <TD>{@code list<float64>}, {@code list<float64>[m]}</TD>
 * <TD></TD>
 * </TR>
 * <TR>
 * <TD>{@code String[]}</TD>
 * <TD>{@code list<rstring>}, {@code list<rstring>[m]}, {@code list<rstring[n]>}, {@code list<rstring[n]>[m]}, {@code list<ustring>}, {@code list<ustring>[m]}</TD>
 * <TD>See above note for string types.</TD>
 * </TR>
 * </TABLE>
 *
 * <p>When the Java method has {@code String} or array parameters, then the prototypes for the
 * automatically generated SPL native functions define that all bounded-length strings have the same
 * upper bound, and all bounded-length lists have the same upper bound. If the Java method returns
 * an array and does not have any array parameters, then the SPL function has a return type of an
 * unbounded array only.
 *
 * <p>Here's an example which allows the method to be called from SPL code as "res = sum(a, b)":
 *
 * <pre><code>
 * &#64;Function
 * public static int sum(int a, int b) {
 * return a + b;
 * }
 * </code></pre>
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Documented
@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.METHOD})
public @interface Function {
  /**
   * SPL namespace of the Java native function. The SPL namespace of the function is:
   *
   * <UL>
   *   <LI>The value of this {@code namespace} element of the annotation if it is explicitly
   *       present.
   *   <LI>Otherwise, the value of the {@link Namespace} annotation for the package of the annotated
   *       method, if it exists.
   *   <LI>Otherwise, the package name of the annotated method.
   * </UL>
   *
   * @return Namespace for the native function
   * @see Namespace
   */
  String namespace() default "";

  /**
   * Name of the function. If this {@code name} element is the default value (the empty string) then
   * the name of the function is the name of the annotated method. The name of the function must be
   * a valid SPL identifier.
   *
   * @return Name of the native function
   */
  String name() default "";

  /**
   * Declare the function as stateful or stateless. Functions are stateless by default. A stateful
   * function is a function that is not referentially transparent or has side-effects. A function is
   * not referentially transparent if it does not consistently yield the same result each time it is
   * called with the same inputs. A function has side-effects if it modifies state observable
   * outside the function.
   *
   * @return True if the function is stateful, false if it is stateless.
   */
  boolean stateful() default false;

  /**
   * Description for the native function.
   *
   * @return Description for the native function.
   */
  String description() default "";
}
