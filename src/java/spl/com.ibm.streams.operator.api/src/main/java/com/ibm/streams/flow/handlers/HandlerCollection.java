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
import java.util.Collection;
import java.util.Collections;

/**
 * A {@code StreamHandler} that sends tuples and punctuation marks to a collection of {@code
 * StreamHandler} instances.
 *
 * @param <S> Collection element type
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public class HandlerCollection<S extends StreamHandler<T>, T> implements StreamHandler<T> {

  private final Collection<S> handlers;

  /**
   * Create a HandlerCollection. {@code Collections.synchronizedCollection} to ensure is used to
   * wrap the passed in collection, which ensures that all the callers see a consistent view of the
   * collection.
   *
   * @param handlers
   */
  public HandlerCollection(Collection<S> handlers) {
    this.handlers = Collections.synchronizedCollection(handlers);
  }

  /**
   * Get the handler collection. The collection can be modified, but modification f the collection
   * while tuples or punctuation marks are being handled can cause an exception.
   *
   * @return The handler collection.
   */
  public Collection<S> getHandlers() {
    return handlers;
  }

  /** Propagate the tuple to each handler in the collection. */
  @Override
  public final void tuple(T tuple) throws Exception {
    for (S handler : getHandlers()) handler.tuple(tuple);
  }

  /** Propagate the punctuation mark to each handler in the collection. */
  @Override
  public final void mark(Punctuation mark) throws Exception {
    for (S handler : getHandlers()) handler.mark(mark);
  }
}
