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

package com.ibm.streams.operator.internal.state;

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.RuntimeStreamForwarder;
import java.util.concurrent.Semaphore;

/**
 * Consistent region handling of an input port that is not in the consistent region. Permits are
 * obtained for tuple and mark processing so that any submission into the region from the input
 * holds a permit.
 */
class ConsistentRegionAutonomousInput<T> extends RuntimeStreamForwarder<T> {

  private final Semaphore semaphore;

  ConsistentRegionAutonomousInput(Semaphore semaphore, RuntimeStream<T> destination) {
    super(destination);
    this.semaphore = semaphore;
  }

  @Override
  public void tuple(T tuple) throws Exception {
    semaphore.acquire();
    try {
      super.tuple(tuple);
    } finally {
      semaphore.release();
    }
  }

  @Override
  public void mark(Punctuation mark) throws Exception {
    semaphore.acquire();
    try {
      super.mark(mark);
    } finally {
      semaphore.release();
    }
  }
}
