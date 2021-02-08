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

import com.ibm.streams.flow.declare.InputPortDeclaration.CongestionPolicy;
import com.ibm.streams.flow.handlers.StreamHandler;
import com.ibm.streams.flow.internal.Connections;
import com.ibm.streams.flow.internal.Connections.Connection;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.StreamingOutput.ExportType;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.metrics.RuntimeMetric.MetricValue;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.ports.DirectTuple;
import com.ibm.streams.operator.internal.ports.PortFactory;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.RuntimeStreamDynamicForwarder;
import com.ibm.streams.operator.internal.ports.RuntimeStreamForwarder;
import com.ibm.streams.operator.internal.ports.StreamCollection;
import com.ibm.streams.operator.internal.ports.ThreadedPortHandler;
import com.ibm.streams.operator.internal.ports.actions.TypedAction;
import com.ibm.streams.operator.internal.runtime.AbstractStream;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.internal.runtime.api.ExecutionContext;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.OperatorSetup;
import com.ibm.streams.operator.internal.runtime.api.OutputPortsTransport;
import com.ibm.streams.operator.internal.runtime.api.Parameters;
import com.ibm.streams.operator.internal.state.ConsistentRegionContextImpl;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.management.OperatorManagement;
import com.ibm.streams.operator.management.ProcessingElementMXBean;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.metrics.OperatorMetrics.InputPortMetric;
import com.ibm.streams.operator.metrics.OperatorMetrics.OutputPortMetric;
import com.ibm.streams.platform.services.AnnotationAttributeValueType;
import com.ibm.streams.platform.services.AnnotationTupleValueType;
import com.ibm.streams.platform.services.AnnotationType;
import com.ibm.streams.platform.services.AugmentedPEOutputPortType;
import com.ibm.streams.platform.services.CongestionPolicyType;
import com.ibm.streams.platform.services.OperDefinitionType;
import com.ibm.streams.platform.services.OperInstanceIncomingConnectionType;
import com.ibm.streams.platform.services.OperInstanceInputPortType;
import com.ibm.streams.platform.services.OperInstanceInputPortsType;
import com.ibm.streams.platform.services.OperInstanceOutgoingConnectionType;
import com.ibm.streams.platform.services.OperInstanceOutputPortType;
import com.ibm.streams.platform.services.OperInstanceOutputPortsType;
import com.ibm.streams.platform.services.OperatorRuntimeType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.streams.platform.services.SplitterType;
import com.ibm.streams.platform.services.ThreadedPortType;
import java.math.BigInteger;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.management.Notification;
import javax.management.NotificationEmitter;
import javax.management.NotificationFilterSupport;
import javax.management.NotificationListener;

/**
 * Runtime information about the operator executing in an SPL runtime. This is used to manage fusing
 * of Java operators while we have limited info about the full topology.
 */
public class SPLRuntimeOperator {

  private static Logger trace = Logger.getLogger(SPLRuntimeOperator.class.getName());

  // Just in case we have an issue, allow Cpp to be forced.
  private static boolean forceCPP;
  private static final String FORCE_CPP = "com.ibm.streams.operator.internal.fuse.forcecpp";
  public static final String JAVAFUSING_ENABLE_LOG = "com.ibm.streams.operator.internal.fuse.info";

  static {
    // Disable by default - since this is just internal tracing.
    trace.setLevel(Boolean.getBoolean(JAVAFUSING_ENABLE_LOG) ? TraceLevel.INFO : TraceLevel.OFF);
    forceCPP = Boolean.getBoolean(FORCE_CPP);
  }

  public static enum SPLPortType {
    /** Tuple serialized and passed to C++ through JNI(the original) */
    CPP,
    /** Tuple is passed by reference to a connected Java input. */
    JREF_CPP;

    public String reason;
  }

  private final int operatorIndex;

  /**
   * Maintains the state of connections from {@link OperatorAdapter#OP_INTRA_PE_CONNS} just for this
   * operator.
   */
  private final Connections connectionInfo = new Connections();

  /** Output port types. */
  private final SPLOutputPort[] outputPorts;

  private OperatorAdapter adapter;
  private PEModel peModel;

  /** */
  private Map<Integer, RuntimeStream<Tuple>> fusedInputs;

  private Map<Integer, TupleVisualizationHandler> tupleVisualizers; // for each fused output port
  private Map<Integer, Schema> outputPortSchemae = new HashMap<Integer, Schema>();

  // The possible states of the switching handler.
  @SuppressWarnings("unused")
  private static final int cppHandlerState = 0;

  private static final int javaHandlerState = 1;

  public SPLRuntimeOperator(
      final OperatorSetup setup,
      final int operatorIndex,
      final Parameters parameters,
      final PEModel model) {
    this.operatorIndex = operatorIndex;
    this.peModel = model;

    outputPorts = new SPLOutputPort[setup.getOutputSet().getPortNames().length];
    for (int p = 0; p < outputPorts.length; p++) outputPorts[p] = new SPLOutputPort();
    addConnectionInfo(setup, parameters);

    determineFusability(parameters);
  }

  /**
   * Set the output port objects in the adapter that support serializing the tuples into the passed
   * in conduit (transport). To support direct fusing of Java operators we support two types of
   * ports (enum SPLPortType). CPP) Tuple serialized and passed to C++ through JNI(the original).
   * JREF_CPP) Tuple is passed by reference to a connected Java input.
   *
   * <p>CPP) Port is always used when: F1) System properties FORCE_CPP set to true S1) Standalone
   * and trace level is DEBUG or trace C1) Output port is connected to a PE port C2) Output port is
   * exported (which I think will also be a PE port) C3) Debugging is enabled C4) Profiling is
   * enabled L1) Output port is connected to input port of operator with logic clause. U1) Output
   * port is connected to one or more C++ input ports (mixed ports not supported) U2) Output port is
   * connected to one or more UDP splitters. U3) Output port is connected to one or more Java input
   * ports with a threaded port that is also connected to a PE input port or C++ output port.
   */
  private void determineFusability(Parameters parameters) {
    final PhysOperInstanceType opInst = peModel.getOperatorInstance(operatorIndex);
    final List<OperInstanceOutputPortType> portsDefs = opInst.getOutputPorts().getOutputPort();
    for (OperInstanceOutputPortType portModel : portsDefs) {

      final int port = portModel.getIndex().intValue();
      final String portName = portModel.getName();

      final AugmentedPEOutputPortType pePort =
          peModel.getPEOutputPort(opInst.getIndex(), portModel.getIndex());

      final int pePortNumber =
          pePort == null ? AbstractStream.NOT_CONNECTED_TO_PE_PORT : pePort.getIndex().intValue();

      final ExportType exportType;
      if (pePort != null && pePort.getExportedStream() != null) {
        if (pePort.getExportedStream().getProperties() != null)
          exportType = ExportType.BY_PROPERTIES;
        else exportType = ExportType.BY_NAME;
      } else {
        exportType = ExportType.NOT_EXPORTED;
      }

      final SPLPortType splPortType;
      final String reason;
      // F1, S1, C1, C2, C3, C4, U1, U2, U3, L1
      if (forceCPP) {
        // F1
        reason = "ForceCPP";
        splPortType = SPLPortType.CPP;
      } else if (isStandaloneCPPAndTracingIsDebugLevel()) {
        // S1
        reason = "StandaloneCPP";
        splPortType = SPLPortType.CPP;
      } else if (pePortNumber != AbstractStream.NOT_CONNECTED_TO_PE_PORT) {
        // C1
        reason = "Connected to PE port";
        splPortType = SPLPortType.CPP;
      } else if (exportType != ExportType.NOT_EXPORTED) {
        // C2
        reason = "Exported";
        splPortType = SPLPortType.CPP;
      } else if (isDebugging(parameters)) {
        // C3
        reason = "Debugging";
        splPortType = SPLPortType.CPP;
      } else if (isProfiling(parameters)) {
        // C4
        reason = "Profiling";
        splPortType = SPLPortType.CPP;
      } else if (isConnectedToCPPPort(portModel)) {
        // U1
        reason = "Connected to C++ port";
        splPortType = SPLPortType.CPP;

      } else if (isConnectedToUDPSplitter(portModel)) {
        // U2
        reason = "Connected to UDP Splitter";
        splPortType = SPLPortType.CPP;

      } else if (!connectedThreadedInputPortsConnectsToOnlyFusableJavaOutputPorts(portModel)) {
        // Only allow java threaded port if all outputs
        // connecting to the threaded port are fusable
        // java connections.
        // U3
        reason = "Connected to threaded port with PE input or C++ output";
        splPortType = SPLPortType.CPP;

      } else if (hasInputPortLogic(portModel)) {
        // L1
        reason = "Connected to input port with logic";
        splPortType = SPLPortType.CPP;
      } else {
        reason = "Fused Java";
        splPortType = SPLPortType.JREF_CPP;
      }
      // This is internal tracing only.
      // We trace at info so we can debug situations
      // as when trace is debug and standalone we will always be C++.
      if (trace.isLoggable(Level.INFO))
        trace.info(
            MessageFormat.format(
                "JAVAFUSING: Op: {0} Output port: {1} ({2}) initial fused port selection: {3}",
                getOperatorIndex(), portName, port, reason));

      splPortType.reason = reason;
      outputPorts[port].setPortType(splPortType);
    }
  }

  private boolean connectedThreadedInputPortsConnectsToOnlyFusableJavaOutputPorts(
      OperInstanceOutputPortType outputPort) {
    boolean allOutputsAreFusableJava = true;

    List<OperInstanceOutgoingConnectionType> intraPeConnections = null;

    if (allOutputsAreFusableJava && outputPort != null) {
      intraPeConnections = outputPort.getIntraPeConnection();
    }
    if (intraPeConnections != null) {

      for (OperInstanceOutgoingConnectionType intraPeConnection : intraPeConnections) {
        ThreadedPortType threadedPort = null;
        BigInteger operInstanceIndex = null;
        BigInteger iPortIndex = null;

        OperInstanceInputPortType inputPort = null;
        PhysOperInstanceType physOperInstance = null;

        iPortIndex = intraPeConnection.getIportIndex();
        operInstanceIndex = intraPeConnection.getOperInstanceIndex();
        physOperInstance = findOperator(operInstanceIndex);

        inputPort = getInputPort(physOperInstance, iPortIndex.intValue());
        if (inputPort != null) {
          threadedPort = inputPort.getThreadedPort();
        }
        if (threadedPort != null) {
          // Now test whether C++ output ports are connected to the threaded port.
          allOutputsAreFusableJava = inputPortConnectsToOnlyFusableJavaOutputPorts(inputPort);
        }
        if (!allOutputsAreFusableJava) {
          break;
        }
      }
    }
    return allOutputsAreFusableJava;
  }

  private boolean inputPortConnectsToOnlyFusableJavaOutputPorts(
      OperInstanceInputPortType inputPort) {
    boolean onlyFusableJava = true;
    // If there are any inter-PE connections this test fails
    List<OperInstanceIncomingConnectionType> interPeConnections = inputPort.getInterPeConnection();
    if (interPeConnections != null && !interPeConnections.isEmpty()) {
      onlyFusableJava = false;
    }

    // If any intra-PE connections are not java, this test fails.
    if (onlyFusableJava) {
      List<OperInstanceIncomingConnectionType> connections = inputPort.getIntraPeConnection();
      for (OperInstanceIncomingConnectionType incomingConnection : connections) {
        BigInteger operIndex = incomingConnection.getOperInstanceIndex();
        PhysOperInstanceType physOperator = null;
        if (operIndex != null) {
          physOperator = findOperator(operIndex);
        }
        if (physOperator != null) {
          onlyFusableJava = isJavaOp(physOperator);
        }
        if (!onlyFusableJava) {
          break;
        }
      }
    }
    return onlyFusableJava;
  }

  /**
   * Determine whether any input port connected to the specified output port has any type of input
   * port logic.
   */
  private boolean hasInputPortLogic(OperInstanceOutputPortType portModel) {
    boolean inputPortLogic = false;
    List<OperInstanceOutgoingConnectionType> connectionList = null;
    if (portModel != null) {
      connectionList = portModel.getIntraPeConnection();
    }
    if (connectionList != null) {
      for (OperInstanceOutgoingConnectionType connection : connectionList) {
        if (hasInputPortLogic(connection)) {
          inputPortLogic = true;
          break;
        }
      }
    }

    return inputPortLogic;
  }

  /**
   * Determine whether the input port connected by the specified connection has any type of input
   * port logic.
   */
  private boolean hasInputPortLogic(OperInstanceOutgoingConnectionType connection) {
    boolean hasLogic = false;
    PhysOperInstanceType operator = null;
    BigInteger iportIndex = null;
    BigInteger operatorIndex = null;
    List<AnnotationType> annotationList = null;
    AnnotationTupleValueType logicClauseAnnotation = null;

    if (connection != null) {
      iportIndex = connection.getIportIndex();
      operatorIndex = connection.getOperInstanceIndex();
    }
    if (iportIndex != null && operatorIndex != null) {
      operator = findOperator(operatorIndex);
    }
    if (operator != null) {
      annotationList = operator.getAnnotation();
    }
    if (annotationList != null) {
      logicClauseAnnotation = findLogicClauseAnnotation(annotationList);
    }
    if (logicClauseAnnotation != null) {
      for (AnnotationAttributeValueType value : logicClauseAnnotation.getAttr()) {
        String key = value.getKey();
        if (key.equals("hasOnTuple") || key.equals("hasOnPunct")) {
          if (value.getValue().equals("true")) {
            hasLogic = true;
            break;
          }
        }
      }
    }
    return hasLogic;
  }

  private static AnnotationTupleValueType findLogicClauseAnnotation(
      List<AnnotationType> annotationList) {
    AnnotationTupleValueType logicClauseAnnotation = null;
    if (annotationList != null) {
      for (AnnotationType annotation : annotationList) {
        String tag = annotation.getTag();
        if (tag != null && tag.equals("logicClause")) {
          logicClauseAnnotation = annotation.getTuple();
          break;
        }
      }
    }
    return logicClauseAnnotation;
  }

  private ThreadedPortType getThreadedPort(Connection connection) {
    ThreadedPortType threadedPort = null;
    int operInstanceIndex;
    int iPortIndex;

    OperInstanceInputPortType inputPort = null;
    PhysOperInstanceType physOperInstance = null;

    iPortIndex = connection.getInputPortIndex();

    operInstanceIndex = connection.getInputOperatorIndex();

    physOperInstance = peModel.getOperatorInstance(operInstanceIndex);

    inputPort = getInputPort(physOperInstance, iPortIndex);
    if (inputPort != null) {
      threadedPort = inputPort.getThreadedPort();
    }
    return threadedPort;
  }

  private OperInstanceInputPortType getInputPort(
      PhysOperInstanceType physOperInstance, int iPortIndex) {
    OperInstanceInputPortsType inputPorts = null;
    List<OperInstanceInputPortType> inputPortsList = null;
    OperInstanceInputPortType inputPort = null;

    if (physOperInstance != null) {
      inputPorts = physOperInstance.getInputPorts();
    }
    if (inputPorts != null) {
      inputPortsList = inputPorts.getInputPort();
    }
    if (inputPortsList != null) {
      inputPort = inputPortsList.get(iPortIndex);
    }

    return inputPort;
  }

  /** Find the operator with the specified operator index. */
  private PhysOperInstanceType findOperator(BigInteger operInstanceIndex) {
    int operatorCount = peModel.getOperatorCount();
    int definitionIndex = 0;
    PhysOperInstanceType physOperInstance = null;
    while (physOperInstance == null && definitionIndex < operatorCount) {
      physOperInstance = peModel.getOperatorInstance(definitionIndex);
      BigInteger logicalIndex = physOperInstance.getLogicalIndex();
      if (!logicalIndex.equals(operInstanceIndex)) {
        physOperInstance = null;
        ++definitionIndex;
      }
    }
    return physOperInstance;
  }

  /**
   * Determine whether the specified output port connects to a C++ input port.
   *
   * @return true if there is at least one connection to a C++ port
   */
  private boolean isConnectedToCPPPort(OperInstanceOutputPortType portModel) {
    boolean connectedToCPPPort = false;
    List<OperInstanceOutgoingConnectionType> connectionList = null;
    if (portModel != null) {
      connectionList = portModel.getIntraPeConnection();
    }
    if (connectionList != null) {
      for (OperInstanceOutgoingConnectionType connection : connectionList) {
        if (isConnectionToCPPPort(connection)) {
          connectedToCPPPort = true;
          break;
        }
      }
    }
    return connectedToCPPPort;
  }

  /**
   * Determine whether the specified output port connects to a UDP splitter.
   *
   * @return true if there is at least one connection to a UDP splitter
   */
  private boolean isConnectedToUDPSplitter(OperInstanceOutputPortType portModel) {
    boolean connectedToUDPSplitter = false;
    List<SplitterType> splitters = null;
    if (portModel != null) {
      splitters = portModel.getSplitter();
    }
    if (splitters != null && !splitters.isEmpty()) {
      connectedToUDPSplitter = true;
    }
    return connectedToUDPSplitter;
  }

  /**
   * Determine whether the input port of the specified connection is the input port of a C++
   * operator.
   */
  private boolean isConnectionToCPPPort(OperInstanceOutgoingConnectionType connection) {
    boolean connectedToCPPPort = true;
    BigInteger iportIndex = null;
    BigInteger operInstance = null;
    PhysOperInstanceType physOperator = null;

    iportIndex = connection.getIportIndex();
    operInstance = connection.getOperInstanceIndex();

    if (iportIndex != null && operInstance != null) {
      physOperator = findOperator(operInstance);
    }
    if (physOperator != null) {
      connectedToCPPPort = !isJavaOp(physOperator);
    }

    return connectedToCPPPort;
  }

  /** Determine whether the specified operator is a Java operator. */
  private boolean isJavaOp(PhysOperInstanceType physOperator) {
    boolean javaOp = false;

    OperDefinitionType operatorDefinition = null;
    OperatorRuntimeType runtimeType = null;
    assert (physOperator != null);

    operatorDefinition = peModel.getOperatorDefinition(physOperator);
    if (operatorDefinition != null) {
      runtimeType = operatorDefinition.getRuntimeType();
    }
    if (runtimeType != null) {
      javaOp = (runtimeType == OperatorRuntimeType.SPL_JAVA);
    }

    return javaOp;
  }

  /** Determine whether profiling is enabled. */
  private static boolean isProfiling(Parameters parameters) {
    boolean profiling = false;
    List<String> profilingParams = parameters.getParameterValues(OperatorAdapter.OP_PROFILING);
    profiling = !profilingParams.isEmpty() && Boolean.valueOf(profilingParams.get(0));
    return profiling;
  }
  /** Determine whether debugging is enabled. */
  private static boolean isDebugging(Parameters parameters) {
    boolean debugging = false;
    List<String> debugParams = parameters.getParameterValues(OperatorAdapter.OP_PE_DEBUG);
    assert (debugParams != null);
    debugging = !debugParams.isEmpty() && Boolean.valueOf(debugParams.get(0));

    return debugging;
  }

  int getOperatorIndex() {
    return operatorIndex;
  }

  /**
   * Get the connection information that is passed in (originally from the AADL for the PE). TODO:
   * get intraPE connections from the Java PE model and remove the operator parameter
   * OperatorAdapter.OP_INTRA_PE_CONNS + port
   */
  void addConnectionInfo(final OperatorSetup setup, final Parameters parameters) {

    final int portCount = setup.getOutputSet().getPortNames().length;

    for (int port = 0; port < portCount; port++) {
      String param = OperatorAdapter.OP_INTRA_PE_CONNS + port;
      connectionInfo.addConnections(getOperatorIndex(), port, parameters.getParameterValues(param));
    }
  }

  List<StreamingOutput<OutputTuple>> setupOutputPorts(
      final ExecutionContext ec,
      final OperatorSetup setup,
      JNIPE pe,
      OutputPortsTransport transport) {

    final PhysOperInstanceType opInst = peModel.getOperatorInstance(operatorIndex);

    final boolean inConsistentRegion =
        ConsistentRegionContextImpl.inConsistentRegion(operatorIndex, peModel);

    final List<OperInstanceOutputPortType> portsDefs = opInst.getOutputPorts().getOutputPort();
    final int portCount = portsDefs.size();

    final List<StreamingOutput<OutputTuple>> ports =
        new ArrayList<StreamingOutput<OutputTuple>>(portCount);
    for (int i = 0; i < portCount; i++) ports.add(null);

    for (OperInstanceOutputPortType portModel : portsDefs) {

      final int port = portModel.getIndex().intValue();

      final String portName = portModel.getName();
      final Schema portSchema = (Schema) peModel.getSchema(portModel.getTupleTypeIndex());
      outputPortSchemae.put(port, portSchema);

      final AugmentedPEOutputPortType pePort =
          peModel.getPEOutputPort(opInst.getIndex(), portModel.getIndex());

      final int pePortNumber =
          pePort == null ? AbstractStream.NOT_CONNECTED_TO_PE_PORT : pePort.getIndex().intValue();

      final ExportType exportType;
      if (pePort != null && pePort.getExportedStream() != null) {
        if (pePort.getExportedStream().getProperties() != null)
          exportType = ExportType.BY_PROPERTIES;
        else exportType = ExportType.BY_NAME;
      } else {
        exportType = ExportType.NOT_EXPORTED;
      }

      // Operator visible object used to generate and submit tuples.
      final StreamingOutput<OutputTuple> outputPort;

      SPLPortType splPortType = outputPorts[port].getPortType();
      switch (splPortType) {
        case JREF_CPP:
          outputPort =
              PortFactory.getSwitchableConduitOutputPort(
                  ec,
                  inConsistentRegion,
                  port,
                  exportType,
                  portName,
                  (Schema) portSchema,
                  pePortNumber,
                  outputPorts[port].createHandler(port),
                  transport);
          outputPorts[port].getHandler().setCPPHandler();
          break;
        case CPP:
        default:
          outputPort =
              PortFactory.getConduitOutputPort(
                  ec,
                  inConsistentRegion,
                  port,
                  exportType,
                  portName,
                  (Schema) portSchema,
                  pePortNumber,
                  transport);
          break;
      }
      ports.set(port, outputPort);
    }

    return Collections.unmodifiableList(ports);
  }

  /**
   * Return true if we are standalone and trace is at debug levels, then we must use CPP to ensure
   * the correct tuple tracing.
   */
  private boolean isStandaloneCPPAndTracingIsDebugLevel() {
    boolean standalone =
        peModel.isStandalone() && Logger.getLogger("").isLoggable(TraceLevel.DEBUG);
    return standalone;
  }

  /**
   * Fuse all the connections that were previously found to be fusable.
   *
   * @throws Exception
   */
  void fuseJavaConnections(final JNIPE pe) throws Exception {

    if (outputPorts.length == 0) return;

    // Information on output ports in this operator with
    // fusable connections. During the processing of all
    // connections an output port is added to this if
    // it has a fusable connection, removed if it has
    // a non-fusable or unknown connection
    // Once the connection processing is complete then
    // it will contain connections that can be fused.
    Map<SPLOutputPort, List<Connection>> portFusableConnections =
        new HashMap<SPLOutputPort, List<Connection>>();

    // Make all output ports possible for fusing.
    for (int p = 0; p < outputPorts.length; p++) {

      final SPLOutputPort output = outputPorts[p];
      if (output.getPortType() == SPLPortType.JREF_CPP)
        portFusableConnections.put(outputPorts[p], new ArrayList<Connection>());
    }

    // No connections left unknown
    if (portFusableConnections.isEmpty()) return;

    for (Connection conn : connectionInfo.allConnections()) {
      assert conn.getOutputOperatorIndex() == getOperatorIndex();

      final SPLOutputPort output = outputPorts[conn.getOutputPortIndex()];

      // Port is removed from the map if it cannot be fused
      // this time around, so if it has been removed ignore
      // this connection as it can't be fused due to some
      // earlier decision.
      if (!portFusableConnections.containsKey(output)) continue;

      switch (output.getPortType()) {
        case CPP: // Already figured out the port type is CPP only.
          portFusableConnections.remove(output);
          continue;
        case JREF_CPP:
          List<Connection> fusableConns = portFusableConnections.get(output);
          fusableConns.add(conn);
          break;
        default:
          assert false : "Unknown SPLPortType";
      }
    }

    for (SPLOutputPort port : portFusableConnections.keySet()) {
      assert port.getPortType() == SPLPortType.JREF_CPP;

      if (port.getPortType() == SPLPortType.JREF_CPP) {
        final List<Connection> fusedConnections = portFusableConnections.get(port);
        port.enableJavaFusing(pe, fusedConnections);
      }
    }
  }

  /**
   * Setup a handler for an input port on this operator for the given connection. The handler will
   * be accepting tuples from a Java output port.
   */
  RuntimeStream<Tuple> setupFusedInput(Connection conn) {

    assert conn.getInputOperatorIndex() == getOperatorIndex();
    assert conn.getInputPortIndex() >= 0;
    assert conn.getInputPortIndex() < adapter.getContext().getNumberOfStreamingInputs();

    final int port = conn.getInputPortIndex();

    if (fusedInputs == null) fusedInputs = new HashMap<Integer, RuntimeStream<Tuple>>();

    RuntimeStream<Tuple> handler = fusedInputs.get(port);
    if (handler == null) {
      RuntimeStream<Tuple> innerHandler = adapter.getInputPortHandler(port);

      innerHandler = getAdapter().undecorateHandler(innerHandler, port);

      handler =
          new DirectTuple(
              innerHandler,
              getInputMetricView(port, InputPortMetric.nTuplesProcessed),
              getInputMetricView(port, InputPortMetric.nWindowPunctsProcessed),
              null);

      ThreadedPortType threadedPort = getThreadedPort(conn);
      if (threadedPort != null) {
        CongestionPolicy congestionPolicy = congestionPolicyFromConfig(threadedPort);

        boolean singleThreadedOnInput = threadedPort.isSingleThreadedOnInput();

        BigInteger queueSize = threadedPort.getQueueSize();
        int size = 1000;
        if (queueSize != null) {
          size = queueSize.intValue();
        }

        handler = getAdapter().addAlternateHandler(handler, port);
        getAdapter().enableSwitching(port);
        getAdapter().setAlternate(port, javaHandlerState);

        MetricValue queueSizeMetric = getInputMetricView(port, InputPortMetric.queueSize);
        MetricValue tuplesQueuedMetric = getInputMetricView(port, InputPortMetric.nTuplesQueued);
        MetricValue windowPunctsQueuedMetric =
            getInputMetricView(port, InputPortMetric.nWindowPunctsQueued);
        MetricValue finalPunctsQueuedMetric =
            getInputMetricView(port, InputPortMetric.nFinalPunctsQueued);
        MetricValue tuplesDroppedMetric = getInputMetricView(port, InputPortMetric.nTuplesDropped);
        MetricValue maxItemsQueuedMetric = getInputMetricView(port, InputPortMetric.maxItemsQueued);
        MetricValue recentMaxItemsQueuedMetric =
            getInputMetricView(port, InputPortMetric.recentMaxItemsQueued);
        MetricValue enqueueWaitsMetric = getInputMetricView(port, InputPortMetric.nEnqueueWaits);
        queueSizeMetric.setValue(size > 0 ? size : 1);
        ThreadedPortHandler threadedHandler =
            new ThreadedPortHandler(
                handler,
                congestionPolicy,
                singleThreadedOnInput,
                getAdapter(),
                queueSizeMetric,
                tuplesQueuedMetric,
                windowPunctsQueuedMetric,
                finalPunctsQueuedMetric,
                tuplesDroppedMetric,
                maxItemsQueuedMetric,
                recentMaxItemsQueuedMetric,
                enqueueWaitsMetric);
        handler = threadedHandler;

        getAdapter().addThreadedHandler(threadedHandler, port);

      } else {
        handler = getAdapter().addAlternateHandler(handler, port);
      }

      fusedInputs.put(port, handler);
    }
    getAdapter()
        .addAlternateHandlerOutput(
            conn.getInputPortIndex(), conn.getOutputOperatorIndex(), conn.getOutputPortIndex());

    // Stack a handler that sets the operator index in the
    // C++ runtime to ensure the operator name appears correctly
    // in trace/log output. This handler is specific to this
    // connection as it restores the tracker to the operator
    // of the output port.
    final int inputOpIndex = getOperatorIndex();
    final int outputOpIndex = conn.getOutputOperatorIndex();
    handler =
        new RuntimeStreamForwarder<Tuple>(handler) {
          @Override
          public void tuple(Tuple tuple) throws Exception {
            JNIBridge.setOperatorTracker(inputOpIndex);
            try {
              super.tuple(tuple);
            } finally {
              JNIBridge.setOperatorTracker(outputOpIndex);
            }
          }

          @Override
          public void mark(Punctuation mark) throws Exception {
            JNIBridge.setOperatorTracker(inputOpIndex);
            try {
              super.mark(mark);
            } finally {
              JNIBridge.setOperatorTracker(outputOpIndex);
            }
          }
        };

    return handler;
  }

  private static CongestionPolicy congestionPolicyFromConfig(ThreadedPortType threadedPort) {
    CongestionPolicy policy = null;
    assert (threadedPort != null);
    CongestionPolicyType configPolicy = threadedPort.getCongestionPolicy();
    switch (configPolicy) {
      case DROP_FIRST:
        policy = CongestionPolicy.DROP_FIRST;
        break;
      case DROP_LAST:
        policy = CongestionPolicy.DROP_LAST;
        break;
      case WAIT:
      default:
        policy = CongestionPolicy.WAIT;
        break;
    }
    return policy;
  }

  private MetricValue getOutputMetricView(int port, OutputPortMetric name) {
    final OperatorMetrics metrics = getAdapter().getContext().getMetrics();
    JNISystemMetric metric = (JNISystemMetric) metrics.getOutputPortMetric(port, name);
    return metric.getMetricValue();
  }

  private MetricValue getInputMetricView(int port, InputPortMetric name) {
    final OperatorMetrics metrics = getAdapter().getContext().getMetrics();
    JNISystemMetric metric = (JNISystemMetric) metrics.getInputPortMetric(port, name);
    return metric.getMetricValue();
  }

  private RuntimeStream<Tuple> getInputHandler(JNIPE pe, Connection conn) {
    RuntimeStream<Tuple> handler = null;
    final int inputOpIndex = conn.getInputOperatorIndex();
    SPLRuntimeOperator input = pe.getRuntimeOperator(inputOpIndex);
    if (input != null) {
      handler = input.setupFusedInput(conn);
    }
    return handler;
  }

  class SPLOutputPort {
    private SPLPortType portType;
    private JavaCppForwarder handler;

    synchronized void setPortType(SPLPortType portType) {
      this.portType = portType;
    }

    /**
     * Finally we have decided this port can be fused directly with its connections.
     *
     * @param fusedConnections
     * @throws Exception
     * @return true if fusing was successfully enabled
     */
    boolean enableJavaFusing(final JNIPE pe, final List<Connection> fusedConnections)
        throws Exception {
      boolean fused = false;
      final boolean notDynamic = false; // port directions are not dynamic
      final boolean dynamic = true; // port directions are dynamic

      // Get the input side handlers. If a single
      // input port is connected then its just the direct handler
      // otherwise it's a collection of direct handlers.
      RuntimeStream<Tuple> inputHandler = null;
      if (fusedConnections.size() == 0) {
        // fall through
      } else if (fusedConnections.size() == 1) {
        inputHandler = getInputHandler(pe, fusedConnections.get(0));
      } else {
        final List<RuntimeStream<Tuple>> handlers = new ArrayList<RuntimeStream<Tuple>>();
        for (Connection conn : fusedConnections) {
          RuntimeStream<Tuple> handler = getInputHandler(pe, conn);
          if (handler != null) {
            handlers.add(handler);
          } else {
            // Break out of the loop with inputHandler == null
            inputHandler = null;
            break;
          }
        }
        inputHandler = new StreamCollection<Tuple>(handlers);
      }

      if (inputHandler != null) {
        // Now create the DirectTuple that will accept the tuples
        // from the user's StreamingOutput and send them onto the input
        // ports through the inputHandler.
        final int port = fusedConnections.get(0).getOutputPortIndex();

        // Find out whether visualization is enabled for this port, and skip the visualization
        // handler as
        // an optimization when it is not enabled.
        RuntimeStream<Tuple> innerHandler = inputHandler;
        if (isOutputPortViewable(port)) {
          Schema tupleSchema = getOutputPortSchema(port);
          TupleVisualizationHandler tupleVisualizer =
              new TupleVisualizationHandler(inputHandler, tupleSchema, operatorIndex, port);
          if (SPLRuntimeOperator.this.tupleVisualizers == null) {
            SPLRuntimeOperator.this.tupleVisualizers =
                new HashMap<Integer, TupleVisualizationHandler>();
          }
          SPLRuntimeOperator.this.tupleVisualizers.put(port, tupleVisualizer);
          innerHandler = tupleVisualizer;
        }
        DirectTuple outputHandler =
            new DirectTuple(
                innerHandler,
                getOutputMetricView(port, OutputPortMetric.nTuplesSubmitted),
                getOutputMetricView(port, OutputPortMetric.nWindowPunctsSubmitted),
                null // Final is not handled via Java, so no need to have the metric value
                );

        getHandler().setJavaHandler(outputHandler);

        // If we are standalone, then at this point we can always
        // direct to Java because the trace level cannot change
        // and visualization is not supported.
        assert !Logger.getLogger("").isLoggable(TraceLevel.DEBUG);
        if (pe.isStandalone()) {
          getHandler().directToJava(notDynamic);

          if (trace.isLoggable(Level.INFO))
            trace.info(
                MessageFormat.format(
                    "JAVAFUSING: Op: {0} Output port: ({1}) - Standalone fused and directed to Java.",
                    getOperatorIndex(), port));
          fused = true;
        } else {
          // Distributed:
          // Create a notification listener on the trace level
          NotificationEmitter peMXBean =
              (NotificationEmitter) OperatorManagement.getProcessingElementMXBean();
          final NotificationFilterSupport traceLevel = new NotificationFilterSupport();
          traceLevel.enableType(ProcessingElementMXBean.TRACE_LEVEL);

          NotificationListener switchOutput =
              new NotificationListener() {

                @Override
                public void handleNotification(Notification notification, Object handback) {
                  try {
                    directOutput(pe, port, dynamic);
                  } catch (Exception e) {
                    throw new RuntimeException(e);
                  }
                }
              };

          // Watch trace level changes
          peMXBean.addNotificationListener(switchOutput, traceLevel, null);

          // Switch to Java now if not tracing at
          // debug level and not visualizing.
          directOutput(pe, port, notDynamic);

          fused = true;
        }
      }
      return fused;
    }

    /**
     * Direct output to the correct location in distributed mode.
     *
     * @throws Exception
     */
    private void directOutput(JNIPE pe, int port, boolean dynamic) throws Exception {

      final boolean directToCpp;

      if (Logger.getLogger("").isLoggable(TraceLevel.DEBUG)) directToCpp = true;
      else directToCpp = false;

      if (directToCpp) {
        getHandler().directToCpp(dynamic);
      } else {
        getHandler().directToJava(dynamic);
      }

      if (trace.isLoggable(Level.INFO))
        trace.info(
            MessageFormat.format(
                "JAVAFUSING: Op: {0} Output port: ({1}) - Distributed fused and directed to {2}",
                getOperatorIndex(), port, directToCpp ? "CPP" : "Java"));
    }

    synchronized SPLPortType getPortType() {
      return portType;
    }

    synchronized JavaCppForwarder createHandler(int port) {
      return this.handler = new JavaCppForwarder(port);
    }

    synchronized JavaCppForwarder getHandler() {
      return handler;
    }

    boolean isFused() {
      return getPortType().equals(SPLPortType.JREF_CPP)
          && (handler.getDestination() == handler.javaHandler);
    }
  }

  /** Handler that can switch between Java and CPP ports. */
  final class JavaCppForwarder extends RuntimeStreamDynamicForwarder<Tuple> {
    private StreamHandler<Tuple> cppHandler;
    private DirectTuple javaHandler;
    private int portNumber;

    public JavaCppForwarder(int port) {
      this.portNumber = port;
    }

    /**
     * First we setup an output port that goes to CPP with the handler set to point to CPP. Thus
     * after setting up the port we save off what is the CPP handler.
     */
    synchronized void setCPPHandler() {
      cppHandler = getDestination();
    }

    synchronized void setJavaHandler(DirectTuple handler) {
      javaHandler = handler;
    }

    private void switchIfNeeded(boolean dynamic) throws Exception {
      // If we have already set the destination one time,
      // we need to flow a switch to the current destination
      // before changing to the new destination.
      if (dynamic) {
        ((RuntimeStream<Tuple>) getDestination()).action(TypedAction.SwitchHandlers);
      }
    }

    synchronized void directToCpp(boolean dynamic) throws Exception {
      if (getDestination() != cppHandler) {
        switchIfNeeded(dynamic);
        setDestination(cppHandler);
      } else {
        if (javaHandler != null) {
          // By default the java handler is active, so we need to
          // switch to cpp in this case.
          ((RuntimeStream<Tuple>) javaHandler).action(TypedAction.SwitchHandlers);
        }
      }
    }

    synchronized void directToJava(boolean dynamic) throws Exception {
      if (javaHandler != null && getDestination() != javaHandler) {
        switchIfNeeded(dynamic);
        setDestination(javaHandler);
      }
    }

    /**
     * Always send final marks through JNI/CPP. Doesn't use setDestination to avoid any window where
     * we switch to CPP and then someone else switches us back before the mark is sent on. Force all
     * future events to go through the CPP which will be closed by the final. Also solves race
     * conditions between closing the CPP port and the java port remaining open.
     *
     * <p>Also send all cut/restore markers for consistent cut through JNI/CPP so that the ports can
     * handle forwarding correctly.
     */
    @Override
    public void mark(Punctuation mark) throws Exception {
      if (mark == Punctuation.FINAL_MARKER) {
        final DirectTuple javaHandler;
        synchronized (this) {
          // This way everything will go through CPP
          // from now (after the final mark)
          final boolean dynamic = true;
          directToCpp(dynamic);
          javaHandler = this.javaHandler;
          this.javaHandler = null;
        }
        if (javaHandler != null) javaHandler.drain(true /*finalize*/);
      }
      super.mark(mark);
    }

    @Override
    public void tuple(Tuple tuple) throws Exception {
      super.tuple(tuple);
    }

    @Override
    public String toString() {
      return "JavaCPPForwarder "
          + SPLRuntimeOperator.this.getAdapter().getOperatorName()
          + ":"
          + this.portNumber;
    }
  }

  void setAdapter(OperatorAdapter adapter) {
    this.adapter = adapter;
  }

  private OperatorAdapter getAdapter() {
    return adapter;
  }

  public void connect(JNIPE jnipe) throws Exception {
    opTrace.log(TraceLevel.DEBUG, "In SPLRuntimeOperator.connect");

    fuseJavaConnections(jnipe);
  }

  public void addView(int outputPortIndex, long propertiesHandle) {
    if (outputPortIndex >= outputPorts.length) {
    } else {
      TupleVisualizationHandler visualizationHandler = this.tupleVisualizers.get(outputPortIndex);
      if (visualizationHandler != null) {
        visualizationHandler.addView(propertiesHandle);
      }
    }
  }

  public void deleteView(int outputPortIndex, long propertiesHandle) {
    if (outputPortIndex >= outputPorts.length) {
    } else {
      TupleVisualizationHandler visualizationHandler = this.tupleVisualizers.get(outputPortIndex);
      if (visualizationHandler != null) {
        visualizationHandler.deleteView(propertiesHandle);
      }
    }
  }

  private Schema getOutputPortSchema(int outputPortIndex) {
    return outputPortSchemae.get(outputPortIndex);
  }

  private boolean isOutputPortViewable(int outputPortIndex) {
    boolean viewable = true;

    OperInstanceOutputPortsType outputPorts = null;
    List<OperInstanceOutputPortType> outputPortList = null;
    OperInstanceOutputPortType outputPort = null;

    PhysOperInstanceType operatorInstance = peModel.getOperatorInstance(operatorIndex);
    Boolean isViewable = null;
    if (operatorInstance != null) {
      outputPorts = operatorInstance.getOutputPorts();
    }
    if (outputPorts != null) {
      outputPortList = outputPorts.getOutputPort();
    }
    if (outputPortList != null) {
      outputPort = outputPortList.get(outputPortIndex);
    }
    if (outputPort != null) {
      isViewable = outputPort.isViewable();
    }
    if (isViewable != null) {
      viewable = isViewable.booleanValue();
    }

    return viewable;
  }

  // for testing support only
  public Collection<SPLPortType> getOutputPortTypes() {
    List<SPLPortType> portTypes = new ArrayList<SPLPortType>(outputPorts.length);
    for (SPLOutputPort outputPort : outputPorts) {
      portTypes.add(outputPort.portType);
    }

    return Collections.unmodifiableCollection(portTypes);
  }

  @Override
  public String toString() {
    return "SPLRuntimeOperator " + this.adapter.getOperatorName();
  }
}
