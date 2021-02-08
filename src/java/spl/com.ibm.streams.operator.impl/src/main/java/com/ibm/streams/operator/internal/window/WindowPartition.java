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
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowEvent;
import java.io.Serializable;
import java.util.Collection;
import java.util.concurrent.ScheduledFuture;

/**
 * WindowPartition solely deals with tuples in a window's partition and the events related to
 * tuples. When the window is not partitioned there will be a single instance of a WindowPartition
 * for the handler.
 *
 * @param <P> Partition type
 * @param <W> WindowedTuple type.
 * @param <C> Collection type for tuples in the window.
 */
public abstract class WindowPartition<T, P, W extends WindowedTuple<T>, C extends Collection<W>>
    implements Serializable {

  private static final long serialVersionUID = 8290465975294519753L;

  /** */
  private transient WindowHandler<T> windowHandler;
  /** Which partition is this for. */
  protected final P partition;

  /** Tuples that are in the window partition, ordered from oldest to newest */
  private final C tuples;

  protected WindowPartition(WindowHandler<T> windowHandler, P partition, C tuples) {
    this.windowHandler = windowHandler;
    this.partition = partition;
    this.tuples = tuples;
  }

  final void setWindowHandler(WindowHandler<T> windowHandler) {
    this.windowHandler = windowHandler;
  }

  protected final WindowHandler<T> getWindowHandler() {
    return windowHandler;
  }

  protected final C getTuples() {
    return tuples;
  }

  final P getPartitionKey() {
    return partition;
  }

  /**
   * True if this partition has been evicted and thus no more events will be generated. Since
   * partition eviction may take place during any event handling we need this state to ensure that
   * following events driven from the same action do not get handled. E.g. an insert can lead to
   * INSERTION and EVICTION events, if the INSERTION event evicts the partition then no EVICTION
   * event must happen.
   */
  private boolean evicted;

  /*
   ** Methods the window exposes for data manipulation.
   */

  /** If the mark is a FINAL_MARKER then create an FINAL event. Synchronization is held on this. */
  protected void mark(final Punctuation mark) throws Exception {
    if (mark == Punctuation.FINAL_MARKER) {
      actionEvent(StreamWindowEvent.Type.FINAL);
    }
  }

  /**
   * Insert tuple into this window partition with an INSERTION event followed by insertion of the
   * tuple into the set of windowed tuples. Synchronization is held on this.
   *
   * @param tuple
   * @throws Exception
   */
  protected void insert(T tuple) throws Exception {

    TupleList<T, W> newTuples = new TupleList<T, W>();

    newTuples.addTuple(getWindowTuple(tuple));

    // Add the tuples to the full window view before
    // the user event handling so that it they get added
    // regardless of any exception and that a partition
    // eviction call from the INSERTION handling would
    // see the full set of tuples.
    //
    getTuples().addAll(newTuples.getWindowedTuples());

    actionEvent(StreamWindowEvent.Type.INSERTION, newTuples);
  }

  @SuppressWarnings("unchecked")
  protected W getWindowTuple(T tuple) {
    return (W) new WindowedTuple<T>(tuple);
  }

  /**
   * Evict this partition from the window. The caller has already removed it, so it's just a matter
   * of notifying the event handler.
   *
   * @throws Exception
   */
  final void evictPartition() throws Exception {
    // Disable any future tasks.
    deactivate();
    try {
      actionEvent(StreamWindowEvent.Type.PARTITION_EVICTION);
    } finally {
      // disables any future events.
      this.evicted = true;
    }
  }

  private final boolean isEvicted() {
    return evicted;
  }

  /** Return a full view of tuples in the window that is handed off to user code through events. */
  protected final Iterable<T> getFullTupleView() {
    return new TupleView<T, W>(getTuples());
  }

  protected final void actionEvent(StreamWindowEvent.Type type, Iterable<T> tuples)
      throws Exception {
    assert getWindowHandler().windowIsLocked();

    if (isEvicted()) return;
    windowHandler.actionEvent(new StreamWindowEvent<T>(type, getStreamWindow(), partition, tuples));
  }

  private StreamWindow<T> getStreamWindow() {
    return windowHandler.getStreamWindow();
  }

  /**
   * Create an event and notify the listener with the tuple set being the entire set of tuples in
   * the window.
   *
   * @param type
   * @throws Exception
   */
  protected final void actionEvent(StreamWindowEvent.Type type) throws Exception {

    actionEvent(type, getFullTupleView());
  }

  /**
   * Inform the window handler that a number of tuples have been evicted due to an tuple EVICTION
   * event.
   *
   * @param evictedCount
   */
  protected final void evictTupleCount(final int evictedCount) {
    windowHandler.evictTupleCount(evictedCount);
  }

  /** Drain any activity for the window. */
  protected void drain() throws Exception {
    assert getWindowHandler().windowIsLocked();
  }

  /**
   * Activate the window partition, a no-op for most windows, excluding time based windows. Activate
   * will be called:
   */
  protected void activate() throws Exception {}

  protected void deactivate() {}

  /** Used for tumbling time eviction and sliding trigger policy. */
  public abstract class PeriodicTimerPolicy implements Serializable {

    private static final long serialVersionUID = -1296436339697715229L;
    private final long period;
    private long lastExecutionTime;
    private transient ScheduledFuture<?> repeatingTask;

    public PeriodicTimerPolicy(long period) {
      this.period = period;
    }

    /**
     * Setup a periodic eviction timer for this partition. Includes supporting the correct delay
     * before the next eviction after a timeout.
     */
    public void activate() {
      if (repeatingTask != null) return;

      long delay = period;
      if (lastExecutionTime != 0) {
        long sinceLastEvent = System.currentTimeMillis() - lastExecutionTime;
        if (sinceLastEvent >= period) delay = 0;
        else if (sinceLastEvent > 0) // could be zero or negative with differing clocks
        delay = period - sinceLastEvent;
      } else {
        // Persist the current time otherwise repeated
        // failures before the first event will
        // always push the tumble out by evictPeriod
        // leading to it being too late when it does occur.
        lastExecutionTime = System.currentTimeMillis();
      }

      repeatingTask =
          getWindowHandler()
              .scheduleRepeating(
                  getWindowHandler().new BackgroundWindowTask() {

                    @Override
                    protected void doWork() throws Exception {
                      try {
                        doTimedEvent();
                      } finally {
                        lastExecutionTime = System.currentTimeMillis();
                      }
                    }
                  },
                  delay,
                  period);
    }

    protected abstract void doTimedEvent() throws Exception;

    /**
     * Stop the repeating task from executing the window. At this point we hold the window lock so
     * there cannot be an active task tumbling the window, but it could be blocked waiting to get
     * the lock.
     */
    public void deactivate() {
      assert getWindowHandler().windowIsLocked();

      if (repeatingTask == null) return;

      // Cancel the repeating task to avoid future executions
      // allowing interruption of the task
      // (it would be waiting for the window lock)
      repeatingTask.cancel(true);
      repeatingTask = null;
    }
  }
}
