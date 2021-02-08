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

import com.ibm.streams.function.model.Function;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.ArrayType;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;

@SupportedAnnotationTypes({
  "com.ibm.streams.function.model.Function",
})
public class FunctionVerifier extends AbstractModelProcessor {

  protected void validateElement(TypeElement annotation, Element e) {
    assert e.getKind() == ElementKind.METHOD;
    final ExecutableElement method = (ExecutableElement) e;

    validatePublic(annotation, method);
    validateStatic(annotation, method);

    // Don't support varags for functions even though they look
    // like array methods, as potentially in the future SPL
    // may support varags functions.
    if (method.isVarArgs()) simpleError(annotation, e, Key.FV_VARARGS);

    for (VariableElement pve : method.getParameters()) {
      validateFunctionParameter(annotation, method, pve);
    }

    // Validate the return type of the method if it is not void
    // (void is allowed).
    if (method.getReturnType().getKind() != TypeKind.VOID) {
      validateFunctionType(annotation, method, method.getReturnType());
    }

    final Function function = e.getAnnotation(Function.class);
    validateSPLName(annotation, e, function.name());
    validateDefinedSPLNamespace(annotation, e, function.namespace());
  }

  /** Ensure the function's parameters are valid types. */
  private void validateFunctionParameter(
      TypeElement annotation, ExecutableElement method, VariableElement pve) {
    validateFunctionType(annotation, method, pve.asType());
  }

  /**
   * Supported types for parameters and return are: boolean byte,short,int,long float,double String
   * array of above types
   */
  private void validateFunctionType(
      final TypeElement annotation, final ExecutableElement method, final TypeMirror type) {

    switch (type.getKind()) {
      case BOOLEAN:
      case BYTE:
      case SHORT:
      case INT:
      case LONG:
      case FLOAT:
      case DOUBLE:
        return;
      case DECLARED:
        if (isTypeClassType(type, String.class)) return;
        break;
      case ARRAY:
        {
          // Support for array types, not all singleton types are
          // supported, e.g. no arrays of primitive wrappers.
          ArrayType at = (ArrayType) type;
          switch (at.getComponentType().getKind()) {
            case BOOLEAN:
            case BYTE:
            case SHORT:
            case INT:
            case LONG:
            case FLOAT:
            case DOUBLE:
              return;
            case DECLARED:
              if (isTypeClassType(at.getComponentType(), String.class)) return;
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
        Key.FV_TYPE,
        annotation.getSimpleName(),
        method.getSimpleName(),
        type.toString());
  }
}
