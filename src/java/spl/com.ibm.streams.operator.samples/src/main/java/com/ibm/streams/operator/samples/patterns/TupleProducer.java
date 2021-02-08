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

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.Parameter;
import java.util.concurrent.Callable;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

/**
 * Abstract pattern class for an operator that produces tuples (a <em>source operator</em>) and
 * typically does not support input ports. This class provides support for an initial delay,
 * allowing submission of tuples to occur a period of time after {@link #allPortsReady()} is called
 * by the SPL runtime. This class calls the abstract method {@link #startProcessing()} to indicate
 * to the sub-class that it can begin to submit tuples.
 *
 * <p>If the operator parameter {@link #setInitDelay(long) initDelay} is set then it is taken as the
 * number of seconds before {@link #startProcessing()} is called. <br>
 * The initial delay can also be set using the methods on this class to allow delays to be specified
 * in different units.
 *
 * <p>While source operators typically do not have input ports, sub-classes may support input ports
 * and implement {@link #process(com.ibm.streams.operator.StreamingInput,
 * com.ibm.streams.operator.Tuple) tuple processing}. Typically these input ports are {@link
 * InputPortSet#controlPort() control ports} that allow updates how the operator fetches its tuples
 * as changing a database connection string.
 */
public abstract class TupleProducer extends AbstractOperator {

  private long initialDelay = 0;
  private TimeUnit initialDelayUnit = TimeUnit.SECONDS;

  /** ScheduledFuture of the call to startProcessing() when the initial delay is non-zero. */
  private ScheduledFuture<?> startProcessingFuture;

  /** {@inheritDoc} */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);

    producerInitialize();
  }

  /**
   * Set the initial delay from the operator parameter {@code initDelay} in seconds.
   *
   * @param delaySeconds Delay in seconds before starting processing.
   */
  @Parameter(
      optional = true,
      description = "Delay in seconds before the operator starts producing tuples.")
  public void setInitDelay(long delaySeconds) {
    setInitialDelay(delaySeconds, TimeUnit.SECONDS);
  }

  /**
   * Set the initial delay in seconds. This method may be used by sub-classes to provide an {@code
   * initDelay} parameter of type {@code float64}. This is achieved by hiding {@link
   * #setInitDelay(long)} in the sub-class and create a public annotated version of {@link
   * #setInitDelay(double)}.
   *
   * <pre>
   * <code>
   * // hide the version annotated with {@literal @}Parameter
   * public void setInitDelay(long delay) { super.setInitDelay(delay); }
   *
   * // Annotated version to create an initDelay parameter of type float64.
   * {@literal @}Parameter(optional=true,
   * description="Delay in seconds before the operator starts producing tuples.")
   * public void setInitDelay(double delay) { super.setInitDelay(delay); }
   * </code>
   * </pre>
   *
   * @param delaySeconds Delay in seconds before starting processing. This is converted to
   *     milliseconds resulting in a call to {@link #setInitialDelay(long, TimeUnit)
   *     setInitialDelay(delaySeconds * 1000.0, TimeUnit.MILLISECONDS)}.
   */
  protected void setInitDelay(double delaySeconds) {
    long delayms = (long) (delaySeconds * 1000.0);
    this.setInitialDelay(delayms, TimeUnit.MILLISECONDS);
  }

  /**
   * {@inheritDoc}
   *
   * <p>Calls {@link #startProcessing()} method immediately if initial delay is zero, otherwise
   * asynchronously calls {@code startProcessing()} after the required initial delay.
   */
  @Override
  public synchronized void allPortsReady() throws Exception {

    if (getInitialDelay() == 0) {
      startProcessing();
      return;
    }

    // Kick off asynchronously a call to startProcessing() after the initial
    // delay has expired.
    startProcessingFuture =
        getOperatorContext()
            .getScheduledExecutorService()
            .schedule(
                new Callable<Object>() {

                  public Object call() throws Exception {
                    startProcessing();
                    return null;
                  }
                },
                getInitialDelay(),
                getInitialDelayUnit());
  }

  /**
   * Initialize this operator. Called during <code>initialize()</code>.
   *
   * @deprecated Sub-classes should instead override {@link #initialize(OperatorContext)} to be
   *     consistent with {@link AbstractOperator}.
   * @see AbstractOperator#initialize(OperatorContext)
   */
  protected void producerInitialize() throws Exception {}

  /**
   * Cancel the startProcessing() task if it was asynchronously kicked off due to an initial delay
   * and has not yet started.
   *
   * <p>Sub-classes <b>must</b> call <code>super.shutdown()</code> if they override this method.
   */
  public synchronized void shutdown() throws Exception {

    if (startProcessingFuture != null) {
      if (!startProcessingFuture.isDone()) startProcessingFuture.cancel(true);
    }
    super.shutdown();
  }

  /**
   * Get the initial delay. The default value is 0 but is overwritten by the value (in seconds) of
   * the initDelay parameter. The initDelay parameter is read prior to {@link
   * #initialize(OperatorContext)}.
   *
   * @return Value of the initial delay.
   */
  public final synchronized long getInitialDelay() {
    return initialDelay;
  }

  /**
   * Get the unit of the initial delay. Defaults to SECONDS.
   *
   * @return Unit for the initial delay.
   */
  public final synchronized TimeUnit getInitialDelayUnit() {
    return initialDelayUnit;
  }

  /**
   * Set the initial delay for processing tuples from the external source. Sub-classes may call this
   * from {@link #initialize(OperatorContext)} to override the default.
   *
   * @param initialDelay Value for the initial delay
   * @param unit Unit for {@code initialDelay}.
   */
  public final synchronized void setInitialDelay(long initialDelay, TimeUnit unit) {
    this.initialDelay = initialDelay;
    this.initialDelayUnit = unit;
  }

  /**
   * Called to indicate that the operator can start submitting tuples. It will be called after any
   * configured initial delay. This method is required to return immediately to the SPL runtime, by
   * starting the processing to fetch tuples asynchronously, this may be achieved using the
   * OperatorContext's ScheduledExecutorService or starting a thread created using the
   * OperatorContext's ThreadFactory during producerInitialize().
   */
  protected abstract void startProcessing() throws Exception;
}
