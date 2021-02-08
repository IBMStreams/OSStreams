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
/// group of exlocated PEs.
public class SCHPartitionExlocationGroup {

  /// Add PE to colocation Group
  public void addNode(ComposerCriteriaNode newNodeToAdd) {
    Trace.logTrace(
        "Exloc-grp ["
            + _exlocId
            + "] #"
            + (_exlocatedNodes.size() + 1)
            + " PE["
            + newNodeToAdd.getNodeIndex()
            + "]");
    _exlocatedNodes.add(newNodeToAdd);
  }

  // return stringified state info for debugger purposes.
  public String toString() {

    StringBuilder outStrStrm = new StringBuilder();
    outStrStrm.append("SCHExlocationPartitionGroup[" + _exlocId + "] ");
    outStrStrm.append("ExlocatedParitions(");
    for (ComposerCriteriaNode node : _exlocatedNodes) {
      outStrStrm.append(node.getNodeIndex() + " ");
    }
    outStrStrm.append(")");
    return outStrStrm.toString();
  }

  /// Constructor
  public SCHPartitionExlocationGroup() {
    _exlocId = "No_name";
    _exlocatedNodes = new HashSet<>();
  }

  public SCHPartitionExlocationGroup(String id) {
    _exlocId = id;
    _exlocatedNodes = new HashSet<>();
  }

  /// Colocation id, also used to index colocMap
  public String _exlocId;

  /// Array of colocated SCHPe objects
  public Set<ComposerCriteriaNode> _exlocatedNodes;
};
