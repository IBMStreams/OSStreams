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
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.operator.model.PrimitiveOperator;

/**
 * Simple class that maintains an per-input port int64 sequence number for arriving tuples and sets
 * it as the portSequence attribute of the output tuple. This class is to demonstrate
 * synchronization on a per-port basis. Each incoming tuple is assigned to an output port at the
 * same index using OutputTuple.assign(), that is the code assumes there are N input ports and N
 * output ports. <br>
 * The portSequence attribute value corresponds to order arrival of the tuples to the process
 * method, there is no guarantee that this operator will submit tuples in the portSequence order.
 * See PortSequencer.process() for details.
 */
@PrimitiveOperator(
    description =
        "Operator that maintains an per-input port `int64` sequence number for arriving tuples and sets it as the `portSequence` attribute of the output tuple.")
@InputPorts(@InputPortSet)
@OutputPorts(@OutputPortSet)
public class PortSequencer extends AbstractOperator {

  /** Array of sequence number for the tuples for each port. */
  private long[] portSequences;

  @ContextCheck
  public static void checkPorts(OperatorContextChecker checker) {
    final OperatorContext context = checker.getOperatorContext();

    // Verify the number of output ports is the same as
    // the input ports.
    if (context.getNumberOfStreamingOutputs() != context.getNumberOfStreamingInputs()) {
      checker.setInvalidContext(
          "Number of output ports {0} must match the number of input ports {1}.",
          new Object[] {
            context.getNumberOfStreamingOutputs(), context.getNumberOfStreamingInputs()
          });
    }

    // Verify that each output port has a int64 portSequence attribute
    for (StreamingOutput<?> port : context.getStreamingOutputs()) {
      checker.checkRequiredAttributes(port, "portSequence");
      checker.checkAttributeType(
          port.getStreamSchema().getAttribute("portSequence"), MetaType.INT64);
    }
  }

  /**
   * Initialize the portSequences field holding the Operator's (this) synchronization to ensure
   * later threads fetching it using getPortSequences() are guaranteed to see the initialized value.
   */
  @Override
  public synchronized void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
    portSequences = new long[context.getNumberOfStreamingInputs()];
  }

  /**
   * Get the portSequences instance field using synchronization. The synchronization (on this)
   * guarantees the reader will see the initialized value. Accessing the field directly does not
   * guarantee a thread will see the initialized value due to the visibility rules defined by the
   * Java Memory model.
   *
   * @return the portSequences
   */
  private synchronized long[] getPortSequences() {
    return portSequences;
  }

  /**
   * Process a tuple simply assigning it to the output port with the same index and adding the
   * portSequence field set to the sequence of the arriving tuple within the port.
   *
   * <p>Synchronization is on the StreamingInput object corresponding to the port the tuple arrived
   * on. This per-port synchronization allows threads processing tuples from different input ports
   * to concurrently calculate their own port sequence value.
   *
   * <p>Note that the order submission of tuples is <b>not</b> guaranteed by this method since no
   * lock is held between the assignment of portSequence and the submission of the tuple. Thus, for
   * example, after one thread has portSequence=100 another thread may grab portSequence=101 and
   * this '101' thread may call submit() before the '100' thread.
   */
  @Override
  public void process(StreamingInput<Tuple> port, Tuple tuple) throws Exception {

    /*
     * Synchronize the port sequence on the StreamingInput
     * reference for the port the tuple arrives on. Note
     * the synchronization is only held to calculate and
     * set the portSequence to reduce contention by making
     * the critical section as small as possible.
     */
    long portSequence;
    synchronized (port) {
      portSequence = getPortSequences()[port.getPortNumber()]++;
    }

    StreamingOutput<OutputTuple> outPort = getOutput(port.getPortNumber());

    OutputTuple outTuple = outPort.newTuple();

    outTuple.assign(tuple);
    outTuple.setLong("portSequence", portSequence);

    // No synchronization held during submission.
    getOutput(port.getPortNumber()).submit(outTuple);
  }
}
