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

import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.AnnotationValue;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;

@SupportedAnnotationTypes({
  "com.ibm.streams.operator.model.InputPorts",
  "com.ibm.streams.operator.model.InputPortSet",
  "com.ibm.streams.operator.model.OutputPorts",
  "com.ibm.streams.operator.model.OutputPortSet"
})
public class PortAnnotationsProcessor extends AbstractProcessor {

  @SuppressWarnings("unchecked")
  @Override
  public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {
    if (!roundEnv.processingOver()) {
      for (Element elem : roundEnv.getRootElements()) {
        ArrayList<String> ids = new ArrayList<String>();
        ArrayList<AnnotationMirror> outputAnnotationList = new ArrayList<AnnotationMirror>();
        List<? extends AnnotationMirror> annoMirrors =
            processingEnv.getElementUtils().getAllAnnotationMirrors(elem);

        // Get a list of output port annotations with windowPunctuationInputPort set.
        for (AnnotationMirror mirror : annoMirrors) {
          if (isAnnotationType(mirror, OutputPorts.class)) {
            boolean isOpenOutputPortSeen = false;
            Map<? extends ExecutableElement, ? extends AnnotationValue> map =
                mirror.getElementValues();
            Iterator<? extends AnnotationValue> valuesIter = map.values().iterator();
            while (valuesIter.hasNext()) {
              AnnotationValue val = valuesIter.next();
              List<? extends AnnotationValue> values =
                  (List<? extends AnnotationValue>) val.getValue();

              for (AnnotationValue value : values) {
                AnnotationMirror inputPortAnnotationMirror = (AnnotationMirror) value.getValue();
                boolean isOpenOutputPort =
                    addToOutputIdList(elem, outputAnnotationList, inputPortAnnotationMirror);
                if (isOpenOutputPort && isOpenOutputPortSeen) {
                  processingEnv
                      .getMessager()
                      .printMessage(
                          Diagnostic.Kind.ERROR,
                          new Message(Key.PAP_OUTPUT_CARDINALITY_REQUIRED, "")
                              .getLocalizedMessage(),
                          elem,
                          inputPortAnnotationMirror);
                } else {
                  isOpenOutputPortSeen = isOpenOutputPort;
                }
              }
            }
          } else if (isAnnotationType(mirror, OutputPortSet.class)) {
            addToOutputIdList(elem, outputAnnotationList, mirror);
          }
        }

        // get a list of input port annotations
        for (AnnotationMirror mirror : annoMirrors) {
          if (isAnnotationType(mirror, InputPorts.class)) {
            boolean isOpenInputPortSeen = false;
            Map<? extends ExecutableElement, ? extends AnnotationValue> map =
                mirror.getElementValues();
            Iterator<? extends AnnotationValue> valuesIter = map.values().iterator();
            while (valuesIter.hasNext()) {
              AnnotationValue val = valuesIter.next();
              List<? extends AnnotationValue> values =
                  (List<? extends AnnotationValue>) val.getValue();

              for (AnnotationValue value : values) {
                AnnotationMirror inputPortAnnotationMirror = (AnnotationMirror) value.getValue();
                boolean isOpenInputPort =
                    checkAndAddToInputIdList(elem, ids, inputPortAnnotationMirror);
                if (isOpenInputPort && isOpenInputPortSeen) {
                  processingEnv
                      .getMessager()
                      .printMessage(
                          Diagnostic.Kind.ERROR,
                          new Message(Key.PAP_INPUT_CARDINALITY_REQUIRED, "").getLocalizedMessage(),
                          elem,
                          inputPortAnnotationMirror);
                } else {
                  isOpenInputPortSeen = isOpenInputPort;
                }
              }
            }
          } else if (isAnnotationType(mirror, InputPortSet.class)) {
            checkAndAddToInputIdList(elem, ids, mirror);
          }
        }

        // make sure that the ids referenced in the output port annotations match one of the input
        // ports
        for (AnnotationMirror outputAnnotationMirror : outputAnnotationList) {
          Map<? extends ExecutableElement, ? extends AnnotationValue> map =
              outputAnnotationMirror.getElementValues();
          Iterator<? extends ExecutableElement> attribIter = map.keySet().iterator();
          while (attribIter.hasNext()) {
            ExecutableElement exeElement = attribIter.next();
            if (exeElement.getSimpleName().toString().equals("windowPunctuationInputPort")) {
              AnnotationValue val = map.get(exeElement);
              String id = (String) val.getValue();

              if (!ids.contains(id)) {
                processingEnv
                    .getMessager()
                    .printMessage(
                        Diagnostic.Kind.ERROR,
                        new Message(Key.PAP_INVALID_PUNCTUATION_INPUT_PORT, id)
                            .getLocalizedMessage(),
                        elem,
                        outputAnnotationMirror);
              }
            }
          }
        }

        // check and make sure that InputPortSet/InputPorts and OutputPortSet/OutputPorts do not
        // occur together
        if (elem.getAnnotation(InputPorts.class) != null
            && elem.getAnnotation(InputPortSet.class) != null) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR,
                  new Message(Key.PAP_INVALID_INPUT_PORTS, "").getLocalizedMessage(),
                  elem,
                  getAnnotationMirror(elem, InputPortSet.class, InputPorts.class));
        }

        if (elem.getAnnotation(OutputPorts.class) != null
            && elem.getAnnotation(OutputPortSet.class) != null) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR,
                  new Message(Key.PAP_INVALID_OUTPUT_PORTS, "").getLocalizedMessage(),
                  elem,
                  getAnnotationMirror(elem, OutputPortSet.class, OutputPorts.class));
        }
      }
    }
    return false; // no further processing of this annotation type
  }

  AnnotationMirror getAnnotationMirror(Element e, Class<?>... clazz) {
    for (Class<?> cls : clazz) {
      TypeElement primOper = processingEnv.getElementUtils().getTypeElement(cls.getCanonicalName());
      for (AnnotationMirror mirror : e.getAnnotationMirrors()) {
        if (mirror.getAnnotationType().asElement().equals(primOper)) return mirror;
      }
    }
    return null;
  }

  private boolean checkAndAddToInputIdList(
      Element elem, ArrayList<String> ids, AnnotationMirror mirror) {
    boolean isOpenInputPort = true;
    Map<? extends ExecutableElement, ? extends AnnotationValue> map = mirror.getElementValues();
    Iterator<? extends ExecutableElement> attribIter = map.keySet().iterator();
    while (attribIter.hasNext()) {
      ExecutableElement exeElement = attribIter.next();
      String attributeName = exeElement.getSimpleName().toString();
      if (exeElement.getSimpleName().toString().equals("id")) {
        AnnotationValue val = map.get(exeElement);
        String str = (String) val.getValue();
        if (str != null && str.length() > 0) {
          if (!ids.contains(str)) ids.add(str);
          else {
            processingEnv
                .getMessager()
                .printMessage(
                    Diagnostic.Kind.ERROR,
                    new Message(Key.PAP_DUPLICATE_INPUT_PORT_ID, "").getLocalizedMessage(),
                    elem,
                    mirror);
          }
        }
      } else if (attributeName.equals("cardinality")) {
        AnnotationValue val = map.get(exeElement);
        int cardinality = ((Integer) val.getValue()).intValue();

        if (cardinality != ((Integer) exeElement.getDefaultValue().getValue()).intValue())
          isOpenInputPort = false;
      }
    }
    return isOpenInputPort;
  }

  /*
   * This method performs a number of things:
   *
   * 1) It collects all annotations that has windowPunctuationInputPort specified.
   * 2) It makes sure that if windowPunctuationInputPort is specified, then the window output mode is Preserving
   * 3) It returns whether the given output port set is open (with NO cardinality ) or not.
   *
   * @param elem
   * @param outputAnnotationList
   * @param mirror
   */
  private boolean addToOutputIdList(
      Element elem, ArrayList<AnnotationMirror> outputAnnotationList, AnnotationMirror mirror) {

    boolean isOpenOutputPort = true;
    boolean isPunctInputPortSpecified = false;
    VariableElement mode = null;
    Map<? extends ExecutableElement, ? extends AnnotationValue> map = mirror.getElementValues();
    Iterator<? extends ExecutableElement> attribIter = map.keySet().iterator();
    while (attribIter.hasNext()) {
      ExecutableElement exeElement = attribIter.next();
      String attributeName = exeElement.getSimpleName().toString();
      if (attributeName.equals("windowPunctuationInputPort")) {
        AnnotationValue val = (AnnotationValue) map.get(exeElement);
        String str = (String) val.getValue();
        if (str != null && str.length() > 0) {
          outputAnnotationList.add(mirror);
          isPunctInputPortSpecified = true;
        }
      } else if (attributeName.equals("windowPunctuationOutputMode")) {
        AnnotationValue val = map.get(exeElement);
        Object value = val.getValue();
        if (value instanceof VariableElement) mode = (VariableElement) value;
      } else if (attributeName.equals("cardinality")) {
        AnnotationValue val = map.get(exeElement);
        int cardinality = ((Integer) val.getValue()).intValue();

        if (cardinality != ((Integer) exeElement.getDefaultValue().getValue()).intValue())
          isOpenOutputPort = false;
      }
    }

    // if punct input port is specified, then the output mode must be preserving
    if (isPunctInputPortSpecified
        && (mode == null
            || !mode.getSimpleName()
                .toString()
                .equals(WindowPunctuationOutputMode.Preserving.name()))) {
      processingEnv
          .getMessager()
          .printMessage(
              Diagnostic.Kind.ERROR,
              new Message(Key.PAP_REQUIRE_PUNCTUATION_PRESERVING, "").getLocalizedMessage(),
              elem,
              mirror);
    }

    return isOpenOutputPort;
  }

  private boolean isAnnotationType(AnnotationMirror mirror, Class<?> clazz) {
    TypeElement element = (TypeElement) mirror.getAnnotationType().asElement();
    return element.getQualifiedName().toString().equals(clazz.getCanonicalName());
  }

  @Override
  public SourceVersion getSupportedSourceVersion() {
    return SourceVersion.latest();
  }
}
