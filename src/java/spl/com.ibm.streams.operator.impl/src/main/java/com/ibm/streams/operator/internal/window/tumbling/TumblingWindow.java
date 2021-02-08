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

package com.ibm.streams.operator.internal.window.tumbling;

import com.ibm.streams.operator.internal.window.PartitionFactory;
import com.ibm.streams.operator.internal.window.WindowHandler;
import com.ibm.streams.operator.internal.window.WindowPartition;
import com.ibm.streams.operator.internal.window.WindowedTuple;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowEvent;
import java.util.ArrayList;
import java.util.List;

public abstract class TumblingWindow<T, P>
    extends WindowPartition<T, P, WindowedTuple<T>, List<WindowedTuple<T>>> {

  private static final long serialVersionUID = 2492007070007096410L;

  protected TumblingWindow(WindowHandler<T> windowHandler, P partition) {
    this(windowHandler, partition, 1000);
  }

  protected TumblingWindow(WindowHandler<T> windowHandler, P partition, int capacity) {
    super(windowHandler, partition, new ArrayList<WindowedTuple<T>>(capacity));
  }

  /**
   * Evict all the tuples from the tumbling window first notifying the listener with an event.
   *
   * @throws Exception
   */
  void tumble() throws Exception {
    try {
      actionEvent(StreamWindowEvent.Type.EVICTION);
    } finally {
      evictTupleCount(getTuples().size());
      getTuples().clear();
    }
  }

  public static <T> PartitionFactory<T, ? extends TumblingWindow<T, ?>> getTumbleFactory(
      StreamWindow<T> window) {

    switch (window.getEvictionPolicy()) {
      case TIME:
        return new PartitionFactory<T, TumblingTimeEvict<T, Object>>() {

          @Override
          public void initialize(WindowHandler<T> handler) {}

          @Override
          public TumblingTimeEvict<T, Object> newPartition(
              WindowHandler<T> handler, Object partition) {
            return new TumblingTimeEvict<T, Object>(handler, partition);
          }
        };
      case COUNT:
        return new PartitionFactory<T, TumblingCountEvict<T, Object>>() {

          @Override
          public TumblingCountEvict<T, Object> newPartition(
              WindowHandler<T> handler, Object partition) {
            return new TumblingCountEvict<T, Object>(handler, partition);
          }
        };
      case PUNCTUATION:
        return new PartitionFactory<T, TumblingPunctuation<T, Object>>() {

          @Override
          public TumblingPunctuation<T, Object> newPartition(
              WindowHandler<T> handler, Object partition) {
            return new TumblingPunctuation<T, Object>(handler, partition);
          }
        };
      case DELTA:
        return new PartitionFactory<T, TumblingDeltaEvict<T, Object>>() {

          @Override
          public TumblingDeltaEvict<T, Object> newPartition(
              WindowHandler<T> handler, Object partition) {
            return new TumblingDeltaEvict<T, Object>(handler, partition);
          }
        };
      case NONE:
        assert false;
    }

    return null;
  }
}
