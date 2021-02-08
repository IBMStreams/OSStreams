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

package com.ibm.streams.operator.samples.sources;

import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.operator.types.Timestamp;

/**
 * Sample source operator that creates a tuple with random values and a timestamp. Extension of
 * {@link RandomBeacon} that assumes the output schema has a <code>timestamp ts</code> attribute,
 * and sets it to the current time when the tuple was submitted. All other attributes in the output
 * tuple are set to random values by the parent.
 *
 * <p>This operator provided as the sample Java primitive operator <br>
 * {@code com.ibm.streams.javaprimitivesamples.sources.RandomBeaconWithTimestamp} <br>
 * in the sample {@code JavaOperators} toolkit located in: <br>
 * {@code $STREAMS_INSTALL/samples/spl/feature/JavaOperators}
 */
@PrimitiveOperator(
    description = "Sample source operator that creates a tuple with random values and a timestamp.")
public class RandomBeaconWithTimestamp extends RandomBeacon {

  /**
   * Check that the output port schema contains an attribute {@code ts} of type {@code timestamp}.
   */
  @ContextCheck
  public static void checkTsAttribute(OperatorContextChecker checker) {
    final StreamingOutput<?> port = checker.getOperatorContext().getStreamingOutputs().get(0);
    checker.checkAttributes(port, false, "tuple<timestamp ts>");
  }

  /**
   * Use the parent-class's <code>fetchSingleTuple</code> and then overwrites the <code>ts</code>
   * attribute's value with a current timestamp.
   */
  @Override
  protected boolean fetchSingleTuple(final OutputTuple tuple) throws Exception {

    super.fetchSingleTuple(tuple);

    tuple.setTimestamp("ts", Timestamp.currentTime());

    // Always submit the tuple
    return true;
  }
}
