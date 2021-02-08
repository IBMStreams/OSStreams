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

import com.ibm.streams.flow.handlers.StreamHandler;

/**
 * A RuntimeStream is an internal representation of a stream including the additional control
 * mechanism action that is hidden from7 the Operator API (i.e. user defined code). It uses
 * StreamHandler for the visible attributes of a Stream.
 *
 * @param <T> Tuple type.
 */
public interface RuntimeStream<T> extends StreamHandler<T> {

  /**
   * Pass an action through on a Stream.
   *
   * @param action Action to be executed for the Stream.
   * @throws Exception
   */
  <A> A action(StreamAction<A> action) throws Exception;
}
