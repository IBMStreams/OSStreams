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
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import java.math.BigInteger;
import java.util.Objects;
import org.apache.commons.lang.builder.EqualsBuilder;

public class FusedContainerInputPort extends FusedContainerPort {

  private BigInteger tnipAppId;
  private BigInteger tnipNodeIndex;
  private BigInteger tnipPortIndex;

  public FusedContainerInputPort(
      TopologyNodeInputPort tnip, BigInteger index, BigInteger containerIndex, BigInteger appId) {
    super(index, containerIndex, appId);
    tnipAppId = tnip.getApplicationId();
    tnipNodeIndex = tnip.getNodeIndex();
    tnipPortIndex = tnip.getIndex();
    SAMStaticContext.getFusedTracker().registerFusedContainerInputPort(appId, containerIndex, this);
  }

  // for recovery
  FusedContainerInputPort() {
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
    FusedContainerInputPort rhs = (FusedContainerInputPort) obj;
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
        super.hashCode(), tnipAppId, tnipNodeIndex, tnipPortIndex, FusedContainerInputPort.class);
  }

  public BigInteger getTopologyNodeAppId() {
    return tnipAppId;
  }

  public BigInteger getTopologyNodeIndex() {
    return tnipNodeIndex;
  }

  public BigInteger getTopologyNodePortIndex() {
    return tnipPortIndex;
  }

  public TopologyNodeInputPort getTopologyNodeInputPort() {
    TopologyNodeInputPort tnip =
        SAMStaticContext.getTopologyTracker()
            .getTopologyNodeInputPort(tnipAppId, tnipNodeIndex, tnipPortIndex);
    if (tnip == null) {
      Trace.logError(
          "TopologyNodeInputPort missing at appId="
              + tnipAppId
              + ", nodeIndex="
              + tnipNodeIndex
              + ", portIndex="
              + tnipPortIndex);
      // Trace.logError("TopologyApp: " +
      // String.valueOf(SAMStaticContext.getTopologyTracker().getTopologyApplication(tnipAppId)));
    }
    return tnip;
  }
}
