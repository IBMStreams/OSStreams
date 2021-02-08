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
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnectionId;

public class FusedContainerConnection {

  private FusedContainerConnectionId connId;
  private TopologyNodeConnectionId tncId;

  public FusedContainerConnection(
      TopologyNodeConnection tnc, FusedContainerOutputPort output, FusedContainerInputPort input) {
    tncId = tnc.getId();
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("Connection Output: " + output.toString());
      Trace.logTrace("Connection Input: " + input.toString());
    }
    connId =
        new FusedContainerConnectionId(
            output.getFusedApplicationId(),
            output.getContainerIndex(),
            output.getIndex(),
            input.getFusedApplicationId(),
            input.getContainerIndex(),
            input.getIndex());

    if (SAMStaticContext.getFusedTracker()
            .getFusedContainerConnection(output.getFusedApplicationId(), connId)
        == null) {
      SAMStaticContext.getFusedTracker()
          .registerFusedContainerConnection(output.getFusedApplicationId(), this);
      output.addContainerConnection(connId);
      input.addContainerConnection(connId);
    }
  }

  public FusedContainerConnectionId getId() {
    return connId;
  }

  public TopologyNodeConnectionId getTopologyNodeConnectionId() {
    return tncId;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((connId == null) ? 0 : connId.hashCode());
    result = prime * result + ((tncId == null) ? 0 : tncId.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    FusedContainerConnection other = (FusedContainerConnection) obj;
    if (connId == null) {
      if (other.connId != null) return false;
    } else if (!connId.equals(other.connId)) return false;
    if (tncId == null) {
      if (other.tncId != null) return false;
    } else if (!tncId.equals(other.tncId)) return false;
    return true;
  }
}
