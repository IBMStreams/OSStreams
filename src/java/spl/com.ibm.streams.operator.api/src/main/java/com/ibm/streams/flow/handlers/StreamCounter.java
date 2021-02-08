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

package com.ibm.streams.flow.handlers;

import com.ibm.streams.operator.StreamingData.Punctuation;
import java.util.EnumMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

/**
 * Count the number of tuples and punctuation marks that are seen on a stream. <br>
 * Useful for testing graphs by verifying the output of an operator produced the correct number of
 * tuples and punctuation marks.
 *
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public class StreamCounter<T> implements StreamHandler<T> {

  private final Map<Punctuation, Long> markCounts =
      new EnumMap<Punctuation, Long>(Punctuation.class);
  private long tupleCount;

  /** Construct a {@code StreamCounter} that initializes all the counts to zero. */
  public StreamCounter() {
    reset();
  }

  /**
   * Return the number of tuples that are seen on the stream.
   *
   * @return Number of tuples that are seen on the stream.
   */
  public synchronized long getTupleCount() {
    return tupleCount;
  }

  /**
   * Return the number of punctuation marks of type {@code mark} that are seen on the stream.
   *
   * @return Number of punctuation marks of type {@code mark} that are seen on the stream.
   */
  public synchronized long getMarkCount(Punctuation mark) {
    return markCounts.get(mark);
  }

  /** Count the number of tuples. */
  @Override
  public synchronized void tuple(T tuple) throws Exception {
    tupleCount++;
    notifyAll();
  }

  /** Count the number of punctuation marks. */
  @Override
  public synchronized void mark(Punctuation mark) throws Exception {
    markCounts.put(mark, markCounts.get(mark) + 1L);
  }

  /** Reset all the counts to zero. */
  public synchronized void reset() {
    tupleCount = 0;
    for (Punctuation mark : Punctuation.values()) markCounts.put(mark, 0L);
  }

  public boolean waitForTuples(final int count, long timeout, TimeUnit unit)
      throws InterruptedException {

    final long ems = System.currentTimeMillis() + unit.toMillis(timeout);
    synchronized (this) {
      while (tupleCount < count) {
        final long waitms = ems - System.currentTimeMillis();
        if (waitms <= 0) return false;
        wait(waitms);
      }
    }
    return true;
  }
}
