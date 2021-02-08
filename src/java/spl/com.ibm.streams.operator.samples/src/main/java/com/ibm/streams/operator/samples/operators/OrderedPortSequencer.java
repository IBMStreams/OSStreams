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

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.model.PrimitiveOperator;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * OrderedPortSequencer extends PortSequencer to provide submission of tuples within a port ordered
 * by the portSequence attribute.
 */
@PrimitiveOperator(
    description =
        "Operator that maintains an per-input port `int64` sequence number for arriving tuples and sets it as the `portSequence` attribute of the output tuple and ensures tuples are submitted to its output port is sequence number order.")
public class OrderedPortSequencer extends PortSequencer {

  /**
   * Array of locks, one per port. Note this object's access is synchronized using this to guarantee
   * visibility across threads.
   */
  private Lock[] portLocks;

  /**
   * Create a Lock object for each input/output port (remember the class assumes a 1-1 mapping
   * between input and output ports). The lock is a "fair" lock to try and ensure the oldest waiting
   * thread is granted access first.
   */
  @Override
  public synchronized void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
    portLocks = new Lock[context.getNumberOfStreamingInputs()];
    for (int p = 0; p < portLocks.length; p++) portLocks[p] = new ReentrantLock(true);
  }

  /**
   * Get the lock for a specific port. Method is synchronized to guarantee visibility of the array
   * and its contents.
   *
   * @return Lock object for the port.
   */
  private synchronized Lock getPortLock(int port) {
    return portLocks[port];
  }

  /**
   * Process the tuple and submit it holding the port specific lock for the entire processing of the
   * tuple. This means the assignment of the portSequence attribute and submission of the tuple are
   * in the same critical section, leading to the order of submission matching the portSequence.
   */
  @Override
  public void process(StreamingInput<Tuple> port, Tuple tuple) throws Exception {

    final Lock portLock = getPortLock(port.getPortNumber());

    try {
      portLock.lock();
      super.process(port, tuple);
    } finally {
      // Ensure the lock is always released by calling the unlock
      // in a finally block.
      portLock.unlock();
    }
  }
}
