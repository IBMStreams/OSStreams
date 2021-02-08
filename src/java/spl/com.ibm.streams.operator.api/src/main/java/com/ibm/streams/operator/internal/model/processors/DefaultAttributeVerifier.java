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

import com.ibm.streams.operator.model.DefaultAttribute;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;

@SupportedAnnotationTypes({
  "com.ibm.streams.operator.model.DefaultAttribute",
})
public class DefaultAttributeVerifier extends AbstractModelProcessor {

  @Override
  protected void validateElement(TypeElement annotation, Element e) {
    assert e.getKind() == ElementKind.METHOD;
    if (e.getAnnotation(Parameter.class) == null) {
      simpleError(annotation, e, Key.PV_DEFAULTATTRIBUTE_NOPARAM);
      return;
    }
    ExecutableElement method = (ExecutableElement) e;
    validateSetterParameter(annotation, method);
    validateAttributeName(annotation, method);
  }

  private void validateAttributeName(TypeElement annotation, ExecutableElement method) {
    String value = method.getAnnotation(DefaultAttribute.class).value();
    if (value.length() == 0 || !Util.isSPLIdentifier(value) || Util.isRestrictedKeyword(value)) {
      error(
          annotation,
          method,
          Key.PV_DEFAULTATTRIBUTE_INVALIDVALUE,
          annotation.getSimpleName(),
          value);
    }
  }

  /** Ensure the setter's parameter is a type supported by @Parameter */
  private void validateSetterParameter(TypeElement annotation, ExecutableElement method) {

    VariableElement pve = method.getParameters().get(0);

    switch (pve.asType().getKind()) {
      case DECLARED:
        Element typeElem = ((DeclaredType) pve.asType()).asElement();

        if (typeElem.getKind() == ElementKind.INTERFACE && isAttribute(pve.asType())) {
          return;
        }
        break;

      default:
        break;
    }
    error(
        annotation,
        method,
        Key.FV_TYPE, // yes, same as function verifier error
        annotation.getSimpleName(),
        method.getSimpleName(),
        pve.asType().toString());
  }

  private boolean isAttribute(TypeMirror type) {
    DeclaredType infoElemWildCard = getTupleAttributeTypeMirror();
    if (infoElemWildCard != null)
      return processingEnv.getTypeUtils().isAssignable(type, infoElemWildCard);
    return false;
  }

  private DeclaredType getTupleAttributeTypeMirror() {
    TypeElement infoElement =
        processingEnv.getElementUtils().getTypeElement("com.ibm.streams.operator.TupleAttribute");
    TypeMirror wildCard = processingEnv.getTypeUtils().getWildcardType(null, null);
    DeclaredType infoElemWildCard =
        processingEnv.getTypeUtils().getDeclaredType(infoElement, wildCard, wildCard);
    return infoElemWildCard;
  }
}
