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

import com.ibm.streams.operator.internal.runtime.api.Adapter;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.ThreadFactory;

/**
 * A ThreadFactory for an operator that provides an operator specific ThreadGroup. It initializes
 * all the threads to have their context class loader to be set to be the operator's class loader
 * when created. The threads will also be named with the operator's name.
 */
public final class OperatorThreadFactory implements ThreadFactory {

  private final Adapter adapter;

  private ThreadGroup threadGroup;
  private final ClassLoader loader;
  private long threadCount;
  private boolean shutdown;
  private final String groupName;
  /**
   * Use a fixed thread group for the parent to avoid the thread groups we create for the operator
   * being related which seems to cause the vm to hang when they are destroyed.
   */
  private final ThreadGroup parentGroup;

  private final Set<Thread> threads = Collections.synchronizedSet(new HashSet<Thread>());

  public OperatorThreadFactory(Adapter adapter, String suffix) {
    this.adapter = adapter;
    this.groupName = adapter.getOperatorName() + suffix;
    this.loader = adapter.getClassLoader();
    parentGroup = Thread.currentThread().getThreadGroup();
  }

  private synchronized ThreadGroup getThreadGroup(boolean create) {
    if (create && threadGroup == null) {
      threadGroup =
          new ThreadGroup(parentGroup, groupName) {
            /**
             * Handle uncaught background exceptions from user code by notifying the single
             * completer thread (through backgroundException()) that an exception occurred. It will
             * then throw the exception which will cause the PE to terminate.
             */
            @Override
            public void uncaughtException(Thread t, Throwable e) {
              if (!OperatorThreadFactory.this.adapter.backgroundException(t, e))
                super.uncaughtException(t, e);
            }
          };
      threadGroup.setDaemon(false);
    }
    return threadGroup;
  }

  Adapter getAdapter() {
    return adapter;
  }

  ClassLoader getLoader() {
    return loader;
  }

  /**
   * Return a thread within the single thread group and with an operator specific name. The thread
   * context class loader is set to the class loader for the operator.
   */
  @Override
  public Thread newThread(Runnable r) {
    long tc;
    synchronized (this) {
      if (shutdown) return null;
      tc = threadCount++;
    }
    final ThreadGroup group = getThreadGroup(true);
    Thread thread =
        new Thread(group, r, group.getName() + "-thread-" + tc) {

          /** Keep track of non-daemon threads so that complete can wait for their termination. */
          @Override
          public void start() {
            threads.add(this);
            super.start();
          }

          /** Once thread is complete then remove from the list of tracked threads. */
          @Override
          public void run() {
            // Set the thread local to associate this
            // thread with this operator context
            // through the adapter.
            adapter.setActive();

            super.run();

            threads.remove(this);
            // On exception the default exception handler
            // will notify the completer.
            adapter.notifyCompleter();
          }
        };
    thread.setContextClassLoader(loader);
    thread.setDaemon(true);
    return thread;
  }

  /** Shutdown this thread factory due to the Operator being shutdown. */
  public void shutdown() {
    synchronized (this) {
      shutdown = true;
    }
  }

  public void shutdownNow() {
    // Ensure no new threads will be created.
    shutdown();

    final ThreadGroup group = getThreadGroup(false);
    if (group == null) return;

    if (!threads.isEmpty()) {
      Thread[] leftoverThreads = threads.toArray(new Thread[0]);
      for (Thread t : leftoverThreads) t.interrupt();

      // Briefly try to wait for threads to
      // complete so that the thread group
      // can be destroyed.
      for (Thread t : leftoverThreads) {
        try {
          t.join(10);
        } catch (InterruptedException e) {
        }
      }
    }

    // Allow the thread group to be destroyed
    // automatically if we can't destroy it
    // due to a thread still executing.
    group.setDaemon(true);

    try {
      group.destroy();
    } catch (IllegalThreadStateException e) {
      // group.setDaemon(true);
    }

    synchronized (this) {
      this.threadGroup = null;
    }
  }

  /**
   * Check to see if there are non daemon threads that have not yet completed.
   *
   * @return True if there are active non daemon threads, false otherwise.
   */
  public boolean haveActiveNonDaemonThreads() {

    synchronized (threads) {
      if (threads.isEmpty()) return false;

      for (Thread t : threads) {
        if (t.isDaemon()) continue;
        return true;
      }
    }

    return false;
  }

  /**
   * Get the number of created threads.
   *
   * @return
   */
  public synchronized long getThreadCount() {
    return threadCount;
  }

  /** Return the list of active threads. */
  public long[] getActiveThreadIds() {
    synchronized (threads) {
      long[] ids = new long[threads.size()];
      int i = 0;
      for (Thread t : threads) ids[i++] = t.getId();
      return ids;
    }
  }
}
