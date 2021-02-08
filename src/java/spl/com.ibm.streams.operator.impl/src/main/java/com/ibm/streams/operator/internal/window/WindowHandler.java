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
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.state.Checkpoint;
import com.ibm.streams.operator.state.StateHandler;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowEvent;
import com.ibm.streams.operator.window.StreamWindowListener;
import com.ibm.streams.operator.window.StreamWindowPartitioner;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;
import javax.management.ListenerNotFoundException;
import javax.management.Notification;
import javax.management.NotificationFilterSupport;
import javax.management.NotificationListener;

/**
 * A tuple processor that creates events for a StreamWindowListener and sends the tuples and
 * punctuation off to the real processor. The real processor invokes the process methods on the
 * user's operator. Thus this class acts as an "intercepter" for a windowed input port.
 *
 * <p>Window synchronization.
 *
 * <p>There are four mechanisms that can be called concurrently by multiple threads that may result
 * in window events handled by user code (StreamWindowListener). 1) insert() - Tuple insertion
 * (INSERTION, TRIGGER, INITIAL_FULL, EVICTION and PARTITION_EVICTION events) 2) mark() -
 * Punctuation (FINAL_MARKER and WINDOW_MARKER) (FINAL and EVICTION events) 3) evictPartition() -
 * Partition eviction (PARTITION_EVICTION events) 4) Background events from time based windows
 * definitions (PARTITION_EVICTION, TRIGGER, INITIAL_FULL and EVICTION events).
 *
 * <p>All of these go through this class and use the lockWindow()/unlockWindow() methods to single
 * thread access through the StreamWindowListener and provide the happens-before memory visibility
 * guarantees.
 *
 * <p>All background events must use the BackgroundWindowTask to automatically perform the locking
 * and to ensure all window events can be completed once a FINAL_MARKER is received).
 *
 * <p>State for this object is synchronized using its monitor.
 *
 * @param <T> Tuple type.
 */
public abstract class WindowHandler<T> implements StateHandler {

  /** Listener for the window events. Defaults to a no-op listener. */
  private StreamWindowListener<T> listener =
      new StreamWindowListener<T>() {

        @Override
        public void handleEvent(StreamWindowEvent<T> event) {}
      };

  /** Partitioner for incoming tuples, only used when window is partitioned. */
  private StreamWindowPartitioner<T, ?> partitioner;

  final StreamWindow<T> window;

  private final OperatorAdapter adapter;

  /** List of scheduled and repeating tasks. */
  private List<ScheduledFuture<?>> timers = new ArrayList<ScheduledFuture<?>>();

  /** Number of background tasks that are active */
  private int activeBackgroundWorkCount;

  private boolean finalMarkSeen;

  /** Lock that controls access to the window and visibility of modified window state. */
  private final ReentrantLock windowLock = new ReentrantLock();

  /**
   * Has the window seen the final marker.
   *
   * @return the finalMarkSeen
   */
  public boolean isFinalMarkSeen() {
    return finalMarkSeen;
  }

  @SuppressWarnings("unchecked")
  protected WindowHandler(OperatorAdapter adapter, StreamWindow<T> window) {
    this.adapter = adapter;
    this.window = window;
    ((InputPortWindow<T, ?, ?>) window).setHandler(this);
  }

  final void lockWindow() {
    windowLock.lock();
  }

  private final void lockWindowInterruptibly() throws InterruptedException {
    windowLock.lockInterruptibly();
  }

  final void unlockWindow() {
    windowLock.unlock();
  }

  public boolean windowIsLocked() {
    return windowLock.isHeldByCurrentThread();
  }

  /**
   * Insert a tuple into a specific partition. Only called from insert() with the window already
   * locked and implemented by each sub-class.
   */
  abstract void insertIntoPartition(T tuple) throws Exception;

  synchronized void cancelAllBackgroundTasks() throws InterruptedException {
    for (Future<?> timer : timers) timer.cancel(true);

    finalMarkSeen = true;
    while (activeBackgroundWorkCount != 0) wait();
  }

  abstract void markAllPartitions(Punctuation mark) throws Exception;

  /**
   * Evict a partition.
   *
   * @param partition
   * @throws Exception
   */
  final void evictPartition(Object partition) throws Exception {
    lockWindow();
    try {
      evictSpecificPartition(partition);
    } finally {
      unlockWindow();
    }
  }

  /**
   * Evict a specific partition. Only called from evictPartition() with the window already locked
   * and implemented by each sub-class.
   */
  abstract void evictSpecificPartition(Object partition) throws Exception;

  public synchronized void registerListener(StreamWindowListener<T> listener) {
    this.listener = listener;
  }

  public synchronized void registerPartitioner(StreamWindowPartitioner<T, ?> partitioner) {
    this.partitioner = partitioner;
  }

  public StreamWindow<T> getStreamWindow() {
    return window;
  }

  void actionEvent(StreamWindowEvent<T> event) throws Exception {
    assert windowIsLocked();

    listener.handleEvent(event);
  }

  @SuppressWarnings("unchecked")
  public int getPolicyCount(boolean eviction) {
    Integer count;
    if (eviction) count = ((InputPortWindow<T, Integer, ?>) window).getEvictionPolicyConfig();
    else count = ((InputPortWindow<T, ?, Integer>) window).getTriggerPolicyConfig();

    return count;
  }

  @SuppressWarnings("unchecked")
  public final long getPolicyTimeMilliseconds(boolean eviction) {
    String period;
    if (eviction) period = ((InputPortWindow<T, String, ?>) window).getEvictionPolicyConfig();
    else period = ((InputPortWindow<T, ?, String>) window).getTriggerPolicyConfig();

    return timeSecondsToMillis(period);
  }

  static long timeSecondsToMillis(String value) {
    double periodSeconds = Double.valueOf(value);
    double periodMillis = periodSeconds * 1000.0;
    return (long) periodMillis;
  }

  private synchronized void addTimer(ScheduledFuture<?> timer) {
    timers.add(timer);
  }

  private final ScheduledExecutorService getScheduledExecutorService() {
    return adapter.getScheduledExecutorService();
  }

  /**
   * Schedule a one off task for window processing but only if the final mark has not been seen.
   * Once the final mark is seen then all window background activity stops, e.g. sliding window time
   * eviction.
   */
  public synchronized ScheduledFuture<?> scheduleOnOff(
      final BackgroundWindowTask task, long delay, TimeUnit unit) {
    if (!isFinalMarkSeen()) return getScheduledExecutorService().schedule(task, delay, unit);
    else return null;
  }

  public synchronized ScheduledFuture<?> scheduleRepeating(
      final BackgroundWindowTask task, long delay, long period) {
    if (!isFinalMarkSeen()) {
      ScheduledFuture<?> sf =
          getScheduledExecutorService()
              .scheduleAtFixedRate(task, delay, period, TimeUnit.MILLISECONDS);
      addTimer(sf);
      return sf;
    } else return null;
  }

  /**
   * Setup a notification on allPortsReady() that calls kickOff.run().
   *
   * @param kickOff
   */
  public final void kickOff(final BackgroundWindowTask kickOff) {

    final NotificationFilterSupport apr = new NotificationFilterSupport();
    apr.enableType(OperatorContextMXBean.ALL_PORTS_READY);

    adapter
        .getManagement()
        .addNotificationListener(
            new NotificationListener() {

              @Override
              public void handleNotification(Notification notification, Object handback) {

                kickOff.run();

                try {
                  adapter.getManagement().removeNotificationListener(this, apr, handback);
                } catch (ListenerNotFoundException e) {
                }
              }
            },
            apr,
            kickOff);
  }

  /** @return the partitioner */
  StreamWindowPartitioner<T, ?> getPartitioner() {
    return partitioner;
  }

  /** Return the set of active WindowPartitions for this window. */
  public abstract Collection<?> getActiveWindowPartitions();

  /**
   * Runnable implementation that allows the number of active tasks to be tracked so that processing
   * of the final mark can wait for outstanding tasks to complete. Locking of the window is also
   * performed here to have a consistent lock protocol for background window tasks.
   */
  public abstract class BackgroundWindowTask implements Runnable {

    @Override
    public void run() {
      synchronized (WindowHandler.this) {
        if (isFinalMarkSeen()) return;

        activeBackgroundWorkCount++;
      }
      try {
        lockWindowInterruptibly();
      } catch (InterruptedException e) {
        synchronized (WindowHandler.this) {
          activeBackgroundWorkCount--;
          if (activeBackgroundWorkCount == 0) WindowHandler.this.notifyAll();
          return;
        }
      }

      try {
        if (!Thread.currentThread().isInterrupted()) doWork();
      } catch (Throwable t) {
        WindowHandler.this.adapter.backgroundException(Thread.currentThread(), t);
      } finally {
        unlockWindow();
        synchronized (WindowHandler.this) {
          activeBackgroundWorkCount--;
          if (activeBackgroundWorkCount == 0) WindowHandler.this.notifyAll();
        }
      }
    }

    /**
     * Do the actual work of the background window task. If the window has seen the final mark then
     * this will not be called even if the task was scheduled and executed.
     *
     * @throws Exception Error handling background event.
     */
    protected abstract void doWork() throws Exception;
  }

  /**
   * Allows a partition to notify the handler tuples have been evicted. Only used by
   * TupleCountEvictionHandler.
   *
   * @param evictedCount
   */
  void evictTupleCount(final int evictedCount) {}

  @SuppressWarnings("unchecked")
  private Collection<WindowPartition<T, ?, ?, ?>> activePartitions() {
    return (Collection<WindowPartition<T, ?, ?, ?>>) getActiveWindowPartitions();
  }

  /** Kick off the activations once allPortsReady occurs. */
  void kickOffActivation() {
    kickOff(
        new BackgroundWindowTask() {
          @Override
          protected void doWork() throws Exception {
            activatePartitions();
          }
        });
  }

  void activatePartitions() throws Exception {
    for (WindowPartition<T, ?, ?, ?> p : activePartitions()) {
      p.activate();
    }
  }

  /**
   * Drain all activity by draining all partitions.
   *
   * <p>and deactivate each partition.
   */
  @Override
  public void drain() throws Exception {
    lockWindow();
    try {
      for (WindowPartition<T, ?, ?, ?> partition : activePartitions()) {
        partition.deactivate();
        partition.drain();
      }

    } finally {
      unlockWindow();
    }
  }

  /** Writes the collection of partitions if there is at least one partition. */
  @Override
  public void checkpoint(Checkpoint checkpoint) throws Exception {

    lockWindow();
    try {
      // Need to drain any outstanding evictions, as the
      // set of evicted tuples is not persisted.
      for (WindowPartition<T, ?, ?, ?> partition : activePartitions()) {
        partition.drain();
      }

      final ObjectOutputStream out = checkpoint.getOutputStream();
      final int partitionCount = activePartitions().size();
      out.writeInt(partitionCount);
      if (partitionCount != 0) {
        Collection<?> partitions = activePartitions();
        for (Object partition : partitions) out.writeObject(partition);
      }

      checkpointHandler(out);

    } finally {
      unlockWindow();
    }
  }
  // Allow sub-classes to have additional state.
  void checkpointHandler(ObjectOutputStream out) throws IOException {}

  void resetHandler(ObjectInputStream in) throws IOException, ClassNotFoundException {}

  @Override
  public void resetToInitialState() throws Exception {
    lockWindow();
    try {

      discardContents();

    } finally {
      unlockWindow();
    }
  }

  /** Discard all contents of all window partitions and the partitions themselves. */
  abstract void discardContents();

  /**
   * Window state is stored as a count of partitions followed by the collection of WindowPartitions.
   * If the partition count is zero then no map is stored.
   *
   * <p>The format is identical for the non-partitioned case, so that the map will contain a single
   * with key zero.
   */
  @Override
  public void reset(Checkpoint checkpoint) throws Exception {

    lockWindow();
    try {

      discardContents();

      ObjectInputStream ckptIn = checkpoint.getInputStream();
      final int partitionCount = ckptIn.readInt();
      if (partitionCount != 0) {

        for (int i = 0; i < partitionCount; i++) {
          @SuppressWarnings("unchecked")
          WindowPartition<T, ?, WindowedTuple<T>, ?> windowPartition =
              (WindowPartition<T, ?, WindowedTuple<T>, ?>) ckptIn.readObject();
          windowPartition.setWindowHandler(this);
          addPartition(windowPartition);
        }
      }

      resetHandler(ckptIn);

    } finally {
      unlockWindow();
    }
  }

  @Override
  public void close() throws IOException {}

  @Override
  public void retireCheckpoint(long id) throws Exception {}

  /**
   * Add a partition from a checkpoint
   *
   * @param windowPartition
   */
  abstract void addPartition(WindowPartition<T, ?, ? extends WindowedTuple<T>, ?> windowPartition);
}
