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
import java.math.BigDecimal;
import java.math.MathContext;

final class DecimalDelta extends ComparableDeltaPolicy<BigDecimal> {

  private final MathContext context;

  DecimalDelta(int attributeIndex, BigDecimal delta, MathContext context) {
    super(attributeIndex, delta);
    this.context = context;
  }

  @Override
  BigDecimal getEvictBoundary(BigDecimal value) {
    return value.subtract(delta, context);
  }

  @Override
  BigDecimal getTupleAttribute(Tuple tuple) {
    return tuple.getBigDecimal(attributeIndex);
  }

  @Override
  BigDecimal getNextTriggerBoundary(BigDecimal value) {
    return value.add(delta, context);
  }
}
