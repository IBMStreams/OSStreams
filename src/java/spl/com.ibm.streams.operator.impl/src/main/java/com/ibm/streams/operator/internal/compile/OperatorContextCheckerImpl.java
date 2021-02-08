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

package com.ibm.streams.operator.internal.compile;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingData;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.logging.LogLevel;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.MessageKey;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessages;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.Locale;
import java.util.Set;
import java.util.logging.Logger;

/** Utility class for verifying the context an operator is being invoked in is correct. */
public final class OperatorContextCheckerImpl implements OperatorContextChecker {

  private final OperatorContext context;
  private static final Logger logger =
      Logger.getLogger("com.ibm.streams.operator.compile", StreamsSPLJavaMessages.class.getName());
  private boolean contextValid = true;

  public OperatorContextCheckerImpl(OperatorContext context) {
    this.context = context;
  }

  /**
   * Get the operator context for the operator's invocation.
   *
   * @return OperatorContext for this
   */
  @Override
  public OperatorContext getOperatorContext() {
    return context;
  }

  @Override
  public synchronized void setInvalidContext() {
    contextValid = false;
  }

  @Override
  public void setInvalidContext(String msg, Object[] params) {
    setInvalidContext();
    logger.log(LogLevel.ERROR, msg, params);
  }

  /** Internal setInvalidContextFromKey for our errors using a message key. */
  private void setInvalidContextFromKey(MessageKey msgKey, Object... params) {
    setInvalidContext();
    final String msg = new Message(msgKey, params).getLocalizedMessageAndPrefix();
    logger.log(LogLevel.ERROR, msg);
  }

  @Override
  public synchronized boolean isContextValid() {
    return contextValid;
  }

  /**
   * Verify that if {@code parameterName} is set as a parameter, then all the parameters in {@code
   * dependentParameterNames} are also set.
   *
   * @param parameterName Parameter that requires other parameters.
   * @param dependentParameterNames Parameters that {@code parameterName} depends
   *     on. @IllegalStateException {@code parameterName} is set and at least one dependent
   *     parameter is not set.
   */
  public final boolean checkDependentParameters(
      String parameterName, String... dependentParameterNames) {

    final Set<String> parameters = getOperatorContext().getParameterNames();

    if (!parameters.contains(parameterName)) return true;
    boolean ok = true;
    for (String dp : dependentParameterNames) {
      if (!parameters.contains(dp)) {
        setInvalidContextFromKey(Key.SPL_JAVA_OCCI_DEPENDS_PARAMS, parameterName, dp);
        ok = false;
      }
    }

    return ok;
  }

  /** */
  @Override
  public final boolean checkMatchingSchemas(StreamingData port, StreamingData... matchingPorts) {
    boolean ok = true;
    for (StreamingData matchingPort : matchingPorts) {
      if (!port.getStreamSchema().equals(matchingPort.getStreamSchema())) {
        setInvalidContextFromKey(
            Key.SPL_JAVA_OCCI_SCHEMA_MISMATCH,
            port.getName(),
            port.getStreamSchema().getLanguageType(),
            matchingPort.getName(),
            matchingPort.getStreamSchema().getLanguageType());
        ok = false;
      }
    }

    return ok;
  }

  @Override
  public boolean checkExcludedParameters(String parameterName, String... excludedParameterNames) {

    final Set<String> parameters = getOperatorContext().getParameterNames();

    if (!parameters.contains(parameterName)) return true;

    boolean ok = true;
    for (String ep : excludedParameterNames) {
      if (parameters.contains(ep)) {
        setInvalidContextFromKey(Key.SPL_JAVA_OCCI_EXPARAMS, parameterName, ep);
        ok = false;
      }
    }
    return ok;
  }

  /** Ensure each named attribute is present in the schema regardless of the schema type. */
  @Override
  public boolean checkRequiredAttributes(StreamSchema schema, String... attributeNames) {
    return hasRequiredAttributes(schema, null, attributeNames);
  }

  private boolean hasRequiredAttributes(
      final StreamSchema schema, final String portName, final String... attributeNames) {
    boolean ok = true;
    for (String attributeName : attributeNames) {
      if (schema.getAttributeIndex(attributeName) == -1) {
        setInvalidContext();
        errorRequiredAttribute(schema, portName, attributeName);

        ok = false;
      }
    }
    return ok;
  }

  private void errorRequiredAttribute(
      final StreamSchema schema, final String portName, String attributeName) {
    if (portName == null) {
      logger.log(
          LogLevel.ERROR,
          new Message(Key.SPL_JAVA_OCCI_MISSING_ATTR, attributeName, schema.getLanguageType())
              .getLocalizedMessageAndPrefix());
    } else {
      logger.log(
          LogLevel.ERROR,
          new Message(
                  Key.SPL_JAVA_OCCI_MISSING_PORT_ATTR,
                  attributeName,
                  schema.getLanguageType(),
                  portName)
              .getLocalizedMessageAndPrefix());
    }
  }

  /** Ensure each named attribute is present in the port regardless of the schema type. */
  @Override
  public boolean checkRequiredAttributes(StreamingData port, String... attributeNames) {
    return hasRequiredAttributes(port.getStreamSchema(), port.getName(), attributeNames);
  }

  /**
   * Check each attribute in the expected schema has the correct type if it exists (optional==false)
   * and that it must exist (optional = true).
   */
  @Override
  public boolean checkAttributes(
      StreamSchema schema, boolean optional, StreamSchema expectedAttributes) {
    return hasAttributes(schema, null, optional, expectedAttributes);
  }

  private boolean hasAttributes(
      StreamSchema schema, String portName, boolean optional, StreamSchema expectedAttributes) {
    boolean ok = true;
    for (Attribute expectedAttribute : expectedAttributes) {
      int schemaAttributeIndex = schema.getAttributeIndex(expectedAttribute.getName());
      if (schemaAttributeIndex == -1) {
        // Attribute is not present
        if (!optional) {
          setInvalidContext();
          errorRequiredAttribute(schema, portName, expectedAttribute.getName());
          ok = false;
          continue;
        }
      } else {
        Attribute schemaAttribute = schema.getAttribute(schemaAttributeIndex);
        if (!schemaAttribute.getType().equals(expectedAttribute.getType())) {
          setInvalidContext();
          logger.log(
              LogLevel.ERROR,
              new Message(
                      Key.SPL_JAVA_OCCI_WRONG_ATTR_TYPE,
                      expectedAttribute.getName(),
                      expectedAttribute.getType().getLanguageType(),
                      schemaAttribute.getType().getLanguageType())
                  .getLocalizedMessageAndPrefix());
          ok = false;
        }
      }
    }
    return ok;
  }

  @Override
  public boolean checkAttributes(StreamSchema schema, boolean optional, String expectedAttributes) {
    return checkAttributes(
        schema, optional, Type.Factory.getTupleType(expectedAttributes).getTupleSchema());
  }

  @Override
  public boolean checkAttributes(
      StreamingData port, boolean optional, StreamSchema expectedAttributes) {
    return hasAttributes(port.getStreamSchema(), port.getName(), optional, expectedAttributes);
  }

  @Override
  public boolean checkAttributes(StreamingData port, boolean optional, String expectedAttributes) {

    return hasAttributes(
        port.getStreamSchema(),
        port.getName(),
        optional,
        Type.Factory.getTupleType(expectedAttributes).getTupleSchema());
  }

  @Override
  public boolean checkAttributeType(Attribute attribute, MetaType... validTypes) {
    MetaType attributeType = attribute.getType().getMetaType();
    for (MetaType validType : validTypes) if (attributeType == validType) return true;

    setInvalidContextFromKey(
        Key.SPL_JAVA_OCCI_ATTR_TYPE, attribute.getType().getLanguageType(), attribute.getName());
    return false;
  }

  @Override
  public boolean checkTumblingWindow(final StreamingInput<?> inputPort) {
    return checkWindowType(inputPort, StreamWindow.Type.TUMBLING);
  }

  @Override
  public boolean checkSlidingWindow(final StreamingInput<?> inputPort) {
    return checkWindowType(inputPort, StreamWindow.Type.SLIDING);
  }

  private boolean checkWindowType(
      final StreamingInput<?> inputPort, final StreamWindow.Type expectedType) {
    final StreamWindow.Type portType = inputPort.getStreamWindow().getType();
    if (portType == expectedType) return true;
    setInvalidContextFromKey(
        Key.SPL_JAVA_OCCI_WINDOW_TYPE,
        inputPort.getName(),
        getSPLWindowType(expectedType),
        getSPLWindowType(portType));

    return false;
  }

  private static String getSPLWindowType(StreamWindow.Type type) {
    return type.toString().toLowerCase(Locale.US);
  }
}
