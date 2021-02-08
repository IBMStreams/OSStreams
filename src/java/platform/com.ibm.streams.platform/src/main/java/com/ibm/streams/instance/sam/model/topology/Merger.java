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

import static com.google.common.base.Preconditions.checkNotNull;

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import java.math.BigInteger;

/**
 * Represent the merge-point for a parallel region. Note that there is no representation of the
 * merger in the generated XML that becomes the AADL, which is shipped to the running PEs. Mergers
 * exists in the topology model so that we can reason about where parallel regions end, but they
 * don't actually do anything. At the moment, they are purely for demarcation, not for performing
 * any action at the exit of a parallel region.
 */
public class Merger implements Validator {

  private final BigInteger index;
  private final BigInteger parallelRegionIndex;
  private final BigInteger portIndex;
  private BigInteger nodeIndex;
  private BigInteger appId;
  private transient TopologyNodeInputPort owningInputPort;

  public Merger(
      BigInteger index, TopologyNodeInputPort owningInputPort, BigInteger parallelRegionIndex) {
    this.index = index;
    this.owningInputPort = owningInputPort;
    this.parallelRegionIndex = parallelRegionIndex;
    this.nodeIndex = owningInputPort.getNodeIndex();
    this.portIndex = owningInputPort.getIndex();
    this.appId = owningInputPort.getApplicationId();
  }

  private int hash(Object obj) {
    if (obj != null) {
      return obj.hashCode();
    }
    return 0;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + hash(index);
    result = prime * result + hash(parallelRegionIndex);
    result = prime * result + hash(nodeIndex);
    result = prime * result + hash(portIndex);
    result = prime * result + hash(appId);
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;

    Merger other = (Merger) obj;

    if (appId == null) {
      if (other.appId != null) return false;
    } else if (!appId.equals(other.appId)) return false;

    if (index == null) {
      if (other.index != null) return false;
    } else if (!index.equals(other.index)) return false;

    if (nodeIndex == null) {
      if (other.nodeIndex != null) return false;
    } else if (!nodeIndex.equals(other.nodeIndex)) return false;

    if (parallelRegionIndex == null) {
      if (other.parallelRegionIndex != null) return false;
    } else if (!parallelRegionIndex.equals(other.parallelRegionIndex)) return false;

    if (portIndex == null) {
      if (other.portIndex != null) return false;
    } else if (!portIndex.equals(other.portIndex)) return false;

    return true;
  }

  // for recovery
  public void populateTransientFields() {
    this.owningInputPort =
        SAMStaticContext.getTopologyTracker().getTopologyNodeInputPort(appId, nodeIndex, portIndex);
  }

  public TopologyNodeInputPort getOwningInputPort() {
    return owningInputPort;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<merger index=\"" + index + "\" parallelRegionIndex=\"" + parallelRegionIndex + "\"/>");
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNull(index);
    checkNotNull(parallelRegionIndex);
  }

  public BigInteger getIndex() {
    return index;
  }

  public BigInteger getParallelRegionIndex() {
    return parallelRegionIndex;
  }

  public void transferOwnership(BigInteger appId, BigInteger nodeIndex) {
    this.appId = appId;
    this.nodeIndex = nodeIndex;
  }

  public BigInteger getApplicationId() {
    return appId;
  }
}
