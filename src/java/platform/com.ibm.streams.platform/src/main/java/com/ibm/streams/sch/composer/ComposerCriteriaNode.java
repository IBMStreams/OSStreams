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

package com.ibm.streams.sch.composer;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.MembershipMode;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey.Key;
import com.ibm.streams.platform.services.MessageElementType;
import com.ibm.streams.sch.composer.FusingConstraintChecker.ConstraintViolation;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import org.apache.commons.lang.builder.EqualsBuilder;

public class ComposerCriteriaNode {
  private static final long DEFAULT_OP_CPU_USAGE = 5;

  public static String INFERRED_COLOCATION = "StreamsIUO_InferredColocation_";
  public static String INFERRED_COLOCATION_PARTITION = INFERRED_COLOCATION + "Partition_";
  private static String INFERRED_COLOCATION_FROM_SAMEHOST = INFERRED_COLOCATION + "SameHost_";
  private static String INFERRED_COLOCATION_FROM_SAMEHOSTPOOLINDEX =
      INFERRED_COLOCATION + "SameHostPoolLocation_";

  public MessageElementType getCurrentConstraintMessage(
      Set<ConstraintViolation> cViols, boolean hostColocated) {
    return (getCurrentConstraintMessage(
        _topNode, _composer.getApplication(), cViols, hostColocated));
  }

  // cViols = null means every attribute.
  public static MessageElementType getCurrentConstraintMessage(
      TopologyNode topNode,
      TopologyApplication topApp,
      Set<ConstraintViolation> cViols,
      boolean hostColocated) {
    MessageElementType msg = new MessageElementType();
    msg.setMessageCode(
        (!hostColocated)
            ? Key.SCHOperatorConstraints.getMessageId()
            : Key.SCHHostColocatedOperatorConstraints.getMessageId());
    msg.getSubstitutionText().add(topNode.getName());
    msg.getSubstitutionText().add(getConstraintString(topNode, topApp, cViols));
    return msg;
  }

  private static String getConstraintString(
      TopologyNode topNode, TopologyApplication topApp, Set<ConstraintViolation> cViols) {
    StringBuilder str = new StringBuilder();

    // partition constraints
    boolean isolation = topNode.getPartitions().isIsolation();
    boolean softIsolation = topNode.getPartitions().isSoftIsolation();
    //        Set<String> colocs = topNode.getPartitions().getCoLocations();
    Set<String> cleanedColocs = new HashSet<>();
    for (String coloc : topNode.getPartitions().getCoLocations()) {
      if (!coloc.startsWith(INFERRED_COLOCATION)) {
        cleanedColocs.add(coloc);
      }
    }
    Set<String> exlocs = topNode.getPartitions().getExLocations();
    Map<String, Set<String>> provs = topNode.getPartitions().getProvisionalLocations();
    Map<String, Set<String>> segrs = topNode.getPartitions().getSegregatedLocations();
    Map<String, Set<String>> softProvs = topNode.getPartitions().getSoftProvisionalLocations();
    Map<String, Set<String>> softSegrs = topNode.getPartitions().getSoftSegregatedLocations();
    boolean notRestartable = !(topNode.isRestartable());
    boolean notRelocatable = !(topNode.isRelocatable());

    boolean first = true;
    if (isolation && cViols.contains(ConstraintViolation.partitionIsolation)) {
      str.append("partitionIsolation");
      first = false;
    }
    if (softIsolation && cViols.contains(ConstraintViolation.softPartitionIsolation)) {
      str.append("softPartitionIsolation");
      first = false;
    }
    if (cleanedColocs.size() > 0) {
      if (!first) str.append("; ");
      str.append("partitionColocation=" + cleanedColocs);
      first = false;
    }
    if (exlocs.size() > 0 && cViols.contains(ConstraintViolation.partitionExlocation)) {
      if (!first) str.append("; ");
      str.append("partitionExlocation=" + exlocs);
      first = false;
    }
    if (provs.size() > 0 && cViols.contains(ConstraintViolation.provisionalLocation)) {
      if (!first) str.append("; ");
      str.append("internalProperties_1=" + provs);
      first = false;
    }
    if (softProvs.size() > 0 && cViols.contains(ConstraintViolation.softProvisionalLocation)) {
      if (!first) str.append("; ");
      str.append("internalProperties_2=" + softProvs);
      first = false;
    }
    if (segrs.size() > 0 && cViols.contains(ConstraintViolation.segregatedLocation)) {
      if (!first) str.append("; ");
      str.append("internalProperties_3=" + segrs);
      first = false;
    }
    if (softSegrs.size() > 0 && cViols.contains(ConstraintViolation.softSegregatedLocation)) {
      if (!first) str.append("; ");
      str.append("internalProperties_4=" + softSegrs);
      first = false;
    }

    // resource constraints
    boolean hostIsolation = topNode.getResources().isIsolation();
    Set<String> cleanedHostColocs = new HashSet<>();
    for (String coloc : topNode.getResources().getCoLocations()) {
      if (!coloc.startsWith(INFERRED_COLOCATION)) {
        cleanedHostColocs.add(coloc);
      }
    }
    Set<String> hostExlocs = topNode.getResources().getExLocations();
    String hostname = topNode.getResources().getHostLocation();
    PoolLocation poolLoc = topNode.getResources().getPoolLocation();
    BigInteger hostpoolIndex = null;
    BigInteger inpoolIndex = null;
    if (poolLoc != null) {
      hostpoolIndex = poolLoc.getPoolIndex();
      inpoolIndex = poolLoc.getInpoolIndex();
    }
    Hostpool hostpool = topApp.getHostpools().get(hostpoolIndex);

    if (hostIsolation && cViols.contains(ConstraintViolation.hostIsolation)) {
      str.append("hostIsolation");
      first = false;
    }
    if (cleanedHostColocs.size() > 0 && cViols.contains(ConstraintViolation.hostColocation)) {
      if (!first) str.append("; ");
      str.append("hostColocation=" + cleanedHostColocs);
      first = false;
    }
    if (hostExlocs.size() > 0 && cViols.contains(ConstraintViolation.hostExlocation)) {
      if (!first) str.append("; ");
      str.append("hostExlocation=" + hostExlocs);
      first = false;
    }
    if (hostname != null && cViols.contains(ConstraintViolation.declaredHost)) {
      if (!first) str.append("; ");
      str.append("declaredHost{" + hostname + "}");
      first = false;
    }
    if (poolLoc != null && cViols.contains(ConstraintViolation.hostpool)) {
      if (!first) str.append("; ");
      str.append("hostpool{");
      str.append(hostpool.getName() + ",");
      if (hostpool.getTags() != null && hostpool.getTags().size() > 0) {
        str.append("tags" + hostpool.getTags() + ",");
      }
      str.append(
          (hostpool.getMembershipMode().compareTo(MembershipMode.exclusive) == 0)
              ? "Exclusive"
              : "Shared");
      if (hostpool.getSize() != null) {
        str.append(",size=" + hostpool.getSize());
      }
      str.append((inpoolIndex != null) ? (",(" + inpoolIndex + ")") : "");
      str.append("}");
      first = false;
    }
    if (notRestartable && cViols.contains(ConstraintViolation.restartable)) {
      if (!first) str.append("; ");
      str.append("notRestartable");
      first = false;
    }
    if (notRelocatable && cViols.contains(ConstraintViolation.relocatable)) {
      if (!first) str.append("; ");
      str.append("notRelocatable");
      first = false;
    }

    return str.toString();
  }

  //	public static class Location_Attributes  {
  //		public Set<String> _locAttrSet;
  //		public Location_Attributes(Set<String> locAttrSet) {
  //			_locAttrSet = locAttrSet;
  //		}
  //		public int hashCode() {
  //		    return Objects.hash(_locAttrSet);
  //		 }
  //		@Override
  //		public boolean equals(Object locAtts) {
  //			if ((locAtts instanceof Location_Attributes) && (_locAttrSet.equals(locAtts)) ) {
  //				return true;
  //			} else {
  //				return false;
  //			}
  //		}
  //		public String toString() {
  //			return (_locAttrSet.toString());
  //		}
  //	}

  public BigInteger getNodeIndex() {
    return _topNode.getIndex();
  }

  public String getNodeName() {
    return _topNode.getName();
  }

  public TopologyNode getTopologyNode() {
    return _topNode;
  }

  public boolean isRestartable() {
    return _topNode.isRestartable();
  }

  public boolean isRelocatable() {
    return _topNode.isRelocatable();
  }

  // host level constraints
  public String getHostLocation() {
    return _topNode.getResources().getHostLocation();
  }

  public String getDerivedHostLocation() {
    String hostLoc = getHostLocation();
    if (hostLoc == null) {
      if ((getHostpoolIndex() != null) && (getIndexInPool() != null)) {
        // check if hostpool has pre-declared hosts in it.
        Hostpool hp = _composer.getApplication().getHostpools().get(getHostpoolIndex());
        if (hp.getHosts().size() > 0) {
          hostLoc = hp.getHosts().get(getIndexInPool().intValue());
        }
      }
    }
    return hostLoc;
  }

  public List<String> getDeclaredHostPoolMembers() {
    List<String> hostpoolMembers = new ArrayList<>();
    if (getHostpoolIndex() != null) {
      // check if hostpool has pre-declared hosts in it.
      Hostpool hp = _composer.getApplication().getHostpools().get(getHostpoolIndex());
      hostpoolMembers = hp.getHosts();
    }
    return hostpoolMembers;
  }

  public List<String> getDeclaredHosts() {
    List<String> dHosts = new ArrayList<>();
    if (getHostLocation() != null) {
      dHosts.add(getHostLocation());
    } else {
      if (getDerivedHostLocation() != null) {
        dHosts.add(getDerivedHostLocation());
      } else {
        dHosts.addAll(getDeclaredHostPoolMembers());
      }
    }
    return dHosts;
  }

  public Set<String> getHostpoolTags() {
    Set<String> hostpoolTags = new HashSet<>();
    if (getHostpoolIndex() != null) {
      // check if hostpool has pre-declared hosts in it.
      Hostpool hp = _composer.getApplication().getHostpools().get(getHostpoolIndex());
      hostpoolTags = hp.getTags();
    }
    return hostpoolTags;
  }

  public BigInteger getHostpoolIndex() {
    return (_topNode.getResources().getPoolLocation() != null)
        ? _topNode.getResources().getPoolLocation().getPoolIndex()
        : null;
  }

  public PoolLocation getPoolLocation() {
    return _topNode.getResources().getPoolLocation();
  }

  public BigInteger getIndexInPool() {
    return (_topNode.getResources().getPoolLocation() != null)
        ? _topNode.getResources().getPoolLocation().getInpoolIndex()
        : null;
  }

  public HostSpec getHostSpec() {

    Set<String> declaredHosts = null;
    Set<PoolLocation> indexedPoolLocs = null;
    Set<BigInteger> hpIndexes = new HashSet<>();
    if (getHostpoolIndex() != null) {
      hpIndexes.add(getHostpoolIndex());
    }

    String hostName = getHostLocation();
    if (hostName != null) {
      // declared host
      declaredHosts = new HashSet<>();
      declaredHosts.add(hostName);
    } else {
      //
      List<String> declaredPoolHosts = getDeclaredHostPoolMembers();
      if (declaredPoolHosts.size() > 0) {
        // declared pool hosts
        if ((getPoolLocation() != null) && (getPoolLocation().getInpoolIndex() != null)) {
          // indexed
          String declaredHost =
              declaredPoolHosts.get(getPoolLocation().getInpoolIndex().intValue());
          declaredHosts = new HashSet<>();
          declaredHosts.add(declaredHost);
        } else {
          declaredHosts = new HashSet<>(declaredPoolHosts);
        }

      } else {
        // no declared pool hosts.
        if ((getPoolLocation() != null) && (getPoolLocation().getInpoolIndex() != null)) {
          // indexed
          indexedPoolLocs = new HashSet<>();
          indexedPoolLocs.add(getPoolLocation());
        }
      }
    }

    return new HostSpec(declaredHosts, getHostpoolTags(), indexedPoolLocs, hpIndexes);
  }

  public Set<String> getHostCoLocationGroupIds() {
    if (_topNode.getResources().getCoLocations() == null) {
      return new HashSet<>();
    } else return _topNode.getResources().getCoLocations();
  }

  public Set<String> getHostExLocationGroupIds() {
    return _topNode.getResources().getExLocations();
  }

  public boolean isHostIsolation() {
    return _topNode.getResources().isIsolation();
  }
  // partition level constraints
  public Set<String> getPartitionCoLocationGroupIds() {
    if (_topNode.getPartitions() == null || _topNode.getPartitions().getCoLocations() == null) {
      return new HashSet<>();
    }
    return _topNode.getPartitions().getCoLocations();
  }

  public Set<String> getPartitionExLocationGroupIds() {
    return _topNode.getPartitions().getExLocations();
  }

  public Map<String, Set<String>> getProvisionalLocations() {
    Map<String, Set<String>> locMap = new HashMap<>();
    Map<String, Set<String>> nodeProvMap = _topNode.getPartitions().getProvisionalLocations();
    if (nodeProvMap != null) {
      for (String provKey : nodeProvMap.keySet()) {
        locMap.put(provKey, new HashSet<String>(nodeProvMap.get(provKey)));
      }
    }
    return locMap;
  }

  public Map<String, Set<String>> getSegregatedLocations() {
    Map<String, Set<String>> locMap = new HashMap<>();
    Map<String, Set<String>> nodeSegMap = _topNode.getPartitions().getSegregatedLocations();
    if (nodeSegMap != null) {
      for (String setKey : nodeSegMap.keySet()) {
        locMap.put(setKey, new HashSet<String>(nodeSegMap.get(setKey)));
      }
    }
    return locMap;
  }

  public Map<String, Set<String>> getSoftProvisionalLocations() {
    Map<String, Set<String>> locMap = new HashMap<>();
    Map<String, Set<String>> nodeProvMap = _topNode.getPartitions().getSoftProvisionalLocations();
    if (nodeProvMap != null) {
      for (String provKey : nodeProvMap.keySet()) {
        locMap.put(provKey, new HashSet<String>(nodeProvMap.get(provKey)));
      }
    }
    return locMap;
  }

  public Map<String, Set<String>> getSoftSegregatedLocations() {
    Map<String, Set<String>> locMap = new HashMap<>();
    Map<String, Set<String>> nodeSegMap = _topNode.getPartitions().getSoftSegregatedLocations();
    if (nodeSegMap != null) {
      for (String setKey : nodeSegMap.keySet()) {
        locMap.put(setKey, new HashSet<String>(nodeSegMap.get(setKey)));
      }
    }
    return locMap;
  }

  public Boolean isPartitionIsolated() {
    return _topNode.getPartitions().isIsolation();
  }

  public Boolean isPartitionSoftIsolated() {
    return _topNode.getPartitions().isSoftIsolation();
  }

  public String getAbstract() {
    StringBuilder str = new StringBuilder();
    str.append("CCN[" + getNodeName() + "(" + getNodeIndex() + ")]");
    str.append("conns{");
    boolean first = true;
    for (TopologyNode node : getConnectedTopNodes()) {
      if (!first) str.append(";");
      str.append(node.getIndex());
      first = false;
    }
    str.append("} ");
    return (str.toString());
  }

  public long getSize() {
    return 1;
  }

  /// get colocation group
  ///   .empty() if none.
  public String getColocationGroupId() {
    String cgId = null;
    if ((getPartitionCoLocationGroupIds() != null) && !getPartitionCoLocationGroupIds().isEmpty()) {
      cgId = getPartitionCoLocationGroupIds().iterator().next();
    }
    return (cgId);
  }

  public Set<ComposerCriteriaNode> getColocatedNodes() {
    Set<ComposerCriteriaNode> nodeSet = new HashSet<>();

    String cgId = getColocationGroupId();
    Trace.logTrace(" CG_ID = '" + cgId + "'");

    if (cgId != null) {
      nodeSet = _composer.getColocatedPartitionGroup(cgId)._colocatedNodes;
    }

    for (ComposerCriteriaNode node : nodeSet) {
      Trace.logTrace(" NodeSetMbr: " + node.getNodeIndex());
    }
    return (nodeSet);
  }

  @Override
  public boolean equals(Object obj) {

    if (obj == null) {
      return false;
    }
    if (obj == this) {
      return true;
    }
    if (!obj.getClass().equals(getClass())) {
      return false;
    }
    ComposerCriteriaNode rhs = (ComposerCriteriaNode) obj;

    return new EqualsBuilder().append(_topNode.getIndex(), rhs._topNode.getIndex()).isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(_topNode.getIndex());
  }

  //	public String getConnectedTopNodes_abstract(TopologyNode topNode) {
  //		StringBuilder str = new StringBuilder();
  //		str.append("conns{");
  //		boolean first = true;
  //		for (TopologyNode node : getConnectedTopNodes_helper()) {
  //			if (!first) str.append(";");
  //			str.append(node.getIndex());
  //			first = false;
  //		}
  //		str.append("}");
  //		return str.toString();
  //	}

  public List<TopologyNode> getUpstreamConnectedTopNodes() {
    List<TopologyNode> inputConnectedNodes = new ArrayList<>();
    for (TopologyNodeInputPort inputPort : _topNode.getInputPorts().values()) {
      for (TopologyNodeConnection conn : inputPort.getOperConnections()) {
        inputConnectedNodes.add(conn.getSource().getOwningOperator());
      }
    }
    return (inputConnectedNodes);
  }

  public List<TopologyNode> getDownstreamConnectedTopNodes() {
    List<TopologyNode> outputConnectedNodes = new ArrayList<>();
    for (TopologyNodeOutputPort outPort : _topNode.getOutputPorts().values()) {
      for (TopologyNodeConnection conn : outPort.getOperConnections()) {
        outputConnectedNodes.add(conn.getDestination().getOwningOperator());
      }
    }
    return (outputConnectedNodes);
  }

  public List<TopologyNode> getConnectedTopNodes() {
    List<TopologyNode> connectedNodes = new ArrayList<>();
    connectedNodes.addAll(getUpstreamConnectedTopNodes());
    connectedNodes.addAll(getDownstreamConnectedTopNodes());
    return (connectedNodes);
  }

  public boolean isSource() {
    return (_topNode.getInputPorts().size() == 0);
  }

  public boolean isSink() {
    return (_topNode.getOutputPorts().size() == 0);
  }

  public Set<BigInteger> getExclusiveHostPoolsInApplication() {
    return (_composer.getExclusiveHostPools());
  }

  public Set<String> getOperatorHostColocationGroupIds() {
    Set<String> opHostColocGrpIds = getHostCoLocationGroupIds();
    if (getHostLocation() != null) {
      opHostColocGrpIds.add(INFERRED_COLOCATION_FROM_SAMEHOST + getHostLocation());
    }
    if ((getHostpoolIndex() != null) && (getIndexInPool() != null)) {
      opHostColocGrpIds.add(
          INFERRED_COLOCATION_FROM_SAMEHOSTPOOLINDEX + getHostpoolIndex() + "_" + getIndexInPool());
      // check if hostpool has pre-declared hosts in it.
      Hostpool hp = _composer.getApplication().getHostpools().get(getHostpoolIndex());
      if (hp.getHosts().size() > 0) {
        String declHostName = hp.getHosts().get(getIndexInPool().intValue());
        opHostColocGrpIds.add(INFERRED_COLOCATION_FROM_SAMEHOST + declHostName);
      }
    }
    return (opHostColocGrpIds);
  }

  public String getAnOperatorHostColocGrpId() {
    String anOpHostColocGrpId = null;
    for (String opHostColocGrpId : getOperatorHostColocationGroupIds()) {
      anOpHostColocGrpId = opHostColocGrpId;
      break;
    }
    return (anOpHostColocGrpId);
  }

  //    protected Set<ComposerCriteriaNode> getHostColocatedNodes_internal() {
  //        Set<ComposerCriteriaNode> hcNodes = new
  // HashSet<>(_composer.getHostColocatedNodes(this));
  //        hcNodes.add(this);
  //        return (hcNodes);
  //    }

  public void addPartitionColocationGroupId(String synthesizedPartitionColocId) {
    _topNode.getPartitions().getCoLocations().add(synthesizedPartitionColocId);
  }

  public String getId() {
    return ("CCN_" + _id);
  }

  public String getName() {
    return (_topNode.getName());
  }

  public FusableUnit getFusableUnit() {
    return _fusUnit;
  }

  public String toString() {
    return ("CCN{" + _topNode.getName() + "}");
  }

  public Set<ComposerCriteriaNode> getFusingSets() {
    if (_fusUnit != null) {
      return (_fusUnit.getFusingSets());
    }
    return getFusingSets_internal();
  }

  protected Set<ComposerCriteriaNode> getFusingSets_internal() {
    Set<ComposerCriteriaNode> fusedNodes = new HashSet<>();
    fusedNodes.add(this);
    return fusedNodes;
  }

  public void setFusableUnit(FusableUnit fu) {
    _fusUnit = fu;
  }

  public long getCpuUsage() {
    return _cpuUsage;
  }

  public void setCpuUsage(long cpuUsage) {
    _cpuUsage = cpuUsage;
  }

  public ComposerCriteriaNode(TopologyNode node, IEUComposer composer) {
    _topNode = node;
    _composer = composer;
    _id = _composer.getNextCritNodeId();
    _cpuUsage = DEFAULT_OP_CPU_USAGE;
  }

  private long _id;
  private TopologyNode _topNode;
  private IEUComposer _composer;
  private FusableUnit _fusUnit;
  private long _cpuUsage;

  public ComposedContainer getContainer() {
    ComposedContainer cont = null;
    if (_fusUnit != null) {
      cont = _fusUnit.getContainer();
    }
    return cont;
  }
}
