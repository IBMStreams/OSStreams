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

package com.ibm.streams.operator.state;

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OptionalContext;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.window.StreamWindowEvent;
import com.ibm.streams.operator.window.StreamWindowListener;
import java.util.Set;
import java.util.concurrent.Semaphore;

/**
 * Context information for the consistent region the operator is in. A consistent region is declared
 * using the SPL {@code consistent} annotation
 *
 * <p>In a consistent region an <em>establishing</em> operator establishes points where the region
 * becomes consistent. A region is consistent if all operators in the region have state that
 * reflects having completely processed all streams derived from the <em>establishing</em>
 * operator's output streams. <br>
 * Thus the <em>establishing</em> operator creates a <em>cut</em> point where it requires that the
 * region becomes consistent, by ensuring all operators in the region have processed all of its
 * output streams (tuples and punctuation marks), effectively the tuples and marks have completely
 * flowed through the region. A <em>cut</em> may be defined by the <em>establishing</em> operator,
 * such as at a file boundary, or periodically such as every five seconds. <br>
 * To establish the consistent state the <em>establishing</em> operator stops submitting tuples and
 * then calls {@link #makeConsistent()}. <br>
 * Each downstream operator in the region becomes consistent by {@link StateHandler#drain()
 * draining} its processing for all its input streams and then {@link
 * StateHandler#checkpoint(Checkpoint) checkpointing} its state. <br>
 * Once all operators in the region have made themselves consistent, the region has reached a
 * consistent state.
 *
 * <p>Operators may have input and output ports that are outside of the consistent region, in which
 * case these ports are not involved in becoming consistent. <br>
 * An operator in a consistent region will have at least one input or output port that is part of
 * the region.
 *
 * <p>Any operator in a consistent region must acquire a <em>permit</em> before submitting tuples or
 * punctuations to output ports in the region. Only background activity needs to explicitly acquire
 * and release permits, see {@link #getSubmissionSemaphore()} for details on permit handling.
 *
 * <p>
 *
 * <h4>Consistent Region limitations in InfoSphere&reg; Streams Version 4.0</h4>
 *
 * <UL>
 *   <LI>The {@link com.ibm.streams.flow.javaprimitives.JavaOperatorTester mock test framework} for
 *       Java primitives does not support testing of consistent region functionality
 * </UL>
 *
 * @see OperatorContext#getOptionalContext(Class)
 * @since InfoSphere&reg; Streams Version 4.0
 */
public interface ConsistentRegionContext extends OptionalContext {
  /**
   * Check if this operator is a start of a consistent region.
   *
   * @return True if this operator is a start of a consistent region, false otherwise.
   */
  public boolean isStartOfRegion();

  /**
   * Check if this operator is an end of a consistent region.
   *
   * @return True if this operator is an end of a consistent region, false otherwise.
   */
  public boolean isEndOfRegion();

  /**
   * Check if the operator is the trigger operator of a consistent cut region.
   *
   * @return True if the operator is the trigger operator of a consistent region, false otherwise.
   */
  public boolean isTriggerOperator();

  /** Return the set of this operators input ports that are part of this consistent region. */
  public Set<StreamingInput<?>> getInputPorts();

  /**
   * Get the region index.
   *
   * @return region index
   */
  public int getIndex();

  /**
   * This method returns the current sequence identifier of a consistent region.
   *
   * <p>The sequence identifier is the id that a drain or a reset should be associated to. After
   * operator startup, this method returns 1. On operator restart, the method returns -1 until it
   * reset.
   *
   * <p>While a submission <em>permit</em> is held the returned value is stable.
   *
   * @return current sequence id of a drain or reset or -1 if the method is accessed after the
   *     operator restarts due to a PE crash but before the operator resets.
   */
  public long getSequenceId();

  /**
   * This method returns the current number of attempts of resetting a consistent region.
   *
   * <p>If a drain is completed after a reset, this method returns -1.
   *
   * @return last reset attempt, or -1 if a drain has been successfully processed after a reset.
   */
  public int getResetAttempt();

  /**
   * Make the region consistent by draining processing and checkpointing state. <br>
   * {@code makeConsistent()} is called by {@link #isStartOfRegion() start operators} to establish a
   * point in time when the region was consistent. Normally the region becomes consistent by each
   * operator draining all processing and checkpointing its state, in the case this method returns
   * true. <br>
   * If a failure occurs, then the region becomes consistent by resetting to a previous consistent
   * state, in this case the method returns false.
   *
   * <p>The {@link #isTriggerOperator() triggering operator} calls {@code makeConsistent()} to start
   * the drain and checkpoint cycle. <br>
   * With periodic triggering of drain and checkpoint cycles, {@link #isStartOfRegion() start
   * operators} may also call {@code makeConsistent()} to start the drain and checkpoint cycle,
   * though the start may be delayed until the next scheduled cycle.
   *
   * <p>A drain and checkpoint cycle results in a call to {@link StateHandler#drain()} followed by
   * {@link StateHandler#checkpoint(com.ibm.streams.operator.state.Checkpoint)}. <br>
   * This call blocks until either the drain and and checkpoint cycle was completed for the region,
   * or until the region is reset, if a failure occurred. <br>
   * When {@code true} is returned it is guaranteed that the cut identified by the value of {@link
   * #getSequenceId()} upon entry was completed without any reset. Thus any tuples submitted prior
   * to this call or prior to the return from {@link StateHandler#drain()} have been completely
   * processed by the region and the resultant operators' state checkpointed. <br>
   * When {@code false} is returned it is guaranteed that the cut identified by the value of {@link
   * #getSequenceId()} upon entry was reset. Tuples submitted during that cut should be resubmitted
   * to ensure they are processed. <br>
   * If a drain and checkpoint cycle is not required or cannot be completed due to an ongoing reset,
   * then it may not be started. In this case then the {@code drain()} and {@code checkpoint}
   * methods may not be called, though the return value will represent the correct guarantee.
   *
   * <p>For correct behavior, upon entry the caller must have acquired (implicitly or explicitly) a
   * single {@link #acquirePermit() permit}. <br>
   * This permit is released during the method's processing but will be re-acquired before the
   * method returns. After the method returns, thus the caller still has a permit and thus is free
   * to continue to submit tuples. The current {@link #getSequenceId() sequence identifier} most
   * likely will have changed.
   *
   * <p>After the method returns the state of the operator may have changed, either due to a reset
   * or concurrent processing by other threads.
   *
   * <p>{@code makeConsistent()} <em>happens-before</em> its call to {@link StateHandler#drain()}.
   *
   * <p>Note that calling {@code makeConsistent()} in the start operator would block the tuple flow
   * until all operators in the consistent region have finished checkpointing, including those
   * operator which enable {@link #enableNonBlockingCheckpoint() non-blocking checkpointing}. In
   * order to achieve non-blocking checkpointing, please use {@link #makeConsistentNonBlocking()} in
   * the start operator.
   *
   * <p>Since Streams&reg; Version 4.2.0, before {@code makeConsistent()} returns true, the SPL
   * Runtime invokes start operator's {@link StateHandler#regionCheckpointed(long)} callback.
   *
   * @return {@code true} Region became consistent without any reset, {@code false} region was
   *     reset.
   */
  public boolean makeConsistent();

  /**
   * Reset this consistent region to its last consistent state. <br>
   * This method can be used by operators when detecting an operator-specific transient failure. If
   * the region is already being reset, a new attempt for reset is requested.
   *
   * @throws IOException Reset can not be requested.
   */
  public void reset() throws java.io.IOException;

  /**
   * Get the semaphore used to control processing and tuple submission. A <em>permit</em> from this
   * semaphore is required to modify state and submit tuples to output ports in this region. <br>
   * A <em>permit</em> is implicitly acquired by the SPL Runtime during calls to any of these
   * methods (and released once the method completes):
   *
   * <UL>
   *   <LI>Input port processing:
   *       <UL>
   *         <LI>{@link Operator#process(StreamingInput, Tuple)}
   *         <LI>{@link Operator#processPunctuation(StreamingInput,
   *             com.ibm.streams.operator.StreamingData.Punctuation)}
   *       </UL>
   *   <LI>Window event handling:
   *       <UL>
   *         <LI>{@link StreamWindowListener#handleEvent(StreamWindowEvent)}
   *       </UL>
   *   <LI>State handling:
   *       <UL>
   *         <LI>{@link StateHandler#drain()}
   *       </UL>
   * </UL>
   *
   * <br>
   * When a <em>permit</em> is implicitly acquired, operator implementations need not code any
   * permit acquisition or release, thus only background activity, such a threads in a source
   * operator, that needs to partake in the consistent region must acquire a permit. A typical
   * pattern for a source operator submitting tuples would be:
   *
   * <pre>
   * semaphore.acquire();
   * try {
   *   //get external data for tuples
   *   OutputTuple tuple = ...
   *   // update state to reflect tuple was submitted
   *   ...
   *   // submit tuples (can be one or many)
   *   getOutput(0).submit(tuple);
   * } finally {
   *    semaphore.release();
   * }
   * </pre>
   *
   * @return Semaphore used to acquire permits for tuple submission to output ports in this region.
   * @see #acquirePermit()
   * @see #releasePermit()
   */
  public Semaphore getSubmissionSemaphore();

  /**
   * Acquire a single submission <em>permit</em>. Convenience method equivalent to {@code
   * getSemaphore().acquire()}
   *
   * @throws InterruptedException
   */
  public void acquirePermit() throws InterruptedException;

  /**
   * Release a single submission <em>permit</em>. Convenience method equivalent to {@code
   * getSemaphore().release()}
   *
   * @throws InterruptedException
   */
  public void releasePermit();

  /**
   * Return a task that wraps {@code task} with permit acquisition and release for this region. The
   * permit is acquired before calling {@code task.run()} and released before the returned {@code
   * Runnable} {@code run()} method returns, even if an undeclared exception is thrown.
   *
   * @param task Task to be wrapped.
   * @return {@code task} wrapped in a {@code Runnable} that acquires and releases a permit.
   */
  public Runnable getPermitRunnable(Runnable task);

  /**
   * Get the drain timeout of the consistent region.
   *
   * @return drain timeout
   * @since InfoSphere&reg; Streams Version 4.0.1
   */
  public double getDrainTimeout();

  /**
   * Get the reset timeout of the consistent region.
   *
   * @return reset timeout
   * @since InfoSphere&reg; Streams Version 4.0.1
   */
  public double getResetTimeout();

  /**
   * Enables non-blocking checkpointing.
   *
   * <p>This method must be invoked from {@link Operator#initialize(OperatorContext)}. Non-blocking
   * checkpointing enables an operator to checkpoint its state while new tuples are processed. This
   * should be used only when the operator implements techniques that guarantee that the operator
   * state saved on the {@link StateHandler#checkpoint(Checkpoint)} call is consistent with
   * processing all tuples prior to the {@link StateHandler#drain()} call even after tuple
   * processing is resumed. One such technique is user-level copy-on-write of operator state.
   *
   * @since IBM Streams Version 4.2
   */
  public void enableNonBlockingCheckpoint();

  /**
   * Return value of {@link #makeConsistentNonBlocking()}.
   *
   * @since IBM Streams Version 4.2.0
   */
  public enum DrainResult {
    /*
     * Implementation note. The order of the enum matches the order
     * in the C++ code.
     */

    /** The region has been drained and checkpointed. */
    COMPLETED,

    /** The region has been drained but there is still pending non-blocking checkpoint. */
    CHECKPOINT_PENDING,

    /**
     * During draining the region, there was shutdown request, or there was failure and the region
     * was reseted.
     */
    FAILED
  }

  /**
   * Make the region consistent by draining processing and checkpointing state.
   *
   * <p>{@code makeConsistent()} has similar functionality as {@link #makeConsistent()}, and is
   * called by {@link #isStartOfRegion() start operators} to initiate the establishment of a
   * consistent state. <br>
   * If the region is operator driven, and the operator is the trigger operator of the region,
   * draining of the region starts immediately. If the region is periodic, the calling thread blocks
   * until the SPL runtime finishes the next periodic consistent state. If the operator needs to
   * submit more tuples prior to the start of a consistent state, such tuples can be submitted in
   * the {@link StateHandler#drain()} method. <br>
   * To successfully execute makeConsistentNonBlocking(), the thread must currently hold a permit.
   * If multiple threads call this method at the same time, only a single consistent state will be
   * established. <br>
   * The {@code makeConsistentNonBlocking()} method has similar functionality as {@link
   * #makeConsistent()}, but should be preferred when the consistent region contains operator(s)
   * with non-blocking checkpointing enabled. <br>
   * If a consistent region has no operator which enables non-blocking checkpointing, then the
   * {@code makeConsistentNonBlocking()} method returns {@code COMPLETED} when a consistent state
   * has successfully established, or {@code FAILED} when there is a shutdown request or there is a
   * failure and the region has been reset. So if the consistent region has no operator with
   * non-blocking checkpointing enabled, {@code makeConsistentNonBlocking()} returning {@code
   * COMPLETED} is equivalent to {@link #makeConsistent()} returning {@code true}, and {@code
   * makeConsistentNonBlocking()} returning {@code FAILED} is the same as {@link #makeConsistent()}
   * returning {@code false}. <br>
   * If a consistent region has any operator which enables non-blocking checkpointing, then the
   * {@code makeConsistentNonBlocking()} may also return {@code CHECKPOINT_PENDING}, indicating that
   * all blocking operators have finished checkpointing and all non-blocking operators have finished
   * preparation but there is still non-blocking checkpointing ongoing. The start operator can
   * resume tuple processing upon the return of {@code CHECKPOINT_PENDING}. In comparison, if {@link
   * #makeConsistent()} is used when the consistent region has any operator with non-blocking
   * checkpointing enabled, the {@link #makeConsistent()} call would not return until all operator
   * have finished checkpointing (including those non-blocking ones). <br>
   * If {@code makeConsistentNonBlocking()} returns {@code COMPLETED} (no matter whether there is
   * non-blocking operator in the region), a consistent state has been successfully established, and
   * it is safe to resume tuple processing and retire any tuples between this checkpoint and the
   * previous checkpoint. The previous checkpoint has also been retired by the SPL Runtime via
   * calling {@link StateHandler#retireCheckpoint(long)}. Before letting {@code
   * makeConsistentNonBlocking()} returning {@code COMPLETED}, the SPL Runtime also invokes start
   * operator's {@link StateHandler#regionCheckpointed(long)} callback. <br>
   * If {@code makeConsistentNonBlocking()} returns {@code CHECKPOINT_PENDING}, it is not safe to
   * retire tuples between this checkpoint and the previous checkpoint, and the SPL Runtime has not
   * yet retired the previous checkpoint; however, it is ok to resume tuple processing after {@code
   * CHECKPOINT_PENDING} is returned. When all the pending checkpoints are complete, the SPL Runtime
   * invokes start operator's {@link StateHandler#regionCheckpointed(long)} callback. <br>
   * {@code makeConsistentNonBlocking()} and {@link #makeConsistent()} cannot be used in mix.
   *
   * @return {@code COMPLETED} if all operators have finished checkpointing, return {@code
   *     CHECKPOINT_PENDING} if all operators that do blocking checkpointing have finished
   *     checkpointing, and all operators that do non-blocking checkpointing have finished
   *     preparation for non-blocking checkpointing, but at least one operator still has
   *     non-blocking checkpointing ongoing, return {@code FAILED} if there is failure and reset or
   *     shutdown request during the draining and checkpointing process.
   * @since IBM Streams Version 4.2
   */
  public DrainResult makeConsistentNonBlocking();
}
