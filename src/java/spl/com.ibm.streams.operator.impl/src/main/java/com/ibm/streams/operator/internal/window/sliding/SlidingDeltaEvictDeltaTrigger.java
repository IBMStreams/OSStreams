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

package com.ibm.streams.operator.internal.window.sliding;

import com.ibm.streams.operator.internal.window.InputPortWindow;
import com.ibm.streams.operator.internal.window.WindowHandler;
import com.ibm.streams.operator.internal.window.WindowedTuple;
import com.ibm.streams.operator.internal.window.delta.DeltaPolicy;

/** @param <T> Tuple type. */
final class SlidingDeltaEvictDeltaTrigger<T, P> extends SlidingDeltaEvict<T, P> {

  private static final long serialVersionUID = 5259128747825341217L;
  private final DeltaPolicy<T, WindowedTuple<T>> trigger;

  @SuppressWarnings("unchecked")
  SlidingDeltaEvictDeltaTrigger(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);
    trigger =
        ((InputPortWindow<T, ?, DeltaPolicy<T, WindowedTuple<T>>>) windowHandler.getStreamWindow())
            .getTriggerPolicyConfig();
  }

  @Override
  protected void insert(T tuple) throws Exception {

    checkDeltaTrigger(trigger, tuple);

    super.insert(tuple);
  }
}
