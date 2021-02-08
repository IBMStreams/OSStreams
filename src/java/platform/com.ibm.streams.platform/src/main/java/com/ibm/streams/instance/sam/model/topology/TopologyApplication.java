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

package com.ibm.streams.instance.sam.model.topology;

import static com.google.common.base.Preconditions.checkArgument;
import static com.google.common.base.Preconditions.checkNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNullAndValidate;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsPositionIndex;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.validateIfPresent;

import com.google.common.collect.ImmutableSortedMap;
import com.google.common.collect.ImmutableSortedSet;
import com.google.common.collect.Sets;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.model.logical.spl.SubmissionTimeValue;
import com.ibm.streams.instance.sam.persistence.IDotGraph;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.sch.composer.Composer_helper;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.IdentityHashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.stream.Collectors;

public class TopologyApplication extends TopologyObject implements IDotGraph, Validator {

  private final BigInteger id;
  private final String name;
  private final String applicationScope;
  private final String version;
  private final String dataDirectory;
  private final String applicationDirectory;
  private final String outputDirectory;
  private final String checkpointDirectory;
  private final String adlPath;
  private final String buildId;
  private final String bundlePath;
  private final String productVersion;
  private final ToolkitMap toolkitMap;
  private RuntimeConstant display;
  private SortedMap<BigInteger, CustomMetric> customMetrics;
  private SortedMap<BigInteger, Library> libraries;
  private SortedMap<BigInteger, Hostpool> hostpools;
  private SortedMap<BigInteger, ParallelRegion> parallelRegions;
  private SortedMap<String, BigInteger>
      parallelRegionNames; // value of this map is key in parallelRegions map
  private SortedMap<BigInteger, TopologyTupleTypeType> tupleTypes;
  private SortedSet<BigInteger> nodeIndexes;
  private transient SortedSet<TopologyNode> nodes;
  private Map<BigInteger, SubmissionTimeValue> submissionTimeValues;
  private final YieldBehaviour yieldBehaviour;

  public TopologyApplication(
      String name,
      String applicationScope,
      String version,
      String dataDirectory,
      String applicationDirectory,
      String outputDirectory,
      String checkpointDirectory,
      String adlPath,
      String buildId,
      String bundlePath,
      ToolkitMap toolkitMap,
      String productVersion,
      YieldBehaviour yieldBehaviour) {
    this.tracker = SAMStaticContext.getTopologyTracker();
    this.name = name;
    this.applicationScope = applicationScope;
    this.version = version;
    this.dataDirectory = dataDirectory;
    this.applicationDirectory = applicationDirectory;
    this.outputDirectory = outputDirectory;
    this.checkpointDirectory = checkpointDirectory;
    this.adlPath = adlPath;
    this.buildId = buildId;
    this.bundlePath = bundlePath;
    this.productVersion = productVersion;
    this.toolkitMap = toolkitMap;
    this.yieldBehaviour = yieldBehaviour;
    customMetrics = new TreeMap<>();
    libraries = new TreeMap<>();
    hostpools = new TreeMap<>();
    parallelRegions = new TreeMap<>();
    tupleTypes = new TreeMap<>();
    nodeIndexes = new TreeSet<>();
    nodes = new TreeSet<>();
    submissionTimeValues = new HashMap<>();

    // for oleta, topology application id = fused application id = job id
    this.id = SAMStaticContext.getIDGeneratorFactory().newTopologyApplicationId();

    build();
  }

  @Override
  public void build() {
    tracker.registerTopologyApplication(id, this);
  }

  public String getProductVersion() {
    return productVersion;
  }

  // product version without "."
  public String getBundleVersion() {
    return productVersion.replaceAll("\\.", "");
  }

  public ToolkitMap getToolkitMap() {
    return toolkitMap;
  }

  public BigInteger getId() {
    return id;
  }

  public String getName() {
    return name;
  }

  public String getApplicationScope() {
    return applicationScope;
  }

  public String getVersion() {
    return version;
  }

  public String getDataDirectory() {
    return dataDirectory;
  }

  public String getApplicationDirectory() {
    return applicationDirectory;
  }

  public String getOutputDirectory() {
    return outputDirectory;
  }

  public String getCheckpointDirectory() {
    return checkpointDirectory;
  }

  public String getAdlPath() {
    return adlPath;
  }

  public ImmutableSortedMap<BigInteger, CustomMetric> getCustomMetrics() {
    return ImmutableSortedMap.copyOf(customMetrics);
  }

  public void setCustomMetrics(SortedMap<BigInteger, CustomMetric> customMetrics) {
    this.customMetrics = customMetrics;
  }

  public ImmutableSortedMap<BigInteger, Library> getLibraries() {
    return ImmutableSortedMap.copyOf(libraries);
  }

  public void setLibraries(SortedMap<BigInteger, Library> libraries) {
    this.libraries = libraries;
  }

  public ImmutableSortedMap<BigInteger, Hostpool> getHostpools() {
    return ImmutableSortedMap.copyOf(hostpools);
  }

  public SortedMap<BigInteger, Hostpool> getHostpools_forUpdate() {
    return (hostpools);
  }

  public void setHostpools(SortedMap<BigInteger, Hostpool> hostpools) {
    this.hostpools = hostpools;
  }

  public ImmutableSortedMap<BigInteger, ParallelRegion> getParallelRegions() {
    return ImmutableSortedMap.copyOf(parallelRegions);
  }

  public void setParallelRegions(SortedMap<BigInteger, ParallelRegion> parallelRegions) {
    this.parallelRegions = parallelRegions;
  }

  public void setParallelRegionNames(SortedMap<String, BigInteger> parallelRegionNames) {
    this.parallelRegionNames = parallelRegionNames;
  }

  public ImmutableSortedMap<BigInteger, TopologyTupleTypeType> getTupleTypes() {
    return ImmutableSortedMap.copyOf(tupleTypes);
  }

  public void setTupleTypes(SortedMap<BigInteger, TopologyTupleTypeType> tupleTypes) {
    this.tupleTypes = tupleTypes;
  }

  public ImmutableSortedSet<BigInteger> getNodeIndexes() {
    return ImmutableSortedSet.copyOf(nodeIndexes);
  }

  public void setNodes(List<TopologyNode> nodes) {
    this.nodes.clear();
    this.nodes.addAll(nodes);
    for (TopologyNode node : nodes) {
      nodeIndexes.add(node.getIndex());
    }
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "TopologyApplication.setNodes() nodeIndexes for id="
              + id
              + ", tracker="
              + tracker
              + ": "
              + nodeIndexes);
    }
  }

  private void removeNodes(SortedSet<TopologyNode> nodes) {
    this.nodes.removeAll(nodes);
    for (TopologyNode node : nodes) {
      nodeIndexes.remove(node.getIndex());
      node.deepUnbuild();
    }
  }

  public RuntimeConstant getDisplay() {
    return display;
  }

  public void setDisplay(RuntimeConstant display) {
    this.display = display;
  }

  public Map<BigInteger, SubmissionTimeValue> getSubmissionTimeValues() {
    return submissionTimeValues;
  }

  public void setSubmissionTimeValues(Map<BigInteger, SubmissionTimeValue> values) {
    this.submissionTimeValues = values;
  }

  public String getBuildId() {
    return buildId;
  }

  public String getBundlePath() {
    return bundlePath;
  }

  // for recovery
  public void populateTransientFields() {
    this.tracker = SAMStaticContext.getTopologyTracker();
    nodes = null;
    getNodes();
  }

  // refresh operator cache, retrieve operators from registry
  private void refreshNodes() {
    nodes = new TreeSet<>();
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "TopologyApplication.refreshNodes() nodeIndexes for id="
              + id
              + ", tracker="
              + tracker
              + ": "
              + nodeIndexes);
    }
    for (BigInteger index : nodeIndexes) {
      TopologyNode tn = tracker.getTopologyNode(id, index);
      if (tn == null) {
        throw new IllegalStateException("TopologyNode id=" + id + " index=" + index + " is null");
      }
      nodes.add(tn);
    }
  }

  public SortedSet<TopologyNode> getNodes() {
    if (nodes == null || nodes.size() != nodeIndexes.size()) {
      refreshNodes();
    }
    return nodes;
  }

  public String toDotGraph() {

    // Initialization Stuff
    String graph = "digraph " + name + " {\n";
    graph += "\tnode [shape=record];";

    // Nodes
    for (BigInteger index : nodeIndexes)
      graph += tracker.getTopologyNode(id, index).toDotGraph() + "\n";

    graph += "\n}";
    return graph;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<topologyModel");
    dumper.append(" name=\"" + name);
    dumper.append("\" applicationScope=\"" + applicationScope);
    dumper.append("\" dataDirectory=\"" + dataDirectory);
    dumper.append("\" adlPath=\"" + adlPath);
    dumper.append("\" checkpointDirectory=\"" + checkpointDirectory);
    dumper.append("\" outputDirectory=\"" + outputDirectory);
    dumper.append("\" version=\"" + version);
    dumper.append("\" productVersion=\"" + productVersion);
    dumper.append("\">").end();
    dumper.indent();
    dumper.line("<libraries>");
    dumper.indent();
    dumper.append(libraries);
    dumper.outdent();
    dumper.line("</libraries>");
    dumper.line("<hostpools>");
    dumper.indent();
    dumper.append(hostpools);
    dumper.outdent();
    dumper.line("</hostpools>");
    dumper.line("<parallelRegions>");
    dumper.indent();
    dumper.append(parallelRegions);
    dumper.outdent();
    dumper.append("</parallelRegions>");
    dumper.line("<tupleTypes>");
    dumper.indent();
    dumper.append(tupleTypes);
    dumper.outdent();
    dumper.line("</tupleTypes>");
    dumper.line("<nodes>");
    dumper.indent();
    dumper.append(getNodes());
    dumper.outdent();
    dumper.line("</nodes>");
    dumper.line("<customMetrics>");
    dumper.indent();
    dumper.append(customMetrics);
    dumper.outdent();
    dumper.line("</customMetrics>");
    dumper.outdent();
    dumper.line("</topologyModel>");
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNull(name);
    checkNotNull(applicationScope);
    // Version can be null
    // Data Directory can be null
    checkNotNull(applicationDirectory);
    checkNotNull(outputDirectory);
    // Checkpoint Directory can be null
    // ADL Path can be null
    validateIfPresent(display);

    checkContentsPositionIndex(customMetrics.keySet());
    checkContentsNotNullAndValidate(customMetrics.values());

    checkContentsPositionIndex(libraries.keySet());
    checkContentsNotNullAndValidate(libraries.values());

    checkContentsPositionIndex(hostpools.keySet());
    checkContentsNotNullAndValidate(hostpools.values());

    checkContentsPositionIndex(tupleTypes.keySet());
    // TODO: Re-enable when tuple things are sorted out
    // checkContentsNotNullAndValidate(tupleTypes.values());

    // We need at least 1 node per application
    checkArgument(nodeIndexes.size() > 0, "At least 1 Topology Node required per application");
    checkContentsNotNullAndValidate(getNodes());
  }

  public YieldBehaviour getYieldBehaviour() {
    return yieldBehaviour;
  }

  // for change detection
  @Override
  public int hashCode() {
    return Objects.hash(id, hostpools, nodeIndexes, parallelRegions);
  }

  private void transferOwnership(
      Set<TopologyNode> nodes,
      Map<BigInteger, BigInteger> nodeIndexFix,
      Map<TopologyNodeOutputPort, Map<BigInteger, BigInteger>> splitterIndexFix,
      Map<BigInteger, BigInteger> hostpoolIndexFix) {
    // Transfering the ownership of a set of nodes from another TopologyApplication to this
    // TopologyApplication is tricky. We must do it in the order we do it below. We also must
    // transfer all connected nodes at the same time. That is, if we have a bunch of nodes
    // that are connected, and we want to transfer them, we need to collect them all into a
    // single set and pass that set to transferOwership. Connected nodes will share
    // TopologyNodeConnection objects, and we must handle them all at once.
    //
    // Transfering ownership is tricky because we have to change node indexes, change the
    // application ID, fixup connections (because of the changed node indexes), unregister
    // the nodes, ports and connections with the old application's object tracker, and then
    // re-register the nodes, ports and connections with this application's object tracker.
    // The tricky part is that application IDs and node indexes are how much of this is
    // stored in the topology tracker. In principle, we have to do this in four phases:
    //
    //   1. Load all connections from the topology tracker.
    //   2. Unregister all connections from the topology tracker.
    //   3. Change all connection node indexes and application IDs, while fixing
    //      connection indexes.
    //   4. Re-register all connections using their new information with the tracker,
    //      while handling all ports and nodes at the same time.
    //
    // Because TopologyNodeConnections are shared objects between TopologyNodePorts, we have
    // to do all of each step at once, which means we must iterate over all nodes four times.
    // We cannot iterate over the nodes once, and try to do these steps individually for
    // each node in isolation.

    // 1. Load all connections from the topology tracker. Note that getOperConnections will
    //    retrieve the TopologyNodeConnections from the tracker if they are not already stored
    //    locally, which is why we call it without using its result.
    for (TopologyNode node : nodes) {
      for (TopologyNodeInputPort iport : node.getInputPorts().values()) {
        iport.getOperConnections();
      }
      for (TopologyNodeOutputPort oport : node.getOutputPorts().values()) {
        oport.getOperConnections();
      }
    }

    // 2. Unregister all connections from the topology tracker. These are shared among
    //    TopologyNodePorts, so we have to do all at once. It is safe to do this now
    //    because we can be certain we've loaded all of them from the tracker.
    for (TopologyNode node : nodes) {
      for (TopologyNodeInputPort iport : node.getInputPorts().values()) {
        for (TopologyNodeConnection tnc : iport.getOperConnections()) {
          tnc.unbuild();
        }
      }
      for (TopologyNodeOutputPort oport : node.getOutputPorts().values()) {
        for (TopologyNodeConnection tnc : oport.getOperConnections()) {
          tnc.unbuild();
        }
      }
    }

    // 3. Change all connection node indexes and application IDs, while fixing
    //    connection indexes. It is safe to do this because we can be certain none
    //    are registered with the tracker.

    // Tracks which ConnectionIds have already been fixed - since they are shared
    // by TopologyNodeConnections and TopologyNode objects, we run into the danger of
    // double-fixing a ConnectionId when we encounter it through a different object.
    IdentityHashMap<TopologyNodeConnectionId, Boolean> fixed = new IdentityHashMap<>();
    for (TopologyNode node : nodes) {
      for (TopologyNodeInputPort iport : node.getInputPorts().values()) {
        iport.fixConnections(id, nodeIndexFix, fixed);
      }
      for (TopologyNodeOutputPort oport : node.getOutputPorts().values()) {
        oport.fixConnections(id, nodeIndexFix, fixed);
      }
    }

    // 4. Re-register all connections using their new information with the tracker,
    //    while handling all ports and nodes at the same time.
    for (TopologyNode node : nodes) {
      BigInteger nodeIndex = node.getIndex();
      if (nodeIndexFix.containsKey(nodeIndex)) {
        nodeIndex = nodeIndexFix.get(nodeIndex);
      }

      for (TopologyNodeInputPort iport : node.getInputPorts().values()) {
        iport.transferOwnership(id, nodeIndex);
        for (TopologyNodeConnection tnc : iport.getOperConnections()) {
          tnc.build();
        }
      }
      for (TopologyNodeOutputPort oport : node.getOutputPorts().values()) {
        oport.transferOwnership(id, nodeIndex, splitterIndexFix.get(oport));
        for (TopologyNodeConnection tnc : oport.getOperConnections()) {
          if (splitterIndexFix.get(oport) != null) {
            if (splitterIndexFix.get(oport).containsKey(tnc.getSplitterIndex())) {
              tnc.setSplitterIndex(splitterIndexFix.get(oport).get(tnc.getSplitterIndex()));
            }
          }
          tnc.build();
        }
      }

      // Note that we can only do this AFTER building our connections and transferring
      // our ports. If we try to do it before, we will not be properly connected
      // with our ports.
      node.transferOwnership(id, nodeIndex, hostpoolIndexFix);
      this.nodeIndexes.add(node.getIndex());
    }
    this.nodes.addAll(nodes);
  }

  private static void checkString(String a, String b, String msg) throws StreamsException {
    if (a == null && b == null) {
      return;
    }
    if (a != null) {
      if (b != null) {
        if (!a.equals(b)) {
          throw new StreamsException(
              "TopologyApplications have different " + msg + "; " + a + ", " + b);
        }
      } else {
        throw new StreamsException(
            "TopologyApplications have different " + msg + "; " + a + ", null");
      }
    } else {
      throw new StreamsException("TopologyApplications have different " + msg + "; null, " + b);
    }
  }

  public BigInteger getParallelRegionWidth(String regionName) {
    return parallelRegions.get(parallelRegionNames.get(regionName)).getLocalMaxChannels();
  }

  public Map<String, BigInteger> getParallelRegionWidths() {
    Map<String, BigInteger> nameToWidth = new TreeMap<>();
    for (ParallelRegion pr : parallelRegions.values()) {
      nameToWidth.put(pr.getName(), pr.getLocalMaxChannels());
    }
    return nameToWidth;
  }

  private static boolean localChannelEquals(List<ParallelRegion> a, List<ParallelRegion> b) {
    if (a == null && b == null) {
      return true;
    }
    if (a == null || b == null) {
      return false;
    }

    Iterator<ParallelRegion> aIt = a.iterator();
    Iterator<ParallelRegion> bIt = b.iterator();
    while (aIt.hasNext() && bIt.hasNext()) {
      if (!aIt.next().getLocalChannelIndex().equals(bIt.next().getLocalChannelIndex())) {
        return false;
      }
    }
    return true;
  }

  private static TopologyNode findLogicalNode(TopologyNode node, Set<TopologyNode> otherNodes) {
    for (TopologyNode otherNode : otherNodes) {
      if (node.getLogicalIndex().equals(otherNode.getLogicalIndex())
          && localChannelEquals(node.getParallelRegions(), otherNode.getParallelRegions())) {
        return otherNode;
      }
    }
    return null;
  }

  public String resolveParallelRegionName(String pattern) throws StreamsException {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "ENTER TopologyApplication.resolveParallelRegionName with pattern="
              + pattern
              + ", parallelRegionNames="
              + parallelRegionNames);
    }

    List<String> matches = new ArrayList<>();
    for (String name : parallelRegionNames.keySet()) {
      if (name.matches(Composer_helper.prepareConditionString(pattern))) {
        matches.add(name);
      }
    }

    if (matches.size() == 0) {
      throw new StreamsException(
          StreamsRuntimeMessagesKey.Key.SAMNoMatchingParallelRegion, pattern, getId());
    }
    if (matches.size() > 1) {
      throw new StreamsException(
          StreamsRuntimeMessagesKey.Key.SAMConfigOverlayMultipleParallelRegionsSpecified,
          pattern,
          matches.toString(),
          getId());
    }
    return matches.get(0);
  }

  public void transferParallelRegionDifference(
      TopologyApplication updated,
      String regionPattern,
      SortedSet<TopologyNode> changedNodes,
      SortedSet<TopologyNode> addedNodes,
      SortedSet<TopologyNode> removedNodes)
      throws StreamsException {
    // We only allow diffs of applications that differ in nodes only - they must derive from the
    // same ADL,
    // which means their names, toolkits, product version, etc. must be the same. In theory, we're
    // leaving
    // out a bunch of fields that should be equal, but I think it's okay to just check the strings,
    // which
    // are easy to check, and the most important ones are names and versions.
    checkString(this.getProductVersion(), updated.getProductVersion(), "ProductVersion");
    checkString(this.getName(), updated.getName(), "Name");
    checkString(this.getApplicationScope(), updated.getApplicationScope(), "ApplicationScope");
    checkString(this.getVersion(), updated.getVersion(), "Version");
    checkString(
        this.getApplicationDirectory(), updated.getApplicationDirectory(), "ApplicationDirectory");
    checkString(this.getOutputDirectory(), updated.getOutputDirectory(), "OutputDirectory");
    checkString(
        this.getCheckpointDirectory(), updated.getCheckpointDirectory(), "CheckpointDirectory");
    checkString(this.getAdlPath(), updated.getAdlPath(), "AdlPath");
    checkString(this.getBuildId(), updated.getBuildId(), "BuildId");
    checkString(this.getBundlePath(), updated.getBundlePath(), "BundlePath");

    SortedSet<TopologyNode> originalNodes = this.getNodes();
    SortedSet<TopologyNode> updatedNodes = updated.getNodes();

    // should never happen as TopologyApplications always have one node, but
    // it still makes logical sense for a diff
    if (originalNodes.isEmpty() && updatedNodes.isEmpty()) {
      return;
    }

    String regionName = resolveParallelRegionName(regionPattern);
    BigInteger originalWidth = this.getParallelRegionWidth(regionName);
    BigInteger updatedWidth = updated.getParallelRegionWidth(regionName);

    if (originalWidth.equals(updatedWidth)) {
      return;
    }

    // We accomplish the diff of the two TopologyApplications and the transfer in four phases:
    //
    //   1. Determine which nodes were added, deleted or changed in all affected parallel regions.
    //   2. Determine which nodes have splitters or mergers that feed affected parallel regions.
    //   3. Figure out all splitter indexes that have changed in connections between ports,
    // assigning new
    //      indexes as needed.
    //   4. Figure out all of the hostpool indexes that have changed, assigning new indexes are
    // needed.
    //   5. Apply all changes to this TopologyApplication.

    // updated node index -> fixed node index (original or new)
    Map<BigInteger, BigInteger> nodeIndexFix = new TreeMap<>();

    // while we're only changing one parallel region, if it's part of a parallel region nesting,
    // then we
    // need to keep track of all affected regions
    Map<String, ParallelRegion> affectedParallelRegions = new HashMap<>();

    // keeps track of the nodes in the original application which are the logical
    // equivalent of the "changed" nodes; these nodes to be replaced with the changed set.
    SortedSet<TopologyNode> toReplace = new TreeSet<>();

    //   1. Determine which nodes were added, deleted or changed in all affected parallel regions.
    // During
    //      this phase, we also have to figure out if the nodes from the updated TopologyApplication
    // have
    //      indexes that conflict with this one. If so, we need to track the conflicting indexes,
    // and
    //      come up with new indexes to assign to them later.
    if (originalWidth.compareTo(updatedWidth) < 0) {
      // we're adding nodes

      // We will use nextIndex to assign new, "corrected" indexes to the added nodes. We need to
      // make sure it does
      // not clash with either the original or updated nodes.
      int nextIndex =
          Math.max(
                  originalNodes.last().getIndex().intValue(),
                  updatedNodes.last().getIndex().intValue())
              + 1;
      for (TopologyNode node : updatedNodes) {

        // Note that at this outer-level, we have to check for parallel regions in general; it is
        // not correct to
        // just ask if the node is in the parallel region we care about. That's because we need to
        // do index fixups
        // on parallel nodes outside of the target parallel region.
        if (node.getParallelRegions() != null) {
          TopologyNode originalNode = findLogicalNode(node, originalNodes);
          if (originalNode == null) {
            SortedSet<TopologyNode> greaterThanOrEqual = originalNodes.tailSet(node);
            if (greaterThanOrEqual.size() > 0
                && greaterThanOrEqual.first().getIndex().equals(node.getIndex())) {
              nodeIndexFix.put(node.getIndex(), BigInteger.valueOf(nextIndex));
              ++nextIndex;
            }
            addedNodes.add(node);
            affectedParallelRegions.putAll(
                node.getParallelRegions().stream()
                    .collect(Collectors.toMap(e -> e.getName(), e -> e)));
          } else {
            if (!originalNode.getIndex().equals(node.getIndex())) {
              nodeIndexFix.put(node.getIndex(), originalNode.getIndex());
            }

            // We need to replace this node if:
            //   1. Its in the targetted parallel region. This means, at least, that
            //      we need to change its maxChannels number.
            //   2. Its name does not match with its logical equivalent. If that's
            //      the case, then we need to change its name, but also we need to
            //      change at least one of its global channel numbers.
            if (node.isInParallelRegion(regionName)
                || !node.getName().equals(originalNode.getName())) {
              toReplace.add(originalNode);
              changedNodes.add(node);
            }
          }
        }
      }
    } else {
      // we're removing nodes
      for (TopologyNode node : originalNodes) {
        if (node.getParallelRegions() != null) {
          TopologyNode updatedNode = findLogicalNode(node, updatedNodes);
          if (updatedNode == null) {
            removedNodes.add(node);
            affectedParallelRegions.putAll(
                node.getParallelRegions().stream()
                    .collect(Collectors.toMap(e -> e.getName(), e -> e)));
          } else {
            if (!updatedNode.getIndex().equals(node.getIndex())) {
              nodeIndexFix.put(updatedNode.getIndex(), node.getIndex());
            }
            if (updatedNode.isInParallelRegion(regionName)
                || !updatedNode.getName().equals(node.getName())) {
              toReplace.add(node);
              changedNodes.add(updatedNode);
            }
          }
        }
      }
    }

    //   2. Determine which nodes have splitters or mergers that feed affected parallel regions.
    // These
    //      nodes are changed, and need to also be replaced.
    for (TopologyNode node : updatedNodes) {
      if (node.isInParallelRegion(regionName)) {
        for (TopologyNodeOutputPort oport : node.getOutputPorts().values()) {
          for (TopologyNodeConnection conn : oport.getOperConnections()) {
            // the current node IS in the parallel region, so if the node its output port connects
            // to
            // IS NOT in the parallel region, then it's not in any of our sets yet and we need to
            // add
            // it; these are the merge points
            TopologyNode dest = conn.getDestination().getOwningOperator();
            if (!dest.isInParallelRegion(regionName)) {
              changedNodes.add(dest);
              toReplace.add(findLogicalNode(dest, originalNodes));
            }
          }
        }
        for (TopologyNodeInputPort iport : node.getInputPorts().values()) {
          for (TopologyNodeConnection conn : iport.getOperConnections()) {
            // the current node IS in the parallel region, so if the node its input port connects to
            // IS NOT in the parallel region, then it's not in any of our sets yet and we need to
            // add
            // it; these are splitters
            TopologyNode src = conn.getSource().getOwningOperator();
            if (!src.isInParallelRegion(regionName)) {
              changedNodes.add(src);
              toReplace.add(findLogicalNode(src, originalNodes));
            }
          }
        }
      }
    }

    // {updated output port -> {updated splitter index -> fixed splitter index (original or new)} }
    Map<TopologyNodeOutputPort, Map<BigInteger, BigInteger>> splitterIndexFix = new TreeMap<>();

    //   3. Figure out all splitter indexes that have changed in connections between ports,
    // assigning new
    //      indexes as needed. This only happens in the nested UDP case. In nested UDP, it is
    // possible
    //      for the total number of splitters on an node's output port to change. When that happens,
    // the
    //      connections between the original and updated nodes will disagree on splitter indexes.
    // Similar
    //      to the node index fixup, we need to track these to apply later.
    //
    //      Note that we have to do this AFTER phase 2 because we need the full nodeIndexFix map. We
    // need to
    //      compare connections paying attenion to the node indexes that updated connections will
    // have.
    for (TopologyNode node : updatedNodes) {
      for (TopologyNodeOutputPort oport : node.getOutputPorts().values()) {
        TopologyNode originalNode = null;
        if (oport.getSplitters() != null
            && (originalNode = findLogicalNode(node, originalNodes)) != null) {
          TopologyNodeOutputPort originalOPort =
              originalNode.getOutputPorts().get(oport.getIndex());
          for (TopologyNodeConnection tnc : oport.getOperConnections()) {
            for (TopologyNodeConnection originalTnc : originalOPort.getOperConnections()) {
              if (tnc.getSplitterIndex() != null
                  && !tnc.getSplitterIndex().equals(originalTnc.getSplitterIndex())
                  && tnc.getId().equalsWithNodeIndexFix(originalTnc.getId(), nodeIndexFix)) {
                if (splitterIndexFix.get(oport) == null) {
                  splitterIndexFix.put(oport, new TreeMap<>());
                }
                splitterIndexFix
                    .get(oport)
                    .put(tnc.getSplitterIndex(), originalTnc.getSplitterIndex());
              }
            }
          }

          if (splitterIndexFix.get(oport) != null) {
            long nextIndex =
                Math.max(
                        oport.maxSplitterIndex().longValue(),
                        Collections.max(splitterIndexFix.get(oport).values()).longValue())
                    + 1;
            Map<BigInteger, Splitter> updatedSplitters = oport.getAllSplitters();
            Map<BigInteger, BigInteger> moreFixes = new TreeMap<>();
            for (BigInteger value : splitterIndexFix.get(oport).values()) {
              if (updatedSplitters.containsKey(value)
                  && !splitterIndexFix.get(oport).containsKey(value)) {
                moreFixes.put(value, BigInteger.valueOf(nextIndex));
                ++nextIndex;
              }
            }
            splitterIndexFix.get(oport).putAll(moreFixes);
          }
        }
      }
    }

    // updated hostpool index -> fixed hostpool index (original or new)
    Map<BigInteger, BigInteger> hostpoolIndexFix = new TreeMap<>();

    //   4. Figure out all hostpool indexes that have changed, assigning new indexes as needed. This
    // only happens in
    //      when the replicatedHostTags option is used in multiple parallel regions.
    int nextIndex =
        Math.max(this.hostpools.lastKey().intValue(), updated.hostpools.lastKey().intValue()) + 1;
    for (Hostpool originalHp : this.hostpools.values()) {
      for (Hostpool updatedHp : updated.hostpools.values()) {
        // There are two situations where we might encounter hostpool collisions: in the index, and
        // in the name. When
        // we encounter a name collision, we MUST map the updated hostpool to the original one.
        // That's an identity
        // issue;
        // the updated hostpool IS the original hostpool. When the indexes collide, they are
        // different hostpools that
        // just
        // have an index collision, so we must create a new index for the updated hostpool. But we
        // have to be careful
        // when
        // we do this, as we need to make sure that we remap all name collisions, and that if we
        // already mapped an index
        // collision, we don't miss a name collision. Hence, we check the existing fixup map before
        // doing an index
        // remap,
        // but do the name remap whenever we see it.
        if (!originalHp.getIndex().equals(updatedHp.getIndex())
            && originalHp.getName().equals(updatedHp.getName())) {
          hostpoolIndexFix.put(updatedHp.getIndex(), originalHp.getIndex());
        } else if (!hostpoolIndexFix.containsKey(updatedHp.getIndex())
            && originalHp.getIndex().equals(updatedHp.getIndex())
            && !originalHp.getName().equals(updatedHp.getName())) {
          hostpoolIndexFix.put(updatedHp.getIndex(), BigInteger.valueOf(nextIndex));
          ++nextIndex;
        }
      }
    }

    //   5. Apply all changes to this TopologyApplication: remove deleted and original changed
    // nodes, add
    //      new and updated changed nodes, transfer ownership of nodes to this TopologyApplication,
    // apply
    //      all of the node, splitter and hostpool index fixups, add or remove hostpools, and update
    // the
    //      parallel region sizes.
    removeNodes(toReplace);
    removeNodes(removedNodes);
    transferOwnership(
        Sets.union(addedNodes, changedNodes), nodeIndexFix, splitterIndexFix, hostpoolIndexFix);
    for (ParallelRegion pr : affectedParallelRegions.values()) {
      parallelRegions
          .get(pr.getRegionIndex())
          .setLocalMaxChannels(
              updated.parallelRegions.get(pr.getRegionIndex()).getLocalMaxChannels());
      parallelRegions
          .get(pr.getRegionIndex())
          .setGlobalMaxChannels(
              updated.parallelRegions.get(pr.getRegionIndex()).getGlobalMaxChannels());
    }

    // if we're decreasing channels, we may need to remove hostpools - but we need to go through our
    // index fixup
    this.hostpools
        .entrySet()
        .removeIf(
            entry ->
                !updated.hostpools.containsKey(
                    hostpoolIndexFix.getOrDefault(entry.getKey(), entry.getKey())));

    // if we're adding channels, we may need to add hostpools - but we need to apply the index
    // fixups first
    for (Hostpool updatedHp : updated.hostpools.values()) {
      BigInteger updatedIndex = updatedHp.getIndex();
      if (hostpoolIndexFix.containsKey(updatedIndex)) {
        updatedIndex = hostpoolIndexFix.get(updatedIndex);
      }
      updatedHp.setIndex(updatedIndex);
      this.hostpools.putIfAbsent(updatedIndex, updatedHp);
    }
  }
}
