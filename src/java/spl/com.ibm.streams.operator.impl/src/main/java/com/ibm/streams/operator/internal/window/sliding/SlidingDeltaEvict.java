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

/**
 * Sliding delta eviction based window.
 *
 * <p><I> From the SPL toolkit developers guide </I> <br>
 * For a sliding window, when used to characterize an eviction policy, the size defines the boundary
 * of the window, as it slides, in the units of the eviction policy attribute. When a new tuple is
 * received, existing tuples in the window for which the delta between the eviction policy attribute
 * value of the new tuple and that of the existing tuple is greater than the eviction policy size,
 * are evicted. The new tuple is inserted into the window after the evictions (if any) are
 * performed.
 *
 * @param <T> Tuple type.
 */
abstract class SlidingDeltaEvict<T, P> extends SlidingWindow<T, P, WindowedTuple<T>> {

  private static final long serialVersionUID = 4842337750235350257L;

  /** * How tuples are evicted due to the delta policy. */
  private final DeltaPolicy<T, WindowedTuple<T>> eviction;

  @SuppressWarnings("unchecked")
  SlidingDeltaEvict(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);
    eviction =
        ((InputPortWindow<T, DeltaPolicy<T, WindowedTuple<T>>, ?>) windowHandler.getStreamWindow())
            .getEvictionPolicyConfig();
  }

  /**
   * Our WindowedTuple implementation records the value of the attribute used for delta processing.
   */
  @Override
  protected final WindowedTuple<T> getWindowTuple(T tuple) {
    return eviction.getWindowedTuple(tuple);
  }

  /**
   * Evict all the oldest tuples that fall outside of the window defined by the attribute value for
   * the new incoming tuple.
   */
  @Override
  protected void insert(T tuple) throws Exception {

    boolean possibleInitialFull = true;
    for (; ; ) {

      WindowedTuple<T> oldestTuple = getTuples().peekFirst();
      if (oldestTuple == null) break;

      if (eviction.isWindowedTupleInWindow(oldestTuple, tuple)) break;

      if (possibleInitialFull) initialFull();

      possibleInitialFull = false;

      evictOldestTuple();
    }
    evictEvent();

    super.insert(tuple);
  }
}
