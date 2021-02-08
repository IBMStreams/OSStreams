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
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Deque;
import java.util.LinkedList;
import java.util.List;

/**
 * Partition handler that maintains an LRU order of partitions based upon the last tuple insertion.
 * Has support methods for LRU based partition eviction.
 *
 * @param <T> Tuple type
 */
public abstract class OrderedPartitionHandler<T> extends PartitionedHandler<T> {

  /** Lower case names to match the name tokens returned from Perl for the C++ code generation. */
  public enum PartitionEvictionPolicy {
    none,
    partitionAge,
    partitionCount,
    tupleCount
  }

  /**
   * list of partitions in time (LRU) order. Synchronized list to allow a consistent view for
   * getPartitions() without holding the coarse grained window lock.
   */
  final Deque<Object> orderedPartitions;

  final long evictionConfiguration;

  @SuppressWarnings("unchecked")
  OrderedPartitionHandler(
      OperatorAdapter adapter, StreamWindow<T> window, PartitionFactory<T, ?> factory) {
    super(adapter, window, factory);

    orderedPartitions = new LinkedList<Object>();

    evictionConfiguration = ((InputPortWindow<T, ?, ?>) window).getPartitionEvictionConfiguration();
  }

  /**
   * Return a read only copy of the partitions in LRU order, removing the time marker if it exists.
   */
  @Override
  public Collection<?> getPartitions() {

    final ArrayList<Object> userList;
    synchronized (orderedPartitions) {
      userList = new ArrayList<Object>(orderedPartitions.size());
      userList.addAll(orderedPartitions);
    }

    return Collections.unmodifiableList(userList);
  }

  /** Maintain the LRU order of partitions. synchronized by caller. */
  @Override
  WindowPartition<T, ?, ? extends WindowedTuple<T>, ?> getWindowPartitionForInsert(Object partition)
      throws Exception {

    synchronized (orderedPartitions) {
      drainOldAgeMarkers();

      final Object last = orderedPartitions.peekLast();
      if (last == partition || (last != null && last.equals(partition))) {
        // Already the last element, nothing to do!
      } else {
        // Move to the end of the list
        orderedPartitions.remove(partition);
        orderedPartitions.addLast(partition);
      }
    }

    return super.getWindowPartitionForInsert(partition);
  }

  /**
   * Drain any age markers that are of no use in the LRU list, only implemented by partition age.
   */
  void drainOldAgeMarkers() {}

  /** Remove the partition from the ordered list as well. */
  @Override
  WindowPartition<T, ?, ?, ?> removeWindowPartition(final Object partition) {
    final WindowPartition<T, ?, ?, ?> windowPartition = super.removeWindowPartition(partition);

    synchronized (orderedPartitions) {
      if (windowPartition != null) {
        orderedPartitions.remove(partition);
      }
    }

    return windowPartition;
  }

  @Override
  void checkpointHandler(ObjectOutputStream out) throws IOException {
    super.checkpointHandler(out);
    synchronized (orderedPartitions) {
      drainOldAgeMarkers();
      out.writeObject(orderedPartitions);
    }
  }

  @Override
  void discardContents() {
    assert windowIsLocked();

    super.discardContents();
    synchronized (orderedPartitions) {
      orderedPartitions.clear();
    }
  }

  @Override
  void resetHandler(ObjectInputStream in) throws IOException, ClassNotFoundException {
    super.resetHandler(in);

    @SuppressWarnings("unchecked")
    List<Object> savedOrder = (List<Object>) in.readObject();
    synchronized (orderedPartitions) {
      orderedPartitions.addAll(savedOrder);
    }
  }

  /** Evict all empty oldest partitions up to but not including the first non-empty partition. */
  private void evictEmptyPartitions() throws Exception {
    assert windowIsLocked();

    for (; ; ) {
      final Object oldest = getOldestPartitionForEviction();
      if (oldest == null) break;

      if (!partitions.get(oldest).getTuples().isEmpty()) break;

      evictSpecificPartition(oldest);
    }
  }

  /**
   * Evict partitions according to the LRU scheme when partition eviction is tupleCount or
   * partitionCount based.
   */
  void countBasedEviction() throws Exception {

    assert windowIsLocked();

    evictEmptyPartitions();

    do {

      Object oldest;
      if (!needsPartitionEviction()) break;
      oldest = getOldestPartitionForEviction();
      if (oldest == null) break;
      evictSpecificPartition(oldest);
    } while (needsPartitionEviction());
  }
  /**
   * Get the oldest partition key for partition eviction. If the oldest partition is the time marker
   * then it is removed and null returned.
   *
   * @return Partition to be evicted, or null if nothing to be evicted.
   */
  Object getOldestPartitionForEviction() {

    assert windowIsLocked();

    synchronized (orderedPartitions) {
      return orderedPartitions.peekFirst();
    }
  }
}
