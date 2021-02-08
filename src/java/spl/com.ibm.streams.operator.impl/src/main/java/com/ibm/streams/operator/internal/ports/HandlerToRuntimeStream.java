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

import com.ibm.streams.flow.handlers.StreamForwarder;
import com.ibm.streams.flow.handlers.StreamHandler;

/**
 * Class to wrap a StreamHandler provided by the user, e.g. through the mock framework into a
 * RuntimeStream for internal use.
 */
public final class HandlerToRuntimeStream<T> extends StreamForwarder<T>
    implements RuntimeStream<T> {

  public HandlerToRuntimeStream(StreamHandler<T> destination) {
    super(destination);
  }

  /** Discard the action, as they do not escape into user code (StreamHandlers). */
  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    return null;
  }
}
