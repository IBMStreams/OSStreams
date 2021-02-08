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

import java.util.EventObject;

/**
 * An event for a windowed input port. <br>
 * The source of the event ({@code getSource()}) is the {@link StreamWindow} for the window.
 *
 * @param <T> Tuple type.
 */
public class StreamWindowEvent<T> extends EventObject {
  /** */
  private static final long serialVersionUID = 5401674568489909870L;

  /** Stream window event type. */
  public static enum Type {

    /**
     * Insertion of tuples into a window. Listeners typically perform incremental state maintenance
     * based upon the incoming tuples attribute values.
     */
    INSERTION,

    /**
     * Eviction of tuples from the window. Tuples are evicted from the window according to the
     * eviction policy of the window.
     *
     * <p>For a tumbling window all tuples are evicted as follows:
     *
     * <UL>
     *   <LI>For a count-based eviction policy all tuples are evicted after the insertion of the
     *       N'th tuple where N is the size of the window. The {@code INSERTION} event precedes the
     *       {@code EVICTION} event.
     *   <LI>For a time-based eviction policy all tuple are evicted every T seconds independent of
     *       tuple arrival at the port, where T is the time period. The first eviction will take
     *       place T seconds after the SPL runtime has notified the operator of port readiness, that
     *       is T seconds after the call to Operator.allPortsReady() is made. Note the timer will
     *       start at the time allPortsReady() is called, not at the point the Operator's
     *       implementation returns control to the SPL runtime.
     *   <LI>For an attribute delta based policy, when a new tuple is received, if the delta between
     *       its eviction policy attribute value and that of the oldest tuple in the window is
     *       larger than the eviction policy size, then all tuples are evicted. The {@code EVICTION}
     *       event occurs before the {@code INSERTION} event.
     *   <LI>For a punctuation-based eviction policy all tuples are evicted when a {@link
     *       com.ibm.streams.operator.StreamingData.Punctuation#WINDOW_MARKER WINDOW_MARKER}
     *       punctuation mark arrives.
     * </UL>
     *
     * <br>
     * Note that for a tumbling window an {@code EVICTION} event will occur even if the window
     * logically contained zero tuples or if the window contained tuples but no Tuple references
     * were maintained due to their removal during event processing.
     *
     * <p>For a sliding window tuples are evicted as follows:
     *
     * <UL>
     *   <LI>For a count-based eviction policy, once the window contains N logical tuples the
     *       arrival of a tuple on the window's input port will result in the eviction of the oldest
     *       logical tuple in the window. The {@code EVICTION} event precedes the {@code INSERTION}
     *       event.
     *   <LI>For a time-based eviction policy, tuples that have been in the window more than T
     *       seconds are evicted. Eviction is independent of insertion.
     *   <LI>For an attribute delta based policy, the size defines the boundary of the window, as it
     *       slides, in the units of the eviction policy attribute. When a new tuple is received,
     *       existing tuples in the window for which the delta between the eviction policy attribute
     *       value of the new tuple and that of the existing tuple is greater than the eviction
     *       policy size, are evicted. The {@code EVICTION} event precedes the {@code INSERTION}
     *       event.
     * </UL>
     *
     * <br>
     * For a sliding window an {@code EVICTION} event that would contain an empty Iterable is not
     * made. This reduces overhead of empty calls for window handers that do not need to be notified
     * of evictions, or where the state of the window did not change.
     */
    EVICTION,

    /**
     * Trigger of a sliding window. Trigger events occur according to the trigger policy of the
     * sliding window.
     *
     * <UL>
     *   <LI>For a count-based trigger policy of size N the trigger fires after N tuples have been
     *       logically inserted since the last trigger was fired, or since the window was created
     *       for the first trigger firing. The {@code INSERTION} event precedes the {@code TRIGGER}
     *       event.
     *   <LI>For a time-based trigger policy the trigger fires every T seconds independent of tuple
     *       arrival at the port, where T is the time period of the policy. The first trigger will
     *       take place T seconds after the SPL runtime has notified the operator of port readiness,
     *       that is T seconds after the call to Operator.allPortsReady() is made. Note the timer
     *       will start at the time allPortsReady() is called, not at the point the Operator's
     *       implementation returns control to the SPL runtime.
     *   <LI>For an attribute delta based trigger policy, the size defines the boundary of the
     *       sequence of tuples since the last trigger, that trigger the window again. When a new
     *       tuple is received, if the value of its trigger attribute is larger than that of the
     *       last tuple which triggered the window plus the trigger policy size, the window is
     *       triggered again. The {@code TRIGGER} event occurs before any {@code EVICTION} event
     *       when the window has a count or delta based eviction policy, and thus also precedes the
     *       {@code INSERTION} event.
     * </UL>
     */
    TRIGGER,

    /**
     * Event that signifies a sliding window has reached its initial window full event. The initial
     * window full event only occurs for a sliding window, it allows window handlers to delay any
     * actions until the window has been full at least once. The initial window full event timing
     * depends upon the window's eviction policy.
     *
     * <UL>
     *   <LI>For a count-based eviction policy of size N the window is initially full after the
     *       insertion of the N'th tuple. The {@code INSERTION} event precedes the {@code
     *       INITIAL_FULL} event, and the {@code INITIAL_FULL} precedes any count-based policy
     *       {@code TRIGGER} event due to the {@code INSERTION}. When the window is partitioned each
     *       partition will receive a {@code INITIAL_FULL} event once it has had N tuples inserted
     *       into it.
     *   <LI>For a time-based eviction policy the {@code INITIAL_FULL} event will take place once
     *       SPL runtime has notified the operator of port readiness, that is the window starts out
     *       as full. When the window is partitioned, partitions created after the all ports ready
     *       notification will receive a {@code INITIAL_FULL} event when they are created. The
     *       {@code INITIAL_FULL} event precedes the the {@code INSERTION} event that caused the
     *       partition to be created.
     *   <LI>For an attribute delta-based eviction policy, the window is initially full when delta
     *       between the first tuple in the window and the tuple being inserted exceeds the policy
     *       size for the defined attribute. The {@code INITIAL_FULL} event precedes the {@code
     *       EVICTION} and {@code INSERTION} events that triggered the initial full state. When the
     *       window is partitioned each partition will receive a {@code INITIAL_FULL} event
     *       independently driven by deltas between the first tuple in each partition and arriving
     *       tuples.
     * </UL>
     *
     * <p>If a partition is evicted (@link StreamWindow#evictPartition(Object)} then subsequent
     * insertion of tuples with the same partition value act as a new partition, leading to another
     * {@code INITIAL_FULL} event according to the eviction policy.
     */
    INITIAL_FULL,

    /**
     * Final punctuation event. The final punctuation occurs when the {@link
     * com.ibm.streams.operator.StreamingData.Punctuation#FINAL_MARKER FINAL_MARKER} is received by
     * the input port, signifying that no more tuples or punctuation marks will arrive. Handling of
     * the event will typically perform final actions based upon the remaining tuples in the window.
     * <br>
     * Note that a {@code FINAL} event will be generated for each active window partition when the
     * window is partitioned.
     */
    FINAL,

    /**
     * Eviction of a partition and its tuples from a partitioned window.
     *
     * <p>When a partition eviction is defined by the SPL invocation of the operator, partitions
     * will be evicted according to the partition eviction policy. Partitions are always evicted
     * oldest first, with the order defined by {@link StreamWindow#getPartitions()}. An empty
     * partition is one that has no tuples according to its window eviction policy, e.g. a {@code
     * TIME} policy {@code TUMBLING} window that has an {@code EVICTION} event, but no subsequent
     * {@code INSERTION} event.
     *
     * <UL>
     *   <LI>{@code partitionCount(N)} If after an {@code INSERTION} event the number of partitions
     *       is greater than {@code N} then the oldest empty partitions are evicted up to the oldest
     *       non-empty partition. The oldest non-empty partition will then be evicted if the
     *       partition count is greater than {@code N}.
     *   <LI>{@code tupleCount(N)} If after an {@code INSERTION} event the total number of tuples in
     *       all partitions is greater than {@code N} then the oldest partitions (empty and
     *       non-empty) are evicted until the tuple count is less than or equal to {@code N}.
     *   <LI>{@code partitionAge(T)} Any partitions that have not had a tuple inserted into them in
     *       the last {@code T} seconds are evicted. Partition eviction is independent of tuple
     *       insertion.
     * </UL>
     *
     * For {@code partitionCount(N)} and {@code tupleCount(N)} if at least one partition is evicted
     * then additionally all empty old partitions up to (but not including) the first non-empty
     * partition partition are evicted. A partition is empty if it logically has no tuples in it
     * according to its window configuration. An implementation of {@code StreamWindowListener} may
     * call the {@link StreamWindow#needsPartitionEviction()} to determine if partition eviction
     * after an {@code INSERTION} event will occur. If the listener evicts sufficient partitions
     * (until {@code needsPartitionEviction()} returns false) during handling of the {@code
     * INSERTION} event, then no additional partitions will be evicted automatically.
     *
     * @since InfoSphere&reg; Streams Version 2.0.0.3
     */
    PARTITION_EVICTION;
  }

  private final Object partition;

  private final Iterable<T> tuples;

  private final StreamWindowEvent.Type type;

  public StreamWindowEvent(
      StreamWindowEvent.Type type, StreamWindow<T> window, Object partition, Iterable<T> tuples) {
    super(window);
    this.partition = partition;
    this.type = type;
    this.tuples = tuples;
  }

  /**
   * The partition the event is for. When a window is partitioned the returned value will be the
   * result of executing the {@link StreamWindow#registerPartitioner(StreamWindowPartitioner)
   * registered} {@link StreamWindowPartitioner} against a tuple. <br>
   * When a window is not partitioned all events will return a {@code java.lang.Integer} with value
   * zero.
   *
   * @return the partition
   */
  public final Object getPartition() {
    return partition;
  }

  /**
   * The type of the event.
   *
   * @return Type of this event
   */
  public final StreamWindowEvent.Type getType() {
    return type;
  }

  /**
   * A sequence of tuples in arrival order, from oldest to most recent.
   *
   * <p>For an {@code INSERTION} event the sequence represents the tuples being inserted into a
   * window. Multiple tuple arrivals may be batched into a single {@code INSERTION} event, thus a
   * listener should never assume the event only contains a single tuple. <br>
   * For an {@code EVICTION} event the sequence represents the sub-set of tuples being evicted from
   * a window that have a reference to their tuple instance. Multiple tuple evictions may be batched
   * into a single EVICITON event, thus a listener should never assume the event only contains a
   * single tuple. <br>
   * For all other event types the collection represents sub-set of logical tuples currently in the
   * window that have a reference to their tuple instance.
   *
   * <p>Executing <code>Iterator.remove()</code> for a tuple will result in the tuple reference
   * being removed from the window to allow garbage collection of the tuple object (by not having
   * the <code>StreamWindow</code> implementation hold its reference). The tuple logically remains
   * in the window but its attribute values are no longer available through instances of <code>
   * StreamWindowEvent</code>.
   */
  public final Iterable<T> getTuples() {
    return tuples;
  }
}
