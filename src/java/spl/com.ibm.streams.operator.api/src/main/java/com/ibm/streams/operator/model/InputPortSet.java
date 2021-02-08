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
 * Declare a single input port set. A port set is a fixed number of ports that share the same
 * configuration. This avoids repetition of the same configuration for different ports.
 *
 * <p>The default input set has the following attributes:
 *
 * <UL>
 *   <LI>Is required.
 *   <LI>Is an open input port set i.e. any number of input ports can be specified.
 *   <LI>Is not a control port.
 *   <LI>Is non windowed.
 * </UL>
 *
 * <p>If multiple input port sets need to specified, use the {@code InputPorts} annotation instead
 * of the {@code InputPortSet} annotation. Note that these annotations are mutually exclusive and
 * cannot be used at the same time. A unique identifier can be attached to each input port set using
 * the {@code InputPortSet#id()} element. This identifier can be used in other annotations that need
 * to refer to this input port set. For example, the {@code
 * OutputPortSet#windowPunctuationInputPort()} element's value is set to an input port set
 * identifier.
 *
 * <p>If the {@code InputPortSet} annotation is not specified on the operator class and no {@code
 * InputPorts} annotation is present on the class or inherited from a base class, then the operator
 * has no input ports.
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface InputPortSet {

  /** Enumeration listing the types of punctuation semantics that can be supported by a port. */
  public enum WindowPunctuationInputMode {
    /**
     * This port expects window punctuations in order for the operator to function correctly and
     * thus must be fed a punctuated stream.
     */
    Expecting,

    /**
     * This port does not require punctuations in order for the operator to work correctly and thus
     * has no restrictions on the connections that can be attached to it.
     */
    Oblivious,

    /**
     * This port is an Expecting port if it has a punctuation-based window, and an Oblivious port
     * otherwise.
     */
    WindowBound
  }

  /** Enumeration containing the types of windowing semantics supported by an operator. */
  public enum WindowMode {
    NonWindowed,
    Windowed,
    OptionallyWindowed
  }

  /**
   * A unique identifier for the input port set. This id can be used to refer to the input port set
   * in the {@code OutputPortSet.windowPunctuationInputPort} element.
   *
   * @return A unique identifier for the input port set
   */
  String id() default "";

  /**
   * Description of the port set.
   *
   * @return Description for the input port set
   */
  String description() default "";

  /**
   * The number of input ports allowed. If not specified, the default value of the {@code
   * cardinality} element is -1 to indicate unbounded. <br>
   * Note that only one input port set with cardinality set to -1 can be specified.
   *
   * @return Number of input ports allowed in this set
   */
  int cardinality() default -1;

  /**
   * Indicates whether this port set is optional. If not specified, the default value of the {@code
   * optional} element is false indicating that the port set is required.
   *
   * @return true if this port set is optional
   */
  boolean optional() default false;

  /**
   * Indicates whether this port set is a control port. If not specified, the default value of the
   * {@code controlPort} element is false indicating that this port set is not a control port
   *
   * @return true if this port set is a control port
   */
  boolean controlPort() default false;

  /**
   * Description of the supported windowing system.
   *
   * @return description of the supported windowing system
   */
  String windowingDescription() default "";

  /**
   * Declares the valid windowing configuration for the port set. One of the values in {@code
   * WindowMode} can be specified.
   *
   * @return Valid windowing configuration for the port set
   */
  WindowMode windowingMode() default WindowMode.NonWindowed;

  /**
   * Declares the punctuation semantics of the input port set. The valid options are one of values
   * specified in the {@code WindowPunctuationInputMode} enumeration.
   *
   * @return punctuation semantics of the input port set.
   */
  WindowPunctuationInputMode windowPunctuationInputMode() default
      WindowPunctuationInputMode.Oblivious;
}
