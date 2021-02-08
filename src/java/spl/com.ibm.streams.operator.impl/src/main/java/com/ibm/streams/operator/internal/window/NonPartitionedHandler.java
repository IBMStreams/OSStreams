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

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.window.StreamWindow;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

/**
 * WindowHandler that maintains a single partition.
 *
 * @param <T>
 */
final class NonPartitionedHandler<T> extends WindowHandler<T> {

  private WindowPartition<T, ?, ? extends WindowedTuple<T>, ?> singlePartition;
  private List<?> partitions;

  private static final Integer FIXED_PARTITION = 0;

  NonPartitionedHandler(
      OperatorAdapter adapter, StreamWindow<T> window, PartitionFactory<T, ?> factory) {
    super(adapter, window);
    factory.initialize(this);
    addPartition(factory.newPartition(this, FIXED_PARTITION));
  }

  @Override
  void insertIntoPartition(T tuple) throws Exception {
    singlePartition.insert(tuple);
  }

  @Override
  void markAllPartitions(Punctuation mark) throws Exception {
    singlePartition.mark(mark);
  }

  @Override
  public Collection<?> getActiveWindowPartitions() {
    return partitions;
  }

  /** No-op, never called since this is a non-partitioned window. */
  @Override
  void evictSpecificPartition(Object partition) {}

  /*
   * State Handling
   *
   */

  /** Simply remove all tuples from the window. */
  @Override
  void discardContents() {
    assert windowIsLocked();

    singlePartition.getTuples().clear();
  }

  @Override
  void addPartition(WindowPartition<T, ?, ? extends WindowedTuple<T>, ?> windowPartition) {
    assert FIXED_PARTITION.equals(windowPartition.getPartitionKey());

    singlePartition = windowPartition;
    partitions = Collections.singletonList(singlePartition);
  }
}
