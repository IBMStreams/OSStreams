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
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.model.Parameter;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

/**
 * Abstract pattern class for an operator that consumes tuples and does not produce an output
 * stream. Incoming tuples are processed in batches, driven by the size of the batch and a timeout.
 * A batch is processed when it reaches at least getBatchSize() or at least getBatchTimeout() time
 * have passed since the first tuple was added to the batch.
 *
 * <p>The batch is processed by the processBatch provided by a concrete sub-class. <br>
 * Processing of the batch is asynchronous with incoming tuples.
 *
 * <p>A batch size of 1 (the default) is not treated specially, in this case each incoming tuple may
 * start batch processing meaning that all processing of the tuple in processBatch() is handled
 * asynchronously with other tuple arrival.
 *
 * <p>Note that there is no requirement to use this class to implement a sink style operator. <br>
 */
public abstract class TupleConsumer extends AbstractOperator {

  /** A batched tuple representing the tuple and the port it arrived on. */
  public static class BatchedTuple {
    private final StreamingInput<Tuple> stream;
    private final Tuple tuple;

    BatchedTuple(StreamingInput<Tuple> stream, Tuple tuple) {
      this.stream = stream;
      this.tuple = tuple;
    }

    /**
     * Get the port the tuple arrived on.
     *
     * @return The stream the tuple arrived on.
     */
    public final StreamingInput<Tuple> getStream() {
      return stream;
    }

    /**
     * Get the tuple.
     *
     * @return The tuple.
     */
    public final Tuple getTuple() {
      return tuple;
    }
  }

  protected Queue<BatchedTuple> batch;

  private int batchSize = 1;

  private long batchTimeout = 0;

  private TimeUnit batchTimeoutUnit;

  private boolean preserveOrder = true;

  /**
   * Initialize this operator. Called once before any tuples are processed. Sub-classes must call
   * super.initialize(context) to correctly setup an operator.
   */
  @Override
  public synchronized void initialize(OperatorContext context) throws Exception {
    super.initialize(context);

    batch = newBatch();
  }

  private Queue<BatchedTuple> newBatch() {
    return new LinkedList<BatchedTuple>();
  }

  private Future<Object> activeTimeout;

  /**
   * Process incoming tuples by adding them to the current batch and submitting the batch for
   * execution if required.
   */
  @Override
  public synchronized void process(StreamingInput<Tuple> stream, Tuple tuple) throws Exception {

    batch.add(new BatchedTuple(stream, tuple));

    if (batch.size() < getBatchSize()) {
      if (getBatchTimeout() == 0) return;

      if (activeTimeout != null && !activeTimeout.isDone()) return;

      // If we have a timeout for batch submission then the
      // first time a tuple gets added to the batch create a
      // timeout event to process the batch in case no more tuples come in.
      {
        activeTimeout =
            getOperatorContext()
                .getScheduledExecutorService()
                .schedule(
                    new Callable<Object>() {
                      public Object call() throws Exception {
                        submitBatch(false);
                        return null;
                      }
                    },
                    getBatchTimeout(),
                    getBatchTimeoutUnit());
      }
      return;
    }

    submitBatch(false);
  }

  /**
   * Process any final punctuation by flushing the batch and waiting for all the work to complete.
   */
  @SuppressWarnings("incomplete-switch")
  public void processPunctuation(StreamingInput<Tuple> port, Punctuation mark) throws Exception {
    // No need to call super.processPunctuation because that just forwards
    // window punctuation marks to the output ports and this operator
    // doesn't support output ports.

    // If a final marker comes in then need to flush out any remaining tuples.
    if (mark != Punctuation.FINAL_MARKER) return;
    switch (mark) {
      case WINDOW_MARKER:
        break;
      case FINAL_MARKER:
        flushTuples(port);
        break;
    }
  }

  /** Process all batched tuples from this port. */
  private void flushTuples(StreamingInput<Tuple> port) {

    outer:
    for (; ; ) {

      // Submit any remaining batch tuples.
      submitBatch(false);

      // Need to wait for a current batch to complete
      // otherwise it would be a busy wait calling
      // submitBatch().
      batchWait();

      synchronized (this) {
        // See if we have any tuples left
        // from this port.
        for (BatchedTuple bt : batch) {
          // At least one, need to continue submitting.
          if (bt.getStream() == port) continue outer;
        }

        // No tuples in the current batch,
        // but need to ensure that all other
        // batches are complete.
        if (batchesComplete()) break;
      }
    }
  }

  /**
   * Discard all batched tuples from this port.
   *
   * @param port
   */
  // TODO: Remove this code. Used by RESET_MARKER, but that punctuation
  // is no longer exposed to Java code.
  /* private void discardTuples(StreamingInput<Tuple> port) {
      for (;;) {
          synchronized (this) {
              for (Iterator<BatchedTuple> it = batch.iterator(); it.hasNext();) {
                  BatchedTuple bt = it.next();
                  if (bt.getStream() == port)
                      it.remove();
              }

              if (batchesComplete())
                  return;
          }
          batchWait();
      }
  } */

  /**
   * Set of the background tasks that have been kicked off for batch processing (calling
   * processBatch()).
   */
  private final Set<Future<?>> activeBatches = new HashSet<Future<?>>();

  /** Test to see if all the batches are complete. */
  private synchronized boolean batchesComplete() {
    boolean allComplete = true;
    for (Iterator<Future<?>> i = activeBatches.iterator(); i.hasNext(); ) {
      Future<?> batch = i.next();
      if (batch.isDone()) i.remove();
      else allComplete = false;
    }
    return allComplete;
  }

  /** Wait for any outstanding batch item. */
  private void batchWait() {
    Future<?> waitBatch = null;

    synchronized (this) {
      for (Iterator<Future<?>> i = activeBatches.iterator(); i.hasNext(); ) {
        Future<?> batch = i.next();
        if (!batch.isDone()) {
          waitBatch = batch;
          break;
        }
      }
    }
    if (waitBatch != null)
      try {
        waitBatch.get();
      } catch (InterruptedException e) {;
      } catch (ExecutionException e) {;
      }
  }

  /**
   * Submit a batch of tuples to be processed by the sub-classes processBatch using a asynchronous
   * task.
   *
   * @param submitter True if this is being called from a task that itself was created by
   *     submitBatch.
   */
  private synchronized void submitBatch(boolean submitter) {
    // We are processing so remove any existing timeout
    if (activeTimeout != null) {
      activeTimeout.cancel(false);
      activeTimeout = null;
    }

    // Async threads may mean this is called just after
    // another submit batch so if nothing to do, just complete.
    if (batch.isEmpty()) return;

    boolean allComplete = batchesComplete();

    // Limit the number of outstanding tasks.
    // if (!allComplete && activeBatches.size() > taskThreadold )
    //    return;

    // If we are the submitter then even if preserveOrder is true we can continue
    // because we must be the only submitter.
    if (!submitter && preserveOrder) {
      // If we are preserving order then we cannot
      // have more than one processBatch() request outstanding
      // as it would be non-deterministic which one ran first,
      // thus leading to processing tuples out of order.
      if (!allComplete) return;
    }

    final Queue<BatchedTuple> asyncBatch = batch;
    batch = newBatch();
    activeBatches.add(
        getOperatorContext()
            .getScheduledExecutorService()
            .submit(
                new Callable<Object>() {
                  public Object call() throws Exception {

                    boolean addLeftovers = processBatch(asyncBatch);

                    synchronized (TupleConsumer.this) {
                      // Add remaining elements back into the queue
                      // placing them at the front.
                      if (addLeftovers && !asyncBatch.isEmpty()) {
                        asyncBatch.addAll(batch);
                        batch = asyncBatch;
                      }

                      if (batch.size() < getBatchSize()) return null;

                      // Will need to submit a batch if we just added
                      // enough tuples to exceed the batch size, or
                      // we are preserving order and enough tuples
                      // arrived while this batch was being processed.
                      submitBatch(true);
                    }
                    return null;
                  }
                }));
  }

  /**
   * Get the batch size.
   *
   * @return Current batch size, defaults to 1 if setBatchSize() has not been called.
   */
  public final synchronized int getBatchSize() {
    return batchSize;
  }

  /**
   * Set the current batch size. If least batchSize tuples are currently batched then batch
   * processing will be initiated asynchronously.
   *
   * @param batchSize New batch size, must be one or greater.
   */
  @Parameter(optional = true, description = "Set the batch size.")
  public synchronized void setBatchSize(int batchSize) {
    if (batchSize <= 0)
      throw new IllegalArgumentException("Sink batchSize must be not be zero or negative.");

    this.batchSize = batchSize;

    // When called via the Parameter annotation
    // there will not batch set up.
    if (batch != null) {
      if (batch.size() >= batchSize) submitBatch(false);
    }
  }

  /**
   * Get the batch timeout value.
   *
   * @return Current batch timeout, defaults to 0 (no timeout) if setBatchTimeout has not been
   *     called.
   */
  public final synchronized long getBatchTimeout() {
    return batchTimeout;
  }

  /**
   * Set the batch timeout. The batch timeout is the amount of time the operator will wait after the
   * first tuple in the batch before processing the batch. For example with a batch size of 100 and
   * a timeout of two seconds a batch will be processed when whichever comes first out of 100 tuples
   * are in the batch or 2 seconds have passed since the first tuple was added. <br>
   * This method should be made at initialization time, within the initialize method. Calling it
   * once tuples are being received and batched may have unpredictable effects.
   *
   * <p>A timeout of 0 means no timeout.
   *
   * @param batchTimeout Timeout value
   * @param unit Unit of the timeout value
   */
  public final synchronized void setBatchTimeout(long batchTimeout, TimeUnit unit) {
    this.batchTimeout = batchTimeout;
    this.batchTimeoutUnit = unit;
  }

  /**
   * Process a batch of tuples.
   *
   * <p>This call is made asynchronously with incoming tuples and tuples that arrive after this
   * processing was initiated will not be added to set defined by the passed in collection batch.
   * Such tuples will be processed in a future batch. <br>
   * The expected case is that this method will process all tuples in the batch, but a mechanism
   * exists to allow a subset of tuples to be processed in a future batch. If this method returns
   * true any tuples remaining in the passed in collection are added into a future batch. If false
   * is returned then it is assumed that all tuples have been processed and none will processed in
   * the future. <br>
   * No assumptions should be made about the number of tuples in the passed in collection batch, it
   * may be less than, equal to or greater than the current batch size. <br>
   * If tuple arrival order is not being preserved then this method may be called concurrently by
   * multiple threads (with different batches) and thus needs to be thread safe.
   *
   * @param batch Batch of tuples to be processed, its contents are not modified by any incoming
   *     tuples.
   * @return True process any tuples left in batch in a future batch, false no tuples are added back
   *     for future processing.
   */
  protected abstract boolean processBatch(Queue<BatchedTuple> batch) throws Exception;

  /**
   * Set if order of tuples is preserved in the batch and in the calls to processBatch. If set to
   * true then no concurrent calls will be made to processBatch. <br>
   * This method should be made at initialization time, within the initialize method. Calling it
   * once tuples are being received and batched may have unpredictable effects.
   *
   * @param preserveOrder True to preserve order, false to not care about ordering.
   */
  public final synchronized void setPreserveOrder(boolean preserveOrder) {
    this.preserveOrder = preserveOrder;
  }

  /**
   * True if arrival order is to be preserved when processing tuples.
   *
   * @return True if arrival order is to be preserved.
   */
  public final synchronized boolean isPreserveOrder() {
    return preserveOrder;
  }

  public synchronized TimeUnit getBatchTimeoutUnit() {
    return batchTimeoutUnit;
  }
}
