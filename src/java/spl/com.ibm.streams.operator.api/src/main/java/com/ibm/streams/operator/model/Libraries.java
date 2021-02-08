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

import com.ibm.streams.operator.OperatorContext;
import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Inherited;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declare a set of Java class libraries to be added to the operator's class loader at execution
 * time. A path may be one of:
 *
 * <UL>
 *   <LI>A valid URL referring to a jar file or directory
 *   <LI>An absolute file path
 *   <LI>A file path relative to the root of the SPL {@link OperatorContext#getToolkitDirectory()
 *       toolkit containing the primitive operator}
 * </UL>
 *
 * <p>A path may contain an environment variable delimited by {@code @} characters, e.g.
 * {@code @DERBY_INSTALL@/lib/derby.jar}. The environment variable is evaluated during SPL
 * compilation when the SPL application invokes a {@link PrimitiveOperator} with this annotation.
 * This means that the resultant path must be available on any resource (host) where the operator
 * will execute.
 *
 * <p>If a file path ends with "{@code /* }" then all the files in the specified directory with the
 * extension {@code .jar } or {@code .JAR } will be added to the operator's class loader. The
 * expansion occurs at execution time. <br>
 * Otherwise the file path is assumed to refer to a jar file or a directory.
 *
 * <p>Example of including all the jar files in {@code $DERBY_INSTALL/lib }.
 *
 * <pre>
 * <code>
 * &#64;PrimitiveOperator
 * &#64;Libraries({"&#64;DERBY_INSTALL&#64;/lib/*"})
 * public class DerbyOp extends AbstractOperator
 * </code>
 * </pre>
 *
 * <em>Note that this annotation can be inherited i.e. if specified on a class, this annotation will
 * be inherited by any subclasses unless the subclass provides its own version of the
 * annotation.</em>
 *
 * <p>The search order for classes for a Java primitive operator is:
 *
 * <OL>
 *   <LI>The classes provided by the virtual machine's Java runtime (Java 6 or Java 7).
 *   <LI>The classes provided by the SPL Java runtime, including:
 *       <UL>
 *         <LI>Java Operator samples and patterns - {@code com.ibm.streams.operator.samples.jar}
 *         <LI>Java Operator API - {@code com.ibm.streams.operator.jar}
 *         <LI>Apache Common Math 2.2
 *         <LI>Apache Logging 1.2
 *         <LI>IBM JSON for Java
 *       </UL>
 *   <LI>The jar or directory containing the operator class annotated with {@link
 *       PrimitiveOperator}, including any dependent classes from the jar's manifest {@code
 *       Class-Path} entry.
 *   <LI>The classes specified by the operator class's {@link Libraries} annotation. Order within
 *       {@link #value()} is preserved, with the path listed first being earliest in the search
 *       order.
 *   <LI>Any classes added by {@link OperatorContext#addClassLibraries(String[])}
 * </OL>
 *
 * When a path defined by {@code @Libraries} or {@link OperatorContext#addClassLibraries(String[])
 * addClassLibraries} ends with '{@code /*}' then all jar files in that directory are included in
 * the search order. The order within the set of jars loaded though this mechanism is not defined,
 * but they remain ordered with respect to the path that included them. For example: <code>
 *  @Libraries({"opt/a/*", "opt/b/*"})</code> will include all the jars in {@code opt/a} and {@code
 * opt/b}, and all jars in {@code opt/a} are earlier in the search path than those in {@code opt/b}.
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
public @interface Libraries {
  /**
   * The paths to be added to the operator's class loader at execution time to be used when
   * searching for referenced Java classes.
   */
  String[] value();
}
