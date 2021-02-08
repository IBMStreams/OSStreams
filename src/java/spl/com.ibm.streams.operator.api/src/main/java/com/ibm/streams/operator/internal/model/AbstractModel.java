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

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.AnnotationValue;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.VariableElement;

public abstract class AbstractModel {

  private final List<AnnotationMirror> annotations;

  public AbstractModel() {
    annotations = new ArrayList<AnnotationMirror>();
  }

  public void addAnnotation(AnnotationMirror anno) {
    annotations.add(anno);
  }

  public void writeAsString(StringBuffer buffer) {
    for (AnnotationMirror anno : annotations) {
      buffer.append("\n");
      buffer.append(getStringRepresentation(anno));
    }
  }

  private String getStringRepresentation(AnnotationMirror anno) {
    StringBuffer buffer = new StringBuffer();
    buffer.append("@");
    buffer.append(anno.getAnnotationType().toString());
    buffer.append("(");
    Map<? extends ExecutableElement, ? extends AnnotationValue> elements = anno.getElementValues();

    // Add additional elements to replace those not explicitly set.
    final Map<String, String> additionalElements = addAnnotationElements(anno);
    for (Iterator<String> it = additionalElements.keySet().iterator(); it.hasNext(); ) {
      String elementName = it.next();
      String elementValue = additionalElements.get(elementName);
      buffer.append(elementName);
      buffer.append('=');
      buffer.append(elementValue);
      if (it.hasNext() || !elements.isEmpty()) buffer.append(", ");
    }

    Iterator<? extends ExecutableElement> keys = elements.keySet().iterator();
    while (keys.hasNext()) {
      ExecutableElement e = keys.next();
      AnnotationValue val = elements.get(e);
      getStringRepresentation(buffer, e, val, keys.hasNext());
    }

    buffer.append(")");
    return buffer.toString();
  }

  /**
   * Allow sub-classes to add additional elements to the string representation of an annotation.
   * This is used to provide values for elements where the default needs to be overridden. Any
   * element added must not be in anno.getElementValues(). This implementation does nothing.
   */
  protected Map<String, String> addAnnotationElements(AnnotationMirror anno) {
    return Collections.emptyMap();
  }

  static boolean isAnnotation(AnnotationMirror anno, Class<?> annClass) {
    return anno.getAnnotationType().toString().equals(annClass.getCanonicalName());
  }
  /** Does the declared annotation contain an specific element. */
  protected final boolean containsElement(AnnotationMirror anno, String name) {
    final Map<? extends ExecutableElement, ? extends AnnotationValue> elements =
        anno.getElementValues();
    if (elements.isEmpty()) return false;

    for (ExecutableElement ee : elements.keySet()) {
      if (ee.getSimpleName().toString().equals(name)) return true;
    }
    return false;
  }

  private void getStringRepresentation(
      StringBuffer buffer, ExecutableElement e, AnnotationValue val, boolean addSpace) {
    String element = e.getSimpleName().toString();
    buffer.append(element);
    buffer.append("=");
    buffer.append(getStringRepresentation(val));
    if (addSpace) buffer.append(", ");
  }

  private String getStringRepresentation(AnnotationValue val) {
    // val.toString() would have been enough for all standard java compilers
    // However, for the eclipse JDT compiler val.toString returns incorrect values
    // (i.e. values that makes the generated source syntactically invalid) so
    // we have to play this game of checking for specific value types and returning
    // appropriate strings
    Object obj = val.getValue();
    if (obj instanceof String) {
      return "\"" + (String) obj + "\"";
    } else if (obj instanceof VariableElement) {
      VariableElement elem = (VariableElement) obj;
      String parent = elem.getEnclosingElement().getSimpleName().toString();
      String enumName = elem.getSimpleName().toString();
      return parent + "." + enumName;
    } else if (obj instanceof AnnotationMirror) {
      return getStringRepresentation((AnnotationMirror) obj);
    } else if (obj instanceof List<?>) {
      @SuppressWarnings("unchecked")
      List<? extends AnnotationValue> list = (List<? extends AnnotationValue>) obj;
      StringBuffer buffer = new StringBuffer();
      buffer.append("{");
      for (int i = 0; i < list.size(); i++) {
        AnnotationValue av = list.get(i);
        buffer.append(getStringRepresentation(av));
        if (i != list.size() - 1) {
          buffer.append(", ");
        }
      }
      buffer.append("}");
      return buffer.toString();
    }
    return val.toString();
  }
}
