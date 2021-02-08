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

package com.ibm.streams.operator.internal.model;

import com.ibm.streams.operator.internal.model.processors.Util;
import com.ibm.streams.operator.model.Namespace;
import com.ibm.streams.operator.model.PrimitiveOperator;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;

public class ClassModel extends AbstractModel {

  private final String simpleBinaryName;
  private final String name;
  private final String packageName;
  private final boolean isOperatorClass;
  final String defaultNamespace;

  private final List<FieldModel> fields;
  private final List<MethodModel> methods;

  public ClassModel(
      ProcessingEnvironment processingEnv,
      PackageElement packageElement,
      TypeElement classElement,
      boolean isOperatorClass) {
    this.packageName = packageElement.getQualifiedName().toString();
    this.name = processingEnv.getElementUtils().getBinaryName(classElement).toString();
    this.simpleBinaryName = Util.getBinarySimpleName(processingEnv, classElement, packageElement);
    this.isOperatorClass = isOperatorClass;
    fields = new ArrayList<FieldModel>();
    methods = new ArrayList<MethodModel>();

    Namespace ns = packageElement.getAnnotation(Namespace.class);
    defaultNamespace = ns == null ? packageElement.getQualifiedName().toString() : ns.value();
  }

  public void addField(FieldModel field) {
    fields.add(field);
  }

  public void addMethod(MethodModel method) {
    methods.add(method);
  }

  @Override
  public void writeAsString(StringBuffer buffer) {
    if (packageName.length() > 0) {
      buffer.append("package ");
      buffer.append(packageName);
      buffer.append(";\n");
    }

    if (isOperatorClass) {
      buffer.append("import com.ibm.streams.operator.metrics.Metric.Kind;\n");
      buffer.append("import com.ibm.streams.operator.model.InputPortSet.WindowMode;\n");
      buffer.append(
          "import com.ibm.streams.operator.model.InputPortSet.WindowPunctuationInputMode;\n");
      buffer.append(
          "import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;\n");
    }

    super.writeAsString(buffer);
    buffer.append("\n");

    buffer.append("@com.ibm.streams.operator.internal.model.ShadowClass(\"");
    buffer.append(name);
    buffer.append("\")\n");
    buffer.append(
        "@javax.annotation.Generated(\"com.ibm.streams.operator.internal.model.processors.ShadowClassGenerator\")\n");
    buffer.append("public class ");
    buffer.append(simpleBinaryName);
    buffer.append(Util.GENERATED_CLASS_SUFFIX);
    if (isOperatorClass) buffer.append(" extends com.ibm.streams.operator.AbstractOperator\n");
    buffer.append(" {\n");

    for (FieldModel field : fields) {
      field.writeAsString(buffer);
      buffer.append("\n");
    }

    for (MethodModel method : methods) {
      method.writeAsString(buffer);
      buffer.append("\n");
    }

    buffer.append("}");
  }

  @Override
  protected Map<String, String> addAnnotationElements(AnnotationMirror anno) {

    final Map<String, String> additionalElements = new HashMap<String, String>();
    addDefaultNamespace(anno, PrimitiveOperator.class, additionalElements);

    return additionalElements;
  }

  final void addDefaultNamespace(
      AnnotationMirror anno, Class<?> annotationClass, Map<String, String> additionalElements) {
    if (isAnnotation(anno, annotationClass)) {
      // If the namespace is not provided set it to
      // the namespace of the @Namespace annotation
      // on the package if it exists.
      if (!containsElement(anno, "namespace")) {
        additionalElements.put("namespace", "\"" + defaultNamespace + "\"");
      }
    }
  }
}
