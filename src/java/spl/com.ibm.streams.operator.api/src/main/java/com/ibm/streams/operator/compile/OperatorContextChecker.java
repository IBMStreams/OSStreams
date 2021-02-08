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

package com.ibm.streams.operator.compile;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingData;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Type.MetaType;

/**
 * Interface for checking a Java primitive operator is being invoked with a valid context.
 *
 * <p>Any public static method of an {@link com.ibm.streams.operator.Operator} implementation or its
 * super-classes annotated {@code @ContextCheck} will be called at compile and/or runtime to verify
 * its invocation environment. The method must be declared with a single parameter of type {@code
 * OperatorContextChecker}
 *
 * <p>A single {@code OperatorContextChecker} instance is used to check an operator's invocation
 * context. Any call to one of its {@code checkXXX} methods that returned false will result in the
 * context being marked invalid ({@code isContextValid()} will return {@code false}). Once all
 * {@code @ContextCheck} annotated methods have been called then if {@code isContextValid()} returns
 * {@code false} then the compile or runtime invocation will fail.
 *
 * <p>OperatorContextChecker serves a similar purpose for Java primitive operators that the validate
 * and check helper functions in {@code CodeGen.pm} serve for C++ operators.
 *
 * @see com.ibm.streams.operator.OperatorContext.ContextCheck
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface OperatorContextChecker {

  /**
   * Get the operator context for the operator's invocation. At compile time the functionality of
   * the {@code OperatorContext} is limited and use of any runtime only functionality, such as
   * submitting tuples will result in an {@code IllegalStateException}. Specifically for compile
   * time context checking these restrictions exist:
   *
   * <UL>
   *   <LI>The operator's name {@link OperatorContext#getName()} is not available.
   *   <LI>Parameter values are not available, though the fact a parameter is set can be checked
   *       through {@link OperatorContext#getParameterNames()}.
   * </UL>
   *
   * Some of these restrictions are due to the SPL compiler creating a single compiled instance to
   * represent multiple invocations of an operator type that only differ in their parameter values.
   *
   * @return OperatorContext for this invocation.
   */
  public OperatorContext getOperatorContext();

  /**
   * Set that the context is invalid and thus the compile or runtime invocation will fail once all
   * {@code @ContextCheck} annotated methods have been called. It is recommended that an associated
   * error message is also logged, this may be achieved using {@link
   * OperatorContextChecker#setInvalidContext(String, Object[])}.
   */
  public void setInvalidContext();

  /**
   * Set that the context is invalid and log a message at {@code ERROR} level.
   *
   * @param msg the message to log
   * @param params the parameter array associated with the error that need to be logged
   * @see #setInvalidContext()
   */
  public void setInvalidContext(String msg, Object[] params);

  /**
   * Is the invocation context valid so far.
   *
   * @return True if no errors have been reported against the invocation context, false if at least
   *     one error has been reported.
   */
  public boolean isContextValid();

  /**
   * Check that if {@code parameterName} is set as a parameter, then all of the parameters in {@code
   * dependentParameterNames} are set.
   *
   * @param parameterName Parameter that requires other parameters.
   * @param dependentParameterNames Parameters that {@code parameterName} depends on.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkDependentParameters(String parameterName, String... dependentParameterNames);

  /**
   * Check that if {@code parameterName} is set as a parameter, then none of the parameters in
   * {@code excludedParameterNames} are set.
   *
   * @param parameterName Parameter that excludes other parameters.
   * @param excludedParameterNames Parameters that {@code parameterName} excludes.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkExcludedParameters(String parameterName, String... excludedParameterNames);

  /**
   * Check that one or more ports has a stream schema that matches another port. Typically used to
   * check that one or more output ports matches an input port.
   *
   * @param port Port with schema to check against.
   * @param matchingPorts Ports that must have the same schema as {@code port}.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkMatchingSchemas(StreamingData port, StreamingData... matchingPorts);

  /**
   * Check that a set of required attributes exist in a {@link StreamSchema}. No ordering is
   * required for the attributes.
   *
   * @param schema Schema to be checked.
   * @param attributeNames Set of required attributes.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkRequiredAttributes(StreamSchema schema, String... attributeNames);

  /**
   * Check that one or more required attributes exist for a port. No ordering is required for the
   * attributes within the schema.
   *
   * @param port Port to be checked.
   * @param attributeNames Set of required attributes.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkRequiredAttributes(StreamingData port, String... attributeNames);

  /**
   * Check that one or more attributes for a {@link StreamSchema} have the required type. No
   * ordering is required for the attributes within the schema.
   *
   * @param schema Schema to be checked.
   * @param optional True if all the attributes must exist, false if each attributes is optional but
   *     when present must have the required type.
   * @param expectedAttributes Schema object representing expected attributes.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkAttributes(
      StreamSchema schema, boolean optional, StreamSchema expectedAttributes);

  /**
   * Check that one or more attributes for a {@link StreamSchema} have the required type. No
   * ordering is required for the attributes within the schema.
   *
   * @param schema Schema to be checked.
   * @param optional True if all the attributes must exist, false if each attributes is optional but
   *     when present must have the required type.
   * @param expectedAttributes SPL tuple definition containing expected attributes.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkAttributes(StreamSchema schema, boolean optional, String expectedAttributes);

  /**
   * Check that one or more attributes for a input or output port have the required type. No
   * ordering is required for the attributes within the port's schema.
   *
   * @param port Port to be checked.
   * @param optional True if all the attributes must exist, false if each attributes is optional but
   *     when present must have the required type.
   * @param expectedAttributes Schema object representing expected attributes.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkAttributes(
      StreamingData port, boolean optional, StreamSchema expectedAttributes);

  /**
   * Check that one or more attributes for a input or output port have the required type. No
   * ordering is required for the attributes within the port's schema.
   *
   * @param port Port to be checked.
   * @param optional True if all the attributes must exist, false if each attributes is optional but
   *     when present must have the required type.
   * @param expectedAttributes SPL tuple definition containing expected attributes.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkAttributes(StreamingData port, boolean optional, String expectedAttributes);

  /**
   * Check that {@code attribute} is one of the types listed in {@code validTypes}.
   *
   * @param attribute Attribute to be checked.
   * @param validTypes Valid {@code MetaType} values that the operator supports for this attribute.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkAttributeType(Attribute attribute, MetaType... validTypes);

  /**
   * Check the streaming window for an input port is {@code tumbling}.
   *
   * @param inputPort Input port to be checked.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkTumblingWindow(StreamingInput<?> inputPort);

  /**
   * Check the streaming window for an input port is {@code sliding}.
   *
   * @param inputPort Input port to be checked.
   * @return True if invocation context is valid for this check, false otherwise.
   */
  public boolean checkSlidingWindow(StreamingInput<?> inputPort);
}
