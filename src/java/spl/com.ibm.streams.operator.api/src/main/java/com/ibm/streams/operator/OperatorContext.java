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

import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.state.ConsistentRegionContext;
import com.ibm.streams.operator.state.StateHandler;
import java.io.File;
import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.net.MalformedURLException;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadFactory;

/**
 * Context information for the {@code Operator}'s execution context. <br>
 * At SPL compile time an operator class can verify its invocation context by annotating static
 * methods with {@link OperatorContext.ContextCheck}, such as verify its input port schema matches
 * its output port schema. <br>
 * At execution time an operator uses its context to interact with its invocation context including
 * its {@link #getStreamingInputs() input ports}, {@link #getStreamingOutputs() output ports} for
 * tuple submission, and {@link #getThreadFactory() thread factory} & {@link
 * #getScheduledExecutorService() execution service} for background activity.
 *
 * <p>A JMX management bean {@link com.ibm.streams.operator.management.OperatorContextMXBean
 * OperatorContextMXBean} is registered in the platform MBean server for every operator's {@code
 * OperatorContext}. The management bean allows code to respond to the operator's life cycle
 * notifications without being tightly integrated with a specific operator's {@link
 * Operator#allPortsReady()} or {@link Operator#shutdown()} methods. For example an object can be
 * initialized by {@link Operator#initialize(OperatorContext)} and then manage its own shutdown by
 * registering a {@code NotificationListener} against the {@link
 * com.ibm.streams.operator.management.OperatorContextMXBean#SHUTDOWN} notification.
 *
 * @see com.ibm.streams.operator.management.OperatorContextMXBean
 */
public interface OperatorContext {

  /**
   * A method annotated {@code @ContextCheck} allows checking of an operator's invocation context.
   * Annotated methods can be executed during SPL compilation and at runtime before {@link
   * Operator#initialize(OperatorContext)} is called.
   *
   * <p>Annotated methods are searched for in the operator's Java class (which must be an instance
   * of {@link Operator}) and all of its super classes. Order of invocation of annotated methods is
   * not defined. <br>
   * The annotated method must be {@code public}, {@code static} and have a single parameter of type
   * {@link com.ibm.streams.operator.compile.OperatorContextChecker}. Any other method annotated
   * with {@code @ContextCheck} will be ignored. <br>
   * At runtime {@code @ContextCheck} methods are invoked before invocation of {@code @Parameter}
   * annotated methods.
   *
   * <p>Here's an example where if the optional parameter {@code user} is set then the optional
   * parameter {@code password} must also be set.
   *
   * <pre><code>
   * &#64;ContextCheck
   * public static void checkUserPassword(OperatorContextChecker checker) {
   * checker.checkDependentParameters("user", "password");
   * }
   * </code></pre>
   *
   * <p>Any exception thrown during the method's invocation causes compilation or runtime invocation
   * to fail.
   *
   * <p>Compile time checking using {@code @ContextCheck} is only supported by the SPL compiler for
   * Java primitive operators. Invocation of an {@link Operator} using the SPL standard toolkit
   * operator {@code spl.utility.JavaOp} will not invoke methods annotated with
   * {@code @ContextCheck}.
   *
   * @see com.ibm.streams.operator.compile.OperatorContextChecker
   */
  @Documented
  @Retention(RetentionPolicy.RUNTIME)
  @Target({ElementType.METHOD})
  public @interface ContextCheck {
    /**
     * Check at compile time.
     *
     * @return True to invoke method at SPL compile time, false to not execute at SPL compile time.
     */
    boolean compile() default true;

    /**
     * Check at runtime.
     *
     * @return True to invoke method at runtime, false to not execute at runtime.
     */
    boolean runtime() default true;
  }

  /**
   * Get the name of the operator.
   *
   * <p>The operator name is unique across the application. For operators in a parallel region, the
   * name includes the {@link #getChannel() channel} they are in.
   *
   * @return the name of the operator.
   * @see #getLogicalName()
   */
  public String getName();

  /**
   * Get the number of streaming input ports.
   *
   * @return number of streaming input ports.
   */
  public int getNumberOfStreamingInputs();

  /**
   * Get the list of input ports. The order of the list will match the order of the ports in the SPL
   * application.
   *
   * @return list of input ports
   */
  public List<StreamingInput<Tuple>> getStreamingInputs();

  /**
   * Get the number of output ports.
   *
   * @return number of output ports.
   */
  public int getNumberOfStreamingOutputs();

  /**
   * Get a list of output ports. The order of the list will match the order of the ports in the SPL
   * application.
   *
   * @return list of output ports.
   */
  public List<StreamingOutput<OutputTuple>> getStreamingOutputs();

  /**
   * Returns the list of parameter values for the given parameter name. If no values were supplied
   * for the parameter name then an empty list is returned. <br>
   * The recommended mechanism for handling operator parameters is using the {@link Parameter}
   * annotation. The annotation is applied to a setter method on the operator's class, and then at
   * runtime the setter method will be called with the appropriate value for the SPL parameter. See
   * the {@link Parameter documentation for &#64;Parameter} for full details.
   *
   * <p>Supported SPL types for parameters are {@code ustring}, {@code rstring} (using UTF-8
   * encoding), {@code boolean}, {@code int8}, {@code int16}, {@code int32}, {@code int64}, {@code
   * float32}, {@code float64}, {@code decimal32}, {@code decimal64} and {@code decimal128},
   * operator custom literals and stream attributes. <br>
   * Parameter values set in SPL source are converted to {@code String} values by this method. For
   * example an SPL {@code int32} parameter set as follows:
   *
   * <pre>
   * threshold: 42;
   * </pre>
   *
   * can be converted into a Java primitive {@code int} using:
   *
   * <pre>
   * int threshold = Integer.valueOf(context.getParameterValues("threshold").get(0));
   * </pre>
   *
   * <br>
   * For an stream attribute parameter, this method returns each attribute value as a string in the
   * form of {@code InputPortName.AttributeName}
   *
   * @param parameterName name of parameter
   * @return list of values, an empty list is returned if the parameter was not defined
   * @see Parameter &#64;Parameter is the recommended mechanism for parameters
   * @since InfoSphere&reg; Streams Version 4.0 - Support for custom literals and attributes
   *     parameter types.
   */
  public List<String> getParameterValues(String parameterName);

  /**
   * Return the set of names for parameters that have values.
   *
   * @return set of parameter names, an empty set if no parameters are defined
   * @see Parameter &#64;Parameter is the recommended mechanism for parameters
   */
  public Set<String> getParameterNames();

  /**
   * Return a scheduler to execute background tasks. Operators should utilize this service or {@link
   * #getThreadFactory()} rather than creating their own threads to ensure that the SPL runtime will
   * wait for the operator's background work before completing the operator.
   *
   * <p>The scheduler will be shutdown when the operator is to be shutdown and the scheduler
   * shutdown will occur before {@link Operator#shutdown()} is called. Once the scheduler is
   * shutdown no new tasks will be accepted. Existing scheduled tasks will remain in the scheduler's
   * queue but periodic tasks will canceled. Once the {@code Operator.shutdown()} returns {@code
   * ExecutorService.shutdownNow()} will be called on the scheduler to cancel any outstanding tasks.
   * <br>
   * Operator implementations that wish to complete any outstanding tasks at shutdown time can call
   * {@code ExecutorService.awaitTermination()} to wait for outstanding tasks to complete or wait on
   * the specific {@code Future} reference for a task.
   *
   * <p>The Operator will not be seen as complete by the SPL runtime until all of its outstanding
   * scheduled tasks have completed.
   *
   * <p>The returned scheduler service is guaranteed to be an instance of {@code
   * java.util.concurrent.ScheduledThreadPoolExecutor} and initially has this configuration:
   *
   * <UL>
   *   <LI>{@code corePoolSize} Set to {@code Runtime.availableProcessors()} with a minimum of 2 and
   *       maximum of 8.
   *   <LI>{@code allowsCoreThreadTimeOut()} set to {@code true}
   *   <LI>{@code keepAliveTime} set to 5 seconds
   * </UL>
   *
   * Threads are created on demand to execute tasks, so that even if the {@code corePoolSize} is
   * eight, eight threads will only be created if there are eight concurrent tasks scheduled.
   * Threads will be removed if they are not needed for the {@code keepAliveTime} value and {@code
   * allowsCoreThreadTimeOut()} returns {@code true}. <br>
   * Operators may modify these settings, typically during {@link
   * Operator#initialize(OperatorContext)}.
   *
   * @return Scheduler service for the operator.
   * @see Operator#shutdown()
   * @see ExecutorService#awaitTermination(long, java.util.concurrent.TimeUnit)
   * @see ExecutorService#shutdown()
   * @see ExecutorService#shutdownNow()
   * @see Future
   * @see java.util.concurrent.ScheduledThreadPoolExecutor
   */
  public ScheduledExecutorService getScheduledExecutorService();

  /**
   * Return a ThreadFactory specific to the Operator with the thread context class loader set
   * correctly. Operators should utilize the returned factory to create Threads.
   *
   * <p>Threads returned by the ThreadFactory have not been started and are set as daemon threads.
   * Operators may set the threads as non-daemon before starting them. The SPL runtime will wait for
   * non-daemon threads before terminating a processing element in standalone mode.
   *
   * <p>Any uncaught exception thrown by the {@code Runnable} passed to the {@code
   * ThreadFactory.newThread(Runnable)} will cause the processing element containing the operator to
   * terminate.
   *
   * <p>The ThreadFactory will be shutdown when the operator is to be shutdown and the ThreadFactory
   * shutdown will occur before <code>Operator.shutdown()</code> is called. Once the ThreadFactory
   * is shutdown a call to <code>newThread()
   * </code> will return null.
   *
   * @return A ThreadFactory specific to the Operator.
   */
  public ThreadFactory getThreadFactory();

  /**
   * Get the processing element hosting this Operator
   *
   * @return ProcessingElement hosting this Operator instance.
   */
  public ProcessingElement getPE();

  /**
   * Get the OperatorMetrics object for access to metrics specific to this operator.
   *
   * @return OperatorMetrics instance for this operator.
   */
  public OperatorMetrics getMetrics();

  /**
   * Get the SPL primitive operator kind that is executing the this operator. For a Java primitive
   * operator the name of the operator will be returned, otherwise {@code spl.utility::JavaOp} will
   * be returned.
   *
   * <p>The return will be {@code null} if the SPL application executing the {@code Operator} has
   * not been recompiled using InfoSphere Streams 3.0 or later.
   *
   * @return SPL primitive operator kind that is executing the this operator.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public String getKind();

  /**
   * Add class libraries to the operator's class loader. The operator's class loader is set as the
   * thread context class loader for the operator's {@link #getThreadFactory() thread factory},
   * {@link #getScheduledExecutorService() executor} and any method invocation on the operator
   * instance, such as {@link Operator#initialize(OperatorContext) initialize} and {@link
   * Operator#process(StreamingInput, Tuple) process}.
   *
   * <p>Operators use this method to add class libraries specific to the invocation of an operator
   * in an SPL application in a consistent manner. An example is defining the jar files that contain
   * the JDBC driver to be used by the application. Typically an {@link
   * com.ibm.streams.operator.model.Parameter operator parameter } of type {@code rstring} with
   * cardinality greater than one that maps to a Java bean of type {@code String[]}, for example:
   *
   * <pre>
   * <code>
   * private String[] driverLibrary;
   * &#64;Parameter
   * public void setDriverLibrary(String[] driverLibrary) {
   *     this.driverLibrary = driverLibrary;
   * }
   * public String[] getDriverLibrary() {
   *    return
   * }
   *
   * public void initialize(OperatorContext context) throws Exception {
   *   ...
   *   // Add JDBC drivers to the class context loader
   *   context.addClassLibraries(getDriverLibrary());
   *   ...
   * }
   * </code>
   * </pre>
   *
   * <p>Each element of {@code libraries} is trimmed and then converted into a {@code java.net.URL}.
   * If the element cannot be converted to a {@code URL} then it is assumed to represent a file
   * system path and is converted into an {@code URL} representing that path. If the file path is
   * relative it is taken as relative to the {@link #getToolkitDirectory() toolkit directory} that
   * declares the operator being invoked. In an SPL application an invocation of an operator may use
   * the SPL function {@code getThisToolkitDir()} to locate jars in the toolkit of the SPL file, for
   * example with the above operator parameter example the SPL invocation would include:
   *
   * <pre>
   * <code>
   *    param driverLibrary: getThisToolkitDir() + "/opt/derby/lib/derby.jar";
   * </code>
   * </pre>
   *
   * <br>
   * If a file path ends with {@code /* } then it is assumed to be a directory and all jar files in
   * the directory with the extension {@code .jar} or {@code .JAR} are added to the operator's class
   * loader.
   *
   * <p><B>Note:</B> To support the changes to relocatable application bundles in InfoSphere&reg;
   * Streams Version 4.0, the root for relative paths was changed to the {@link
   * #getToolkitDirectory() toolkit directory} from the {@link ProcessingElement#getDataDirectory()
   * data directory}.
   *
   * @param libraries String representations of URLs and file paths to be added into the operator's
   *     class loader. If {@code null} then no libraries are added to the class loader.
   * @throws IllegalStateException Method is being called at SPL compile time.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public void addClassLibraries(String[] libraries) throws MalformedURLException;

  /**
   * Get the index of the parallel channel the operator is on.
   *
   * <p>If the operator is in a parallel region, this method returns a value from 0 to N-1, where N
   * is the {@link #getMaxChannels() number of channels in the parallel region}; otherwise it
   * returns -1.
   *
   * @return the index of the parallel channel this operator is on, if the operator is in a parallel
   *     region, or -1 if the operator is not in a parallel region.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public int getChannel();

  /**
   * Get the index of the parallel channel within the most immediate parallel region the operator is
   * in.
   *
   * <p>If the operator is in a parallel region, this method returns a value from 0 to N-1, where N
   * is the {@link #getLocalMaxChannels() number of channels in the parallel region}; otherwise it
   * returns -1.
   *
   * @return the index of the parallel channel this operator is on, if the operator is in a parallel
   *     region, or -1 if the operator is not in a parallel region.
   * @since Streams&reg; Version 4.2
   */
  public int getLocalChannel();

  /**
   * Get a list of all the indexes of the parallel channels the operator is in.
   *
   * <p>If the operator is in one or more parallel regions, this method returns a list of values
   * from 0 to N-1, where N is the {@link #getMaxChannels() number of channels in each parallel
   * region}; otherwise it returns a single entry of -1.
   *
   * @return list of the indexes of the parallel channels this operator is in, if the operator is in
   *     one or more parallel regions, or [-1] if the operator is not in a parallel region.
   * @since IBM Streams Version 4.2
   */
  public List<Integer> getAllChannels();

  /**
   * Get the total number of parallel channels for the parallel region that the operator is in. If
   * the operator is not in a parallel region, this method returns 0.
   *
   * @return the number of parallel channels for the parallel region that this operator is in, or 0
   *     if the operator is not in a parallel region.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public int getMaxChannels();

  /**
   * Get the total number of parallel channels for the most immediate parallel region that the
   * operator is in. If the operator is not in a parallel region, this method returns 0.
   *
   * @return the number of parallel channels for the parallel region that this operator is in, or 0
   *     if the operator is not in a parallel region.
   * @since IBM Streams Version 4.2
   */
  public int getLocalMaxChannels();

  /**
   * Get the total number of parallel channels for the parallel regions that the operator is in. If
   * the operator is not in a parallel region, this method returns [0].
   *
   * @return list of the number of parallel channels for the parallel regions that this operator is
   *     in, or [0] if the operator is not in a parallel region.
   * @since IBM Streams Version 4.2
   */
  public List<Integer> getAllMaxChannels();

  /**
   * Get the logical name of the operator.
   *
   * @return the logical name of the operator.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public String getLogicalName();

  /**
   * Get an optional context for this operator invocation. An invocation of an operator may have
   * optional contexts, that provide information specific to a particular environment, such as a
   * {@link ConsistentRegionContext consistent region}.
   *
   * @param contextType Type of the context the operator might be invoked in.
   * @return Optional context of type {@code contextType} if this invocation supports that context,
   *     otherwise {@code null}.
   * @see OptionalContext
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public <O extends OptionalContext> O getOptionalContext(Class<O> contextType);

  /**
   * Register a {@link StateHandler} to manage state for this operator. The SPL Runtime will call
   * the handler's methods to make the operator's state consistent or to reset it to a previous
   * consistent state. <br>
   * Multiple state handlers may be registered by an operator either explicitly using this method or
   * implicitly by:
   *
   * <UL>
   *   <LI>If an {@link Operator} itself implements {@code StateHandler} then it is registered as a
   *       {@code StateHandler}. The registration occurs prior to calling {@link
   *       Operator#initialize(OperatorContext)}.
   *   <LI>If a {@link com.ibm.streams.operator.window.StreamWindowListener} implements {@code
   *       StateHandler} then when it is {@link
   *       com.ibm.streams.operator.window.StreamWindow#registerListener(com.ibm.streams.operator.window.StreamWindowListener,
   *       boolean) registered as a window listener} it is also registered as a {@code
   *       StateHandler}. The registration occurs when {@code registerListener()} is called.
   * </UL>
   *
   * Operators must ensure that instances of {@code StateHandler} and {@code StreamWindowListener}
   * are registered in a consistent order to ensure the order of information written to a checkpoint
   * and read from a checkpoint upon reset is consistent.
   *
   * @param handler State handler for this operator.
   * @see
   *     com.ibm.streams.operator.window.StreamWindow#registerListener(com.ibm.streams.operator.window.StreamWindowListener,
   *     boolean)
   * @see com.ibm.streams.operator.window.StreamWindowListener
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public void registerStateHandler(StateHandler handler);

  /**
   * Get the root directory of the toolkit that declares the {@link #getKind()} of this operator
   * invocation. <br>
   * At SPL compile time this is the location of the toolkit specified to the SPL compiler {@code
   * sc}. <br>
   * At runtime this is the location of the toolkit in the application landing zone that has been
   * unpacked from the application's bundle ({@code sab} file).
   *
   * @return the absolute path of the toolkit root directory
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public File getToolkitDirectory();
}
