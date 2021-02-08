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

final class FloatDelta extends DeltaPolicy<Tuple, FloatWindowedTuple> {

  private final float delta;

  FloatDelta(int attributeIndex, float delta) {
    super(attributeIndex);
    this.delta = delta;
  }

  @Override
  public boolean isWindowedTupleInWindow(FloatWindowedTuple windowedTuple, Tuple tuple) {
    return isTupleInWindow(windowedTuple, getBoundary(tuple));
  }

  @Override
  public FloatWindowedTuple getWindowedTuple(Tuple tuple) {
    return new FloatWindowedTuple(tuple, tuple.getFloat(attributeIndex));
  }

  private boolean seenATuple;
  private float nextTriggerBoundary;

  @Override
  public boolean checkTrigger(Tuple tuple) {
    final float value = tuple.getFloat(attributeIndex);

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

  float getBoundary(Tuple tuple) {
    return tuple.getFloat(attributeIndex) - delta;
  }

  private boolean isTupleInWindow(FloatWindowedTuple tuple, float boundary) {

    float attribute = tuple.getAttribute();
    return attribute >= boundary;
  }
}
