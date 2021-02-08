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

package com.ibm.streams.sch;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.sch.composer.ComposerCriteriaNode;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class SCHLocationGroupManager {
  // return stringified state info for debugger purposes.
  public String toString() {
    StringBuilder outStrStrm = new StringBuilder();
    outStrStrm.append("SCHLocationGroupManager" + "\n");
    for (String gId : _partitionExlocGrpMap.keySet()) {
      outStrStrm.append(
          "exlocPartitionGrpId[" + gId + "] " + _partitionExlocGrpMap.get(gId).toString() + "\n");
    }
    for (String gId : _partitionColocGrpMap.keySet()) {
      outStrStrm.append(
          "colocPartitionGrpId[" + gId + "] " + _partitionColocGrpMap.get(gId).toString() + "\n");
    }
    //	    for (String gId : _operatorHostColocGrpMap.keySet()) {
    //	    	outStrStrm.append("opHostColocGrpId["+ gId + "] " +
    // _operatorHostColocGrpMap.get(gId).toString() +
    //	    	"\n");
    //		    }
    return (outStrStrm.toString());
  }

  // add node to associated partition groups
  public void addNodeToAssociatedPartitionLocationGroups(ComposerCriteriaNode ccNode) {
    addNodeToAssociatedPartitionColocGroups(ccNode);
    addNodeToAssociatedPartitionExlocGroups(ccNode);
  }

  // Add PE to Colocation Group(s)
  //    if group(s) doesn't yet exist, create it.
  private void addNodeToAssociatedPartitionColocGroups(ComposerCriteriaNode ccNode) {
    Set<String> cgIdSet = ccNode.getPartitionCoLocationGroupIds();
    if (!cgIdSet.isEmpty()) {
      SCHPartitionColocationGroup cgPtr = getPartitionColocGroup(cgIdSet.iterator().next());
      cgPtr.addNode(ccNode);

      // PE is added only to one colocation group.
      // will never have to merge colocation groups that have common PEs.

      // Don't need to merge unless multiple colocation ids
      // even then, may not be necessary if ids are already merged.
      // but still, we try to merge them anyway.
      if (cgIdSet.size() > 1) {
        mergePartitionColocGroups(cgIdSet);
      }
    }
  }

  // Add Node to Exlocation Group(s)
  //    if group(s) doesn't yet exist, create it.
  private void addNodeToAssociatedPartitionExlocGroups(ComposerCriteriaNode ccNode) {
    if (ccNode.getPartitionExLocationGroupIds() == null) {
      return;
    }
    for (String xgId : ccNode.getPartitionExLocationGroupIds()) {
      SCHPartitionExlocationGroup xgPtr = getPartitionExlocGroup(xgId);
      xgPtr.addNode(ccNode);
    }
  }

  public void addCcNodeToAssociatedOperatorHostLocationGroups(ComposerCriteriaNode ccNode) {
    addCcNodeToAssociatedOperatorHostColocGroups(ccNode);
    //		addPeToAssociatedExlocGroups(ccNode);
  }

  public Set<ComposerCriteriaNode> getCgCoMembers(ComposerCriteriaNode ccNode) {
    Set<ComposerCriteriaNode> ccNodeSet = new HashSet<>();
    ccNodeSet.add(ccNode);
    for (Set<ComposerCriteriaNode> cgOpMembers : _hostColocOpGroups.values()) {
      if (cgOpMembers.contains(ccNode)) {
        ccNodeSet.addAll(cgOpMembers);
      }
    }
    return ccNodeSet;
  }

  public Collection<Set<ComposerCriteriaNode>> getColocOpGroups() {
    return _hostColocOpGroups.values();
  }

  private void addCcNodeToAssociatedOperatorHostColocGroups(ComposerCriteriaNode ccNode) {
    for (String cgId : ccNode.getOperatorHostColocationGroupIds()) {
      Set<ComposerCriteriaNode> hostColocOpGroup = _hostColocOpGroups.get(cgId);
      if (hostColocOpGroup == null) {
        hostColocOpGroup = new HashSet<>();
      }
      hostColocOpGroup.add(ccNode);
      _hostColocOpGroups.put(cgId, hostColocOpGroup);
    }
  }

  // Merge groups into one group, but retain individual, original IDs to common group.
  //     if group doesn't exist, create it.
  private void mergePartitionColocGroups(Set<String> groupIdSet) {

    SCHPartitionColocationGroup superGroup = new SCHPartitionColocationGroup("MergedGroup", true);
    Trace.logDebug("Coloc Grps to be merged  " + groupIdSet);
    // merge all groups into a super group
    for (String groupId : groupIdSet) {
      // get old group
      SCHPartitionColocationGroup oldGrp = getPartitionColocGroup(groupId);
      superGroup.merge(oldGrp);
    }
    Trace.logTrace("Pointing old co-location groups to new group");

    // Need to change reference for all original colocation ids that point
    // to super group, not just those in the current groupIdSet
    // Need to maintain this for each super group. (_origColocIds)
    // remove all old groups (these could be merged groups),
    // and change references to point to super group.
    for (String gId : superGroup._origColocIds) {
      _partitionColocGrpMap.put(gId, superGroup);
    }
  }

  // get pointer to group
  //    ... if not found, create new one.
  public SCHPartitionColocationGroup getPartitionColocGroup(String groupId) {

    Trace.logTrace("Trying to get group pointer for Coloc group: " + groupId);
    if (!_partitionColocGrpMap.containsKey(groupId)) {
      // not found, so create one.
      Trace.logDebug("Group for Coloc group: " + groupId + " doesn't exist. Creating");
      SCHPartitionColocationGroup cGrp = new SCHPartitionColocationGroup(groupId, false);
      _partitionColocGrpMap.put(groupId, cGrp);
    }
    return (_partitionColocGrpMap.get(groupId));
  }

  // get pointer to group
  //    ... if not found, create new one.
  public SCHPartitionExlocationGroup getPartitionExlocGroup(String groupId) {
    Trace.logTrace("Trying to get group pointer for Exloc group: " + groupId);
    if (!_partitionExlocGrpMap.containsKey(groupId)) {
      // not found, so create one.
      Trace.logDebug("Group for Exloc group: " + groupId + " doesn't exist. Creating");
      SCHPartitionExlocationGroup xGrp = new SCHPartitionExlocationGroup(groupId);
      _partitionExlocGrpMap.put(groupId, xGrp); // add to map
    }
    return (_partitionExlocGrpMap.get(groupId));
  }

  /// Constructor
  public SCHLocationGroupManager() {
    _partitionColocGrpMap = new HashMap<>();
    _partitionExlocGrpMap = new HashMap<>();
    //		_operatorHostColocGrpMap = new HashMap<>();
    _hostColocOpGroups = new HashMap<>();
  }

  // map of colocation groups indexed by original ids.
  // duplicates in value (merged groups)
  private Map<String, SCHPartitionColocationGroup> _partitionColocGrpMap;

  // map of exlocation groups indexed by original ids.
  private Map<String, SCHPartitionExlocationGroup> _partitionExlocGrpMap;

  // Map<cgId, Set<op>>
  private Map<String, Set<ComposerCriteriaNode>> _hostColocOpGroups;
}
