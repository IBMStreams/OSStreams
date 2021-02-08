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
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

/** Evict partitions by age. */
final class PartitionAgeEvictionHandler<T> extends OrderedPartitionHandler<T> {

  private transient ScheduledFuture<?> ageTimer;

  PartitionAgeEvictionHandler(
      OperatorAdapter adapter, StreamWindow<T> window, PartitionFactory<T, ?> factory) {
    super(adapter, window, factory);
  }

  @Override
  void activatePartitions() throws Exception {
    super.activatePartitions();

    long period = evictionConfiguration / 10;
    if (period <= 500) period = 500;
    else if (period < TimeUnit.SECONDS.toMillis(10)) period = 1000;
    else period = TimeUnit.SECONDS.toMillis(10);

    ageTimer =
        scheduleRepeating(
            new BackgroundWindowTask() {

              @Override
              protected void doWork() throws Exception {
                ageBasedEviction();
              }
            },
            period,
            period);
  }

  @Override
  public void drain() throws Exception {
    ageTimer.cancel(true);
    super.drain();
  }

  /**
   * AgeMarkers are put into the LRU list of partition keys. Any partition key earlier in the list
   * is older than that AgeMarker.
   *
   * @throws Exception
   */
  private void ageBasedEviction() throws Exception {
    assert windowIsLocked();

    // Find the first age marker that is more recent
    // than at least one partition
    AgeMarker ageMarker = null;
    synchronized (orderedPartitions) {
      drainOldAgeMarkers();

      for (Object o : orderedPartitions) {
        if (o instanceof AgeMarker) {
          ageMarker = (AgeMarker) o;
          break;
        }
      }
    }

    // The ageMarker will be the time the
    // marker was added to the list and
    // thus any partition key earlier in
    // the LRU list has not been inserted
    // into since that time.
    if (ageMarker != null) {
      final long now = System.currentTimeMillis();
      if (ageMarker.age >= (now - evictionConfiguration))
        ageMarker = null; // not ready for eviction.
    }

    if (ageMarker != null) {
      for (; ; ) {
        final Object oldest;
        synchronized (orderedPartitions) {
          oldest = orderedPartitions.peekFirst();
          if (oldest instanceof AgeMarker) {
            AgeMarker am = (AgeMarker) orderedPartitions.removeFirst();
            assert ageMarker.age == am.age;
            break;
          }
        }

        // Must have been a AgeMarker in the list
        assert oldest != null;

        evictSpecificPartition(oldest);
      }
    }

    // Add a new AgeMarker as the MRU item on the list
    // all existing partitions are older than now.
    synchronized (orderedPartitions) {
      // If there have been no tuples since the
      // last AgeMarker, then any partitions earlier
      // than it have aged so can be aged by the marker
      // we are adding.
      if (orderedPartitions.peekLast() instanceof AgeMarker) orderedPartitions.removeLast();

      if (!orderedPartitions.isEmpty()) orderedPartitions.addLast(new AgeMarker());
    }
  }

  @Override
  void drainOldAgeMarkers() {
    while (orderedPartitions.peekFirst() instanceof AgeMarker) orderedPartitions.removeFirst();
  }

  @Override
  public Collection<?> getPartitions() {

    final ArrayList<Object> userList;
    synchronized (orderedPartitions) {
      userList = new ArrayList<Object>(orderedPartitions.size());
      for (Object o : orderedPartitions) if (!(o instanceof AgeMarker)) userList.add(o);
    }

    return Collections.unmodifiableList(userList);
  }

  /**
   * Marker inserted into orderedPartitions to mark which partitions should be evicted when evicting
   * by age. Any partition that is older than the marker will be removed. We use markers to avoid
   * always having to generate a timestamp on each insert.
   */
  private static final class AgeMarker implements Serializable {
    private static final long serialVersionUID = 5820477849288371452L;
    final long age = System.currentTimeMillis();
  }
}
