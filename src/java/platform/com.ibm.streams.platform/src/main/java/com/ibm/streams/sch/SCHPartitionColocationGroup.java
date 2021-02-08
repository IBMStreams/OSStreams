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
import java.util.HashSet;
import java.util.Set;

/// This public class contains the methods/elements needed for maintaining each
/// group of colocated PEs.
public class SCHPartitionColocationGroup {

  /// Add PE to colocation Group
  public void addNode(ComposerCriteriaNode newNodeToAdd) {

    Trace.logTrace(
        "Coloc-grp ["
            + getId()
            + "] #"
            + (_colocatedNodes.size() + 1)
            + " Node["
            + newNodeToAdd.getAbstract()
            + "]");
    _colocatedNodes.add(newNodeToAdd);
    _hasNoNodesBeenAdded = false;

    // merge sizeHints
    _sizeHint += newNodeToAdd.getSize();
  }

  /// Merge supplied group with this one
  public void merge(SCHPartitionColocationGroup cGrp) {

    // merge original colocation ids.
    _origColocIds.addAll(cGrp._origColocIds);

    Trace.logTrace("ColocGrp - original colocId after merging " + _origColocIds);

    // merge colocation id
    //		_colocId = _colocId + "." + cGrp._colocId;

    // merge colocated PE sets  (union)
    Trace.logTrace("ColocGrp - colocId after merging " + getId());
    Trace.logTrace(
        "ColocGrp -colocated Nodes from merge source = # . " + cGrp._colocatedNodes.size());

    for (ComposerCriteriaNode node : cGrp._colocatedNodes) {
      _colocatedNodes.add(node);
    }

    // merge sizeHints
    // DR - this assumes that the groups will not have common PEs,
    // which is true for us since a PE is only added to one colocation object.
    // but won't work in general.
    _sizeHint += cGrp._sizeHint;

    if (_hasNoNodesBeenAdded && !cGrp._hasNoNodesBeenAdded) {
      // copy from cGrp, set isVirgin to false
      _hasNoNodesBeenAdded = false;
    } else {
      if (cGrp._hasNoNodesBeenAdded) {
        // do nothing
      } else {
        // merge validated hosts set  (intersection)
        // merge whether pes scheduled
        // true if any true
        _hasNoNodesBeenAdded = false;
      }
    }
  }

  // return stringified state info for debugger purposes.
  public String toString() {
    StringBuilder outStrStrm = new StringBuilder();

    outStrStrm.append("SCHColocationPartitionGroup[" + getId() + "] ");
    outStrStrm.append("ColocatedPartitions(");
    for (ComposerCriteriaNode node : _colocatedNodes) {
      outStrStrm.append(node.getNodeIndex() + " ");
    }
    outStrStrm.append(")");
    outStrStrm.append("Size Hint (" + _sizeHint + ") ");

    return outStrStrm.toString();
  }

  public String getId() {
    return (_origColocIds.toString());
  }

  public SCHPartitionColocationGroup(String id, boolean isMergedGrp) {
    _sizeHint = 0;
    _hasNoNodesBeenAdded = true;
    _colocatedNodes = new HashSet<>();
    _origColocIds = new HashSet<>();
    if (!isMergedGrp) {
      _origColocIds.add(id);
    }
  }

  /// Colocation id, also used to index colocMap
  //  public String                             _colocId;

  /// Array of colocated SCHPe objects
  public Set<ComposerCriteriaNode> _colocatedNodes;

  /// Sum of the sizes of each of the PEs.
  public long _sizeHint;

  /// set of original colocation ids   (colocId will be set to this on merging)
  public Set<String> _origColocIds;

  // indication of whether a PE has been added to this group yet or not.
  protected boolean _hasNoNodesBeenAdded;
};
