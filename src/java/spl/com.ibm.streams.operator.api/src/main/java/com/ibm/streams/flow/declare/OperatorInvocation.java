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

package com.ibm.streams.flow.declare;

import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.model.PrimitiveOperator;
import java.math.BigDecimal;

/**
 * Declaration of an invocation of a Java operator in an {@link OperatorGraph}.
 *
 * <p>If {@code T} is annotated with {@link PrimitiveOperator} then the invocation of the operator
 * must be consistent with the model defined by its annotations.
 *
 * @param <T> Operator implementation type.
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface OperatorInvocation<T> {

  /**
   * Set a parameter to a list of {@code String} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setStringParameter(String parameterName, String... values);

  /**
   * Set a parameter to a list of {@code int} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setIntParameter(String parameterName, int... values);

  /**
   * Set a parameter to a list of {@code boolean} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setBooleanParameter(String parameterName, boolean... values);
  /**
   * Set a parameter to a list of {@code byte} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setByteParameter(String parameterName, byte... values);
  /**
   * Set a parameter to a list of {@code short} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setShortParameter(String parameterName, short... values);
  /**
   * Set a parameter to a list of {@code long} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setLongParameter(String parameterName, long... values);
  /**
   * Set a parameter to a list of {@code float} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setFloatParameter(String parameterName, float... values);
  /**
   * Set a parameter to a list of {@code double} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setDoubleParameter(String parameterName, double... values);
  /**
   * Set a parameter to a list of {@code BigDecimal} values.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setBigDecimalParameter(String parameterName, BigDecimal... values);
  /**
   * Set a parameter to a list of {@code Attribute} names as {@code String} values. The values must
   * be specified as <I>InputPortName.AttributeName</I> or as <I>AttributeName</I> if the attribute
   * name is unique in the schema. An {@code IllegalArgumentException} will be thrown if a valid
   * attribute with the given name does not exist.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setAttributeParameter(String parameterName, String... values);
  /**
   * Set a custom literal parameter to a list of {@code Enum} values. Note that the {@code
   * Enum#name()} method must return a string that is accepted by this custom literal parameter.
   *
   * @param parameterName Parameter name.
   * @param values List of values.
   * @return this
   */
  public OperatorInvocation<T> setCustomLiteralParameter(String parameterName, Enum<?>... values);

  /**
   * Declare an output port for this operator. This method is equivalent to the {@link
   * OperatorInvocation#addOutput(String, StreamSchema)} method after the SPL representation of
   * {@code schema} is converted to a {@link StreamSchema} object.
   *
   * @param name Name of the output port.
   * @param schema Schema of the output port as a string that uses SPL syntax.
   * @return An {@link OutputPortDeclaration} that represents the output port.
   * @see com.ibm.streams.operator.Type.Factory#getTupleType(String)
   */
  public OutputPortDeclaration addOutput(String name, String schema);

  /**
   * Declare an output port for this operator. The index of this output port is equal to the number
   * of previously declared output ports for this invocation.
   *
   * @param name Name of the output port.
   * @param schema Schema of the output port.
   * @return An {@link OutputPortDeclaration} that represents the output port.
   */
  public OutputPortDeclaration addOutput(String name, StreamSchema schema);

  /**
   * Declare an output port for this operator and generate the name of the port. This method is
   * equivalent to {@link OperatorInvocation#addOutput(StreamSchema)} method after the SPL
   * representation of {@code schema} is converted to a {@link StreamSchema} object
   *
   * @param schema Schema of the output port.
   * @return An {@link OutputPortDeclaration} that represents the output port.
   * @see com.ibm.streams.operator.Type.Factory#getTupleType(String)
   */
  public OutputPortDeclaration addOutput(String schema);

  /**
   * Declare an output port for this operator and generate the name of the port. This method is
   * equivalent to {@link OperatorInvocation#addOutput(String, StreamSchema)} with a generated name
   * that is unique within this invocation.
   *
   * @param schema Schema of the output port.
   * @return An {@link OutputPortDeclaration} that represents the output port.
   */
  public OutputPortDeclaration addOutput(StreamSchema schema);

  /**
   * Declare an input port for this operator. The index of this input port is equal to the number of
   * previously declared input ports for this invocation.
   *
   * @param name Name of the input port.
   * @param schema Schema of the input port.
   * @return An {@link InputPortDeclaration} that represents the input port.
   */
  public InputPortDeclaration addInput(String name, StreamSchema schema);

  /**
   * Declare an input port for this operator. This method is equivalent to {@link
   * OperatorInvocation#addOutput(String, StreamSchema)} method after the SPL representation of
   * {@code schema} is converted to a {@link StreamSchema} object
   *
   * @param name Name of the input port.
   * @param schema Schema of the input port as a string that uses SPL syntax.
   * @return An {@link InputPortDeclaration} that represents the input port.
   * @see com.ibm.streams.operator.Type.Factory#getTupleType(String)
   */
  public InputPortDeclaration addInput(String name, String schema);

  /**
   * Declare an input port for this operator and generate the name of the port. This method is
   * equivalent to {@link OperatorInvocation#addInput(StreamSchema)} method after the SPL
   * representation of {@code schema} is converted to a {@link StreamSchema} object
   *
   * @param schema Schema of the input port.
   * @return An {@link InputPortDeclaration} that represents the input port.
   * @see com.ibm.streams.operator.Type.Factory#getTupleType(String)
   */
  public InputPortDeclaration addInput(String schema);

  /**
   * Declare an input port for this operator and generate the name of the port. This method is
   * equivalent to {@link OperatorInvocation#addOutput(String, StreamSchema)} with a generated name
   * that is unique within this invocation.
   *
   * @param schema Schema of the input port.
   * @return An {@link InputPortDeclaration} that represents the input port.
   */
  public InputPortDeclaration addInput(StreamSchema schema);

  /**
   * Declare an input port that is connected to an output port. <br>
   * The name of the input port is equal to {@code outputPort.getName()}. <br>
   * The schema of the input port is equal to {@code outputPort.getStreamSchema()}. <br>
   * The index of this input port is equal to the number of previously declared input ports for this
   * invocation.
   *
   * @param outputPort Output port to connect to.
   * @return An {@link InputPortDeclaration} that represents the input port.
   */
  public InputPortDeclaration addInput(OutputPortDeclaration outputPort);

  /**
   * Run compile checks against this invocation.
   *
   * @param verbose
   * @return True if the context is valid, which means all the checks passed. Otherwise false.
   * @throws Exception Exception thrown by a compile time check.
   */
  public boolean compileChecks(boolean verbose) throws Exception;

  /**
   * Run compile checks against this invocation. Equivalent to calling {@link
   * #compileChecks(boolean)} and passing {@code false}.
   *
   * @return True if the context is valid, which means all the checks passed. Otherwise false.
   * @throws Exception Exception thrown by a compile time check.
   */
  public boolean compileChecks() throws Exception;

  /**
   * Get the complete name of the operator.
   *
   * @return complete name of the operator.
   */
  public String getName();

  /**
   * Return the graph for this operator invocation.
   *
   * @return the graph for this operator invocation
   */
  public OperatorGraph graph();

  /**
   * Get the class representing this operator invocation.
   *
   * @return Class representing this operator invocation.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public Class<T> getOperatorClass();
}
