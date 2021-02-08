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

package com.ibm.streams.flow.handlers;

import com.ibm.streams.operator.StreamingData.Punctuation;

/**
 * Forward a stream to another {@link StreamHandler}.
 *
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public abstract class AbstractStreamForwarder<T> implements StreamHandler<T> {

  /**
   * Get the current destination for the stream.
   *
   * @return Handler that the stream is forwarded to.
   */
  public abstract StreamHandler<T> getDestination();

  /** Send the tuple to the destination that is returned by {@link #getDestination()}. */
  @Override
  public void tuple(final T tuple) throws Exception {
    getDestination().tuple(tuple);
  }

  /** Send the punctuation mark to the destination that is returned by {@link #getDestination()}. */
  @Override
  public void mark(Punctuation mark) throws Exception {
    getDestination().mark(mark);
  }
}
