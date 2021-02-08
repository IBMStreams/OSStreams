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

package com.ibm.streams.toolkit.model;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Inherited;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declare a set of Java class library paths to be added at execution time to the toolkit's class
 * loader. The toolkit's class loader is used for loading SPL Java native functions declared with
 * the {@link com.ibm.streams.function.model.Function} annotation.
 *
 * <p>A path may be one of:
 *
 * <UL>
 *   <LI>A valid URL referring to a jar file or directory .
 *   <LI>An absolute file path.
 *   <LI>A file path relative to the root directory of the toolkit.
 * </UL>
 *
 * <p>Environment variables in the path are not supported.
 *
 * <p>If a file path ends with "{@code /* }" then all the files in the specified directory with the
 * extension {@code .jar } or {@code .JAR } will be added to the toolkit's class loader. The
 * expansion occurs at execution time. <br>
 * Otherwise the file path is assumed to refer to a jar file or a directory.
 *
 * <p>Example of including all the jar files in the {@code lib } subdirectory of the toolkit.
 *
 * <pre>
 * <code>
 * &#64;ToolkitLibraries({"lib/*"})
 * public class DerbyFunctions
 * </code>
 * </pre>
 *
 * <em>Note that this annotation can be inherited i.e. if specified on a class, this annotation will
 * be inherited by any subclasses unless the subclass provides its own version of the
 * annotation.</em>
 *
 * <p>The search order for classes referenced from an SPL Java native function is:
 *
 * <OL>
 *   <LI>The classes provided by the virtual machine's Java runtime.
 *   <LI>The <A href="../../../../../overview-summary.html#operatorjars">Java Operator API classes
 *       and the dependent jars</A> provided by the SPL Java runtime.
 *   <LI>The jar or directory containing the method annotated with {@link
 *       com.ibm.streams.function.model.Function}, including any dependent classes from the jar's
 *       manifest {@code Class-Path} entry.
 *   <LI>The classes specified by the containing class's {@code @ToolkitLibraries} annotation. Order
 *       within {@link #value()} is preserved, with the path listed first being earliest in the
 *       search order.
 * </OL>
 *
 * <p>When a path defined by {@code @ToolkitLibraries} ends with '{@code /*}' then all jar files in
 * that directory are included in the search order. The order within the set of jars loaded though
 * this mechanism is not defined, but they remain ordered with respect to the path that included
 * them. For example: <code>
 *  @ToolkitLibraries({"opt/a/*", "opt/b/*"})</code> will include all the jars in {@code opt/a} and
 * {@code opt/b}, and all jars in {@code opt/a} are earlier in the search path than those in {@code
 * opt/b}.
 *
 * <p>If a Java class library is to be used by the application, the library path must be accessible
 * at runtime. If the library is not in one of the directories included by default in the
 * application bundle, the library can be included in the bundle by specifying a {@code <sabfiles>}
 * element hierachy in the toolkit's {@code info.xml} file that includes the library path.
 *
 * @since IBM Streams Version 4.2
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
public @interface ToolkitLibraries {
  /**
   * The paths to be added to the toolkit's class loader at execution time to be used when searching
   * for referenced Java classes.
   */
  String[] value();
}
