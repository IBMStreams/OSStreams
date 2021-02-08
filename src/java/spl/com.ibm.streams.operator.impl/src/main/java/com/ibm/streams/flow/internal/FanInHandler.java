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

package com.ibm.streams.flow.internal;

import com.ibm.streams.flow.handlers.StreamHandler;
import com.ibm.streams.operator.StreamingData.Punctuation;

/** Handle fan-in by only forwarding a final marker once the required number have been seen. */
final class FanInHandler<T> implements StreamHandler<T> {

  private final StreamHandler<T> handler;
  private int outstandingFinalMarks;

  FanInHandler(StreamHandler<T> handler, int fanInCount) {
    this.handler = handler;
    this.outstandingFinalMarks = fanInCount;
  }

  @Override
  public void tuple(T tuple) throws Exception {
    handler.tuple(tuple);
  }

  @Override
  public void mark(Punctuation mark) throws Exception {

    synchronized (this) {
      if (mark == Punctuation.FINAL_MARKER) {
        if (--outstandingFinalMarks > 0) return;
      }
    }
    handler.mark(mark);
  }
}
