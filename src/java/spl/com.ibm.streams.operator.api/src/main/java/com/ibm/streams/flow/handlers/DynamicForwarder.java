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
 * Forward a stream to another {@link StreamHandler}. The destination for the forwarded stream can
 * be changed by calling {@link #setDestination(StreamHandler)}.
 *
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public class DynamicForwarder<T> extends AbstractStreamForwarder<T> {

  private StreamHandler<T> destination;

  /** Create a {@code DynamicForwarder} with the destination set to a {@link StreamDiscarder}. */
  public DynamicForwarder() {
    this(new StreamDiscarder<T>());
  }

  /** Create a {@code DynamicForwarder} with the destination set to a {@code handler}. */
  public DynamicForwarder(StreamHandler<T> handler) {
    setDestination(handler);
  }

  /**
   * Set the destination for the stream.
   *
   * @param destination
   */
  public synchronized void setDestination(StreamHandler<T> destination) {
    this.destination = destination;
  }

  /** {@inheritDoc} */
  public synchronized StreamHandler<T> getDestination() {
    return destination;
  }
}
