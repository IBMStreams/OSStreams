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

package com.ibm.streams.instance.sam;

import com.google.common.collect.HashBasedTable;
import com.google.common.collect.Table;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.model.Connection;
import com.ibm.streams.instance.sam.model.ConnectionId;
import com.ibm.streams.instance.sam.model.InputPort;
import com.ibm.streams.instance.sam.model.Job;
import com.ibm.streams.instance.sam.model.Operator;
import com.ibm.streams.instance.sam.model.OperatorId;
import com.ibm.streams.instance.sam.model.OutputPort;
import com.ibm.streams.instance.sam.model.Pe;
import com.ibm.streams.instance.sam.model.SamObject;
import com.ibm.streams.instance.sam.model.StreamView;
import com.ibm.streams.instance.sam.model.fuser.FusedApplication;
import com.ibm.streams.instance.sam.model.fuser.FusedContainer;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnection;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnectionId;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerInputPort;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerOutputPort;
import com.ibm.streams.instance.sam.model.runtime.PeStateReason;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnectionId;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import java.math.BigInteger;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.concurrent.ConcurrentHashMap;

public class ObjectTracker implements IDGeneratorFactory, FusedTracker, TopologyTracker {

  private BigInteger nextPeId;
  private BigInteger nextPortId;
  private BigInteger nextSvId;
  private BigInteger nextTopologyApplicationId;

  private transient BigInteger savedNextPeId;
  private transient BigInteger savedNextPortId;
  private transient BigInteger savedNextTopologyApplicationId;

  private transient Map<BigInteger, BigInteger> peToJob;
  private transient Map<BigInteger, BigInteger> portToPe;

  private transient Map<BigInteger, Job> jobs;
  private transient Map<BigInteger, Pe> pes;
  private transient Map<BigInteger, InputPort> ips;
  private transient Map<BigInteger, OutputPort> ops;
  private transient Map<ConnectionId, Connection> conns;
  private transient Map<BigInteger, StreamView> views;
  private transient Map<OperatorId, Operator> operators;

  private transient Map<BigInteger, PeStateReason> peStateForRestartPeMap;

  // ObjectTracker

  private transient Map<BigInteger, FusedApplicationData> fusedApplications;
  private transient Map<BigInteger, TopologyApplicationData> topologyApplications;

  public ObjectTracker() {
    nextPeId = BigInteger.ZERO;
    nextPortId = BigInteger.ZERO;
    nextSvId = BigInteger.ZERO;
    nextTopologyApplicationId = BigInteger.ZERO;

    peToJob = new ConcurrentHashMap<BigInteger, BigInteger>();
    portToPe = new ConcurrentHashMap<BigInteger, BigInteger>();

    jobs = new ConcurrentHashMap<BigInteger, Job>();
    pes = new ConcurrentHashMap<BigInteger, Pe>();
    operators = new ConcurrentHashMap<OperatorId, Operator>();
    ips = new ConcurrentHashMap<BigInteger, InputPort>();
    ops = new ConcurrentHashMap<BigInteger, OutputPort>();
    conns = new ConcurrentHashMap<ConnectionId, Connection>();
    views = new ConcurrentHashMap<BigInteger, StreamView>();

    peStateForRestartPeMap = Collections.synchronizedMap(new TreeMap<BigInteger, PeStateReason>());

    fusedApplications = new ConcurrentHashMap<BigInteger, FusedApplicationData>();
    topologyApplications = new ConcurrentHashMap<BigInteger, TopologyApplicationData>();
  }

  public BigInteger getJobIdFromPeId(BigInteger peId) {
    return peToJob.get(peId);
  }

  public BigInteger getJobIdFromPortId(BigInteger portId) {
    return peToJob.get(portToPe.get(portId));
  }

  public BigInteger getPeIdFromPortId(BigInteger portId) {
    return portToPe.get(portId);
  }

  public BigInteger getPeIdFromNodeIndex(BigInteger jobId, BigInteger nodeIndex) {
    for (BigInteger peId : SAMStaticContext.getObjectTracker().getPes(jobId)) {
      Pe pe = SAMStaticContext.getObjectTracker().getPe(peId);
      if (pe != null) {
        for (BigInteger nodeIdx : pe.getFusedContainer().getNodeIndexes()) {
          if (nodeIdx.equals(nodeIndex)) {
            return peId;
          }
        }
      }
    }
    return null;
  }

  public Job getJob(BigInteger jobId) {
    return jobs.get(jobId);
  }

  public Pe getPe(BigInteger peId) {
    throw new RuntimeException("called ObjectTracker.getPe(id)");
  }

  public Operator getOperator(OperatorId operatorId) {
    return operators.get(operatorId);
  }

  public InputPort getInputPort(BigInteger ipId) {
    throw new RuntimeException("called ObjectTracker.getInputPort(id)");
  }

  public BigInteger getInputPortIdFromNodePortIndex(
      BigInteger jobId, BigInteger nodeIndex, BigInteger portIndex) {
    for (BigInteger inputPortId : getIps(jobId)) {
      InputPort inputPort = getInputPort(inputPortId);
      if (inputPort.getNodeIndex().equals(nodeIndex)
          && inputPort.getNodePortIndex().equals(portIndex)) {
        return inputPortId;
      }
    }
    return null;
  }

  public BigInteger getInputPortIdFromInputPortIndex(BigInteger peId, BigInteger peInputPortIndex) {
    Pe pe = SAMStaticContext.getObjectTracker().getPe(peId);
    if (pe != null) {
      for (InputPort inputPort : pe.getInputPorts()) {
        if (inputPort.getIndex().equals(peInputPortIndex)) {
          return inputPort.getId();
        }
      }
    }
    return null;
  }

  public OutputPort getOutputPort(BigInteger opId) {
    throw new RuntimeException("called ObjectTracker.getOututPort(id)");
  }

  public BigInteger getOutputPortIdFromOutputPortIndex(
      BigInteger peId, BigInteger peOutputPortIndex) {
    Pe pe = SAMStaticContext.getObjectTracker().getPe(peId);
    if (pe != null) {
      for (OutputPort outputPort : pe.getOutputPorts()) {
        if (outputPort.getIndex().equals(peOutputPortIndex)) {
          return outputPort.getId();
        }
      }
    }
    return null;
  }

  public SortedSet<BigInteger> getOutputPortIdsFromNodePortIndex(
      BigInteger jobId, BigInteger nodeIndex, BigInteger nodePortIndex) {
    SortedSet<BigInteger> portIds = new TreeSet<>();
    for (BigInteger outputPortId : getOps(jobId)) {
      OutputPort outputPort = getOutputPort(outputPortId);
      if (outputPort.getNodeIndex().equals(nodeIndex)
          && outputPort.getNodePortIndex().equals(nodePortIndex)) {
        portIds.add(outputPortId);
      }
    }
    return portIds;
  }

  public SamObject getPort(BigInteger portId) {
    if (ips.containsKey(portId)) {
      return getInputPort(portId);
    } else if (ops.containsKey(portId)) {
      return getOutputPort(portId);
    } else {
      return null;
    }
  }

  public Connection getConnection(ConnectionId connId) {
    return conns.get(connId);
  }

  public boolean doesConnExist(ConnectionId connId) {
    return conns.containsKey(connId);
  }

  public Set<ConnectionId> getConnectionIds() {
    return conns.keySet();
  }

  public StreamView getView(BigInteger viewId) {
    return views.get(viewId);
  }

  public Set<BigInteger> getJobs() {
    return jobs.keySet();
  }

  public Set<BigInteger> getPes() {
    return pes.keySet();
  }

  public Set<BigInteger> getPes(BigInteger jobId) {
    HashSet<BigInteger> peIds = new HashSet<BigInteger>();
    for (BigInteger peId : peToJob.keySet()) {
      if (Objects.equals(peToJob.get(peId), jobId)) {
        peIds.add(peId);
      }
    }
    return peIds;
  }

  public Set<OperatorId> getOperators(BigInteger appId) {
    Set<OperatorId> operatorIds = new HashSet<>();
    for (OperatorId operatorId : operators.keySet()) {
      if (operatorId.getAppId().equals(appId)) {
        operatorIds.add(operatorId);
      }
    }
    return operatorIds;
  }

  public Set<BigInteger> getIps() {
    return ips.keySet();
  }

  public Set<BigInteger> getIps(BigInteger jobId) {
    Set<BigInteger> ipIds = new HashSet<>();
    for (BigInteger ipId : ips.keySet()) {
      if (peToJob.get(portToPe.get(ipId)).equals(jobId)) {
        ipIds.add(ipId);
      }
    }
    return ipIds;
  }

  public Set<BigInteger> getOps() {
    return ops.keySet();
  }

  public Set<BigInteger> getOps(BigInteger jobId) {
    Set<BigInteger> opIds = new HashSet<>();
    for (BigInteger opId : ops.keySet()) {
      if (peToJob.get(portToPe.get(opId)).equals(jobId)) {
        opIds.add(opId);
      }
    }
    return opIds;
  }

  public Set<BigInteger> getViews() {
    return views.keySet();
  }

  public void registerJob(Job job) {
    BigInteger jobId = job.getId();
    jobs.put(jobId, job);
  }

  public void unregisterJob(final BigInteger jobId) {
    /* No-op now. */
  }

  public void registerPe(Pe pe) {
    BigInteger peId = pe.getId();
    peToJob.put(pe.getId(), pe.getJobId());
    pes.put(peId, pe);
  }

  public void registerOperator(Operator operator) {
    operators.put(operator.getOperatorId(), operator);
  }

  public void registerInputPort(InputPort ip) {
    BigInteger ipId = ip.getId();
    portToPe.put(ipId, ip.getPeId());
    ips.put(ipId, ip);
  }

  public void registerOutputPort(OutputPort op) {
    BigInteger opId = op.getId();
    portToPe.put(opId, op.getPeId());
    ops.put(opId, op);
  }

  public void registerConnection(Connection conn) {
    conns.put(conn.getConnectionId(), conn);
  }

  public void registerStreamView(StreamView sv) {
    BigInteger viewId = sv.getId();
    views.put(viewId, sv);
  }

  public void unregisterStreamView(BigInteger viewId) {}

  public synchronized SortedSet<BigInteger> newPeIds(BigInteger blockSize) {
    BigInteger newNextPeId = nextPeId.add(blockSize);

    // TODO: Enable this when Guava v15 or higher is added
    // SortedSet<BigInteger> rval = ContiguousSet.create(Range.closed(nextPeId, newNextPeId),
    // DiscreteDomain
    // .bigIntegers());
    // Until then.... manual looping :(

    SortedSet<BigInteger> rval = new TreeSet<>();
    while (!nextPeId.equals(newNextPeId)) {
      rval.add(nextPeId);
      nextPeId = nextPeId.add(BigInteger.ONE);
    }
    return rval;
  }

  public synchronized SortedSet<BigInteger> newPortIds(BigInteger blockSize) {
    BigInteger newNextPortId = nextPortId.add(blockSize);
    SortedSet<BigInteger> rval = new TreeSet<>();
    while (!nextPortId.equals(newNextPortId)) {
      rval.add(nextPortId);
      nextPortId = nextPortId.add(BigInteger.ONE);
    }
    return rval;
  }

  public synchronized BigInteger newStreamViewId() {
    BigInteger rval = nextSvId;
    nextSvId = nextSvId.add(BigInteger.ONE);
    return rval;
  }

  ////////////////////////////////////
  /// Fused Tracker
  ////////////////////////////////////
  private static class FusedApplicationData {
    public FusedApplicationData() {
      fusedContainers = new HashMap<>();
      fusedContainerInputPorts =
          HashBasedTable.<BigInteger, BigInteger, FusedContainerInputPort>create();
      fusedContainerOutputPorts =
          HashBasedTable.<BigInteger, BigInteger, FusedContainerOutputPort>create();
      fusedContainerConnections = new HashMap<>();
    }

    public FusedApplication fusedApplication;
    public Map<BigInteger, FusedContainer> fusedContainers;

    // Row, Column, Value = Container Index, Port Index, Port Object
    public Table<BigInteger, BigInteger, FusedContainerInputPort> fusedContainerInputPorts;
    public Table<BigInteger, BigInteger, FusedContainerOutputPort> fusedContainerOutputPorts;
    public Map<FusedContainerConnectionId, FusedContainerConnection> fusedContainerConnections;
  }

  public FusedApplication getFusedApplication(BigInteger appId) {
    return fusedApplications.get(appId).fusedApplication;
  }

  public void registerFusedApplication(BigInteger appId, FusedApplication app) {
    FusedApplicationData fad = new FusedApplicationData();
    fad.fusedApplication = app;
    fusedApplications.put(appId, fad);
  }

  public void unregisterFusedApplication(BigInteger appId) {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug(
          "ENTER ObjectTracker.unregisterFusedApplication with "
              + Arrays.deepToString(new Object[] {appId}));
    }
    if (fusedApplications.get(appId) != null) {
      // unregister fused containers
      for (FusedContainer fc : getFusedContainers(appId)) {
        unregisterFusedContainer(appId, fc.getIndex(), true);
      }

      // unregister fused application
      fusedApplications.remove(appId);
    }
  }

  public SortedSet<FusedContainer> getFusedContainers(BigInteger appId) {
    return new TreeSet<FusedContainer>(fusedApplications.get(appId).fusedContainers.values());
  }

  public FusedContainer getFusedContainer(BigInteger appId, BigInteger containerIndex) {
    return fusedApplications.get(appId).fusedContainers.get(containerIndex);
  }

  public FusedContainer unregisterFusedContainer(
      BigInteger appId, BigInteger containerIndex, boolean removingJob) {
    FusedContainer fc = getFusedContainer(appId, containerIndex);
    if (fc != null) {
      // unregister fused container input ports
      for (FusedContainerInputPort fcip : getFusedContainerInputPorts(appId, containerIndex)) {
        unregisterFusedContainerInputPort(appId, containerIndex, fcip.getIndex(), removingJob);
      }
      // unregister fused container output ports
      for (FusedContainerOutputPort fcop : getFusedContainerOutputPorts(appId, containerIndex)) {
        unregisterFusedContainerOutputPort(appId, containerIndex, fcop.getIndex(), removingJob);
      }
    }
    return fusedApplications.get(appId).fusedContainers.remove(containerIndex);
  }

  public void registerFusedContainer(BigInteger appId, FusedContainer container) {
    fusedApplications.get(appId).fusedContainers.put(container.getIndex(), container);
  }

  public SortedSet<FusedContainerInputPort> getFusedContainerInputPorts(
      BigInteger appId, BigInteger containerIndex) {
    return new TreeSet<>(
        fusedApplications.get(appId).fusedContainerInputPorts.row(containerIndex).values());
  }

  public FusedContainerInputPort getFusedContainerInputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex) {
    return fusedApplications.get(appId).fusedContainerInputPorts.get(containerIndex, portIndex);
  }

  public FusedContainerInputPort unregisterFusedContainerInputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex, boolean removingJob) {
    FusedContainerInputPort fcip = getFusedContainerInputPort(appId, containerIndex, portIndex);
    if (fcip != null && !removingJob) {
      for (FusedContainerConnectionId fusedConnId : fcip.getContainerConnections()) {
        // remove fused connection from output port
        FusedContainerOutputPort fcop =
            getFusedContainerOutputPort(
                fusedConnId.getAppId(),
                fusedConnId.getSourceContainerIndex(),
                fusedConnId.getSourceContainerPortIndex());
        if (fcop != null) {
          fcop.removeContainerConnection(fusedConnId);
        }
        // unregister fused connections
        unregisterFusedContainerConnection(appId, fusedConnId);
      }
    }
    return fusedApplications.get(appId).fusedContainerInputPorts.remove(containerIndex, portIndex);
  }

  public void registerFusedContainerInputPort(
      BigInteger appId, BigInteger containerIndex, FusedContainerInputPort port) {
    fusedApplications
        .get(appId)
        .fusedContainerInputPorts
        .put(containerIndex, port.getIndex(), port);
  }

  public SortedSet<FusedContainerOutputPort> getFusedContainerOutputPorts(
      BigInteger appId, BigInteger containerIndex) {
    return new TreeSet<>(
        fusedApplications.get(appId).fusedContainerOutputPorts.row(containerIndex).values());
  }

  public FusedContainerOutputPort getFusedContainerOutputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex) {
    return fusedApplications.get(appId).fusedContainerOutputPorts.get(containerIndex, portIndex);
  }

  public FusedContainerOutputPort unregisterFusedContainerOutputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex, boolean removingJob) {
    FusedContainerOutputPort fcop = getFusedContainerOutputPort(appId, containerIndex, portIndex);
    if (fcop != null) {
      for (FusedContainerConnectionId fusedConnId : fcop.getContainerConnections()) {
        if (!removingJob) {
          // remove fused connection from input port
          FusedContainerInputPort fcip =
              getFusedContainerInputPort(
                  fusedConnId.getAppId(),
                  fusedConnId.getDestinationContainerIndex(),
                  fusedConnId.getDestinationContainerPortIndex());
          if (fcip != null) {
            fcip.removeContainerConnection(fusedConnId);
          }
        }
        // unregister fused connections
        unregisterFusedContainerConnection(appId, fusedConnId);
      }
    }
    return fusedApplications.get(appId).fusedContainerOutputPorts.remove(containerIndex, portIndex);
  }

  public void registerFusedContainerOutputPort(
      BigInteger appId, BigInteger containerIndex, FusedContainerOutputPort port) {
    fusedApplications
        .get(appId)
        .fusedContainerOutputPorts
        .put(containerIndex, port.getIndex(), port);
  }

  public Set<FusedContainerConnection> getFusedContainerConnections(BigInteger appId) {
    return new HashSet<FusedContainerConnection>(
        fusedApplications.get(appId).fusedContainerConnections.values());
  }

  public FusedContainerConnection getFusedContainerConnection(
      BigInteger appId, FusedContainerConnectionId connId) {
    return fusedApplications.get(appId).fusedContainerConnections.get(connId);
  }

  public FusedContainerConnection unregisterFusedContainerConnection(
      BigInteger appId, FusedContainerConnectionId connId) {
    FusedContainerConnection conn =
        fusedApplications.get(appId).fusedContainerConnections.remove(connId);
    if (Trace.isEnabled(Level.TRACE) && conn != null) {
      Trace.logTrace("Unregistered fused connection " + conn.getId());
    }
    return conn;
  }

  public void registerFusedContainerConnection(BigInteger appId, FusedContainerConnection conn) {
    fusedApplications.get(appId).fusedContainerConnections.put(conn.getId(), conn);
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("Registered fused connection " + conn.getId());
    }
  }

  ////////////////////////////////////
  /// Topology Tracker
  ////////////////////////////////////
  private static class TopologyApplicationData {

    public TopologyApplication topologyApplication;
    public Map<BigInteger, TopologyNode> topologyNodes;

    // Row, Column, Value = Container Index, Port Index, Port Object
    public Table<BigInteger, BigInteger, TopologyNodeOutputPort> topologyNodeOutputPorts;
    public Table<BigInteger, BigInteger, TopologyNodeInputPort> topologyNodeInputPorts;
    public Map<TopologyNodeConnectionId, TopologyNodeConnection> topologyNodeConnections;

    public TopologyApplicationData() {
      topologyNodes = new HashMap<>();
      topologyNodeOutputPorts =
          HashBasedTable.<BigInteger, BigInteger, TopologyNodeOutputPort>create();
      topologyNodeInputPorts =
          HashBasedTable.<BigInteger, BigInteger, TopologyNodeInputPort>create();
      topologyNodeConnections = new HashMap<>();
    }
  }

  public synchronized void setTopologyApplicationId(BigInteger id) {
    nextTopologyApplicationId = id;
  }

  public synchronized BigInteger newTopologyApplicationId() {
    BigInteger rval = null;
    rval = nextTopologyApplicationId;
    nextTopologyApplicationId = nextTopologyApplicationId.add(BigInteger.ONE);
    return rval;
  }

  @Override
  public BigInteger getLastTopologyApplicationId() {
    return nextTopologyApplicationId.subtract(BigInteger.ONE);
  }

  public Set<BigInteger> getTopologyApplications() {
    return topologyApplications.keySet();
  }

  public TopologyApplication getTopologyApplication(BigInteger appId) {
    return topologyApplications.get(appId).topologyApplication;
  }

  public void registerTopologyApplication(BigInteger appId, TopologyApplication app) {
    topologyApplications.computeIfAbsent(appId, k -> new TopologyApplicationData());
    topologyApplications.get(appId).topologyApplication = app;
  }

  public void unregisterTopologyApplication(BigInteger appId) {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug(
          "ENTER ObjectTracker.unregisterTopologyApplication with "
              + Arrays.deepToString(new Object[] {appId}));
    }
    topologyApplications.remove(appId);
  }

  public static SortedMap<String, TopologyNode> getOperatorLookupMap(TopologyApplication ta) {
    SortedMap<String, TopologyNode> nodeNametoNodeMap = new TreeMap<>();
    SortedSet<TopologyNode> nodes = ta.getNodes();
    for (TopologyNode node : nodes) {
      nodeNametoNodeMap.put(node.getName(), node);
    }
    return nodeNametoNodeMap;
  }

  public SortedSet<TopologyNode> getTopologyNodes(BigInteger appId) {
    return new TreeSet<TopologyNode>(topologyApplications.get(appId).topologyNodes.values());
  }

  public TopologyNode getTopologyNode(BigInteger appId, BigInteger nodeIndex) {
    return topologyApplications.get(appId).topologyNodes.get(nodeIndex);
  }

  public TopologyNode getTopologyNode(BigInteger appId, String nodeName) {
    for (TopologyNode tn : topologyApplications.get(appId).topologyNodes.values()) {
      if (tn.getName().equals(nodeName)) {
        return tn;
      }
    }
    return null;
  }

  public BigInteger getTopologyNodeIndexByName(BigInteger jobId, String nodeName) {
    Job job = SAMStaticContext.getObjectTracker().getJob(jobId);
    for (BigInteger peId : job.getPeIds()) {
      Pe pe = SAMStaticContext.getObjectTracker().getPe(peId);
      for (TopologyNode tn : pe.getNodes()) {
        if (tn.getName().equals(nodeName) || tn.getLogicalName().equals(nodeName)) {
          return tn.getIndex();
        }
      }
    }
    return null;
  }

  public void registerTopologyNode(BigInteger appId, TopologyNode node) {
    if (topologyApplications.get(appId) == null) {
      throw new IllegalStateException("TopologyApplicationData appId=" + appId + " is null");
    }
    topologyApplications.get(appId).topologyNodes.put(node.getIndex(), node);
  }

  public void unregisterTopologyNode(BigInteger appId, BigInteger nodeIndex) {
    topologyApplications.get(appId).topologyNodes.remove(nodeIndex);
  }

  public SortedSet<TopologyNodeInputPort> getTopologyNodeInputPorts(
      BigInteger appId, BigInteger nodeIndex) {
    return new TreeSet<>(
        topologyApplications.get(appId).topologyNodeInputPorts.row(nodeIndex).values());
  }

  public TopologyNodeInputPort getTopologyNodeInputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex) {
    return topologyApplications.get(appId).topologyNodeInputPorts.get(nodeIndex, portIndex);
  }

  public void registerTopologyNodeInputPort(
      BigInteger appId, BigInteger nodeIndex, TopologyNodeInputPort port) {
    topologyApplications.get(appId).topologyNodeInputPorts.put(nodeIndex, port.getIndex(), port);
  }

  public void unregisterTopologyNodeInputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex) {
    topologyApplications.get(appId).topologyNodeInputPorts.remove(nodeIndex, portIndex);
  }

  public SortedSet<TopologyNodeOutputPort> getTopologyNodeOutputPorts(
      BigInteger appId, BigInteger nodeIndex) {
    return new TreeSet<>(
        topologyApplications.get(appId).topologyNodeOutputPorts.row(nodeIndex).values());
  }

  public TopologyNodeOutputPort getTopologyNodeOutputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex) {
    return topologyApplications.get(appId).topologyNodeOutputPorts.get(nodeIndex, portIndex);
  }

  public void registerTopologyNodeOutputPort(
      BigInteger appId, BigInteger nodeIndex, TopologyNodeOutputPort port) {
    topologyApplications.get(appId).topologyNodeOutputPorts.put(nodeIndex, port.getIndex(), port);
  }

  public void unregisterTopologyNodeOutputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex) {
    topologyApplications.get(appId).topologyNodeOutputPorts.remove(nodeIndex, portIndex);
  }

  public Set<TopologyNodeConnectionId> getTopologyNodeConnectionIds(BigInteger appId) {
    return new TreeSet<TopologyNodeConnectionId>(
        topologyApplications.get(appId).topologyNodeConnections.keySet());
  }

  public Set<TopologyNodeConnection> getTopologyNodeConnections(BigInteger appId) {
    return new HashSet<TopologyNodeConnection>(
        topologyApplications.get(appId).topologyNodeConnections.values());
  }

  public TopologyNodeConnection getTopologyNodeConnection(
      BigInteger appId, TopologyNodeConnectionId connId) {
    return topologyApplications.get(appId).topologyNodeConnections.get(connId);
  }

  public void registerTopologyNodeConnection(BigInteger appId, TopologyNodeConnection conn) {
    topologyApplications.get(appId).topologyNodeConnections.put(conn.getId(), conn);
  }

  public TopologyNodeConnection unregisterTopologyNodeConnection(
      BigInteger appId, TopologyNodeConnectionId connId) {
    return topologyApplications.get(appId).topologyNodeConnections.remove(connId);
  }
}
