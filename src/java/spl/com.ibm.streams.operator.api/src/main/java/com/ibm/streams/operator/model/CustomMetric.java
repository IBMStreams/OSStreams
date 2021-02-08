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

import com.ibm.streams.operator.management.MetricMXBean;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declare a relationship between an {@code Operator}'s Java bean property and custom metric. This
 * allows {@code Operator} implementations to simplify their {@code initialize} method by having the
 * SPL runtime automatically set field values using setter methods.
 *
 * <p>Any Java bean setter methods of an {@code Operator} implementation class or its super classes
 * annotated {@code @CustomMetric} are invoked by the SPL runtime passing the custom metric's {@link
 * Metric} reference. The custom metric's name defaults to the Java bean property's name, but may be
 * set explicitly using {@link CustomMetric#name()}.
 *
 * <p>Setter methods are invoked before {@link
 * com.ibm.streams.operator.Operator#initialize(com.ibm.streams.operator.OperatorContext)
 * Operator.initialize} is invoked. Invocation ordering of setter methods is not defined.
 *
 * <p>Java bean properties of the operator's class and its super-classes are identified using {@code
 * java.beans.BeanInfo.getPropertyDescriptors()} with the {@code BeanInfo} instance obtained using
 * {@code java.beans.Inspector}. The setter method is identified by the {@code getWriteMethod} of a
 * class's {@code BeanInfo}.
 *
 * <p>If the SPL runtime throws an exception during automatic setting of metrics prior to
 * initialization the processing element will terminate. <br>
 * An {@code IllegalStateException} is thrown if the Java Bean property's type is not {@link
 * Metric}. <br>
 * An {@code IllegalStateException} is thrown if the corresponding custom metric definition in the
 * Java primitive's operator model has a different {@link
 * com.ibm.streams.operator.metrics.Metric.Kind kind}. <br>
 * No action is taken for non-setter methods annotated @CustomMetric.
 *
 * <p>The method this annotation is applied to must be:
 *
 * <UL>
 *   <LI>{@code public}
 *   <LI>an instance method (not {@code static})
 *   <LI>return type of {@code void}
 *   <LI>have a single parameter of type {@link Metric}.
 * </UL>
 *
 * These restrictions are enforced at Java compilation time using annotation processing from
 * InfoSphere&reg; Streams Version 3.2 onwards.
 *
 * @see OperatorMetrics
 * @see Metric
 * @since InfoSphere&reg; Streams Version 3.1
 */
@Target({ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface CustomMetric {
  /**
   * Name of the custom metric
   *
   * @return The name of the custom metric or the empty string to indicate the custom metric name
   *     matches the Java bean property name.
   */
  String name() default "";

  /**
   * Description of the custom metric
   *
   * @return The description of the custom metric.
   */
  String description() default "";

  /** Kind of the custom metric. */
  Metric.Kind kind();

  /**
   * Register the custom metric as a {@link MetricMXBean} in the platform MBean server.
   *
   * @return True to register the custom metric at operator startup, false to not register.
   * @see OperatorContextMXBean#registerCustomMetric(String)
   */
  boolean mxbean() default false;
}
