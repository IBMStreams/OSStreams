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
 * Declare a single output port set. A port set is a fixed number of ports that share the same
 * configuration. This avoids repetition of the same configuration for different ports.
 *
 * <p>The default output set has the following attributes:
 *
 * <UL>
 *   <LI>Is required.
 *   <LI>Is an open output port set i.e. any number of output ports can be specified.
 *   <LI>Is free of window punctuations.
 * </UL>
 *
 * <p>If multiple output port sets need to specified, use the {@code OutputPorts} annotation instead
 * of the {@code OutputPortSet} annotation. Note that these annotations are mutually exclusive and
 * cannot be used at the same time.
 *
 * <p>If the {@code OutputPortSet#windowPunctuationOutputMode()} element is set to {@code
 * WindowPunctuationOutputMode#Preserving} then the {@code
 * OutputPortSet#windowPunctuationInputPort()} element must be specified if the operator has more
 * than one input port. The value of this element must be one of the {@code InputPortSet#id()}
 * element values from one of the {@code InputPortSet} annotations specified on the class or
 * inherited from the super class.
 *
 * <p>If the {@code OutputPortSet} annotation is not specified on the operator class and no {@code
 * OutputPorts} annotation is present on the class or inherited from a base class, then the operator
 * has no output ports.
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface OutputPortSet {

  /** Enumeration that lists the types of window punctuation semantics supported by an operator. */
  public enum WindowPunctuationOutputMode {
    /** Indicates that this port generates window punctuations. */
    Generating,

    /** Indicates that this port is free of window punctuations. */
    Free,

    /**
     * Indicates that this port preserves the received window punctuations. If an operator has more
     * than one input port, then the {@code OutputPortSet#windowPunctuationInputPort()} element must
     * be specified in order to identify which input port's punctuation is being preserved.
     */
    Preserving
  }

  /**
   * Description of the port.
   *
   * @return Description for the output port set
   */
  String description() default "";

  /**
   * The number of output ports allowed. If not specified, the default value of the {@code
   * cardinality} element is -1 to indicate unbounded. <br>
   * Note that only one output port set with cardinality set to -1 can be specified.
   *
   * @return Number of output ports allowed in this set
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
   * Declares the window punctuation semantics of the output port set. The options are one of the
   * values specified in the {@code WindowPunctuationOutputMode} enumeration.
   *
   * @return punctuation semantics of the output port set.
   */
  WindowPunctuationOutputMode windowPunctuationOutputMode() default
      WindowPunctuationOutputMode.Free;

  /**
   * Declares a punctuation input port set for a punctuation preserving output port set. The value
   * must be one of the {@code InputPortSet#id()} elements that are present on the class or one of
   * the super classes. This element must be specified under the following condition: <br>
   *
   * <UL>
   *   <LI>The {@code OutputPortSet#windowPunctuationOutputMode()} must be set to {@code
   *       WindowPunctuationOutputMode#Preserving}.
   *   <LI>There must be more than one input port sets specified on the operator.
   *   <LI>The input port set that is specified must have a cardinality of 1
   * </UL>
   *
   * @return The {@code InputPortSet#id()} element of an {@code InputPortSet} annotation.
   */
  String windowPunctuationInputPort() default "";
}
