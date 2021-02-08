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
import com.ibm.streams.operator.internal.window.WindowedTuple;

/**
 * Windowed tuple that keeps the attribute used for delta calculation. The attribute is kept
 * separately to allow the window listener to discard the tuple reference. This generic class
 * supports any attribute that implements Comparable. When the attribute maps to a primitive type
 * (int, float etc.) we use a specific policy and WindowedTuple implementation.
 *
 * @param <A> Object type of the attribute.
 */
class DeltaWindowedTuple<A extends Comparable<A>> extends WindowedTuple<Tuple> {

  /** */
  private static final long serialVersionUID = 173115972345321316L;

  private final A attribute;

  DeltaWindowedTuple(Tuple tuple, A attribute) {
    super(tuple);
    this.attribute = attribute;
  }

  A getAttribute() {
    return attribute;
  }
}
