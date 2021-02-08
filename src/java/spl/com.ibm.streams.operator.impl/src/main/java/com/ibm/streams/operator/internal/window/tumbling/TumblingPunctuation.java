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

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.internal.window.WindowHandler;

final class TumblingPunctuation<T, P> extends TumblingWindow<T, P> {

  private static final long serialVersionUID = 5564192264280856223L;

  TumblingPunctuation(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);
  }

  /** Tumble the window at each punctuation mark. */
  @Override
  protected void mark(Punctuation mark) throws Exception {
    if (mark == Punctuation.WINDOW_MARKER) tumble();
    else super.mark(mark);
  }
}
