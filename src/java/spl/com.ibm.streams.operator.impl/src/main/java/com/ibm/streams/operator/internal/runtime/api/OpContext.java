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

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.internal.runtime.OperatorScheduledExecutor;
import com.ibm.streams.operator.internal.runtime.OperatorThreadFactory;
import java.io.File;
import java.util.List;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;

abstract class OpContext implements OperatorContext {

  private final OpInvocationInfo invokeInfo;
  private final Adapter adapter;

  /** My scheduler which is private to this operator. */
  private OperatorScheduledExecutor scheduler;

  /**
   * My thread factory which is private to this operator. This is for Threads created by the
   * Operator code, the scheduler uses a different instance of OperatorThreadFactory so that when
   * the operator is shutdown we don't interrupt any scheduler work (those will be notified by the
   * scheduler.shutdown() method).
   */
  private OperatorThreadFactory userThreadFactory;

  OpContext(OpInvocationInfo invokeInfo, Adapter adapter) {
    this.invokeInfo = invokeInfo;
    this.adapter = adapter;
  }

  @Override
  public final String getName() {
    return invokeInfo.getName();
  }

  /** Create the scheduler on-demand, especially for C++ operators. */
  @Override
  public final synchronized ScheduledExecutorService getScheduledExecutorService() {
    if (scheduler != null) return scheduler;

    int poolingThreadCount = Math.max(Math.min(8, Runtime.getRuntime().availableProcessors()), 2);
    scheduler =
        new OperatorScheduledExecutor(
            poolingThreadCount, new OperatorThreadFactory(adapter, "-scheduler"));

    scheduler.setKeepAliveTime(5, TimeUnit.SECONDS);
    scheduler.allowCoreThreadTimeOut(true);

    // If we shutdown the scheduler then shutdown any tasks.
    scheduler.setContinueExistingPeriodicTasksAfterShutdownPolicy(false);
    scheduler.setExecuteExistingDelayedTasksAfterShutdownPolicy(false);

    return scheduler;
  }

  /** Create the thread factory on-demand, especially for C++ operators. */
  @Override
  public final synchronized ThreadFactory getThreadFactory() {
    if (userThreadFactory != null) return userThreadFactory;
    return userThreadFactory = new OperatorThreadFactory(adapter, "");
  }

  @Override
  public final String getKind() {
    return invokeInfo.getKind();
  }

  @Override
  public final int getChannel() {
    return invokeInfo.getChannel();
  }

  @Override
  public final int getLocalChannel() {
    return invokeInfo.getLocalChannel();
  }

  @Override
  public final List<Integer> getAllChannels() {
    return invokeInfo.getAllChannels();
  }

  @Override
  public final int getMaxChannels() {
    return invokeInfo.getMaxChannels();
  }

  @Override
  public final int getLocalMaxChannels() {
    return invokeInfo.getLocalMaxChannels();
  }

  @Override
  public final List<Integer> getAllMaxChannels() {
    return invokeInfo.getAllMaxChannels();
  }

  @Override
  public final String getLogicalName() {
    return invokeInfo.getLogicalName();
  }

  @Override
  public final ProcessingElement getPE() {
    return adapter.getExecutionContext().getPE();
  }

  @Override
  public final File getToolkitDirectory() {
    return invokeInfo.getToolkitDirectory();
  }
}
