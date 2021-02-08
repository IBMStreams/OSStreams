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

/**
 * Input port windowing interfaces and classes. A window is a well-defined set of tuples that have
 * arrived at the input port. The definition of the contents of a window is defined by an
 * <em>eviction policy</em>. A window is described by {@link
 * com.ibm.streams.operator.window.StreamWindow}, see its documentation for definitions of tumbling
 * and sliding windows. <br>
 * A window's definition is declared either by the invocation of an operator in an SPL application,
 * or by the operator's {@link com.ibm.streams.flow.declare.InputPortDeclaration} window's
 * configuration (see {@link com.ibm.streams.flow.declare.InputPortDeclaration#tumbling()
 * tumbling()} or {@link com.ibm.streams.flow.declare.InputPortDeclaration#sliding() sliding()} in a
 * Java declared flow.
 *
 * <p>A window generates events as {@link com.ibm.streams.operator.window.StreamWindowEvent} which
 * an operator handles using an implementation of {@link
 * com.ibm.streams.operator.window.StreamWindowListener} and registering it with {@link
 * com.ibm.streams.operator.window.StreamWindow#registerListener(StreamWindowListener, boolean)}.
 * Each event has a specific {@link com.ibm.streams.operator.window.StreamWindowEvent.Type}, the
 * most significant event types are:
 *
 * <UL>
 *   <LI>{@link com.ibm.streams.operator.window.StreamWindowEvent.Type#INSERTION} One or more tuples
 *       have been inserted into the window.
 *   <LI>{@link com.ibm.streams.operator.window.StreamWindowEvent.Type#EVICTION}
 *       <UL>
 *         <LI>For a tumbling window the window has tumbled, all tuples are being evicted. This is
 *             typically where tuples are submitted, representing some aggregation of the window,
 *             the set of tuples being evicted.
 *         <LI>For a sliding window the window has slide, one or more tuples are being evicted, as
 *             defined by the <em>eviction policy</em>.
 *       </UL>
 *   <LI>{@link com.ibm.streams.operator.window.StreamWindowEvent.Type#TRIGGER} A sliding window is
 *       being triggered according to its <em> trigger policy</em>. This is typically where tuples
 *       are submitted, representing some aggregation of the window, the set of tuples currently in
 *       the window.
 * </UL>
 *
 * The operator's {@link com.ibm.streams.operator.window.StreamWindowListener} implements the logic
 * against the window, by {@link
 * com.ibm.streams.operator.window.StreamWindowListener#handleEvent(StreamWindowEvent) handling the
 * window's events}. Typically this is an algorithm that aggregates the window's contents, but there
 * are no restrictions on how the events are handled. <br>
 * Ideally the algorithm can be maintained incrementally as each tuple is inserted into the window,
 * so that latency is reduced when the window tumbles or is triggered, by not having to compute an
 * algorithm over the complete set of tuples in the window.
 *
 * <p>When a window is <em>partitioned</em>, the operator provides an implementation of {@link
 * com.ibm.streams.operator.window.StreamWindowPartitioner} that maps a {@link
 * com.ibm.streams.operator.Tuple Tuple} to a partition key, using {@link
 * com.ibm.streams.operator.window.StreamWindowPartitioner#getPartition(Object)
 * getPartition(tuple)}. <br>
 * Each unique partition key creates its own independent window partition with the window's policy
 * definitions. Thus the set of tuples in each window partition independently honors any
 * <em>eviction</em> and <em>trigger</em> policies. <br>
 * For example with a tumbling window with count eviction policy of size three, partitioned by a
 * customer identifier, will result in a unique window partition for each customer. Thus at some
 * point in time there might be two partitions, for customer identifiers {@code A345} and {@code
 * B823}. The partition for {@code A345} has a single tuple, and {@code B823} two tuples. A single
 * tuple for customer {@code B823} arriving at the input port will cause the partition for {@code
 * B823} to tumble, as it now contains three tuples. Partition {@code A345} tumbles independently of
 * any other partition and in this case would require two more tuples for {@code A345} to arrive at
 * the input port.
 *
 * <p>When an operator is checkpointed, either when a {@link
 * com.ibm.streams.operator.state.CheckpointContext checkpoint configuration} exists or the operator
 * is in a {@link com.ibm.streams.operator.state.ConsistentRegionContext consistent region}, the SPL
 * runtime persists the complete state of the window, including its contents and any state
 * information required to correctly restart event processing if the operator later resets to that
 * checkpoint. This window state is always persisted and does not require the operator to {@link
 * com.ibm.streams.operator.OperatorContext#registerStateHandler(com.ibm.streams.operator.state.StateHandler)
 * register} a {@link com.ibm.streams.operator.state.StateHandler}. <br>
 * Typically the operator will also maintain window state, such as the state of the aggregation
 * being performed, this state may be instance fields of the operator class or its {@link
 * com.ibm.streams.operator.window.StreamWindowListener}. This operator specific state is <b>not
 * persisted automatically</b> by the SPL runtime, instead the operator register a {@link
 * com.ibm.streams.operator.state.StateHandler} instance to persist such state on a checkpoint. <br>
 * Each window performs these actions on these state events:
 *
 * <UL>
 *   <LI>drain - Any outstanding {@link
 *       com.ibm.streams.operator.window.StreamWindowEvent.Type#EVICTION EVICTION} events are
 *       completed, waiting for each {@link
 *       com.ibm.streams.operator.window.StreamWindowListener#handleEvent(StreamWindowEvent)
 *       StreamWindowListener.handleEvent()} to return.
 *   <LI>checkpoint - Contents (tuples) of all window partitions are persisted along with any state
 *       required to correctly restart event processing.
 *   <LI>reset to initial state - All window contents are discarded without any event handling, and
 *       window is reset as though it had just been created. If the window is partitioned this means
 *       that all partitions are discarded and not recreated (until created by subsequent tuple
 *       processing)
 *   <LI>reset from a checkpoint - All window contents are discarded without any event handling. All
 *       partitions that exist in the checkpoint are recreated with their persisted contents and
 *       event handling is restarted to correctly honor the <em>eviction</em> and <em>trigger</em>
 *       policies. Any partitions that existed before the reset but not in the checkpoint are
 *       discarded and not recreated (until created by subsequent tuple processing).
 * </UL>
 *
 * Note that time based policies are based upon wall-clock time and are correctly honored when
 * processing is resumed after a reset from a checkpoint, that is time is not stopped or reset by
 * any reset operation. <br>
 * For example a sliding time based eviction policy of five seconds states that any tuple that has
 * been in the window for more than five seconds must be evicted. So a checkpoint can persist a
 * window containing 20 tuples that had four more seconds to live in the window <em>at the time the
 * checkpoint was taken</em>. However if a reset to that checkpoint occurs more than four seconds
 * later, then by the window's eviction policy all those tuples must be evicted, even though they
 * have just been restored. Thus the tuples are evicted, through an {@code EVICTION} event. <br>
 * Similarly, with a tumbling eviction time policy of 30 seconds (since the last tumble), then any
 * time while the operator was not active before the reset is included in the calculation, so that
 * the tumble takes place 30 seconds after the last tumble recorded in the checkpoint. If, when the
 * reset occurs the tumble is overdue, say in this case the PE was down for 45 seconds, then the
 * tumble takes place as soon as processing is resumed. <br>
 * If the operator registered a {@link com.ibm.streams.operator.state.StateHandler} then its methods
 * are invoked after the corresponding actions have occurred for all its windowed input ports. For
 * example, when {@link com.ibm.streams.operator.state.StateHandler#drain()} is called it is
 * guaranteed all windowed input ports have been drained. <br>
 * The only change in window contents that a {@code StateHandler} may perform is eviction of
 * partitions using {@link com.ibm.streams.operator.window.StreamWindow#evictPartition(Object)}
 * during its drain processing. In this case the window must be partitioned.
 */
package com.ibm.streams.operator.window;
