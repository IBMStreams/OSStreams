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

package com.ibm.streams.operator.internal.runtime.api;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.internal.metrics.OperatorMetricsFactory;
import com.ibm.streams.operator.state.ConsistentRegionContext.DrainResult;
import com.ibm.streams.operator.types.Timestamp;
import java.util.List;

/**
 * Provides an interface for an operator's execution context, which is either the mock framework or
 * the JNI full Streams runtime.
 */
public interface ExecutionContext {

  /**
   * Get the processing element.
   *
   * @return
   */
  ProcessingElement getPE();

  /**
   * Get the metrics factory.
   *
   * @return
   */
  OperatorMetricsFactory<?, ?> getMetricsFactory();

  /** Get the mechanism used to deliver tuples to downstream operators. */
  // List<RuntimeStream<Tuple>> getOutputStreams();

  /** Get the output ports that this operator can deliver tuples on. */
  List<StreamingOutput<OutputTuple>> getOutputPorts();

  /**
   * Get the thread local.
   *
   * @return
   */
  void setOperatorTracker(Adapter adapter);

  /**
   * Reset the thread local.
   *
   * @return
   */
  void resetOperatorTracker();

  /**
   * Get the current sequence number. This method should be invoked only if the operator is in a
   * consistent region.
   */
  long getConsistentRegionSequenceId();

  /**
   * Get the reset attempt. This method should be invoked only if the operator is in a consistent
   * region.
   */
  int getConsistentRegionResetAttempt();

  /** Contacts the ConsistentRegionController to start a reset of the region */
  void resetConsistentRegion() throws java.io.IOException;

  /** Contacts the ConsistentRegionController to indicate a reset of the region is required. */
  void mustResetConsistentRegion();

  /**
   * Make a consistent region consistent. This method should be invoked only by a start operator in
   * a consistent region.
   */
  boolean makeConsistent();

  /**
   * Return domain credentials.
   *
   * @return
   */
  String[] getDomainCredentials();

  /**
   * Set the execution context to rethrow exceptions. It must rethrow if the operator has caught an
   * exception and it does not have the @catch annotation. When running as an SPL application, this
   * configuration has an effect on the PE level. It indicates that other operators in the PE should
   * not catch the exception, even if they have the @catch annotation. Rethrowing exceptions enables
   * the PE to shutdown gracefully. This value can only go from false -> true.
   */
  void setMustRethrowException();

  /**
   * Checks the status of the execution context with respect to rethrowing exceptions
   *
   * @return true if exceptions must be rethrown, false otherwise
   */
  boolean mustRethrowException();

  /**
   * Enables non-blocking checkpointing. This method must be invoked from the operator constructor.
   * Non-blocking checkpointing enables an operator to serialize its state while new tuples are
   * processed. This should be used only when the operator implements techniques that guarantee that
   * the operator state serialized on the checkpoint() call is consistent with processing all tuples
   * prior to the drain() call even after tuple processing is resumed. One such technique is
   * user-level copy-on-write of operator state.
   */
  default void enableNonBlockingCheckpoint() {}

  /**
   * Make the region consistent by draining processing and checkpointing state. This method should
   * be invoked only by a start operator in a consistent region.
   *
   * @since InfoSphere&reg; Streams Version 4.2.0
   */
  default DrainResult makeConsistentNonBlocking() {
    if (makeConsistent()) return DrainResult.COMPLETED;
    else return DrainResult.FAILED;
  }

  /**
   * Submit a watermark to the specified output port. This has no effect if the operator does not
   * have an event-time context.
   *
   * @since IBM Streams Version 4.3
   */
  boolean submitWatermark(Timestamp wm, int port);

  /**
   * Submit a watermark to all output ports This has no effect if the operator does not have an
   * event-time context.
   *
   * @since IBM Streams Version 4.3
   */
  void submitWatermark(Timestamp wm);
}
