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

package com.ibm.streams.operator.internal.window;

/**
 * Factory that creates a new instance of a window partition for a partition.
 *
 * @param <T> Type of the tuple.
 * @param <WP> Type of the partition.
 */
public abstract class PartitionFactory<
    T, WP extends WindowPartition<T, ?, ? extends WindowedTuple<T>, ?>> {

  /** Initialize this partition, by default do nothing. */
  public void initialize(WindowHandler<T> handler) {}
  /**
   * Create a new instance that handles the eviction and trigger policies correctly.
   *
   * @param handler
   * @return
   */
  public abstract WP newPartition(WindowHandler<T> handler, Object partition);
}
