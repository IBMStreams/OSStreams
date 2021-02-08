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

import com.ibm.streams.operator.internal.window.WindowHandler;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.LinkedList;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

/**
 * Time based eviction of tuples in the sliding window. Tuples are evicted when they have been in
 * the tuple more than its defined period. A timer is always active to evict tuples based upon the
 * time the oldest tuple has left in the window. If no tuples exist then the timer is set to the
 * period. When the timer goes off, after evicting the tuples it sets up another timer with the
 * required time. This class additional records the arrival time of each tuple using the
 * TimedWindowedTuple class.
 *
 * @param <T> Tuple type.
 */
abstract class SlidingTimeEvict<T, P> extends SlidingWindow<T, P, TimedWindowedTuple<T>> {

  private static final long serialVersionUID = 2315688299411450047L;
  private final long evictPeriod;

  /** Pending tasks for time based eviction. */
  private transient LinkedList<ScheduledFuture<?>> evictionTasks =
      new LinkedList<ScheduledFuture<?>>();

  SlidingTimeEvict(WindowHandler<T> windowHandler, P partition) {
    super(windowHandler, partition);

    evictPeriod = windowHandler.getPolicyTimeMilliseconds(true);
  }

  private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
    in.defaultReadObject();
    evictionTasks = new LinkedList<ScheduledFuture<?>>();
  }

  /** Our WindowTuple implementation also holds the time the tuple was inserted into the window. */
  protected TimedWindowedTuple<T> getWindowTuple(T tuple) {
    return new TimedWindowedTuple<T>(tuple);
  }

  /**
   * Prior to invoking the trigger evict any tuples that should not be in the window. We need to do
   * this on the trigger as we cannot rely on the execution order of timing events. This ensures the
   * window is correct when the trigger fires.
   */
  @Override
  void trigger() throws Exception {

    evictTuples();
    evictEvent();

    super.trigger();
  }

  /** Evict all tuples that have been in the window for more than the eviction period. */
  private long evictTuples() {
    final long now = System.currentTimeMillis();
    for (; ; ) {
      TimedWindowedTuple<T> wt = getTuples().peekFirst();
      if (wt == null) return evictPeriod;

      long timeInWindow = now - wt.getArrivalTime();

      if (timeInWindow > evictPeriod) evictOldestTuple();
      else return (evictPeriod - timeInWindow) + 1L;
    }
  }

  /**
   * Drain any pending evicted tuples. There may be tuples due for eviction if we tested for them,
   * but this will be called during a checkpoint in a consistent region where we cannot submit any
   * tuples. In the consistent region case there will be no pending tuples as the drain will have
   * evicted them.
   */
  @Override
  protected void drain() throws Exception {

    evictEvent();
    super.drain();
  }

  @Override
  protected void activate() throws Exception {
    initialFull();
    evictAndSchedule();
  }

  /**
   * Cancel all outstanding tasks. They can only be doing useful work if they hold the window lock,
   * and calls to this method will hold the lock. Thus they are cancelled waiting for the lock or
   * after they have released the lock, in either case no window work will be done.
   */
  @Override
  protected void deactivate() {
    assert getWindowHandler().windowIsLocked();

    for (ScheduledFuture<?> evictionTask : evictionTasks) {
      if (!evictionTask.isDone()) evictionTask.cancel(true);
    }
    evictionTasks.clear();
  }

  /**
   * Evict tuples and schedule a task to perform the same action with the with time based upon the
   * required eviction of the next tuple.
   *
   * @param now
   */
  void evictAndSchedule() {
    long waitTime = evictTuples();

    // Schedule a background eviction event
    // if non eviction is require, then no
    // background task will created. It's
    // also possible that by the time the task
    // comes to run, there are no tuples to evict.
    ScheduledFuture<?> task = backgroundEvictEvent();
    if (task != null) evictionTasks.add(task);

    cleanEvictionTasks();

    task =
        getWindowHandler()
            .scheduleOnOff(
                getWindowHandler().new BackgroundWindowTask() {
                  @Override
                  protected void doWork() {

                    evictAndSchedule();
                  }
                },
                waitTime,
                TimeUnit.MILLISECONDS);
    if (task != null && !task.isDone()) evictionTasks.add(task);
  }

  /**
   * Clean tasks of the list, assumption for optimization is that they will be executed in the order
   * added to the list, thus once we see one that is not done, we just assume any others are also
   * not done.
   */
  private void cleanEvictionTasks() {
    while (evictionTasks.peekFirst() != null) {
      if (evictionTasks.getFirst().isDone()) evictionTasks.removeFirst();
      else break;
    }
  }
}
