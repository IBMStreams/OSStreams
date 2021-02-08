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

package com.ibm.streams.instance.sam.model.fuser;

import com.google.common.collect.ImmutableSortedSet;
import com.google.common.collect.Sets;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.composer.ContainerSpecification;
import com.ibm.streams.instance.sam.model.runtime.RuntimeStream;
import com.ibm.streams.instance.sam.model.runtime.StreamId;
import com.ibm.streams.instance.sam.model.runtime.Tracing;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import com.ibm.streams.instance.sam.model.topology.CongestionPolicy;
import com.ibm.streams.instance.sam.model.topology.ConnectionSource;
import com.ibm.streams.instance.sam.model.topology.DeploymentDescriptor;
import com.ibm.streams.instance.sam.model.topology.Language;
import com.ibm.streams.instance.sam.model.topology.ParallelChannel;
import com.ibm.streams.instance.sam.model.topology.Splitter;
import com.ibm.streams.instance.sam.model.topology.ThreadedPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnectionId;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodePort;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleTypeType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;
import org.apache.commons.lang.builder.CompareToBuilder;
import org.apache.commons.lang.builder.EqualsBuilder;

public class FusedContainer implements Comparable<FusedContainer> {

  // The FusedContainer is a container for all the nodes
  // and any properties that need to be summarized for any set of Nodes
  // This means things like Libraries, Hostpools, Tuple Types,
  // Environment Variables, Runtime Constants, etc
  private BigInteger index;

  private BigInteger topologyAppId;
  private Set<BigInteger> nodes;

  private Map<String, String> envVars;
  private List<String> capabilities;
  private Language language;
  private boolean optimized;
  private boolean restartable;
  private boolean relocatable;
  private String clazz; // Not sure how to build from Oper Class, looks randomly generated in Spree
  // private String version; // probably not needed here
  // Toolkits?
  private SortedSet<BigInteger> customMetricIndices;

  private BigInteger appId;
  // TODO: With early release, we support tracing with format of <level>:<size>:<style>, but
  // think we actually never use it.
  // For now, just process <level>
  private String tracing;

  // Index Based Maps
  // TODO: FIX THESE MAPS
  private Map<BigInteger, FusedContainerInputPort> containerInputPorts;
  private Map<BigInteger, FusedContainerOutputPort> containerOutputPorts;

  private transient Map<TopologyNodeConnection, FusedContainerInputPort> containerInputPortConnRef;
  private transient Map<ConnectionSource, FusedContainerOutputPort> containerOutputPortConnRef;

  private BigInteger nextContainerInputPortIndex;
  private BigInteger nextContainerOutputPortIndex;
  private String containerSpecificationId;

  // Connections
  // All FusedContainerConnections are also TopologyNodeConnections
  // It is just a subset that is Inter-PE
  // Both subsets are broken out here
  private transient Set<TopologyNodeConnection> allConnections;
  private transient Set<TopologyNodeConnection> intraConns;
  private transient Set<TopologyNodeConnection> interConns;
  private transient ContainerSpecification cs;

  public FusedContainer(BigInteger appId, ContainerSpecification cs, BigInteger index) {
    // Construct FusedContainer from Composed produced specification
    this.cs = cs;
    this.containerSpecificationId = cs.getId();
    this.index = index;
    this.nextContainerInputPortIndex = BigInteger.ZERO;
    this.nextContainerOutputPortIndex = BigInteger.ZERO;
    this.appId = appId;
    nodes = new HashSet<>();

    containerInputPorts = new HashMap<>();
    containerOutputPorts = new HashMap<>();

    containerInputPortConnRef = new HashMap<>();
    containerOutputPortConnRef = new HashMap<>();

    envVars = new HashMap<>();
    customMetricIndices = new TreeSet<>();
    capabilities = new ArrayList<>();
    clazz = "";
    tracing = "off";

    SortedSet<TopologyNode> topNodes = cs.getNodes();
    if (topNodes != null) {
      // populate other container fields
      optimized = true;
      restartable = true;
      relocatable = true;
      language = Language.Java;
      for (TopologyNode tNode : topNodes) {
        nodes.add(tNode.getIndex());
        DeploymentDescriptor desc = tNode.getDeploymentDesc();
        envVars.putAll(desc.getEnvVars());
        customMetricIndices.addAll(tNode.getCustomMetrics());
        if (language == Language.Java && desc.getLanguage() == Language.C) {
          language = Language.C;
        }
        if (optimized && !desc.isOptimized()) {
          optimized = false;
        }
        if (restartable && !tNode.isRestartable()) {
          restartable = false;
        }
        if (relocatable && !tNode.isRelocatable()) {
          relocatable = false;
        }
        // get tracing config ...
        if (tNode.getTracing() != null) {
          try {
            if (Tracing.Level.fromValue(tNode.getTracing())
                    .compareTo(Tracing.Level.fromValue(tracing))
                > 0) {
              tracing = tNode.getTracing();
            }
          } catch (IllegalArgumentException e) {
            // trace and ignore...
            if (Trace.isEnabled(Level.WARN)) {
              Trace.logWarn(
                  "hit IllegalArgumentException when processing "
                      + tNode.getTracing()
                      + ": "
                      + e.getMessage());
            }
          }
        }
        // capabilities
        if (tNode.getCapabilities() != null && !tNode.getCapabilities().isEmpty()) {
          capabilities.addAll(tNode.getCapabilities());
        }
      }

      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Getting Topology Application ID");
      }
      topologyAppId = topNodes.iterator().next().getApplicationId();

      collectTopologyConnections();
      buildFusedPorts();
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Fused Ports Built");
      }
      injectThreadedPorts();
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Injected Threaded Ports");
      }
      FusedThreadPropagator threader = new FusedThreadPropagator(this);
      threader.propagateThreadsToPorts();
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Propagated threads to Ports");
      }
    } else {
      Trace.logError("No TopologyNodes present in the container");
    }

    SAMStaticContext.getFusedTracker().registerFusedContainer(appId, this);
  }

  public ContainerSpecification getContainerSpecification() {
    return cs;
  }

  public BigInteger getIndex() {
    return index;
  }

  public Set<BigInteger> getNodeIndexes() {
    return nodes;
  }

  public Map<BigInteger, FusedContainerInputPort> getContainerInputPorts() {
    return containerInputPorts;
  }

  public Map<BigInteger, FusedContainerOutputPort> getContainerOutputPorts() {
    return containerOutputPorts;
  }

  public Set<TopologyNodeConnection> getIntraContainerConnections() {
    return intraConns;
  }

  public Set<TopologyNodeConnection> getInterContainerConnections() {
    return interConns;
  }

  FusedContainerOutputPort getFusedContainerOutputPort(ConnectionSource conn) {
    return containerOutputPortConnRef.get(conn);
  }

  FusedContainerInputPort getFusedContainerInputPort(TopologyNodeConnection conn) {
    return containerInputPortConnRef.get(conn);
  }

  private void collectTopologyConnections() {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "ENTER FusedContainer.collectTopologyConnections topologyAppId="
              + topologyAppId.toString()
              + " this.index="
              + this.index.toString());
    }
    allConnections = new HashSet<>();

    // Visit every node and every input and output port
    // collect all their connections into a set (no duplicates)
    for (BigInteger nodeIndex : getNodeIndexes()) {
      TopologyNode tn = SAMStaticContext.getTopologyTracker().getTopologyNode(appId, nodeIndex);
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace(
            "CollectTopologyConnections TopologyNode Index: "
                + nodeIndex
                + " Name: "
                + tn.getName());
      }
      for (TopologyNodePort tnp : tn.getOutputPorts().values())
        allConnections.addAll(tnp.getOperConnections());
      for (TopologyNodePort tnp : tn.getInputPorts().values())
        allConnections.addAll(tnp.getOperConnections());
    }

    // Walk the set (order doesn't matter)
    // See if the Source or Destination is in our container
    // Inter-PE Conns are a simple operation of set difference
    // between All and Intra-PE Conns
    intraConns = new HashSet<>();

    for (TopologyNodeConnection tnc : allConnections) {
      if (nodes.contains(tnc.getId().getSrcNodeIndex())
          && nodes.contains(tnc.getId().getDstNodeIndex())) intraConns.add(tnc);
    }

    interConns = Sets.difference(allConnections, intraConns);
  }

  // for recovery
  public void populateTransientFields() {
    // rebuild allConnections, intraConns, interConns
    collectTopologyConnections();
    buildContainerSpecification();
    // rebuild containerInputPortConnRef, containerOutputPortConnRef maps
    buildPortConnRefs();
  }

  private void buildPortConnRefs() {
    containerInputPortConnRef = new HashMap<>();
    for (FusedContainerInputPort fcip : containerInputPorts.values()) {
      for (FusedContainerConnectionId fusedConnId : fcip.getContainerConnections()) {
        FusedContainerConnection fusedConn =
            SAMStaticContext.getFusedTracker().getFusedContainerConnection(appId, fusedConnId);
        if (fusedConn == null) {
          Trace.logError("FusedContainerConnection is null, fusedConnId: " + fusedConnId);
        }
        TopologyNodeConnectionId tConnId = fusedConn.getTopologyNodeConnectionId();
        TopologyNodeConnection tConn =
            SAMStaticContext.getTopologyTracker().getTopologyNodeConnection(topologyAppId, tConnId);
        if (tConn == null) {
          Trace.logError("TopologyNodeConnection is null, tConnId: " + tConnId);
        }
        containerInputPortConnRef.put(tConn, fcip);
      }
    }

    containerOutputPortConnRef = new HashMap<>();
    for (FusedContainerOutputPort fcop : containerOutputPorts.values()) {
      for (FusedContainerConnectionId fusedConnId : fcop.getContainerConnections()) {
        FusedContainerConnection fusedConn =
            SAMStaticContext.getFusedTracker().getFusedContainerConnection(appId, fusedConnId);
        if (fusedConn == null) {
          Trace.logError("FusedContainerConnection is null, fusedConnId: " + fusedConnId);
        }
        TopologyNodeConnectionId tConnId = fusedConn.getTopologyNodeConnectionId();
        TopologyNodeConnection tConn =
            SAMStaticContext.getTopologyTracker().getTopologyNodeConnection(topologyAppId, tConnId);
        if (tConn == null) {
          Trace.logError("TopologyNodeConnection is null, tConnId: " + tConnId);
        }
        TopologyNodeOutputPort tnop = tConn.getSource().getOutputPort();
        if (tConn.getSplitterIndex() != null && tConn.getChannelIndex() != null) {
          // connection associated with splitter
          Splitter splitter = tnop.getPortSplittersMap().get(tConn.getSplitterIndex());
          if (splitter == null) {
            Trace.logError(
                "Splitter is null, TopologyNodeOutputPort splitters map: "
                    + tnop.getPortSplittersMap());
          }
          ParallelChannel channel = splitter.getParallelChannels().get(tConn.getChannelIndex());
          if (channel == null) {
            Trace.logError(
                "ParallelChannel is null, TopologyNodeOutputPort splitters map: "
                    + tnop.getPortSplittersMap());
          }
          containerOutputPortConnRef.putIfAbsent(channel, fcop);
        } else {
          // connection not associated with splitter
          containerOutputPortConnRef.putIfAbsent(tnop, fcop);
        }
      }
    }
  }

  private void buildContainerSpecification() {
    SortedSet<TopologyNode> topologyNodes = new TreeSet<>();
    for (BigInteger nodeIndex : nodes) {
      TopologyNode tn = SAMStaticContext.getTopologyTracker().getTopologyNode(appId, nodeIndex);
      topologyNodes.add(tn);
    }
    cs = new ContainerSpecification(containerSpecificationId, topologyNodes);
  }

  private void buildFusedPorts() {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "ENTER FusedContainer.buildFusedPorts topologyAppId="
              + topologyAppId.toString()
              + " this.index="
              + this.index.toString());
    }
    Map<ConnectionSource, FusedContainerOutputPort> consumedTopologyOutputPorts = new HashMap<>();
    SortedMap<TopologyNodeInputPort, FusedContainerInputPort> consumedTopologyInputPorts =
        new TreeMap<>();

    // Connections are all unique by virtue of being in a Set
    // Input Ports follow a rule of 1 connection per PE Port, but multiple per Operator Port
    // Output Ports follow a rule of many connections per PE Port, but 1 per Operator Port

    // build ports from pe connections
    for (TopologyNodeConnection conn : interConns) {
      if (nodes.contains(conn.getSource().getOwningOperator().getIndex())) {
        // Output Port, because we own the the source
        TopologyNodeOutputPort tnop = conn.getSource().getOutputPort();

        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("Building FusedContainerOutputPort using " + conn.getId().toString());
        }

        // If this connection is associated with a splitter, then we need to create PE ports for
        // just this connection's channel;
        // If not, then just create a normal PE output port for the connection.
        if (conn.getSplitterIndex() != null && conn.getChannelIndex() != null) {
          if (Trace.isEnabled(Level.TRACE)) {
            Trace.logTrace(
                "Connection Splitter Index: "
                    + conn.getSplitterIndex()
                    + " Channel Index: "
                    + conn.getChannelIndex());
            Trace.logTrace("TopologyNodeOutputPort Splitters Count: " + tnop.getSplitters().size());
          }
          Splitter splitter = tnop.getPortSplittersMap().get(conn.getSplitterIndex());
          if (splitter == null) {
            Trace.logError(
                "TopologyNodeOutputPort splitter is null, TopologyNodeOutputPort splitters map: "
                    + tnop.getPortSplittersMap());
          }

          ParallelChannel channel = splitter.getParallelChannels().get(conn.getChannelIndex());
          // multiple connections can be on one channel, so make sure we only create one output port
          // per channel
          if (!consumedTopologyOutputPorts.containsKey(channel)) {
            BigInteger index = getNewContainerOutputPortIndex();
            FusedContainerOutputPort fcop =
                new FusedContainerOutputPort(
                    tnop,
                    index,
                    getIndex(),
                    appId,
                    conn.getSplitterIndex(),
                    conn.getChannelIndex());
            if (Trace.isEnabled(Level.TRACE)) {
              Trace.logTrace("Creating FusedContainerOutputPort for parallel channel...");
            }
            consumedTopologyOutputPorts.put(channel, fcop);
            containerOutputPortConnRef.put(channel, fcop);
            containerOutputPorts.put(index, fcop);
          }
        } else if (!consumedTopologyOutputPorts.containsKey(tnop)) {
          BigInteger index = getNewContainerOutputPortIndex();
          FusedContainerOutputPort fcop =
              new FusedContainerOutputPort(tnop, index, getIndex(), appId);
          if (Trace.isEnabled(Level.TRACE)) {
            Trace.logTrace("Creating FusedContainerOutputPort...");
          }
          consumedTopologyOutputPorts.put(tnop, fcop);
          containerOutputPortConnRef.put(tnop, fcop);
          containerOutputPorts.put(index, fcop);
        }
      }
      if (nodes.contains(conn.getDestination().getOwningOperator().getIndex())) {
        // Input Port, because we own the destination
        TopologyNodeInputPort tnip = conn.getDestination().getInputPort();

        if (!consumedTopologyInputPorts.containsKey(tnip)) {
          BigInteger index = getNewContainerInputPortIndex();
          FusedContainerInputPort fcip =
              new FusedContainerInputPort(tnip, index, getIndex(), appId);
          if (Trace.isEnabled(Level.TRACE)) {
            Trace.logTrace("Creating FusedContainerInputPort...");
          }
          containerInputPorts.put(index, fcip);
          consumedTopologyInputPorts.put(tnip, fcip);
        }

        containerInputPortConnRef.put(conn, consumedTopologyInputPorts.get(tnip));
      }
    }

    // build pe ports from import/export
    for (BigInteger nodeIndex : nodes) {
      TopologyNode tn = SAMStaticContext.getTopologyTracker().getTopologyNode(appId, nodeIndex);
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("TopologyNode Index: " + nodeIndex + " Name: " + tn.getName());
      }

      // search operator output ports for exported stream
      for (TopologyNodeOutputPort tnop : tn.getOutputPorts().values()) {
        if (tnop.getExport() != null) {
          // if there isn't already a pe output port, create
          if (!consumedTopologyOutputPorts.containsKey(tnop)) {
            BigInteger index = getNewContainerOutputPortIndex();
            FusedContainerOutputPort fcop =
                new FusedContainerOutputPort(tnop, index, getIndex(), appId);
            if (Trace.isEnabled(Level.TRACE)) {
              Trace.logTrace("Creating FusedContainerOutputPort...");
            }
            consumedTopologyOutputPorts.put(tnop, fcop);
            containerOutputPortConnRef.put(tnop, fcop);
            containerOutputPorts.put(index, fcop);
          }
        }
      }

      // search operator input ports for imported streams
      for (TopologyNodeInputPort tnip : tn.getInputPorts().values()) {
        if (tnip.getImports() != null
            && (tnip.getImports().getPropertyBasedImport() != null
                || tnip.getImports().getNameBasedImports().size() > 0)) {
          // if there isn't already a pe input port, create
          if (!consumedTopologyInputPorts.containsKey(tnip)) {
            BigInteger index = getNewContainerInputPortIndex();
            FusedContainerInputPort fcip =
                new FusedContainerInputPort(tnip, index, getIndex(), appId);
            if (Trace.isEnabled(Level.TRACE)) {
              Trace.logTrace("Creating FusedContainerInputPort...");
            }
            containerInputPorts.put(index, fcip);
            consumedTopologyInputPorts.put(tnip, fcip);
          }
        }
      }
    }
  }

  private void injectThreadedPorts() {
    // Walk all internal connections, looking to see if we should inject a threaded port. We
    // have two reasons to do so: UDP and the "dedicated" threading model.
    for (TopologyNodeConnection conn : intraConns) {
      TopologyNodeInputPort tnip = conn.getDestination().getInputPort();

      // UDP case: if we have a splitter, and there is not already a threaded port, inject it
      if (conn.getSplitterIndex() != null
          && conn.getChannelIndex() != null
          && tnip.getThreadedPort() == null) {
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace(
              "Injecting threaded port for splitter "
                  + conn.getSplitterIndex().toString()
                  + " on channel "
                  + conn.getChannelIndex().toString());
        }
        tnip.setThreadedPort(
            new ThreadedPort(
                true,
                CongestionPolicy.wait,
                null /* -> no queue size, use default */,
                true /* yes, this is injected */));
      }

      // dedicated threading model case
      TopologyNode tn = conn.getDestination().getOwningOperator();
      for (Annotation annot : tn.getAnnotations()) {
        if (annot.getTag().equals("threading")) {
          for (AnnotationKeyValue kv : annot.getKeyValues()) {
            if (kv.getKey().equals("model")
                && kv.getValue().toString().equals("dedicated")
                && tnip.getThreadedPort() == null) {
              if (Trace.isEnabled(Level.TRACE)) {
                Trace.logTrace(
                    "Injecting threaded port for dedicated threading model for operator "
                        + tn.getIndex().toString());
              }
              tnip.setThreadedPort(
                  new ThreadedPort(
                      true,
                      CongestionPolicy.wait,
                      null /* -> no queue size, use default */,
                      true /* yes, this is injected */));
            }
          }
        }
      }
    }
  }

  public Map<String, String> getEnvVars() {
    return Collections.unmodifiableMap(envVars);
  }

  public List<String> getCapabilities() {
    return Collections.unmodifiableList(capabilities);
  }

  public Language getLanguage() {
    return language;
  }

  public boolean isOptimized() {
    return optimized;
  }

  public boolean isRestartable() {
    return restartable;
  }

  public boolean isRelocatable() {
    return relocatable;
  }

  public String getTracing() {
    return tracing;
  }

  public ImmutableSortedSet<BigInteger> getCustomMetricIndicies() {
    return ImmutableSortedSet.copyOf(customMetricIndices);
  }

  public String getClazz() {
    return clazz;
  }

  BigInteger getNewContainerInputPortIndex() {
    BigInteger rval = nextContainerInputPortIndex;
    nextContainerInputPortIndex = nextContainerInputPortIndex.add(BigInteger.ONE);
    return rval;
  }

  BigInteger getNewContainerOutputPortIndex() {
    BigInteger rval = nextContainerOutputPortIndex;
    nextContainerOutputPortIndex = nextContainerOutputPortIndex.add(BigInteger.ONE);
    return rval;
  }

  public Set<FusedContainerConnectionId> getOutgoingContainerConnections() {
    Set<FusedContainerConnectionId> conns = new HashSet<>();
    for (FusedContainerOutputPort fcop : containerOutputPorts.values()) {
      conns.addAll(fcop.getContainerConnections());
    }
    return conns;
  }

  public Set<FusedContainerConnectionId> getIncomingContainerConnections() {
    Set<FusedContainerConnectionId> conns = new HashSet<>();
    for (FusedContainerInputPort fcip : containerInputPorts.values()) {
      conns.addAll(fcip.getContainerConnections());
    }
    return conns;
  }

  public BigInteger getFusedApplicationId() {
    return appId;
  }

  public BigInteger getTopologyApplicationId() {
    return topologyAppId;
  }

  public Map<StreamId, RuntimeStream> getViewableStreams(BigInteger jobId, BigInteger peId) {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Enter");
    }

    Map<StreamId, RuntimeStream> streams = new HashMap<StreamId, RuntimeStream>();
    for (BigInteger nodeIndex : nodes) {
      TopologyNode node = SAMStaticContext.getTopologyTracker().getTopologyNode(appId, nodeIndex);
      Map<BigInteger, TopologyNodeOutputPort> oports = node.getOutputPorts();
      for (TopologyNodeOutputPort oport : oports.values()) {
        if (oport.isViewable() == null
            || (oport.isViewable() != null && oport.isViewable().booleanValue())) {
          // create streamId
          StreamId streamId = new StreamId(jobId, node.getName(), oport.getIndex());
          streamId.setPeId(peId);
          streamId.setOperInstanceIndex(node.getIndex());

          // Getting stream related tupleType
          TopologyTupleTypeType ttype =
              SAMStaticContext.getObjectTracker()
                  .getJob(jobId)
                  .getTupleTypes()
                  .get(oport.getTupleTypeIndex());

          // create stream with required attributes
          RuntimeStream stream = new RuntimeStream();
          stream.setStreamId(streamId);
          stream.setOperInstanceName(node.getName());
          stream.setOperInstanceOPortName(oport.getName());
          stream.setStreamName(oport.getStreamName());
          stream.setIsLogical(false);
          if (ttype != null) {
            stream.setTupleType(ttype);
          }
          streams.put(streamId, stream);
          if (Trace.isEnabled(Level.TRACE)) {
            Trace.logTrace(
                "Added physical viewable stream with id " + streamId.toJson().toString());
          }

          // Check if the operator in parallel region. If so, create a logicView too.
          if (!node.getName().equals(node.getLogicalName())) {
            stream.setLogicalOperInstanceName(node.getLogicalName());
            stream.setLogicalStreamName(oport.getLogicalStreamName());
            if (node.getParallelRegions() == null
                || node.getParallelRegions().get(0).getGlobalChannelIndex() == null) {
              stream.setChannelNum(0);
            } else {
              stream.setChannelNum(
                  node.getParallelRegions().get(0).getGlobalChannelIndex().intValue());
            }

            // add logical view (or count, if already added)
            List<String> logicalNames = getAllLogicalNames(node.getName(), node.getLogicalName());
            for (String logicalName : logicalNames) {
              StreamId logStreamId = new StreamId(jobId, logicalName, oport.getIndex());
              if (!streams.containsKey(logStreamId)) {
                RuntimeStream logStream = new RuntimeStream();
                logStream.setStreamId(logStreamId);
                logStream.setOperInstanceName(node.getLogicalName());
                logStream.setStreamName(oport.getStreamName());
                logStream.setOperInstanceOPortName(oport.getName());
                logStream.setNumChannels(1);
                logStream.setIsLogical(true);
                if (ttype != null) {
                  logStream.setTupleType(ttype);
                }
                streams.put(logStreamId, logStream);
              } else {
                int numChannels = streams.get(logStreamId).getNumChannels() + 1;
                streams.get(logStreamId).setNumChannels(numChannels);
              }
              if (Trace.isEnabled(Level.TRACE)) {
                Trace.logTrace(
                    "Added/updated logical viewable stream with id "
                        + logStreamId.toJson().toString());
              }
            }
          }
        }
      }
    }
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Exit");
    }
    return streams;
  }

  // Return a list of potential logicalNames of a stream within a parallel region.
  // such as if logicalName=OuterA.MiddleB.InnerA physicalName=OuterA[i].MiddleB[j].InnerA, then
  // potential logicalNames will be: OuterA.MiddleB.InnerA, OuterA[i].MiddleB.InnerA
  private List<String> getAllLogicalNames(String name, String logicalName) {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("Enter with node physicalName=" + name + " and logicalName=" + logicalName);
    }
    List<String> names = new ArrayList<String>();
    if (name.indexOf(logicalName) == 0) {
      // Single layer UDP (the pre-Oleta behavior)
      // Note: Keep this code here to speed up the single-layer UDP process.
      names.add(logicalName);
    } else {
      String[] phyNames = name.split("\\.");
      String[] logNames = logicalName.split("\\.");
      if (phyNames.length == logNames.length) {
        for (int i = 0; i < phyNames.length; i++) {
          if (phyNames[i].indexOf("[") > 0 && phyNames[i].indexOf(logNames[i]) == 0) {
            String thisLogName =
                name.substring(0, name.indexOf(phyNames[i]))
                    + logicalName.substring(logicalName.indexOf(logNames[i]));
            names.add(thisLogName);
          }
        }
      } else {
        if (Trace.isEnabled(Level.WARN)) {
          Trace.logWarn(
              "physicalName="
                  + name
                  + " and logicalName="
                  + logicalName
                  + " style not match. No logicalView created.");
        }
      }
    }

    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Exit with potential logicalNames: " + names.toString());
    }
    return names;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == null) {
      return false;
    }
    if (obj == this) {
      return true;
    }
    if (obj.getClass() != getClass()) {
      return false;
    }
    FusedContainer rhs = (FusedContainer) obj;
    return new EqualsBuilder()
        .append(index, rhs.index)
        .append(topologyAppId, rhs.topologyAppId)
        .append(nodes, rhs.nodes)
        .append(envVars, rhs.envVars)
        .append(capabilities, rhs.capabilities)
        .append(language, rhs.language)
        .append(optimized, rhs.optimized)
        .append(restartable, rhs.restartable)
        .append(relocatable, rhs.relocatable)
        .append(clazz, rhs.clazz)
        .append(customMetricIndices, rhs.customMetricIndices)
        .append(appId, rhs.appId)
        .append(allConnections, rhs.allConnections)
        .append(intraConns, rhs.intraConns)
        .append(interConns, rhs.interConns)
        .append(containerInputPorts, rhs.containerInputPorts)
        .append(containerOutputPorts, rhs.containerOutputPorts)
        .isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        index,
        topologyAppId,
        nodes,
        envVars,
        capabilities,
        language,
        optimized,
        restartable,
        relocatable,
        clazz,
        customMetricIndices,
        appId,
        allConnections,
        intraConns,
        interConns,
        containerInputPorts,
        containerOutputPorts);
  }

  @Override
  public int compareTo(FusedContainer rhs) {
    return new CompareToBuilder()
        .append(this.appId, rhs.appId)
        .append(this.index, rhs.index)
        .append(this.hashCode(), rhs.hashCode())
        .toComparison();
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("appId=" + appId);
    builder.append(" index=" + index);
    builder.append(" nodes=" + nodes);
    return builder.toString();
  }
}
