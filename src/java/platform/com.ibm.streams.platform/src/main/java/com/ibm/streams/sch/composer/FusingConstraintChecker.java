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
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey.Key;
import com.ibm.streams.platform.services.MessageElementType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class FusingConstraintChecker {

  public enum ConstraintViolation {
    provisionalLocation,
    softProvisionalLocation,
    segregatedLocation,
    softSegregatedLocation,
    restartable,
    relocatable,
    partitionIsolation,
    softPartitionIsolation,
    partitionExlocation,
    hostIsolation,
    hostColocation,
    hostExlocation,
    hostpool,
    declaredHost
  }

  public FusingConstraintChecker(
      FusableUnit fu, ComposedContainer container, IEUComposer composer) {
    _fu = fu;
    _container = container;
    _composer = composer;
    _partitionExLocationGroupIds = new ArrayList<>();
    _partitionIsolations = new ArrayList<>();
    _provLocations = new HashMap<>();
    _segLocations = new HashMap<>();
    _softProvLocations = new HashMap<>();
    _softSegLocations = new HashMap<>();
    _xhpIndexes = HashMultiset.create();
    _restartable = new HashSet<>();
    _relocatable = new HashSet<>();
    _contributingNodeNames = new HashSet<>();
    _hostColocatedNodes = new HashSet<>();
    _usedPartitionExlocGrpId = new HashSet<>();
    _reportedMsgs = new HashSet<>();
    //        _problemConstraintsWithHostPlacements = new ArrayList<>();
    _hostSpecs = new HashSet<>();
    _resolvedHostSpecs = new HashSet<>();
    _poolLocations = new HashSet<>();
    _hostExLocationGroups = new HashMap<>();
    _poolIndexMap = new HashMap<>();
    //        _usedHostExlocGrpId = new HashSet<>();
  }

  // null is returned if all constraints are compatible.
  //   if problemConstraints provided, then return messages in it
  //   container is optional.
  //   if container is supplied, but empty, return true.
  public Set<ConstraintViolation> checkForIncompatibleConstraints(
      boolean relaxRestartableRelocatable,
      boolean standAloneMode,
      boolean heuristicPhase,
      List<MessageElementType> problemConstraints,
      Set<ComposerCriteriaNode> out_hostColocatedNodes) {
    Set<ConstraintViolation> cViols = new HashSet<>();
    if ((_container != null) && _container.isEmpty())
      return null; // empty container always compatible with any FU.

    List<FusingConstraintCriteria> criteriaList = buildConstraintCriteriaModel();

    // Trace.logDebug("========== start of checkForIncompatibleConstraints ===========
    // cycle["+IEUComposer._compCycle+"]
    // compare["+IEUComposer._constCompare +"] =======");

    // partition provisional location groups
    //     different prov value mentioned for same prov key, error.
    //		Trace.logDebug("xyz: provLocations="+_provLocations);
    // must check these constraints even in standAloneMode.
    for (String provKey : _provLocations.keySet()) {
      Set<Set<String>> provValues = _provLocations.get(provKey);
      if (provValues.size() > 1) {
        cViols.add(ConstraintViolation.provisionalLocation);
        if (problemConstraints != null)
          problemConstraints.add(
              createMessage(
                  Key.SCHProvisionalLocationFusingConstraints.getMessageId(),
                  provKey,
                  provValues.toString()));
      }
    }

    if (!standAloneMode) {
      // normal distributed mode.

      if (!relaxRestartableRelocatable) {
        // restartable
        //    all must match
        if (_restartable.size() > 1) {
          cViols.add(ConstraintViolation.restartable);
          if (problemConstraints != null)
            problemConstraints.add(
                createMessage(
                    StreamsRuntimeMessagesKey.Key.SCHRestartableFusingConstraints.getMessageId()));
        }
        // relocatable
        //    all must match
        if (_relocatable.size() > 1) {
          cViols.add(ConstraintViolation.relocatable);
          if (problemConstraints != null)
            problemConstraints.add(
                createMessage(Key.SCHRelocatableFusingConstraints.getMessageId()));
        }
      }

      // partition isolation
      boolean isolation = false;
      for (Boolean isol : _partitionIsolations) {
        if (isol) isolation = true;
      }
      if (isolation && _partitionIsolations.size() > 1) {
        // error
        cViols.add(ConstraintViolation.partitionIsolation);
        if (problemConstraints != null)
          problemConstraints.add(
              createMessage(Key.SCHPartitionIsolationFusingConstraints.getMessageId()));
      }

      // partition ex-loc groups
      //     same ex-loc group mentioned more the once, error.
      //			Trace.logDebug("xyz["+fu.getAbstract()+"]: _partitionExlocs =" +
      // _partitionExLocationGroupIds);
      for (String partExLoc : _partitionExLocationGroupIds) {
        if (!_reportedMsgs.contains(partExLoc)) {
          if (!_usedPartitionExlocGrpId.add(partExLoc)) {
            cViols.add(ConstraintViolation.partitionExlocation);
            if (problemConstraints != null)
              problemConstraints.add(
                  createMessage(
                      Key.SCHPartitionExlocationFusingConstraints.getMessageId(), partExLoc));
            _reportedMsgs.add(partExLoc);
          }
        }
      }

      // isolation & co-location conflict.
      if (_container != null) {
        //				Bwf.p("fu: "+fu.getAbstract()+" cont: " + container.getAbstract());
        //				Bwf.p("fu.hi["+fu.isHostIsolated()+"]
        // fu.cgs["+fu.getCompositeHostCoLocationGroupIds()+"]
        // cont.hi["+container.isHostIsolation()+"]
        // cont.cgs["+container.getCompositeHostCoLocationGroupIds()+"]");
        if ((_fu.isHostIsolated() && _container.getCompositeHostCoLocationGroupIds().size() > 0)
            || (_container.isHostIsolation()
                && _fu.getCompositeHostCoLocationGroupIds().size() > 0)) {
          // error
          cViols.add(ConstraintViolation.hostIsolation);
          cViols.add(ConstraintViolation.hostColocation);
          if (problemConstraints != null)
            problemConstraints.add(
                createMessage(Key.SCHIncompatibleIsolationColocation.getMessageId()));
        }
      }

      // exclusive hostpools
      //     all or none must reference exclusive hostpool, error.
      long numOfContributingNodes = _contributingNodeNames.size();
      for (Multiset.Entry<BigInteger> xhpI : _xhpIndexes.entrySet()) {
        if (xhpI.getCount() != numOfContributingNodes) {
          cViols.add(ConstraintViolation.hostpool);
          if (problemConstraints != null)
            problemConstraints.add(
                createMessage(
                    Key.SCHExclusiveHostpoolFusingConstraints.getMessageId(),
                    Integer.toString(xhpI.getCount()),
                    Long.toString(numOfContributingNodes),
                    _composer.getHostPool(xhpI.getElement()).getName()));
        }
      }

      // partition segregation location groups
      //     different seg value mentioned for same prov key, error.
      //     if 1 node has seg, all must have seg, else error
      for (String segKey : _segLocations.keySet()) {
        Multiset<Set<String>> segValues = _segLocations.get(segKey);
        if (segValues.elementSet().size() > 1) {
          cViols.add(ConstraintViolation.segregatedLocation);
          if (problemConstraints != null)
            problemConstraints.add(
                createMessage(
                    Key.SCHSegregatedLocationFusingConstraints.getMessageId(),
                    segKey,
                    segValues.toString()));
        }
        for (Multiset.Entry<Set<String>> locAttr : segValues.entrySet()) {
          if (locAttr.getCount() != numOfContributingNodes) {
            cViols.add(ConstraintViolation.segregatedLocation);
            if (problemConstraints != null)
              problemConstraints.add(
                  createMessage(
                      Key.SCHSegregatedLocationMandatoryFusingConstraints.getMessageId(),
                      Integer.toString(locAttr.getCount()),
                      Long.toString(numOfContributingNodes),
                      segKey,
                      locAttr.getElement().toString()));
          }
        }
      }

      if (heuristicPhase) {
        // partition soft provisional location groups
        //     different prov value mentioned for same prov key, error.
        for (String provKey : _softProvLocations.keySet()) {
          Set<Set<String>> provValues = _softProvLocations.get(provKey);
          if (provValues.size() > 1) {
            cViols.add(ConstraintViolation.softProvisionalLocation);
            if (problemConstraints != null)
              problemConstraints.add(
                  createMessage(
                      Key.SCHProvisionalLocationFusingConstraints.getMessageId(),
                      provKey,
                      provValues.toString()));
          }
        }
        // partition soft segregation location groups
        //     different seg value mentioned for same prov key, error.
        //     if 1 node has seg, all must have seg, else error
        long numOfContributingFus = _container.getNumOfFusableUnits() + 1;
        for (String segKey : _softSegLocations.keySet()) {
          Multiset<Set<String>> segValues = _softSegLocations.get(segKey);
          if (segValues.elementSet().size() > 1) {
            cViols.add(ConstraintViolation.softSegregatedLocation);
            if (problemConstraints != null)
              problemConstraints.add(
                  createMessage(
                      Key.SCHSegregatedLocationFusingConstraints.getMessageId(),
                      segKey,
                      segValues.toString()));
          }
          for (Multiset.Entry<Set<String>> locAttr : segValues.entrySet()) {
            if (locAttr.getCount() != numOfContributingFus) {
              cViols.add(ConstraintViolation.segregatedLocation);
              if (problemConstraints != null)
                problemConstraints.add(
                    createMessage(
                        Key.SCHSegregatedLocationMandatoryFusingConstraints.getMessageId(),
                        Integer.toString(locAttr.getCount()),
                        Long.toString(numOfContributingNodes),
                        segKey));
            }
          }
        }
      }

      //			Trace.logDebug("xyz: problemDetected b4["+problemDetected+"]");
      Set<ConstraintViolation> constViolations =
          checkForIncompatibleConstraintsWithHostPlacements(
              criteriaList, heuristicPhase, problemConstraints, out_hostColocatedNodes, _composer);
      cViols.addAll(constViolations);
    }

    //		if (problemDetected) {
    //			Trace.logDebug("===========================================");
    //			Trace.logDebug("Problemconstraints for {"+_contributingNodeNames+"}");
    //			for (MessageElementType msg : problemConstraints) {
    //				Trace.logDebug("   "+ SupportFunctions.toStr_forAdditionalExceptionMsg(msg));
    //			}
    //			Trace.logDebug("-------------------------------------------");
    //			Trace.logDebug("");
    //		}

    //		Trace.logDebug("------------------- end of checkForIncompatibleConstraints
    // ----------------------");

    return ((cViols.size() > 0) ? cViols : null);
  }

  // return empty set if compatible
  private Set<ConstraintViolation> checkForIncompatibleConstraintsWithHostPlacements(
      List<FusingConstraintCriteria> criteriaList,
      boolean heuristicPhase,
      List<MessageElementType> problemConstraints,
      Set<ComposerCriteriaNode> out_scrubbedHostColocatedNodes,
      IEUComposer composer) {
    Set<ConstraintViolation> cViols = new HashSet<>();

    // host placement constraint checks..
    //   must include host co-located operators as well

    // how to ensure not doubly accounted??
    //		Trace.logDebug("hostColocNodes: "+_hostColocatedNodes);
    for (ComposerCriteriaNode ccNode : _hostColocatedNodes) {
      _hostSpecs.add(ccNode.getHostSpec());
      //
      // _resolvedHostSpecs.addAll(ccNode.getFusableUnit().getCompositeResolvedHostSpecs());
      //			if (ccNode.getDerivedHostLocation()!=null)
      // _hostLocations.add(ccNode.getDerivedHostLocation());
      if (ccNode.getPoolLocation() != null) _poolLocations.add(ccNode.getPoolLocation());
      for (String hostExLocGrpId : ccNode.getHostExLocationGroupIds()) {
        Set<String> opNameSet = _hostExLocationGroups.get(hostExLocGrpId);
        if (opNameSet == null) {
          opNameSet = new HashSet<>();
          _hostExLocationGroups.put(hostExLocGrpId, opNameSet);
        }
        opNameSet.add(ccNode.getName());
      }
    }

    out_scrubbedHostColocatedNodes.clear();
    out_scrubbedHostColocatedNodes.addAll(_hostColocatedNodes);
    for (FusingConstraintCriteria fCC : criteriaList) {
      out_scrubbedHostColocatedNodes.removeAll(fCC.getCompositeCcNodes());
    }

    for (FusingConstraintCriteria fcc : criteriaList) {
      _hostSpecs.addAll(fcc.getCompositeHostSpecs());
      _resolvedHostSpecs.addAll(fcc.getCompositeResolvedHostSpecs());
      //			_hostLocations.addAll(fcc.getCompositeHostLocations());
      _poolLocations.addAll(fcc.getCompositePoolLocations());
      Map<String, Set<String>> fu_hostExLocs = fcc.getCompositeHostExLocationGroupIds();
      for (String fu_hostExLoc : fu_hostExLocs.keySet()) {
        Set<String> fu_opNameSet = fu_hostExLocs.get(fu_hostExLoc);
        Set<String> comp_opNameSet = _hostExLocationGroups.get(fu_hostExLoc);
        if (comp_opNameSet == null) {
          comp_opNameSet = new HashSet<>();
          _hostExLocationGroups.put(fu_hostExLoc, comp_opNameSet);
        }
        comp_opNameSet.addAll(fu_opNameSet);
      }
    }

    // host ex-loc groups
    // same ex-loc group mentioned more then once, - error.
    for (String hostExLoc : _hostExLocationGroups.keySet()) {
      if (_hostExLocationGroups.get(hostExLoc).size() > 1) {
        cViols.add(ConstraintViolation.hostExlocation);
        cViols.add(ConstraintViolation.hostColocation);
        if (problemConstraints != null)
          problemConstraints.add(
              createMessage(Key.SCHHostExlocationFusingConstraints.getMessageId(), hostExLoc));
      }
    }

    // check host placement-level constraints.

    // check if multiple specific hosts.
    Set<String> declaredHostsByAll = null;
    Set<String> declaredHosts = new HashSet<>();
    for (HostSpec hSpec : _hostSpecs) {
      if (hSpec.getDeclaredHosts() != null) {
        declaredHosts.addAll(hSpec.getDeclaredHosts());
        if (declaredHostsByAll == null) {
          declaredHostsByAll = new HashSet<>();
          declaredHostsByAll.addAll(hSpec.getDeclaredHosts());
        } else {
          declaredHostsByAll.retainAll(hSpec.getDeclaredHosts());
        }
      }
    }
    if ((declaredHostsByAll != null) && (declaredHostsByAll.size() == 0)) {
      cViols.add(ConstraintViolation.declaredHost);
      cViols.add(ConstraintViolation.hostColocation);
      if (problemConstraints != null)
        problemConstraints.add(
            createMessage(
                Key.SCHHostnameFusingConstraints.getMessageId(), declaredHosts.toString()));
    }

    // check if multiple resolved specs.
    if (heuristicPhase) {
      if (_resolvedHostSpecs.size() > 1) {
        cViols.add(ConstraintViolation.declaredHost);
        cViols.add(ConstraintViolation.hostColocation);
        if (problemConstraints != null)
          problemConstraints.add(
              createMessage(
                  Key.SCHHostnameFusingConstraints.getMessageId(), _resolvedHostSpecs.toString()));
      }
    }

    for (PoolLocation pLoc : _poolLocations) {
      Set<BigInteger> pIndexSet = _poolIndexMap.get(pLoc.getPoolIndex());
      if (pIndexSet == null) {
        pIndexSet = new HashSet<BigInteger>();
        _poolIndexMap.put(pLoc.getPoolIndex(), pIndexSet);
      }
      if (pLoc.getInpoolIndex() != null) {
        pIndexSet.add(pLoc.getInpoolIndex());
      }
    }
    for (BigInteger pIndexSetKey : _poolIndexMap.keySet()) {
      String hostpoolName = composer.getHostPool(pIndexSetKey).getName();
      Set<BigInteger> pIndexSet = _poolIndexMap.get(pIndexSetKey);
      if (pIndexSet.size() > 1) {
        cViols.add(ConstraintViolation.hostpool);
        cViols.add(ConstraintViolation.hostColocation);
        if (problemConstraints != null)
          problemConstraints.add(
              createMessage(
                  Key.SCHIndexInHostpoolFusingConstraints.getMessageId(),
                  pIndexSet.toString(),
                  hostpoolName));
      }
    }

    if (cViols.isEmpty()) {
      out_scrubbedHostColocatedNodes.clear();
    }

    return (cViols);
  }

  private static MessageElementType createMessage(String msgCode) {
    MessageElementType msg = new MessageElementType();
    msg.setMessageCode(msgCode);
    return (msg);
  }

  private static MessageElementType createMessage(String msgCode, String subsText) {
    MessageElementType msg = new MessageElementType();
    msg.setMessageCode(msgCode);
    msg.getSubstitutionText().add(subsText);
    return (msg);
  }

  private static MessageElementType createMessage(
      String msgCode, String subsText, String subsText2) {
    MessageElementType msg = new MessageElementType();
    msg.setMessageCode(msgCode);
    msg.getSubstitutionText().add(subsText);
    msg.getSubstitutionText().add(subsText2);
    return (msg);
  }

  private static MessageElementType createMessage(
      String msgCode, String subsText, String subsText2, String subsText3) {
    MessageElementType msg = new MessageElementType();
    msg.setMessageCode(msgCode);
    msg.getSubstitutionText().add(subsText);
    msg.getSubstitutionText().add(subsText2);
    msg.getSubstitutionText().add(subsText3);
    return (msg);
  }

  private static MessageElementType createMessage(
      String msgCode, String subsText, String subsText2, String subsText3, String subsText4) {
    MessageElementType msg = new MessageElementType();
    msg.setMessageCode(msgCode);
    msg.getSubstitutionText().add(subsText);
    msg.getSubstitutionText().add(subsText2);
    msg.getSubstitutionText().add(subsText3);
    msg.getSubstitutionText().add(subsText4);
    return (msg);
  }

  private List<FusingConstraintCriteria> buildConstraintCriteriaModel() {

    List<FusingConstraintCriteria> criteriaList = new ArrayList<>();
    criteriaList.add(_fu);
    if (_container != null) criteriaList.add(_container);

    //    	_hostColocatedNodes = populateHostColocatedNodes(criteriaList);

    Set<ComposerCriteriaNode> origNodes = new HashSet<>();
    for (FusingConstraintCriteria fcc : criteriaList) {
      origNodes.addAll(fcc.getCompositeCcNodes());
    }
    _hostColocatedNodes = _composer.expandHostColocatedNodes(origNodes);
    //    	Set<ComposedContainer> alreadyProcessed = new HashSet<>();
    //		Set<FusableUnit> alreadyProcessed_fus = new HashSet<>();
    //		if (_container != null) {
    //			_hostColocatedNodes.addAll(_container.getHostColocatedNodes(alreadyProcessed,
    // alreadyProcessed_fus));
    //			alreadyProcessed.add(_container);
    //		}
    //
    //	_hostColocatedNodes.addAll(_fu.getHostColocatedNodes(alreadyProcessed,alreadyProcessed_fus));

    for (FusingConstraintCriteria constCrit : criteriaList) {
      //          Trace.logDebug("constCrit["+constCrit.getNamesOfContributingNodes()+"]
      // c"+constCrit.getCompositePartitionCoLocationGroupIds()+"
      // x"+constCrit.getCompositePartitionExLocationGroupIds()+
      //                  " i"+constCrit.getCompositePartitionIsolations()+"
      // v"+constCrit.getCompositeProvisionalLocations()+"
      // g"+constCrit.getCompositeSegregatedLocations()+
      //                  " X"+constCrit.getCompositeHostExLocationGroupIds()+"
      // R"+constCrit.getCompositeRestartable()+"
      // r"+constCrit.getCompositeRelocatable()+" CH"+constCrit.getHostColocatedNodes()+"");
      _contributingNodeNames.addAll(constCrit.getCompositeNamesOfContributingNodes());
      //          _hostColocatedNodes.addAll(constCrit.getCompositeHostColocatedNodes());
      //
      // _partitionCoLocationGroupIds.addAll(constCrit.getCompositePartitionCoLocationGroupIds());
      _partitionExLocationGroupIds.addAll(constCrit.getCompositePartitionExLocationGroupIds());
      _partitionIsolations.addAll(constCrit.getCompositePartitionIsolations());
      _xhpIndexes.addAll(constCrit.getCompositeExclusiveHostPoolIndexes());
      _restartable.addAll(constCrit.getCompositeRestartable());
      _relocatable.addAll(constCrit.getCompositeRelocatable());
      for (String provKey : constCrit.getCompositeProvisionalLocations().keySet()) {
        if (!_provLocations.containsKey(provKey)) {
          _provLocations.put(provKey, new HashSet<Set<String>>());
        }
        Set<Set<String>> provSet = _provLocations.get(provKey);
        provSet.addAll(constCrit.getCompositeProvisionalLocations().get(provKey));
      }
      for (String provKey : constCrit.getCompositeSoftProvisionalLocations().keySet()) {
        if (!_softProvLocations.containsKey(provKey)) {
          _softProvLocations.put(provKey, new HashSet<Set<String>>());
        }
        Set<Set<String>> provSet = _softProvLocations.get(provKey);
        provSet.addAll(constCrit.getCompositeSoftProvisionalLocations().get(provKey));
      }
      //          Trace.logDebug("xyz: provLocations after add ="+_provLocations);

      for (String segKey : constCrit.getCompositeSegregatedLocations().keySet()) {
        if (!_segLocations.containsKey(segKey)) {
          Multiset<Set<String>> newSet = HashMultiset.create();
          _segLocations.put(segKey, newSet);
        }
        Multiset<Set<String>> segSet = _segLocations.get(segKey);
        segSet.addAll(constCrit.getCompositeSegregatedLocations().get(segKey));
        //              Bwf.p("segr: "+segKey+"="+segSet);
      }
      for (String segKey : constCrit.getCompositeSoftSegregatedLocations().keySet()) {
        if (!_softSegLocations.containsKey(segKey)) {
          Multiset<Set<String>> newSet = HashMultiset.create();
          _softSegLocations.put(segKey, newSet);
        }
        Multiset<Set<String>> segSet = _softSegLocations.get(segKey);
        segSet.addAll(constCrit.getCompositeSoftSegregatedLocations().get(segKey));
        //              Bwf.p("segr: "+segKey+"="+segSet);
      }
    }
    return (criteriaList);
  }

  public static boolean _internalDebugMessages = false;

  private FusableUnit _fu;
  private ComposedContainer _container;
  private IEUComposer _composer;
  private List<String> _partitionExLocationGroupIds; //
  private List<Boolean> _partitionIsolations; //
  private Map<String, Set<Set<String>>> _provLocations; //
  private Map<String, Multiset<Set<String>>> _segLocations; //
  private Map<String, Set<Set<String>>> _softProvLocations; //
  private Map<String, Multiset<Set<String>>> _softSegLocations; //
  private Multiset<BigInteger> _xhpIndexes; //
  private Set<Boolean> _restartable; //
  private Set<Boolean> _relocatable; //
  private Set<String> _contributingNodeNames; //
  private Set<ComposerCriteriaNode> _hostColocatedNodes; //
  private Set<String> _usedPartitionExlocGrpId; //
  private Set<String> _reportedMsgs; //
  //    private List<MessageElementType>             _problemConstraintsWithHostPlacements = new
  // ArrayList<>(); //
  private Set<HostSpec> _hostSpecs; //
  private Set<HostSpec> _resolvedHostSpecs; //
  private Set<PoolLocation> _poolLocations; //
  private Map<String, Set<String>> _hostExLocationGroups; //
  private Map<BigInteger, Set<BigInteger>> _poolIndexMap; //
  //    private Set<String>                          _usedHostExlocGrpId = new HashSet<>(); //

  //  private static Set<BigInteger>                      _hostpools = new HashSet<>();
  //  private static Set<BigInteger>                      _indexesInPool = new HashSet<>();
  //  private static Set<String>                          _hostLocations = new HashSet<>();
  //  private static Set<ComposerCriteriaNode>            _ccNodes = new HashSet<>();
  //  private static Set<String>                          _partitionCoLocationGroupIds = new
  // HashSet<>();
}
