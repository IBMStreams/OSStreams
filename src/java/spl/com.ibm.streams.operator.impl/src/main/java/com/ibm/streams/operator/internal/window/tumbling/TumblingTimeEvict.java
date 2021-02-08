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

package com.ibm.streams.operator.internal.window.tumbling;

import com.ibm.streams.operator.internal.window.WindowHandler;

/** Each partition has its own timer scheduled task for tumbling the window. */
class TumblingTimeEvict<T, P> extends TumblingWindow<T, P> {

  private static final long serialVersionUID = -5062455250910660708L;

  private final PeriodicTimerPolicy evictPolicy;

  TumblingTimeEvict(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);
    evictPolicy =
        new PeriodicTimerPolicy(windowHandler.getPolicyTimeMilliseconds(true)) {
          private static final long serialVersionUID = -1485934557298004358L;

          @Override
          protected void doTimedEvent() throws Exception {
            tumble();
          }
        };
  }

  /**
   * Setup a periodic eviction timer for this partition. Includes supporting the correct delay
   * before the next eviction after a timeout.
   */
  protected void activate() {
    evictPolicy.activate();
  }

  /**
   * Stop the repeating task from tumbling the window. At this point we hold the window lock so
   * there cannot be an active task tumbling the window, but it could be blocked waiting to get the
   * lock.
   */
  @Override
  protected void deactivate() {
    evictPolicy.deactivate();
  }
}
