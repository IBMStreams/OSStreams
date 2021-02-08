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
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

class PartitionedHandler<T> extends WindowHandler<T> {

  /** How to create new window partitions that maintain the window semantics for the partition. */
  private final PartitionFactory<T, ?> factory;

  /**
   * Dynamic Map of partition identifiers to WindowPartition. Partitions are creates as needed,
   * based upon incoming tuples. Synchronized map to allow accessing the set of partitions without
   * holding the coarse grained window lock.
   */
  final Map<Object, WindowPartition<T, ?, ? extends WindowedTuple<T>, ?>> partitions;

  PartitionedHandler(
      OperatorAdapter adapter, StreamWindow<T> window, PartitionFactory<T, ?> factory) {
    super(adapter, window);
    this.factory = factory;
    factory.initialize(this);
    partitions =
        Collections.synchronizedMap(
            new HashMap<Object, WindowPartition<T, ?, ? extends WindowedTuple<T>, ?>>());
  }

  @SuppressWarnings("unchecked")
  static <T> PartitionedHandler<T> getPartitionedHandler(
      OperatorAdapter adapter, StreamWindow<T> window, PartitionFactory<T, ?> factory) {

    switch (((InputPortWindow<T, ?, ?>) window).getPartitionEvictionPolicy()) {
      case none:
        return new PartitionedHandler<T>(adapter, window, factory);
      case partitionAge:
        return new PartitionAgeEvictionHandler<T>(adapter, window, factory);
      case tupleCount:
        return new TupleCountEvictionHandler<T>(adapter, window, factory);
      case partitionCount:
        return new PartitionCountEvictionHandler<T>(adapter, window, factory);
      default:
        throw new IllegalStateException();
    }
  }

  /**
   * Determine the partition from the incoming tuple, create a partition if required and then pass
   * the tuple onto it.
   */
  @Override
  void insertIntoPartition(T tuple) throws Exception {

    WindowPartition<T, ?, ?, ?> windowPartition;

    Object partition = getPartitioner().getPartition(tuple);

    windowPartition = getWindowPartitionForInsert(partition);

    windowPartition.insert(tuple);
  }

  /** Get the WindowPartition for the partition of the tuple to be inserted. */
  WindowPartition<T, ?, ? extends WindowedTuple<T>, ?> getWindowPartitionForInsert(Object partition)
      throws Exception {
    WindowPartition<T, ?, ? extends WindowedTuple<T>, ?> windowPartition =
        partitions.get(partition);
    if (windowPartition == null) {
      windowPartition = factory.newPartition(this, partition);
      addPartition(windowPartition);

      // Since this is driven by an insert
      // we can activate immediately.
      windowPartition.activate();
    }
    return windowPartition;
  }

  /**
   * Pass the mark on to every active partition if it's the final marker or the window eviction
   * policy is punctuation.
   */
  @Override
  final void markAllPartitions(Punctuation mark) throws Exception {
    if (mark == Punctuation.FINAL_MARKER
        || window.getEvictionPolicy() == StreamWindow.Policy.PUNCTUATION)
      for (WindowPartition<T, ?, ?, ?> windowPartition : partitions.values()) {
        windowPartition.mark(mark);
      }
  }

  /**
   * Identify the specific WindowPartition and then evict it.
   *
   * @param partition
   * @throws Exception
   */
  @Override
  void evictSpecificPartition(Object partition) throws Exception {

    final WindowPartition<T, ?, ?, ?> windowPartition;

    windowPartition = removeWindowPartition(partition);

    // No such partition.
    if (windowPartition == null) return;

    windowPartition.evictPartition();
  }

  boolean needsPartitionEviction() {
    return false;
  }

  /** Remove a WindowPartition for a partition key due to partition eviction. */
  WindowPartition<T, ?, ?, ?> removeWindowPartition(final Object partition) {
    return partitions.remove(partition);
  }

  /** Return the collection of internal WindowPartitions */
  @Override
  public Collection<?> getActiveWindowPartitions() {
    return partitions.values();
  }

  /** Return the read-only collection of user-specified partitions. */
  public Collection<?> getPartitions() {
    // This is an unordered collection of the current partition keys
    synchronized (partitions) {
      return Collections.unmodifiableCollection(new HashSet<Object>(partitions.keySet()));
    }
  }

  /*
   * State Handling
   *
   */

  @Override
  void discardContents() {
    assert windowIsLocked();

    partitions.clear();
  }

  @Override
  void addPartition(WindowPartition<T, ?, ? extends WindowedTuple<T>, ?> windowPartition) {
    partitions.put(windowPartition.getPartitionKey(), windowPartition);
  }
}
