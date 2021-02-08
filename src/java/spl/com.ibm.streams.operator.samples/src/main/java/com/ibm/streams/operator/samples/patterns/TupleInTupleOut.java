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

package com.ibm.streams.operator.samples.patterns;

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;
import com.ibm.streams.operator.model.OutputPorts;

/**
 * Abstract pattern class for an operator that receives a tuple and then optionally submits a tuple.
 * This pattern supports a number of input streams and a single output stream. A sub-class provides
 * optional filtering and transformation.
 */
@InputPorts(@InputPortSet)
@OutputPorts(
    @OutputPortSet(
        cardinality = 1,
        windowPunctuationOutputMode = WindowPunctuationOutputMode.Preserving))
public abstract class TupleInTupleOut extends AbstractOperator {

  /**
   * Initialize this operator. Called once before any tuples are processed. Sub-classes must call
   * super.initialize(context) to correctly setup an operator.
   */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
  }

  /** Get the output schema for the single output port. */
  public final StreamSchema getOutputSchema() {
    return getOutput().getStreamSchema();
  }

  /** Get the single output. */
  public final StreamingOutput<OutputTuple> getOutput() {
    return getOutput(0);
  }

  /**
   * Called when a tuple is received on the input port. First filter() is called on the incoming
   * tuple, if that returns true transform() will be called otherwise no further action is taken.
   */
  @Override
  public final void process(StreamingInput<Tuple> inputStream, Tuple tuple) throws Exception {

    if (!filter(inputStream, tuple)) return;

    StreamingOutput<OutputTuple> outStream = getOutput();

    OutputTuple outTuple = outStream.newTuple();

    if (transform(inputStream, tuple, outTuple)) outStream.submit(outTuple);
  }

  /**
   * Perform any filtering rules on the incoming tuple. This implementation returns true. <br>
   * Sub-classes override this method to provide specific filtering.
   *
   * @param inputStream
   * @param tuple
   * @return True to continue processing this tuple and call transform, false to stop processing
   *     this tuple and do not send any output tuples.
   */
  protected boolean filter(StreamingInput<Tuple> inputStream, Tuple tuple) {
    return true;
  }

  /**
   * Perform any required transformation.
   *
   * @param inputStream
   * @param tuple Incoming tuple.
   * @param outTuple An empty tuple.
   * @return True to send outTuple, false to not send the tuple.
   * @throws Exception
   */
  protected abstract boolean transform(
      StreamingInput<Tuple> inputStream, Tuple tuple, OutputTuple outTuple) throws Exception;
}
