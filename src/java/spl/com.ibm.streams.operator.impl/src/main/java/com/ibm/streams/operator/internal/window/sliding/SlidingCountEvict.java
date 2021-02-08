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
import com.ibm.streams.operator.internal.window.WindowedTuple;

/**
 * Abstract class for sliding windows with a count based eviction policy. When the window is full
 * (defined by the size of the eviction policy) then any new incoming tuple will cause eviction of a
 * single tuple before the new tuple is inserted into the window.
 *
 * @param <T>
 */
abstract class SlidingCountEvict<T, P> extends SlidingWindow<T, P, WindowedTuple<T>> {

  private static final long serialVersionUID = 302430094959509402L;
  private final int windowSize;

  SlidingCountEvict(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);

    this.windowSize = windowHandler.getPolicyCount(true);
  }

  /** If the windowSize size is less than the default eviction capacity then decrease to that. */
  @Override
  protected int evictionCapacity() {
    return Math.min(super.evictionCapacity(), windowSize);
  }

  /**
   * Insert the tuple into the window, first evicting a single tuple if the window is full. The
   * first time the window becomes full initialFull() is called.
   */
  @Override
  protected void insert(T tuple) throws Exception {
    boolean possibleInitialFull = true;
    if (getTuples().size() == windowSize) {
      evictOldestTuple();
      evictEvent();
      possibleInitialFull = false;
    }
    super.insert(tuple);

    if (possibleInitialFull && (getTuples().size() == windowSize)) {
      initialFull();
    }
  }
}
