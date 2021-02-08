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

package com.ibm.streams.instance.sam.model.fuser;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import java.math.BigInteger;
import java.util.Objects;
import org.apache.commons.lang.builder.EqualsBuilder;

public class FusedContainerOutputPort extends FusedContainerPort {

  private BigInteger tnopAppId;
  private BigInteger tnopNodeIndex;
  private BigInteger tnopPortIndex;
  private BigInteger splitterIndex = null;
  private BigInteger channelIndex = null;

  public FusedContainerOutputPort(
      TopologyNodeOutputPort tnop, BigInteger index, BigInteger containerIndex, BigInteger appId) {
    super(index, containerIndex, appId);
    tnopAppId = tnop.getApplicationId();
    tnopNodeIndex = tnop.getNodeIndex();
    tnopPortIndex = tnop.getIndex();
    SAMStaticContext.getFusedTracker()
        .registerFusedContainerOutputPort(appId, containerIndex, this);
  }

  public FusedContainerOutputPort(
      TopologyNodeOutputPort tnop,
      BigInteger index,
      BigInteger containerIndex,
      BigInteger appId,
      BigInteger splitterIndex,
      BigInteger channelIndex) {
    this(tnop, index, containerIndex, appId);
    this.splitterIndex = splitterIndex;
    this.channelIndex = channelIndex;
  }

  // for recovery
  FusedContainerOutputPort() {
    super();
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == null) {
      return false;
    }
    if (obj == this) {
      return true;
    }
    if (obj.getClass() != getClass()) {
      return false;
    }
    FusedContainerOutputPort rhs = (FusedContainerOutputPort) obj;
    return new EqualsBuilder()
        .appendSuper(super.equals(obj))
        .append(getTopologyNodeAppId(), rhs.getTopologyNodeAppId())
        .append(getTopologyNodeIndex(), rhs.getTopologyNodeIndex())
        .append(getTopologyNodePortIndex(), rhs.getTopologyNodePortIndex())
        .isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        super.hashCode(), tnopAppId, tnopNodeIndex, tnopPortIndex, FusedContainerOutputPort.class);
  }

  public BigInteger getTopologyNodeAppId() {
    return tnopAppId;
  }

  public BigInteger getTopologyNodeIndex() {
    return tnopNodeIndex;
  }

  public BigInteger getTopologyNodePortIndex() {
    return tnopPortIndex;
  }

  public BigInteger getSplitterIndex() {
    return splitterIndex;
  }

  public BigInteger getChannelIndex() {
    return channelIndex;
  }

  public TopologyNodeOutputPort getTopologyNodeOutputPort() {
    TopologyNodeOutputPort tnop =
        SAMStaticContext.getTopologyTracker()
            .getTopologyNodeOutputPort(tnopAppId, tnopNodeIndex, tnopPortIndex);
    if (tnop == null) {
      Trace.logError(
          "TopologyNodeOutputPort missing at appId="
              + tnopAppId
              + ", nodeIndex="
              + tnopNodeIndex
              + ", portIndex="
              + tnopPortIndex);
      // Trace.logError("TopologyApp: " +
      // String.valueOf(SAMStaticContext.getTopologyTracker().getTopologyApplication(tnopAppId)));
    }
    return tnop;
  }
}
