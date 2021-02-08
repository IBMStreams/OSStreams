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

import com.ibm.streams.function.model.Function;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.lang.model.element.AnnotationMirror;

public class MethodModel extends AbstractModel {

  private final ClassModel classModel;
  private final String name;
  private final String returnType;
  private final boolean isPrimitiveReturnType;
  private final List<MethodParameterModel> parameters;

  private final boolean isStatic;

  public MethodModel(
      ClassModel classModel,
      String name,
      String returnType,
      boolean isPrimitiveReturnType,
      boolean isStatic) {
    this.classModel = classModel;
    this.name = name;
    this.returnType = returnType;
    this.isPrimitiveReturnType = isPrimitiveReturnType;
    this.isStatic = isStatic;
    parameters = new ArrayList<MethodParameterModel>();
  }

  public void addParameter(MethodParameterModel param) {
    parameters.add(param);
  }

  @Override
  public void writeAsString(StringBuffer buffer) {
    super.writeAsString(buffer);
    buffer.append("\n");
    // Since parameter names are not stored in class files otherwise, store them in an internal
    // annotation instead
    buffer.append("@com.ibm.streams.operator.internal.model.MethodParameters({");
    for (int i = 0; i < parameters.size(); i++) {
      buffer.append("\"" + parameters.get(i).name + "\"");
      if (i < parameters.size() - 1) {
        buffer.append(", ");
      }
    }
    buffer.append("})\n");
    buffer.append("public ");
    if (isStatic) buffer.append("static ");
    buffer.append(returnType);
    buffer.append(" ");
    buffer.append(name);
    buffer.append("(");
    for (int i = 0; i < parameters.size(); i++) {
      MethodParameterModel param = parameters.get(i);
      param.writeAsString(buffer);
      if (i < parameters.size() - 1) {
        buffer.append(", ");
      }
    }
    buffer.append(") {");
    if (returnType.equals("void")) {
      buffer.append("}");
    } else if (isPrimitiveReturnType) {
      if (returnType.equals("boolean")) {
        buffer.append("return false; }");
      } else {
        buffer.append("return 0; }");
      }
    } else {
      buffer.append("return null; }");
    }
  }

  @Override
  protected Map<String, String> addAnnotationElements(AnnotationMirror anno) {

    final Map<String, String> additionalElements = new HashMap<String, String>();
    classModel.addDefaultNamespace(anno, Function.class, additionalElements);

    return additionalElements;
  }
}
