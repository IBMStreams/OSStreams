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

/**
 * Annotations to define SPL Java native functions.
 *
 * <p>The typical steps to create an SPL Java native function in an SPL toolkit are:
 *
 * <OL>
 *   <LI>Annotate a method with the {@link com.ibm.streams.function.model.Function} annotation.
 *   <LI>Optionally, annotate the containing class with the {@link
 *       com.ibm.streams.toolkit.model.ToolkitLibraries} annotation.
 *   <LI>Compile the Java class containing the method.
 *   <LI>Package the class and its nested classes in a jar file in {@code lib} or {@code impl/lib}
 *       within the toolkit.
 *   <LI>Index the toolkit.
 * </OL>
 *
 * Details of these steps are covered in the following paragraphs.
 *
 * <p>Methods annotated with {@code @Function} define SPL Java native functions in an SPL toolkit.
 * The native functions are created when the toolkit is indexed, which occurs by running {@code
 * spl-make-toolkit}. Toolkit indexing may be explicit, by executing {@code spl-make-toolkit}, or
 * implicit through the SPL compiler {@code sc} or Streams Studio.
 *
 * <p>A Java method annotated with {@code @Function} defines one or more SPL native functions during
 * toolkit indexing if its class file is in one of these locations relative to the root directory of
 * the toolkit (the value passed as the {@code -i} argument to {@code spl-make-toolkit}):
 *
 * <UL>
 *   <LI>Contained in a jar file within {@code lib} or {@code impl/lib}. The jar file must have the
 *       suffix {@code .jar} or {@code .JAR}.
 *   <LI>The class directory {@code impl/java/bin} (in its correct package based sub-directory).
 * </UL>
 *
 * <p>The class must have been compiled with annotation processing enabled using the annotation
 * processors defined by the jar file {@code com.ibm.streams.operator.jar}. This annotation
 * processing occurs automatically when using a Java compiler or Streams Studio. <br>
 * The annotation processing creates a nested class for the class containing the annotated method,
 * with the class name addition being {@code $StreamsModel}. When creating a jar file containing
 * classes for annotated methods, these nested classes must be included. Toolkit indexing creates
 * SPL native function definitions from these nested classes. This is similar to including nested
 * classes created by anonymous inner classes. Typically creating a jar file includes all classes
 * within a package and thus no special action is required.
 *
 * <p>Toolkit indexing creates SPL artifacts under the {@code native.function} directory of the
 * directory for the SPL namespace of the native function. The namespace for a native function is
 * defined according to the {@link com.ibm.streams.function.model.Function#namespace()} element or
 * the package annotation {@link com.ibm.streams.operator.model.Namespace}. The generated artifacts
 * include:
 *
 * <UL>
 *   <LI>The model file {@code javaFunction.xml} for the SPL Java native functions in the namespace.
 *   <LI>A C++ header file containing code to call the Java methods from SPL
 * </UL>
 *
 * <EM>These generated artifacts are read-only and must not be modified.</EM> <br>
 * The {@code native.function} directory may also contain manually generated SPL C++ native
 * functions, represented by the function model file {@code function.xml}.
 *
 * <p>A single {@code ClassLoader} is created for each toolkit. It contains:
 *
 * <UL>
 *   <LI>Jar files in {@code lib} and {@code impl/lib} that contain a class with a method annotated
 *       with {@code @Function}.
 *   <LI>The class directory {@code impl/java/bin} if it contains a class with a method annotated
 *       with {@code @Function}.
 *   <LI>Jar files and class directories added with {@code @ToolkitLibraries}.
 * </UL>
 *
 * The order of jars and {@code impl/java/bin} within the class loader is not defined. <br>
 * Classes containing SPL Java native functions are loaded using this class loader. If additional
 * jars are required in the class search path, they can be specified by using one of the following
 * methods:
 *
 * <UL>
 *   <LI>Use the {@link com.ibm.streams.toolkit.model.ToolkitLibraries} annotation to annotate the
 *       class and specify the required paths.
 *   <LI>Package the class in a jar file where the jar file has a {@code Class-Path} manifest entry
 *       that references the required jars.
 * </UL>
 *
 * The required jars would typically be contained within the toolkit. For example third-party jars
 * may be under {@code opt} and a jar file in {@code lib} would reference them using a relative path
 * starting with {@code ../opt/}.
 *
 * <p>The <A href="../../../../../overview-summary.html#operatorjars">Java Operator API classes and
 * the dependent jars</A> are available to SPL Java native functions at runtime. <br>
 * Methods may use the SPL logging facilities of the Java Operator API, using Java platform logging
 * or Apache Log4j. <br>
 * Methods do not have access to the processing element or operator context.
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
package com.ibm.streams.function.model;
