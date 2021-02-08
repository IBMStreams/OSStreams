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

package com.ibm.streams.flow.internal;

import com.ibm.streams.flow.handlers.StreamForwarder;
import com.ibm.streams.flow.handlers.StreamHandler;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import java.util.List;

/**
 * Tracks the final marks on input ports and once all N input ports have received and processed the
 * final marks then a final mark is delivered to each output port.
 *
 * <p>From the operator model.
 *
 * <p>By default, operators that have both input and output ports will automatically forward final
 * punctuations from their input ports to their output ports. This is achieved by generating a final
 * punctuation on an output port when a final punctuation is received on all input ports.
 */
class FinalTracker {

  private final List<StreamingOutput<OutputTuple>> outputs;
  private int expectedFinalMarks;

  FinalTracker(int expectedFinalMarks, List<StreamingOutput<OutputTuple>> outputs) {
    this.outputs = outputs;
    this.expectedFinalMarks = expectedFinalMarks;
  }

  private void seenFinal() throws Exception {
    synchronized (this) {
      if (--expectedFinalMarks > 0) return;
    }

    for (StreamingOutput<OutputTuple> output : outputs) output.punctuate(Punctuation.FINAL_MARKER);
  }

  StreamHandler<Tuple> finalWatcher(StreamHandler<Tuple> destination) {
    return new FinalMarkWatcher(destination);
  }

  /**
   * A handler that watches for the final mark while forwarding all stream tuples and marks.
   * Notifies the tracker when a final is seen.
   */
  private class FinalMarkWatcher extends StreamForwarder<Tuple> {

    FinalMarkWatcher(StreamHandler<Tuple> destination) {
      super(destination);
    }

    @Override
    public final void mark(Punctuation mark) throws Exception {
      super.mark(mark);
      if (mark == Punctuation.FINAL_MARKER) seenFinal();
    }
  }
}
