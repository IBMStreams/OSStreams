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

package com.ibm.streams.operator.samples.windows;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.window.StatefulWindowListener;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowEvent;
import java.math.BigDecimal;
import java.math.MathContext;

/**
 * Sample window listener to demonstrate window aggregation on a decimal attribute. This class
 * incrementally maintains the number of tuples in the window and the sum of a decimal attribute in
 * the AggregateInfo class. As tuples are inserted their attribute's values are added to the sum and
 * subtracted from the sum as they are evicted. <br>
 * Through the facilities of StatefulWindowListener this class supports partitioned and
 * non-partitioned windows. An AggregateInfo is maintained for each partition. <br>
 * Sliding and tumbling windows are supported. The trigger for a sliding window or the eviction for
 * a tumbling window results in a output tuple being delivered to the specified output port. The
 * attributes <code>sum</code> and <code>count</code> will be set to the sum and count values
 * respectively. Each output tuple will be followed by a {@link
 * com.ibm.streams.operator.StreamingData.Punctuation#WINDOW_MARKER WINDOW_MARKER} punctuation mark.
 * The class supports all window definitions.
 */
public class DecimalSumAggregatorListener
    extends StatefulWindowListener<DecimalSumAggregatorListener.AggregateInfo, Tuple> {

  /** Attribute being aggregated. */
  protected final String attributeName;

  /** Math context used for arithmetic. Set from the context of the attribute being aggregated. */
  protected final MathContext mc;

  /** Output port tuples containing the aggregate values are delivered to. */
  protected final StreamingOutput<OutputTuple> outputPort;

  /**
   * @param window
   * @param attributeName
   * @param outputPort
   */
  public DecimalSumAggregatorListener(
      StreamWindow<Tuple> window, String attributeName, StreamingOutput<OutputTuple> outputPort) {
    super(window);

    this.attributeName = attributeName;
    this.outputPort = outputPort;

    Attribute attr = window.getInputPort().getStreamSchema().getAttribute(attributeName);

    mc = attr.getType().getMetaType().getMathContext();
  }

  /** Class for the per-partition state for the aggregate. */
  public static class AggregateInfo {
    /** Sum of the attribute's value for tuples in the window. */
    public BigDecimal sum = BigDecimal.ZERO;

    /** Number of tuples in the window. */
    public int count;
  }

  /** Create a new AggregateInfo as the initial state. */
  @Override
  protected AggregateInfo getInitializedState(Object partition, AggregateInfo state) {
    return new AggregateInfo();
  }

  /** Perform window based aggregation with incremental state updates. */
  @Override
  public void handleEvent(StreamWindowEvent<Tuple> event) throws Exception {

    AggregateInfo state = getPartitionState(event.getPartition());

    switch (event.getType()) {
      case INITIAL_FULL:
        /**
         * Nothing to do, StatefulWindowListener provides a utility method to track if the
         * INITIAL_FULL has occurred on a per-partition basis.
         */
        break;

      case INSERTION:
        /*
         * On insertion update the sum and count with each tuple being
         * inserted by adding its attribute's value to the sum.
         */
        for (Tuple tuple : event.getTuples()) {
          state.sum = state.sum.add(tuple.getBigDecimal(attributeName), mc);
          state.count++;
        }
        break;

      case EVICTION:
        if (getWindow().getType() == StreamWindow.Type.TUMBLING) {
          // No need to maintain the state incrementally,
          // each tumble will allocate a new state via
          // getInitializedState().
          // Just submit the aggregation.
          submitAggregateOutput(state);
          break;
        }

        /*
         * Sliding window only.
         * On eviction update the sum and count with each tuple being
         * removed by subtracting its attribute's value from the sum.
         */
        for (Tuple tuple : event.getTuples()) {
          if (mc != null) state.sum = state.sum.subtract(tuple.getBigDecimal(attributeName), mc);
          state.count--;
        }
        break;

      case TRIGGER:
        // Only trigger once a full window has been seen.
        if (!seenInitialFull(event.getPartition())) break;

        /** Submit the aggregation to the output port. */
        submitAggregateOutput(state);
        break;

      case FINAL:
        /**
         * Submit the partial aggregation if any tuples remain in the window at final marker time.
         */
        submitAggregateOutput(state);
        outputPort.punctuate(StreamingData.Punctuation.FINAL_MARKER);
        break;
    }
  }

  /**
   * Submit the aggregate tuple for this partition.
   *
   * @param state State for a partition.
   * @throws Exception Exception submitting information.
   */
  protected void submitAggregateOutput(AggregateInfo state) throws Exception {

    // Only submit output if the window contains tuples.
    if (state.count == 0) return;

    OutputTuple tuple = outputPort.newTuple();
    setupOutputTuple(tuple, state);

    outputPort.submit(tuple);
    outputPort.punctuate(StreamingData.Punctuation.WINDOW_MARKER);
  }

  /**
   * Set attributes in the tuple representing the aggregation. Sets the <code>sum</code> and <code>
   * count</code> fields.
   *
   * @param tuple Tuple that will be submitted.
   * @param state Partitioned aggregation state.
   */
  protected void setupOutputTuple(OutputTuple tuple, AggregateInfo state) {
    tuple.setBigDecimal("sum", state.sum);
    tuple.setInt("count", state.count);
  }
}
