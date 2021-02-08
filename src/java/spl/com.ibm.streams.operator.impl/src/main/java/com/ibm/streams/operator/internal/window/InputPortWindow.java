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

package com.ibm.streams.operator.internal.window;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.state.StateHandler;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindowListener;
import com.ibm.streams.operator.window.StreamWindowPartitioner;
import java.util.Collection;
import java.util.List;

/**
 * Meta data for the window declared on an input port. Implements StreamWindow as the object exposed
 * to the user's operator that describes the window.
 *
 * <p>Works with a WindowProcessor.
 *
 * @param <T> Tuple type
 * @param <E> Data for Eviction policy type.
 */
public class InputPortWindow<T, E, R> implements StreamWindow<T> {

  private final OperatorContext context;
  private final StreamingInput<T> port;
  private final StreamWindow.Type type;
  private final boolean isPartitioned;
  private final StreamWindow.Policy evictionPolicy;
  private final E evictionPolicyConfig;
  private final StreamWindow.Policy triggerPolicy;
  private final R triggerPolicyConfig;

  private final OrderedPartitionHandler.PartitionEvictionPolicy partitionEvictionPolicy;
  private final long partitionEvictionConfiguration;

  /** My processor, where my listener will be registered. */
  private WindowHandler<T> handler;

  /** StreamWindow instance for a non-windowed port. */
  InputPortWindow(OperatorContext context, StreamingInput<T> port) {
    this(
        context,
        port,
        StreamWindow.Type.NOT_WINDOWED,
        false,
        StreamWindow.Policy.NONE,
        null,
        StreamWindow.Policy.NONE,
        null,
        null,
        -1);
  }
  /** StreamWindow instance for a tumbling windowed port. */
  InputPortWindow(
      OperatorContext context,
      StreamingInput<T> port,
      boolean isPartitioned,
      StreamWindow.Policy evictionPolicy,
      E evictionPolicyConfig,
      List<String> info,
      int infoOffset) {
    this(
        context,
        port,
        StreamWindow.Type.TUMBLING,
        isPartitioned,
        evictionPolicy,
        evictionPolicyConfig,
        StreamWindow.Policy.NONE,
        null,
        info,
        infoOffset);
  }

  InputPortWindow(
      OperatorContext context,
      StreamingInput<T> port,
      StreamWindow.Type type,
      boolean isPartitioned,
      StreamWindow.Policy evictionPolicy,
      E evictionPolicyConfig,
      StreamWindow.Policy triggerPolicy,
      R triggerPolicyConfig,
      List<String> info,
      int infoOffset) {
    this.context = context;
    this.port = port;
    this.type = type;
    this.isPartitioned = isPartitioned;
    this.evictionPolicy = evictionPolicy;
    this.evictionPolicyConfig = evictionPolicyConfig;
    this.triggerPolicy = triggerPolicy;
    this.triggerPolicyConfig = triggerPolicyConfig;

    // Remaining window information starts at infoOffset
    // element of info if it is available.
    if (info != null && infoOffset < info.size()) {

      // First optional additional elements are
      // 0: partition eviction type
      // 1: partition eviction configuration (if not NONE)

      partitionEvictionPolicy =
          OrderedPartitionHandler.PartitionEvictionPolicy.valueOf(info.get(infoOffset++));
      switch (partitionEvictionPolicy) {
        default:
        case none:
          partitionEvictionConfiguration = -1L;
          break;
        case partitionAge:
          // Work in milliseconds
          partitionEvictionConfiguration =
              WindowHandler.timeSecondsToMillis(info.get(infoOffset++));
          break;
        case partitionCount:
        case tupleCount:
          partitionEvictionConfiguration = Long.valueOf(info.get(infoOffset++));
          break;
      }

      if (partitionEvictionPolicy != OrderedPartitionHandler.PartitionEvictionPolicy.none)
        checkPartitioned();

    } else {
      partitionEvictionPolicy = OrderedPartitionHandler.PartitionEvictionPolicy.none;
      partitionEvictionConfiguration = -1l;
    }
  }

  @Override
  public StreamWindow.Policy getEvictionPolicy() {
    return evictionPolicy;
  }

  @Override
  public boolean isPartitioned() {
    return isPartitioned;
  }

  @Override
  public StreamWindow.Policy getTriggerPolicy() {
    return triggerPolicy;
  }

  @Override
  public StreamWindow.Type getType() {
    return type;
  }

  @Override
  public void registerListener(StreamWindowListener<T> listener, boolean useAsynchronousTasks) {

    getHandler().registerListener(listener);
    if (listener instanceof StateHandler)
      getOperatorContext().registerStateHandler((StateHandler) listener);
  }

  @Override
  public void registerPartitioner(StreamWindowPartitioner<T, ?> partitioner) {

    checkPartitioned();

    getHandler().registerPartitioner(partitioner);
  }

  @Override
  public StreamingInput<T> getInputPort() {
    return port;
  }

  synchronized void setHandler(WindowHandler<T> handler) {
    this.handler = handler;
  }

  /** @return the handler */
  private synchronized WindowHandler<T> getHandler() {
    return handler;
  }
  /**
   * Return the eviction policy configuration.
   *
   * @return the evictionPolicyData
   */
  public E getEvictionPolicyConfig() {
    return evictionPolicyConfig;
  }

  public R getTriggerPolicyConfig() {
    return triggerPolicyConfig;
  }

  /**
   * Evict a partition from a partitioned window.
   *
   * @throws Exception
   */
  @Override
  public void evictPartition(Object partition) throws Exception {
    checkPartitioned();

    getHandler().evictPartition(partition);
  }

  @Override
  public Collection<?> getPartitions() {
    checkPartitioned();

    return ((PartitionedHandler<T>) getHandler()).getPartitions();
  }

  @Override
  public boolean needsPartitionEviction() {
    checkPartitioned();
    return ((PartitionedHandler<T>) getHandler()).needsPartitionEviction();
  }

  @Override
  public final OperatorContext getOperatorContext() {
    return context;
  }

  /** Throw an IllegalStateException if the window is not partitioned. */
  private void checkPartitioned() {
    if (!isPartitioned()) throw new IllegalStateException("isPartitioned()==false");
  }
  /**
   * Type of partition eviction.
   *
   * @return the partitionEvictionPolicy
   */
  OrderedPartitionHandler.PartitionEvictionPolicy getPartitionEvictionPolicy() {
    return partitionEvictionPolicy;
  }
  /**
   * Configuration of partition eviction.
   *
   * @return the partitionEvictionConfiguration
   */
  long getPartitionEvictionConfiguration() {
    return partitionEvictionConfiguration;
  }
}
