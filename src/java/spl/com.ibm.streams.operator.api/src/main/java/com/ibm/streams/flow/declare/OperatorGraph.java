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

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.ProcessingElement;

/**
 * {@code OperatorGraph} represents a declaration of a flow graph that contains Java primitive
 * operators.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface OperatorGraph {

  /**
   * Declare an invocation of an {@link Operator} in this graph. This method is equivalent to
   * calling {@link OperatorGraph#addOperator(String, Class)} with a generated name that is unique
   * to this graph.
   *
   * @param <T> Type of the operator class
   * @param opClass Class of the operator
   * @return {@link OperatorInvocation} for this invocation
   */
  public <T extends Operator> OperatorInvocation<T> addOperator(Class<T> opClass);

  /**
   * Declare an invocation of an {@link Operator} in this graph.
   *
   * @param <T> Type of the operator class
   * @param name Invocation name of the operator.
   * @param opClass Class of the operator
   * @return {@link OperatorInvocation} for this invocation
   * @exception IllegalArgumentException This graph already contains an operator named {@code name}
   */
  public <T extends Operator> OperatorInvocation<T> addOperator(String name, Class<T> opClass);

  /**
   * Run compile checks for all the declared operators in the graph. The compile time environment
   * for the checks simulates an SPL standalone application. <br>
   * During the compile, the {@link OperatorContext} has the following settings:
   *
   * <UL>
   *   <LI><B>{@link OperatorContext#getToolkitDirectory() toolkit directory}</B> - {@code File}
   *       that represents the Java system property named {@code user.dir}, which is the current
   *       working directory. This is is the same value as the {@link
   *       ProcessingElement#getApplicationDirectory() application directory}.
   * </UL>
   *
   * During the compile, the {@link ProcessingElement} has the following settings:
   *
   * <UL>
   *   <LI><B>{@link ProcessingElement#getInstanceId() instance identifier}</B> - The value of the
   *       Java system property named {@code user.name}, which is the current user identifier for
   *       the operating system.
   *   <LI><B>{@link ProcessingElement#getApplicationDirectory() application directory}</B> - {@code
   *       File} that represents the Java system property named {@code user.dir}, which is the
   *       current working directory.
   *   <LI><B>{@link ProcessingElement#getDataDirectory() data directory}</B> - {@code File} that
   *       represents the subdirectory named {@code data} in the {@link
   *       ProcessingElement#getApplicationDirectory() application directory}. This directory might
   *       not exist and is not created by the creation of this {@link OperatorGraph}, the execution
   *       of this method, or the execution of the {@link OperatorInvocation#compileChecks(boolean)}
   *       method.
   *   <LI><B>{@link ProcessingElement#getOutputDirectory() output directory}</B> - {@code File}
   *       that represents the subdirectory named {@code output} in the {@link
   *       ProcessingElement#getApplicationDirectory() application directory}. This directory might
   *       not exist and is not created by the creation of this {@link OperatorGraph}, the execution
   *       of this method, or the execution of the {@link OperatorInvocation#compileChecks(boolean)}
   *       method.
   *   <LI><B>{@link ProcessingElement#getApplicationScope() application scope}</B> - The value
   *       {@code Default}.
   * </UL>
   *
   * @return True if the context is valid, which means that the all {@link ContextCheck checks}
   *     passed. Otherwise, false.
   * @throws Exception The exception thrown by a compile time check.
   */
  public boolean compileChecks() throws Exception;
}
