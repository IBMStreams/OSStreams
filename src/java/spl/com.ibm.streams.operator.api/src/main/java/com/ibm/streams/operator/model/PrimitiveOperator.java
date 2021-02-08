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
 * Declares a Java primitive operator. A class annotated with {@code PrimitiveOperator} annotation
 * will cause a new SPL Java primitive operator to be declared with the properties as described in
 * the annotation. The class must be {@code public} and implement the {@code Operator} interface.
 *
 * <p>A class annotated with the {@code PrimitiveOperator} annotation causes the operator model XML
 * file to be generated for the operator during toolkit indexing. The model file is generated based
 * on the attributes of this annotation as well other annotations in this package present on or
 * within the class. The resulting operator that can be called from SPL:
 *
 * <UL>
 *   <LI>Has a SPL namespace defined according to {@link #namespace()}.
 *   <LI>Has a SPL name defined according to {@link #name()}.
 * </UL>
 *
 * <p>Here is an example of how the annotation can be used to declare an operator called
 * "SampleOperator" within the "com.ent.samp" SPL namespace:
 *
 * <pre><code>
 * &#64;PrimitiveOperator(name="SampleOperator", namespace="com.ent.samp")
 * public class TestClass extends AbstractOperator {
 * ...
 * }
 * </code></pre>
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface PrimitiveOperator {
  /**
   * SPL namespace of the Java primitive operator. The SPL namespace of the operator is:
   *
   * <UL>
   *   <LI>The value of this {@code namespace} element of the annotation if it is explicitly
   *       present.
   *   <LI>Otherwise, the value of the {@link Namespace} annotation for the package of the annotated
   *       class, if it exists.
   *   <LI>Otherwise, the package name of the annotated class.
   * </UL>
   *
   * @return Namespace for the operator class
   * @see Namespace
   */
  String namespace() default "";

  /**
   * Name of the operator. If this {@code name} element is the default value (the empty string) then
   * the name of the operator is the name of the annotated class. The name of the class must be a
   * valid SPL identifier.
   *
   * @return Name of the operator
   */
  String name() default "";

  /**
   * A description for the operator. This is used by Streams Studio during content assist etc.
   *
   * @return Description of the operator
   */
  String description() default "";

  /**
   * A set of arguments to be passed to the Java VM within which this operator will be run.
   *
   * @return VM arguments for the operator
   */
  String[] vmArgs() default {};

  /**
   * A string to be added as a comment element to the operator model XML file that will be generated
   * at toolkit indexing time. The default value for this {@code comment} element is a blank string
   * which means that no comment will be generated into the XML file.
   *
   * @return Comment string for the operator model XML file
   */
  String comment() default "";
}
