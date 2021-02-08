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

import com.ibm.streams.operator.OperatorContext;

/**
 * Abstract pattern class for an operator that produces tuples on output streams using a single
 * process method.
 *
 * @see #process()
 */
public abstract class ProcessTupleProducer extends TupleProducer {

  private Thread processThread;

  /**
   * {@inheritDoc} Create the non-daemon thread used to call the process() method. The thread is
   * created during initialization to ensure that the SPL runtime will wait upon its completion
   * before completing the operator. The thread is started by <code>startProcessing</code> which
   * means the initial delay support provided by the super-class <code>TupleProducer</code> is
   * honored.
   */
  @Override
  public synchronized void initialize(OperatorContext context) throws Exception {
    super.initialize(context);

    /*
     * Create the thread at initialize time but do not start it.
     * The thread will be started by startProcessing() which will
     * be called at allPortsReady() time if no delay is specified,
     * otherwise it will be called once the delay has expired.
     */
    processThread =
        getOperatorContext()
            .getThreadFactory()
            .newThread(
                new Runnable() {

                  @Override
                  public void run() {
                    try {
                      process();
                    } catch (Exception e) {
                      throw new RuntimeException(e);
                    }
                  }
                });

    /*
     * Set the thread not to be a daemon to ensure that the SPL runtime
     * will wait for the thread to complete before determining the
     * operator is complete.
     */
    processThread.setDaemon(false);
  }

  /**
   * Disallow use of deprecated method by a sub-class.
   *
   * @deprecated
   */
  @Override
  protected final void producerInitialize() {}

  /** Starts processing by starting the thread that will call process(). */
  protected synchronized void startProcessing() {
    processThread.start();
  }

  /**
   * Sub-classes provide a process() method that submits tuples on the operator's output ports. <br>
   * When this method returns the the operator will complete.
   *
   * <p>It is guaranteed that the <code>initialize()</code> method <i>happens-before</i> the <code>
   * process()</code> method for purposes of memory visibility.
   *
   * @throws Exception Exception is sub-class processing
   */
  protected abstract void process() throws Exception;

  /**
   * Shutdown this operator, which will interrupt the thread executing the <code>process()</code>
   * method.
   *
   * <p>Sub-classes <b>must</b> call <code>super.shutdown()</code> if they override this method.
   */
  @Override
  public synchronized void shutdown() throws Exception {
    if (processThread != null) {
      processThread.interrupt();
      processThread = null;
    }

    super.shutdown();
  }
}
