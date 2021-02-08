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

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.samples.state.Counter;
import com.ibm.streams.operator.state.ConsistentRegionContext;
import com.ibm.streams.operator.state.DelegateStateHandler;
import java.util.logging.Logger;

/**
 * Abstract pattern class for an operator that produces a single tuple periodically on a single
 * output port. This class extends PollingTupleProducer by providing an implementation of
 * fetchTuples() which calls fetchSingleTuple at the polling frequency. Sub-classes provide an
 * implementation of fetchSingleTuple to set the tuple's attribute values.
 *
 * <p>If the operator parameter <code>iterations</code> is set then it is taken as the number of
 * tuples that will be submitted. If not set or if sub-classes do not call setIterations() then the
 * operator will run until the job is terminated.
 */
public abstract class PollingSingleTupleProducer extends PollingTupleProducer {

  /** SPL Trace facility */
  private static final Logger trace = Logger.getLogger(PollingSingleTupleProducer.class.getName());

  /** Number of tuples to submit, -1 means submit until the operator is terminated. */
  private int iterations = -1;

  /** Number of tuples submitted so far. */
  private Counter counter;

  /**
   * {@inheritDoc}
   *
   * <p>Sets the number of iterations from the operator parameter <code>iterations</code>
   */
  @Override
  public synchronized void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
    counter = new Counter();
    context.registerStateHandler(getStateHandler(counter));
  }

  protected DelegateStateHandler<Counter> getStateHandler(Counter counter) {
    return new DelegateStateHandler<Counter>(counter);
  }

  /**
   * Set the number of iterations.
   *
   * @param iterations Number of tuples to submit, -1 means submit tuples until the job is
   *     terminated.
   */
  @Parameter(
      optional = true,
      description =
          "The number of tuples to submit. Negative value or zero means continually fetch and submit tuples.")
  public final synchronized void setIterations(int iterations) {
    if (iterations < 0) iterations = -1;
    trace.log(TraceLevel.DEBUG, "iterations=" + iterations);
    this.iterations = iterations;
  }

  /**
   * Call {@link #fetchSingleTuple(OutputTuple)} to allow a sub-class to submit a tuple. Keeps track
   * of the number of tuples submitted and if the number to submit has been limited then complete()
   * is called to terminate the periodic call of this method, and then a window and final
   * punctuation marks are submitted.
   */
  @Override
  protected synchronized void fetchTuples() throws Exception {

    if (iterations != -1 && counter.getCount() == iterations) return;

    final StreamingOutput<OutputTuple> output = getOutput(0);

    final OutputTuple tuple = output.newTuple();

    if (fetchSingleTuple(tuple)) output.submit(tuple);
    else return;

    if (iterations != -1) {
      if (counter.increment() == iterations) {
        trace.log(TraceLevel.DEBUG, "Complete - sending window & final marks");
        output.punctuate(Punctuation.WINDOW_MARKER);
        completedTuples();
      }
    }
  }

  /**
   * Complete the operator. Called when the parameter {@link #setIterations(int) iterations} is set
   * and the required number of tuples has been submitted. Terminates the polling task by calling
   * {@link PollingTupleProducer#complete() super.complete()}. <br>
   * If this operator is in a consistent region then before terminating the polling task, we must
   * ensure all of the tuples have been processed, by calling {@link
   * ConsistentRegionContext#makeConsistent()}. If a failure occurred a reset will have occurred and
   * we must continue to send tuples as, as the count of submitted tuples will have been reset. Thus
   * if {@code makeConsistent()} returns {@code false} then {@code super.complete()} is not called.
   */
  protected void completedTuples() throws Exception {
    ConsistentRegionContext crc =
        getOperatorContext().getOptionalContext(ConsistentRegionContext.class);
    if (crc != null) {
      // In a consistent region
      if (!crc.makeConsistent()) return;

    } else {
      getOutput(0).punctuate(Punctuation.FINAL_MARKER);
    }

    // cancel the repeating task
    super.complete();
  }

  /**
   * Fetch the information for the tuple and set its attributes. Return true to submit the tuple to
   * the output port, false to not send. Only submitted tuples are counted against the number of
   * iterations.
   *
   * <p>When in a {@link ConsistentRegionContext consistent region} a submission permit is acquired
   * before {@code fetchSingleTuple()} is invoked and released once it completes.
   *
   * @param tuple Tuple to be submitted.
   * @return True to submit the tuple, false to not submit.
   */
  protected abstract boolean fetchSingleTuple(OutputTuple tuple) throws Exception;
}
