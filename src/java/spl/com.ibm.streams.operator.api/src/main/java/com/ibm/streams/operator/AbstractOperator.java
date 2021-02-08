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

package com.ibm.streams.operator;

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.management.OperatorManagement;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.Map;
import javax.management.Notification;
import javax.management.NotificationEmitter;
import javax.management.NotificationListener;

/**
 * An abstract implementation of Operator that may be used by developers to implement their own
 * Operator. <br>
 * Simple implementations of the required methods are provided and a number of utility methods.
 */
public abstract class AbstractOperator implements Operator {

  private OperatorContext context;

  public AbstractOperator() {}

  /**
   * {@inheritDoc}
   *
   * <p>This implementation initializes the operator to ensure its utility methods return the
   * correct information.
   *
   * <p>Sub-classes <b>must</b> call <code>super.initialize(context)</code> if they override this
   * method.
   */
  public synchronized void initialize(OperatorContext context) throws Exception {
    this.context = context;
  }

  /**
   * {@inheritDoc}
   *
   * <p>This implementation does nothing.
   */
  public void allPortsReady() throws Exception {}

  /**
   * Return the OperatorContext object describing the execution environment of this operator.
   *
   * @return OperatorContext context for this operator
   */
  public final synchronized OperatorContext getOperatorContext() {
    return context;
  }

  /**
   * {@inheritDoc}
   *
   * <p>This implementation does nothing, sub-classes that need to process incoming tuples must
   * override this method.
   */
  public void process(StreamingInput<Tuple> stream, Tuple tuple) throws Exception {}

  /**
   * {@inheritDoc}
   *
   * <p>This implementation punctuates all (if any) output ports with the incoming mark if it is a
   * WINDOW_MARKER. Sub-classes may override this method to provide different behavior.
   */
  public void processPunctuation(StreamingInput<Tuple> stream, Punctuation mark) throws Exception {
    if (mark != Punctuation.WINDOW_MARKER) return;

    for (StreamingOutput<OutputTuple> output : getOperatorContext().getStreamingOutputs())
      output.punctuate(mark);
  }

  /**
   * {@inheritDoc}
   *
   * <p>This implementation does nothing.
   */
  public void shutdown() throws Exception {}

  /**
   * Check that the correct number of ports exists for the operator. <br>
   * Use of this deprecated method can limit the flexibility of an {@code Operator} implementation
   * class by fixing its ports thus disallowing sub-classes from adding optional ports, thus its use
   * is <B>not</B> recommended. <br>
   * Instead, it is recommended that input and output port configurations in the Java primitive
   * operator model are used.
   *
   * <p>Typically called by a sub-class' initialize method. If the number of ports is positive then
   * exactly that many ports must exist according to the context. If the number of ports is negative
   * then at least that many must exist.
   *
   * <p>As an example, checkPorts(-1,1) indicates one or more input port and one output port.
   *
   * @param numberInputPorts expected number of input ports
   * @param numberOutputPorts expected number of output ports
   * @throws IllegalStateException if ports do not match the requirements
   * @deprecated As of InfoSphere Streams 3.0, replaced by input and output port configurations in
   *     the Java primitive operator model.
   */
  @Deprecated
  public final void checkPorts(int numberInputPorts, int numberOutputPorts) {
    if (numberInputPorts >= 0) {
      if (getOperatorContext().getNumberOfStreamingInputs() != numberInputPorts)
        throw new IllegalStateException(
            /*"Operator: "
            + getClass().getName() + " requires exactly "
            + numberInputPorts + " input port(s)."*/
            new Message(Key.AO_INVALID_NUMBER_INPUT_PORTS, getClass().getName(), numberInputPorts)
                .getLocalizedMessageAndPrefix());
    } else {
      numberInputPorts *= -1;
      if (getOperatorContext().getNumberOfStreamingInputs() < numberInputPorts)
        throw new IllegalStateException(
            /*"Operator: "
            + getClass().getName() + " requires at least "
            + numberInputPorts + " input port(s)."*/
            new Message(Key.AO_TOO_FEW_INPUT_PORTS, getClass().getName(), numberInputPorts)
                .getLocalizedMessageAndPrefix());
    }

    if (numberOutputPorts >= 0) {
      if (getOperatorContext().getNumberOfStreamingOutputs() != numberOutputPorts)
        throw new IllegalStateException(
            /*"Operator: "
            + getClass().getName() + " requires exactly "
            + numberOutputPorts + " output port(s)."*/
            new Message(Key.AO_INVALID_NUMBER_OUTPUT_PORTS, getClass().getName(), numberOutputPorts)
                .getLocalizedMessageAndPrefix());
    } else {
      numberOutputPorts *= -1;
      if (getOperatorContext().getNumberOfStreamingOutputs() < numberOutputPorts)
        throw new IllegalStateException(
            /*"Operator: "
            + getClass().getName() + " requires at least "
            + numberOutputPorts + " output port(s)."*/
            new Message(Key.AO_TOO_FEW_OUTPUT_PORTS, getClass().getName(), numberOutputPorts)
                .getLocalizedMessageAndPrefix());
    }
  }

  /**
   * Shorthand method to get the StreamingInput object for a given port.
   *
   * @param port input port number.
   * @return the description for the input port
   */
  public final StreamingInput<Tuple> getInput(int port) {
    return getOperatorContext().getStreamingInputs().get(port);
  }

  /**
   * Shorthand method to get the StreamingOutput object for a given port.
   *
   * @param port output port number
   * @return the description for the input port
   */
  public final StreamingOutput<OutputTuple> getOutput(int port) {
    return getOperatorContext().getStreamingOutputs().get(port);
  }

  /**
   * Set the aspects associated with a named {@code Logger}. The aspects replace any existing
   * aspects associated with any {@code Logger} of the given name. Aspects are not inherited by
   * child {@code Logger}s.
   *
   * @param loggerName Logger name
   * @param aspects Aspects to associate with named logger.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public final void setLoggerAspects(String loggerName, String... aspects) {
    getOperatorContext().getPE().setLoggerAspects(loggerName, aspects);
  }

  /**
   * Create a thread to avoid completion of the operator by the SPL runtime. This is equivalent to
   * calling <br>
   * {@code AbstractOperator.createAvoidCompletionThread(getOperatorContext())}. <br>
   * This method must be called at {@link Operator#initialize(OperatorContext)} time.
   *
   * @see #createAvoidCompletionThread(OperatorContext)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public Thread createAvoidCompletionThread() {
    return createAvoidCompletionThread(getOperatorContext());
  }

  /**
   * Create a thread to avoid completion of the operator by the SPL runtime if the operator has no
   * input ports. If the operator has input ports then this is equivalent to calling {@link
   * #createAvoidCompletionThread()}. <br>
   * If the operator has input ports then no thread is created and operator completion will occur
   * when all the input ports have received {@link StreamingData.Punctuation#FINAL_MARKER final
   * marks}. <br>
   * This method must be called at {@link Operator#initialize(OperatorContext)} time.
   *
   * @see #createAvoidCompletionThread()
   * @see #createAvoidCompletionThread(OperatorContext)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public Thread createAvoidCompletionThreadIfNoInputs() {
    if (getOperatorContext().getNumberOfStreamingInputs() == 0)
      return createAvoidCompletionThread();
    return null;
  }

  /**
   * Create a thread that avoids the SPL runtime completing the operator. The SPL runtime will
   * complete an operator once it has no remaining active threads or tasks and all of its input
   * ports have received {@link StreamingData.Punctuation#FINAL_MARKER final marks}. Completing an
   * operator will send final marks to all of its output ports, thus closing the ports. <br>
   * Source operators that produce tuples in threads that are not visible to the SPL runtime may
   * immediately complete after {@link Operator#allPortsReady()} and thus not produce any tuples on
   * their output ports. This typically will happen when the source operator is producing tuples
   * from an event framework. In this case the operator has registered an event handler with the
   * event framework, but the framework is executing using its own threads that are not visible to
   * the SPL runtime. <br>
   * The correct mechanism to handle this is to pass the operator's {@link
   * OperatorContext#getThreadFactory() thread factory} or {@link
   * OperatorContext#getScheduledExecutorService() execution service} when initializing the
   * framework. <br>
   * If the framework does not support passing in a thread factory or execution service then the
   * operator needs to create a thread to avoid completion. This convenience method is called at
   * {@link Operator#initialize(OperatorContext)} to create such a thread.
   *
   * <p>This method:
   *
   * <OL>
   *   <LI>Creates a thread using {@link OperatorContext#getThreadFactory()}
   *   <LI>Sets the thread to not be a daemon thread
   *   <LI>Registers a notification listener with the operator's {@link OperatorContextMXBean} to
   *       start the thread at {@link OperatorContextMXBean#ALL_PORTS_READY}
   *   <LI>Registers a notification listener with the operator's {@link OperatorContextMXBean} to
   *       interrupt the thread at {@link OperatorContextMXBean#SHUTDOWN}
   * </OL>
   *
   * <br>
   * The thread's {@code run()} method simply waits to be interrupted. This method must be called at
   * {@link Operator#initialize(OperatorContext)} time.
   *
   * @param context Context for the operator avoiding completion
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public static Thread createAvoidCompletionThread(final OperatorContext context) {

    final Thread avoidCompletion =
        context
            .getThreadFactory()
            .newThread(
                new Runnable() {

                  @Override
                  public void run() {
                    while (!Thread.currentThread().isInterrupted()) {
                      try {
                        synchronized (this) {
                          wait();
                        }
                      } catch (InterruptedException e) {
                        break;
                      }
                    }
                  }
                });

    avoidCompletion.setDaemon(false);
    avoidCompletion.setName(avoidCompletion.getName() + " - AvoidCompletion");

    OperatorContextMXBean ocmxb = OperatorManagement.getOperatorContextMXBean(context);

    NotificationEmitter ocne = (NotificationEmitter) ocmxb;

    ocne.addNotificationListener(
        new NotificationListener() {

          @Override
          public void handleNotification(Notification notification, Object handback) {
            final String type = notification.getType();
            if (OperatorContextMXBean.ALL_PORTS_READY.equals(type)) avoidCompletion.start();
            else if (OperatorContextMXBean.SHUTDOWN.equals(type)) avoidCompletion.interrupt();
          }
        },
        null,
        null);

    return avoidCompletion;
  }

  /**
   * Set tag data for the operator. If this value is {@link Operator.TagNames#OperatorIGC}, the tags
   * will be registered in the IG catalog. Other tag names may be defined in future releases.
   *
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public void setTagData(String tagName, Map<String, String> tagValues) {
    getOperatorContext().getPE().setTagData(tagName, tagValues);
  }
}
