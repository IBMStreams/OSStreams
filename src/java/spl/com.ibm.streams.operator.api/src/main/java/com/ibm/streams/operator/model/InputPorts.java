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
 * Declare a set of input port sets for an operator. The set of input ports are specified as an
 * array of {@code InputPortSet} annotations.
 *
 * <p>The order in which the {@code InputPortSet} annotations are specified is the order of the port
 * sets in the operator model XML file. If the operator consists of only one input port set then the
 * {@code InputPortSet} annotation can be used instead of the {@code InputPorts} annotation.
 * However, note that these annotations are mutually exclusive and cannot be used at the same time.
 *
 * <p>The annotation is also inherited from super classes unless the subclass specifies the same
 * annotation. <br>
 * For example, consider the snippet below: <br>
 *
 * <pre><code>
 * &#64;InputPorts({&#64;InputPortSet(...), &#64;InputPortSet(...)})
 * class BaseOperator extends AbstractOperator {...}
 *
 * &#64;PrimitiveOperator
 * public class MyOperator extends BaseOperator {...}
 * </code></pre>
 *
 * The {@code MyOperator} class will inherit the 2 input port sets that are specified using the
 * {@code InputPorts} annotation from the {@code BaseOperator} class. However, if {@code MyOperator}
 * specifies the same annotation then that would take effect instead of the one specified on {@code
 * BaseOperator}
 *
 * <p>If the {@code InputPorts} annotation is not specified or inherited on the operator class and
 * no {@code InputPortSet} annotation is present on the operator class, then the operator has no
 * input ports.
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
public @interface InputPorts {
  /**
   * A list of input port sets for an operator.
   *
   * @return A set of {@code InputPortSet} annotations
   */
  InputPortSet[] value();
}
