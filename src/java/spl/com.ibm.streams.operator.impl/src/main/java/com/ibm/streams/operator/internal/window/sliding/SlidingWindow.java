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

package com.ibm.streams.operator.internal.window.sliding;

import com.ibm.streams.operator.internal.window.PartitionFactory;
import com.ibm.streams.operator.internal.window.WindowHandler;
import com.ibm.streams.operator.internal.window.WindowPartition;
import com.ibm.streams.operator.internal.window.WindowedTuple;
import com.ibm.streams.operator.internal.window.delta.DeltaPolicy;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowEvent;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public abstract class SlidingWindow<T, P, W extends WindowedTuple<T>>
    extends WindowPartition<T, P, W, SlidingWindowContents<W>> {

  private static final long serialVersionUID = 8187515044486651661L;

  private static final int DEFAULT_EVICTION_CAPACITY = 20;

  /** Tuples that have been evicted from the window. Ordered from oldest to newest. */
  private final ArrayList<T> evictedTuples = new ArrayList<T>(DEFAULT_EVICTION_CAPACITY);

  SlidingWindow(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition, new SlidingWindowContents<W>());
  }

  /**
   * Allow sub-classes to specify the size of the evicted tuples buffer. It will only get reset on
   * to the sub-classes capacity on the first eviction.
   */
  protected int evictionCapacity() {
    return DEFAULT_EVICTION_CAPACITY;
  }

  /**
   * Evict the oldest tuple, adding it into the set of evicted tuples if it contained a reference to
   * the original tuple.
   */
  void evictOldestTuple() {

    // Evict the oldest tuple which is the first in the Deque
    WindowedTuple<T> wt = getTuples().removeFirst();

    T tuple = wt.getTuple();
    if (tuple != null) evictedTuples.add(tuple);
    evictTupleCount(1);
  }

  /** Schedule a background window event to evict tuples, if there are outstanding tuples. */
  protected final ScheduledFuture<?> backgroundEvictEvent() {
    if (evictedTuples.isEmpty()) return null;

    return getWindowHandler()
        .scheduleOnOff(
            getWindowHandler().new BackgroundWindowTask() {
              @Override
              protected void doWork() throws Exception {

                // This will also be a no-op if
                // there are no tuples in evictedTuples.
                // (e.g. due to multiple concurrent background tasks)
                evictEvent();
              }
            },
            0,
            TimeUnit.MILLISECONDS);
  }

  /**
   * Notify the listener of evictions, multiple single tuple evictions may be combined into a single
   * evict.
   *
   * @throws Exception
   */
  protected final void evictEvent() throws Exception {
    assert getWindowHandler().windowIsLocked();

    if (!evictedTuples.isEmpty()) {
      try {
        actionEvent(StreamWindowEvent.Type.EVICTION, evictedTuples);
      } finally {
        final int size = evictedTuples.size();
        evictedTuples.clear();
        if (size > 2 * this.evictionCapacity()) {
          evictedTuples.trimToSize();
          evictedTuples.ensureCapacity(evictionCapacity());
        }
      }
    }
  }

  final void checkDeltaTrigger(DeltaPolicy<T, WindowedTuple<T>> trigger, T tuple) throws Exception {
    if (trigger.checkTrigger(tuple)) {
      trigger();
    }
  }

  /**
   * Trigger this window. notifying the listener of a TRIGGER event.
   *
   * @throws Exception
   */
  void trigger() throws Exception {
    actionEvent(StreamWindowEvent.Type.TRIGGER);
  }

  final void initialFull() throws Exception {
    if (getTuples().isWindowFullForFirstTime()) actionEvent(StreamWindowEvent.Type.INITIAL_FULL);
  }

  final class CountTrigger implements Serializable {

    private static final long serialVersionUID = 1624426522960244237L;

    private final int triggerSize;

    private int triggerCount;

    CountTrigger() {
      this.triggerSize = getWindowHandler().getPolicyCount(false);
    }

    void checkTrigger() throws Exception {
      if (++triggerCount == triggerSize) {
        triggerCount = 0;
        trigger();
      }
    }
  }

  final PeriodicTimerPolicy getTimedTriggerPolicy() {
    return new PeriodicTimerPolicy(getWindowHandler().getPolicyTimeMilliseconds(false)) {
      private static final long serialVersionUID = -1722477925436976950L;

      @Override
      protected void doTimedEvent() throws Exception {
        trigger();
      }
    };
  }

  public static <T> PartitionFactory<T, ? extends SlidingWindow<T, ?, ?>> getSlideFactory(
      StreamWindow<T> window) {
    switch (window.getEvictionPolicy()) {
      case COUNT:
        switch (window.getTriggerPolicy()) {
          case COUNT:
            return new PartitionFactory<T, SlidingCountEvictCountTrigger<T, Object>>() {

              @Override
              public SlidingCountEvictCountTrigger<T, Object> newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingCountEvictCountTrigger<T, Object>(handler, partition);
              }
            };
          case TIME:
            return new PartitionFactory<T, SlidingCountEvictTimeTrigger<T, Object>>() {

              @Override
              public void initialize(WindowHandler<T> handler) {
                // SlidingWindow.timeTrigger(handler);
              }

              @Override
              public SlidingCountEvictTimeTrigger<T, Object> newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingCountEvictTimeTrigger<T, Object>(handler, partition);
              }
            };
          case DELTA:
            return new PartitionFactory<T, SlidingCountEvictDeltaTrigger<T, Object>>() {

              @Override
              public SlidingCountEvictDeltaTrigger<T, Object> newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingCountEvictDeltaTrigger<T, Object>(handler, partition);
              }
            };
        }
      case TIME:
        switch (window.getTriggerPolicy()) {
          case COUNT:
            return new SlidingTimeEvictFactory<T, SlidingTimeEvictCountTrigger<T, Object>>() {

              @Override
              SlidingTimeEvictCountTrigger<T, Object> _newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingTimeEvictCountTrigger<T, Object>(handler, partition);
              }
            };
          case TIME:
            return new SlidingTimeEvictFactory<T, SlidingTimeEvictTimeTrigger<T, Object>>() {

              @Override
              public void initialize(WindowHandler<T> handler) {
                super.initialize(handler);
                // SlidingWindow.timeTrigger(handler);
              }

              @Override
              SlidingTimeEvictTimeTrigger<T, Object> _newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingTimeEvictTimeTrigger<T, Object>(handler, partition);
              }
            };
          case DELTA:
            return new SlidingTimeEvictFactory<T, SlidingTimeEvictDeltaTrigger<T, Object>>() {

              @Override
              SlidingTimeEvictDeltaTrigger<T, Object> _newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingTimeEvictDeltaTrigger<T, Object>(handler, partition);
              }
            };
        }
      case DELTA:
        switch (window.getTriggerPolicy()) {
          case COUNT:
            return new PartitionFactory<T, SlidingDeltaEvictCountTrigger<T, Object>>() {

              @Override
              public SlidingDeltaEvictCountTrigger<T, Object> newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingDeltaEvictCountTrigger<T, Object>(handler, partition);
              }
            };
          case TIME:
            return new PartitionFactory<T, SlidingDeltaEvictTimeTrigger<T, Object>>() {

              @Override
              public void initialize(WindowHandler<T> handler) {
                // SlidingWindow.timeTrigger(handler);
              }

              @Override
              public SlidingDeltaEvictTimeTrigger<T, Object> newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingDeltaEvictTimeTrigger<T, Object>(handler, partition);
              }
            };
          case DELTA:
            return new PartitionFactory<T, SlidingDeltaEvictDeltaTrigger<T, Object>>() {

              @Override
              public SlidingDeltaEvictDeltaTrigger<T, Object> newPartition(
                  WindowHandler<T> handler, Object partition) {
                return new SlidingDeltaEvictDeltaTrigger<T, Object>(handler, partition);
              }
            };
        }
    }
    return null;
  }

  /**
   * PartitionFactory for SlidingTimeEvict that kicks off the timer that will initiate evictions.
   * The first expiration of the timer will cause initialFull events for all active partitions.
   * Partitions created after that point in time will immediately receive a initial full event.
   *
   * @param <T>
   */
  abstract static class SlidingTimeEvictFactory<T, WP extends SlidingTimeEvict<T, Object>>
      extends PartitionFactory<T, WP> {

    private boolean seenInitialFull;

    /** Create an initial full event N seconds after the allPortsReady notification. */
    @Override
    public void initialize(final WindowHandler<T> handler) {
      /*

      handler.kickOff(handler.new BackgroundWindowTask() {

          @SuppressWarnings("unchecked")
          @Override
          public void doWork() throws Exception {
              seenInitialFull = true;
              Collection<SlidingTimeEvict<T, ?>> partitions = (Collection<SlidingTimeEvict<T, ?>>) handler
                      .getActiveWindowPartitions();
              for (SlidingTimeEvict<T, ?> sw : partitions) {
                  sw.initialFull();
                  sw.evictAndSchedule();
              }
          }

      });
      */
    }

    public final WP newPartition(WindowHandler<T> handler, Object partition) {

      WP sw = _newPartition(handler, partition);

      return sw;
    }

    abstract WP _newPartition(WindowHandler<T> handler, Object partition);
  }
}
