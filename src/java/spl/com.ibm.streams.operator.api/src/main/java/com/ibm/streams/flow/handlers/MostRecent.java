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

/**
 * Keeps track of the most recent tuple and punctuation mark that are seen on a stream. <br>
 * Useful for testing graphs by capturing the output of an operator after a tuple is submitted to
 * that operator. For example, testing a filter style operator.
 *
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public class MostRecent<T> implements StreamHandler<T> {

  private T mostRecentTuple;
  private Punctuation mostRecentMark;

  /**
   * Return the most recent tuple that is seen on the stream.
   *
   * @return The most recent tuple that is seen on the stream or null if a tuple has not been seen
   *     since construction or since {@link MostRecent#clearMostRecentTuple()} was called.
   */
  public synchronized T getMostRecentTuple() {
    return mostRecentTuple;
  }

  /**
   * Return the most recent punctuation that is seen on the stream.
   *
   * @return The most recent punctuation mark that is seen on the stream or null if a mark has not
   *     been seen since construction or since {@link MostRecent#clearMostRecentMark()} was called.
   */
  public synchronized Punctuation getMostRecentMark() {
    return mostRecentMark;
  }

  /**
   * Clear the most recent tuple and punctuation mark by calling {@link #clearMostRecentTuple()} and
   * {@link #clearMostRecentMark()}.
   */
  public synchronized void clear() {
    clearMostRecentTuple();
    clearMostRecentMark();
  }

  /**
   * Clear the most recent punctuation mark. After this call {@link #getMostRecentMark()} returns
   * {@code null} until a punctuation mark is seen on the stream.
   */
  public synchronized void clearMostRecentMark() {
    mostRecentMark = null;
  }

  /**
   * Clear the most recent tuple. After this call {@link #getMostRecentTuple()} returns {@code null}
   * until a tuple is seen on the stream.
   */
  public synchronized void clearMostRecentTuple() {
    mostRecentTuple = null;
  }

  /**
   * Save the most recent tuple. After this call, {@link #getMostRecentTuple()} returns {@code
   * tuple} until {@link #clearMostRecentTuple()} is called.
   */
  @Override
  public synchronized void tuple(T tuple) throws Exception {
    mostRecentTuple = tuple;
  }

  /**
   * Save the most recent punctuation mark. After this call, {@link #getMostRecentMark()} returns
   * {@code mark} until {@link #clearMostRecentMark()} is called.
   */
  @Override
  public synchronized void mark(Punctuation mark) throws Exception {
    mostRecentMark = mark;
  }
}
