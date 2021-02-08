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

package com.ibm.streams.tools.doc.generator.operviz;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class OperatorModelProperties {
  private String operatorKind_ = "";
  private OperatorLanguage operatorLanguage_ = OperatorLanguage.CPP;
  private SingleThreadedContext providesSingleThreadedContext_ = null;
  private ArrayList<InputPortSet> inputPortSets_ = new ArrayList<InputPortSet>();
  private ArrayList<OutputPortSet> outputPortSets_ = new ArrayList<OutputPortSet>();
  private String icon_ = null;
  private ArrayList<String> parameterNames_ = new ArrayList<String>();
  private ArrayList<String> optionalParameterNames_ = new ArrayList<String>();

  public static enum SingleThreadedContext {
    Always,
    Never,
    WindowDependent
  }

  public void setIconPath(String icon) {
    icon_ = icon;
  }

  public String getIconPath() {
    return icon_;
  }

  public OperatorModelProperties(String kind) {
    operatorKind_ = kind;
  }

  public String getOperatorKind() {
    return operatorKind_;
  }

  public SingleThreadedContext getThreadContext() {
    return providesSingleThreadedContext_;
  }

  public void setThreadContext(SingleThreadedContext context) {
    providesSingleThreadedContext_ = context;
  }

  OperatorLanguage getOperatorLanguage() {
    return operatorLanguage_;
  }

  void setOperatorLanguage(OperatorLanguage lang) {
    operatorLanguage_ = lang;
  }

  public List<InputPortSet> getInputPortSets() {
    return Collections.unmodifiableList(inputPortSets_);
  }

  public void addInputPortSet(InputPortSet iport) {
    inputPortSets_.add(iport);
  }

  public List<OutputPortSet> getOutputPortSets() {
    return Collections.unmodifiableList(outputPortSets_);
  }

  public void addOutputPortSet(OutputPortSet oport) {
    outputPortSets_.add(oport);
  }

  public static class InputPortSet {
    public static enum PunctuationMode {
      Expecting,
      Oblivious,
      WindowBound
    }

    public static enum WindowingMode {
      Windowed,
      OptionallyWindowed,
      NonWindowed
    }

    private boolean optional_ = true;
    private boolean open_ = true;
    private PunctuationMode punctMode_ = PunctuationMode.Oblivious;
    private int cardinality_ = 0;
    private boolean mutable_ = false;
    private WindowingMode windowingMode_ = WindowingMode.OptionallyWindowed;

    boolean isOptional() {
      return optional_;
    }

    void setOptional(boolean optional) {
      optional_ = optional;
    }

    boolean isOpen() {
      return open_;
    }

    void setOpen(boolean open) {
      open_ = open;
    }

    PunctuationMode getPunctuationMode() {
      return punctMode_;
    }

    void setPunctuationMode(PunctuationMode mode) {
      punctMode_ = mode;
    }

    int getCardinality() {
      return cardinality_;
    }

    void setCardinality(int cardinality) {
      cardinality_ = cardinality;
    }

    boolean isMutable() {
      return mutable_;
    }

    void setMutable(boolean mutable) {
      mutable_ = mutable;
    }

    WindowingMode getWindowingMode() {
      return windowingMode_;
    }

    void setWindowingMode(WindowingMode mode) {
      windowingMode_ = mode;
    }
  }

  public static class OutputPortSet {
    public static enum PunctuationMode {
      Free,
      Preserving,
      Generating
    }

    private boolean optional_ = true;
    private boolean open_ = true;
    private PunctuationMode punctMode_ = PunctuationMode.Free;
    private int cardinality_ = 0;
    private boolean mutable_ = false;

    boolean isOptional() {
      return optional_;
    }

    void setOptional(boolean optional) {
      optional_ = optional;
    }

    boolean isOpen() {
      return open_;
    }

    void setOpen(boolean open) {
      open_ = open;
    }

    PunctuationMode getPunctuationMode() {
      return punctMode_;
    }

    void setPunctuationMode(PunctuationMode mode) {
      punctMode_ = mode;
    }

    int getCardinality() {
      return cardinality_;
    }

    void setCardinality(int cardinality) {
      cardinality_ = cardinality;
    }

    boolean isMutable() {
      return mutable_;
    }

    void setMutable(boolean mutable) {
      mutable_ = mutable;
    }
  }

  public static enum OperatorLanguage {
    CPP,
    Java
  }

  public List<String> getMandatoryParameters() {
    return Collections.unmodifiableList(parameterNames_);
  }

  public List<String> getOptionalParameters() {
    return Collections.unmodifiableList(optionalParameterNames_);
  }

  public void addParameter(String param, boolean optional) {
    if (optional) optionalParameterNames_.add(param);
    else parameterNames_.add(param);
  }
}
