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

import com.google.common.collect.HashMultiset;
import com.google.common.collect.Multiset;
import com.ibm.streams.instance.sam.model.composer.ContainerSpecification;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

public class ComposedContainer implements FusingConstraintCriteria {

  public void addFusableUnits(Collection<FusableUnit> fusableUnits) {
    for (FusableUnit fu : fusableUnits) {
      addFusableUnit(fu);
    }
  }

  public void addFusableUnit(FusableUnit fu) {
    _fusUnits.add(fu);
    fu.setContainer(this);
    _interUpstreamContainerConnections.remove(fu, 10000);
    _interDownstreamContainerConnections.remove(fu, 10000);
    for (Multiset.Entry<FusableUnit> fuEntry : fu.getInterUpstreamFuConnections().entrySet()) {
      if (!_fusUnits.contains(fuEntry.getElement())) {
        for (int i = 0; i < fuEntry.getCount(); i++) {
          _interUpstreamContainerConnections.add(fuEntry.getElement());
        }
      }
    }
    for (Multiset.Entry<FusableUnit> fuEntry : fu.getInterDownstreamFuConnections().entrySet()) {
      if (!_fusUnits.contains(fuEntry.getElement())) {
        for (int i = 0; i < fuEntry.getCount(); i++) {
          _interDownstreamContainerConnections.add(fuEntry.getElement());
        }
      }
    }
    // update cached composite info
    //		_compositeHostLocs.addAll(fu.getCompositeHostLocations());
    _compositePoolLocs.addAll(fu.getCompositePoolLocations());
    _compositeHostSpecs.addAll(fu.getCompositeHostSpecs());
    _compositeResolvedHostSpecs.addAll(fu.getCompositeResolvedHostSpecs());
    if (fu.isHostIsolated()) _compositeHostIsolation = true;
    Map<String, Set<String>> fu_hostExLocs = fu.getCompositeHostExLocationGroupIds();
    for (String fu_hostExLoc : fu_hostExLocs.keySet()) {
      Set<String> fu_opNameSet = fu_hostExLocs.get(fu_hostExLoc);
      Set<String> comp_opNameSet = _compositeHostExLocs.get(fu_hostExLoc);
      if (comp_opNameSet == null) {
        comp_opNameSet = new HashSet<>();
        _compositeHostExLocs.put(fu_hostExLoc, comp_opNameSet);
      }
      comp_opNameSet.addAll(fu_opNameSet);
    }
    Map<String, Set<String>> fu_hostCoLocs = fu.getCompositeHostCoLocationGroupIds();
    for (String fu_hostCoLoc : fu_hostCoLocs.keySet()) {
      Set<String> fu_opNameSet = fu_hostCoLocs.get(fu_hostCoLoc);
      Set<String> comp_opNameSet = _compositeHostCoLocs.get(fu_hostCoLoc);
      if (comp_opNameSet == null) {
        comp_opNameSet = new HashSet<>();
        _compositeHostCoLocs.put(fu_hostCoLoc, comp_opNameSet);
      }
      comp_opNameSet.addAll(fu_opNameSet);
    }
    _compositePartCoLocs.addAll(fu.getCompositePartitionCoLocationGroupIds());
    _compositePartExLocs.addAll(fu.getCompositePartitionExLocationGroupIds());
    _compositePartIsols.addAll(fu.getCompositePartitionIsolations());
    Map<String, Set<Set<String>>> provMap = fu.getCompositeProvisionalLocations();
    for (String provKey : provMap.keySet()) {
      Set<Set<String>> provSetSet = _compositeProvLocs.get(provKey);
      if (provSetSet == null) {
        provSetSet = new HashSet<>();
        _compositeProvLocs.put(provKey, provSetSet);
      }
      provSetSet.addAll(provMap.get(provKey));
    }

    Map<String, Multiset<Set<String>>> segMap = fu.getCompositeSegregatedLocations();
    for (String segKey : segMap.keySet()) {
      Multiset<Set<String>> segSetSet = _compositeSegLocs.get(segKey);
      if (segSetSet == null) {
        segSetSet = HashMultiset.create();
        _compositeSegLocs.put(segKey, segSetSet);
      }
      segSetSet.addAll(segMap.get(segKey));
    }
    Map<String, Set<Set<String>>> softProvMap = fu.getCompositeSoftProvisionalLocations();
    for (String provKey : softProvMap.keySet()) {
      Set<Set<String>> softProvSetSet = _compositeSoftProvLocs.get(provKey);
      if (softProvSetSet == null) {
        softProvSetSet = new HashSet<>();
        _compositeSoftProvLocs.put(provKey, softProvSetSet);
      }
      softProvSetSet.addAll(softProvMap.get(provKey));
    }

    Map<String, Multiset<Set<String>>> softSegMap = fu.getCompositeSoftSegregatedLocations();
    for (String segKey : softSegMap.keySet()) {
      Multiset<Set<String>> segSetSet = _compositeSoftSegLocs.get(segKey);
      if (segSetSet == null) {
        segSetSet = HashMultiset.create();
        _compositeSoftSegLocs.put(segKey, segSetSet);
      }
      segSetSet.addAll(softSegMap.get(segKey));
    }

    _compositeNodeNames.addAll(fu.getCompositeNamesOfContributingNodes());
    _compositeRst.addAll(fu.getCompositeRestartable());
    _compositeRloc.addAll(fu.getCompositeRelocatable());
    _compositeXhpIndexes.addAll(fu.getCompositeExclusiveHostPoolIndexes());
    //		_compositeHostColocNodes.addAll(fu.getCompositeHostColocatedNodes_internal());
    _compositeCcNodes.addAll(fu.getCompositeCcNodes());
  }

  public SortedSet<TopologyNode> getTopologyNodes() {
    SortedSet<TopologyNode> topNodes = new TreeSet<>();
    for (FusableUnit fusUnit : _fusUnits) {
      topNodes.addAll(fusUnit.getTopologyNodes());
    }
    return topNodes;
  }

  public int getNumOfFusedNodes() {
    return getTopologyNodes().size();
  }

  //	@Override
  //	public Set<String> getCompositeHostLocations() {
  //		return (_compositeHostLocs);
  //	}
  //
  @Override
  public Set<PoolLocation> getCompositePoolLocations() {
    return (_compositePoolLocs);
  }

  @Override
  public Set<HostSpec> getCompositeResolvedHostSpecs() {
    return _compositeResolvedHostSpecs;
  }

  @Override
  public Set<HostSpec> getCompositeHostSpecs() {
    return _compositeHostSpecs;
  }

  public boolean isHostIsolation() {
    return _compositeHostIsolation;
  }

  @Override
  public Map<String, Set<String>> getCompositeHostExLocationGroupIds() {
    return (_compositeHostExLocs);
  }

  @Override
  public Map<String, Set<String>> getCompositeHostCoLocationGroupIds() {
    return (_compositeHostCoLocs);
  }

  @Override
  public Set<String> getCompositePartitionCoLocationGroupIds() {
    return (_compositePartCoLocs);
  }

  @Override
  public List<String> getCompositePartitionExLocationGroupIds() {
    return (_compositePartExLocs);
  }

  @Override
  public List<Boolean> getCompositePartitionIsolations() {
    return (_compositePartIsols);
  }

  @Override
  public Map<String, Set<Set<String>>> getCompositeProvisionalLocations() {
    return (_compositeProvLocs);
  }

  @Override
  public Map<String, Multiset<Set<String>>> getCompositeSegregatedLocations() {
    return (_compositeSegLocs);
  }

  @Override
  public Map<String, Set<Set<String>>> getCompositeSoftProvisionalLocations() {
    return (_compositeSoftProvLocs);
  }

  @Override
  public Map<String, Multiset<Set<String>>> getCompositeSoftSegregatedLocations() {
    return (_compositeSoftSegLocs);
  }

  @Override
  public Set<String> getCompositeNamesOfContributingNodes() {
    return (_compositeNodeNames);
  }

  @Override
  public Set<Boolean> getCompositeRestartable() {
    return (_compositeRst);
  }

  @Override
  public Set<Boolean> getCompositeRelocatable() {
    return (_compositeRloc);
  }

  @Override
  public Multiset<BigInteger> getCompositeExclusiveHostPoolIndexes() {
    return (_compositeXhpIndexes);
  }

  @Override
  public Set<ComposerCriteriaNode> getCompositeCcNodes() {
    return (_compositeCcNodes);
  }

  public boolean isEmpty() {
    return (numOfFusableUnits() == 0);
  }

  public int numOfFusableUnits() {
    return _fusUnits.size();
  }

  public String toString() {
    StringBuilder str = new StringBuilder();
    str.append("\n====================== CC:[" + _id + "] {\n");
    if (!_compositeProvLocs.isEmpty()) {
      str.append("compProvLocs:" + _compositeProvLocs);
    }
    if (!_compositeSoftProvLocs.isEmpty()) {
      str.append("compSoftProvLocs:" + _compositeSoftProvLocs);
    }
    if (!_compositeSegLocs.isEmpty()) {
      str.append("compSegrLocs:" + _compositeSegLocs);
    }
    if (!_compositeSoftSegLocs.isEmpty()) {
      str.append("compSoftSegrLocs:" + _compositeSoftSegLocs);
    }
    str.append("  numOfFus [" + _fusUnits.size() + "]\n");
    str.append("  targetCpuCapacity [" + _targetCpuLoad + "]");
    for (FusableUnit fu : _fusUnits) {
      str.append("  " + fu.toString());
    }
    str.append(connectionsToString());
    str.append("-----------------------}\n");
    return (str.toString());
  }

  public String connectionsToString() {
    StringBuilder str = new StringBuilder();
    str.append("upstreamConns {");
    for (Multiset.Entry<FusableUnit> fuEntry : _interUpstreamContainerConnections.entrySet()) {
      str.append(fuEntry.getCount() + ":" + fuEntry.getElement().getAbstract() + " ");
    }
    str.append("}\n");
    str.append("downstreamConns {");
    for (Multiset.Entry<FusableUnit> fuEntry : _interDownstreamContainerConnections.entrySet()) {
      str.append(fuEntry.getCount() + ":" + fuEntry.getElement().getAbstract() + " ");
    }
    str.append("}\n");
    return str.toString();
  }

  public String getDisplayName() {
    StringBuilder str = new StringBuilder();
    str.append(getId() + ":\n");
    //		boolean first = true;
    for (FusableUnit fu : _fusUnits) {
      //			if (!first) str.append(";");
      //			first = false;
      str.append("      " + fu.getId() + "\n");
    }
    return str.toString();
  }

  public String getAbstract() {
    StringBuilder str = new StringBuilder();
    str.append(getDisplayName());
    //		str.append("conns{");
    //		boolean first = true;
    //		for (Multiset.Entry<FusableUnit> fuEntry : _interContainerConnections.entrySet()) {
    //			if (!first) str.append(";");
    //			str.append("n"+fuEntry.getElement().getId()+"("+fuEntry.getCount()+")");
    //			first = false;
    //		}
    //		str.append("}");
    return str.toString();
  }

  public int numOfConnectionsWith(FusableUnit fu) {
    return (numOfUpstreamConnectionsWith(fu) + numOfDownstreamConnectionsWith(fu));
  }

  public int numOfUpstreamConnectionsWith(FusableUnit fu) {
    return _interUpstreamContainerConnections.count(fu);
  }

  public int numOfDownstreamConnectionsWith(FusableUnit fu) {
    return _interDownstreamContainerConnections.count(fu);
  }

  //	Multiset<FusableUnit> getInterContainerConnections() {
  //		return (_interContainerConnections);
  //	}

  //	public int numOfInterContainerConnections() {
  //		int numOfConns = 0;
  //		for (Multiset.Entry<FusableUnit> entry : getInterContainerConnections().entrySet()) {
  ////			Bwf.p("  Connection to:" + entry.getElement().toString());
  //			numOfConns += entry.getCount();
  //		}
  //		return (numOfConns);
  //	}

  public FusableUnit getFuForTopNode(TopologyNode tNode) {
    return (_composer.getFuForTopNode(tNode));
  }

  public void addTopologyNodesOnly(Collection<TopologyNode> nodes) {
    for (TopologyNode node : nodes) {
      addTopologyNodeOnly(node);
    }
  }

  public void addTopologyNodeOnly(TopologyNode node) {
    FusableUnit fu = new FusableUnit(_composer);
    fu.addTopologyNodeOnly(node);
    _fusUnits.add(fu);
  }

  @Override
  public String getId() {
    return "CC_" + _id;
  }

  public void setNumberOfTargetOperators(Integer numOfOps) {
    _targetNumOfOperators = numOfOps;
  }

  public void setTargetCpuLoad(Long cpuLoad) {
    // TODO - commented out until op metrics support is ready.
    //        _targetCpuLoad = cpuLoad;
  }

  @Override
  public IEUComposer getComposer() {
    return _composer;
  }

  public long getAvailableCpuCapacity() {
    if (_targetCpuLoad != null) {
      return _targetCpuLoad - getCompositeCcNodeCpuUsage();
    } else {
      return 99;
    }
  }

  public boolean full() {
    // cpu load based check has priority
    if (_targetCpuLoad != null) {
      return (getCompositeCcNodeCpuUsage() >= _targetCpuLoad);
    }
    if (_targetNumOfOperators != null) {
      return (_compositeCcNodes.size() >= _targetNumOfOperators);
    }
    return false;
  }

  private long getCompositeCcNodeCpuUsage() {
    long cpuLoad = 1;
    for (ComposerCriteriaNode ccNode : _compositeCcNodes) {
      cpuLoad += ccNode.getCpuUsage();
    }
    return cpuLoad;
  }

  public int getNumOfFusableUnits() {
    return (_fusUnits.size());
  }

  public int numOfDownstreamInterContainerConnections() {
    int numOfInterContConns = 0;
    for (Multiset.Entry<FusableUnit> iConn : _interDownstreamContainerConnections.entrySet()) {
      numOfInterContConns += iConn.getCount();
    }
    return numOfInterContConns;
  }

  public int numOfUpstreamInterContainerConnections() {
    int numOfInterContConns = 0;
    for (Multiset.Entry<FusableUnit> iConn : _interUpstreamContainerConnections.entrySet()) {
      numOfInterContConns += iConn.getCount();
    }
    return numOfInterContConns;
  }

  public int numOfInterContainerConnections() {
    int numOfInterContConns = 0;
    for (Multiset.Entry<FusableUnit> iConn : _interDownstreamContainerConnections.entrySet()) {
      numOfInterContConns += iConn.getCount();
    }
    for (Multiset.Entry<FusableUnit> iConn : _interUpstreamContainerConnections.entrySet()) {
      numOfInterContConns += iConn.getCount();
    }
    return numOfInterContConns;
  }

  public Set<ComposerCriteriaNode> getFusingSets() {
    Set<ComposerCriteriaNode> fSet = new HashSet<>();
    for (FusableUnit fu : _fusUnits) {
      fSet.addAll(fu.getFusingSets_internal());
    }
    return fSet;
  }

  public Set<FusableUnit> getFusableUnits() {
    return new HashSet<>(_fusUnits);
  }

  // get set of nodes that must be host co-located with this container.
  //   skip the containers & fu's that have already been processed.
  //	public Set<ComposerCriteriaNode> getHostColocatedNodes(Set<ComposedContainer> alreadyProcessed,
  // Set<FusableUnit>
  // alreadyProcessed_fus) {
  //		Set<ComposerCriteriaNode> hostColocNodes = new HashSet<>();
  //		Set<ComposerCriteriaNode> primaryNodes = getFusingSets();
  //		hostColocNodes.addAll(primaryNodes);
  //		Set<ComposerCriteriaNode> secondaryNodes = new HashSet<>();
  //		for (ComposerCriteriaNode ccNode : primaryNodes) {
  //			secondaryNodes.addAll(_composer.getCoLocGroupMembers(ccNode));
  //		}
  //		secondaryNodes.removeAll(primaryNodes);
  //		hostColocNodes.addAll(secondaryNodes);
  //		alreadyProcessed.add(this);
  //		for (ComposerCriteriaNode ccNode: secondaryNodes) {
  //			ComposedContainer nodeCont = ccNode.getContainer();
  //			if (nodeCont != null) {
  //				if (!alreadyProcessed.contains(nodeCont)) {
  //					hostColocNodes.addAll(nodeCont.getHostColocatedNodes(alreadyProcessed,
  // alreadyProcessed_fus));
  //				}
  //			} else {
  //				// must be a fusable unit that hasn't been placed into a container yet.
  //				FusableUnit fu = ccNode.getFusableUnit();
  //				hostColocNodes.addAll(fu.getHostColocatedNodes(alreadyProcessed, alreadyProcessed_fus));
  //			}
  //		}
  //		return hostColocNodes;
  //	}

  @Override
  public long getCpuUsage() {
    long cpuUsage = 0;
    for (FusableUnit fu : _fusUnits) {
      cpuUsage += fu.getCpuUsage();
    }
    return cpuUsage;
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
    ComposedContainer rhs = (ComposedContainer) obj;

    return new EqualsBuilder()
        .append(_id, rhs._id)
        .
        //          append(_fusUnits, rhs._fusUnits).
        isEquals();
  }

  @Override
  public int hashCode() {
    return new HashCodeBuilder()
        .append(_id)
        .
        //              append(_fusUnits).
        toHashCode();
  }

  public ComposedContainer(IEUComposer composer, ContainerSpecification contSpec) {
    this(composer);
    Set<FusableUnit> fuSet = new HashSet<>();
    for (TopologyNode tNode : contSpec.getNodes()) {
      fuSet.add(_composer.getFuForTopNode(tNode));
    }
    addFusableUnits(fuSet);
  }

  public boolean contains(FusableUnit fu) {
    return _fusUnits.contains(fu);
  }

  public ComposedContainer(IEUComposer composer) {
    _fusUnits = new ArrayList<>();
    _targetNumOfOperators = null;
    _targetCpuLoad = null;
    _id = composer.getNextContainerId();
    _composer = composer;
    _interUpstreamContainerConnections = HashMultiset.create();
    _interDownstreamContainerConnections = HashMultiset.create();
    //		_compositeHostLocs = new HashSet<>();
    _compositePoolLocs = new HashSet<>();
    _compositeHostSpecs = new HashSet<>();
    _compositeResolvedHostSpecs = new HashSet<>();
    _compositeHostIsolation = false;
    _compositeHostCoLocs = new HashMap<>();
    _compositeHostExLocs = new HashMap<>();
    _compositePartCoLocs = new HashSet<>();
    _compositePartExLocs = new ArrayList<>();
    _compositePartIsols = new ArrayList<>();
    _compositeProvLocs = new HashMap<>();
    _compositeSegLocs = new HashMap<>();
    _compositeSoftProvLocs = new HashMap<>();
    _compositeSoftSegLocs = new HashMap<>();
    _compositeCcNodes = new HashSet<>();
    _compositeXhpIndexes = HashMultiset.create();
    //		_compositeHostColocNodes = new HashSet<>();
    _compositeRloc = new HashSet<>();
    _compositeRst = new HashSet<>();
    _compositeNodeNames = new HashSet<>();
  }

  private long _id;
  private List<FusableUnit> _fusUnits;
  private Integer _targetNumOfOperators;
  private Long _targetCpuLoad;
  private IEUComposer _composer;
  private Multiset<FusableUnit> _interUpstreamContainerConnections;
  private Multiset<FusableUnit> _interDownstreamContainerConnections;

  //	private Set<String> 			           _compositeHostLocs;
  private Set<PoolLocation> _compositePoolLocs;
  private Set<HostSpec> _compositeHostSpecs;
  private Set<HostSpec> _compositeResolvedHostSpecs;
  private boolean _compositeHostIsolation;
  private Map<String, Set<String>> _compositeHostCoLocs;
  private Map<String, Set<String>> _compositeHostExLocs;
  private Set<String> _compositePartCoLocs;
  private List<String> _compositePartExLocs;
  private List<Boolean> _compositePartIsols;
  private Map<String, Set<Set<String>>> _compositeProvLocs;
  private Map<String, Multiset<Set<String>>> _compositeSegLocs;
  private Map<String, Set<Set<String>>> _compositeSoftProvLocs;
  private Map<String, Multiset<Set<String>>> _compositeSoftSegLocs;
  private Set<ComposerCriteriaNode> _compositeCcNodes;
  private Multiset<BigInteger> _compositeXhpIndexes;
  //	private Set<ComposerCriteriaNode>          _compositeHostColocNodes;
  private Set<Boolean> _compositeRloc;
  private Set<Boolean> _compositeRst;
  private Set<String> _compositeNodeNames;
}
