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

package com.ibm.streams.spl.internal.tkindexer.operator;

import com.ibm.streams.operator.TupleAttribute;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.internal.model.ShadowClass;
import com.ibm.streams.operator.internal.model.processors.Util;
import com.ibm.streams.operator.internal.runtime.OperatorAnnotationsInspector;
import com.ibm.streams.operator.metrics.Metric.Kind;
import com.ibm.streams.operator.model.CustomMetric;
import com.ibm.streams.operator.model.DefaultAttribute;
import com.ibm.streams.operator.model.Icons;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.Libraries;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.spl.internal.tkindexer.ModelGeneratorHelper;
import java.beans.BeanInfo;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.GenericArrayType;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.math.BigDecimal;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/** Generate a toolkit operator model XML file for a Java operator. */
public final class OperatorModelGenerator {

  private ModelGeneratorHelper mgh;
  private Document xmlDocument;
  private File toolkitDir;

  private Class<?> cls;

  private OperatorAnnotationProcessor processor;
  private List<String> inputPortIds;

  private static final String ICON16 = "16";
  private static final String ICON32 = "32";

  public OperatorModelGenerator(OperatorAnnotationProcessor oap, File tkDir, Class<?> cls)
      throws Exception {
    this.toolkitDir = tkDir;
    this.cls = cls;
    this.processor = oap;
    mgh = new ModelGeneratorHelper(oap);
    xmlDocument = mgh.newDocument();
    inputPortIds = new ArrayList<String>();
  }

  public void createModel() throws Exception {
    Element mod = xmlDocument.createElement("operatorModel");
    mod.setAttribute("xmlns", "http://www.ibm.com/xmlns/prod/streams/spl/operator");
    mod.setAttribute("xmlns:cmn", "http://www.ibm.com/xmlns/prod/streams/spl/common");
    mod.setAttribute("xmlns:xsi", ModelGeneratorHelper.W3C_XML_SCHEMA_INSTANCE);
    mod.setAttribute(
        "xsi:schemaLocation",
        "http://www.ibm.com/xmlns/prod/streams/spl/operator " + "operatorModel.xsd");
    mod.appendChild(mgh.getDoNotEditThisFileComment(xmlDocument));
    mod.appendChild(createJavaOperatorModel());
    xmlDocument.appendChild(mod);
    writeModel();
  }

  private Element createJavaOperatorModel() throws Exception {
    Element jmod = xmlDocument.createElement("javaOperatorModel");
    jmod.appendChild(mgh.createGeneratedFromComment(xmlDocument));

    createContext(jmod);
    createParameters(jmod);
    createInputPorts(jmod);
    createOutputPorts(jmod);

    return jmod;
  }

  private void createParameters(Element jmod) throws Exception {

    Element parameters = xmlDocument.createElement("parameters");
    jmod.appendChild(parameters);

    BeanInfo info = Introspector.getBeanInfo(cls);
    PropertyDescriptor[] properties = info.getPropertyDescriptors();
    for (PropertyDescriptor property : properties) {
      Method setter = property.getWriteMethod();
      if (setter != null) {
        if (setter.isAnnotationPresent(Parameter.class)) {
          parameters.appendChild(createParameter(property));
        }
      }
    }
  }

  private Element createMetric(PropertyDescriptor property) {
    final CustomMetric metric = property.getWriteMethod().getAnnotation(CustomMetric.class);
    final String name = metric.name().length() == 0 ? property.getName() : metric.name();
    final Element e = xmlDocument.createElement("metric");
    e.appendChild(mgh.createTextElement(xmlDocument, "name", name));
    e.appendChild(mgh.createTextElement(xmlDocument, "description", metric.description()));
    e.appendChild(mgh.createTextElement(xmlDocument, "kind", getMetricKindString(metric.kind())));
    return e;
  }

  private String getMetricKindString(Kind kind) {
    switch (kind) {
      case COUNTER:
        return "Counter";
      case GAUGE:
        return "Gauge";
      default:
        return "Time";
    }
  }

  private Element createParameter(PropertyDescriptor property) {
    final Parameter param = property.getWriteMethod().getAnnotation(Parameter.class);
    final DefaultAttribute defaultAttr =
        property.getWriteMethod().getAnnotation(DefaultAttribute.class);
    final String name = param.name().length() == 0 ? property.getName() : param.name();
    final Element e = xmlDocument.createElement("parameter");
    e.appendChild(mgh.createTextElement(xmlDocument, "name", name));
    e.appendChild(mgh.createTextElement(xmlDocument, "description", param.description()));
    e.appendChild(
        mgh.createTextElement(
            xmlDocument,
            "optional",
            defaultAttr != null ? "true" : String.valueOf(param.optional())));
    Class<?> functionParam = property.getPropertyType();
    if (functionParam.isEnum()) {
      e.appendChild(mgh.createTextElement(xmlDocument, "expressionMode", "CustomLiteral"));
      e.appendChild(mgh.createTextElement(xmlDocument, "type", functionParam.getCanonicalName()));
    } else if (functionParam.isInterface()
        && functionParam.getCanonicalName().equals("com.ibm.streams.operator.TupleAttribute")) {
      e.appendChild(mgh.createTextElement(xmlDocument, "expressionMode", "Attribute"));

      Type type = property.getWriteMethod().getGenericParameterTypes()[0];
      setAttributeParameterType(type, e);
    } else if (functionParam.isArray()
        && functionParam
            .getComponentType()
            .getCanonicalName()
            .equals("com.ibm.streams.operator.TupleAttribute")) {
      e.appendChild(mgh.createTextElement(xmlDocument, "expressionMode", "Attribute"));

      GenericArrayType arrayType =
          (GenericArrayType) property.getWriteMethod().getGenericParameterTypes()[0];
      setAttributeParameterType(arrayType.getGenericComponentType(), e);
    } else if (functionParam.equals(List.class)) {
      Type type = property.getWriteMethod().getGenericParameterTypes()[0];
      if (type instanceof ParameterizedType) {
        Type actual = ((ParameterizedType) type).getActualTypeArguments()[0];
        if (actual instanceof ParameterizedType
            && TupleAttribute.class.equals(((ParameterizedType) actual).getRawType())) {
          e.appendChild(mgh.createTextElement(xmlDocument, "expressionMode", "Attribute"));
          setAttributeParameterType(actual, e);
        } else {
          setParameterTypeElement(property, e);
        }
      } else {
        setParameterTypeElement(property, e);
      }
    } else {
      setParameterTypeElement(property, e);
    }
    int cardinality = param.cardinality();
    if (cardinality == -1 && isSingleCardinality(property)) {
      cardinality = 1;
    }
    e.appendChild(mgh.createTextElement(xmlDocument, "cardinality", String.valueOf(cardinality)));
    return e;
  }

  private void setAttributeParameterType(Type tupleAttributeType, final Element e) {
    ParameterizedType type = (ParameterizedType) tupleAttributeType;
    Type[] argTypes = type.getActualTypeArguments();
    if (argTypes.length == 2) {
      if (argTypes[1] instanceof Class) {
        Class<?> clz = (Class<?>) argTypes[1];
        MetaType metaType = OperatorAnnotationsInspector.getParameterSPLType(clz);
        e.appendChild(mgh.createTextElement(xmlDocument, "type", metaType.getLanguageType()));
      }
    }
  }

  private void setParameterTypeElement(PropertyDescriptor property, final Element e) {
    MetaType type = OperatorAnnotationsInspector.getParameterSPLType(property);
    // if type is BigDecimal then always used decimal128 to cover the biggest case.
    if (type == MetaType.DECIMAL32) {
      type = MetaType.DECIMAL128;
    }
    e.appendChild(mgh.createTextElement(xmlDocument, "type", type.getLanguageType()));
  }

  private boolean isSingleCardinality(PropertyDescriptor property) {
    Class<?> clz = property.getPropertyType();
    if (clz.isEnum()
        || clz.equals(TupleAttribute.class)
        || clz.equals(Boolean.class)
        || clz.equals(Boolean.TYPE)
        || clz.equals(Byte.class)
        || clz.equals(Byte.TYPE)
        || clz.equals(Short.class)
        || clz.equals(Short.TYPE)
        || clz.equals(Integer.class)
        || clz.equals(Integer.TYPE)
        || clz.equals(Long.class)
        || clz.equals(Long.TYPE)
        || clz.equals(Float.class)
        || clz.equals(Float.TYPE)
        || clz.equals(Double.class)
        || clz.equals(Double.TYPE)
        || clz.equals(BigDecimal.class)
        || clz.equals(String.class)) {
      return true;
    }
    return false;
  }

  private void createOutputPorts(Element jmod) {
    OutputPorts ports = cls.getAnnotation(OutputPorts.class);
    OutputPortSet[] portSets = null;
    if (ports != null) {
      portSets = ports.value();
    } else if (cls.isAnnotationPresent(OutputPortSet.class)) {
      portSets = new OutputPortSet[] {cls.getAnnotation(OutputPortSet.class)};
    }

    Element outputPortsElem = xmlDocument.createElement("outputPorts");
    jmod.appendChild(outputPortsElem);
    Element openOutputPortSet = null;
    if (portSets != null && portSets.length > 0) {
      for (OutputPortSet set : portSets) {
        if (set.cardinality() == -1) {
          openOutputPortSet = createOutputPortElement(set);
        } else {
          outputPortsElem.appendChild(createOutputPortElement(set));
        }
      }
      if (openOutputPortSet != null) {
        outputPortsElem.appendChild(openOutputPortSet);
      }
    }
  }

  private Element createOutputPortElement(OutputPortSet set) {
    int cardinality = set.cardinality();
    // if no cardinality, then its an open port set
    Element outputPortElem =
        xmlDocument.createElement(cardinality != -1 ? "outputPortSet" : "outputPortOpenSet");
    addDescription(outputPortElem, false, set.description());

    outputPortElem.appendChild(
        mgh.createTextElement(
            xmlDocument,
            "windowPunctuationOutputMode",
            set.windowPunctuationOutputMode().toString()));

    String inputPortId = set.windowPunctuationInputPort();
    if (inputPortId.length() > 0) {
      int index = inputPortIds.indexOf(inputPortId);
      outputPortElem.appendChild(
          mgh.createTextElement(xmlDocument, "windowPunctuationInputPort", String.valueOf(index)));
    }

    if (cardinality != -1) {
      outputPortElem.appendChild(
          mgh.createTextElement(xmlDocument, "cardinality", String.valueOf(cardinality)));
      outputPortElem.appendChild(
          mgh.createTextElement(xmlDocument, "optional", String.valueOf(set.optional())));
    }

    return outputPortElem;
  }

  private void createInputPorts(Element jmod) {
    InputPorts ports = cls.getAnnotation(InputPorts.class);
    InputPortSet[] portSets = null;
    if (ports != null) {
      portSets = ports.value();
    } else if (cls.isAnnotationPresent(InputPortSet.class)) {
      portSets = new InputPortSet[] {cls.getAnnotation(InputPortSet.class)};
    }

    Element inputPortsElem = xmlDocument.createElement("inputPorts");
    jmod.appendChild(inputPortsElem);
    Element openInputPortSet = null;
    if (portSets != null && portSets.length > 0) {
      for (InputPortSet set : portSets) {
        if (set.cardinality() == -1) {
          openInputPortSet = createInputPortElement(set);
        } else {
          inputPortsElem.appendChild(createInputPortElement(set));
        }
        inputPortIds.add(set.id());
      }
      if (openInputPortSet != null) {
        inputPortsElem.appendChild(openInputPortSet);
      }
    }
  }

  private Element createInputPortElement(InputPortSet set) {
    int cardinality = set.cardinality();
    // if no cardinality, then its an open port set
    Element inputPortElem =
        xmlDocument.createElement(cardinality != -1 ? "inputPortSet" : "inputPortOpenSet");
    addDescription(inputPortElem, false, set.description());

    inputPortElem.appendChild(
        mgh.createTextElement(xmlDocument, "windowingDescription", set.windowingDescription()));
    inputPortElem.appendChild(
        mgh.createTextElement(xmlDocument, "windowingMode", set.windowingMode().toString()));
    inputPortElem.appendChild(
        mgh.createTextElement(
            xmlDocument,
            "windowPunctuationInputMode",
            set.windowPunctuationInputMode().toString()));
    inputPortElem.appendChild(
        mgh.createTextElement(xmlDocument, "controlPort", String.valueOf(set.controlPort())));

    if (cardinality != -1) {
      inputPortElem.appendChild(
          mgh.createTextElement(xmlDocument, "cardinality", String.valueOf(cardinality)));
      inputPortElem.appendChild(
          mgh.createTextElement(xmlDocument, "optional", String.valueOf(set.optional())));
    }

    return inputPortElem;
  }

  private Element createContext(Element jmod) throws Exception {
    PrimitiveOperator primOperAnn = cls.getAnnotation(PrimitiveOperator.class);
    String comment = primOperAnn.comment();
    if (comment.length() > 0) {
      jmod.appendChild(xmlDocument.createComment(comment));
    }

    Element context = xmlDocument.createElement("context");
    jmod.appendChild(context);

    // description
    addDescription(context, false, primOperAnn.description());

    // iconURI
    Icons icons = cls.getAnnotation(Icons.class);
    if (icons != null) {
      String icon = icons.location16();
      moveIconAndAddToXML(context, ICON16, icon);
      icon = icons.location32();
      moveIconAndAddToXML(context, ICON32, icon);
    }

    // metrics
    Element metrics = xmlDocument.createElement("metrics");
    context.appendChild(metrics);

    BeanInfo info = Introspector.getBeanInfo(cls);
    PropertyDescriptor[] properties = info.getPropertyDescriptors();
    HashMap<Class<?>, Class<?>> enumsSeen = new HashMap<Class<?>, Class<?>>();

    Element customLiterals = null;

    for (PropertyDescriptor property : properties) {
      Method setter = property.getWriteMethod();
      if (setter != null) {
        if (setter.isAnnotationPresent(CustomMetric.class)) {
          metrics.appendChild(createMetric(property));
        }
        // custom literals
        else if (setter.isAnnotationPresent(Parameter.class)) {
          Class<?> param = setter.getParameterTypes()[0];
          if (param.isEnum() && enumsSeen.get(param) == null) {
            if (customLiterals == null) {
              customLiterals = xmlDocument.createElement("customLiterals");
              context.appendChild(customLiterals);
            }

            Element enumElement = xmlDocument.createElement("enumeration");
            customLiterals.appendChild(enumElement);

            enumElement.appendChild(
                mgh.createTextElement(xmlDocument, "name", param.getCanonicalName()));
            Object[] values = param.getEnumConstants();
            for (Object val : values) {
              enumElement.appendChild(mgh.createTextElement(xmlDocument, "value", val.toString()));
            }
            enumsSeen.put(param, param);
          }
        }
      }
    }

    // exe settings
    Element exeSettings = xmlDocument.createElement("executionSettings");
    context.appendChild(exeSettings);

    // class name
    ShadowClass op = cls.getAnnotation(ShadowClass.class);
    exeSettings.appendChild(mgh.createTextElement(xmlDocument, "className", op.value()));

    // vm args
    String[] vmArgs = primOperAnn.vmArgs();
    if (vmArgs.length > 0) {
      Element vmArgsElem = xmlDocument.createElement("vmArgs");
      exeSettings.appendChild(vmArgsElem);

      for (String vmArg : vmArgs) {
        vmArgsElem.appendChild(mgh.createTextElement(xmlDocument, "vmArg", vmArg));
      }
    }

    // lib dependencies
    Element libDep = xmlDocument.createElement("libraryDependencies");
    context.appendChild(libDep);

    Element library = xmlDocument.createElement("library");
    libDep.appendChild(library);

    addDescription(library, true, "Operator class library");

    Element mgdLib = xmlDocument.createElement("cmn:managedLibrary");
    library.appendChild(mgdLib);

    mgdLib.appendChild(
        mgh.createTextElement(
            xmlDocument, "cmn:libPath", getDirectoryPrefix() + processor.getClassRelClasspath()));

    Libraries libs = cls.getAnnotation(Libraries.class);
    if (libs != null) {
      for (String lib : libs.value()) {
        if (lib.startsWith("@")) {
          mgdLib.appendChild(mgh.createTextElement(xmlDocument, "cmn:libPath", lib));
        } else {
          File file = new File(lib);
          if (file.isAbsolute()) {
            mgdLib.appendChild(mgh.createTextElement(xmlDocument, "cmn:libPath", lib));
          } else {
            mgdLib.appendChild(
                mgh.createTextElement(xmlDocument, "cmn:libPath", getDirectoryPrefix() + lib));
          }
        }
      }
    }

    return context;
  }

  private String getDirectoryPrefix() throws Exception {
    File curDir = new File(getOperatorXMLDirectory());
    StringBuilder dirPrefix = new StringBuilder();
    while (curDir != null && !curDir.getCanonicalPath().equals(toolkitDir.getCanonicalPath())) {
      dirPrefix.append("..");
      dirPrefix.append(File.separator);
      curDir = curDir.getParentFile();
    }
    return dirPrefix.toString();
  }

  private void moveIconAndAddToXML(Element context, String size, String icon) throws Exception {
    File iconFile = new File(toolkitDir, icon);

    if (iconFile != null && iconFile.exists()) {
      String iconFileName = iconFile.getName();
      File resourceLocation = new File(getOperatorXMLDirectory(), iconFileName);
      processor.mkdirs(resourceLocation.getParentFile());

      InputStream stream = new FileInputStream(iconFile);
      if (stream != null) {

        FileOutputStream outStream = new FileOutputStream(resourceLocation);
        byte[] buffer = new byte[4096];
        int readBytes;

        while ((readBytes = stream.read(buffer)) != -1) {
          outStream.write(buffer, 0, readBytes);
        }
        stream.close();
        outStream.close();
        Element iconElement = xmlDocument.createElement("iconUri");
        iconElement.setAttribute("size", size);
        iconElement.appendChild(xmlDocument.createTextNode(iconFileName));
        context.appendChild(iconElement);
      }
    } else {
      System.err.println(MessageFormat.format("Icon file {0} not found", iconFile));
    }
  }

  private String getOperatorName() {
    PrimitiveOperator primOper = cls.getAnnotation(PrimitiveOperator.class);
    String name = primOper.name();
    if (name.length() == 0) {
      int index = cls.getSimpleName().indexOf(Util.GENERATED_CLASS_SUFFIX);
      name = cls.getSimpleName().substring(0, index);

      int dollarIndex = name.lastIndexOf('$');
      // if its a nested class then name of operator should just be operator name
      if (dollarIndex != -1) {
        name = name.substring(dollarIndex + 1);
      }
    }
    return name;
  }

  /*
   * This method return the namespace folder.
   * Annotation name is determined at Java compile time
   */
  private String getNamespace() throws IOException {
    PrimitiveOperator primOper = cls.getAnnotation(PrimitiveOperator.class);
    String namespace = primOper.namespace();
    if (namespace.length() > 0) {
      return File.separator + ModelGeneratorHelper.getNamespacePath(namespace);
    }
    return "";
  }

  private void addDescription(Element parent, boolean common, String text) {
    Element description = xmlDocument.createElement(common ? "cmn:description" : "description");
    description.appendChild(xmlDocument.createTextNode(text));
    parent.appendChild(description);
  }

  private String getOperatorXMLDirectory() throws Exception {
    return toolkitDir.getCanonicalPath() + getNamespace() + File.separator + getOperatorName();
  }

  public final void writeModel() throws Exception {
    mgh.writeModelFile(getOperatorXMLDirectory(), getOperatorName() + ".xml", xmlDocument);
  }
}
