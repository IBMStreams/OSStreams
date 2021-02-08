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

import com.ibm.streams.operator.model.Namespace;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.Set;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.RoundEnvironment;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic.Kind;

abstract class AbstractModelProcessor extends AbstractProcessor {

  @Override
  public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {

    for (TypeElement annotation : annotations) {

      for (Element e : roundEnv.getElementsAnnotatedWith(annotation)) {
        validateElement(annotation, e);
      }
    }
    return false;
  }

  protected abstract void validateElement(TypeElement annotation, Element e);

  protected void validateNonStatic(TypeElement annotation, Element e) {
    if (e.getModifiers().contains(Modifier.STATIC)) {
      simpleError(annotation, e, Key.AMP_INSTANCE);
    }
  }

  protected void validateStatic(TypeElement annotation, Element e) {
    if (!e.getModifiers().contains(Modifier.STATIC)) {
      simpleError(annotation, e, Key.AMP_STATIC);
    }
  }

  protected void validateVoid(TypeElement annotation, ExecutableElement e) {
    if (e.getReturnType().getKind() != TypeKind.VOID) {
      simpleError(annotation, e, Key.AMP_VOID);
    }
  }

  protected void validatePublic(TypeElement annotation, Element e) {
    if (!e.getModifiers().contains(Modifier.PUBLIC)) {
      simpleError(annotation, e, Key.AMP_PUBLIC);
    }
  }

  protected void validateNonAbstract(TypeElement annotation, Element e) {
    if (e.getModifiers().contains(Modifier.ABSTRACT)) {
      simpleError(annotation, e, Key.AMP_CONCRETE);
    }
  }
  /** Validate the method looks like a instance setter, void and taking a single argument. */
  protected void validateSetter(TypeElement annotation, ExecutableElement method) {
    validateNonStatic(annotation, method);
    validateVoid(annotation, method);
    if (method.getParameters().size() != 1) {
      simpleError(annotation, method, Key.AMP_SETTER);
    }
  }

  protected void simpleError(TypeElement annotation, Element e, Key msgKey) {
    error(annotation, e, msgKey, annotation.getSimpleName(), e.getSimpleName());
  }

  protected void error(TypeElement annotation, Element e, Key msgKey, Object... msgArguments) {
    if (msgArguments == null) {
      msgArguments = new Object[] {""};
    }
    final String msg = new Message(msgKey, msgArguments).getLocalizedMessageAndPrefix();
    processingEnv
        .getMessager()
        .printMessage(Kind.ERROR, msg, e, getAnnotationMirror(e, annotation));
  }
  /** Get the specific AnnotationMirror for an annotation. */
  static AnnotationMirror getAnnotationMirror(Element e, TypeElement annotation) {
    for (AnnotationMirror mirror : e.getAnnotationMirrors()) {
      if (mirror.getAnnotationType().asElement().equals(annotation)) return mirror;
    }
    return null;
  }

  protected boolean isAnnotationType(TypeElement annotation, Class<?> annotationClass) {
    return annotation.getQualifiedName().toString().equals(annotationClass.getCanonicalName());
  }

  protected boolean isTypeClassType(TypeMirror type, Class<?>... classTypes) {

    if (type.getKind() != TypeKind.DECLARED) return false;

    for (Class<?> classType : classTypes) {
      if (processingEnv.getTypeUtils().isSameType(type, getClassTypeMirror(classType))) {
        return true;
      }
    }
    return false;
  }

  protected TypeMirror getClassTypeMirror(Class<?> type) {
    return processingEnv.getElementUtils().getTypeElement(type.getCanonicalName()).asType();
  }

  /** Verify the SPL name is valid, taken from name or the element's simple name. */
  protected void validateSPLName(TypeElement annotation, Element element, String name) {
    if (name.length() == 0) name = element.getSimpleName().toString();

    if (!Util.isSPLIdentifier(name) || Util.isReservedDir(name) || Util.isRestrictedKeyword(name)) {
      simpleError(annotation, element, Key.AMP_SPLID);
    }
  }

  /**
   * Verify the SPL name is valid, taken from name, the Namespace annotation for the package or the
   * element's package name.
   */
  protected void validateDefinedSPLNamespace(
      TypeElement annotation, Element element, String namespace) {
    if (namespace.length() == 0) {

      PackageElement pkg = processingEnv.getElementUtils().getPackageOf(element);
      if (pkg.getAnnotation(Namespace.class) != null) {
        // namespace will be validated by NamespaceVerifier
        return;
      }

      namespace = pkg.getQualifiedName().toString();
    }

    validateSPLNamespace(annotation, element, namespace);
  }

  /** Validate the actual value of a namespace. */
  protected void validateSPLNamespace(TypeElement annotation, Element element, String namespace) {

    if (namespace != null && namespace.length() != 0) {
      String[] sections = namespace.split("\\.", -1); // $NON-NLS-1$
      for (String token : sections) {
        if (token.length() == 0
            || !Util.isSPLIdentifier(token)
            || Util.isReservedDir(token)
            || Util.isRestrictedKeyword(token)) {
          error(annotation, element, Key.AMP_SPLNS, annotation.getSimpleName(), namespace);
        }
      }
    }
  }

  @Override
  public SourceVersion getSupportedSourceVersion() {
    return SourceVersion.latest();
  }
}
