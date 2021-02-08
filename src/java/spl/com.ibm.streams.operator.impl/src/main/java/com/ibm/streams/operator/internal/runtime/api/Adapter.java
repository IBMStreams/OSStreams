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

package com.ibm.streams.operator.internal.runtime.api;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.internal.runtime.OperatorThreadFactory;

/**
 * Generic api for an instance (or invocation) of an operator. Works for all operator invocations,
 * including:
 *
 * <p>Java primitives in PE Java primitives in mock framework C++ operators in PE TODO
 */
public interface Adapter {
  // TODO
  // public OperatorContainer getContainer();

  public String getOperatorName();

  public int getOperatorIndex();

  public ExecutionContext getExecutionContext();

  public void notifyCompleter();

  public ClassLoader getClassLoader();

  public OperatorContext getContext();

  /**
   * Handle an exception thrown by a background activity for the operator.
   *
   * @return true if this exception was handled.
   */
  public boolean backgroundException(Thread t, Throwable e);

  public String getOperatorClassName();

  public OperatorThreadFactory getUserThreadFactory();

  public OperatorInfo getManagement();

  public void setActive();

  /** Is this a JavaOp (Java primitive or invocation of JavaOp). */
  public boolean isJavaOp();
}
