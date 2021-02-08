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

import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.window.StreamWindow;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/**
 * Evict partitions according to the total number of tuples in the window. State is synchronized by
 * the WindowHandler window lock.
 */
public class TupleCountEvictionHandler<T> extends OrderedPartitionHandler<T> {

  /** Volatile to ensure that needsPartitionEviction() sees a current value. */
  private volatile long totalTupleCount;

  TupleCountEvictionHandler(
      OperatorAdapter adapter, StreamWindow<T> window, PartitionFactory<T, ?> factory) {
    super(adapter, window, factory);
  }

  /** */
  boolean needsPartitionEviction() {
    return totalTupleCount > evictionConfiguration;
  }

  @Override
  void discardContents() {
    super.discardContents();
    totalTupleCount = 0;
  }

  @Override
  void checkpointHandler(ObjectOutputStream out) throws IOException {
    super.checkpointHandler(out);
    out.writeLong(totalTupleCount);
  }

  @Override
  void resetHandler(ObjectInputStream in) throws IOException, ClassNotFoundException {
    super.resetHandler(in);
    totalTupleCount = in.readLong();
  }

  /** Tuple eviction. */
  @Override
  void evictTupleCount(final int evictedCount) {
    assert windowIsLocked();

    totalTupleCount -= evictedCount;
  }

  /** Partition eviction */
  @Override
  WindowPartition<T, ?, ?, ?> removeWindowPartition(final Object partition) {
    assert windowIsLocked();

    final WindowPartition<T, ?, ?, ?> windowPartition = super.removeWindowPartition(partition);
    if (windowPartition != null) totalTupleCount -= windowPartition.getTuples().size();
    return windowPartition;
  }

  @Override
  void insertIntoPartition(T tuple) throws Exception {
    assert windowIsLocked();

    final boolean evictPartitions;
    totalTupleCount++;
    evictPartitions = needsPartitionEviction();

    super.insertIntoPartition(tuple);
    if (evictPartitions) countBasedEviction();
  }
}
