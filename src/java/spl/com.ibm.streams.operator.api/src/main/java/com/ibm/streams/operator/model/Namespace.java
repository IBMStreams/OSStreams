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

import com.ibm.streams.function.model.Function;
import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declares a default SPL namespace for SPL artifacts in the annotated package. SPL artifacts that
 * are declared by these annotations:
 *
 * <UL>
 *   <LI>{@link Function} for SPL Java native functions
 *   <LI>{@link PrimitiveOperator} for SPL Java primitive operators
 * </UL>
 *
 * <br>
 * The defined SPL namespace for an SPL artifact will be:
 *
 * <UL>
 *   <LI>The value of the {@code namespace} element of the annotation ({@link
 *       PrimitiveOperator#namespace()} or {@link Function#namespace()}) if it is explicitly
 *       present.
 *   <LI>Otherwise, the value of the {@code Namespace} annotation for the package of the annotated
 *       class or method, if it exists.
 *   <LI>Otherwise, the package name of the annotated class or method.
 * </UL>
 *
 * <p>Any generated SPL artifact must define a valid SPL namespace. An SPL namespace is a dot
 * separated list of valid SPL identifiers. <br>
 * This annotation allows a single definition of a valid SPL namespace for a package when the Java
 * package name is not valid in SPL. This can occur if elements of the package name are SPL keywords
 * or include non-ASCII characters.
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Target({ElementType.PACKAGE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface Namespace {
  String value();
}
