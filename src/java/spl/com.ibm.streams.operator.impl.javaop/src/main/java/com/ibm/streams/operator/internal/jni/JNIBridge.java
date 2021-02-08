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

package com.ibm.streams.operator.internal.jni;

import static com.ibm.streams.operator.internal.logtrace.Setup.opTrace;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.internal.compile.OperatorSource;
import com.ibm.streams.operator.internal.eventtime.Watermark;
import com.ibm.streams.operator.internal.logtrace.Log;
import com.ibm.streams.operator.internal.logtrace.Setup;
import com.ibm.streams.operator.internal.logtrace.Trace;
import com.ibm.streams.operator.internal.metrics.CustomMetric;
import com.ibm.streams.operator.internal.metrics.OperatorMetricsFactory;
import com.ibm.streams.operator.internal.network.OutputByteStream;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.ports.actions.GetExportProperties;
import com.ibm.streams.operator.internal.ports.actions.RemoveExportProperties;
import com.ibm.streams.operator.internal.ports.actions.SetExportProperties;
import com.ibm.streams.operator.internal.ports.actions.TypedAction;
import com.ibm.streams.operator.internal.runtime.api.Adapter;
import com.ibm.streams.operator.internal.runtime.api.ExecutionContext;
import com.ibm.streams.operator.internal.runtime.api.InputPortsConduit;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.OperatorSetup;
import com.ibm.streams.operator.internal.runtime.api.OutputPortsTransport;
import com.ibm.streams.operator.internal.runtime.api.Parameters;
import com.ibm.streams.operator.internal.runtime.api.SPLRuntime;
import com.ibm.streams.operator.internal.toolkits.ToolkitManagement;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.metrics.OperatorMetrics.InputPortMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics.OutputPortMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics.SystemMetric;
import com.ibm.streams.operator.state.ConsistentRegionContext.DrainResult;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.File;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Future;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.TimeUnit;
import javax.management.remote.JMXConnector;

/**
 * Bridge between the C++ operator and the Java operator using JNI as the communication mechanism.
 * Tuples are passed between the two operators using the serialized form.
 *
 * <p>The C++ code through the UBOP performs initialization as follows:
 *
 * <UL>
 *   <LI>Create an instance of this class for the operator.
 *   <LI>Call various set methods to provide the complete information about the environment to the
 *       Java operator including the input ports, the output ports and the parameters. The Java
 *       class name for the operator and the libraries are passed via the parameters.
 *   <LI>Call initialize() which completes the setup.
 * </UL>
 *
 * <p>
 */
public final class JNIBridge
    implements ExecutionContext,
        OutputPortsTransport,
        OperatorMetricsFactory<CustomMetric, JNISystemMetric>,
        JNIPointerState {

  /**
   * Limit of capacity for ByteBuffer re-use. To avoid a edge case where a number of "large"
   * ByteBuffers become cached that are never re-used due to infrequent large tuples with unbounded
   * types. Set to 128k as an arbitrary limit.
   */
  private static final int BYTE_BUFFER_REUSE_LIMIT = 128 * 1024;

  /**
   * A representation of the memory pointer to the C++ operator that is wrapping the Java operator.
   */
  private final long cppOperator;

  /**
   * Set to true when the operator has shutdown. After this point the cppOperator must no longer be
   * used as sometime later it will be deconstructed by C++.
   */
  private volatile boolean shutdown;

  /** Network byte order of the tuples. */
  private final ByteOrder tupleByteOrder;

  /** Helper object to deserialize tuples and send them onto the operator. */
  private final InputPortsConduit inputs;

  /** The object used to interact with the user's operator. */
  private final OperatorAdapter adapter;

  /** The operator's metrics. */
  private final OperatorMetrics opMetrics;

  /** Cached direct buffers - per port. */
  private final ByteBuffer[][] cachedDirectBuffers;

  private final JNIPE jniPe;

  private final List<StreamingOutput<OutputTuple>> outputPorts;

  /** Bytes of allocated direct buffers since last forced GC. */
  private static volatile int allocatedDirectBuffers;

  /** Outstanding background task for GC one per SPL Java runtime. */
  private static Future<?> gcFuture;

  private static final Object singleGC = new Object();

  /**
   * Load the C++ native methods required to support the API for Java operators and Java native
   * functions.
   *
   * <p>Use an explicit path for an installation to prevent load failures.
   */
  static {
    String path = System.getenv("STREAMS_INSTALL");
    if (path == null)
      throw new RuntimeException(
          new Message(Key.SPL_RUNTIME_STREAMS_INSTALL_NOT_SET, "").getLocalizedMessageAndPrefix());
    System.load((new File(path, "system/impl/lib/libstreams-stdtk-javaop.so")).getPath());
  }

  /**
   * Initialize the Java operator including:
   *
   * <UL>
   *   <LI>Finding and creating the instance of the user class.
   *   <LI>Setting up the complete runtime context from the static information passed in using the
   *       set methods.
   *   <LI>Calling initialize on the instance class to start it.
   * </UL>
   *
   * @throws Exception
   */
  public JNIBridge(long cppOperator, final int operatorIndex, final OperatorSetup setup)
      throws Throwable {

    try {

      this.cppOperator = cppOperator;

      // Determine the byte order of the serialized tuples.

      // For SPL we always serialize tuples using the native byte
      // buffer, thus the flag is ignored. The flag is left in case
      // the JavaOp ever writes directly to the network itself.
      //
      // BIG_ENDIAN is Java's natural encoding and all ByteBuffers
      // are created with order set to BIG_ENDIAN.
      tupleByteOrder = ByteOrder.nativeOrder();

      final Parameters parameters = setup.getParameters();
      String operatorClassName = parameters.getParameterValue(OperatorAdapter.OPERATOR_CLASS_NAME);
      parameters.remove(OperatorAdapter.OPERATOR_CLASS_NAME);

      List<String> classLibraries =
          parameters.getParameterValues(OperatorAdapter.OPERATOR_CLASS_LIBRARIES);
      parameters.remove(OperatorAdapter.OPERATOR_CLASS_LIBRARIES);

      // Remove visibility of vmArg parameters from the user code.
      parameters.remove(OperatorAdapter.OPERATOR_VMARGS);

      List<String> generated = parameters.getParameterValues(OperatorAdapter.OPERATOR_GENERATED);
      if (!generated.isEmpty() && Boolean.valueOf(generated.get(0))) {
        // Operator is generated, add in output directory/java/bin automatically
        classLibraries = new ArrayList<String>(classLibraries);
        classLibraries.add(OperatorSource.BIN_DIR);
      }
      parameters.remove(OperatorAdapter.OPERATOR_GENERATED);

      final SPLRuntime runtime = SPLRuntime.get();
      this.jniPe = JNIPE.getPE();
      PEModel peModel = runtime.getModel();

      SPLRuntimeOperator rtOp = new SPLRuntimeOperator(setup, operatorIndex, parameters, peModel);

      this.outputPorts = rtOp.setupOutputPorts(this, setup, jniPe, this);

      if (!this.outputPorts.isEmpty()) {
        cachedDirectBuffers = new ByteBuffer[this.outputPorts.size()][];
        for (int i = 0; i < cachedDirectBuffers.length; i++)
          cachedDirectBuffers[i] = new ByteBuffer[4];
      } else {
        // no output ports.
        cachedDirectBuffers = null;
      }

      // Create the OperatorAdapter to allow us to invoke
      // methods on it corresponding to actions from the Streams
      // SPL runtime and pass ourself in as a conduit to
      // handle tuples submitted from the operator (to send
      // them back to the C++ code
      boolean isCheckpointingOn = JNICheckpointBridge.isCheckpointingOn(cppOperator);
      adapter =
          OperatorAdapter.newAdapter(
              operatorClassName,
              classLibraries,
              setup,
              peModel,
              operatorIndex,
              this, // as an ExecutionContext
              isCheckpointingOn ? new JNICheckpointBridge(this) : null // as CheckpointContext
              );
      opMetrics = adapter.getContext().getMetrics();

      runtime.registerOperator(adapter);

      rtOp.setAdapter(adapter);
      jniPe.addRuntimeOperator(rtOp);

      // Create the conduit we use to de-serialize tuples
      // from the C++ serialized form into Java Tuples and
      // then send them onto the user's operator.
      inputs = new InputPortsConduit(tupleByteOrder, adapter);

      // Everything is setup, initialize the operator.
      adapter.initialize();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  /** Throw an exception if the operator has been shutdown. */
  final void checkShutdown() {
    if (shutdown)
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_OPERATOR_SHUTDOWN, "").getLocalizedMessageAndPrefix());
  }

  @Override
  public void validPointers() {
    checkShutdown();
  }

  /**
   * Return the pointer to the C++ operator
   *
   * @return The valid pointer to the C++ operator
   * @exception IllegalStateException If the operator has shutdown
   */
  final synchronized long getCppOperator() {
    checkShutdown();
    return cppOperator;
  }

  /**
   * Process a tuple from C++ to the input stream.
   *
   * @param port
   * @param rawTuple
   * @throws Exception
   */
  public void process(final int port, final ByteBuffer rawTuple) throws Throwable {
    try {
      inputs.processRawTuple(port, rawTuple);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  /**
   * Process an incoming punctuation method.
   *
   * @param port
   * @param mark
   * @throws Exception
   */
  public void process(final int port, final int mark) throws Throwable {
    try {
      inputs.processPunctuation(port, mark);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  /**
   * Process an incoming watermark
   *
   * @param port the port
   * @param secs the seconds of the watermark
   * @param nanos the nanoseconds of the watermark
   * @thows Exception an exception indicating failure
   * @since IBM Streams Version 4.3
   */
  public void processWatermark(
      final int port, final long secs, final int nanos, final int operatorId, final int oportId)
      throws Throwable {
    try {
      Watermark wm = new Watermark(secs, nanos, operatorId, oportId);
      inputs.processWatermark(port, wm);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  /**
   * Get the current watermark value
   *
   * @since IBM Streams Version 4.3
   */
  public Timestamp getWatermark() {
    return adapter.getWatermark();
  }

  private static final StreamAction.Type[] ACTIONS = StreamAction.Type.values();

  public void action(final int port, final int actionOrdinal) throws Throwable {
    final StreamAction.Type type = ACTIONS[actionOrdinal];

    final StreamAction<?> action;
    switch (type) {
      case RESUME_SUBMISSION:
        action = TypedAction.ResumeSubmission;
        break;
      case SWITCH:
        action = TypedAction.SwitchHandlers;
        break;
      default:
        // No other actions handled yet.
        return;
    }
    inputs.action(port, action);
  }

  /*
   * State Handler interfaces. C++ SPL Runtime controls
   * the state handler and the Java implementation
   * just responds to the requests.
   */

  public void drain() throws Throwable {
    try {
      adapter.drain();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void reset(final long ckptPtr, final long key) throws Throwable {
    try {
      adapter.reset(new JNICheckpointTransport(ckptPtr), key);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void checkpoint(final long ckptPtr, final long key) throws Throwable {
    try {
      adapter.checkpoint(new JNICheckpointTransport(ckptPtr), key);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void resetToInitialState() throws Throwable {
    try {
      adapter.resetToInitialState();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void retireCheckpoint(long key) throws Throwable {
    try {
      adapter.retireCheckpoint(key);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void prepareForNonBlockingCheckpoint(long key) throws Throwable {
    try {
      adapter.prepareForNonBlockingCheckpoint(key);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void regionCheckpointed(long key) throws Throwable {
    try {
      adapter.regionCheckpointed(key);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void blockConsistentRegionPermit() throws Throwable {
    try {
      adapter.blockConsistentRegionPermit();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void releaseConsistentRegionPermit() throws Throwable {
    try {
      adapter.releaseConsistentRegionPermit();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  /** Submit the tuple back to the C++ operator using the native method */
  private void submitTuple(final int port, final ByteBuffer buffer) {

    submitTuple(getCppOperator(), port, buffer, buffer.limit());

    if (buffer.capacity() <= BYTE_BUFFER_REUSE_LIMIT) {

      // Put the ByteBuffer back into the list to be re-used.
      final ByteBuffer[] buffers = cachedDirectBuffers[port];
      synchronized (buffers) {

        // Index of the smallest buffer in the reuse list
        int sbi = -1;
        for (int i = 0; i < buffers.length; i++) {
          if (buffers[i] == null) {
            buffers[i] = buffer;
            return;
          }
          if ((sbi == -1) || (buffers[i].capacity() < buffers[sbi].capacity())) sbi = i;
        }

        // If this buffer is bigger than an existing buffer
        // then swap it to allow better re-use.
        if (buffer.capacity() > buffers[sbi].capacity()) {
          buffers[sbi] = buffer;
          return;
        }
      }
    }

    // ByteBuffer will be garbage collected. If we have allocated
    // some significant amount of direct byte buffers then try
    // to force garbage collection and finalization to free
    // up the memory. This avoids Java defect:
    // 4469299: Excessive native memory growth with NIO due to finalization delay
    //
    doBackgroundGC(allocatedDirectBuffers);
  }

  /** Perform a background garbage collection if any previous one has completed. */
  private final void doBackgroundGC(int adb) {
    if (adb < 0 || adb > (16 * BYTE_BUFFER_REUSE_LIMIT)) {
      // Only have one outstanding background gc request.
      synchronized (singleGC) {
        if (gcFuture != null && !gcFuture.isDone()) return;

        // Delay for ten seconds to avoid frequent
        // gc forced requests.
        try {
          gcFuture =
              adapter
                  .getScheduledExecutorService()
                  .schedule(
                      new Runnable() {

                        @Override
                        public void run() {

                          System.gc();
                          allocatedDirectBuffers = 0;
                          Thread.yield();
                          Runtime.getRuntime().runFinalization();
                          System.gc();
                          synchronized (singleGC) {
                            gcFuture = null;
                          }
                        }
                      },
                      10,
                      TimeUnit.SECONDS);
        } catch (RejectedExecutionException e) {
          // do nothing, PE is shutting down.
        }
      }
    }
  }

  /**
   * Native method to pass an output tuple back to the C++ operator. The UBOP code registers the
   * actual method to call dynamically.
   *
   * @param objId Pointer (as a long) to the C++ operator.
   * @param port Output port the tuple is to be sent on.
   * @param buffer ByteBuffer holding the serialized tuple
   * @param len Length of the serialized data.
   */
  private static native void submitTuple(long objId, int port, ByteBuffer buffer, int len);

  private static native void punctuate(long objId, int port, int mark);

  /**
   * Set the current SPL application log level corresponding to SPLAPPTRC.
   *
   * @param level Level for SPLAPPTRC (L_INFO etc.)
   * @deprecated in InfoSphere&reg; Streams Version 4.1 TODO remove after testing binary
   *     compatibility
   */
  public static void setAppTrcLevel(int level) {
    final Trace traceLevel;
    switch (level) {
      case JNIMessageSend.L_ERROR:
        traceLevel = Trace.error;
        break;
      case JNIMessageSend.L_WARN:
        traceLevel = Trace.warn;
        break;
      case JNIMessageSend.L_INFO:
        traceLevel = Trace.info;
        break;
      case JNIMessageSend.L_DEBUG:
        traceLevel = Trace.debug;
        break;
      case JNIMessageSend.L_TRACE:
        traceLevel = Trace.trace;
        break;
      case JNIMessageSend.L_OFF:
        traceLevel = Trace.off;
        break;
      default:
        return;
    }
    String newLevel = Setup.setTraceLevel(traceLevel);
    JNIPE pe = JNIPE.getPE();
    if (pe != null) pe.traceChange(newLevel);
  }

  /**
   * This method will be notified from the C++ runtime whenever the visualization state changes. If
   * no port is being visualized in this PE then this would be sent false otherwise this be sent
   * true.
   *
   * @param visualizing true if at least one operator port in the current PE is being visualized
   * @deprecated in InfoSphere&reg; Streams Version 4.1 TODO remove after testing binary
   *     compatibility
   */
  public static void setVisualizing(final boolean visualizing) {
    final JNIPE pe = JNIPE.getPE();
    if (pe != null) pe.visualizationChange(visualizing);
  }

  /**
   * Set the current SPL application log level corresponding to SPLAPPLOG.
   *
   * @param level Level for SPLAPPLOG (L_INFO etc.)
   * @deprecated in InfoSphere&reg; Streams Version 4.1 TODO remove after testing binary
   *     compatibility
   */
  public static void setAppLogLevel(final int level) {
    final Log logLevel;
    switch (level) {
      case JNIMessageSend.L_ERROR:
        logLevel = Log.error;
        break;
      case JNIMessageSend.L_WARN:
        logLevel = Log.warn;
        break;
      case JNIMessageSend.L_INFO:
        logLevel = Log.info;
        break;
      default:
        return;
    }
    Setup.setLogLevel(logLevel);
  }

  /** @deprecated TODO remove after testing binary compatibility */
  public void setLoggingLevel(int level) {}

  /**
   * Called by the SPL PE after it starts the JVM and has set the trace level.
   *
   * @param runtimeModel XML PE model. The PE model is an element of type augmentedPEType defined by
   *     the AADL schema "http://www.ibm.com/xmlns/prod/streams/application/augmented".
   * @param params array containing additional parameters, null if no extra parameters get passed.
   * @deprecated in InfoSphere&reg; Streams Version 4.1 TODO remove after testing binary
   *     compatibility
   */
  public static void startSPLJavaRuntime(final String runtimeModel, final String[] params)
      throws Exception {
    SPLRuntime runtime = SPLRuntime.load(runtimeModel, params);
    JNIPE.createPE(runtime);
  }

  /**
   * Shutdown the SPL Java runtime,
   *
   * <p>When finalNotice is false this is being called when the shutdown of the PE is being
   * requested, prior to the operators being shutdown.
   *
   * <p>When finalNotice is true, all the Java operators will have been shutdown and the JVM will be
   * destroyed when this call returns.
   *
   * @deprecated in InfoSphere&reg; Streams Version 4.1 TODO remove after testing binary
   *     compatibility
   */
  public static void shutdownSPLJavaRuntime(boolean finalNotice) {}

  /**
   * Everything is ready to go!
   *
   * @throws Throwable
   */
  public void allPortsReady() throws Throwable {
    try {
      adapter.allPortsReady();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }
  /**
   * Wait for work started by allPortsReady to complete.
   *
   * @throws Throwable
   */
  public void complete() throws Throwable {
    try {
      resetOperatorTrackerInternal();
      adapter.complete();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  /**
   * Shutdown the operator, called from the C++ prepareToShutdown. Once this returns the C++
   * operator will subsequently be deconstructed and thus cppOperator will become invalid. To avoid
   * segmentation faults etc. we reset cppOperator to zero and subsequently throw an
   * IllegalStateExeception on any attempt to use cppOperator via getCppOperator().
   */
  public void shutdown() throws Throwable {
    synchronized (singleGC) {
      if (gcFuture != null) {
        // Cancel the GC future to avoid any delay
        // in shutdown, as it may be up to 10 seconds
        // away from being executed.
        gcFuture.cancel(false);
        gcFuture = null;
        // Try to close the window for any tuples
        // being processed to avoid kicking off a
        // forced GC.
        allocatedDirectBuffers = 0;
      }
    }
    try {
      adapter.shutdown();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    } finally {
      shutdown = true;
    }
  }

  /*
   * Execution Context methods
   */

  @Override
  public final OperatorMetricsFactory<?, ?> getMetricsFactory() {
    return this;
  }

  @Override
  public final List<StreamingOutput<OutputTuple>> getOutputPorts() {
    return outputPorts;
  }

  @Override
  public final ProcessingElement getPE() {
    return jniPe;
  }

  @Override
  public void setOperatorTracker(Adapter adapter) {
    setOperatorTracker(adapter.getOperatorIndex());
  }

  @Override
  public void resetOperatorTracker() {
    resetOperatorTrackerInternal();
  }

  @Override
  public long getConsistentRegionSequenceId() {
    return getConsistentRegionSequenceId(getCppOperator());
  }

  @Override
  public int getConsistentRegionResetAttempt() {
    return getConsistentRegionResetAttempt(getCppOperator());
  }

  @Override
  public boolean makeConsistent() {
    return makeConsistent(getCppOperator());
  }

  @Override
  public void resetConsistentRegion() throws java.io.IOException {
    resetConsistentRegion(getCppOperator());
  }

  @Override
  public void mustResetConsistentRegion() {
    mustResetConsistentRegion(getCppOperator());
  }

  @Override
  public void setMustRethrowException() {
    setMustRethrowException(getCppOperator());
  }

  @Override
  public boolean mustRethrowException() {
    return mustRethrowException(getCppOperator());
  }

  @Override
  public void enableNonBlockingCheckpoint() {
    enableNonBlockingCheckpoint(getCppOperator());
  }

  @Override
  public DrainResult makeConsistentNonBlocking() {
    return DrainResult.values()[makeConsistentNonBlocking(getCppOperator())];
  }

  @Override
  public boolean submitWatermark(Timestamp wm, int port) {
    return submitWatermark(getCppOperator(), wm.getSeconds(), wm.getNanoseconds(), port);
  }

  @Override
  public void submitWatermark(Timestamp wm) {
    submitWatermarkAll(getCppOperator(), wm.getSeconds(), wm.getNanoseconds());
  }

  /*
  @Override
  public void waitForResume() {
      waitForResume(getCppOperator());
  }
  */

  /**
   * Get the current sequence identifier for the operator.
   *
   * @param objId C++ pointer to the operator
   * @return the drain marker identifier.
   */
  private static native long getConsistentRegionSequenceId(long objId);

  /**
   * Get the reset attempt for the operator.
   *
   * @param objId C++ pointer to the operator
   * @return the reset attempt identifier.
   */
  private static native int getConsistentRegionResetAttempt(long objId);

  /**
   * Make a region consistent.
   *
   * @param objId C++ pointer to the operator
   */
  private static native boolean makeConsistent(long objId);

  /**
   * Contact the ConsistentRegionController to reset a consistent region.
   *
   * @param objId C++ pointer to operator
   */
  private static native void resetConsistentRegion(long objId) throws java.io.IOException;

  /**
   * Contact the ConsistentRegionController to indicate a reset is needed, thus a drain should not
   * proceed.
   *
   * @param objId C++ pointer to operator
   */
  private static native void mustResetConsistentRegion(long objId);

  /**
   * Wait until processing is resumed for a consistent region. Only used for start operators.
   *
   * @param objId C++ pointer to the operator
   */
  private static native void waitForResume(long objId);

  /**
   * Register the operator to do non-blocking checkpoint.
   *
   * @param objId C++ pointer to the operator
   */
  private static native void enableNonBlockingCheckpoint(long objId);

  /**
   * Make a region consistent.
   *
   * @param objId C++ pointer to the operator
   * @return DrainResult as its ordinal.
   */
  private static native int makeConsistentNonBlocking(long objId);

  /** Set the PE to rethrow exceptions */
  private static native void setMustRethrowException(long objId);

  /**
   * Check if the PE is set to rethrow exceptions
   *
   * @return true if the PE is set, false otherwise
   */
  private static native boolean mustRethrowException(long objId);

  private static native boolean submitWatermark(long objId, long secs, int nanos, int port);

  private static native void submitWatermarkAll(long objId, long secs, int nanos);

  static final ByteBuffer EMPTY_C_STRING = toCString("").asReadOnlyBuffer();

  /**
   * Convert a Java String to a C string represented in UTF-8 format with a nul terminating byte as
   * a direct ByteBuffer.
   *
   * @param value Java String to be passed to C.
   * @return ByteBuffer to be passed into C.
   */
  static ByteBuffer toCString(String value) {

    byte[] cbytes;
    try {
      cbytes = value.getBytes("UTF-8");
    } catch (UnsupportedEncodingException e) {
      // UTF-8 is mandated by the Java platform.
      return null;
    }

    ByteBuffer cstring = ByteBuffer.allocateDirect(cbytes.length + 1);
    cstring.put(cbytes);
    cstring.put((byte) 0); // C nul termination
    return cstring;
  }

  /** {@inheritDoc} */
  // @Override
  public ByteBuffer getByteBuffer(final int port, final int size) {

    ByteBuffer[] buffers = cachedDirectBuffers[port];

    synchronized (buffers) {
      if (size <= BYTE_BUFFER_REUSE_LIMIT) {

        int pbi = -1;
        for (int i = 0; i < buffers.length; i++) {
          ByteBuffer bb = buffers[i];
          if (bb == null) {
            continue;
          }
          // Optimally select a buffer of the exact size.
          if (bb.capacity() == size) {
            buffers[i] = null;
            bb.clear().limit(size);
            return bb.order(tupleByteOrder);
          }
          if (bb.capacity() < size) continue;

          // Otherwise select the smallest capacity
          // buffer that will suffice.
          if (pbi == -1) {
            pbi = i;
            continue;
          }

          if (bb.capacity() < buffers[pbi].capacity()) pbi = i;
        }
        // Use a buffer that is bigger.
        if (pbi != -1) {
          ByteBuffer bb = buffers[pbi];
          buffers[pbi] = null;
          bb.clear().limit(size);
          return bb.order(tupleByteOrder);
        }
      }
    }

    // No suitable buffer to re-use, create a new one
    allocatedDirectBuffers += size;
    return ByteBuffer.allocateDirect(size).order(tupleByteOrder);
  }

  /**
   * Custom metrics support.
   *
   * <p>Custom metrics for a JavaOp operator are created and stored in Java.
   */
  @Override
  public CustomMetric newCustomMetric(String name, String description, Metric.Kind kind) {
    addCustomMetric(getCppOperator(), toCString(name), toCString(description), kind.ordinal());
    return new CustomMetric(name, kind, description);
  }

  /** Get a custom metric from C++ that was defined in the operator model and store it in Java. */
  @Override
  public CustomMetric getCustomMetric(String name) {
    long metricPtr = getCustomMetric(getCppOperator(), toCString(name));
    CustomMetric metric =
        new CustomMetric(
            name, Metric.Kind.values()[getMetricKind(metricPtr)], getMetricDescription(metricPtr));
    metric.setValue(getMetricValue(metricPtr));
    return metric;
  }

  /**
   * Get the list of metric names from the C++ operator but only if Java does not have all the
   * current metrics, indicated by the count of the custom metrics being the same. This is used to
   * get any custom metrics from C++ that were defined in the operator model.
   */
  @Override
  public String[] getCustomMetricNames(int currentCount) {
    return getCustomMetricNames(getCppOperator(), currentCount);
  }

  /** Methods for JavaOp custom metrics access called from C++. */
  public void createCustomMetric(String name, String description, int kindOrdinal, long value)
      throws Throwable {
    try {
      opMetrics
          .createCustomMetric(name, description, Metric.Kind.values()[kindOrdinal])
          .setValue(value);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public long getCustomMetricValue(String name) throws Throwable {
    try {
      return opMetrics.getCustomMetric(name).getValue();
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public void setCustomMetricValue(String name, long value) throws Throwable {
    try {
      opMetrics.getCustomMetric(name).setValue(value);
    } catch (Throwable t) {
      opTrace.log(TraceLevel.ERROR, t.getLocalizedMessage(), t);
      throw t;
    }
  }

  public boolean hasCustomMetric(String name) throws Throwable {
    return opMetrics.getCustomMetrics().containsKey(name);
  }

  public ByteBuffer[] getCustomMetricNames() throws Throwable {
    // Get a stable list of the metric names as the
    // returned map points to the live set of metric and thus
    // can change while we are trying to iterate across it.
    String[] names = opMetrics.getCustomMetrics().keySet().toArray(new String[0]);
    ByteBuffer[] nameBuffers = new ByteBuffer[names.length];
    int i = 0;
    for (String name : names) nameBuffers[i++] = toCString(name);
    return nameBuffers;
  }

  /** Get a Metric for a port. */
  /*
  @Override
  public JNISystemMetric getMetric(int port, Enum<?> name) {
      boolean output =
          name instanceof OperatorMetrics.OutputPortMetric ||
          name instanceof PEMetrics.OutputPortMetric;
      boolean pe =
          name instanceof PEMetrics.InputPortMetric ||
          name instanceof PEMetrics.OutputPortMetric;
      long metricPtr = getPortMetric(getCppOperator(), port, pe, output, name.ordinal());
      return new JNISystemMetric(this, metricPtr, getMetricName(metricPtr),
              Metric.Kind.values()[getMetricKind(metricPtr)],
              getMetricDescription(metricPtr));
  }
  */
  @Override
  public JNISystemMetric getInputMetric(int port, InputPortMetric name) {
    final long metricPtr = getPortMetric(getCppOperator(), port, false, false, name.ordinal());
    return new JNISystemMetric(
        this,
        metricPtr,
        getMetricName(metricPtr),
        Metric.Kind.values()[getMetricKind(metricPtr)],
        getMetricDescription(metricPtr));
  }

  public JNISystemMetric getOutputMetric(int port, OutputPortMetric name) {
    final long metricPtr = getPortMetric(getCppOperator(), port, false, true, name.ordinal());
    return new JNISystemMetric(
        this,
        metricPtr,
        getMetricName(metricPtr),
        Metric.Kind.values()[getMetricKind(metricPtr)],
        getMetricDescription(metricPtr));
  };

  public JNISystemMetric getOperatorMetric(SystemMetric name) {
    final long metricPtr = getOperatorMetricInternal(getCppOperator(), name.ordinal());
    return new JNISystemMetric(
        this,
        metricPtr,
        getMetricName(metricPtr),
        Metric.Kind.values()[getMetricKind(metricPtr)],
        getMetricDescription(metricPtr));
  };

  // @Override
  public byte[] getStreamProperties(int port) {
    return getStreamProperties(getCppOperator(), port);
  }

  /**
   * Ask the C++ JavaOp to fill-in the properties for an output port. A serialized representation of
   * the properties is returned, see OutputPortsConduit.getStreamProperties for details of the
   * format.
   */
  private static native byte[] getStreamProperties(long objId, int port);

  private void removeStreamProperties(int port, String... names) {

    ByteBuffer[] rawNames = new ByteBuffer[names.length];
    int i = 0;
    for (String name : names) rawNames[i++] = toCString(name);
    removeStreamProperties(getCppOperator(), port, rawNames);
  }

  private static native void removeStreamProperties(long objId, int port, ByteBuffer[] rawNames);

  private void setStreamProperties(int port, ByteBuffer serialzedProperties, boolean set) {

    setStreamProperties(
        getCppOperator(), port, set, serialzedProperties, serialzedProperties.limit());
  }

  /**
   * Load a class from a toolkit classloader.
   *
   * @param rootDirectory Root directory of the toolkit, absolute path.
   * @param classLibrary Path to the class library, relative to rootDirectory.
   * @param className Class to be loaded.
   * @see ToolkitManagement#loadToolkitClass(String, String, String, String, String)
   */
  public static Class<?> loadToolkitClass(
      String rootDirectory, String classLibrary, String className) throws Exception {
    return ToolkitManagement.loadToolkitClass(rootDirectory, classLibrary, className);
  }

  /**
   * @param objId C++ pointer to operator
   * @param port Output port index
   * @param count Number stream properties
   * @param set True to set, false to add.
   */
  private static native void setStreamProperties(
      long objId, int port, boolean set, ByteBuffer serializedProperties, int size);

  /**
   * Call through to the C++ layer to get the value of a metric.
   *
   * @param pointer Pointer to the SPL::Metric
   * @return Value of the metric.
   */
  static native long getMetricValue(long pointer);

  /**
   * Call through to the C++ layer to get the name of a metric.
   *
   * @param pointer Pointer to the SPL::Metric
   * @return Name of the metric.
   */
  static native String getMetricName(long pointer);

  /**
   * Call through to the C++ layer to get the description of a metric.
   *
   * @param pointer Pointer to the SPL::Metric
   * @return Description of the metric.
   */
  static native String getMetricDescription(long pointer);

  /**
   * Call through to the C++ layer to get the kind of a metric.
   *
   * @param pointer Pointer to the SPL::Metric
   * @return Kind of the metric as its ordinal.
   */
  static native int getMetricKind(long pointer);

  /**
   * Call through to the C++ layer to change the value of a metric.
   *
   * @param pointer Pointer to the SPL::Metric
   * @param value Value to increment the metric by.
   */
  static native void bumpSystemMetricValue(long pointer);

  static native void decrementSystemMetricValue(long pointer);

  static native void setSystemMetricValue(long pointer, long value);

  static native void setMaxSystemMetricValue(long pointer, long value);

  /**
   * Register a new custom metric with the platform.
   *
   * @param objId Operator's C++ pointer.
   * @param name The metric's name.
   * @param description The metric's description.
   * @param kind The metric's kind.
   */
  private static native void addCustomMetric(
      long objId, ByteBuffer name, ByteBuffer description, int kind);

  /**
   * Get a custom metric defined in the operator model.
   *
   * @return The C++ pointer to the SPL::Metric
   */
  private static native long getCustomMetric(long objId, ByteBuffer name);

  /**
   * Get the C++ pointer to a port's SPL::Metric
   *
   * @param objId Operator's C++ pointer.
   * @param port Index of port.
   * @param pe True if processing element port, false if operator.
   * @param output True if output port, false if input..
   * @param metricOrdinal Ordinal of the C++ enum.
   * @return The C++ pointer to the SPL::Metric
   */
  static native long getPortMetric(
      long objId, int port, boolean pe, boolean output, int metricOrdinal);

  /**
   * Get the C++ pointer to a port's SPL::Metric
   *
   * @param objId Operator's C++ pointer.
   * @param port Index of port.
   * @param pe True if processing element port, false if operator.
   * @param output True if output port, false if input..
   * @param metricOrdinal Ordinal of the C++ enum.
   * @return The C++ pointer to the SPL::Metric
   */
  static native long getOperatorMetricInternal(long objId, int metricOrdinal);

  /**
   * Get the list of custom metric names from the C++ operator if the count of metrics in Java is
   * different to that in C++. If the count is the same then Java has the same set of metrics.
   *
   * @param objId Operator's C++ pointer.
   * @param currentCount Current number of custom metrics for Java.
   * @return Null if no update needed, otherwise complete list of the names.
   */
  private static native String[] getCustomMetricNames(long objId, int currentCount);

  /**
   * Set the C++ Operator Tracker thread local for a thread started in Java.
   *
   * @param operatorIndex Operator index in the PE.
   */
  static native void setOperatorTracker(int operatorIndex);

  /** Reset the C++ Operator Tracker thread local for a thread started in Java. */
  static native void resetOperatorTrackerInternal();

  /**
   * Get the C++ Operator Tracker thread local
   *
   * @return Current operator index.
   */
  static native int getOperatorTracker();

  /** Wait for shutdown. */
  static native void waitForShutdown();

  /**
   * Get a session which clients can use to access Platform services.
   *
   * @return the Platform session id.
   */
  static native String getUserSession();

  /**
   * Get the current job user's account name.
   *
   * @return the name of the user running the current job.
   */
  static native String getUserName();

  static String[] getCredentials() {
    return new String[] {getUserName(), "aastoken:" + getUserSession()};
  }

  @Override
  public String[] getDomainCredentials() {
    return getCredentials();
  }

  public static void getJMXCredentials(final Map<String, Object> env) {

    String[] creds = getCredentials();

    env.put(JMXConnector.CREDENTIALS, creds);
    env.put("jmx.remote.protocol.provider.pkgs", "com.ibm.streams.management");
  }

  /** Get a Stream handler that delivers encoded tuples to their output destination through JNI. */
  @Override
  public OutputByteStream getOutputStream(final int port) {
    return new OutputByteStream() {

      @Override
      public <A> A action(StreamAction<A> action) throws Exception {
        switch (action.getType()) {
          case REMOVE_EXPORT_PROPERTIES:
            {
              RemoveExportProperties rep = (RemoveExportProperties) action;
              removeStreamProperties(port, rep.getNames().toArray(new String[0]));
              break;
            }
          case SET_EXPORT_PROPERTIES:
            {
              SetExportProperties sep = (SetExportProperties) action;
              setStreamProperties(port, sep.serializeProperties(), sep.isSet());
              break;
            }
          case GET_EXPORT_PROPERTIES:
            {
              GetExportProperties gep = (GetExportProperties) action;
              gep.deserializeProperties(getStreamProperties(getCppOperator(), port));
              break;
            }

          case SWITCH:
            {
              punctuate(getCppOperator(), port, OperatorAdapter.SWITCH_PUNCT_VALUE);
              break;
            }
          case DRAIN:
          case RESET:
          case RESUME_SUBMISSION:
          default:
            throw new UnsupportedOperationException(action.getType().name());
        }
        return action.call();
      }

      @Override
      public void tuple(ByteBuffer tuple) throws Exception {
        submitTuple(port, tuple);
      }

      @Override
      public void mark(Punctuation mark) throws Exception {
        punctuate(getCppOperator(), port, mark.ordinal());
      }

      @Override
      public ByteBuffer getByteBuffer(int size) {
        return JNIBridge.this.getByteBuffer(port, size);
      }
    };
  }
}
