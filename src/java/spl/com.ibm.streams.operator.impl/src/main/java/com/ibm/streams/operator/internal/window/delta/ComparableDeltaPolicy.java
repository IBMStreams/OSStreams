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

/**
 * Delta policy when the attribute is naturally an object type that implements Comparable.
 * Attributes that map to primitive types (int, float etc.) have a specific primitive based
 * DeltaPolicy.
 *
 * @param <A> Type of attribute.
 */
abstract class ComparableDeltaPolicy<A extends Comparable<A>>
    extends DeltaPolicy<Tuple, DeltaWindowedTuple<A>> {

  final A delta;

  ComparableDeltaPolicy(int attributeIndex, A delta) {
    super(attributeIndex);
    this.delta = delta;
  }

  /**
   * Get the eviction boundary based upon the incoming tuple.
   *
   * @param tuple
   * @return
   */
  abstract A getEvictBoundary(A value);

  abstract A getTupleAttribute(Tuple tuple);

  @Override
  public boolean isWindowedTupleInWindow(DeltaWindowedTuple<A> windowedTuple, Tuple tuple) {
    return isTupleInWindow(windowedTuple, getEvictBoundary(getTupleAttribute(tuple)));
  }

  private boolean isTupleInWindow(DeltaWindowedTuple<A> tuple, A boundry) {

    A attribute = tuple.getAttribute();
    return attribute.compareTo(boundry) >= 0;
  }

  @Override
  public final DeltaWindowedTuple<A> getWindowedTuple(Tuple tuple) {
    return new DeltaWindowedTuple<A>(tuple, getTupleAttribute(tuple));
  }

  private A nextTriggerBoundary;

  @Override
  public final boolean checkTrigger(Tuple tuple) {
    A value = getTupleAttribute(tuple);
    if (nextTriggerBoundary == null) {
      nextTriggerBoundary = getNextTriggerBoundary(value);
      return false;
    }

    if (value.compareTo(nextTriggerBoundary) > 0) {
      nextTriggerBoundary = getNextTriggerBoundary(value);
      return true;
    }

    return false;
  }

  /**
   * Return the next trigger boundary which is value plus delta.
   *
   * @param value
   * @return
   */
  abstract A getNextTriggerBoundary(A value);
}
