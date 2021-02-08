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
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey.Key;
import com.ibm.streams.platform.services.AdditionalExceptionDataType;
import com.ibm.streams.platform.services.MessageElementType;
import com.ibm.streams.sch.composer.FusingConstraintChecker.ConstraintViolation;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import org.apache.commons.lang.builder.EqualsBuilder;

public class FusableUnit implements FusingConstraintCriteria {

  // return null if no problems found.
  public AdditionalExceptionDataType validate(boolean standAloneMode) throws StreamsException {
    //		Trace.logDebug("FU:validate - begin");
    AdditionalExceptionDataType aedt = null;
    List<MessageElementType> problemConstraints = new ArrayList<>();
    Set<ComposerCriteriaNode> hostColocatedNodes = new HashSet<>();
    FusingConstraintChecker fcc = new FusingConstraintChecker(this, null, _composer);
    Set<ConstraintViolation> cViols =
        fcc.checkForIncompatibleConstraints(
            false, standAloneMode, false, problemConstraints, hostColocatedNodes);
    //		Trace.logDebug("xyz: problemDetected["+problemDetected+"]
    // problemconstraints("+problemConstraints.size()+"): " +
    // problemConstraints);

    if (cViols != null) {
      List<MessageElementType> constraintMsgs = new ArrayList<>();
      constraintMsgs.addAll(problemConstraints);
      for (ComposerCriteriaNode compCritNode : _ccNodes) {
        constraintMsgs.add(compCritNode.getCurrentConstraintMessage(cViols, false));
      }
      for (ComposerCriteriaNode compCritNode : hostColocatedNodes) {
        constraintMsgs.add(compCritNode.getCurrentConstraintMessage(cViols, true));
      }
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Problem constraints discovered: ");
        for (String textMsg : ComposerSupportHelper.convertMessagesToText(problemConstraints)) {
          Trace.logDebug(textMsg);
        }
      }
      MessageElementType msg = new MessageElementType();
      msg.setMessageCode(Key.SCHIncompatibleFusingConstraints_forPE.getMessageId());
      msg.getSubstitutionText().add(getDisplayName());
      aedt = new AdditionalExceptionDataType();
      aedt.setPrimaryInformation(msg);
      aedt.getSecondaryInformation().addAll(constraintMsgs);
    }
    ;
    //		Trace.logDebug("FU:validate - end");
    return (aedt);
  }

  protected void addTopologyNodeOnly(TopologyNode tNode) {
    ComposerCriteriaNode ccNode = new ComposerCriteriaNode(tNode, _composer);
    addCriteriaNode(ccNode);
  }

  public void addCriteriaNodes(Collection<ComposerCriteriaNode> ccNodes) {
    for (ComposerCriteriaNode ccNode : ccNodes) {
      addCriteriaNode(ccNode);
    }
  }

  private void addCriteriaNode_internal(ComposerCriteriaNode ccNode) {
    if (!_ccNodes.contains(ccNode)) {
      ccNode.setFusableUnit(this);
      _ccNodes.add(ccNode);
      _compositeHostSpecs.add(ccNode.getHostSpec());
      if (ccNode.isHostIsolation()) _compositeHostIsolation = true;
      if (ccNode.getPoolLocation() != null) _compositePoolLocs.add(ccNode.getPoolLocation());
      for (String hostExLocGrpId : ccNode.getHostExLocationGroupIds()) {
        Set<String> opNameSet = _compositeHostExLocs.get(hostExLocGrpId);
        if (opNameSet == null) {
          opNameSet = new HashSet<>();
          _compositeHostExLocs.put(hostExLocGrpId, opNameSet);
        }
        opNameSet.add(ccNode.getName());
      }
      for (String hostCoLocGrpId : ccNode.getHostCoLocationGroupIds()) {
        Set<String> opNameSet = _compositeHostCoLocs.get(hostCoLocGrpId);
        if (opNameSet == null) {
          opNameSet = new HashSet<>();
          _compositeHostCoLocs.put(hostCoLocGrpId, opNameSet);
        }
        opNameSet.add(ccNode.getName());
      }
      _compositePartCoLocs.addAll(ccNode.getPartitionCoLocationGroupIds());
      _compositePartExLocs.addAll(ccNode.getPartitionExLocationGroupIds());
      _compositePartIsols.add(ccNode.isPartitionIsolated());
      _compositeNodeNames.add(ccNode.getName());

      BigInteger hpIndex = ccNode.getHostpoolIndex();
      if (hpIndex != null) {
        if (ccNode.getExclusiveHostPoolsInApplication().contains(hpIndex)) {
          _compositeXhpIndexes.add(hpIndex);
        }
      }
      _compositeRst.add(ccNode.isRestartable());
      _compositeRloc.add(ccNode.isRelocatable());
      //			_compositeHostColocNodes.addAll(ccNode.getHostColocatedNodes());
      //			_compositeHostColocNodes.add(ccNode);

      Map<String, Set<String>> provMap = ccNode.getProvisionalLocations();
      for (String provKey : provMap.keySet()) {
        Set<Set<String>> provSetSet = _compositeProvLocs.get(provKey);
        if (provSetSet == null) {
          provSetSet = new HashSet<>();
          _compositeProvLocs.put(provKey, provSetSet);
        }
        provSetSet.add(provMap.get(provKey));
      }

      Map<String, Set<String>> segMap = ccNode.getSegregatedLocations();
      for (String segKey : segMap.keySet()) {
        Multiset<Set<String>> segSetSet = _compositeSegLocs.get(segKey);
        if (segSetSet == null) {
          segSetSet = HashMultiset.create();
          _compositeSegLocs.put(segKey, segSetSet);
        }
        segSetSet.add(segMap.get(segKey));
      }
      Map<String, Set<String>> softProvMap = ccNode.getSoftProvisionalLocations();
      for (String provKey : softProvMap.keySet()) {
        Set<Set<String>> softProvSetSet = _compositeSoftProvLocs.get(provKey);
        if (softProvSetSet == null) {
          softProvSetSet = new HashSet<>();
          _compositeSoftProvLocs.put(provKey, softProvSetSet);
        }
        softProvSetSet.add(softProvMap.get(provKey));
      }

      Map<String, Set<String>> softSegMap = ccNode.getSoftSegregatedLocations();
      for (String segKey : softSegMap.keySet()) {
        Multiset<Set<String>> segSetSet = _compositeSoftSegLocs.get(segKey);
        if (segSetSet == null) {
          segSetSet = HashMultiset.create();
          _compositeSoftSegLocs.put(segKey, segSetSet);
        }
        if (!segSetSet.contains(softSegMap.get(segKey))) {
          // keep multiset to one entry.
          segSetSet.add(softSegMap.get(segKey));
        }
      }
    }
  }

  public Set<BigInteger> addCriteriaNode(ComposerCriteriaNode ccNode) {
    Trace.logTrace("addNode for " + ccNode.getNodeIndex());
    Set<BigInteger> addedNodes = new HashSet<BigInteger>();
    addCriteriaNode_internal(ccNode);

    addedNodes.add(ccNode.getNodeIndex());

    // add colocated PEs as well
    for (ComposerCriteriaNode colocNode : ccNode.getColocatedNodes()) {
      Trace.logTrace("adding colocated Node:" + colocNode.getNodeIndex());
      addCriteriaNode_internal(colocNode);
      addedNodes.add(colocNode.getNodeIndex());
    }

    return (addedNodes);
  }

  public boolean isHostIsolated() {
    return _compositeHostIsolation;
  }

  public boolean isPartitionIsolated() {
    for (ComposerCriteriaNode node : _ccNodes) {
      if (node.isPartitionIsolated() != null && node.isPartitionIsolated()) {
        return (true);
      }
    }
    return false;
  }

  public void setPartitionSoftIsolated(boolean partSoftIsol) {
    _partitionGroupIsolation = partSoftIsol;
  }

  public boolean isPartitionSoftIsolated() {
    if (_partitionGroupIsolation) return true;
    for (ComposerCriteriaNode node : _ccNodes) {
      if (node.isPartitionSoftIsolated() != null && node.isPartitionSoftIsolated()) {
        return (true);
      }
    }
    return false;
  }

  public List<TopologyNode> getTopologyNodes() {
    List<TopologyNode> topNodes = new ArrayList<>();
    for (ComposerCriteriaNode compCritNode : _ccNodes) {
      topNodes.add(compCritNode.getTopologyNode());
    }
    return topNodes;
  }

  public HostSpec getHostSpec() {
    Set<String> declaredHostNames = null;
    Set<PoolLocation> indexedPoolLocs = null;
    Set<String> tags = new HashSet<>();
    Set<BigInteger> hpIndexes = new HashSet<>();
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      HostSpec ccHS = ccNode.getHostSpec();
      hpIndexes.addAll(ccHS.getHostpoolIndexes());

      // this.dHosts  new.dHosts  result.dHosts
      //     null        null        null
      //     null        hosts       new.dHosts
      //     hosts       null        this.dHosts
      //     hosts       hosts       intersect of both hosts.
      if (declaredHostNames == null) {
        declaredHostNames = ccHS.getDeclaredHosts();
      } else {
        if (ccHS.getDeclaredHosts() != null) {
          declaredHostNames.retainAll(ccHS.getDeclaredHosts());
        }
      }
      tags.addAll(ccHS.getTags());
      if (ccHS.getIndexedPoolLocations() != null) {
        if (indexedPoolLocs == null) {
          indexedPoolLocs = new HashSet<>();
        }
        indexedPoolLocs.addAll(ccHS.getIndexedPoolLocations());
      }
    }
    return new HostSpec(declaredHostNames, tags, indexedPoolLocs, hpIndexes);
  }

  @Override
  public Set<HostSpec> getCompositeHostSpecs() {
    return (_compositeHostSpecs);
  }

  //    @Override
  //    public Set<String> getCompositeHostLocations() {
  //        return(_compositeHostLocations);
  //    }
  //
  @Override
  public Set<PoolLocation> getCompositePoolLocations() {
    return (_compositePoolLocs);
  }

  @Override
  public Map<String, Set<String>> getCompositeHostCoLocationGroupIds() {
    return (_compositeHostCoLocs);
  }

  @Override
  public Map<String, Set<String>> getCompositeHostExLocationGroupIds() {
    return (_compositeHostExLocs);
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
    return _compositeProvLocs;
  }

  @Override
  public Map<String, Multiset<Set<String>>> getCompositeSegregatedLocations() {
    return (_compositeSegLocs);
  }

  @Override
  public Map<String, Set<Set<String>>> getCompositeSoftProvisionalLocations() {
    return _compositeSoftProvLocs;
  }

  @Override
  public Map<String, Multiset<Set<String>>> getCompositeSoftSegregatedLocations() {
    return (_compositeSoftSegLocs);
  }

  @Override
  public Multiset<BigInteger> getCompositeExclusiveHostPoolIndexes() {
    return (_compositeXhpIndexes);
  }

  @Override
  public Set<Boolean> getCompositeRestartable() {
    return (_compositeRst);
  }

  @Override
  public Set<Boolean> getCompositeRelocatable() {
    return (_compositeRloc);
  }

  public Set<ComposerCriteriaNode> getFusingSets() {
    if (_container != null) {
      return (_container.getFusingSets());
    }
    return getFusingSets_internal();
  }

  protected Set<ComposerCriteriaNode> getFusingSets_internal() {
    Set<ComposerCriteriaNode> fSets = new HashSet<>();
    for (ComposerCriteriaNode ccN : _ccNodes) {
      fSets.addAll(ccN.getFusingSets_internal());
    }
    return fSets;
  }

  //	public Set<ComposerCriteriaNode> getCompositeHostColocatedNodes_internal() {
  //        Set<ComposerCriteriaNode> hCoLocNodes = new HashSet<>();
  //        for (ComposerCriteriaNode ccNode : _ccNodes) {
  //            hCoLocNodes.addAll(ccNode.getHostColocatedNodes_internal());
  //        }
  //        return(hCoLocNodes);
  //    }

  @Override
  public Set<String> getCompositeNamesOfContributingNodes() {
    return (_compositeNodeNames);
  }

  public String toString() {
    StringBuilder str = new StringBuilder();
    str.append("FU: [" + _id + "] ");
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      str.append(ccNode.getAbstract() + ";");
    }
    if (!_compositeHostCoLocs.isEmpty()) {
      str.append(" compHostColoc:" + _compositeHostCoLocs);
    }
    if (!_compositeHostExLocs.isEmpty()) {
      str.append("compHostExloc:" + _compositeHostExLocs);
    }
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
    str.append("\n");
    str.append("      connections: " + toStringForConnections() + "\n");
    return str.toString();
  }

  public String toStringForConnections() {
    StringBuilder str = new StringBuilder();
    str.append("up:");
    for (Multiset.Entry<FusableUnit> fuEntry : _interFuUpstreamConnections.entrySet()) {
      str.append("[" + fuEntry.getCount() + ":" + fuEntry.getElement().getAbstract() + "] ");
    }
    str.append(" dn:");
    for (Multiset.Entry<FusableUnit> fuEntry : _interFuDownstreamConnections.entrySet()) {
      str.append("[" + fuEntry.getCount() + ":" + fuEntry.getElement().getAbstract() + "] ");
    }
    return (str.toString());
  }

  public String getDisplayName() {
    StringBuilder str = new StringBuilder();
    boolean first = true;
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      if (!first) str.append(",");
      str.append(ccNode.getName());
      first = false;
    }
    return (str.toString());
  }

  public static Set<String> extractOperatorNamesFromFusUnitAbstract(String fuAbstract) {
    String[] opSetStr = fuAbstract.split(",");
    Set<String> opsSet = new HashSet<>();
    for (String opName : opSetStr) {
      opsSet.add(opName);
    }
    return opsSet;
  }

  public String getName() {
    StringBuilder str = new StringBuilder();
    str.append("[" + _id + ":");
    boolean first = true;
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      if (!first) str.append(";");
      str.append(ccNode.getNodeIndex());
      first = false;
    }
    str.append("]");
    return (str.toString());
  }

  public String getAbstract() {
    StringBuilder str = new StringBuilder();
    str.append(getDisplayName());
    //		str.append("conns{");
    //		boolean first = true;
    //		for (Multiset.Entry<FusableUnit> fuEntry : _interFuConnections.entrySet()) {
    //			if (!first) str.append(";");
    //			str.append("n"+fuEntry.getElement().getId()+"("+fuEntry.getCount()+")");
    //			first = false;
    //		}
    //		str.append("}");
    return str.toString();
  }

  public boolean includesSinkNode() {
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      if (ccNode.isSink()) return true;
    }
    return false;
  }

  public int numOfSinkNodes() {
    int nSinks = 0;
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      if (ccNode.isSink()) nSinks++;
    }
    return nSinks;
  }

  public boolean includesSourceNode() {
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      if (ccNode.isSource()) return true;
    }
    return false;
  }

  public int numOfSourceNodes() {
    int nSources = 0;
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      if (ccNode.isSource()) nSources++;
    }
    return nSources;
  }

  public FusableUnit getFuForTopNode(TopologyNode tNode) {
    return (_composer.getFuForTopNode(tNode));
  }

  public void populateConnections() {
    for (TopologyNode tNode : getUpstreamConnectedTopNodes()) {
      FusableUnit fu = _composer.getFuForTopNode(tNode);
      if (fu != this) {
        _interFuUpstreamConnections.add(fu);
      }
    }
    for (TopologyNode tNode : getDownstreamConnectedTopNodes()) {
      FusableUnit fu = _composer.getFuForTopNode(tNode);
      if (fu != this) {
        _interFuDownstreamConnections.add(fu);
      }
    }
  }

  protected List<TopologyNode> getConnectedTopNodes() {
    List<TopologyNode> connTopNodes = new ArrayList<>();
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      connTopNodes.addAll(ccNode.getConnectedTopNodes());
    }
    return (connTopNodes);
  }

  protected List<TopologyNode> getUpstreamConnectedTopNodes() {
    List<TopologyNode> connTopNodes = new ArrayList<>();
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      connTopNodes.addAll(ccNode.getUpstreamConnectedTopNodes());
    }
    return (connTopNodes);
  }

  protected List<TopologyNode> getDownstreamConnectedTopNodes() {
    List<TopologyNode> connTopNodes = new ArrayList<>();
    for (ComposerCriteriaNode ccNode : _ccNodes) {
      connTopNodes.addAll(ccNode.getDownstreamConnectedTopNodes());
    }
    return (connTopNodes);
  }

  public int numOfUpstreamConnectionsWith(FusableUnit fu) {
    return (_interFuUpstreamConnections.count(fu));
  }

  public int numOfDownstreamConnectionsWith(FusableUnit fu) {
    return (_interFuDownstreamConnections.count(fu));
  }

  public int numOfConnectionsWith(FusableUnit fu) {
    return (numOfUpstreamConnectionsWith(fu) + numOfDownstreamConnectionsWith(fu));
  }

  public Multiset<FusableUnit> getInterUpstreamFuConnections() {
    return (_interFuUpstreamConnections);
  }

  public Multiset<FusableUnit> getInterDownstreamFuConnections() {
    return (_interFuDownstreamConnections);
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
    FusableUnit rhs = (FusableUnit) obj;

    return new EqualsBuilder().append(_ccNodes, rhs._ccNodes).isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(_ccNodes);
  }

  @Override
  public Set<ComposerCriteriaNode> getCompositeCcNodes() {
    return (_ccNodes);
  }

  @Override
  public String getId() {
    Set<String> opNames = new HashSet<>();
    for (ComposerCriteriaNode ccN : _ccNodes) {
      opNames.add(ccN.getName());
    }
    return (opNames.toString());
    //    	return ("FU_"+_id);
  }

  // minimize # of hostspecs, by collapsing appropriate ones.
  //	private Set<HostSpec> collapseHostSpecs(Set<HostSpec> hostSpecs) {
  //	    Set<HostSpec> collapsedHostSpecs = new HashSet<>();
  //	    for (HostSpec hSpec : hostSpecs) {
  //	        boolean collapsed = false;
  //	        for (HostSpec chSpec : collapsedHostSpecs) {
  //	             if (chSpec.collapse(hSpec)) {
  //	                 collapsed = true;
  //	                 break;
  //	             }
  //	        }
  //	        if (!collapsed) {
  //	            collapsedHostSpecs.add(hSpec);
  //	        }
  //	    }
  //        Set<HostSpec> refreshedCollapsedHostSpecs = new HashSet<>(collapsedHostSpecs);
  //	    return refreshedCollapsedHostSpecs;
  //	}

  @Override
  public Set<HostSpec> getCompositeResolvedHostSpecs() {
    Set<HostSpec> rHostSpecs = new HashSet<>();
    rHostSpecs.add(getHostSpec());
    return rHostSpecs;
  }

  public int getNumOfFusedNodes() {
    return getTopologyNodes().size();
  }

  public void setContainer(ComposedContainer cont) {
    _container = cont;
  }

  public ComposedContainer getContainer() {
    return _container;
  }

  // get set of nodes that must be host-colocated with this FU.
  //   skip the already processed containers & fu's
  //	public Set<ComposerCriteriaNode> getHostColocatedNodes(Set<ComposedContainer>
  // alreadyProcessed_containers,
  // Set<FusableUnit> alreadyProcess_fus) {
  //		Set<ComposerCriteriaNode> hostColocNodes = new HashSet<>();
  //		Set<ComposerCriteriaNode> primaryNodes = getFusingSets();
  //		hostColocNodes.addAll(primaryNodes);
  //		Set<ComposerCriteriaNode> secondaryNodes = new HashSet<>();
  //		for (ComposerCriteriaNode ccNode : primaryNodes) {
  //			secondaryNodes.addAll(_composer.getCoLocGroupMembers(ccNode));
  //		}
  //		secondaryNodes.removeAll(primaryNodes);
  //		hostColocNodes.addAll(secondaryNodes);
  //		alreadyProcess_fus.add(this);
  //		for (ComposerCriteriaNode ccNode: secondaryNodes) {
  //			ComposedContainer nodeCont = ccNode.getContainer();
  //			if (nodeCont != null) {
  //				if (!alreadyProcessed_containers.contains(nodeCont)) {
  //
  //	hostColocNodes.addAll(nodeCont.getHostColocatedNodes(alreadyProcessed_containers,alreadyProcess_fus));
  //				}
  //			} else {
  //				// must be a fusable unit that hasn't been placed into a container yet.
  //				FusableUnit fu = ccNode.getFusableUnit();
  //				if (!alreadyProcess_fus.contains(fu)) {
  //					hostColocNodes.addAll(fu.getHostColocatedNodes(alreadyProcessed_containers,
  // alreadyProcess_fus));
  //				}
  //			}
  //		}
  //		return hostColocNodes;
  //	}

  @Override
  public long getCpuUsage() {
    long cpuUsage = 0;
    for (ComposerCriteriaNode ccn : _ccNodes) {
      cpuUsage += ccn.getCpuUsage();
    }
    return cpuUsage;
  }

  public boolean speciallyFused() {
    int numOfNodes = _ccNodes.size();
    for (Multiset<Set<String>> ssLocs : _compositeSoftSegLocs.values()) {
      if (ssLocs.entrySet().size() != numOfNodes) {
        return true;
      }
    }
    return false;
  }

  @Override
  public IEUComposer getComposer() {
    return (_composer);
  }

  public FusableUnit(IEUComposer composer) {
    _ccNodes = new HashSet<>();
    _container = null;
    _interFuUpstreamConnections = HashMultiset.create();
    _interFuDownstreamConnections = HashMultiset.create();
    _composer = composer;
    _id = _composer.getNextFusableUnitId();
    _compositePoolLocs = new HashSet<>();
    _compositeHostSpecs = new HashSet<>();
    _compositeHostCoLocs = new HashMap<>();
    _compositeHostExLocs = new HashMap<>();
    _compositeHostIsolation = false;
    _compositePartCoLocs = new HashSet<>();
    _compositePartExLocs = new ArrayList<>();
    _compositePartIsols = new ArrayList<>();
    _compositeProvLocs = new HashMap<>();
    _compositeSegLocs = new HashMap<>();
    _compositeSoftProvLocs = new HashMap<>();
    _compositeSoftSegLocs = new HashMap<>();
    _compositeXhpIndexes = HashMultiset.create();
    _compositeRst = new HashSet<>();
    _compositeRloc = new HashSet<>();
    _compositeNodeNames = new HashSet<>();
    _partitionGroupIsolation = false;
  }

  private long _id;
  private Set<ComposerCriteriaNode> _ccNodes;
  private ComposedContainer _container;
  private Multiset<FusableUnit> _interFuUpstreamConnections;
  private Multiset<FusableUnit> _interFuDownstreamConnections;
  private IEUComposer _composer;
  private Set<PoolLocation> _compositePoolLocs;
  private Set<HostSpec> _compositeHostSpecs;
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
  private Multiset<BigInteger> _compositeXhpIndexes;
  private Set<Boolean> _compositeRst;
  private Set<Boolean> _compositeRloc;
  private Set<String> _compositeNodeNames;
  private boolean _partitionGroupIsolation;
}
