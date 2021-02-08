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

import com.ibm.streams.instance.sam.TopologyTracker;
import java.math.BigInteger;
import java.util.Map;
import org.apache.commons.lang.builder.CompareToBuilder;

public class TopologyNodeConnectionId implements Comparable<TopologyNodeConnectionId> {

  private BigInteger srcNodeIndex;
  private BigInteger dstNodeIndex;
  private BigInteger srcPortIndex;
  private BigInteger dstPortIndex;
  private BigInteger appId;

  public TopologyNodeConnectionId(
      BigInteger appId, ConnectionSource src, ConnectionDestination dst) {
    srcNodeIndex = src.getOperatorIndex();
    dstNodeIndex = dst.getOperatorIndex();
    srcPortIndex = src.getPortIndex();
    dstPortIndex = dst.getPortIndex();
    this.appId = appId;
  }

  @Override
  public int compareTo(TopologyNodeConnectionId rhs) {
    return new CompareToBuilder()
        .append(getSrcNodeIndex(), rhs.getSrcNodeIndex())
        .append(getSrcPortIndex(), rhs.getSrcPortIndex())
        .append(getDstNodeIndex(), rhs.getDstNodeIndex())
        .append(getDstPortIndex(), rhs.getDstPortIndex())
        .toComparison();
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append(srcNodeIndex).append(",").append(srcPortIndex).append(",");
    builder.append(dstNodeIndex).append(",").append(dstPortIndex);
    return builder.toString();
  }

  public BigInteger getSrcNodeIndex() {
    return srcNodeIndex;
  }

  public BigInteger getDstNodeIndex() {
    return dstNodeIndex;
  }

  public BigInteger getSrcPortIndex() {
    return srcPortIndex;
  }

  public BigInteger getDstPortIndex() {
    return dstPortIndex;
  }

  public BigInteger getApplicationId() {
    return appId;
  }

  public void setSrcNodeIndex(BigInteger nodeIndex) {
    this.srcNodeIndex = nodeIndex;
  }

  public void setDstNodeIndex(BigInteger nodeIndex) {
    this.dstNodeIndex = nodeIndex;
  }

  public void setAppId(BigInteger appId) {
    this.appId = appId;
  }

  public void fixConnection(Map<BigInteger, BigInteger> nodeIndexes) {
    if (nodeIndexes.containsKey(srcNodeIndex)) {
      srcNodeIndex = nodeIndexes.get(srcNodeIndex);
    }
    if (nodeIndexes.containsKey(dstNodeIndex)) {
      dstNodeIndex = nodeIndexes.get(dstNodeIndex);
    }
  }

  public boolean equalsWithNodeIndexFix(
      TopologyNodeConnectionId other, Map<BigInteger, BigInteger> nodeIndexes) {
    BigInteger fixedSrcNodeIndex = srcNodeIndex;
    BigInteger fixedDstNodeIndex = dstNodeIndex;

    if (nodeIndexes.containsKey(srcNodeIndex)) {
      fixedSrcNodeIndex = nodeIndexes.get(srcNodeIndex);
    }
    if (nodeIndexes.containsKey(dstNodeIndex)) {
      fixedDstNodeIndex = nodeIndexes.get(dstNodeIndex);
    }

    return fixedSrcNodeIndex.equals(other.srcNodeIndex)
        && srcPortIndex.equals(other.srcPortIndex)
        && fixedDstNodeIndex.equals(other.dstNodeIndex)
        && dstPortIndex.equals(other.dstPortIndex);
  }

  public ConnectionSource getSource(TopologyTracker tracker) {
    return tracker.getTopologyNodeOutputPort(appId, srcNodeIndex, srcPortIndex);
  }

  public ConnectionDestination getDestination(TopologyTracker tracker) {
    return tracker.getTopologyNodeInputPort(appId, dstNodeIndex, dstPortIndex);
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((dstNodeIndex == null) ? 0 : dstNodeIndex.hashCode());
    result = prime * result + ((dstPortIndex == null) ? 0 : dstPortIndex.hashCode());
    result = prime * result + ((srcNodeIndex == null) ? 0 : srcNodeIndex.hashCode());
    result = prime * result + ((srcPortIndex == null) ? 0 : srcPortIndex.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    TopologyNodeConnectionId other = (TopologyNodeConnectionId) obj;
    if (dstNodeIndex == null) {
      if (other.dstNodeIndex != null) return false;
    } else if (!dstNodeIndex.equals(other.dstNodeIndex)) return false;
    if (dstPortIndex == null) {
      if (other.dstPortIndex != null) return false;
    } else if (!dstPortIndex.equals(other.dstPortIndex)) return false;
    if (srcNodeIndex == null) {
      if (other.srcNodeIndex != null) return false;
    } else if (!srcNodeIndex.equals(other.srcNodeIndex)) return false;
    if (srcPortIndex == null) {
      if (other.srcPortIndex != null) return false;
    } else if (!srcPortIndex.equals(other.srcPortIndex)) return false;
    return true;
  }
}
