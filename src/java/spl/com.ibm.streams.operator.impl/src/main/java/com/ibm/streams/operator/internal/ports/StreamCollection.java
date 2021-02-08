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

import com.ibm.streams.flow.handlers.HandlerCollection;
import java.util.Collection;

public class StreamCollection<T> extends HandlerCollection<RuntimeStream<T>, T>
    implements RuntimeStream<T> {

  public StreamCollection(Collection<RuntimeStream<T>> handlers) {
    super(handlers);
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    for (RuntimeStream<T> stream : getHandlers()) stream.action(action);
    return null;
  }
}
