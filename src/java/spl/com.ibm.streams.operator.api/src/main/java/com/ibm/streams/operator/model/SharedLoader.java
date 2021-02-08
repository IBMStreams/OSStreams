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
import java.lang.annotation.Inherited;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declare if the class loader for a Java primitive operator is shared within an processing element.
 * <br>
 * Invocations of operators with classes annotated with {@code SharedLoader} with {@code value=true}
 * share a {@code ClassLoader} with other operator invocations with an identical set of library
 * dependencies. This is typically the same operator kind or operator kinds from the same toolkit,
 * that depend on the same set of jar files. <br>
 * Operator classes not annotated with {@code SharedLoader} or annotated with {@code SharedLoader}
 * with {@code value=false} result in each operator invocation have its own class loader. <br>
 *
 * <p>Operator classes must be annotated to have a shared class loader:
 *
 * <UL>
 *   <LI>If they load a dynamic library, see {@code Runtime.loadLibrary}.
 *   <LI>If they depend on a class or jar file that loads a dynamic library.
 * </UL>
 *
 * <br>
 * Using a shared class loader reduces overhead for a Java primitive operator when multiple
 * invocations are fused into a single processing element.
 *
 * <p>This annotation is inherited so if any parent class is annotated simply {@code @SharedLoader}
 * then the operator's invocations will use a shared class loader. A specific operator class can
 * force a class loader for each invocation by annotating itself {@code @SharedLoader(false)}. Note
 * the default without any annotation is to have a class loader for each invocation.
 *
 * @see Libraries
 * @since InfoSphere&reg; Streams Version 3.2.1
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
public @interface SharedLoader {
  /**
   * Share a class loader between invocations of the annotated operator class.
   *
   * @return True to shared a class loader across invocations. False to have a single class loader
   *     per invocation.
   */
  boolean value() default true;
}
