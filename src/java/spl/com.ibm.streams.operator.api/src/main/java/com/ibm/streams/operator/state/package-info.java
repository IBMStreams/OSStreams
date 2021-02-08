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
 * State management for operators. Operators generally fall into three categories for state:
 *
 * <UL>
 *   <LI>Stateless - Operator maintains no state related to stream processing, typically an input
 *       tuple is processed completely within the {@link
 *       com.ibm.streams.operator.Operator#process(com.ibm.streams.operator.StreamingInput,
 *       com.ibm.streams.operator.Tuple) process} method resulting in zero or more tuples being
 *       submitted, for example a filtering operator.
 *   <LI>Buffering - Operator processes a tuple partially within the {@link
 *       com.ibm.streams.operator.Operator#process(com.ibm.streams.operator.StreamingInput,
 *       com.ibm.streams.operator.Tuple) process} method, leaving outstanding work once the process
 *       method returns. For example a delay operator creates a list of tuples to be submitted
 *       later.
 *   <LI>Stateful - The operator maintains state related to tuple processing that persists across
 *       multiple calls to the {@link
 *       com.ibm.streams.operator.Operator#process(com.ibm.streams.operator.StreamingInput,
 *       com.ibm.streams.operator.Tuple) process} method. For example, windowing operators. Some
 *       stateful operators may also be buffering.
 * </UL>
 *
 * Buffering and stateful operators utilize {@link com.ibm.streams.operator.state.StateHandler} to
 * manage state and its coordination within an SPL application. For example:
 *
 * <UL>
 *   <LI>a buffering operator provides a {@link com.ibm.streams.operator.state.StateHandler}
 *       implementation to drain its outstanding processing in a {@link
 *       com.ibm.streams.operator.state.ConsistentRegionContext consistent region}.
 *   <LI>a stateful operator provides a {@link com.ibm.streams.operator.state.StateHandler}
 *       implementation to persist its state for recovery after failure when in a {@link
 *       com.ibm.streams.operator.state.ConsistentRegionContext consistent region} or {@link
 *       com.ibm.streams.operator.state.CheckpointContext checkpointing}.
 * </UL>
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
package com.ibm.streams.operator.state;
