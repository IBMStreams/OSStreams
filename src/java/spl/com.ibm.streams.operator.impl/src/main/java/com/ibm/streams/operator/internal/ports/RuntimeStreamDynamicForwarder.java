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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.flow.handlers.DynamicForwarder;

public class RuntimeStreamDynamicForwarder<T> extends DynamicForwarder<T>
    implements RuntimeStream<T> {

  public RuntimeStreamDynamicForwarder() {
    super();
  }

  public RuntimeStreamDynamicForwarder(RuntimeStream<T> handler) {
    super(handler);
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    if (getDestination() instanceof RuntimeStream)
      return ((RuntimeStream<T>) this.getDestination()).action(action);
    return null;
  }
}
