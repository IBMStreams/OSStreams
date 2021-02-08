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

class IntWindowedTuple extends WindowedTuple<Tuple> {

  /** */
  private static final long serialVersionUID = -1607920311654819417L;

  private final int attribute;

  IntWindowedTuple(Tuple tuple, int attribute) {
    super(tuple);
    this.attribute = attribute;
  }

  int getAttribute() {
    return attribute;
  }
}
