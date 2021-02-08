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

package com.ibm.streams.operator.window;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingInput;
import java.util.Collection;

/**
 * A window connected to an input port (stream). A window's type is either tumbling or sliding. <br>
 * A tumbling window has an eviction policy that determines when all tuples are evicted and the
 * window tumbles. <br>
 * A sliding window has an eviction policy that defines how tuples are evicted (the slide) and a
 * trigger policy that determines when a trigger event is generated.
 *
 * <p>An operator registers a listener against a window to handle the defined window events such as
 * insertion and eviction.
 *
 * <p>The StreamWindow maintains the logical collection of ordered tuples that are contained in the
 * window, ordered by arrival time, from oldest to most recent. The logical collection by default
 * includes the reference to the tuple object that contains the tuple's attribute values, its data.
 * The window event listener may remove the tuple reference from the window while handling an event,
 * if it no longer requires the tuple's data to maintain its state. This allows reduction of memory
 * usage by a window, by potentially allowing the tuple to be garbage collected. <br>
 * While the tuple reference will be removed from the window, logically the tuple remains in the
 * window, though its attribute values are no longer available. This is to ensure correct eviction
 * processing, for example removing a tuple reference does not reduce the number of logical tuples
 * in the window ensuring eviction for a count based policy is honored correctly.
 *
 * <p>The StreamWindow manages ordering of StreamWindowEvent processing so that only a single event
 * is being processed at any time and events are always processed by the StreamEventListener in the
 * correct order.
 *
 * @param T Input port tuple type.
 * @see StreamWindowEvent.Type
 * @see StreamWindowListener
 */
public interface StreamWindow<T> {

  /** Policy type for eviction and trigger policies. */
  public enum Policy {
    // Implementation note:
    // Policy ordinal need to match the values in SPL's Window.pm

    /** No policy exists for this port. */
    NONE,

    /** The policy is a time period. */
    TIME,

    /** The policy is a count of tuples. */
    COUNT,

    /** The policy is windows boundaries are indicated by a punctuation mark. */
    PUNCTUATION,

    /** The policy is a delta between an attribute's value in tuples. */
    DELTA,
    ;
  }

  /** Window type. */
  public enum Type {
    // Implementation note:
    // TUMBLING and SLIDING ordinal need to match the values in
    // SPL's Window.pm (1 and 2)
    /** This port is not windowed. */
    NOT_WINDOWED,

    /** This port has a tumbling window. */
    TUMBLING,

    /** This port has a sliding window. */
    SLIDING
  }

  /**
   * Return the type of the window defined against the port.
   *
   * @return NONE, TUMBLING or SLIDING.
   */
  public Type getType();

  /**
   * Get the StreamingInput reference for the input port this window is for.
   *
   * @return The input port for the window.
   */
  public StreamingInput<T> getInputPort();

  /**
   * Get the eviction policy for the window.
   *
   * @return The eviction policy or NONE if the port is not windowed.
   */
  public Policy getEvictionPolicy();

  /**
   * Get the trigger policy for a sliding window.
   *
   * @return The trigger policy or NONE for a tumbling window.
   */
  public Policy getTriggerPolicy();

  /**
   * Is this window partitioned. The window definition only indicates if the window should be
   * partitioned, the operator implementation defines the partitions and the tuple to partitioning
   * mapping.
   *
   * @return True if the window is partitioned, false otherwise..
   */
  public boolean isPartitioned();

  /**
   * Register a listener to handle window events. The listener replaces the previous listener as
   * only a single listener is supported. If useAsynchronousTasks is set to true then threads that
   * generated the event (such as the thread handling the arrival of a tuple at a port) may not wait
   * for the completion of the event's handling. Use of asynchronous does not change the guaranteed
   * ordering of event handling.
   *
   * <p>The listener must be registered during {@link
   * com.ibm.streams.operator.Operator#initialize(com.ibm.streams.operator.OperatorContext)
   * Operator.initialize()} to guarantee that all tuples and punctuation arriving on the port are
   * handled by the listener.
   *
   * <p>If {@code listener} also implements {@link com.ibm.streams.operator.state.StateHandler} then
   * it is also {@link
   * com.ibm.streams.operator.OperatorContext#registerStateHandler(com.ibm.streams.operator.state.StateHandler)
   * registered as an operator's state handler}. <br>
   * Operators must ensure that instances of {@code StateHandler} and {@code StreamWindowListener}
   * are registered in a consistent order to ensure the order of information written to a checkpoint
   * and read from a checkpoint upon reset is consistent.
   *
   * @param listener Listener to handle events.
   * @param useAsynchronousTasks Where possible execute the event handling as an asynchronous task.
   */
  public void registerListener(StreamWindowListener<T> listener, boolean useAsynchronousTasks);

  /**
   * Register a StreamWindowPartitioner for the window. The getPartition() method of partitioner
   * will be called for each tuple inserted into the window. The returned partition will set as the
   * {@link StreamWindowEvent#getPartition() partition} value for the corresponding {@link
   * StreamWindowEvent} objects related to the tuple.
   *
   * @param partitioner Window partitioner logic.
   */
  public void registerPartitioner(StreamWindowPartitioner<T, ?> partitioner);

  /**
   * Evict a partition and its tuples from the window. Eviction of a partition results in a window
   * event of type {@link StreamWindowEvent.Type#PARTITION_EVICTION}.
   *
   * @param partition The partition to be evicted. If the partition does not exist in the window
   *     then no action is taken.
   * @exception IllegalStateException Window is not partitioned.
   * @exception Exception Exception thrown handling the {@code PARTITION_EVICTION} event.
   * @since InfoSphere&reg; Streams Version 2.0.0.3
   */
  public void evictPartition(Object partition) throws Exception;

  /**
   * Get a read-only copy of the partition keys in the window. The partition keys are the values
   * returned by {@link StreamWindowPartitioner#getPartition(Object)}.
   *
   * <p>When the window has no partition eviction specified in SPL the ordering of iteration of the
   * elements is undefined.
   *
   * <p>When partition eviction is defined, iterating over the {@code Collection} is ordered by the
   * time of the last tuple insertion into the partition, with the partition with the oldest
   * insertion first, and the partition with the most recent insertion last.
   *
   * @exception IllegalStateException - Window is not partitioned.
   * @since InfoSphere&reg; Streams Version 2.0.0.3
   */
  public Collection<?> getPartitions();

  /**
   * Determine if partition eviction is required according to policy. Returns true when the window
   * is partitioned, has a partition eviction policy of {@code partitionCount()} or {@code
   * tupleCount()} and partitions need to be evicted to honor the policy.
   *
   * @return True if the number of partitions or tuples exceed the eviction policy. False if the
   *     partition eviction policy is age based or no count based eviction is required.
   * @exception IllegalStateException - Window is not partitioned.
   * @see StreamWindowEvent.Type#PARTITION_EVICTION
   * @since InfoSphere&reg; Streams Version 2.0.0.3
   */
  public boolean needsPartitionEviction();

  /**
   * Return the {@link OperatorContext} for this window's operator invocation.
   *
   * @return Context for this window's operator invocation.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public OperatorContext getOperatorContext();
}
