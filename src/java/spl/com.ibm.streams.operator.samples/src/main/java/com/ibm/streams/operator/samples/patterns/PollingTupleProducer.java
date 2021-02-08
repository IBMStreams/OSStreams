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
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.state.ConsistentRegionContext;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

/**
 * Abstract pattern class for an operator that produces tuples by polling an external source for
 * information and converting that information into outgoing tuples. This class just provides the
 * mechanism for polling, sub-classes implement fetchTuples() which will be called at the polling
 * frequency.
 */
public abstract class PollingTupleProducer extends TupleProducer {

  /** SPL Trace facility */
  private static final Logger trace = Logger.getLogger(PollingTupleProducer.class.getName());

  private long pollFrequency = 1;
  private TimeUnit pollFrequencyUnit = TimeUnit.SECONDS;

  /**
   * Get the polling frequency, defaults to 1.
   *
   * @return Current frequency.
   */
  public final synchronized long getPollFrequency() {
    return pollFrequency;
  }

  /**
   * Get the unit for the polling frequency, defaults to SECONDS.
   *
   * @return Unit for polling frequency.
   */
  public final synchronized TimeUnit getPollFrequencyUnit() {
    return pollFrequencyUnit;
  }

  /**
   * Set the polling frequency. Must be called in a sub-class's initialize() to have an effect as
   * the values are read by startProcessing().
   *
   * @param pollFrequency Polling frequency.
   * @param unit Unit of the polling frequency.
   */
  public final synchronized void setPollFrequency(long pollFrequency, TimeUnit unit) {
    this.pollFrequency = pollFrequency;
    this.pollFrequencyUnit = unit;
  }

  /**
   * Sets the polling period in seconds from the optional operator parameter {@code float64}
   * parameter {@code period}. Resolution from the value is milliseconds.
   *
   * @param period Polling period in seconds.
   */
  @Parameter(
      optional = true,
      description = "Polling period in seconds. Tuples will be fetched every `period` seconds.")
  public void setPeriod(double period) {
    setPollFrequency((long) (period * 1000.0), TimeUnit.MILLISECONDS);
  }

  private ScheduledFuture<?> future;

  /** {@inheritDoc} */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
  }

  /**
   * {@inheritDoc}
   *
   * <p>Starts processing by scheduling a fixed rate task that will call fetchTuples() at the
   * configured poll frequency.
   */
  @Override
  public synchronized void allPortsReady() throws Exception {

    final long delay = getPollFrequencyUnit().convert(getInitialDelay(), getInitialDelayUnit());

    trace.log(TraceLevel.DEBUG, "initialDelay=" + getInitialDelay() + " " + getInitialDelayUnit());
    trace.log(
        TraceLevel.DEBUG, "pollFrequency=" + getPollFrequency() + " " + getPollFrequencyUnit());
    trace.log(TraceLevel.DEBUG, "delay=" + delay);

    final Runnable tupleFetcher = new InvokeFetchTuples();
    final Runnable scheduledTupleFetcher;
    final ConsistentRegionContext crc =
        getOperatorContext().getOptionalContext(ConsistentRegionContext.class);
    if (crc == null) {
      scheduledTupleFetcher = tupleFetcher;
    } else {
      scheduledTupleFetcher =
          new Runnable() {

            @Override
            public void run() {
              if (crc.getSubmissionSemaphore().tryAcquire()) {
                try {
                  tupleFetcher.run();
                } finally {
                  crc.getSubmissionSemaphore().release();
                }
              }
            }
          };
    }

    future =
        getOperatorContext()
            .getScheduledExecutorService()
            .scheduleAtFixedRate(
                scheduledTupleFetcher, delay, getPollFrequency(), getPollFrequencyUnit());
  }

  private class InvokeFetchTuples implements Runnable {
    @Override
    public void run() {
      if (trace.isLoggable(TraceLevel.TRACE))
        trace.log(
            TraceLevel.TRACE, PollingTupleProducer.this.getClass().getName() + ".fetchTuples()");
      try {
        fetchTuples();
      } catch (Exception e) {
        trace.log(TraceLevel.ERROR, e.toString(), e);
        throw new RuntimeException(e);
      }
    }
  }

  /**
   * Instead of using <code>TupleProducer.startProcessing</code> to implement the initial delay, use
   * the delay parameter of <code>ScheduledExecutorService.scheduleAtFixedRate</code>.
   *
   * @see ScheduledExecutorService#scheduleAtFixedRate(Runnable, long, long, TimeUnit)
   * @see TupleProducer#allPortsReady()
   */
  @Override
  protected final void startProcessing() {}

  /**
   * Called periodically to fetch tuples from an external source and submit them to the output
   * stream(s). Implementations will typically access the external source to fetch the available
   * data, create OutputTuples based upon the data, submit them to the output ports and then return.
   * <br>
   * Implementation must not block.
   *
   * <p>When in a {@link ConsistentRegionContext consistent region} a submission permit is acquired
   * before {@code fetchTuples()} is invoked and released once it completes.
   */
  protected abstract void fetchTuples() throws Exception;

  /** Complete the operator by canceling the periodic task that calls <code>fetchTuples</code>. */
  protected synchronized void complete() throws Exception {
    if (future != null) {
      future.cancel(false);
      future = null;
    }
  }

  /**
   * Complete this operator by canceling the periodic task that implements the polling.
   *
   * <p>Sub-classes <b>must</b> call <code>super.shutdown()</code> if they override this method.
   */
  @Override
  public void shutdown() throws Exception {
    complete();
    super.shutdown();
  }
}
