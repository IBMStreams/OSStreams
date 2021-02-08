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

package com.ibm.streams.operator.internal.window.delta;

import com.ibm.streams.operator.Tuple;

class IntDelta extends DeltaPolicy<Tuple, IntWindowedTuple> {

  private final int delta;

  IntDelta(int attributeIndex, int delta) {
    super(attributeIndex);
    this.delta = delta;
  }

  @Override
  public boolean isWindowedTupleInWindow(IntWindowedTuple windowedTuple, Tuple tuple) {
    return isTupleInWindow(windowedTuple, getBoundary(tuple));
  }

  @Override
  public IntWindowedTuple getWindowedTuple(Tuple tuple) {
    return new IntWindowedTuple(tuple, tuple.getInt(attributeIndex));
  }

  private boolean seenATuple;
  private int nextTriggerBoundary;

  @Override
  public boolean checkTrigger(Tuple tuple) {
    final int value = tuple.getInt(attributeIndex);

    if (!seenATuple) {
      seenATuple = true;
      nextTriggerBoundary = value + delta;
      return false;
    }

    if (value > nextTriggerBoundary) {
      nextTriggerBoundary = value + delta;
      return true;
    }
    return false;
  }

  int getBoundary(Tuple tuple) {
    return tuple.getInt(attributeIndex) - delta;
  }

  private boolean isTupleInWindow(IntWindowedTuple tuple, int boundary) {

    int attribute = tuple.getAttribute();
    return attribute >= boundary;
  }
}
