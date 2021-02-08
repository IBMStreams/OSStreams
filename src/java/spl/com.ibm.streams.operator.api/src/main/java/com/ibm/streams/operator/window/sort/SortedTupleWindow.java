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

package com.ibm.streams.operator.window.sort;

import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.window.StatefulWindowListener;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowEvent;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;

/**
 * Abstract window handler that sorts incoming generic tuples according to a comparator for the
 * window's tuples. Each partition's state contains the full set as a List&lt;Tuple&gt; of tuples in
 * the window for that partition in order of the comparator. <br>
 * Tuples that are duplicates according to the comparator are kept in the state, though the order
 * within duplicates is not defined.
 */
public abstract class SortedTupleWindow extends StatefulWindowListener<List<Tuple>, Tuple> {

  private final Comparator<Tuple> comparator;

  /**
   * Sort tuples according to a set of attributes using comparators that match the natural ordering
   * of the Java object mapping for the attributes SPL type.
   *
   * @param window Window being sorted.
   * @param ascending True if the comparator is to order from low to high, false for high to low.
   * @param attributeNames Names of attributes to be included in the ordering. If no names are
   *     supplied then the ordering will be across all attributes that are comparable.
   * @see com.ibm.streams.operator.StreamSchema#getAttributesComparator(boolean, String...)
   */
  protected SortedTupleWindow(
      StreamWindow<Tuple> window, boolean ascending, String... attributeNames) {
    this(
        window,
        window.getInputPort().getStreamSchema().getAttributesComparator(ascending, attributeNames));
  }

  /**
   * Sort tuples according to a set of attributes using comparators that match the natural ordering
   * of the Java object mapping for the attributes SPL type.
   *
   * @param window Window being sorted.
   * @param ascending True if the comparator is to order from low to high, false for high to low.
   * @param attributeNames Names of attributes to be included in the ordering. If no names are
   *     supplied then the ordering will be across all attributes that are comparable.
   */
  protected SortedTupleWindow(
      StreamWindow<Tuple> window, boolean ascending, List<String> attributeNames) {
    this(window, ascending, attributeNames.toArray(new String[attributeNames.size()]));
  }

  /**
   * Sort the tuples according to a specific comparator.
   *
   * @param window Window being sorted.
   * @param comparator Comparator used to sort.
   */
  protected SortedTupleWindow(StreamWindow<Tuple> window, Comparator<Tuple> comparator) {
    super(window);
    this.comparator = comparator;
  }

  /** Return a new linked list each time. */
  @Override
  protected List<Tuple> getInitializedState(Object partition, List<Tuple> state) {
    return new LinkedList<Tuple>();
  }

  /** Handle INSERTION and EVICTION events to maintain a sorted List of tuples in the window. */
  @Override
  public void handleEvent(StreamWindowEvent<Tuple> event) throws Exception {
    switch (event.getType()) {
      case INSERTION:
        insertTuples(event);
        break;
      case EVICTION:
        if (getWindow().getType() == StreamWindow.Type.SLIDING) evictTuples(event);
        break;
    }
  }

  /**
   * Insert the tuple into the partition's state maintaining the the order required by the
   * comparator.
   */
  private void insertTuples(StreamWindowEvent<Tuple> event) {

    final List<Tuple> state = getPartitionState(event.getPartition());

    for (Tuple tuple : event.getTuples()) {
      int index = getOrderedIndex(state, tuple);
      if (index < 0) {
        // Tuple is not in the list
        // index = (-(insertion point) - 1)
        // =>
        // -(index + 1) = insertion point
        index = -(index + 1);
      }

      state.add(index, tuple);
    }
  }

  /** Remove the evicted tuples from the partition's state. */
  private void evictTuples(StreamWindowEvent<Tuple> event) {
    final List<Tuple> state = getPartitionState(event.getPartition());
    for (Tuple tuple : event.getTuples()) {
      state.remove(getOrderedIndex(state, tuple));
    }
  }

  private int getOrderedIndex(List<Tuple> state, Tuple tuple) {
    return Collections.binarySearch(state, tuple, comparator);
  }
}
