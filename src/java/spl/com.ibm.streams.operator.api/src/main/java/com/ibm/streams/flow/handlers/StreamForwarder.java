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

/**
 * Forward a stream to another {@link StreamHandler}.
 *
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public class StreamForwarder<T> extends AbstractStreamForwarder<T> {

  private final StreamHandler<T> destination;

  /**
   * Create a {@code StreamForwarder} that forwards to {@code destination}.
   *
   * @param destination Handler tuples and punctuation marks will be forwarded to.
   */
  public StreamForwarder(StreamHandler<T> destination) {
    if (destination == null) throw new NullPointerException();
    this.destination = destination;
  }
  /** {@inheritDoc} */
  public StreamHandler<T> getDestination() {
    return destination;
  }
}
