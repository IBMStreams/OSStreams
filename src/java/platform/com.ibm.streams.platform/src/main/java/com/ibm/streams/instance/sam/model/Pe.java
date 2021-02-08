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

package com.ibm.streams.instance.sam.model;

import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;
import com.ibm.distillery.utils.DistilleryException;
import com.ibm.distillery.utils.DistilleryExceptionCode;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.ObjectTracker;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.SamUtilFunctions;
import com.ibm.streams.instance.sam.model.fuser.FusedContainer;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnection;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnectionId;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerInputPort;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerOutputPort;
import com.ibm.streams.instance.sam.model.runtime.PeHealth;
import com.ibm.streams.instance.sam.model.runtime.PeStateReason;
import com.ibm.streams.instance.sam.model.runtime.RoutingNotificationType;
import com.ibm.streams.instance.sam.model.runtime.RuntimePe;
import com.ibm.streams.instance.sam.model.runtime.Tracing;
import com.ibm.streams.instance.sam.model.runtime.TracingEmployment;
import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.Library;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleTypeType;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.AnnotationAttributeValueType;
import com.ibm.streams.platform.services.AnnotationTupleValueType;
import com.ibm.streams.platform.services.AnnotationType;
import com.ibm.streams.platform.services.AugmentedPEInputPortType;
import com.ibm.streams.platform.services.AugmentedPEInputPortsType;
import com.ibm.streams.platform.services.AugmentedPEOutputPortType;
import com.ibm.streams.platform.services.AugmentedPEOutputPortsType;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.platform.services.AvoidRuntimeYieldType;
import com.ibm.streams.platform.services.CustomMetricIndiciesType;
import com.ibm.streams.platform.services.CustomMetricsType;
import com.ibm.streams.platform.services.DependenciesType;
import com.ibm.streams.platform.services.EncodingType;
import com.ibm.streams.platform.services.EnvVarType;
import com.ibm.streams.platform.services.EnvVarsType;
import com.ibm.streams.platform.services.ExecutableType;
import com.ibm.streams.platform.services.ExportedStreamType;
import com.ibm.streams.platform.services.HostpoolsType;
import com.ibm.streams.platform.services.ImportedStreamsType;
import com.ibm.streams.platform.services.IncomingStaticIPConnectionsType;
import com.ibm.streams.platform.services.LanguageType;
import com.ibm.streams.platform.services.LibrariesType;
import com.ibm.streams.platform.services.OperDefinitionType;
import com.ibm.streams.platform.services.OperDefinitionsType;
import com.ibm.streams.platform.services.OperInstanceIncomingConnectionType;
import com.ibm.streams.platform.services.OperInstanceInputPortType;
import com.ibm.streams.platform.services.OperInstanceOutgoingConnectionType;
import com.ibm.streams.platform.services.OperInstanceOutputPortType;
import com.ibm.streams.platform.services.OperInstancePEInputConnectionType;
import com.ibm.streams.platform.services.OperInstancePEOutputConnectionType;
import com.ibm.streams.platform.services.OperInstancesType;
import com.ibm.streams.platform.services.OperatorRuntimeType;
import com.ibm.streams.platform.services.OutgoingStaticOPConnectionsType;
import com.ibm.streams.platform.services.ParallelChannelType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.streams.platform.services.SplitterType;
import com.ibm.streams.platform.services.TransportType;
import com.ibm.streams.platform.services.TupleTypeType;
import com.ibm.streams.platform.services.TupleTypesType;
import com.ibm.streams.sam.exc.PECannotRestartException;
import java.math.BigInteger;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.PriorityQueue;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;
import org.apache.commons.lang.builder.CompareToBuilder;

/* This PE class exists as the bridge between the logical, physical, and runtime
 * constructs that contain Processing Element information
 * This is not intended to be the data model definition, rather a convenient container
 * for accessing all of the data elements can be identified by the Job ID, PE ID,
 * Application Name, and/or PE Index. Each of the data elements belong to their respective
 * vertical container (ADL, PADL, AADL)
 */

public class Pe extends SamObject implements IXmlSerializer<AugmentedPEType>, Comparable<Pe> {

  private RuntimePe runPe;
  private FusedContainer fused;
  private BiMap<BigInteger, BigInteger> ipIdToIndex;
  private BiMap<BigInteger, BigInteger> opIdToIndex;
  private SortedMap<String, TopologyNode>
      nodeMap; // cache nodes for use during updateoperators dynamic udp
  private SortedMap<BigInteger, InputPort> inputPorts;
  private SortedMap<BigInteger, OutputPort> outputPorts;
  private Map<BigInteger, InputPort> fusedContainerInputPortIndexToPeInputPort;
  private Map<BigInteger, OutputPort> fusedContainerOutputPortIndexToPeOutputPort;
  private final Map<BigInteger, BigInteger> fusedContainerIndexToPeId;

  private BigInteger minIncomingPeConnection(
      FusedContainerInputPort fcip, Map<BigInteger, BigInteger> fusedContainerIndexToPeId) {
    return fcip.getContainerConnections().stream()
        .map(f -> fusedContainerIndexToPeId.get(f.getSourceContainerIndex()))
        .min(BigInteger::compareTo)
        .orElse(
            BigInteger.valueOf(
                Long.MAX_VALUE)); // in the case of Imports, we have no incoming connections, so
    // let's return an
    // enormously large value
  }

  private BigInteger minOutgoingPeConnection(
      FusedContainerOutputPort fcop, Map<BigInteger, BigInteger> fusedContainerIndexToPeId) {
    return fcop.getContainerConnections().stream()
        .map(f -> fusedContainerIndexToPeId.get(f.getDestinationContainerIndex()))
        .min(BigInteger::compareTo)
        .orElse(
            BigInteger.valueOf(
                Long.MAX_VALUE)); // in the case of Exports, we have no outgoing connections, so
    // let's return an
    // enormously large value
  }

  public Pe(
      BigInteger jobId,
      FusedContainer fused,
      BigInteger peIndex,
      BigInteger peId,
      Map<BigInteger, BigInteger> fusedContainerIndexToPeId,
      BigInteger genId,
      boolean isPreviewOnly,
      boolean buildingJob,
      boolean pesStopped) {
    super(isPreviewOnly);

    this.fusedContainerIndexToPeId = fusedContainerIndexToPeId;
    initialize(
        fused, new RuntimePe(jobId, peId, peIndex, genId, buildingJob, pesStopped), buildingJob);

    // Generate Ports
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("Beginning Port Generation");
    }

    PriorityQueue<FusedContainerInputPort> sortedFusedInputPorts =
        new PriorityQueue<>(
            (f1, f2) -> {
              BigInteger i1 = minIncomingPeConnection(f1, fusedContainerIndexToPeId);
              BigInteger i2 = minIncomingPeConnection(f2, fusedContainerIndexToPeId);
              return i1.compareTo(i2);
            });
    for (FusedContainerInputPort fcip : fused.getContainerInputPorts().values()) {
      sortedFusedInputPorts.add(fcip);
    }

    BigInteger nextInputPortId = BigInteger.ZERO;
    while (!sortedFusedInputPorts.isEmpty()) {
      FusedContainerInputPort fcip = sortedFusedInputPorts.poll();
      InputPort ip = getInputPortCreator(jobId, getId(), nextInputPortId, fcip);
      ipIdToIndex.put(ip.getId(), ip.getIndex());
      inputPorts.put(ip.getId(), ip);
      fusedContainerInputPortIndexToPeInputPort.put(fcip.getIndex(), ip);
      nextInputPortId = nextInputPortId.add(BigInteger.ONE);
    }

    PriorityQueue<FusedContainerOutputPort> sortedFusedOutputPorts =
        new PriorityQueue<>(
            (f1, f2) -> {
              BigInteger i1 = minOutgoingPeConnection(f1, fusedContainerIndexToPeId);
              BigInteger i2 = minOutgoingPeConnection(f2, fusedContainerIndexToPeId);
              return i1.compareTo(i2);
            });
    for (FusedContainerOutputPort fcop : fused.getContainerOutputPorts().values()) {
      sortedFusedOutputPorts.add(fcop);
    }

    BigInteger nextOutputPortId = BigInteger.ZERO;
    while (!sortedFusedOutputPorts.isEmpty()) {
      FusedContainerOutputPort fcop = sortedFusedOutputPorts.poll();
      OutputPort op = getOutputPortCreator(jobId, getId(), nextOutputPortId, fcop);
      opIdToIndex.put(op.getId(), op.getIndex());
      outputPorts.put(op.getId(), op);
      fusedContainerOutputPortIndexToPeOutputPort.put(fcop.getIndex(), op);
      nextOutputPortId = nextOutputPortId.add(BigInteger.ONE);
    }

    runPe.setInputPortIds(ipIdToIndex.keySet());
    runPe.setOutputPortIds(opIdToIndex.keySet());

    // Logging the PE info
    String operNames = new String();
    int number = 0;
    for (BigInteger operIndex : fused.getNodeIndexes()) {
      if (number >= 3) { // we just get three operator names, not all
        break;
      }
      TopologyNode tn =
          SAMStaticContext.getTopologyTracker()
              .getTopologyNode(fused.getFusedApplicationId(), operIndex);
      operNames += tn.getName();
      number++;
      if (number < 3 && (number < fused.getNodeIndexes().size())) {
        operNames += ", ";
      }
    }
    if (fused.getNodeIndexes().size() > 3) {
      operNames += "...";
    }

    SAMStaticContext.productLog(
        StreamsRuntimeMessagesKey.Key.SAMPECreated, getId(), getJobId(), operNames);
  }

  private InputPort getInputPortCreator(
      final BigInteger jobId,
      final BigInteger peId,
      final BigInteger ipId,
      final FusedContainerInputPort fcip) {
    return new InputPort(jobId, peId, ipId, fcip, isPreviewOnly());
  }

  private OutputPort getOutputPortCreator(
      final BigInteger jobId,
      final BigInteger peId,
      final BigInteger opId,
      final FusedContainerOutputPort fcop) {
    return new OutputPort(jobId, peId, opId, fcop, isPreviewOnly());
  }

  // for recovery
  public Pe(FusedContainer fused, RuntimePe runPe) {
    super(false);
    throw new RuntimeException("recovery Pe constructor called");
  }

  private void initialize(FusedContainer fused, RuntimePe runPe, boolean buildingJob) {
    this.runPe = runPe;
    this.fused = fused;
    this.inputPorts = new TreeMap<>();
    this.outputPorts = new TreeMap<>();
    this.fusedContainerInputPortIndexToPeInputPort = new HashMap<>();
    this.fusedContainerOutputPortIndexToPeOutputPort = new HashMap<>();

    ipIdToIndex = HashBiMap.<BigInteger, BigInteger>create();
    opIdToIndex = HashBiMap.<BigInteger, BigInteger>create();

    nodeMap = new TreeMap<>();
    refreshNodeMap();

    // Set tracing.
    Tracing tracing = new Tracing();
    tracing.setLevel(Tracing.Level.fromValue(fused.getTracing()));
    TracingEmployment currentTracing = new TracingEmployment(tracing, BigInteger.ZERO);
    setCurrentTracing(currentTracing);

    // This must be the last thing, otherwise CurrentTracing might be null
    SAMStaticContext.getObjectTracker().registerPe(this);
  }

  ////////////////////
  /// Operations
  ////////////////////

  public RuntimePe getRuntime() {
    return runPe;
  }

  public void setRuntime(RuntimePe runPe) {
    this.runPe = runPe;
  }

  public BigInteger getId() {
    return runPe.getId();
  }

  public BigInteger getJobId() {
    return runPe.getJobId();
  }

  public BigInteger getIndex() {
    return runPe.getIndex();
  }

  public BigInteger getLastRunTimestamp() {
    return runPe.getLastRunTimestamp();
  }

  public void setLastRunTimestamp(BigInteger lastRunTimestamp) {
    runPe.setLastRunTimestamp(lastRunTimestamp);
  }

  public BigInteger getLaunchCount() {
    return runPe.getLaunchCount();
  }

  public void setLaunchCount(BigInteger launchCount) {
    runPe.setLaunchCount(launchCount);
  }

  public BigInteger getSamRetryCount() {
    return runPe.getSamRetryCount();
  }

  public void setSamRetryCount(BigInteger samRetryCount) {
    runPe.setSamRetryCount(samRetryCount);
  }

  public boolean isRunning() {
    return false;
  }

  public boolean isStartingUp() {
    return false;
  }

  public PeStateReason getReason() {
    return runPe.getReason();
  }

  public void setReason(PeStateReason reason) {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug(
          "ENTER Pe.setReason with " + Arrays.deepToString(new Object[] {getId(), reason}));
    }
    runPe.setReason(reason);
  }

  public String getHost() {
    return runPe.getHost();
  }

  public Collection<InputPort> getInputPorts() {
    return inputPorts.values();
  }

  public InputPort getInputPortById(BigInteger id) {
    return inputPorts.get(id);
  }

  public InputPort getInputPortByFusedContainerInputPortIndex(BigInteger index) {
    return fusedContainerInputPortIndexToPeInputPort.get(index);
  }

  public BigInteger getInputPortIdByIndex(BigInteger index) {
    return ipIdToIndex.inverse().get(index);
  }

  public Collection<OutputPort> getOutputPorts() {
    return outputPorts.values();
  }

  public OutputPort getOutputPortByFusedContainerOutputPortIndex(BigInteger index) {
    return fusedContainerOutputPortIndexToPeOutputPort.get(index);
  }

  public Map<String, String> getEnvVars() {
    return fused.getEnvVars();
  }

  public FusedContainer getFusedContainer() {
    return fused;
  }

  public boolean isRestartable() {
    return fused.isRestartable();
  }

  public boolean isRelocatable() {
    return fused.isRelocatable();
  }

  public boolean isHostValid() {
    return runPe.isHostValid();
  }

  public boolean isStateStale() {
    return runPe.isStateStale();
  }

  public void setStateStale(boolean isStateStale) {
    runPe.setStateStale(isStateStale);
  }

  public TracingEmployment getPendingTracing() {
    return runPe.getPendingTracing();
  }

  public void setPendingTracing(TracingEmployment pendingTracing) {
    runPe.setPendingTracing(pendingTracing);
  }

  public TracingEmployment getCurrentTracing() {
    return runPe.getCurrentTracing();
  }

  public void setCurrentTracing(TracingEmployment currentTracing) {
    runPe.setCurrentTracing(currentTracing);
  }

  public BigInteger getNextTrcUpdateSeqNum() {
    return runPe.getNextTrcUpdateSeqNum();
  }

  public void setNextTrcUpdateSeqNum(BigInteger nextTrcUpdateSeqNum) {
    runPe.setNextTrcUpdateSeqNum(nextTrcUpdateSeqNum);
  }

  public BigInteger getPid() {
    return runPe.getPid();
  }

  public BigInteger getGenId() {
    return runPe.getGenId();
  }

  public SortedSet<TopologyNode> getNodes() {
    return new TreeSet<>(nodeMap.values());
  }

  public void refreshNodeMap() {
    nodeMap.clear();
    for (BigInteger nodeIndex : fused.getNodeIndexes()) {
      TopologyNode tNode =
          SAMStaticContext.getTopologyTracker()
              .getTopologyNode(fused.getTopologyApplicationId(), nodeIndex);
      nodeMap.put(tNode.getName(), tNode);
    }
  }

  public Set<BigInteger> getNodeIndexes() {
    return fused.getNodeIndexes();
  }

  ////////////////////
  /// Internal Use
  ////////////////////
  public void setHealth(PeHealth health) {}

  public PeHealth getHealth() {
    return runPe.getHealth();
  }

  private SplitterType findSplitter(List<SplitterType> splitters, BigInteger index) {
    for (SplitterType splitter : splitters) {
      if (splitter.getIndex().equals(index)) {
        return splitter;
      }
      // See if we have a nested splitter
      for (ParallelChannelType pct : splitter.getParallelChannel()) {
        if (pct.getSplitter() != null) {
          SplitterType s = findSplitter(pct.getSplitter(), index);
          if (s != null) return s;
        }
      }
    }

    return null;
  }

  private ParallelChannelType findParallelChannelType(
      TopologyNodeConnection tnc, OperInstanceOutputPortType operOp) {

    // We need to walk to find the splitter
    //      List<SplitterType> splitters =
    // operOp.getSplitter().get(tnc.getSplitterIndex().intValue());
    List<SplitterType> splitters = operOp.getSplitter();
    SplitterType splitter = findSplitter(splitters, tnc.getSplitterIndex());

    // Linear walk through this list, which is bad, but it's an ArrayList and the channel index is
    // not
    // how the channels are stored, so we don't have a choice.
    for (ParallelChannelType pct : splitter.getParallelChannel()) {
      if (pct.getIndex().equals(tnc.getChannelIndex())) {
        return pct;
      }
    }

    assert (false); // Should never get here
    //    ParallelChannelType pct = new ParallelChannelType();
    //    pct.setIndex(tnc.getChannelIndex());
    //    splitter.getParallelChannel().add(pct);

    // Logically, setting these values for the splitter needs to only happen once, but we end up
    // doing it
    // multiple times, once for each connection in the splitter. It's harmless because we re-write
    // the
    // same values over again, but strange.
    //    splitter.setIportIndex(tnc.getId().getDstPortIndex());
    //
    // splitter.setRegionIndex(tnc.getDestination().getOwningOperator().getParallelRegions().get(0).getRegionIndex());

    return null;
  }

  ////////////////////
  /// XML Serialization
  ////////////////////
  public AugmentedPEType toXml() {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Starting Pe.toXml()");
    }

    Job job = SAMStaticContext.getObjectTracker().getJob(runPe.getJobId());
    AugmentedPEType ape = new AugmentedPEType();
    ape.setExecStreamsVersion(job.getExecStreamsVersion());
    if (getGenId() != null) {
      ape.setGenId(getGenId());
    }

    LibrariesType libType = new LibrariesType();
    for (Library lib : job.getLibraries().values()) {
      libType.getLibrary().add(lib.toXml());
    }
    ape.setLibraries(libType);

    HostpoolsType hostType = new HostpoolsType();
    for (Hostpool hp : job.getHostpools().values()) {
      hostType.getHostpool().add(hp.toXml());
    }
    ape.setHostpools(hostType);

    // TupleTypes (Under rework by Howard)
    TupleTypesType ttt = new TupleTypesType();
    for (Map.Entry<BigInteger, TopologyTupleTypeType> t : job.getTupleTypes().entrySet()) {
      TupleTypeType tt = t.getValue().toXml();
      tt.setIndex(t.getKey());
      ttt.getTupleType().add(tt);
    }
    ape.setTupleTypes(ttt);
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Starting Pe.toXml() - Finished TuplesTypes");
    }

    EnvVarsType envs = new EnvVarsType();
    for (Map.Entry<String, String> var : getEnvVars().entrySet()) {
      EnvVarType varType = new EnvVarType();
      varType.setName(var.getKey());
      varType.setValue(var.getValue());
      envs.getEnvVar().add(varType);
    }
    ape.setEnvVars(envs);

    // Executable
    // TODO: See if it can be removed from AADL for the future
    ExecutableType exec = new ExecutableType();
    String wrapper = "none";
    for (TopologyNode tn : getNodes()) {
      if (SamUtilFunctions.isValidWrapper(tn.getWrapper())) {
        // Assume that, all operators have same wrapper.
        // Otherwise, we pick the first one.
        wrapper = tn.getWrapper();
        break;
      }
    }
    exec.setWrapper(wrapper);
    exec.setExecutableUri("");
    exec.setArguments("");
    exec.setDependencies(new DependenciesType());
    exec.setDynamicLoad(true);
    exec.setDigest("");
    ape.setExecutable(exec);
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Starting Pe.toXml() - Finished Executable");
    }

    ObjectTracker objectTracker = SAMStaticContext.getObjectTracker();

    // OperInstances
    OperInstancesType oit = new OperInstancesType();
    for (TopologyNode tn : getNodes()) {
      PhysOperInstanceType physOperInstance = tn.toXml();
      // rtc39133 - operator launchCount starts at 1
      BigInteger nextLaunchCount =
          SAMStaticContext.getObjectTracker()
              .getOperator(new OperatorId(tn.getApplicationId(), tn.getIndex()))
              .getLaunchCount()
              .add(BigInteger.ONE);
      physOperInstance.setLaunchCount(nextLaunchCount);
      // Have to inject InterPeConnections & IntraPeConnections
      // for both OperInstanceOutputPort & OperInstanceInputPort
      // Also have to inject it for Splitters
      for (OperInstanceOutputPortType operOp : physOperInstance.getOutputPorts().getOutputPort()) {
        for (TopologyNodeOutputPort tnop : tn.getOutputPorts().values()) {
          if (!tnop.getIndex().equals(operOp.getIndex())) {
            continue; // This node port doesn't match the xml node port, skip it
          }
          for (TopologyNodeConnection tnc : tnop.getOperConnections()) {

            OperInstanceOutgoingConnectionType connXml = new OperInstanceOutgoingConnectionType();
            connXml.setOperInstanceIndex(tnc.getId().getDstNodeIndex());
            connXml.setIportIndex(tnc.getId().getDstPortIndex());

            if (fused.getInterContainerConnections().contains(tnc)) {
              for (FusedContainerConnectionId fccId : fused.getOutgoingContainerConnections()) {
                FusedContainerConnection fcc =
                    SAMStaticContext.getFusedTracker()
                        .getFusedContainerConnection(fused.getFusedApplicationId(), fccId);
                if (fcc.getTopologyNodeConnectionId().equals(tnc.getId())) {
                  BigInteger destPeId =
                      fusedContainerIndexToPeId.get(fccId.getDestinationContainerIndex());
                  Pe destPe = job.getPe(destPeId);
                  // Note that we are using the PE port ID as the PE port index; that is something
                  // we make sure remains
                  // true
                  connXml.setPePortIndex(
                      destPe
                          .getInputPortByFusedContainerInputPortIndex(
                              fccId.getDestinationContainerPortIndex())
                          .getId());
                  break;
                }
              }

              if (Trace.isEnabled(Level.TRACE)) {
                Trace.logTrace("Operator OP Splitter Size: " + operOp.getSplitter().size());
              }

              // Connection is from a Splitter
              if (tnc.getSplitterIndex() != null
                  && operOp.getSplitter().size()
                      > 0) { // TODO: Decide if the Splitter Size check is correct?
                findParallelChannelType(tnc, operOp).getInterPeConnection().add(connXml);
              } else {
                operOp.getInterPeConnection().add(connXml);
              }
            } else {
              // Connection is from a Splitter
              if (tnc.getSplitterIndex() != null && operOp.getSplitter().size() > 0) {
                findParallelChannelType(tnc, operOp).getIntraPeConnection().add(connXml);
              } else {
                operOp.getIntraPeConnection().add(connXml);
              }
            }
          }
        }
      }

      for (OperInstanceInputPortType operIp : physOperInstance.getInputPorts().getInputPort()) {
        for (TopologyNodeInputPort tnip : tn.getInputPorts().values()) {
          if (!tnip.getIndex().equals(operIp.getIndex())) {
            continue;
          }
          for (TopologyNodeConnection tnc : tnip.getOperConnections()) {

            OperInstanceIncomingConnectionType connXml = new OperInstanceIncomingConnectionType();
            connXml.setOperInstanceIndex(tnc.getId().getSrcNodeIndex());
            connXml.setOportIndex(tnc.getId().getSrcPortIndex());

            // We set splitter values on the output port side, here we just need to reference them
            if (tnc.getSplitterIndex() != null) {
              connXml.setSplitterIndex(tnc.getSplitterIndex());
            }
            if (tnc.getChannelIndex() != null) {
              connXml.setChannelIndex(tnc.getChannelIndex());
            }

            if (fused.getInterContainerConnections().contains(tnc)) {
              for (FusedContainerConnectionId fccId : fused.getIncomingContainerConnections()) {
                FusedContainerConnection fcc =
                    SAMStaticContext.getFusedTracker()
                        .getFusedContainerConnection(fused.getFusedApplicationId(), fccId);
                if (fcc.getTopologyNodeConnectionId().equals(tnc.getId())) {
                  BigInteger sourcePeId =
                      fusedContainerIndexToPeId.get(fccId.getSourceContainerIndex());
                  Pe sourcePe = job.getPe(sourcePeId);
                  // Note that we are using the PE port ID as the PE port index; that is something
                  // we make sure remains
                  // true
                  connXml.setPePortIndex(
                      sourcePe
                          .getOutputPortByFusedContainerOutputPortIndex(
                              fccId.getSourceContainerPortIndex())
                          .getId());
                  break;
                }
              }
              operIp.getInterPeConnection().add(connXml);
            } else {
              operIp.getIntraPeConnection().add(connXml);
            }
          }
        }
      }

      // If there is an "eventTimeContext" annotation, we need to update
      // the value of the "inputConnections" key (if present) to
      // use physical instead of logical operator indices.
      List<AnnotationType> annotations = physOperInstance.getAnnotation();
      if (annotations != null && !annotations.isEmpty()) {
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("Found annotations");
        }
        for (AnnotationType annotation : annotations) {
          if (annotation.getTag().equals("eventTimeContext")) {

            // We will need to find the physical operator indexes
            // corresponding to each logical operator index.  There
            // may be more than one physical index in the case where
            // a UDP region has been expanded.

            // This could possibly be moved out of this loop so
            // we don't reconstruct this map for every eventTimeContext
            // annotation
            Map<BigInteger, Set<BigInteger>> logicalToPhysical =
                new HashMap<BigInteger, Set<BigInteger>>();

            Set<TopologyNode> topologyNodes =
                SAMStaticContext.getObjectTracker().getTopologyNodes(tn.getApplicationId());
            for (TopologyNode topologyNode : topologyNodes) {
              BigInteger physicalIndex = topologyNode.getIndex();
              BigInteger logicalIndex = topologyNode.getLogicalIndex();

              Set<BigInteger> indexes = logicalToPhysical.get(logicalIndex);
              if (indexes == null) {
                indexes = new HashSet<>();
                indexes.add(physicalIndex);
                logicalToPhysical.put(logicalIndex, indexes);
              } else {
                indexes.add(physicalIndex);
              }
            }
            AnnotationTupleValueType tuple = annotation.getTuple();
            List<AnnotationAttributeValueType> values = tuple.getAttr();
            for (AnnotationAttributeValueType attributeValue : values) {
              if (attributeValue.getKey().equals("inputConnections")) {
                StringBuilder physInputConnections = new StringBuilder();
                String value = attributeValue.getValue();
                if (Trace.isEnabled(Level.TRACE)) {
                  Trace.logTrace("Found inputConnections " + value);
                }

                // Change the value to use physical instead of
                // logical operator index.

                // This will be a bracketed list of three ints,
                // such as [1,2,4]
                // Remove brackets.
                String triple = value.substring(1, value.length() - 1);
                // Separate at comma
                String[] connectionParts = triple.split(",");

                assert (connectionParts.length == 3);
                BigInteger inputPortIndex = new BigInteger(connectionParts[0]);
                BigInteger connectedLogicalIndex = new BigInteger(connectionParts[1]);
                BigInteger outputPortIndex = new BigInteger(connectionParts[2]);

                // find all physical indexes corresponding to the
                // logical index
                Set<BigInteger> physicalIndexes = logicalToPhysical.get(connectedLogicalIndex);
                if (physicalIndexes != null) {
                  physInputConnections.append('[');
                  for (BigInteger connectedPhysicalIndex : physicalIndexes) {
                    physInputConnections
                        .append("[")
                        .append(inputPortIndex)
                        .append(",")
                        .append(connectedPhysicalIndex)
                        .append(",")
                        .append(outputPortIndex)
                        .append("],");
                  }
                  if (physInputConnections.length() > 1) {
                    // replace final comma
                    physInputConnections.setCharAt(physInputConnections.length() - 1, ']');
                  } else {
                    physInputConnections.append(']');
                  }
                  if (Trace.isEnabled(Level.TRACE)) {
                    Trace.logTrace(
                        "Setting inputConnections to " + physInputConnections.toString());
                  }
                  attributeValue.setValue(physInputConnections.toString());
                }
              }
            }
          }
        }
      }

      oit.getOperInstance().add(physOperInstance);
    }

    ape.setOperInstances(oit);
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Starting Pe.toXml() - Finished (Phys)OperInstances");
    }

    // Note that we are processing the input ports in the order we constructed them in, and the
    // order we constructed them in guarantees that the ports which communicate with the lowest
    // PE IDs appear first.
    BigInteger nextInputPortIndex = BigInteger.ZERO;
    ape.setInputPorts(new AugmentedPEInputPortsType());
    for (InputPort ip : getInputPorts()) {
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Inserting InputPort " + ip.getId());
      }
      AugmentedPEInputPortType aip = new AugmentedPEInputPortType();

      TupleTypeType tt = job.getTupleTypes().get(ip.getTupleTypeIndex()).toXml();
      tt.setIndex(ip.getTupleTypeIndex());
      aip.setTupleType(tt);
      if (ip.getImportedStreams() != null) {
        ImportedStreamsType imported = ip.getImportedStreams().toXml();
        aip.setImportedStreams(imported);
      }
      IncomingStaticIPConnectionsType staticConns = new IncomingStaticIPConnectionsType();
      for (ConnectionId id : ip.getIncomingStaticConnectionIds()) {
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("Operating On ConnectionId: " + id);
        }
        staticConns.getStaticConnection().add(id.getIncomingStaticIPConnectionType());
      }
      aip.setStaticConnections(staticConns);

      OperInstancePEInputConnectionType operToPePortConn = new OperInstancePEInputConnectionType();
      // OperInstanceIndex Reconstruction
      // InputPort -> FusedContainerInputPort -> FusedNodeInputPort -> FusedNode
      // PE -> FusedContainer -> FusedNode Lookup By Array Index
      operToPePortConn.setOperInstanceIndex(ip.getNodeIndex());
      operToPePortConn.setIportIndex(ip.getNodePortIndex());
      aip.setOperInstanceConnection(operToPePortConn);

      aip.setId(ip.getId());
      aip.setIndex(nextInputPortIndex);
      aip.setPeId(getId());
      aip.setEncoding(EncodingType.fromValue(ip.getEncoding().toString()));
      aip.setTransport(TransportType.fromValue(ip.getTransport().toString()));
      ape.getInputPorts().getInputPort().add(aip);

      nextInputPortIndex = nextInputPortIndex.add(BigInteger.ONE);
    }

    // Note that we are processing the output ports in the order we constructed them in, and the
    // order we constructed them in guarantees that the ports which communicate with the lowest
    // PE IDs appear first.
    BigInteger nextOutputPortIndex = BigInteger.ZERO;
    ape.setOutputPorts(new AugmentedPEOutputPortsType());
    for (OutputPort op : getOutputPorts()) {
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Inserting OutputPort " + op.getId());
      }
      AugmentedPEOutputPortType aop = new AugmentedPEOutputPortType();

      TupleTypeType tt = job.getTupleTypes().get(op.getTupleTypeIndex()).toXml();
      tt.setIndex(op.getTupleTypeIndex());
      aop.setTupleType(tt);
      if (op.getExportedStream() != null) {
        ExportedStreamType export = op.getExportedStream().toXml();
        aop.setExportedStream(export);
      }
      OutgoingStaticOPConnectionsType staticConns = new OutgoingStaticOPConnectionsType();
      for (ConnectionId id : op.getOutgoingStaticConnectionIds()) {
        staticConns.getStaticConnection().add(id.getOutgoingStaticOPConnectionType());
      }
      aop.setStaticConnections(staticConns);

      OperInstancePEOutputConnectionType operToPePortConn =
          new OperInstancePEOutputConnectionType();
      operToPePortConn.setOperInstanceIndex(op.getNodeIndex());
      operToPePortConn.setOportIndex(op.getNodePortIndex());
      if (op.getSplitterIndex() != null && op.getChannelIndex() != null) {
        operToPePortConn.setSplitterIndex(op.getSplitterIndex());
        operToPePortConn.setChannelIndex(op.getChannelIndex());
      }

      aop.setOperInstanceConnection(operToPePortConn);
      aop.setId(op.getId());
      aop.setIndex(nextOutputPortIndex);
      aop.setPeId(getId());
      aop.setEncoding(EncodingType.fromValue(op.getEncoding().toString()));
      aop.setTransport(TransportType.fromValue(op.getTransport().toString()));
      aop.setSingleThreadedOnOutput(op.isSingleThreadedOnOutput());
      ape.getOutputPorts().getOutputPort().add(aop);

      nextOutputPortIndex = nextOutputPortIndex.add(BigInteger.ONE);
    }
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Starting Pe.toXml() - Finished Ports");
    }

    Set<BigInteger> seen = new HashSet<BigInteger>();
    // OperDefinitionsType
    ape.setOperDefinitions(new OperDefinitionsType());
    for (BigInteger nodeIndex : fused.getNodeIndexes()) {
      TopologyNode tn =
          SAMStaticContext.getTopologyTracker()
              .getTopologyNode(fused.getTopologyApplicationId(), nodeIndex);
      BigInteger defIndex = tn.getOperDefinitionIndex();
      if (!seen.contains(defIndex)) {
        seen.add(defIndex);
        OperDefinitionType odt = new OperDefinitionType();
        odt.setKind(tn.getOperKind());
        odt.setIndex(defIndex);
        com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType ort =
            tn.getOperRuntimeType();
        if (ort == com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType.SPL_CPlusPlus) {
          odt.setRuntimeType(OperatorRuntimeType.SPL_C_PLUS_PLUS);
        } else if (ort
            == com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType.SPL_Java) {
          odt.setRuntimeType(OperatorRuntimeType.SPL_JAVA);
        } else {
          odt.setRuntimeType(OperatorRuntimeType.JAVA);
        }
        List<BigInteger> cmi = tn.getCustomMetrics();
        if (cmi != null && cmi.size() > 0) {
          CustomMetricIndiciesType cmis = new CustomMetricIndiciesType();
          odt.setCustomMetricIndicies(cmis);
          for (BigInteger metricIndex : cmi) {
            cmis.getCustomMetricIndex().add(metricIndex);
          }
        }
        ape.getOperDefinitions().getOperDefinition().add(odt);
      }
    }

    CustomMetricsType cmt = new CustomMetricsType();
    for (BigInteger cmIndex : fused.getCustomMetricIndicies()) {
      cmt.getCustomMetric().add(job.getCustomMetrics().get(cmIndex).toXml());
    }
    ape.setCustomMetrics(cmt);

    ape.setCurrentTracing(runPe.getCurrentTracing().toXml());
    // ape.setPendingTracing(runPe.getPendingTracing().toXml());
    ape.setPendingTracing(null);
    // ape.setNextTrcUpdtSeqNum(runPe.getNextTrcUpdateSeqNum());
    ape.setNextTrcUpdtSeqNum(null);

    // Toolkits Map
    //    ape.setToolkitsMap(new ToolkitsMapType());
    ape.setToolkitsMap(job.getToolkitMap().toXml());

    for (String s : fused.getCapabilities()) {
      ape.getCapability().add(s);
    }
    ape.setId(runPe.getId());
    ape.setJobId(runPe.getJobId());
    ape.setJobName(job.getName());
    ape.setUser(job.getUser());
    ape.setApplicationScope(job.getApplicationScope());
    ape.setClazz(fused.getClazz());

    // Log Level -> NULL (Deprecated)
    ape.setLanguage(
        LanguageType.fromValue(fused.getLanguage().toString())); // Guess what? Hardcoded to C++
    ape.setOptimized(fused.isOptimized());
    ape.setRestartable(fused.isRestartable());
    ape.setRelocatable(fused.isRelocatable());
    ape.setLaunchCount(runPe.getLaunchCount());
    ape.setDataDirectory(job.getDataDirectory());
    ape.setCurrentWorkingDirectory(runPe.getCurrentWorkingDirectory());
    ape.setApplicationDirectory(job.getApplicationDirectory());
    ape.setOutputDirectory(job.getOutputDirectory());
    ape.setCheckpointDirectory(job.getCheckpointDirectory());
    if (job.getDisplay() != null) {
      String displayVal = job.getDisplay().getValue();
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("DISPLAY value is " + displayVal);
      }
      if (displayVal.startsWith("\"") && displayVal.endsWith("\"")) {
        displayVal = displayVal.substring(1, displayVal.length() - 1);
      }
      ape.setDisplay(displayVal);
    } else {
      ape.setDisplay("");
    }
    ape.setAvoidRuntimeYield(AvoidRuntimeYieldType.valueOf(job.getYieldBehaviour().name()));

    // Many of these indexes are no longer applicable in Sopris, hardcoded to 0
    // ape.setLogicalIndex(BigInteger.ZERO);
    ape.setLogicalIndex(fused.getIndex()); // set it to contain's index
    //    ape.setChannelIndex(0); // Silly that this is an Integer and not BigInteger, I mean,
    // consistency please
    //    ape.setRegionIndex(BigInteger.ZERO);
    // BuildId
    ape.setBuildId(job.getBuildId());
    // bundle
    ape.setBundle(job.getBundlePath());
    // Profile Directory
    ape.setProfileDirectory("");
    ape.setVersion(job.getBundleVersion());
    ape.setApplicationName(job.getApplicationName());
    ape.setJobGroup(job.getJobGroup());
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("End Pe.toXml()");
    }
    return ape;
  }

  public void checkIfRestartable(PeStateReason stateReason)
      throws PECannotRestartException, StreamsException {
    if (!isRestartable()) {
      Vector<String> args = new Vector<>();
      args.add(getId().toString());
      args.add(getJobId().toString());
      SAMStaticContext.productLog(
          StreamsRuntimeMessagesKey.Key.SAMPENoRestart, getId().toString(), getJobId().toString());
      throw new PECannotRestartException(
          "PE " + getId() + " is not restartable", new DistilleryExceptionCode("CDISR1001E"), args);
    }

    int maximumPeRestartCount = SAMStaticContext.getMaximumPERestartCount();
    if (stateReason != PeStateReason.REQUEST
        && getSamRetryCount().compareTo(BigInteger.valueOf(maximumPeRestartCount)) >= 0) {
      Vector<String> args = new Vector<>();
      args.add(getId().toString());
      args.add(getSamRetryCount().toString());
      SAMStaticContext.productLog(
          StreamsRuntimeMessagesKey.Key.SAMPENoRestartRetryCount,
          getId().toString(),
          getSamRetryCount().toString());
      throw new PECannotRestartException(
          "Cannot restart PE " + getId() + ". Retry count exceeded.",
          new DistilleryExceptionCode("CDISR1142E"),
          args);
    }
  }

  public void setupDynConnections(boolean isRecovery) {
    getInputPorts().stream()
        .forEach(
            (ip) -> {
              if (ip != null) {
                ip.setupDynConnections(getJobId(), isRecovery);
              }
            });

    getOutputPorts().stream()
        .forEach(
            (op) -> {
              if (op != null) {
                op.setupDynConnections(getJobId(), isRecovery);
              }
            });
  }

  public void emitRoutingNotifications(RoutingNotificationType action) throws StreamsException {
    throw new RuntimeException("called Pe.emitRoutingNotifications");
  }

  // this method updates the connections and emits routing notifications
  // related to changes in the input port
  public synchronized void updateInputPortConnections(BigInteger ipid, boolean updateFilter)
      throws StreamsException {
    throw new RuntimeException("Pe.updateInputPortConnections called");
  }

  public synchronized void updateOutputPortConnections(BigInteger opid) throws StreamsException {
    throw new RuntimeException("Pe.updateOutputPortConnections called");
  }

  public void emitViewNotification(boolean start) {
    // this method start/stop views related to this specific PE
    ObjectTracker objectTracker = SAMStaticContext.getObjectTracker();
    for (BigInteger viewId : objectTracker.getViews()) {
      StreamView view = objectTracker.getView(viewId);
      if (start) {
        view.start(getId());
      } else {
        view.stop(getId());
      }
    }
  }

  public void placePE() throws DistilleryException {}

  public Set<String> getOperatorNames() {
    return new TreeSet<>(nodeMap.keySet());
  }

  @Override
  public int compareTo(Pe rhs) {
    return new CompareToBuilder().append(getId(), rhs.getId()).toComparison();
  }

  @Override
  public boolean isJobConstructed() {
    Job job = SAMStaticContext.getObjectTracker().getJob(getJobId());
    if (job == null) {
      return false;
    }
    return job.isConstructed();
  }

  @Override
  public int hashCode() {
    return Objects.hash(fused, runPe);
  }
}
