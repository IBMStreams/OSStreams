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
 * Annotations to define an SPL Java primitive operator.
 *
 * <p>The typical steps to create a Java primitive operator model are:
 *
 * <OL>
 *   <LI>Annotate an operator class (i.e. a class implementing the {@link
 *       com.ibm.streams.operator.Operator} interface) with the annotations in this package. Each
 *       annotation's javadoc describes the usage in detail.
 *   <LI>Compile the Java operator class.
 *   <LI>Package the class and its nested classes in a jar file in {@code lib} or {@code impl/lib}
 *       within the toolkit.
 *   <LI>Index the toolkit.
 * </OL>
 *
 * Details of these steps are covered in the following paragraphs.
 *
 * <p>Annotations on or within the operator class define the model for the operator in an SPL
 * toolkit. The operator model XML file is created when the toolkit is indexed, which occurs by
 * running {@code spl-make-toolkit}. Toolkit indexing may be explicit, by executing {@code
 * spl-make-toolkit}, or implicit through the SPL compiler {@code sc} or Streams Studio.
 *
 * <p>Instead of being specified on the operator class itself, it is possible for some annotations
 * to be specified on a super class of the operator class. Such annotations are marked with the
 * {@link java.lang.annotation.Inherited} meta-annotation. These annotations will be inherited by
 * the operator class as if they were present on the operator class itself. If the same annotation
 * is specified on the operator class as well, the operator class annotation will take effect and
 * the super class version will be hidden.
 *
 * <p>For example, the {@link com.ibm.streams.operator.model.Libraries} annotation can be inherited.
 * In the code snippet below, the annotation on the super class will be inherited by the
 * SampleOperator operator class: <br>
 *
 * <pre><code>
 * &#64;Libraries("opt/lib1.jar")
 * public class BaseOperator extends AbstractOperator {
 * ...
 * }
 *
 * public class SampleOperator extends BaseOperator {
 * ...
 * }
 * </code></pre>
 *
 * <br>
 * However, in the code snippet below, the super class annotation will be hidden and the operator
 * class annotation will take effect leading to only the {@code opt/lib2.jar} library to be added to
 * the operator model XML file: <br>
 *
 * <pre><code>
 * &#64;Libraries("opt/lib1.jar")
 * public class BaseOperator extends AbstractOperator {
 * ...
 * }
 *
 * &#64;Libraries("opt/lib2.jar")
 * public class SampleOperator extends BaseOperator {
 * ...
 * }
 * </code></pre>
 *
 * <p>A Java operator class (i.e. a class that implements the {@link
 * com.ibm.streams.operator.Operator} interface) annotated with at least {@code @PrimitiveOperator}
 * will lead to the operator XML file to be generated during toolkit indexing if its class file is
 * in one of these locations relative to the root directory of the toolkit (the value passed as the
 * {@code -i} argument to {@code spl-make-toolkit}):
 *
 * <UL>
 *   <LI>The class directory {@code impl/java/bin} (in its correct package based sub-directory).
 *   <LI>Contained in a jar file within {@code impl/lib}. The jar file must have the suffix {@code
 *       .jar} or {@code .JAR}.
 *   <LI>Contained in a jar file within {@code lib}. The jar file must have the suffix {@code .jar}
 *       or {@code .JAR}.
 * </UL>
 *
 * <br>
 * If the operator class file is present in more than one location listed above then the first
 * instance of the class will be used for the generation of the operator XML file and subsequent
 * instances will be ignored.
 *
 * <p>The class must have been compiled with annotation processing enabled using the annotation
 * processors defined by the jar file {@code com.ibm.streams.operator.jar}. This annotation
 * processing occurs automatically when using a Java compiler or Streams Studio. <br>
 * The annotation processing creates a nested class for the operator class, with the class name and
 * suffix {@code $StreamsModel}. When creating a jar file containing operator classes, these nested
 * classes must be included. Toolkit indexing creates the operator model XML file from these nested
 * classes. This is similar to including nested classes created by anonymous inner classes.
 * Typically creating a jar file includes all classes within a package and thus no special action is
 * required.
 *
 * <p>Toolkit indexing creates the operator model XML file under the the directory for the SPL
 * namespace of the operator. The namespace for an operator is defined according to {@link
 * com.ibm.streams.operator.model.PrimitiveOperator#namespace()} element or the package annotation
 * {@link com.ibm.streams.operator.model.Namespace}. <EM>The generated operator model XML file is
 * read-only and must not be modified.</EM> <br>
 *
 * <p>A single {@code ClassLoader} is created for each operator, it contains:
 *
 * <UL>
 *   <LI>Jar files in {@code lib} and {@code impl/lib} that contain the operator class annotated
 *       with {@code @PrimitiveOperator}.
 *   <LI>The class directory {@code impl/java/bin} if it contains the operator class annotated with
 *       {@code @PrimitiveOperator}.
 *   <LI>The jars or locations specified using the {@code @Libraries} annotation.
 * </UL>
 *
 * <br>
 * Operator classes are loaded using this class loader. If additional jars are required in the class
 * path then it is recommended that the class is packaged in a jar file and the jar file has a
 * {@code Class-Path} manifest entry that references required jars. These required jars would
 * typically be contained within the toolkit. For example third-party jars may be under {@code opt}
 * and a jar file in {@code lib} would reference them using a relative path starting with {@code
 * ../opt/}. <br>
 * For any reason, if required jars cannot be specified using the {@code Class-Path} mechanism
 * mentioned above, the {@link com.ibm.streams.operator.model.Libraries} annotation on the operator
 * class can be used to specify libraries that are required. <br>
 *
 * <p>As the example, to define a model using annotations to describe a sample operator with the
 * following properties: <br>
 *
 * <UL>
 *   <LI>A parent SPL namespace - "com.ent.test"
 *   <LI>An operator specific VM argument that should be passed to the JVM at startup - "-Xmx128m"
 *   <LI>An input port and one output port. We will assume default values for the attributes of the
 *       ports for this example
 *   <LI>An SPL parameter - "interval" that takes an SPL int32 value
 *   <LI>A required 3rd party dependency - opt/dep.jar
 * </UL>
 *
 * The following code snippet describes this operator: <br>
 *
 * <pre><code>
 * &#64;PrimitiveOperator(namespace="com.ent.test", vmArgs="-Xmx128m")
 * &#64;InputPorts(@InputPortSet)
 * &#64;OutputPorts(@OutputPortSet)
 * &#64;Libraries("opt/dep.jar")
 * public class SampleOperator extends AbstractOperator{
 * private int interval;
 *
 * &#64;Parameter
 * public void setInterval(int intVal) {
 * this.interval = intVal;
 * }
 * }
 * </code></pre>
 */
package com.ibm.streams.operator.model;
