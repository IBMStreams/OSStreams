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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.Type.MetaType;
import java.beans.BeanInfo;
import java.beans.IntrospectionException;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import java.lang.annotation.Annotation;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

public class OperatorAnnotationsInspector {

  private final Class<? extends Operator> operatorClass;

  public OperatorAnnotationsInspector(Class<? extends Operator> operatorClass) {
    this.operatorClass = operatorClass;
  }

  public final Class<? extends Operator> getOperatorClass() {
    return operatorClass;
  }

  private PropertyDescriptor[] getBeanProperties() throws IntrospectionException {
    Class<?> stopClass =
        AbstractOperator.class.isAssignableFrom(getOperatorClass())
            ? AbstractOperator.class
            : Object.class;

    BeanInfo opInfo = Introspector.getBeanInfo(getOperatorClass(), stopClass);
    return opInfo.getPropertyDescriptors();
  }

  /**
   * Get the list of Java bean properties that have a setter method with a single parameter and
   * annotated with supplied annotation T.
   *
   * @throws IntrospectionException
   */
  public <T extends Annotation> List<PropertyDescriptor> getAnnotatedProperties(
      Class<T> annotationClass) throws IntrospectionException {
    final PropertyDescriptor[] opProperties = getBeanProperties();

    final List<PropertyDescriptor> annotatedProperties =
        new ArrayList<PropertyDescriptor>(opProperties.length);

    for (PropertyDescriptor property : opProperties) {

      // Can the property even be set?
      Method setter = property.getWriteMethod();
      if (setter == null) continue;

      if (setter.getParameterTypes().length != 1) continue;

      T annotation = setter.getAnnotation(annotationClass);

      if (annotation == null) continue;

      annotatedProperties.add(property);
    }
    return annotatedProperties;
  }

  public static MetaType getParameterSPLType(PropertyDescriptor property) {
    return ParameterSetter.getSPLType(property);
  }

  public static MetaType getParameterSPLType(Class<?> clz) {
    return ParameterSetter.getSPLType(clz);
  }
}
