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

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric.MetricValue;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * Generic tuple handling for direct handling of tuples with a generic port type. An instance
 * represents a single port. Handles metrics and finalization upon receipt of the final mark.
 *
 * <p>Used directly for: Output ports that connect to Java ports. In this case the MetricValue are
 * objects that update the C++ metrics.
 */
public class DirectTuple implements RuntimeStream<Tuple> {

  private final RuntimeStream<Tuple> rawHandler;

  final MetricValue nTuples;
  final MetricValue nWindowPuncts;
  final MetricValue nFinalPuncts;

  private final ReadWriteLock processing = new ReentrantReadWriteLock();
  private boolean finalized;

  /** Requires three metrics representing tuples count window mark count final mark count */
  public DirectTuple(final RuntimeStream<Tuple> rawHandler, MetricValue... metrics) {
    this.rawHandler = rawHandler;

    //
    nTuples = metrics[0];
    nWindowPuncts = metrics[1];
    nFinalPuncts = metrics[2];
  }

  final Lock activityLock() {
    return processing.readLock();
  }

  final Lock finalizationLock() {
    return processing.writeLock();
  }

  /**
   * Process a tuple for this input. The shared lock is held (which allows concurrent processing of
   * tuples and window punctuation before the abstract processTuple is called. Operator input port
   * metrics are calculated.
   */
  @Override
  public final void tuple(final Tuple tuple) throws Exception {
    if (tuple == null) throw new NullPointerException();

    handleTuple(tuple.asReadOnlyTuple());
  }

  /**
   * Handle a tuple by obtaining the shared lock, bumping the metric and then delivering the tuple.
   *
   * @param tuple
   * @throws Exception
   */
  private final void handleTuple(final Tuple tuple) throws Exception {
    activityLock().lock();
    try {
      if (finalized) return;

      nTuples.bump();
      rawHandler.tuple(tuple);
    } finally {
      activityLock().unlock();
    }
  }

  @Override
  public final void mark(Punctuation mark) throws Exception {
    if (mark == null) throw new NullPointerException();

    if (mark == Punctuation.NON_EXISTENT) return;

    handlePunctuation(mark);
  }

  /**
   * Handle a punctuation mark. If the punctuation is a final marker then obtain a write lock to
   * ensure no other tuple processing and then mark the port as finalized before delivering the
   * tuple.
   *
   * <p>If the punctuation is not a final marker then obtain a read lock and deliver the
   * punctuation.
   *
   * <p>In both cases the correct metric is bumped.
   */
  private final void handlePunctuation(Punctuation mark) throws Exception {
    final Lock lock = mark == Punctuation.FINAL_MARKER ? finalizationLock() : activityLock();

    lock.lock();
    try {
      if (finalized) return;

      switch (mark) {
        case FINAL_MARKER:
          nFinalPuncts.bump();
          finalized = true;
          break;
        case WINDOW_MARKER:
          nWindowPuncts.bump();
          break;
      }

      rawHandler.mark(mark);

    } finally {
      lock.unlock();
    }
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    switch (action.getType()) {
        // For a drain and reset, drain the processing of the port to ensure
        // tuples flowing between C++/Java do not get out of order.
      case DRAIN:
      case RESET:
        drain(false);
        break;
      default:
        break;
    }
    return rawHandler.action(action);
  }

  /**
   * Drain all work out of the port to ensure that the marker handed to C++ doesn't race ahead of
   * the in-progress tuples since it takes a different path.
   *
   * @param finalize true if the port should be finalized after draining
   */
  public void drain(boolean finalize) {
    final Lock lock = finalizationLock();
    lock.lock();
    if (finalize) finalized = true;
    lock.unlock();
  }
}
