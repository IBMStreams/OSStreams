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

import static com.ibm.streams.operator.internal.logtrace.Setup.opTrace;

import com.ibm.streams.operator.DataException;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OptionalContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.eventtime.EventTimeContext;
import com.ibm.streams.operator.internal.eventtime.EventTimeContextImpl;
import com.ibm.streams.operator.internal.eventtime.Watermark;
import com.ibm.streams.operator.internal.logtrace.Setup;
import com.ibm.streams.operator.internal.metrics.OpMetrics;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.ports.OperatorOutputPort;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.ports.SwitchingHandler;
import com.ibm.streams.operator.internal.ports.ThreadedPortHandler;
import com.ibm.streams.operator.internal.ports.actions.WatermarkAction;
import com.ibm.streams.operator.internal.runtime.AbstractStream;
import com.ibm.streams.operator.internal.runtime.CatchContext;
import com.ibm.streams.operator.internal.runtime.InputPort;
import com.ibm.streams.operator.internal.runtime.OperatorAnnotations;
import com.ibm.streams.operator.internal.runtime.OperatorScheduledExecutor;
import com.ibm.streams.operator.internal.runtime.OperatorThreadFactory;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.internal.runtime.TupleProcessor;
import com.ibm.streams.operator.internal.state.CheckpointImpl;
import com.ibm.streams.operator.internal.state.CheckpointTransport;
import com.ibm.streams.operator.internal.state.ConsistentRegionContextImpl;
import com.ibm.streams.operator.internal.state.InternalCheckpointContext;
import com.ibm.streams.operator.internal.window.WindowProcessor;
import com.ibm.streams.operator.logging.LogLevel;
import com.ibm.streams.operator.management.OperatorContextMXBean;
import com.ibm.streams.operator.management.OperatorLifeCycle;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.model.SharedLoader;
import com.ibm.streams.operator.state.CheckpointContext;
import com.ibm.streams.operator.state.ConsistentRegionContext;
import com.ibm.streams.operator.state.DelegateStateHandler;
import com.ibm.streams.operator.state.OrderedStateHandlers;
import com.ibm.streams.operator.state.StateHandler;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.platform.services.AnnotationType;
import com.ibm.streams.platform.services.AugmentedPEInputPortType;
import com.ibm.streams.platform.services.ImportedStreamsType;
import com.ibm.streams.platform.services.OperInstanceInputPortType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ScheduledExecutorService;
import javax.management.MalformedObjectNameException;

/**
 * Wraps the user's Operator to allow consistent behaviour and attachment points. OperatorAdapter is
 * thread safe and it different threads may call the initialize, allPortsReady and shutdown methods.
 */
public class OperatorAdapter extends BaseOpAdapter
    implements TupleProcessor<StreamingInput<Tuple>, Tuple> {

  /*
   ** Parameter names for passing runtime information as parameters.
   ** All have names that are enclosed in square brackets as those
   ** values are invalid in SPL and hence can never be entered by a
   ** invocation of a JavaOp in a SPL application.
   */
  public static final String OP_PREFIX = "[";
  public static final String OP_DATA_DIRECTORY = "[dataDir]";
  public static final String OP_OUTPUT_DIRECTORY = "[outputDir]";
  public static final String OP_APPLICATION_DIRECTORY = "[applicationDir]";
  public static final String OP_APPLICATION_NAME = "[applicationName]";
  public static final String OP_PE_ID = "[peId]";
  public static final String OP_JOB_GROUP = "[jobGroup]";
  public static final String OP_JOB_ID = "[jobId]";
  public static final String OP_JOB_NAME = "[jobName]";
  // Count of PE input and output ports.
  public static final String OP_PE_INPUT_PORTS = "[peInputPorts]";
  public static final String OP_PE_OUTPUT_PORTS = "[peOutputPorts]";
  public static final String OP_PE_STANDALONE = "[standalone]";
  public static final String OP_PE_OPTIMIZED = "[optimized]";
  public static final String OP_PE_RELAUNCH = "[relaunch]";
  public static final String OP_PE_APPLICATION_SCOPE = "[applicationScope]";
  public static final String OP_PE_INSTANCE_ID = "[instanceId]";
  public static final String OP_PE_OPERATOR_INDEX = "[operatorIndex]"; // for an operator
  public static final String OP_PE_OPERATOR_KIND = "[kind]";
  public static final String OP_PE_DOMAIN_ID = "[domainId]";

  /**
   * Information pertaining a surrounding parallel region. This information is sent to the Java
   * runtime from the C++ runtime using the JNIBridge setParameter call.
   */
  public static final String OP_LOGICAL_NAME = "[logicalName]";

  /**
   * Connection information from an output port for this operator to another operator within the
   * PE/graph on a single JVM/PE. Each connection is encoded in two values.
   * [intraPeConnection].outputPortIndex = receiving operator index
   * [intraPeConnection].outputPortIndex = input port index in receiving operator
   */
  public static final String OP_INTRA_PE_CONNS = "[intraPeConnection].";
  /** For testing pass the java operator model directory into the operator invocation. */
  public static final String FLOW_JAVAOP_DIR = "[flow.javaOpDir]";

  /** Window information parameter gets appended with the input port number, e.g. [window].3 */
  public static final String OP_WINDOW = "[window].";
  /** PE port information parameter gets appended with the port number, e.g. [peInputPortIndex].3 */
  public static final String OP_PE_INPUT_PORT_INDEX = "[peInputPortIndex].";

  public static final String OP_PE_OUTPUT_PORT_INDEX = "[peOutputPortIndex].";

  /**
   * Pass whether the PE is being profiled or debugged. This information is sent to the Java runtime
   * from the C++ runtime using the JNIBridge setParameter call.
   */
  public static final String OP_PE_DEBUG = "[debug]";

  public static final String OP_PROFILING = "[profiling]";

  /**
   * Operator input port information gets appended with the input port number. e.g.,
   * [opInputPortControl].1
   */
  public static final String OP_INPUT_PORT_CONTROL = "[opInputPortControl].";

  /** Parameters related to consistent cut. */
  public static final String OP_INPUT_PORT_CUT_OBLIVIOUS = "[opInputPortCutOblivious].";

  public static final String OP_IS_IN_CUT_REGION = "[opIsInCutRegion]";
  public static final String OP_IS_START_OF_CUT_REGION = "[opIsStartOfCutRegion]";
  public static final String OP_IS_END_OF_CUT_REGION = "[opIsEndOfCutRegion]";
  public static final String OP_IS_TRIGGER_OPERATOR = "[opIsTriggerOperator]";
  public static final String OP_CC_REGION_INDEX = "[opCCRegionIndex]";

  /** The value of the switch punctuation, which exists only in the C++ code. */
  public static final int SWITCH_PUNCT_VALUE = 124;

  /** Class loaders that are shared across multiple operator invocations. */
  private static final Map<List<String>, OperatorClassLoader> sharedLoaders =
      new HashMap<List<String>, OperatorClassLoader>();

  /*
   ** Instance fields
   */

  /** The user defined operator handling the streams. */
  protected final Operator operator;

  private final ExecutionContext executionContext;

  private final InternalCheckpointContext ckptContext;

  protected OperatorMetrics opMetrics;

  /** Operator's class name. */
  private final String operatorClassName;

  /** Parameters passed in from application (immutable). */
  private final Map<String, List<String>> params;

  /** Set of parameter names (immutable) */
  final Set<String> parameterNames;

  /** Class loader used to load the user code and any third party libraries. */
  final OperatorClassLoader loader;

  /** My scheduler which is private to this operator. */
  final OperatorScheduledExecutor scheduler;

  /**
   * My thread factory which is private to this operator. This is for Threads created by the
   * Operator code, the scheduler uses a different instance of OperatorThreadFactory so that when
   * the operator is shutdown we don't interrupt any scheduler work (those will be notified by the
   * scheduler.shutdown() method).
   */
  final OperatorThreadFactory userThreadFactory;

  /** Read-only list of input ports, may be empty. */
  final List<StreamingInput<Tuple>> inputPorts;

  /** Number of input ports that have not been finalized. */
  private int openInputPorts;

  /** List of output ports, may be empty, will be read-only after complete is called. */
  private final List<StreamingOutput<OutputTuple>> outputPorts;

  /** Has shutdown been called on this operator. */
  private boolean shutdown;

  /** Management object for this operator adapter. */
  private OperatorInfo mgmt;

  /** Map of port to logic clauses. */
  private final Map<Integer, List<InputPortLogic>> logicClauses =
      new HashMap<Integer, List<InputPortLogic>>();

  /**
   * Input handlers used to deliver tuples and punctuation to the input ports. Need to maintain a
   * reference so that when deciding to fuse late, we don't create a second window processor.
   */
  private final RuntimeStream<Tuple>[] inputHandlers;

  private HashMap<Integer, SwitchingHandler<Tuple>> switchingHandlers =
      new HashMap<Integer, SwitchingHandler<Tuple>>();
  private HashMap<Integer, ThreadedPortHandler> threadedHandlers =
      new HashMap<Integer, ThreadedPortHandler>();

  static {
    // Force loading of Apache Log4j classes to
    // ensure the Streams log4j.properties is
    // picked up first.
    org.apache.log4j.Logger.getRootLogger();
  }

  public synchronized boolean isShutdown() {
    return shutdown;
  }

  /** The thread that is waiting for completion of the Operator's asynchronous work, may be null. */
  private Thread completer;

  private boolean completerNotify;

  /** OperatorContext that describes the invocation context to the user operator. */
  private final OperatorContext context;

  /** CatchContext describes the invocation context of the operator w.r.t the @catch annotation */
  private final CatchContext catchContext;

  /**
   * Create a new ClassLoader for the user's operator.
   *
   * @param classLibraries Classpath.
   * @param pe ProcessingElement containing the operator
   * @return A class loader using the passed in class libraries.
   */
  public static OperatorClassLoader newOperatorClassLoader(
      List<String> classLibraries, ProcessingElement pe, boolean internalOp)
      throws MalformedURLException {

    return OperatorClassLoader.newLoader(classLibraries, pe.getOutputDirectory(), internalOp);
  }

  /**
   * Get a class loader for this operator invocation. A shared class loader will be returned if the
   * operator class is annotated @SharedLoader. A loader is shared across operator invocations for
   * any operators annotated with @SharedLoader(true) and having the same list of libraries.
   */
  private static OperatorClassLoader getOperatorClassLoader(
      final List<String> classLibraries, final String className, final ProcessingElement pe)
      throws MalformedURLException, ClassNotFoundException {
    OperatorClassLoader loader;
    synchronized (sharedLoaders) {
      loader = sharedLoaders.get(classLibraries);
    }

    if (loader != null) {
      Class<? extends Operator> clazz = loadOperatorClass(loader, className);
      SharedLoader sl = clazz.getAnnotation(SharedLoader.class);
      if (sl != null && sl.value()) return loader;

      // this operator doesn't want to share, even though
      // other operators with the same set of jars
      // are sharing.
      // Note that the class we just loaded will not
      // be used as the class for the operator, as is it
      // from the shared loader.
    }

    boolean internalOp = className.startsWith("com.ibm.streams.control.");

    loader = newOperatorClassLoader(classLibraries, pe, internalOp);
    Class<? extends Operator> clazz = loadOperatorClass(loader, className);
    SharedLoader sl = clazz.getAnnotation(SharedLoader.class);
    if (sl != null && sl.value()) {
      synchronized (sharedLoaders) {
        // Just in case another operator has loaded it in the
        // meantime.
        if (sharedLoaders.containsKey(classLibraries)) return sharedLoaders.get(classLibraries);

        sharedLoaders.put(classLibraries, loader);
      }
    }
    return loader;
  }

  @Override
  public ClassLoader getClassLoader() {
    return loader;
  }

  /**
   * Load the operator's class using a class loader obtained from newClassLoader().
   *
   * @param loader Operator class loader.
   * @param operatorClassName Class name of the operator
   * @return The loaded class which will be an instance of com.ibm.streams.operator.Operator.
   * @throws ClassNotFoundException Class cannot be found
   * @throws IllegalArgumentException Class is not an instance of Operator
   */
  public static Class<? extends Operator> loadOperatorClass(
      ClassLoader loader, String operatorClassName) throws ClassNotFoundException {

    opTrace.info("className=" + operatorClassName);
    final Class<?> operatorClass = loader.loadClass(operatorClassName);
    if (Operator.class.isAssignableFrom(operatorClass))
      return operatorClass.asSubclass(Operator.class);
    else
      throw new IllegalArgumentException(
          new Message(
                  Key.SPL_RUNTIME_JAVA_INTERFACE_NOT_IMPLEMENT_INTERFACE,
                  operatorClass.getName(),
                  Operator.class.getName())
              .getLocalizedMessageAndPrefix());
  }

  /**
   * Create an OperatorAdapter that wraps the user's operator of the specified class name and
   * provides the context for it to execute in.
   *
   * <p>The operator's class is loaded from the specified classpath.
   *
   * @param operatorClassName ClassName of Operator implementation.
   * @param classLibraries Classpath.
   * @param setup Setup information.
   * @param executionContext How the operator interacts with SPL runtime
   * @return An OperatorAdapter implementation that can be used to fulfill streaming functionality
   *     against the user's operator.
   * @throws ClassNotFoundException
   * @throws InstantiationException
   * @throws IllegalAccessException
   * @throws MalformedURLException
   * @throws MalformedObjectNameException
   */
  public static OperatorAdapter newAdapter(
      String operatorClassName,
      List<String> classLibraries,
      OperatorSetup setup,
      ExecutionContext executionContext,
      InternalCheckpointContext ckptContext)
      throws ClassNotFoundException, InstantiationException, IllegalAccessException,
          MalformedURLException, MalformedObjectNameException {

    final ProcessingElement pe = executionContext.getPE();

    final OperatorClassLoader loader =
        OperatorAdapter.getOperatorClassLoader(classLibraries, operatorClassName, pe);
    final Class<? extends Operator> operatorClass = loadOperatorClass(loader, operatorClassName);

    Parameters setupParameters = setup.getParameters();
    String operatorName = setup.getName();

    // Index of the operator within the PE
    int operatorIndex = Integer.valueOf(setupParameters.getParameterValue(OP_PE_OPERATOR_INDEX));

    // Get the kind (from 3.0), but if running against a 2.x compiled application
    // kind will not be sent.
    String kind = setupParameters.getParameterValue(OperatorAdapter.OP_PE_OPERATOR_KIND, null);

    String operatorLogicalName =
        setupParameters.getParameterValue(OperatorAdapter.OP_LOGICAL_NAME, operatorName);

    OpInvocationInfo invokeInfo =
        new OpInvocationInfo(operatorIndex, operatorName, operatorLogicalName, kind);

    return new OperatorAdapter(
        operatorClass, loader, setup, invokeInfo, executionContext, ckptContext);
  }

  public static OperatorAdapter newAdapter(
      String operatorClassName,
      List<String> classLibraries,
      OperatorSetup setup,
      PEModel model,
      int operatorIndex,
      ExecutionContext executionContext,
      InternalCheckpointContext ckptContext)
      throws ClassNotFoundException, InstantiationException, IllegalAccessException,
          MalformedURLException, MalformedObjectNameException {

    final ProcessingElement pe = executionContext.getPE();

    final OperatorClassLoader loader =
        OperatorAdapter.getOperatorClassLoader(classLibraries, operatorClassName, pe);
    final Class<? extends Operator> operatorClass = loadOperatorClass(loader, operatorClassName);

    return new OperatorAdapter(
        operatorClass, loader, setup, model, operatorIndex, executionContext, ckptContext);
  }

  /**
   * Create an OperatorAdapter that wraps the user's operator of the specified operator class and
   * provides the context for it to execute in.
   *
   * @param operatorClass Operator class.
   * @param setup Setup information.
   * @param executionContext How the operator interacts with SPL runtime
   * @return An OperatorAdapter implementation that can be used to fulfill streaming functionality
   *     against the user's operator.
   * @throws ClassNotFoundException
   * @throws InstantiationException
   * @throws IllegalAccessException
   * @throws MalformedURLException
   * @throws MalformedObjectNameException
   */
  public static OperatorAdapter newAdapter(
      Class<? extends Operator> operatorClass,
      OperatorSetup setup,
      ExecutionContext executionContext,
      InternalCheckpointContext ckptContext)
      throws MalformedObjectNameException, InstantiationException, IllegalAccessException {

    Parameters setupParameters = setup.getParameters();
    String operatorName = setup.getName();

    // Index of the operator within the PE
    int operatorIndex = Integer.valueOf(setupParameters.getParameterValue(OP_PE_OPERATOR_INDEX));

    // Get the kind (from 3.0), but if running against a 2.x compiled application
    // kind will not be sent.
    String kind = setupParameters.getParameterValue(OperatorAdapter.OP_PE_OPERATOR_KIND, null);

    String operatorLogicalName =
        setupParameters.getParameterValue(OperatorAdapter.OP_LOGICAL_NAME, operatorName);

    OpInvocationInfo invokeInfo =
        new OpInvocationInfo(operatorIndex, operatorName, operatorLogicalName, kind);

    return new OperatorAdapter(
        operatorClass,
        new OperatorClassLoader(operatorClass.getClassLoader()),
        setup,
        invokeInfo,
        executionContext,
        ckptContext);
  }

  @SuppressWarnings("unchecked")
  private OperatorAdapter(
      final Class<? extends Operator> operatorClass,
      final OperatorClassLoader loader,
      OperatorSetup setup,
      OpInvocationInfo invokeInfo,
      ExecutionContext executionContext,
      InternalCheckpointContext ckptContext)
      throws MalformedObjectNameException, IllegalAccessException, InstantiationException {
    super(invokeInfo);

    this.operator = operatorClass.newInstance();

    this.executionContext = executionContext;
    this.ckptContext = ckptContext;
    this.operatorClassName = operatorClass.getName();

    this.loader = loader;

    // Setup ports, which requires parameters from the setup.
    inputPorts = setupInputPorts(setup);
    inputHandlers = inputPorts.isEmpty() ? null : new RuntimeStream[inputPorts.size()];
    this.outputPorts = getExecutionContext().getOutputPorts();

    Parameters setupParameters = setup.getParameters();

    addInputPortLogicState(setup, setupParameters);

    // Remove all internal parameters from visible set.
    setupParameters.removeInternal();

    this.params = setupParameters.getReadOnlyParams();

    parameterNames = Collections.unmodifiableSet(params.keySet());

    this.opMetrics =
        new OpMetrics(
            this, getExecutionContext().getMetricsFactory(), inputPorts.size(), outputPorts.size());

    mgmt = new OperatorInfo(this);

    getManagement().registerMXBeans();

    // Generate an OperatorContext object that only exposes
    // the required information. Context implementation
    // is thread safe.
    context = new Context(invokeInfo, this);

    // TODO: RESOLVE this to allow dynamic creation of threads.
    this.userThreadFactory = (OperatorThreadFactory) context.getThreadFactory();
    this.scheduler = (OperatorScheduledExecutor) context.getScheduledExecutorService();

    openInputPorts = inputPorts.size();

    catchContext = null;
  }

  @SuppressWarnings("unchecked")
  private OperatorAdapter(
      final Class<? extends Operator> operatorClass,
      final OperatorClassLoader loader,
      OperatorSetup setup,
      PEModel model,
      int operatorIndex,
      ExecutionContext executionContext,
      InternalCheckpointContext ckptContext)
      throws MalformedObjectNameException, IllegalAccessException, InstantiationException {

    super(model, operatorIndex);

    this.operator = operatorClass.newInstance();
    this.executionContext = executionContext;
    this.ckptContext = ckptContext;
    this.operatorClassName = operatorClass.getName();

    this.loader = loader;

    // Setup ports, which requires parameters from the setup.
    inputPorts = setupInputPorts(setup, model, operatorIndex);
    inputHandlers = inputPorts.isEmpty() ? null : new RuntimeStream[inputPorts.size()];
    // Check for event-time context in output ports.
    outputPorts = setupOutputPorts(setup, model, operatorIndex);

    Parameters setupParameters = setup.getParameters();

    addInputPortLogicState(setup, setupParameters);

    // Remove all internal parameters from visible set.
    setupParameters.removeInternal();

    this.params = setupParameters.getReadOnlyParams();

    parameterNames = Collections.unmodifiableSet(params.keySet());

    this.opMetrics =
        new OpMetrics(
            this, getExecutionContext().getMetricsFactory(), inputPorts.size(), outputPorts.size());

    mgmt = new OperatorInfo(this);

    // Generate an OperatorContext object that only exposes
    // the required information. Context implementation
    // is thread safe.
    context = new Context(getInvokeInfo(), this);

    // TODO: RESOLVE this to allow dynamic creation of threads.
    this.userThreadFactory = (OperatorThreadFactory) context.getThreadFactory();
    this.scheduler = (OperatorScheduledExecutor) context.getScheduledExecutorService();
    getManagement().registerMXBeans();

    // Create consistent region context if in a consistent region.
    opTrace.info("Adding consistent region");
    ConsistentRegionContextImpl.addConsistentRegion(this, model);
    opTrace.info("Added consistent region");

    CatchContext.addCatchContext(this, model);
    catchContext = context.getOptionalContext(CatchContext.class);

    // add checkpoint context to the optional context list.
    if (ckptContext != null) addOptionalContext(CheckpointContext.class, ckptContext);

    // Automatically register state handlers.
    if (operator instanceof StateHandler) registerStateHandler((StateHandler) operator);

    openInputPorts = inputPorts.size();
  }

  @Override
  public boolean isJavaOp() {
    return true;
  }

  public final Operator getOperatorInstance() {
    return operator;
  }

  public OperatorContext getContext() {
    return context;
  }

  public ExecutionContext getExecutionContext() {
    return executionContext;
  }

  public String getOperatorClassName() {
    return operatorClassName;
  }

  public OperatorThreadFactory getUserThreadFactory() {
    return userThreadFactory;
  }

  /**
   * Get the list of values for a given parameter.
   *
   * @param parameterName
   * @return
   */
  final List<String> getParameter(String parameterName) {
    List<String> values = params.get(parameterName);
    if (values == null) return Collections.emptyList();
    return Collections.unmodifiableList(values);
  }

  /**
   * Complete the setup of the operator. Calls operator.initialize(OperatorContext) on the user's
   * operator.
   *
   * @throws Exception User's initialize method threw an exception.
   */
  public final void initialize() throws Exception {

    opTrace.info("Operator.initialize() : " + operatorClassName);

    // RTC 1784
    // Always set the thread context loader to the loader for
    // user operator & third party jars as many projects expect
    // it to be able to load classes and resources from their
    // own jars.
    final Thread myThread = Thread.currentThread();
    final ClassLoader oldContextLoader = myThread.getContextClassLoader();
    final Adapter oldAdapter = SPLRuntime.setActive(this);
    myThread.setContextClassLoader(loader);
    try {
      OperatorAnnotations annotator = new OperatorAnnotations(getContext(), operator);
      annotator.applyRuntimeAnnotations(getManagement());

      operator.initialize(getContext());

      EventTimeContextImpl etc =
          (EventTimeContextImpl) (context.getOptionalContext(EventTimeContext.class));
      if (etc != null) {
        etc.initialize();

        for (StreamingOutput<OutputTuple> outputPort : outputPorts) {
          if (outputPort instanceof OperatorOutputPort) {
            ((OperatorOutputPort) outputPort).initializeWatermarkSource();
          }
        }
      }
    } catch (RuntimeException e) {
      cleanup();
      throw e;
    } catch (Exception e) {
      cleanup();
      throw e;
    } finally {
      myThread.setContextClassLoader(oldContextLoader);
      SPLRuntime.setActive(oldAdapter);
    }

    opTrace.fine("Operator.initialize() complete: " + operatorClassName);
  }

  /** Cleanup this operator after a failures. Method is idempotent. */
  public void cleanup() {
    // Remove any registered MXBeans
    getManagement().cleanup();

    userThreadFactory.shutdown();
    scheduler.shutdownNow();
  }

  /**
   * Call Operator.allPortsReady().
   *
   * @throws Exception
   */
  public final void allPortsReady() throws Exception {

    synchronized (this) {
      if (shutdown) return;
    }

    opTrace.info("Operator.allPortsReady() : " + operatorClassName);

    getManagement().notify(OperatorContextMXBean.ALL_PORTS_READY);

    // See comment on the call to Operator.initialize().
    final Thread myThread = Thread.currentThread();
    final ClassLoader oldContextLoader = myThread.getContextClassLoader();
    final Adapter oldAdapter = SPLRuntime.setActive(this);
    myThread.setContextClassLoader(loader);

    try {
      operator.allPortsReady();
    } catch (RuntimeException e) {
      cleanup();
      throw e;
    } catch (Exception e) {
      cleanup();
      throw e;
    } finally {
      myThread.setContextClassLoader(oldContextLoader);
      SPLRuntime.setActive(oldAdapter);
    }

    opTrace.fine("Operator.allPortsReady() complete: " + operatorClassName);
  }

  /**
   * Return a raw StreamHandler that delivers tuples to the users operator. This handler does not
   * implement metrics or finalization, but does implement all logic associated with the operator.
   * When tuples are delivered from C++ through JNI this handler is used as metrics and finalization
   * are handled in C++.
   *
   * @param portNumber Input port's position for Operator
   */
  public final synchronized RuntimeStream<Tuple> getInputPortHandler(final int portNumber) {
    if (inputHandlers[portNumber] != null) return inputHandlers[portNumber];

    final StreamingInput<Tuple> port = inputPorts.get(portNumber);

    // RuntimeStream that directly calls to OperatorAdapter
    // methods to deliver events onto the user's Operator.
    RuntimeStream<Tuple> irs =
        new RuntimeStream<Tuple>() {

          @Override
          public void tuple(Tuple tuple) throws Exception {
            assert tuple.asReadOnlyTuple() == tuple;
            assert !(tuple instanceof OutputTuple);

            processTuple(port, tuple);
          }

          @Override
          public void mark(Punctuation mark) throws Exception {
            processPunctuation(port, mark);
          }

          @Override
          public <A> A action(StreamAction<A> action) throws Exception {
            // Actions are not visible to the operator
            // implementation so ignore at this point.
            // (except watermark is sorta visible to the operator)
            if (action.getType() == StreamAction.Type.WATERMARK) {
              processWatermark(port, ((WatermarkAction) action).getWatermark());
            }
            return null;
          }
        };

    if (port.getStreamWindow().getType() != StreamWindow.Type.NOT_WINDOWED) {
      // Insert a processor that sends events to the window listener
      // and to the Operator.process method.
      WindowProcessor<Tuple> wp = new WindowProcessor<Tuple>(this, irs, port.getStreamWindow());

      if (needToPersistWindowState()) {
        createStateHandlerHolder().addHandler(wp.getStateHandler());
      }

      irs = wp;
    }

    ConsistentRegionContext cac = optionalContext(ConsistentRegionContext.class);
    if (cac != null) {
      ConsistentRegionContextImpl crc = (ConsistentRegionContextImpl) cac;
      irs = crc.newStreamHandler(port, irs);
    }

    // TODO if possible only create this for threaded ports
    SwitchingHandler<Tuple> switchingHandler =
        new SwitchingHandler<Tuple>(getOperatorName(), portNumber);
    irs = switchingHandler.addAlternate(irs);
    switchingHandlers.put(portNumber, switchingHandler);

    return inputHandlers[portNumber] = irs;
  }

  public final void shutdown() throws Exception {

    opTrace.info("Operator.shutdown() : " + operatorClassName);

    getManagement().notify(OperatorContextMXBean.SHUTDOWN);

    userThreadFactory.shutdown();
    scheduler.shutdown();

    synchronized (this) {
      shutdown = true;
      if (completer != null) completer.interrupt();
    }

    final Thread myThread = Thread.currentThread();
    final ClassLoader oldContextLoader = myThread.getContextClassLoader();
    final Adapter oldAdapter = SPLRuntime.setActive(this);
    myThread.setContextClassLoader(loader);
    try {
      operator.shutdown();
    } catch (RuntimeException e) {
      cleanup();
      throw e;
    } catch (Exception e) {
      cleanup();
      throw e;
    } finally {

      scheduler.shutdownNow();
      userThreadFactory.shutdownNow();
      getManagement().notify(OperatorContextMXBean.SHUTDOWN_COMPLETE);
      myThread.setContextClassLoader(oldContextLoader);
      SPLRuntime.setActive(oldAdapter);
    }

    opTrace.fine("Operator.shutdown() complete: " + operatorClassName);
  }

  /** */
  public void notifyCompleter() {
    Thread completer;
    synchronized (this) {
      completer = this.completer;
    }
    if (completer == null) return;

    synchronized (completer) {
      completerNotify = true;
      completer.notifyAll();
    }
  }

  private Throwable backgroundException;

  /**
   * State of logic clauses attached to the input port. [inputPortLogic].N={tuple,punct,none}
   * Supports multiple values when onTuple and OnPunct are setup for the port. Explicit none is used
   * to differentiate from the value not being set. This is only for the real SPL runtime so see
   * SPLRuntimeOperator.InputPortLogic enum.
   */
  public static final String OP_INPUT_PORT_LOGIC = "[inputPortLogic].";

  /**
   * Handle an exception thrown by a background activity for the operator.
   *
   * @return true if this exception was handled.
   */
  public boolean backgroundException(Thread t, Throwable e) {
    final boolean firstException;
    synchronized (this) {
      // if the completer is throwing the error
      // then no point notifying it about a background error.
      // leave it to the default handling.
      if (t == completer) return false;

      if (backgroundException == null) {
        firstException = true;
        backgroundException = e;
      } else {
        firstException = false;
      }
    }

    // Only log the exception if it is not the first one.
    // the first one will cause the completer thread to
    // terminate and it will be logged by higher levels.
    // So log subsequent exceptions to leave some trace
    // of background errors.
    if (!firstException) opTrace.log(LogLevel.ERROR, e.getLocalizedMessage(), e);

    notifyCompleter();
    return true;
  }

  /**
   * Return true when all input ports have been finalized. For a source operator (no input ports) it
   * always returns true.
   *
   * @return
   */
  private boolean inputPortProcessingComplete() {
    if (inputPorts.isEmpty()) return true;
    return inputPortsFinalized();
  }

  /** Wait for any outstanding outstanding threads or outstanding tasks. */
  public final void complete() throws Throwable {

    synchronized (this) {
      completer = Thread.currentThread();
    }

    try {
      for (; ; ) {

        checkforUncaughtBackgroundException();

        if (isShutdown()) {
          opTrace.finest("complete(): operator has shutdown");
          break;
        }

        if (!scheduler.hasActiveTasks() && !userThreadFactory.haveActiveNonDaemonThreads()) {

          opTrace.finest("complete(): no background activity");

          // No background activity and no ability
          // for the operator to start any new background
          // activity since the input ports are finalized
          // (or it's a source operator and there are no input ports).
          if (inputPortProcessingComplete()) {
            checkforUncaughtBackgroundException();
            opTrace.finest("complete(): input port processing complete");
            break;
          }
        }

        // Wait for some notification that something
        // interesting to us has terminated.
        // Shutdown will also interrupt us.
        synchronized (completer) {
          if (!completerNotify) {
            try {
              completer.wait(10000);
            } catch (InterruptedException e) {;
            }
          }
          completerNotify = false;
        }
      }

      // Debug that async activity is complete but only
      // if not shutting down and background activity was created.
      if (!isShutdown()) {
        if (scheduler.getTaskCount() != 0 || scheduler.getCompletedTaskCount() != 0)
          opTrace.fine("Operator asynchronous tasks complete: " + operatorClassName);

        if (userThreadFactory.getThreadCount() != 0) {
          opTrace.fine("Operator non-daemon threads complete: " + operatorClassName);
        }
      }

    } finally {
      synchronized (this) {
        completer = null;
      }
    }
    opTrace.finest("complete(): completed");
  }

  /**
   * Check for an uncaught background exception and throw it (from the completer thread) if one was
   * raised.
   */
  private void checkforUncaughtBackgroundException() throws Throwable {
    synchronized (this) {
      if (backgroundException != null) {
        opTrace.finest("complete(): background execption:" + backgroundException.getMessage());
        cleanup();
        throw backgroundException;
      }
    }
  }

  public final ScheduledExecutorService getScheduledExecutorService() {
    return scheduler;
  }

  public final OperatorInfo getManagement() {
    return mgmt;
  }

  private List<StreamingInput<Tuple>> setupInputPorts(OperatorSetup setup) {

    final int[] typeOrdinals = setup.getInputSet().getPortTypeOrdinals();
    final String[] names = setup.getInputSet().getPortNames();
    final Schema[] schemas = setup.getInputSet().getSchemas();

    final int portCount = names.length;
    final List<StreamingInput<Tuple>> ports = new ArrayList<StreamingInput<Tuple>>(portCount);

    for (int port = 0; port < portCount; port++) {

      StreamingInput<Tuple> inputPort =
          newStreamingInput(setup, port, typeOrdinals[port], names[port], schemas[port]);

      ports.add(inputPort);
    }
    // Since we pass lists back to the user code
    // ensure that they cannot be modified by user code.
    return Collections.unmodifiableList(ports);
  }

  private List<StreamingInput<Tuple>> setupInputPorts(
      OperatorSetup setup, PEModel model, int operatorIndex) {

    List<OperInstanceInputPortType> portsDefs =
        model.getOperatorInstance(operatorIndex).getInputPorts().getInputPort();
    final List<StreamingInput<Tuple>> ports =
        new ArrayList<StreamingInput<Tuple>>(portsDefs.size());

    // Don't assume the ports are in order of the port index.
    for (int i = 0; i < portsDefs.size(); i++) ports.add(null);

    for (OperInstanceInputPortType portDef : portsDefs) {

      final int port = portDef.getIndex().intValue();
      final String portName = portDef.getName();
      Schema schema = (Schema) model.getSchema(portDef.getTupleTypeIndex());
      Boolean ctrl = portDef.isIsControl();
      final boolean control = ctrl == null ? false : ctrl;

      List<String> windowInfo = setup.getParameters().getParameterValues(OP_WINDOW + port);

      AugmentedPEInputPortType pePort =
          model.getPEInputPort(
              model.getOperatorInstance(operatorIndex).getIndex(), portDef.getIndex());
      final int pePortNumber;
      if (pePort == null) {
        pePortNumber = AbstractStream.NOT_CONNECTED_TO_PE_PORT;
      } else {
        pePortNumber = pePort.getIndex().intValue();
      }

      ImportedStreamsType importTypeModel = pePort == null ? null : pePort.getImportedStreams();
      StreamingInput.ImportType importType;
      if (importTypeModel != null) {
        if (importTypeModel.getPropertyBasedImport() != null)
          importType = StreamingInput.ImportType.BY_SUBSCRIPTION;
        else importType = StreamingInput.ImportType.BY_NAME;
      } else {
        importType = StreamingInput.ImportType.NOT_IMPORTED;
      }

      // TODO Change importType to use enum.
      StreamingInput<Tuple> inputPort =
          new InputPort<Tuple>(
              context,
              port,
              importType.ordinal(),
              portName,
              schema,
              pePortNumber,
              windowInfo,
              control);

      ports.set(port, inputPort);

      boolean hasEventTimeContext = false;
      PhysOperInstanceType opInstance = model.getOperatorInstance(operatorIndex);
      List<AnnotationType> annotations = opInstance.getAnnotation();
      for (AnnotationType annotation : annotations) {
        if (EventTimeContextImpl.ET_CONTEXT_ANNOTATION.equals(annotation.getTag())) {
          hasEventTimeContext = true;
          break;
        }
      }
      if (hasEventTimeContext) {
        EventTimeContextImpl impl = new EventTimeContextImpl(this, model, operatorIndex, setup);
        addOptionalContext(EventTimeContext.class, impl);
      }
    }
    // Since we pass lists back to the user code
    // ensure that they cannot be modified by user code.
    return Collections.unmodifiableList(ports);
  }

  private List<StreamingOutput<OutputTuple>> setupOutputPorts(
      OperatorSetup setup, PEModel model, int operatorIndex) {
    List<StreamingOutput<OutputTuple>> outputPorts = getExecutionContext().getOutputPorts();

    return outputPorts;
  }

  /**
   * Return a StreamingInput implementation for the com.ibm.streams.operator or spade package.
   *
   * @param setup The OperatorSetup for this operator invocation.
   * @param port Port index
   * @param typeOrdinal Ordinal of the StreamingInput.InputType enum
   * @param portName Name of the port
   * @param schema Schema for tuples on the port.
   * @return A StreamingInput that will be visible to the user's operator.
   */
  private StreamingInput<Tuple> newStreamingInput(
      OperatorSetup setup, int port, int typeOrdinal, String portName, Schema schema) {
    List<String> windowInfo = setup.getParameters().getParameterValues(OP_WINDOW + port);

    int pePortNumber = getPEPort(setup, port, OP_PE_INPUT_PORT_INDEX);
    boolean isControl = getOperatorPortValue(setup, port, OP_INPUT_PORT_CONTROL);
    return new InputPort<Tuple>(
        context, port, typeOrdinal, portName, schema, pePortNumber, windowInfo, isControl);
  }

  /** get the PE port number from the parameter peInputPort{In|Out}dex.port */
  public static int getPEPort(OperatorSetup setup, int port, String pePortKey) {
    List<String> pePort = setup.getParameters().getParameterValues(pePortKey + port);
    return pePort.isEmpty()
        ? AbstractStream.NOT_CONNECTED_TO_PE_PORT
        : Integer.valueOf(pePort.get(0));
  }

  public boolean inputPortHasClause(int port, InputPortLogic... checkClauses) {
    assert port >= 0;
    assert port < getContext().getNumberOfStreamingInputs();

    List<InputPortLogic> clauses = logicClauses.get(port);
    for (InputPortLogic clause : checkClauses) if (clauses.contains(clause)) return true;
    return false;
  }

  /*
   * Get the boolean value associated to an operator input port.
   */
  private static boolean getOperatorPortValue(OperatorSetup setup, int port, String opPortKey) {
    return Boolean.parseBoolean(setup.getParameters().getParameterValue(opPortKey + port, null));
  }

  /** Determine logic state for input ports. */
  private void addInputPortLogicState(final OperatorSetup setup, final Parameters parameters) {
    final int inputPortCount = setup.getInputSet().getPortNames().length;
    for (int p = 0; p < inputPortCount; p++) {
      final String param = OperatorAdapter.OP_INPUT_PORT_LOGIC + p;
      final List<String> clauses = parameters.getParameterValues(param);
      final List<InputPortLogic> eClauses;
      if (clauses.isEmpty()) {
        // unknown state! Don't put an entry in the logicClauses map
        eClauses = InputPortLogic.UNKNOWN_CLAUSES;
      } else if (clauses.contains(InputPortLogic.none)) {
        // Common case will be none, so optimize that with a single reference.
        eClauses = InputPortLogic.NO_CLAUSES;
      } else {
        eClauses = new ArrayList<InputPortLogic>(clauses.size());
        for (String clause : clauses) {
          eClauses.add(InputPortLogic.valueOf(clause));
        }
      }
      logicClauses.put(p, eClauses);
    }
  }

  /**
   * Context object that provides the implementation for OperatorContext Implementation provides a
   * read-only view of the context the user Operator is executing in.
   */
  protected final class Context extends OpContext {

    Context(OpInvocationInfo invokeInfo, Adapter adapter) {
      super(invokeInfo, adapter);
    }

    @Override
    public List<StreamingInput<Tuple>> getStreamingInputs() {
      return inputPorts;
    }

    @Override
    public int getNumberOfStreamingInputs() {
      return inputPorts.size();
    }

    @Override
    public List<StreamingOutput<OutputTuple>> getStreamingOutputs() {
      return outputPorts;
    }

    @Override
    public int getNumberOfStreamingOutputs() {
      return outputPorts.size();
    }

    @Override
    public Set<String> getParameterNames() {
      return parameterNames;
    }

    @Override
    public List<String> getParameterValues(String parameterName) {
      return getParameter(parameterName);
    }

    @Override
    public OperatorMetrics getMetrics() {
      return opMetrics;
    }

    @Override
    public void addClassLibraries(String[] classLibraries) throws MalformedURLException {
      if (classLibraries == null) return;
      loader.addLibraries(Arrays.asList(classLibraries), getToolkitDirectory());
    }

    @Override
    public <O extends OptionalContext> O getOptionalContext(Class<O> contextType) {
      return optionalContext(contextType);
    }

    @Override
    public void registerStateHandler(StateHandler handler) {
      OperatorAdapter.this.registerStateHandler(handler);
    }
  }

  /*
   * State Handler.
   *
   * StateHandler state is driven by the C++ runtime
   * using a C++ StateHandler implementation for JavaOp.
   */

  private DelegateStateHandler<OrderedStateHandlers> stateHandler;

  /** Instance configured host name, passed as a parameter. */
  public static final String CONFIGURED_HOST_NAME = "[configuredHostName]";

  public static final String OPERATOR_GENERATED = "generated";

  public static final String OPERATOR_VMARGS = "vmArg";

  public static final String OPERATOR_CLASS_LIBRARIES = "classLibrary";

  public static final String OPERATOR_CLASS_NAME = "className";

  synchronized void registerStateHandler(StateHandler handler) {
    if (handler == null) throw new NullPointerException();

    createStateHandlerHolder().addHandler(handler);
  }

  private boolean needToPersistWindowState() {
    if (optionalContext(CheckpointContext.class) != null) return true;

    if (optionalContext(ConsistentRegionContext.class) != null) return true;

    return false;
  }

  private synchronized OrderedStateHandlers createStateHandlerHolder() {
    if (stateHandler != null) return stateHandler.getDelegateHandler();

    OrderedStateHandlers osh = new OrderedStateHandlers();

    ConsistentRegionContext crc = optionalContext(ConsistentRegionContext.class);
    if (crc != null) stateHandler = ((ConsistentRegionContextImpl) crc).wrapStateHandler(osh);
    else stateHandler = new DelegateStateHandler<OrderedStateHandlers>(osh);

    OperatorLifeCycle.closeOnShutdown(context, stateHandler);
    return stateHandler.getDelegateHandler();
  }

  private synchronized StateHandler getStateHandler() {
    return stateHandler;
  }

  public void drain() throws Exception {

    final ConsistentRegionContext cac =
        getContext().getOptionalContext(ConsistentRegionContext.class);

    // TODO - need to handle op driven.
    // if (cac != null)
    //    cac.acquirePermit();

    if (cac != null) {
      for (ThreadedPortHandler threadedHandler : threadedHandlers.values()) {
        threadedHandler.drain();
      }
    }

    final StateHandler handler = getStateHandler();
    if (handler != null) {

      Setup.opTrace.fine("Calling StateHandler.drain() for " + handler);

      final Thread myThread = Thread.currentThread();
      final ClassLoader oldContextLoader = myThread.getContextClassLoader();
      final Adapter oldAdapter = SPLRuntime.setActive(this);
      myThread.setContextClassLoader(loader);

      try {
        handler.drain();
      } finally {
        myThread.setContextClassLoader(oldContextLoader);
        SPLRuntime.setActive(oldAdapter);
      }

      Setup.opTrace.fine("Called StateHandler.drain() for " + handler);
    }

    if (cac != null) {
      // ConsistentRegionContextImpl cr = (ConsistentRegionContextImpl) cac;
      // cr.waitForAllPermits();
    }
  }

  public void reset(final CheckpointTransport transport, final long key) throws Exception {
    final ConsistentRegionContext cac =
        getContext().getOptionalContext(ConsistentRegionContext.class);
    if (cac != null) {
      for (ThreadedPortHandler threadedHandler : threadedHandlers.values()) {
        threadedHandler.reset();
      }
    }

    final StateHandler handler = getStateHandler();
    if (handler != null) {
      Setup.opTrace.fine("Calling StateHandler.reset() for " + handler);

      final Thread myThread = Thread.currentThread();
      final ClassLoader oldContextLoader = myThread.getContextClassLoader();
      final Adapter oldAdapter = SPLRuntime.setActive(this);
      myThread.setContextClassLoader(loader);

      try {
        CheckpointImpl ckptImpl = new CheckpointImpl(this, transport, key, false);
        handler.reset(ckptImpl);
        ckptImpl.close();
      } finally {
        myThread.setContextClassLoader(oldContextLoader);
        SPLRuntime.setActive(oldAdapter);
      }
      Setup.opTrace.fine("Called StateHandler.reset() for " + handler);
    }
  }

  public void resetToInitialState() throws Exception {
    final ConsistentRegionContext cac =
        getContext().getOptionalContext(ConsistentRegionContext.class);
    if (cac != null) {
      for (ThreadedPortHandler threadedHandler : threadedHandlers.values()) {
        threadedHandler.reset();
      }
    }

    final StateHandler handler = getStateHandler();
    if (handler != null) {
      Setup.opTrace.fine("Calling StateHandler.resetToInitialState() for " + handler);
      final Thread myThread = Thread.currentThread();
      final ClassLoader oldContextLoader = myThread.getContextClassLoader();
      final Adapter oldAdapter = SPLRuntime.setActive(this);
      myThread.setContextClassLoader(loader);

      try {
        handler.resetToInitialState();
      } finally {
        myThread.setContextClassLoader(oldContextLoader);
        SPLRuntime.setActive(oldAdapter);
      }
      Setup.opTrace.fine("Called StateHandler.resetToInitialState() for " + handler);
    }
  }

  public void checkpoint(final CheckpointTransport transport, final long key) throws Exception {
    final StateHandler handler = getStateHandler();
    if (handler != null) {
      Setup.opTrace.fine("Calling StateHandler.checkpoint() for " + handler);
      final Thread myThread = Thread.currentThread();
      final ClassLoader oldContextLoader = myThread.getContextClassLoader();
      final Adapter oldAdapter = SPLRuntime.setActive(this);
      myThread.setContextClassLoader(loader);

      try {
        CheckpointImpl ckptImpl = new CheckpointImpl(this, transport, key, true);
        handler.checkpoint(ckptImpl);
        ckptImpl.close();
      } finally {
        myThread.setContextClassLoader(oldContextLoader);
        SPLRuntime.setActive(oldAdapter);
      }
      Setup.opTrace.fine("Called StateHandler.checkpoint() for " + handler);
    }
  }

  public void retireCheckpoint(final long key) throws Exception {
    final StateHandler handler = getStateHandler();
    if (handler != null) {
      final Thread myThread = Thread.currentThread();
      final ClassLoader oldContextLoader = myThread.getContextClassLoader();
      final Adapter oldAdapter = SPLRuntime.setActive(this);
      myThread.setContextClassLoader(loader);

      try {
        handler.retireCheckpoint(key);
      } finally {
        myThread.setContextClassLoader(oldContextLoader);
        SPLRuntime.setActive(oldAdapter);
      }
    }
  }

  public void prepareForNonBlockingCheckpoint(long key) throws Exception {
    final StateHandler handler = getStateHandler();
    if (handler != null) {
      final Thread myThread = Thread.currentThread();
      final ClassLoader oldContextLoader = myThread.getContextClassLoader();
      final Adapter oldAdapter = SPLRuntime.setActive(this);
      myThread.setContextClassLoader(loader);

      try {
        handler.prepareForNonBlockingCheckpoint(key);
      } finally {
        myThread.setContextClassLoader(oldContextLoader);
        SPLRuntime.setActive(oldAdapter);
      }
    }
  }

  public void regionCheckpointed(long key) throws Exception {
    final StateHandler handler = getStateHandler();
    if (handler != null) {
      final Thread myThread = Thread.currentThread();
      final ClassLoader oldContextLoader = myThread.getContextClassLoader();
      final Adapter oldAdapter = SPLRuntime.setActive(this);
      myThread.setContextClassLoader(loader);

      try {
        handler.regionCheckpointed(key);
      } finally {
        myThread.setContextClassLoader(oldContextLoader);
        SPLRuntime.setActive(oldAdapter);
      }
    }
  }

  public void blockConsistentRegionPermit() throws Exception {
    final ConsistentRegionContext cac =
        getContext().getOptionalContext(ConsistentRegionContext.class);
    if (cac == null) {
      return;
    }
    ConsistentRegionContextImpl crContextImpl = (ConsistentRegionContextImpl) cac;
    crContextImpl.blockPermitAcquisition();
  }

  public void releaseConsistentRegionPermit() throws Exception {
    final ConsistentRegionContext cac =
        getContext().getOptionalContext(ConsistentRegionContext.class);
    if (cac == null) {
      return;
    }
    ConsistentRegionContextImpl crContextImpl = (ConsistentRegionContextImpl) cac;
    crContextImpl.releasePermitAcquisition();
  }

  void handleException(java.lang.Exception e, Tuple tuple, int portNumber) throws Exception {
    if (executionContext.mustRethrowException()) {
      throw e;
    }
    if (catchContext == null
        || catchContext.getExceptionKind() == CatchContext.ExceptionKind.none) {
      executionContext.setMustRethrowException();
      throw e;
    }
    if (e instanceof DataException) {
      // Will always be caught by all configurations
    } else if (e instanceof RuntimeException) {
      // If not DataException, but a RuntimeException, only
      // catch if all
      if (catchContext.getExceptionKind() != CatchContext.ExceptionKind.all) {
        executionContext.setMustRethrowException();
        throw e;
      }
    } else {
      // If not DataException and not a RuntimeException, only throw
      // if streams
      if (catchContext.getExceptionKind() == CatchContext.ExceptionKind.streams) {
        executionContext.setMustRethrowException();
        throw e;
      }
    }
    if (catchContext.mustTraceTuple()) {
      opTrace.severe("Exception in operator " + context.getName() + " in port " + portNumber);
      opTrace.severe("Processed tuple: " + tuple);
    }
    if (catchContext.mustPrintStackTrace()) {
      StringWriter sw = new StringWriter();
      PrintWriter pw = new PrintWriter(sw);
      e.printStackTrace(pw);
      opTrace.severe("Exception: " + sw.toString());
    }
  }

  /** Set the context class loader and then call Operator.process. */
  @Override
  public final void processTuple(StreamingInput<Tuple> stream, Tuple tuple) throws Exception {
    final Thread myThread = Thread.currentThread();
    final ClassLoader oldContextLoader = myThread.getContextClassLoader();
    final Adapter oldAdapter = SPLRuntime.setActive(this);
    myThread.setContextClassLoader(loader);
    try {
      operator.process(stream, tuple);
    } catch (java.lang.Exception e) {
      handleException(e, tuple, stream.getPortNumber());
    } finally {
      myThread.setContextClassLoader(oldContextLoader);
      SPLRuntime.setActive(oldAdapter);
    }
  }

  /** Tuple processing */

  /** Set the context class loader and then call Operator.processPunctuation. */
  @Override
  public final void processPunctuation(StreamingInput<Tuple> stream, Punctuation mark)
      throws Exception {
    final Thread myThread = Thread.currentThread();
    final ClassLoader oldContextLoader = myThread.getContextClassLoader();
    final Adapter oldAdapter = SPLRuntime.setActive(this);
    myThread.setContextClassLoader(loader);

    if (mark == Punctuation.FINAL_MARKER) {
      EventTimeContextImpl etc =
          (EventTimeContextImpl) (context.getOptionalContext(EventTimeContext.class));
      if (etc != null) {
        etc.finalize();
      }
    }
    try {
      operator.processPunctuation(stream, mark);
    } finally {

      if (mark == Punctuation.FINAL_MARKER) {
        synchronized (this) {
          openInputPorts--;
          if (openInputPorts == 0) this.notifyCompleter();
        }
      }
      myThread.setContextClassLoader(oldContextLoader);
      SPLRuntime.setActive(oldAdapter);
    }
  }

  public final void processWatermark(StreamingInput<Tuple> stream, Watermark wm) throws Exception {

    // If the operator is a watermark source, and
    // If the operator implements WatermarkPortHandler, hand it to
    // the operator.  Otherwise, just send it forward (which really means
    // get it back to the operator's EventTimeContext).

    final Thread myThread = Thread.currentThread();
    final ClassLoader oldContextLoader = myThread.getContextClassLoader();
    final Adapter oldAdapter = SPLRuntime.setActive(this);
    myThread.setContextClassLoader(loader);
    try {
      EventTimeContextImpl etc =
          (EventTimeContextImpl) context.getOptionalContext(EventTimeContext.class);
      if (etc != null) {
        etc.receive(wm, stream);
      } else {
        // no event time context.  We just discard this watermark.
      }
    } finally {
      myThread.setContextClassLoader(oldContextLoader);
      SPLRuntime.setActive(oldAdapter);
    }
  }

  /** Get the current watermark value. */
  public Timestamp getWatermark() {
    EventTimeContext etc = context.getOptionalContext(EventTimeContext.class);
    Timestamp ts = Watermark.ZeroTimestamp;
    if (etc != null) {
      ts = etc.getWatermark();
    }

    return ts;
  }

  /** Have the input ports been finalized. */
  protected synchronized boolean inputPortsFinalized() {
    return openInputPorts == 0;
  }

  /**
   * Register an alternate handler with the {@link SwitchingHandler}.
   *
   * @param handler the alternate handler
   * @param inputPortIndex the input port index
   * @return the decorated handler.
   */
  public RuntimeStream<Tuple> addAlternateHandler(
      RuntimeStream<Tuple> handler, int inputPortIndex) {
    assert (switchingHandlers != null);
    assert (switchingHandlers.get(inputPortIndex) != null);
    SwitchingHandler<Tuple> switchingHandler = switchingHandlers.get(inputPortIndex);
    RuntimeStream<Tuple> newHandler = switchingHandler.addAlternate(handler);
    return newHandler;
  }

  /**
   * If a handler is an instance of {@link SwitchingHandler}, return its inner handler. Otherwise,
   * just return the handler.
   *
   * @param outerHandler potential instance of {@link SwitchingHandler}.
   * @param portNumber input port index
   * @return the inner handler if found, or the unchanged outer handler.
   */
  public RuntimeStream<Tuple> undecorateHandler(RuntimeStream<Tuple> outerHandler, int portNumber) {
    RuntimeStream<Tuple> undecorated = outerHandler;
    SwitchingHandler<Tuple> switchingHandler = switchingHandlers.get(portNumber);
    if (switchingHandler != null) {
      undecorated = switchingHandler.undecorate(outerHandler);
    }
    return undecorated;
  }

  /**
   * Register an output port with the {@link SwitchingHandler} instance.
   *
   * @param inputPortIndex the input port index of the {@link SwitchingHandler}
   * @param outputOperatorIndex the operator index of the source feeding the {@link
   *     SwitchingHandler}
   * @param outputPortIndex the output port index of the source feeding the {@link SwitchingHandler}
   */
  public void addAlternateHandlerOutput(
      int inputPortIndex, int outputOperatorIndex, int outputPortIndex) {
    assert (switchingHandlers != null);
    SwitchingHandler<Tuple> switchingHandler = switchingHandlers.get(inputPortIndex);
    assert (switchingHandler != null);
    switchingHandler.addHandlerOutput(outputOperatorIndex, outputPortIndex);
  }

  /**
   * Enable switching mode on the {@link SwitchingHandler}
   *
   * @param inputPortIndex the input port index of the {@link SwitchingHandler}.
   */
  public void enableSwitching(int inputPortIndex) {
    assert (switchingHandlers != null);
    SwitchingHandler<Tuple> switchingHandler = switchingHandlers.get(inputPortIndex);
    assert (switchingHandler != null);
    switchingHandler.enableSwitch();
  }

  /**
   * Switch the active alternate on the {@link SwitchingHandler}.
   *
   * @param inputPortIndex the input port index of the {@link SwitchingHandler}.
   */
  public void setAlternate(int inputPortIndex, int alternateIndex) {
    assert (switchingHandlers != null);
    SwitchingHandler<Tuple> switchingHandler = switchingHandlers.get(inputPortIndex);
    assert (switchingHandler != null);
    switchingHandler.setAlternate(alternateIndex);
  }

  /** Add a threaded port handler to the specified port. */
  public void addThreadedHandler(ThreadedPortHandler handler, int portIndex) {
    threadedHandlers.put(portIndex, handler);
  }
}
