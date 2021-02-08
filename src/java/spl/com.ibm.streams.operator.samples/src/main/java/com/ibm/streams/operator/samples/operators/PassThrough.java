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

package com.ibm.streams.operator.samples.operators;

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.PrimitiveOperator;

/**
 * Simple sample operator that passes all input unchanged from any input port to its single output
 * port. All input ports and the output port must have the same schema.
 */
@PrimitiveOperator(
    description =
        "Simple sample operator that passes all input unchanged from any input port to its output port.")
@InputPortSet
@OutputPortSet(cardinality = 1)
public class PassThrough extends AbstractOperator {

  /** Verify the output port schema matches all input ports. */
  @ContextCheck
  public static void matchingPorts(OperatorContextChecker checker) {
    final OperatorContext context = checker.getOperatorContext();
    for (StreamingInput<?> input : context.getStreamingInputs())
      checker.checkMatchingSchemas(input, context.getStreamingOutputs().get(0));
  }

  /** Forward the input tuple directly to output port 0. */
  @Override
  public void process(StreamingInput<Tuple> stream, Tuple tuple) throws Exception {
    getOutput(0).submit(tuple);
  }

  /** Forward the punctuation mark directly to output port 0. */
  @Override
  public void processPunctuation(StreamingInput<Tuple> stream, Punctuation mark) throws Exception {
    getOutput(0).punctuate(mark);
  }
}
