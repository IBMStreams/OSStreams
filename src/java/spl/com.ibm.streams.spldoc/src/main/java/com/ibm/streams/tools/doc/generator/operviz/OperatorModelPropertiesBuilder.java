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

import com.ibm.streams.tools.doc.generator.operviz.OperatorModelProperties.InputPortSet.WindowingMode;
import com.ibm.xmlns.prod.streams.spl.operator.IconUriType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.OpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.ParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.EList;

public class OperatorModelPropertiesBuilder {
  private String kind_ = null;
  private String modelDir_ = null;
  private OperatorModelType model_ = null;

  public OperatorModelPropertiesBuilder(OperatorModelType model, String kind, String modelDir) {
    model_ = model;
    kind_ = kind;
    modelDir_ = modelDir;
  }

  public OperatorModelProperties build() {
    OpModelType cmodel = model_.getCppOperatorModel();
    JavaOpModelType jmodel = model_.getJavaOperatorModel();

    OperatorModelProperties props = new OperatorModelProperties(kind_);

    String icon = null;
    { // icon
      if (cmodel != null) {
        EList<IconUriType> icons = cmodel.getContext().getIconUri();
        if (icons.size() > 0)
          icon = modelDir_ + Path.SEPARATOR + icons.get(icons.size() - 1).getValue();
      } else {
        EList<IconUriType> icons = jmodel.getContext().getIconUri();
        if (icons.size() > 0)
          icon = modelDir_ + Path.SEPARATOR + icons.get(icons.size() - 1).getValue();
      }
    }
    props.setIconPath(icon);

    OperatorModelProperties.SingleThreadedContext threadContext =
        OperatorModelProperties.SingleThreadedContext.Always;
    { // single threaded context
      if (cmodel != null) {
        SingleThreadedContextType context = cmodel.getContext().getProvidesSingleThreadedContext();
        switch (context) {
          case ALWAYS:
            threadContext = OperatorModelProperties.SingleThreadedContext.Always;
            break;
          case NEVER:
            threadContext = OperatorModelProperties.SingleThreadedContext.Never;
            break;
          case WINDOW_BOUND:
          case WINDOW_EVICTION_BOUND:
          case WINDOW_PARTITION_EVICTION_BOUND:
          case WINDOW_TRIGGER_BOUND:
            threadContext = OperatorModelProperties.SingleThreadedContext.WindowDependent;
            break;
          default:
            throw new AssertionError(context);
        }
      } else if (jmodel != null) {
        // Java operators are always multi-threaded
        threadContext = OperatorModelProperties.SingleThreadedContext.Never;
      }
    }
    props.setThreadContext(threadContext);

    OperatorModelProperties.OperatorLanguage language = null;
    { // language
      if (cmodel != null) language = OperatorModelProperties.OperatorLanguage.CPP;
      else language = OperatorModelProperties.OperatorLanguage.Java;
    }
    props.setOperatorLanguage(language);

    // parameters
    if (cmodel != null) {
      for (ParameterType param : cmodel.getParameters().getParameter())
        props.addParameter(param.getName(), param.isOptional());
    } else {
      for (JavaOpParameterType param : jmodel.getParameters().getParameter())
        props.addParameter(param.getName(), param.isOptional());
    }

    // input port sets
    if (cmodel != null) {
      for (InputPortSetType iport : cmodel.getInputPorts().getInputPortSet()) {
        OperatorModelProperties.InputPortSet iportP = new OperatorModelProperties.InputPortSet();
        iportP.setCardinality(iport.getCardinality().intValue());
        iportP.setMutable(iport.isTupleMutationAllowed());
        iportP.setOpen(false);
        iportP.setOptional(iport.isOptional());
        iportP.setPunctuationMode(getInputPunctuationMode(iport.getWindowPunctuationInputMode()));
        iportP.setWindowingMode(getWindowingMode(iport.getWindowingMode()));
        props.addInputPortSet(iportP);
      }
      InputPortOpenSetType iport = cmodel.getInputPorts().getInputPortOpenSet();
      if (iport != null) {
        OperatorModelProperties.InputPortSet iportP = new OperatorModelProperties.InputPortSet();
        iportP.setCardinality(0);
        iportP.setMutable(iport.isTupleMutationAllowed());
        iportP.setOpen(true);
        iportP.setOptional(true);
        iportP.setPunctuationMode(getInputPunctuationMode(iport.getWindowPunctuationInputMode()));
        iportP.setWindowingMode(getWindowingMode(iport.getWindowingMode()));
        props.addInputPortSet(iportP);
      }
    } else {
      for (JavaOpInputPortSetType iport : jmodel.getInputPorts().getInputPortSet()) {
        OperatorModelProperties.InputPortSet iportP = new OperatorModelProperties.InputPortSet();
        iportP.setCardinality(iport.getCardinality().intValue());
        iportP.setMutable(false);
        iportP.setOpen(false);
        iportP.setOptional(iport.isOptional());
        iportP.setPunctuationMode(getInputPunctuationMode(iport.getWindowPunctuationInputMode()));
        iportP.setWindowingMode(getWindowingMode(iport.getWindowingMode()));
        props.addInputPortSet(iportP);
      }
      JavaOpInputPortOpenSetType iport = jmodel.getInputPorts().getInputPortOpenSet();
      if (iport != null) {
        OperatorModelProperties.InputPortSet iportP = new OperatorModelProperties.InputPortSet();
        iportP.setCardinality(0);
        iportP.setMutable(false);
        iportP.setOpen(true);
        iportP.setOptional(true);
        iportP.setPunctuationMode(getInputPunctuationMode(iport.getWindowPunctuationInputMode()));
        iportP.setWindowingMode(getWindowingMode(iport.getWindowingMode()));
        props.addInputPortSet(iportP);
      }
    }

    // output port sets
    if (cmodel != null) {
      for (OutputPortSetType oport : cmodel.getOutputPorts().getOutputPortSet()) {
        OperatorModelProperties.OutputPortSet oportP = new OperatorModelProperties.OutputPortSet();
        oportP.setCardinality(oport.getCardinality().intValue());
        oportP.setMutable(oport.isTupleMutationAllowed());
        oportP.setOpen(false);
        oportP.setOptional(oport.isOptional());
        oportP.setPunctuationMode(getOutputPunctuationMode(oport.getWindowPunctuationOutputMode()));
        props.addOutputPortSet(oportP);
      }
      OutputPortOpenSetType oport = cmodel.getOutputPorts().getOutputPortOpenSet();
      if (oport != null) {
        OperatorModelProperties.OutputPortSet oportP = new OperatorModelProperties.OutputPortSet();
        oportP.setCardinality(0);
        oportP.setMutable(oport.isTupleMutationAllowed());
        oportP.setOpen(true);
        oportP.setOptional(true);
        oportP.setPunctuationMode(getOutputPunctuationMode(oport.getWindowPunctuationOutputMode()));
        props.addOutputPortSet(oportP);
      }
    } else {
      for (JavaOpOutputPortSetType oport : jmodel.getOutputPorts().getOutputPortSet()) {
        OperatorModelProperties.OutputPortSet oportP = new OperatorModelProperties.OutputPortSet();
        oportP.setCardinality(oport.getCardinality().intValue());
        oportP.setMutable(false);
        oportP.setOpen(false);
        oportP.setOptional(oport.isOptional());
        oportP.setPunctuationMode(getOutputPunctuationMode(oport.getWindowPunctuationOutputMode()));
        props.addOutputPortSet(oportP);
      }
      JavaOpOutputPortOpenSetType oport = jmodel.getOutputPorts().getOutputPortOpenSet();
      if (oport != null) {
        OperatorModelProperties.OutputPortSet oportP = new OperatorModelProperties.OutputPortSet();
        oportP.setCardinality(0);
        oportP.setMutable(false);
        oportP.setOpen(true);
        oportP.setOptional(true);
        oportP.setPunctuationMode(getOutputPunctuationMode(oport.getWindowPunctuationOutputMode()));
        props.addOutputPortSet(oportP);
      }
    }

    return props;
  }

  private OperatorModelProperties.InputPortSet.PunctuationMode getInputPunctuationMode(
      WindowPunctuationInputModeType mode) {
    switch (mode) {
      case EXPECTING:
        return OperatorModelProperties.InputPortSet.PunctuationMode.Expecting;
      case OBLIVIOUS:
        return OperatorModelProperties.InputPortSet.PunctuationMode.Oblivious;
      case WINDOW_BOUND:
        return OperatorModelProperties.InputPortSet.PunctuationMode.WindowBound;
      default:
        throw new AssertionError(mode);
    }
  }

  private WindowingMode getWindowingMode(WindowingModeType mode) {
    switch (mode) {
      case NON_WINDOWED:
        return OperatorModelProperties.InputPortSet.WindowingMode.NonWindowed;
      case OPTIONALLY_WINDOWED:
        return OperatorModelProperties.InputPortSet.WindowingMode.OptionallyWindowed;
      case WINDOWED:
        return OperatorModelProperties.InputPortSet.WindowingMode.Windowed;
      default:
        throw new AssertionError(mode);
    }
  }

  private OperatorModelProperties.OutputPortSet.PunctuationMode getOutputPunctuationMode(
      WindowPunctuationOutputModeType mode) {
    switch (mode) {
      case FREE:
        return OperatorModelProperties.OutputPortSet.PunctuationMode.Free;
      case PRESERVING:
        return OperatorModelProperties.OutputPortSet.PunctuationMode.Preserving;
      case GENERATING:
        return OperatorModelProperties.OutputPortSet.PunctuationMode.Generating;
      default:
        throw new AssertionError(mode);
    }
  }
}
