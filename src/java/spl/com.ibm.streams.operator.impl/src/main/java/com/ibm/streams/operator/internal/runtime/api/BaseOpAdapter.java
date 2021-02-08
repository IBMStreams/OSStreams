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

import com.ibm.streams.operator.OptionalContext;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.runtime.OperatorThreadFactory;
import java.util.HashMap;
import java.util.Map;

abstract class BaseOpAdapter implements Adapter {

  /** Static information about this invocation. */
  private final OpInvocationInfo invokeInfo;

  private OperatorThreadFactory threadFactory;

  /** SPL optional contexts, mainly driven by annotations. */
  private final Map<Class<? extends OptionalContext>, OptionalContext> optionalContexts =
      new HashMap<Class<? extends OptionalContext>, OptionalContext>();

  // TODO TEMP
  BaseOpAdapter(OpInvocationInfo invokeInfo) {
    this.invokeInfo = invokeInfo;
  }

  BaseOpAdapter(PEModel model, int operatorIndex) {
    this.invokeInfo = new OpInvocationInfo(model, operatorIndex);
  }

  @Override
  public final int getOperatorIndex() {
    return invokeInfo.getIndex();
  }

  @Override
  public final String getOperatorName() {
    return invokeInfo.getName();
  }

  @Override
  public final void setActive() {
    SPLRuntime.setActive(this);
  }

  // TODO - dynamically create thread factory.
  @Override
  public synchronized OperatorThreadFactory getUserThreadFactory() {
    return (OperatorThreadFactory) getContext().getThreadFactory();
  }

  protected OpInvocationInfo getInvokeInfo() {
    return invokeInfo;
  }

  public synchronized <O extends OptionalContext> O addOptionalContext(
      Class<O> contextType, O context) {
    optionalContexts.put(contextType, context);
    return context;
  }

  protected synchronized <O extends OptionalContext> O optionalContext(Class<O> contextType) {

    OptionalContext ac = optionalContexts.get(contextType);
    if (ac != null) return contextType.cast(ac);
    return null;
  }
}
