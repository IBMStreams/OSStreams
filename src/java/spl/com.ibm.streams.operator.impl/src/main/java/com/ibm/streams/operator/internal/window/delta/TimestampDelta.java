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
import com.ibm.streams.operator.types.Timestamp;

public class TimestampDelta extends ComparableDeltaPolicy<Timestamp> {

  TimestampDelta(int attributeIndex, Timestamp delta) {
    super(attributeIndex, delta);
  }

  @Override
  Timestamp getEvictBoundary(Timestamp value) {
    return value.subtract(delta);
  }

  @Override
  Timestamp getTupleAttribute(Tuple tuple) {
    return tuple.getTimestamp(attributeIndex);
  }

  @Override
  Timestamp getNextTriggerBoundary(Timestamp value) {
    return value.add(delta);
  }
}
