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

import com.ibm.streams.operator.internal.window.InputPortWindow;
import com.ibm.streams.operator.internal.window.WindowHandler;
import com.ibm.streams.operator.internal.window.WindowedTuple;
import com.ibm.streams.operator.internal.window.delta.DeltaPolicy;

/**
 * Tumbling delta based window.
 *
 * <p><I> From the SPL toolkit developers guide </I> <br>
 * For a tumbling window, when used to characterize an eviction policy, the size defines the
 * boundary of a full window in the units of the eviction policy attribute. When a new tuple is
 * received, if the delta between its eviction policy attribute value and that of the oldest tuple
 * in the window is larger than the eviction policy size, then the whole window is flushed (mass
 * eviction). The new tuple is inserted into the window after the flush operation (if any) is
 * performed.
 *
 * @param <T> Tuple type
 */
final class TumblingDeltaEvict<T, P> extends TumblingWindow<T, P> {

  private static final long serialVersionUID = -2247241587925348460L;
  /** How tuples are evicted due to the delta policy. */
  private final DeltaPolicy<T, WindowedTuple<T>> eviction;

  @SuppressWarnings("unchecked")
  TumblingDeltaEvict(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);
    eviction =
        ((InputPortWindow<T, DeltaPolicy<T, WindowedTuple<T>>, ?>) windowHandler.getStreamWindow())
            .getEvictionPolicyConfig();
  }

  /**
   * Our WindowedTuple implementation also holds the value of the attribute used for delta
   * comparison.
   */
  protected WindowedTuple<T> getWindowTuple(T tuple) {
    return eviction.getWindowedTuple(tuple);
  }

  /**
   * The incoming tuple defines a new boundary set to the attribute(tuple) - delta. If the oldest
   * tuple in the window is outside the boundary then the window tumbles, before the incoming tuple
   * is inserted.
   */
  @Override
  protected void insert(T tuple) throws Exception {

    if (!getTuples().isEmpty()) {
      if (!eviction.isWindowedTupleInWindow(getTuples().get(0), tuple)) tumble();
    }

    super.insert(tuple);
  }
}
