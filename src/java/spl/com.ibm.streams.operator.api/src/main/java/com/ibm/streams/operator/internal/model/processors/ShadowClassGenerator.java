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
import com.ibm.streams.operator.internal.model.AbstractModel;
import com.ibm.streams.operator.internal.model.ClassModel;
import com.ibm.streams.operator.internal.model.MethodModel;
import com.ibm.streams.operator.internal.model.MethodParameterModel;
import com.ibm.streams.operator.internal.model.ShadowClass;
import com.ibm.streams.operator.model.CustomMetric;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.Name;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.TypeKind;
import javax.tools.JavaFileObject;

@SupportedAnnotationTypes({
  "com.ibm.streams.operator.model.PrimitiveOperator",
  "com.ibm.streams.function.model.Function"
})
public class ShadowClassGenerator extends AbstractProcessor {

  private static final String DEFAULT_PARAM_NAME_PREFIX = "__p_";

  @Override
  public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {
    if (!roundEnv.processingOver()) {
      for (Element elem : roundEnv.getElementsAnnotatedWith(PrimitiveOperator.class)) {
        if (elem.getKind() == ElementKind.CLASS) {
          TypeElement classElement = (TypeElement) elem;
          generateShadowClass(classElement, true);
        }
      }
      ArrayList<TypeElement> classesAlreadyProcessed = new ArrayList<TypeElement>();
      for (Element elem : roundEnv.getElementsAnnotatedWith(Function.class)) {
        if (elem.getKind() == ElementKind.METHOD) {
          Element parentElem = elem.getEnclosingElement();

          // dont generate shadow class if parent also has PrimitiveOperator annotation since a
          // shadow for that will
          // already be generated
          if (parentElem.getKind() == ElementKind.CLASS
              && parentElem.getAnnotation(PrimitiveOperator.class) == null) {
            TypeElement classElement = (TypeElement) parentElem;
            if (!classesAlreadyProcessed.contains(classElement)) {
              classesAlreadyProcessed.add(classElement);
              generateShadowClass(classElement, false);
            }
          }
        }
      }
    }
    return false;
  }

  private void generateShadowClass(TypeElement classElement, boolean isOperatorClass) {
    if (classElement.getAnnotation(ShadowClass.class) == null) {
      PackageElement packageElement = processingEnv.getElementUtils().getPackageOf(classElement);

      // create class model
      ClassModel classModel =
          new ClassModel(processingEnv, packageElement, classElement, isOperatorClass);

      // add all class annotations
      addAnnotations(classModel, classElement);

      List<? extends Element> elements =
          processingEnv.getElementUtils().getAllMembers(classElement);
      for (Element childElem : elements) {
        if (childElem.getKind() == ElementKind.METHOD) {
          ExecutableElement method = (ExecutableElement) childElem;
          boolean isParameterOrMetricSetter =
              method.getAnnotation(Parameter.class) != null
                  || method.getAnnotation(CustomMetric.class) != null;
          boolean isFunction = method.getAnnotation(Function.class) != null;
          if (isParameterOrMetricSetter || isFunction) {
            String name = method.getSimpleName().toString();
            String returnType = method.getReturnType().toString();
            boolean isPrimitiveReturnType =
                (method.getReturnType().getKind() != TypeKind.DECLARED
                    && method.getReturnType().getKind() != TypeKind.ARRAY);
            boolean isStatic = method.getModifiers().contains(Modifier.STATIC);

            // if the method is static then only consider it for shadow class if the method is
            // declared in the class
            // itself. inherited static methods
            // are not to be considered.
            if (!isStatic || childElem.getEnclosingElement().equals(classElement)) {
              MethodModel methodModel =
                  new MethodModel(classModel, name, returnType, isPrimitiveReturnType, isStatic);
              classModel.addMethod(methodModel);

              int pi = 0;
              for (VariableElement param : method.getParameters()) {
                Name pn = param.getSimpleName();
                // if for some reason the parameter name is null then generate a name and use that
                String paramName =
                    (pn == null ? (DEFAULT_PARAM_NAME_PREFIX + pi++) : pn.toString());

                String type = param.asType().toString();
                MethodParameterModel paramModel = new MethodParameterModel(paramName, type);
                methodModel.addParameter(paramModel);
              }
              addAnnotations(methodModel, method);
            }
          }
        }
      }

      try {
        Name binaryName = processingEnv.getElementUtils().getBinaryName(classElement);
        JavaFileObject jfo =
            processingEnv
                .getFiler()
                .createSourceFile(
                    binaryName.toString() + Util.GENERATED_CLASS_SUFFIX, classElement);
        BufferedWriter wtr = new BufferedWriter(jfo.openWriter());
        StringBuffer buffer = new StringBuffer();
        classModel.writeAsString(buffer);
        wtr.append(buffer.toString());
        wtr.close();
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
  }

  private void addAnnotations(AbstractModel model, Element elem) {
    for (AnnotationMirror anno : processingEnv.getElementUtils().getAllAnnotationMirrors(elem)) {
      model.addAnnotation(anno);
    }
  }

  @Override
  public SourceVersion getSupportedSourceVersion() {
    return SourceVersion.latest();
  }
}
