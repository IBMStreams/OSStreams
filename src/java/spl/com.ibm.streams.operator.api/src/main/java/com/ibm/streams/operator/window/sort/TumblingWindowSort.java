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

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowEvent;
import java.util.List;

/**
 * Implement a tuple window sort for a tumbling window. When the window tumbles if it contained one
 * or more tuples then the tuples are submitted to the output port in sorted order. Any tuples left
 * in the window when a final punctuation mark is received are optionally submitted in sorted order
 * to the output port.
 *
 * <p>This class supports partitioned and non-partitioned windows.
 */
public final class TumblingWindowSort extends SortedTupleWindow {

  private final StreamingOutput<?> outputPort;
  private final boolean flushOnFinal;

  /**
   * Creates and registers a listener for the window.
   *
   * @param window Window description for input port.
   * @param ascending True if the sort is to be in ascending order.
   * @param attributeNames Attributes to be sorted listed in order of significance, with most
   *     significant first.
   * @param outputPort Output port to submit sorted tuples on.
   * @param flushOnFinal True if tuples remaining in the window are to be submitted when a final
   *     punctuation mark is received. False if remaining tuples are to be discarded.
   */
  public TumblingWindowSort(
      StreamWindow<Tuple> window,
      boolean ascending,
      List<String> attributeNames,
      StreamingOutput<?> outputPort,
      boolean flushOnFinal) {
    super(window, ascending, attributeNames);
    this.outputPort = outputPort;
    this.flushOnFinal = flushOnFinal;
  }

  /** Only support a TUMBLING window. */
  @Override
  protected boolean supportsWindow() {
    return getWindow().getType() == StreamWindow.Type.TUMBLING;
  }

  /**
   * Handle a TUMBLING window events. <br>
   * For a EVICTION event, all tuples in the window are submitted to the output port in sorted
   * order,, followed by a WINDOW_MARKER. <br>
   * For the FINAL, if flushOnFinal is true then all remaining tuples in the window are are
   * submitted to the output port in sorted order, followed by a WINDOW_MARKER, otherwise no action
   * is taken. <br>
   * If no tuples are present in the window at the EVICTION or FINAL events then no WINDOW_MARKER is
   * sent to the output port. <br>
   * After this class's handling of the events <code>super.handleEvent(event)</code> is called to
   * allow {@link SortedTupleWindow} to maintain its state (including sorting of tuples on
   * INSERTION) correctly.
   */
  @Override
  public void handleEvent(StreamWindowEvent<Tuple> event) throws Exception {
    switch (event.getType()) {
      case EVICTION:
        submitTuples(event);
        break;
      case FINAL:
        if (flushOnFinal) {
          submitTuples(event);
        }
        break;
      default:
        break;
    }
    super.handleEvent(event);
  }

  /**
   * Output all the tuples in window sorted from low to high followed by a window punctuation
   * marker. If the window is empty no tuples and no punctuation mark is sent.
   */
  private void submitTuples(StreamWindowEvent<Tuple> event) throws Exception {

    final List<Tuple> state = getPartitionState(event.getPartition());

    if (state.isEmpty()) return;

    for (Tuple tuple : state) {
      outputPort.submit(tuple);
    }
    outputPort.punctuate(Punctuation.WINDOW_MARKER);
  }
}
