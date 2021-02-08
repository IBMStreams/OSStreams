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
import com.ibm.streams.operator.Tuple;
import java.util.Collection;
import java.util.LinkedList;

/**
 * Collect tuples seen on a stream. <br>
 * Useful for testing graphs to verify the output of an operator produced the correct tuples.
 *
 * @param <C> Collection type
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public class StreamCollector<C extends Collection<T>, T> implements StreamHandler<T> {

  private final C tuples;

  /**
   * Create a new {@code StreamCollector} that uses a {@code LinkedList} to collect tuples. Access
   * to the collection returned by {@link #getTuples()} must be synchronized using the collection's
   * reference.
   *
   * @return A new {@code StreamCollector} that uses a {@code LinkedList} to collect tuples.
   */
  public static StreamCollector<LinkedList<Tuple>, Tuple> newLinkedListCollector() {
    return new StreamCollector<LinkedList<Tuple>, Tuple>(new LinkedList<Tuple>());
  }

  /**
   * Construct a {@code StreamCollector} that will use {@code tuples} to collect tuples seen on the
   * stream.
   */
  public StreamCollector(C tuples) {
    this.tuples = tuples;
  }

  /**
   * Get the tuple collection.
   *
   * @return The tuple collection
   */
  public C getTuples() {
    return tuples;
  }

  /**
   * Get the number of tuples in the collection returned by {@link #getTuples()}. Note that if
   * tuples have been removed from or added to the collection outside of this class then the
   * returned value does not represent the number of tuples seen on the stream.
   *
   * @return Number of collected tuples.
   */
  public int getTupleCount() {
    synchronized (tuples) {
      return getTuples().size();
    }
  }

  /**
   * Add the tuple to the collection. Addition of {@code tuple} is synchronized using the referent
   * to the collection passed into the constructor and returned by {@link #getTuples()}.
   *
   * @param tuple Tuple seen on stream
   */
  @Override
  public void tuple(T tuple) throws Exception {
    synchronized (tuples) {
      tuples.add(tuple);
    }
  }

  /**
   * No action is taken with punctuation marks. Sub-classes may override this method to perform
   * specific checks of the contents of the {@link #getTuples() collected tuples} when a punctuation
   * mark is seen.
   */
  @Override
  public void mark(Punctuation mark) throws Exception {}
}
