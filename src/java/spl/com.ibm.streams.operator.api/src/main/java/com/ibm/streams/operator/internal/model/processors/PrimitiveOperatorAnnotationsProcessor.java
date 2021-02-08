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

package com.ibm.streams.operator.internal.model.processors;

import com.ibm.streams.operator.internal.model.ShadowClass;
import com.ibm.streams.operator.model.PrimitiveOperator;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.TypeElement;

/**
 * Validate to make sure that the \@PrimitiveOperator is valid and anny attributes that are
 * specified are valid.
 */
@SupportedAnnotationTypes("com.ibm.streams.operator.model.PrimitiveOperator")
public class PrimitiveOperatorAnnotationsProcessor extends AbstractModelProcessor {

  @Override
  protected void validateElement(TypeElement annotation, Element element) {
    assert element.getKind() == ElementKind.CLASS;

    if (element.getAnnotation(ShadowClass.class) == null) {
      validatePublic(annotation, element);
      validateNonAbstract(annotation, element);
      // if this is a nested class, then it must be static
      if (element.getEnclosingElement().getKind() != ElementKind.PACKAGE) {
        validateStatic(annotation, element);
      }

      PrimitiveOperator oper = element.getAnnotation(PrimitiveOperator.class);
      validateSPLName(annotation, element, oper.name());
      validateDefinedSPLNamespace(annotation, element, oper.namespace());
    }
  }
}
