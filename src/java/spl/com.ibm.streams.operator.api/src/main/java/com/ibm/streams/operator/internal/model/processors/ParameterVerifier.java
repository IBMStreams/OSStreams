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
import java.math.BigDecimal;
import java.util.List;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.AnnotationValue;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.ArrayType;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;
import javax.lang.model.type.WildcardType;

@SupportedAnnotationTypes({
  "com.ibm.streams.operator.model.Parameter",
})
public class ParameterVerifier extends AbstractModelProcessor {

  protected void validateElement(TypeElement annotation, Element e) {
    assert e.getKind() == ElementKind.METHOD;
    ExecutableElement method = (ExecutableElement) e;

    validatePublic(annotation, method);
    validateSetter(annotation, method);

    validateSetterParameter(annotation, method);

    validateSPLName(annotation, e, e.getAnnotation(Parameter.class).name());
    validateOptionalAttribute(annotation, method);
  }

  private void validateOptionalAttribute(TypeElement annotation, ExecutableElement method) {
    if (isDefaultAttributeAnnotationPresent(method)) {
      enforceSingleCardinality(annotation, method, Key.PV_DEFAULTATTRIBUTE_SINGLE);
      AnnotationMirror mirror = getAnnotationMirror(method, annotation);
      for (ExecutableElement key : mirror.getElementValues().keySet()) {
        if (key.getSimpleName().contentEquals("optional")) {

          AnnotationValue value = mirror.getElementValues().get(key);
          boolean isOptional = (Boolean) value.getValue();
          if (!isOptional) simpleError(annotation, method, Key.PV_OPTIONAL);
        }
      }
    }
  }

  private boolean isDefaultAttributeAnnotationPresent(ExecutableElement method) {
    DefaultAttribute defaultAttr = method.getAnnotation(DefaultAttribute.class);
    return defaultAttr != null;
  }

  private void enforceSingleCardinality(TypeElement annotation, ExecutableElement method) {
    enforceSingleCardinality(annotation, method, Key.PV_SINGLE);
  }

  private void enforceSingleCardinality(
      TypeElement annotation, ExecutableElement method, Key keyString) {
    AnnotationMirror mirror = getAnnotationMirror(method, annotation);
    for (ExecutableElement key : mirror.getElementValues().keySet()) {
      if (key.getSimpleName().contentEquals("cardinality")) {
        simpleError(annotation, method, keyString);
      }
    }
  }

  private void verifyMultiCardinality(TypeElement annotation, ExecutableElement method) {
    AnnotationMirror mirror = getAnnotationMirror(method, annotation);
    for (ExecutableElement key : mirror.getElementValues().keySet()) {
      if (key.getSimpleName().contentEquals("cardinality")) {

        AnnotationValue value = mirror.getElementValues().get(key);
        int cardinality = (Integer) value.getValue();
        if (cardinality == -1 || cardinality > 1) return;
        simpleError(annotation, method, Key.PV_CARDINALITY);
      }
    }
  }

  /** Ensure the setter's parameter is a type supported by @Parameter */
  private void validateSetterParameter(TypeElement annotation, ExecutableElement method) {

    VariableElement pve = method.getParameters().get(0);

    switch (pve.asType().getKind()) {
      case BOOLEAN:
      case BYTE:
      case SHORT:
      case INT:
      case LONG:
      case FLOAT:
      case DOUBLE:
        enforceSingleCardinality(annotation, method);
        return;
      case DECLARED:
        Element typeElem = ((DeclaredType) pve.asType()).asElement();
        if (typeElem.getKind() == ElementKind.ENUM
            || isTypeClassType(
                pve.asType(),
                String.class,
                BigDecimal.class,
                Boolean.class,
                Byte.class,
                Short.class,
                Integer.class,
                Long.class,
                Float.class,
                Double.class)) {
          enforceSingleCardinality(annotation, method);
          return;
        }

        if (typeElem.getKind() == ElementKind.INTERFACE && isAttribute(pve.asType())) {
          DeclaredType dType = ((DeclaredType) pve.asType());
          if (isValidAttributeType(dType)) {
            enforceSingleCardinality(annotation, method);
            return;
          }
        }

        TypeElement list = processingEnv.getElementUtils().getTypeElement("java.util.List");

        TypeMirror listString =
            processingEnv.getTypeUtils().getDeclaredType(list, getClassTypeMirror(String.class));
        if (processingEnv.getTypeUtils().isSameType(pve.asType(), listString)) {
          verifyMultiCardinality(annotation, method);
          return;
        }

        TypeMirror listDecimal =
            processingEnv
                .getTypeUtils()
                .getDeclaredType(list, getClassTypeMirror(BigDecimal.class));
        if (processingEnv.getTypeUtils().isSameType(pve.asType(), listDecimal)) {
          verifyMultiCardinality(annotation, method);
          return;
        }

        DeclaredType type = (DeclaredType) pve.asType();
        DeclaredType rawType =
            processingEnv.getTypeUtils().getDeclaredType((TypeElement) type.asElement());

        if (rawType.toString().equals("java.util.List")) {
          for (TypeMirror mirr : type.getTypeArguments()) {
            if (processingEnv.getTypeUtils().isAssignable(mirr, getTupleAttributeTypeMirror())) {
              DeclaredType dType = ((DeclaredType) mirr);
              if (isValidAttributeType(dType)) {
                verifyMultiCardinality(annotation, method);
                return;
              }
            }
          }
        }
        break;
      case ARRAY:
        {
          // Support for array types, not all singleton types are
          // supported, e.g. no arrays of primitive wrappers.
          ArrayType at = (ArrayType) pve.asType();
          switch (at.getComponentType().getKind()) {
            case BOOLEAN:
            case BYTE:
            case SHORT:
            case INT:
            case LONG:
            case FLOAT:
            case DOUBLE:
              verifyMultiCardinality(annotation, method);
              return;
            case DECLARED:
              if (isAttribute(at.getComponentType())
                  || isTypeClassType(at.getComponentType(), String.class, BigDecimal.class)) {
                verifyMultiCardinality(annotation, method);
                return;
              }
              break;
            default:
              break;
          }
          break;
        }

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

  private boolean isValidAttributeType(DeclaredType dType) {
    List<? extends TypeMirror> list = dType.getTypeArguments();
    if (list.size() == 2) {
      // first type is always Tuple. Check the second type
      TypeMirror type = list.get(1);
      if (isTypeClassType(
              type,
              String.class,
              BigDecimal.class,
              Boolean.class,
              Byte.class,
              Short.class,
              Integer.class,
              Long.class,
              Float.class,
              Double.class)
          || type instanceof WildcardType) {
        return true;
      }
    }
    return false;
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
