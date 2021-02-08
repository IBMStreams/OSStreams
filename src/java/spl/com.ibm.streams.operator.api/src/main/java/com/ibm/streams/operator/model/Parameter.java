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

package com.ibm.streams.operator.model;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declare a relationship between an {@code Operator}'s Java bean property and an SPL operator
 * invocation parameter. This allows {@code Operator} implementations to simplify their {@code
 * initialize} method by having the SPL runtime automatically set field values using setter methods.
 *
 * <p>Any Java bean setter methods of an {@code Operator} implementation class or its super classes
 * annotated {@code @Parameter} are invoked by the SPL runtime when a matching SPL operator
 * invocation parameter is set. The operator invocation parameter name defaults to the Java bean
 * property's name, but may be set explicitly using {@link Parameter#name()}.
 *
 * <p>Setter methods are invoked before {@link
 * com.ibm.streams.operator.Operator#initialize(com.ibm.streams.operator.OperatorContext)
 * Operator.initialize} is invoked. Setter methods of the super-class are invoked first starting at
 * the root of the hierarchy. Invocation ordering of setter methods within a class definition is not
 * defined.
 *
 * <p>Supported mapping of SPL parameter types to Java bean property types are:
 *
 * <TABLE border="1" summary="This table describes the mapping between SPL parameter types and Java types for a Java bean setter method annotated with the Parameter annotation">
 * <CAPTION><B><I>SPL parameter type mapping</I></B></CAPTION>
 * <TR>
 * <TH scope="col">SPL Parameter Type</TH>
 * <TH scope="col">Parameter Cardinality</TH>
 * <TH scope="col">Java Property Types</TH>
 * </TR>
 *
 * <TR>
 * <TD rowspan="2">{@code boolean}</TD>
 * <TD>1</TD>
 * <TD>{@code boolean, Boolean}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code boolean[]}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code int8}</TD>
 * <TD>1</TD>
 * <TD>{@code byte, Byte}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code byte[]}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code int16}</TD>
 * <TD>1</TD>
 * <TD>{@code short, Short}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code short[]}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code int32}</TD>
 * <TD>1</TD>
 * <TD>{@code int, Integer}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code int[]}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code int64}</TD>
 * <TD>1</TD>
 * <TD>{@code long, Long}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code long[]}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code float32}</TD>
 * <TD>1</TD>
 * <TD>{@code float, Float}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code float[]}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code float64}</TD>
 * <TD>1</TD>
 * <TD>{@code double, Double}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code double[]}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code decimal32,decimal64,decimal128}</TD>
 * <TD>1</TD>
 * <TD>{@code java.math.BigDecimal}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code BigDecimal[], List<BigDecimal>}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code rstring,ustring}</TD>
 * <TD>1</TD>
 * <TD>{@code String}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code String[], List<String>}</TD>
 * </TR>
 * <TR>
 * <TD>{@code CustomLiteral}</TD>
 * <TD>1</TD>
 * <TD>{@code Enum}</TD>
 * </TR>
 * <TR>
 * <TD rowspan="2">{@code Attribute}</TD>
 * <TD>1</TD>
 * <TD>{@code TupleAttribute}</TD>
 * </TR>
 * <TR>
 * <TD>&gt; 1</TD>
 * <TD>{@code TupleAttribute[], List<TupleAttribute>}</TD>
 * </TR>
 * <TR>
 * </TABLE>
 *
 * Varargs methods are treated as if the method parameter was declared using the underlying array
 * type.
 *
 * <p><br>
 * If the SPL parameter is not the expected type then the value will be converted from its character
 * representation into the Java bean property's type. If this conversion fails, an exception will be
 * thrown, causing the operator and its processing element to terminate. <br>
 * Any instance of {@code List} passed to a setter method is read-only. <br>
 * All {@code rstring} parameters assume UTF-8 encoding.
 *
 * <p>For example, with these annotated Java setter methods
 *
 * <pre><code>
 * private String id;
 * private float threshold = 25.0;
 *
 * // Optional parameter threshold
 * &#64;Parameter
 * public void setThreshold(float threshold) {
 * this.threshold = threshold;
 * }
 * public float getThreshold() {
 * return threshold;
 * }
 *
 * // Mandatory parameter sensorTag mapping to the Java bean property id
 * &#64;Parameter(name="sensorTag", optional=false)
 * public void setId(String id) {
 * this.id = id;
 * }
 * public String getId() {
 * return id;
 * }
 *
 * </code></pre>
 *
 * <br>
 * this SPL operator invocation {@code param} section:
 *
 * <pre>
 * <code>
 * param
 * sensorTag: "295ul67";
 * threshold: 55.0;
 * </code>
 * </pre>
 *
 * would result in {@code setId("295ul67")} and {@code setThreshold(55.0)} being invoked before
 * {@code Operator.initialize}.
 *
 * <p>Java bean properties of the operator's class and its super-classes are identified using {@code
 * java.beans.BeanInfo.getPropertyDescriptors()} with the {@code BeanInfo} instance obtained using
 * {@code java.beans.Inspector}. The setter method is identified by the {@code getWriteMethod} of a
 * class's {@code BeanInfo}.
 *
 * <p>In InfoSphere Streams 4.0, parameters of type CustomLiteral and Attribute are now supported.
 * For example, the code snippet below shows a {@code CustomLiteral} parameter output with values
 * {text, binary}:
 *
 * <pre>
 * <code>
 * //enumeration type
 * public enum OutputType {
 * text,
 * binary
 * }
 * ...
 * private OutputType output;
 * ...
 * &#64;Parameter
 * public void setOutput(OutputType o) {
 * this.output = o;
 * }
 * </code>
 * </pre>
 *
 * <br>
 * Similarly, for passing an attribute as a parameter, the object bean property of {@link
 * com.ibm.streams.operator.TupleAttribute} type must be used. See example below:
 *
 * <pre>
 * <code>
 *
 * private TupleAttribute&lt;Tuple,Integer&gt; sortBy;
 * ...
 * &#64;Parameter
 * public void setSortBy(TupleAttribute&lt;Tuple,Integer&gt; s) {
 * this.sortBy = s;
 * }
 * </code>
 * </pre>
 *
 * To specify a default attribute to be used with an 'Attribute parameter, specify the attribute
 * name using the {@code com.ibm.streams.operator.model.DefaultAttribute} annotation.
 *
 * <p>If the SPL runtime throws an exception during automatic setting of properties prior to
 * initialization the processing element will terminate. <br>
 * An {@code IllegalArgumentException} is thrown if an operator invocation contains multiple values
 * for a parameter that maps to a single value Java bean property, e.g. a setter that takes an
 * {@code int} argument. <br>
 * An {@code IllegalStateException} is thrown if the Java Bean property's type is not supported.
 * <BR
 * An {@code IllegalStateException} is thrown if the corresponding
 * parameter definition in the Java primitive's operator model
 * has a different type, cardinality or optional settings.
 * <BR>
 * No action is taken for non-setter methods annotated @Parameter.
 *
 * <p>The method this annotation is applied to must be:
 *
 * <UL>
 *   <LI>{@code public}
 *   <LI>an instance method (not {@code static})
 *   <LI>return type of {@code void}
 *   <LI>have a single parameter of a supported type.
 * </UL>
 *
 * These restrictions are enforced at Java compilation time using annotation processing from
 * InfoSphere&reg; Streams Version 3.2 onwards.
 */
@Target({ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface Parameter {

  /**
   * Name of the operator invocation parameter. This must be a valid SPL identifier.
   *
   * @return The name of the operator invocation parameter or the empty string to indicate the
   *     operator parameter name matches the Java bean property name.
   */
  String name() default "";

  /**
   * Description for the parameter.
   *
   * @return Description for the parameter
   * @since InfoSphere&reg; Streams Version 3.2
   */
  String description() default "";

  /**
   * The maximum number of values accepted by the operator parameter. If the setter method's
   * parameter type is not an array or {@code List} then the cardinality cannot be specified, and
   * the operator parameter can only accept a single value. <br>
   * For array or {@code List} types the value supplied must be a value greater than one to limit
   * the number of values that are accepted by the operator parameter, or -1 to represent unbounded.
   * If not specified, the default is -1. Varargs methods are treated as if the method parameter was
   * declared using the underlying array type.
   *
   * @return Maximum number of values accepted by the parameter
   * @since InfoSphere&reg; Streams Version 3.2
   */
  int cardinality() default -1;

  /**
   * Indicate the setting of the property via an operator invocation parameter is optional.
   *
   * @return False if operator invocation parameter is required, an {@code IllegalStateException}
   *     will be raised if the parameter is not present, true if the parameter is optional.
   */
  boolean optional() default false;
}
