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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.flow.declare.InputPortDeclaration.CongestionPolicy;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric.MetricValue;
import com.ibm.streams.operator.internal.runtime.OperatorThreadFactory;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import java.util.Iterator;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * This is a handler that implements a threaded port in Java. This is a port backed by a fixed-size,
 * FIFO queue. As tuples and punctuations are received, they are added to the end of the queue.
 * Another thread reads from the queue and forwards the tuples and punctuations to the next handler
 * in the handler chain.
 *
 * <p>When the queue is full, the handling of tuples depends on a congestion policy, which can be
 * WAIT, DROP_FIRST, or DROP_LAST. When the congestion policy is WAIT and there is no room on the
 * queue for a tuple, the thread receiving the tuple blocks until there is room on the queue. When
 * it is DROP_FIRST, the oldest tuple is removed from the queue and discarded, making room for the
 * new tuple, or if there is no tuple in the queue, the current tuple is discarded. When it is
 * DROP_LAST, if there is no room on the queue, the new tuple is discarded.
 *
 * <p>Queuing a punctuation is handled differently from queuing a tuple. When a punctuation is
 * queued, it is never dropped, thus if the queue is full the receiving thread blocks until there is
 * room.
 *
 * <p>Actions are handled synchronously. The action is enqueued, and the thread queuing the action
 * blocks until the action is complete. As with punctuations, if there is no room on the queue, the
 * receiving thread always waits until there is room.
 */
public class ThreadedPortHandler implements RuntimeStream<Tuple> {

  private abstract class CongestionPolicyImpl {

    protected void newBlockingQueue() {
      blockingQueue = new ArrayBlockingQueue<QueueItem>(queueCapacity);
    }

    protected abstract void add(Tuple tuple) throws InterruptedException;

    protected void clearQueue() {
      blockingQueue.clear();
    }

    protected void add(Punctuation punct) throws InterruptedException {
      QueueItem item = new QueuePunct(punct);
      checkQueueSize(true);
      blockingQueue.put(item);
    }

    protected <A> QueueAction<A> add(StreamAction<A> action) throws InterruptedException {
      QueueAction<A> item = new QueueAction<A>(action);
      checkQueueSize(true);
      blockingQueue.put(item);
      return item;
    }

    protected boolean checkQueueSize(boolean willWaitOnFull) {
      int size = blockingQueue.size();
      if (size < queueCapacity) {
        recentMaxItemsQueuedMetric.setMaxValue(size + 1);
        maxItemsQueuedMetric.setMaxValue(size + 1);
        return false; // queue is not full
      } else {
        if (willWaitOnFull) {
          enqueueWaitsMetric.bump();
        }
        return true; // queue is full
      }
    }
  }

  private class WaitCongestionPolicy extends CongestionPolicyImpl {

    @Override
    protected void add(Tuple tuple) throws InterruptedException {
      QueueItem item = new QueueTuple(tuple);
      checkQueueSize(true);
      blockingQueue.put(item);
      tuplesQueuedMetric.bump();
    }
  }

  private class DropFirstCongestionPolicy extends CongestionPolicyImpl {

    @Override
    protected void newBlockingQueue() {
      blockingQueue = new LinkedBlockingQueue<QueueItem>(queueCapacity);
    }

    @Override
    protected void add(Tuple tuple) throws InterruptedException {
      QueueItem item = new QueueTuple(tuple);
      boolean wasFull = checkQueueSize(false);
      if (!wasFull) {
        blockingQueue.put(item);
        tuplesQueuedMetric.bump();
      } else {
        if (dropTupleInQueue()) {
          blockingQueue.put(item);
        } else {
          // No tuple in the queue so drop the current tuple
          tuplesDroppedMetric.bump();
        }
      }
    }

    @Override
    protected void add(Punctuation punct) throws InterruptedException {
      QueueItem item = new QueuePunct(punct);
      boolean wasFull = checkQueueSize(false);
      if (wasFull) {
        if (dropTupleInQueue()) {
          tuplesQueuedMetric.decrement();
        } else {
          // No tuple in the queue so wait as cannot drop punctuation
          enqueueWaitsMetric.bump();
        }
      }
      blockingQueue.put(item);
    }

    @SuppressWarnings("unchecked")
    protected boolean dropTupleInQueue() {
      Iterator<QueueItem> it = blockingQueue.iterator();
      while (it.hasNext()) {
        QueueItem foundItem = it.next();
        if (foundItem.isTuple()) {
          it.remove();
          tuplesDroppedMetric.bump();
          return true;
        }
      }
      return false;
    }
  }

  private class MultithreadedDropFirstCongestionPolicy extends DropFirstCongestionPolicy {
    // The override just adds synchronization
    @Override
    protected synchronized void add(Tuple tuple) throws InterruptedException {
      super.add(tuple);
    }

    // The override just adds synchronization
    @Override
    protected synchronized void add(Punctuation punct) throws InterruptedException {
      super.add(punct);
    }

    // The override just adds synchronization
    @Override
    protected synchronized void clearQueue() {
      super.clearQueue();
    }
  }

  private class DropLastCongestionPolicy extends CongestionPolicyImpl {
    @Override
    protected void add(Tuple tuple) throws InterruptedException {
      QueueItem item = new QueueTuple(tuple);
      if (blockingQueue.offer(item)) {
        tuplesQueuedMetric.bump();
        int size = blockingQueue.size();
        recentMaxItemsQueuedMetric.setMaxValue(size);
        maxItemsQueuedMetric.setMaxValue(size);
      } else {
        tuplesDroppedMetric.bump();
      }
    }
  }

  abstract class QueueItem {
    public abstract void handle(RuntimeStream<Tuple> handler) throws Exception;

    protected boolean isTuple() {
      return false;
    }
  }

  class QueueTuple extends QueueItem {
    QueueTuple(Tuple tuple) {
      this.tuple = tuple.asReadOnlyTuple();
    }

    private Tuple tuple;

    @Override
    public void handle(RuntimeStream<Tuple> handler) throws Exception {
      tuplesQueuedMetric.decrement();
      handler.tuple(tuple);
    }

    @Override
    protected boolean isTuple() {
      return true;
    }

    @Override
    public String toString() {
      return tuple.toString();
    }
  }

  class QueuePunct extends QueueItem {
    QueuePunct(Punctuation punct) {
      this.punct = punct;
    }

    private Punctuation punct;

    @Override
    public void handle(RuntimeStream<Tuple> handler) throws Exception {
      if (punct.equals(Punctuation.FINAL_MARKER)) {
        finalPunctsQueuedMetric.decrement();
      } else if (punct.equals(Punctuation.WINDOW_MARKER)) {
        windowPunctsQueuedMetric.decrement();
      }
      handler.mark(punct);
    }

    @Override
    public String toString() {
      return punct.toString();
    }
  }

  class QueueAction<A> extends QueueItem {
    QueueAction(StreamAction<A> action) {
      this.action = action;
    }

    private StreamAction<A> action;
    private A result = null;
    private boolean complete = false;

    @Override
    public void handle(RuntimeStream<Tuple> handler) throws Exception {
      result = handler.action(action);
      synchronized (this) {
        complete = true;
        this.notifyAll();
      }
    }

    @Override
    public String toString() {
      return action.toString();
    }

    public synchronized A waitForResult() throws InterruptedException {
      while (!complete) {
        this.wait();
      }
      return this.result;
    }
  }

  class ThreadedPortThread implements Runnable {

    private RuntimeStream<Tuple> handler;

    ThreadedPortThread(RuntimeStream<Tuple> handler) {
      this.handler = handler;
    }

    @Override
    public void run() {
      boolean shutdown = false;
      while (!shutdown) {

        QueueItem item = null;
        try {
          item = blockingQueue.take();
        } catch (InterruptedException e1) {
          // If the operator is shutting
          // down, this thread will get
          // interrupted.  In this case,
          // leave item == null so that
          // we will fall through to the
          // test for the shuttingDown
          // flag below.
          synchronized (ThreadedPortHandler.this) {
            shuttingDown = true;
          }
        }
        if (item != null) {
          try {
            item.handle(handler);
          } catch (InterruptedException e) {
            synchronized (ThreadedPortHandler.this) {
              shuttingDown = true;
            }
          } catch (RuntimeException e) {
            throw e;
          } catch (Exception e) {
            throw new RuntimeException(e);
          }
        }

        synchronized (ThreadedPortHandler.this) {
          shutdown = shuttingDown;
        }
      }
    }
  }

  private ThreadedPortThread threadedPortThread;
  private Thread thread;
  private boolean shuttingDown = false;

  private MetricValue tuplesQueuedMetric;
  private MetricValue windowPunctsQueuedMetric;
  private MetricValue finalPunctsQueuedMetric;
  private MetricValue tuplesDroppedMetric;
  private MetricValue maxItemsQueuedMetric;
  private MetricValue recentMaxItemsQueuedMetric;
  private MetricValue enqueueWaitsMetric;
  private BlockingQueue<QueueItem> blockingQueue;
  private int queueCapacity;
  private CongestionPolicyImpl congestionPolicy;

  public ThreadedPortHandler(
      RuntimeStream<Tuple> innerHandler,
      CongestionPolicy congestionPolicy,
      boolean singleThreadedOnInput,
      OperatorAdapter adapter,
      MetricValue queueSizeMetric,
      MetricValue tuplesQueuedMetric,
      MetricValue windowPunctsQueuedMetric,
      MetricValue finalPunctsQueuedMetric,
      MetricValue tuplesDroppedMetric,
      MetricValue maxItemsQueuedMetric,
      MetricValue recentMaxItemsQueuedMetric,
      MetricValue enqueueWaitsMetric) {

    this.tuplesQueuedMetric = tuplesQueuedMetric;
    this.windowPunctsQueuedMetric = windowPunctsQueuedMetric;
    this.finalPunctsQueuedMetric = finalPunctsQueuedMetric;
    this.tuplesDroppedMetric = tuplesDroppedMetric;
    this.maxItemsQueuedMetric = maxItemsQueuedMetric;
    this.recentMaxItemsQueuedMetric = recentMaxItemsQueuedMetric;
    this.enqueueWaitsMetric = enqueueWaitsMetric;

    this.queueCapacity = (int) queueSizeMetric.getValue();
    this.congestionPolicy = getCongestionPolicyImpl(congestionPolicy, singleThreadedOnInput);
    this.congestionPolicy.newBlockingQueue();
    this.threadedPortThread = new ThreadedPortThread(innerHandler);
    OperatorThreadFactory threadFactory = adapter.getUserThreadFactory();
    thread = threadFactory.newThread(threadedPortThread);
    thread.start();
  }

  private CongestionPolicyImpl getCongestionPolicyImpl(
      CongestionPolicy policy, boolean singleThreadedInput) {
    switch (policy) {
      case DROP_FIRST:
        if (singleThreadedInput) {
          return new DropFirstCongestionPolicy();
        }
        return new MultithreadedDropFirstCongestionPolicy();
      case DROP_LAST:
        return new DropLastCongestionPolicy();
      case WAIT:
      default:
        return new WaitCongestionPolicy();
    }
  }

  @Override
  public void tuple(Tuple tuple) throws Exception {
    // Add the tuple to the queue.  The queue
    // may wait or drop a tuple as a result.
    boolean done = false;

    while (!done) {
      try {
        congestionPolicy.add(tuple);
        done = true;
      } catch (InterruptedException e) {
        synchronized (this) {
          shuttingDown = true;
          done = shuttingDown;
        }
      }
    }
  }

  @Override
  public void mark(Punctuation mark) throws Exception {
    // Add the punct to the queue.  If the queue
    // is full, wait for room.

    if (mark == null || mark.equals(Punctuation.NON_EXISTENT)) {
      // do nothing
    } else {
      try {
        congestionPolicy.add(mark);
        if (mark.equals(Punctuation.FINAL_MARKER)) {
          finalPunctsQueuedMetric.bump();
        } else if (mark.equals(Punctuation.WINDOW_MARKER)) {
          windowPunctsQueuedMetric.bump();
        }
      } catch (InterruptedException e) {
        synchronized (this) {
          shuttingDown = true;
        }
      }
    }
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    QueueAction<A> futureAction = congestionPolicy.add(action);
    A result = futureAction.waitForResult();
    return result;
  }

  private static class DrainAction<A> implements StreamAction<A> {

    @Override
    public A call() throws Exception {
      return null;
    }

    @Override
    public StreamAction.Type getType() {
      return Type.DRAIN;
    }
  }

  public void drain() throws Exception {
    // When draining, we want to wait until all items
    // on the queue have been handled.  We do this
    // by enqueuing an action, which will not
    // be processed until after everything ahead of it in the
    // queue.
    action(new DrainAction<Void>());
  }

  public void reset() {
    // When resetting, we can just discard everything
    // on the queue.
    congestionPolicy.clearQueue();
  }
}
