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

package com.ibm.streams.operator.internal.runtime;

import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.Delayed;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.RunnableScheduledFuture;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

/**
 * Executor that allows the operator runtime to monitor if there is still outstanding activity. A
 * completer thread continually runs (OperatorAdapter) checking for output (with sleeps). Once it
 * completes then the operator is complete if there are no input ports.
 */
public final class OperatorScheduledExecutor extends ScheduledThreadPoolExecutor {

  private final OperatorThreadFactory opThreadFactory;

  public OperatorScheduledExecutor(int corePoolSize, OperatorThreadFactory opThreadFactory) {
    super(corePoolSize, opThreadFactory);
    this.opThreadFactory = opThreadFactory;
  }

  @Override
  public List<Runnable> shutdownNow() {
    final List<Runnable> notExecuted = super.shutdownNow();
    opThreadFactory.shutdownNow();
    return notExecuted;
  }

  @Override
  public void shutdown() {
    super.shutdown();
    opThreadFactory.shutdown();
  }

  private final Set<RunnableScheduledFuture<?>> asyncTasks =
      Collections.synchronizedSet(new HashSet<RunnableScheduledFuture<?>>());;

  @Override
  protected <V> RunnableScheduledFuture<V> decorateTask(
      Runnable runnable, RunnableScheduledFuture<V> task) {

    return trackTask(task);
  }

  @Override
  protected <V> RunnableScheduledFuture<V> decorateTask(
      Callable<V> c, RunnableScheduledFuture<V> task) {
    return trackTask(task);
  }

  /**
   * Track an executed task so that we can determine through the complete method when all background
   * activity is complete. asyncTasks contains all the Future for tasks that have been started and
   * not completed. For non-periodic tasks we remove the future from asyncTasks once it is
   * completed. Periodic tasks remain in asyncTasks until they have been cancelled.
   */
  private <V> RunnableScheduledFuture<V> trackTask(RunnableScheduledFuture<V> task) {
    task = new TrackedFuture<V>(task);
    asyncTasks.add(task);
    return task;
  }

  /**
   * Ensure any task starts out with the thread's context class loader set to the operator's loader.
   */
  @Override
  protected void beforeExecute(final Thread t, final Runnable r) {
    t.setContextClassLoader(opThreadFactory.getLoader());
    super.beforeExecute(t, r);
  }

  public boolean hasActiveTasks() {
    synchronized (asyncTasks) {
      if (asyncTasks.isEmpty()) return false;

      Iterator<RunnableScheduledFuture<?>> i = asyncTasks.iterator();
      while (i.hasNext()) {
        RunnableScheduledFuture<?> task = i.next();
        if (task.isDone()) i.remove();
        else return true;
      }
    }
    return false;
  }

  /**
   * Track a non-periodic future so that we are aware of all background activity. Simply wrap the
   * RunnableScheduledFuture that our executor returns ensuring that the future is removed from the
   * list of tracked tasks once it is complete.
   */
  final class TrackedFuture<V> implements RunnableScheduledFuture<V> {

    private final RunnableScheduledFuture<V> realTask;

    TrackedFuture(RunnableScheduledFuture<V> realTask) {
      this.realTask = realTask;
    }

    @Override
    public void run() {
      try {
        realTask.run();
      } finally {
        if (!isPeriodic()) removeTrack();
      }
    }

    /**
     * Remove tracking of the task and notify the completer if the scheduler seems to have no work.
     */
    private void removeTrack() {
      asyncTasks.remove(this);

      if (asyncTasks.isEmpty() || (getActiveCount() <= 1 && getQueue().isEmpty())) {
        opThreadFactory.getAdapter().notifyCompleter();
      }
    }

    @Override
    public boolean cancel(boolean mayInterruptIfRunning) {
      boolean wasCancelled = realTask.cancel(mayInterruptIfRunning);
      if (wasCancelled) removeTrack();
      return wasCancelled;
    }

    @Override
    public boolean isCancelled() {
      return realTask.isCancelled();
    }

    @Override
    public boolean isDone() {
      return realTask.isDone();
    }

    @Override
    public V get() throws InterruptedException, ExecutionException {
      return realTask.get();
    }

    @Override
    public V get(long timeout, TimeUnit unit)
        throws InterruptedException, ExecutionException, TimeoutException {
      return realTask.get(timeout, unit);
    }

    @Override
    public long getDelay(TimeUnit unit) {
      return realTask.getDelay(unit);
    }

    @Override
    public int compareTo(Delayed o) {
      return realTask.compareTo(o);
    }

    @Override
    public boolean isPeriodic() {
      return realTask.isPeriodic();
    }
  }
}
