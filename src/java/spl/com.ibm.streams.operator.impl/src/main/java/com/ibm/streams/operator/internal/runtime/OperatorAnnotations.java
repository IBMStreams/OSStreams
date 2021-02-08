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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.TupleAttribute;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.internal.compile.OperatorContextCheckerImpl;
import com.ibm.streams.operator.internal.runtime.api.OperatorInfo;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.model.CustomMetric;
import com.ibm.streams.operator.model.DefaultAttribute;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.beans.IntrospectionException;
import java.beans.PropertyDescriptor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

/** Helper class for applying annotations to the model. */
public class OperatorAnnotations extends OperatorAnnotationsInspector {

  private final OperatorContext context;
  private final Operator op;

  public OperatorAnnotations(OperatorContext context, Class<? extends Operator> operatorClass) {
    super(operatorClass);
    this.context = context;
    this.op = null;
  }

  public OperatorAnnotations(OperatorContext context, Operator op) {
    super(op.getClass());
    this.context = context;
    this.op = op;
  }

  /**
   * Apply annotations to the Operator class that are "invoked" at runtime.
   *
   * @throws Exception
   */
  public void applyRuntimeAnnotations(final OperatorInfo mgmt) throws Exception {
    try {

      // @OperatorContext.ContextCheck annotations
      if (!applyContextChecks())
        throw new IllegalStateException(
            new Message(
                    Key.SPL_RUNTIME_OPERATOR_INVOCATION_CONTEXT_INVALID,
                    context.getKind(),
                    op.getClass().getName())
                .getLocalizedMessageAndPrefix());
      // @Parameter annotations
      applyParameters();

      // @CustomMetric annotation
      applyCustomMetrics(mgmt);

    } catch (InvocationTargetException e) {
      Throwable t = e.getTargetException();
      if (t instanceof RuntimeException) throw (RuntimeException) t;
      if (t instanceof Exception) throw (Exception) t;
      throw (Error) t;
    }
  }

  /**
   * Apply compile time annotations to the operator invocation.
   *
   * @return True if all ok, false if compile time setup error.
   * @throws Exception
   */
  public boolean applyCompileTimeAnnotations() throws Exception {
    try {

      // @OperatorContext.ContextCheck annotations
      return applyContextChecks();

    } catch (InvocationTargetException e) {
      Throwable t = e.getTargetException();
      if (t instanceof RuntimeException) throw (RuntimeException) t;
      if (t instanceof Exception) throw (Exception) t;
      throw (Error) t;
    }
  }

  /**
   * Assign SPL parameter values to Java bean property setter methods marked with Parameter
   * annotation.
   *
   * @throws Exception
   */
  private void applyParameters() throws Exception {

    Set<String> parameters = context.getParameterNames();

    for (PropertyDescriptor property : getAnnotatedProperties(Parameter.class)) {

      Parameter pa = property.getWriteMethod().getAnnotation(Parameter.class);

      // Marked as a SPL parameter
      String splName = pa.name();
      if (splName.length() == 0) splName = property.getName();

      // If this parameter was set?
      if (parameters.contains(splName)) {

        // TODO - Support automatic tracing
        // of the value?
        @SuppressWarnings("unused")
        Object value =
            ParameterSetter.set(context, property, op, context.getParameterValues(splName));

      } else {
        DefaultAttribute val = property.getWriteMethod().getAnnotation(DefaultAttribute.class);
        if (val != null) {
          String attr = findAttribute(val.value());
          if (attr == null) {
            throw new IllegalStateException(
                new Message(Key.SPL_RUNTIME_DEFAULT_ATTR_NOT_FOUND, val.value(), splName)
                    .getLocalizedMessageAndPrefix());
          } else {
            ParameterSetter.set(context, property, op, Arrays.asList(attr));
          }
          continue;
        }
        if (!pa.optional())
          throw new IllegalStateException(
              new Message(Key.SPL_RUNTIME_MANDATORY_PARAM_NOT_SET, splName)
                  .getLocalizedMessageAndPrefix());
      }
    }
  }

  private String findAttribute(String name) {
    for (StreamingInput<Tuple> input : context.getStreamingInputs()) {
      Attribute attr = input.getStreamSchema().getAttribute(name);
      if (attr != null) {
        return input.getName() + "." + attr.getName();
      }
    }
    return null;
  }

  /** Verify the Parameter annotations against the Java primitive operator model. */
  public void verifyParameters(Document model) throws IntrospectionException {

    NodeList modelParameters = model.getElementsByTagName("parameter");
    Map<String, Element> mps = new HashMap<String, Element>();
    for (int i = 0; i < modelParameters.getLength(); i++) {
      Element paramElement = (Element) modelParameters.item(i);
      Element nameElement = (Element) paramElement.getElementsByTagName("name").item(0);
      mps.put(nameElement.getTextContent(), paramElement);
    }

    for (PropertyDescriptor property : getAnnotatedProperties(Parameter.class)) {

      final Method setter = property.getWriteMethod();
      Parameter pa = setter.getAnnotation(Parameter.class);

      String splName = pa.name();
      if (splName.length() == 0) splName = property.getName();

      Element paramElement = mps.get(splName);
      if (paramElement == null) {
        // @Parameter with no matching model element

        // Optional according to the annotation therefore
        // no requirement for the operator model to expose it.
        if (pa.optional()) continue;

        throw new IllegalStateException(
            new Message(
                    Key.SPL_RUNTIME_MANDATORY_PARAM_NOT_REPRESENTED,
                    splName,
                    pa,
                    setter.toGenericString())
                .getLocalizedMessageAndPrefix());
      } else {

        // @Parameter with a matching model element
        boolean optional;
        NodeList opts = paramElement.getElementsByTagName("optional");
        if (opts.getLength() == 0) optional = false; // missing optional element means mandatory.
        else optional = Boolean.valueOf(opts.item(0).getTextContent());

        // Valid combinations
        // @Parameter(optional=false) <optional>false</optional>
        // @Parameter(optional=true) <optional>true</optional>
        // @Parameter(optional=true) <optional>false</optional>
        //  (A specific operator using the class may require the parameter
        //   even though the Java class defines it as optional)
        //
        //
        //
        // Invalid combination
        // @Parameter(optional=false) <optional>true</optional>
        if (!pa.optional() && optional)
          throw new IllegalStateException(
              new Message(
                      Key.SPL_RUNTIME_MANDATORY_PARAM_OPTIONAL_IN_MODEL,
                      splName,
                      pa,
                      setter.toGenericString())
                  .getLocalizedMessageAndPrefix());
      }

      verifyAnnotatedParameterType(paramElement, property, pa, splName);

      verifyAnnotatedParameterCardinality(paramElement, property, pa, splName);
    }
  }

  /**
   * Verify the type of the {@code <parameter>} element matches the {@code @Parameter} annotation.
   */
  private void verifyAnnotatedParameterType(
      Element paramElement, PropertyDescriptor property, Parameter pa, String splName) {

    // Determine the SPL type.
    // Bean properties of type String always return RSTRING
    // but can also map to USTRING
    // Bean properties of type BigDecimal always return DECIMAL32
    // but can also map to DECIMAL64 and DECIMAL128
    MetaType splType = ParameterSetter.getSPLType(property);

    NodeList expModes = paramElement.getElementsByTagName("expressionMode");
    String expMode =
        ((expModes != null && expModes.getLength() > 0)
            ? expModes.item(0).getTextContent()
            : "AttributeFree");

    // Now validate the type.
    NodeList types = paramElement.getElementsByTagName("type");

    if (!expMode.equals("Attribute") && types.getLength() == 0) {
      throw new IllegalStateException(
          new Message(
                  Key.SPL_RUNTIME_REQUIRED_TYPE_NOT_SPECIFIED_FOR_PARAM,
                  splName,
                  pa,
                  property.getWriteMethod().toGenericString(),
                  splType.getLanguageType())
              .getLocalizedMessageAndPrefix());
    }

    String modelType = types.item(0).getTextContent();
    if (!modelType.equals(splType.getLanguageType())) {
      boolean ok = false;
      switch (splType) {
        case DECIMAL32:
          if (modelType.endsWith(MetaType.DECIMAL64.getLanguageType())) ok = true;
          else if (modelType.endsWith(MetaType.DECIMAL128.getLanguageType())) ok = true;
          break;
        case RSTRING:
          if (modelType.endsWith(MetaType.USTRING.getLanguageType())) ok = true;
          break;
        default:
          break;
      }
      if (!ok) {
        if (expMode.equals("CustomLiteral")) {
          try {
            Class<?> clz = Class.forName(modelType);
            ok = clz.isEnum();
          } catch (ClassNotFoundException e) {
          }
        } else if (expMode.equals("Attribute")) {
          Class<?> type = property.getWriteMethod().getParameterTypes()[0];
          ok = type.equals(TupleAttribute.class);
        }
        if (!ok)
          throw new IllegalStateException(
              new Message(
                      Key.SPL_RUNTIME_TYPE_FOR_PARAM_NOT_COMPATIBLE,
                      splName,
                      pa,
                      property.getWriteMethod().toGenericString(),
                      modelType)
                  .getLocalizedMessageAndPrefix());
      }
    }
  }

  private void verifyAnnotatedParameterCardinality(
      Element paramElement, PropertyDescriptor property, Parameter pa, String splName) {
    // TODO Auto-generated method stub
    NodeList cardinalityElement = paramElement.getElementsByTagName("cardinality");

    int cardinality;
    if (cardinalityElement.getLength() == 0) {
      cardinality = -1;
    } else {
      cardinality = Integer.valueOf(cardinalityElement.item(0).getTextContent());
    }

    Method setter = property.getWriteMethod();

    Class<?> propertyClassType = setter.getParameterTypes()[0];

    boolean multiValue = propertyClassType.isArray() || List.class.equals(propertyClassType);

    if (!multiValue && cardinality != 1) {
      throw new IllegalStateException(
          new Message(
                  Key.SPL_RUNTIME_CARDINALITY_FOR_PARAM_REQUIRED_ONE,
                  splName,
                  pa,
                  property.getWriteMethod().toGenericString())
              .getLocalizedMessageAndPrefix());
    }
  }

  /** Verify the CustomMetric annotations against the Java primitive operator model. */
  public void verifyCustomMetric(Document model) throws IntrospectionException {

    NodeList modelMetrics = model.getElementsByTagName("metric");
    Map<String, Element> mps = new HashMap<String, Element>();
    for (int i = 0; i < modelMetrics.getLength(); i++) {
      Element paramElement = (Element) modelMetrics.item(i);
      Element nameElement = (Element) paramElement.getElementsByTagName("name").item(0);
      mps.put(nameElement.getTextContent(), paramElement);
    }

    for (PropertyDescriptor property : getAnnotatedProperties(CustomMetric.class)) {

      final Method setter = property.getWriteMethod();
      CustomMetric cm = setter.getAnnotation(CustomMetric.class);

      String splName = cm.name();
      if (splName.length() == 0) splName = property.getName();

      Element metricElement = mps.get(splName);
      if (metricElement == null) {
        // @CustomMetric with no matching model element
      } else {

        // @CustomMetric with a matching model element
        NodeList kind = metricElement.getElementsByTagName("kind");
        String modelKind = kind.item(0).getTextContent();
        final String expectedKind;
        switch (cm.kind()) {
          case COUNTER:
            expectedKind = "Counter";
            break;
          case GAUGE:
            expectedKind = "Gauge";
            break;
          case TIME:
            expectedKind = "Time";
            break;
          default:
            expectedKind = null;
            break;
        }
        if (!modelKind.equals(expectedKind))
          throw new IllegalStateException(
              new Message(Key.SPL_RUNTIME_METRIC_KIND_MUST_MATCH, splName, expectedKind, modelKind)
                  .getLocalizedMessageAndPrefix());
      }
    }
  }

  /**
   * Apply the ContextCheck annotation. Calls any public static method with a single
   * OperatorContextCheck parameter.
   *
   * @throws Exception
   */
  private boolean applyContextChecks() throws Exception {

    OperatorContextChecker checker = null;

    for (Method method : getOperatorClass().getMethods()) {

      // Only static methods.
      if (!Modifier.isStatic(method.getModifiers())) continue;

      // and those with the ContextCheck annotation
      OperatorContext.ContextCheck cc = method.getAnnotation(OperatorContext.ContextCheck.class);
      if (cc == null) continue;

      // Runtime only?
      if (isRuntime() && !cc.runtime()) continue;

      // Compile time only?
      if (!isRuntime() && !cc.compile()) continue;

      Class<?>[] params = method.getParameterTypes();

      if (params.length != 1) continue; // do nothing - ignored.

      // Single parameter of type OperatorContextChecker.
      if (params[0].equals(OperatorContextChecker.class)) {
        if (checker == null) checker = new OperatorContextCheckerImpl(context);
        method.invoke(null, new Object[] {checker});
      }
    }

    if (checker != null) {
      if (isRuntime()) {
        if (!checker.isContextValid())
          throw new IllegalStateException(
              new Message(Key.SPL_RUNTIME_CONTEXTCHECK_FAILURES, getOperatorClass().getName())
                  .getLocalizedMessageAndPrefix());
      }

      return checker.isContextValid();
    }
    return true;
  }

  /**
   * Is this a runtime execution?
   *
   * @return True if this is runtime, false otherwise.
   */
  private boolean isRuntime() {
    return op != null;
  }

  /**
   * Call any setters annotated with CustomMetric with the corresponding Metric instance. If the
   * metric does not exist it will be created.
   */
  private void applyCustomMetrics(final OperatorInfo mgmt) throws Exception {

    final OperatorMetrics metrics = context.getMetrics();
    final Map<String, Metric> customMetrics = metrics.getCustomMetrics();

    for (PropertyDescriptor property : getAnnotatedProperties(CustomMetric.class)) {

      final Method setter = property.getWriteMethod();

      CustomMetric cm = setter.getAnnotation(CustomMetric.class);

      final String metricName = cm.name().length() == 0 ? property.getName() : cm.name();
      final String description =
          cm.description().length() == 0 ? property.getShortDescription() : cm.description();

      final Metric customMetric;
      if (customMetrics.containsKey(metricName)) customMetric = customMetrics.get(metricName);
      else customMetric = metrics.createCustomMetric(metricName, description, cm.kind());

      setter.invoke(op, customMetric);
      if (cm.mxbean()) mgmt.registerCustomMetric(metricName);
    }
  }
}
