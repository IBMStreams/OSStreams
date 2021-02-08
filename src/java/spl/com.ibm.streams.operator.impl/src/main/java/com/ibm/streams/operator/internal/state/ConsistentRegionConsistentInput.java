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

import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.RuntimeStreamForwarder;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.state.ConsistentRegionContextImpl.BlockableSemaphore;

/**
 * Consistent region handling of an input port that is in the consistent region. Permits are not
 * obtained for tuple and mark processing.
 *
 * <p>RESUME_SUBMISSION is handled to unblock the semaphore for permits that are acquired by
 * background threads or autonomous ports.
 */
class ConsistentRegionConsistentInput<T> extends RuntimeStreamForwarder<T> {

  private final BlockableSemaphore semaphore;

  ConsistentRegionConsistentInput(BlockableSemaphore semaphore, RuntimeStream<T> destination) {
    super(destination);
    this.semaphore = semaphore;
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    switch (action.getType()) {
      case RESUME_SUBMISSION:
        semaphore.unblock();
        break;
      default:
        break;
    }
    return super.action(action);
  }
}
