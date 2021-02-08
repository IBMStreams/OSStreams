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

import com.ibm.streams.operator.internal.window.WindowHandler;

/**
 * Sliding window with count based eviction and time based trigger. Tuple insertion is handled by
 * super-class and trigger is a timer independent of insertion.
 *
 * @param <T> Tuple type
 */
final class SlidingDeltaEvictTimeTrigger<T, P> extends SlidingDeltaEvict<T, P> {

  private static final long serialVersionUID = -2868168917454625673L;
  private final PeriodicTimerPolicy triggerPolicy;

  SlidingDeltaEvictTimeTrigger(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);
    triggerPolicy = getTimedTriggerPolicy();
  }

  @Override
  protected void activate() throws Exception {
    super.activate();
    triggerPolicy.activate();
  }

  @Override
  protected void deactivate() {
    super.deactivate();
    triggerPolicy.deactivate();
  }
}
