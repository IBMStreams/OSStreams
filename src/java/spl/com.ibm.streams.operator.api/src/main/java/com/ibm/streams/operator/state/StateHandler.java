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
import java.io.Closeable;

/**
 * Handler for operator state. If an {@link Operator} is an instance of {@code StateHandler} then it
 * is automatically registered as a state handler, otherwise an operator registers an instance of
 * this interface using {@link OperatorContext#registerStateHandler(StateHandler)}. <br>
 * Operators that need to manage state are encouraged to always utilize a {@code StateHandler}
 * either by implementing {@code StateHandler} or registering a state handler.
 *
 * <p>The SPL Runtime invokes {@code StateHandler} methods when:
 *
 * <UL>
 *   <LI>{@link CheckpointContext checkpointing} is enabled.
 *   <LI>the operator is in a {@link ConsistentRegionContext consistent region}.
 * </UL>
 *
 * {@code StateHandler} implementations should be careful to honor the contract of this interface to
 * allow use in future scenarios. For example, no assumptions should be made about the operator
 * being in a consistent region or checkpointing, and tuple submission should only occur from {@link
 * #drain()}. <HR>
 *
 * <p>When periodic {@link CheckpointContext checkpointing} is enabled the SPL Runtime will call
 * {@link #checkpoint(Checkpoint)} for each registered state handler at the configured period. <br>
 * With operator driven checkpointing the operator calls {@link
 * CheckpointContext#createCheckpoint()} to create a checkpoint, that will result in a callback to
 * {@link #checkpoint(Checkpoint)} for all registered handlers. These callbacks occur on the thread
 * that called {@code createCheckpoint()}. <br>
 * After PE restart, the SPL Runtime will reset the operator to a persisted state by calling {@link
 * #reset(Checkpoint)} or {@link #resetToInitialState()}. <HR>
 *
 * <p>In a {@link ConsistentRegionContext consistent region}, an operator drains its processing and
 * persists its state for the cut point defined by the <em>establishing</em> operator. The SPL
 * runtime calls {@link #drain()} and {@link #checkpoint(Checkpoint)} at the correct times to ensure
 * all operators in a region drain their processing and persist their state, consistent with having
 * seen all tuples and marks up to the point defined by the <em>establishing</em> operator. <br>
 * An operator becomes consistent with this sequence:
 *
 * <OL>
 *   <LI>The operator has processed all {@link
 *       Operator#process(com.ibm.streams.operator.StreamingInput, com.ibm.streams.operator.Tuple)
 *       tuples} and {@link Operator#processPunctuation(com.ibm.streams.operator.StreamingInput,
 *       com.ibm.streams.operator.StreamingData.Punctuation) marks} on any data (non-control) input
 *       ports in the region.
 *   <LI>Draining operator processing using {@link #drain()} for all state handlers.
 *   <LI>The operator has processed all {@link
 *       Operator#process(com.ibm.streams.operator.StreamingInput, com.ibm.streams.operator.Tuple)
 *       tuples} and {@link Operator#processPunctuation(com.ibm.streams.operator.StreamingInput,
 *       com.ibm.streams.operator.StreamingData.Punctuation) marks} on any control input ports in
 *       the region.
 *   <LI>Persisting state using {@link #checkpoint(Checkpoint)} for all state handlers.
 * </OL>
 *
 * Note that each of these steps is optional, for example a stateless operator may only perform step
 * 2 to drain processing. <br>
 * If the operator code enables non-blocking checkpoint (via invoking {@link
 * ConsistentRegionContext#enableNonBlockingCheckpoint()}), the {@link
 * #prepareForNonBlockingCheckpoint(long)} method is invoked by the SPL Runtime after the {@link
 * #drain()} method. The tuple flow is resumed after the return of {@link
 * #prepareForNonBlockingCheckpoint(long)} method. Meanwhile the SPL Runtime delegates an internal
 * thread to execute the {@link #checkpoint(Checkpoint)} method in the background. Background
 * checkpointing is internally tracked by the SPL Runtime. <br>
 * For start operators of a consistent region, the {@link #regionCheckpointed(long)} method is
 * called once all the operators in the consistent region is drained an checkpointed, including
 * those operators performing non-blocking checkpointing. <br>
 * After a failure in the region, the SPL Runtime will reset the operator to a consistent state by
 * calling {@link #reset(Checkpoint)} or {@link #resetToInitialState()}. <HR>
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
public interface StateHandler extends Closeable {

  /**
   * Drain any outstanding processing. Once this method returns, the operator is indicating that it
   * has drained all outstanding processing for all input ports. <br>
   * The method may:
   *
   * <UL>
   *   <LI>Submit tuples to output ports.
   *   <LI>Interact with external systems
   *   <LI>Wait for background activity to drain outstanding work
   * </UL>
   *
   * <p>For a consistent region:
   *
   * <UL>
   *   <LI>This method is called once all tuples and marks for the current cut for all data
   *       (non-control) input ports in the region have been processed, meaning the {@link
   *       Operator#process(com.ibm.streams.operator.StreamingInput, com.ibm.streams.operator.Tuple)
   *       process} and {@link Operator#processPunctuation(com.ibm.streams.operator.StreamingInput,
   *       com.ibm.streams.operator.StreamingData.Punctuation) processPunctuation} have returned for
   *       all tuples and marks.
   *   <LI>A {@link ConsistentRegionContext#acquirePermit() permit} is implicitly acquired before
   *       the method is called, and released once it returns.
   *   <LI>This method is not called concurrently with tuple or mark processing for any data
   *       (non-control) input ports in the region.
   *   <LI>This method is may be called concurrently with tuple or mark or mark processing for any
   *       autonomous input ports
   * </UL>
   *
   * @throws Exception Exception attempting to drain state.
   */
  public void drain() throws Exception;

  /**
   * Reset the operator to a previous state defined by {@code checkpoint}.
   *
   * <p>When in a consistent region {@code checkpoint} corresponds to the a previous successful
   * drain and checkpoint cut cycle for the entire region. state.
   *
   * @throws Exception Exception attempting to reset state.
   */
  public void reset(Checkpoint checkpoint) throws Exception;

  /**
   * Checkpoint the operator state to support resetting of its state to the saved state, using
   * {@link #reset(Checkpoint)}.
   *
   * <p>For a consistent region this method is called when:
   *
   * <UL>
   *   <LI>{@link #drain()} has completed
   *   <LI>All background threads have released processing permits.
   *   <LI>All control input ports (if any) have been drained.
   * </UL>
   *
   * @throws Exception Exception attempting to checkpoint state.
   */
  public void checkpoint(Checkpoint checkpoint) throws Exception;

  /**
   * Called when a checkpoint can be retired.
   *
   * @param id Id of checkpoint.
   * @throws Exception Exception attempting to retire a checkpoint state.
   */
  public void retireCheckpoint(long id) throws Exception;

  /**
   * Reset the operator to its initial state.
   *
   * <p>If the operator is in a consistent region then this is called if a reset occurs prior to the
   * first successful completion of drain and checkpoint cycle for the region. <br>
   * This method is not called when the operator first initializes.
   *
   * @throws Exception Exception attempting to retire a checkpoint state.
   */
  public void resetToInitialState() throws Exception;

  /**
   * Called to prepare for non-blocking checkpoint.
   *
   * <p>This method is executed only when the operator enables non-blocking checkpointing with the
   * {@link ConsistentRegionContext#enableNonBlockingCheckpoint()} method. This method is guaranteed
   * to execute after {@link #drain()} and before {@link #checkpoint(Checkpoint)}. It is also
   * guaranteed to execute before the consistent region resumes tuple processing. <br>
   * This method was added after the interface was released in Version 4.0. It is defined as a
   * default method for compatibility reasons. The default implementation is sufficient if the
   * operator does not support non-blocking checkpoint.
   *
   * @param id Id of checkpoint.
   * @throws Exception Exception attempting to prepare for non-blocking checkpoint.
   * @since IBM Streams Version 4.2
   */
  public default void prepareForNonBlockingCheckpoint(long id) throws Exception {}

  /**
   * Called when the whole consistent region has been drained and checkpointed.
   *
   * <p>When all operators in a consistent region have finished checkpointing (including
   * non-blocking checkpointing), this method is called to notify start operator(s) of the region.
   * This method is called only for start operator(s) of a consistent region. <br>
   * This method was added after the interface was released in Version 4.0. It is defined as a
   * default method for compatibility reasons.
   *
   * @param id Id of checkpoint.
   * @throws Exception Exception attempting to notify start operator that the region has been
   *     drained and checkpointed.
   * @since IBM Streams Version 4.2
   */
  default void regionCheckpointed(long id) throws Exception {}
}
