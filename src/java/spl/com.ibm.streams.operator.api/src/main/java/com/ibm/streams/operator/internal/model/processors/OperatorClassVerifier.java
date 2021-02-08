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

import com.ibm.streams.operator.Operator;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.TypeElement;

/** Validate that an annotation is attached to an operator class */
@SupportedAnnotationTypes({
  "com.ibm.streams.operator.model.PrimitiveOperator",
  "com.ibm.streams.operator.model.InputPorts",
  "com.ibm.streams.operator.model.OutputPorts",
  "com.ibm.streams.operator.model.InputPortSet",
  "com.ibm.streams.operator.model.OutputPortSet",
  "com.ibm.streams.operator.model.Icons",
  "com.ibm.streams.operator.model.Libraries"
})
public class OperatorClassVerifier extends AbstractModelProcessor {

  @Override
  protected void validateElement(TypeElement annotation, Element element) {
    assert element.getKind() == ElementKind.CLASS;

    validatePublic(annotation, element);
    validateOperatorClass(annotation, element);
  }

  protected void validateOperatorClass(TypeElement annotation, Element e) {
    if (!processingEnv
        .getTypeUtils()
        .isAssignable(e.asType(), getClassTypeMirror(Operator.class))) {
      simpleError(annotation, e, Key.OCV_OPERATOR);
    }
  }
}
