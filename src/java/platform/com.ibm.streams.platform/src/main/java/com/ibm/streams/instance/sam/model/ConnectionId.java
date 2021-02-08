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

package com.ibm.streams.instance.sam.model;

import com.ibm.streams.platform.services.IncomingStaticIPConnectionType;
import com.ibm.streams.platform.services.OutgoingStaticOPConnectionType;
import java.math.BigInteger;

public class ConnectionId {

  private Pe outputPe;
  private Pe inputPe;
  private BigInteger outputPortId;
  private BigInteger inputPortId;

  public ConnectionId() {}

  public ConnectionId(Pe outputPe, BigInteger outputPortId, Pe inputPe, BigInteger inputPortId) {
    this.outputPe = outputPe;
    this.inputPe = inputPe;
    this.outputPortId = outputPortId;
    this.inputPortId = inputPortId;
  }

  public ConnectionId(String connId) {
    throw new RuntimeException("called ConnectionId(String) constructor");
  }

  public BigInteger getOutputPeId() {
    return outputPe.getId();
  }

  public BigInteger getInputPeId() {
    return inputPe.getId();
  }

  public BigInteger getOutputPortId() {
    return outputPortId;
  }

  public BigInteger getInputPortId() {
    return inputPortId;
  }

  public IncomingStaticIPConnectionType getIncomingStaticIPConnectionType() {
    IncomingStaticIPConnectionType xml = new IncomingStaticIPConnectionType();
    xml.setPeId(outputPe.getId());
    xml.setOportId(outputPortId);

    // by construction in the Pe, we've ensured that PE port ID and PE port index are always the
    // same
    xml.setOportIndex(outputPortId);
    return xml;
  }

  public OutgoingStaticOPConnectionType getOutgoingStaticOPConnectionType() {
    // Trace.logWarn("Input PE ID " + inputPeId + " Input Port ID " + inputPortId);
    OutgoingStaticOPConnectionType xml = new OutgoingStaticOPConnectionType();
    xml.setPeId(inputPe.getId());
    xml.setIportId(inputPortId);

    // by construction in the Pe, we've ensured that PE port ID and PE port index are always the
    // same
    xml.setIportIndex(inputPortId);
    xml.setIOperInstanceIndex(inputPe.getInputPortById(inputPortId).getNodeIndex());
    xml.setIOperInstancePortIndex(inputPe.getInputPortById(inputPortId).getNodePortIndex());
    return xml;
  }

  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append(outputPe.getId()).append(",").append(outputPortId).append(",");
    builder.append(inputPe.getId()).append(",").append(inputPortId);
    return builder.toString();
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((inputPe == null) ? 0 : inputPe.getId().hashCode());
    result = prime * result + ((inputPortId == null) ? 0 : inputPortId.hashCode());
    result = prime * result + ((outputPe == null) ? 0 : outputPe.getId().hashCode());
    result = prime * result + ((outputPortId == null) ? 0 : outputPortId.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    ConnectionId other = (ConnectionId) obj;
    if (inputPe == null) {
      if (other.inputPe != null) return false;
    } else if (!inputPe.getId().equals(other.inputPe.getId())) return false;
    if (inputPortId == null) {
      if (other.inputPortId != null) return false;
    } else if (!inputPortId.equals(other.inputPortId)) return false;
    if (outputPe == null) {
      if (other.outputPe != null) return false;
    } else if (!outputPe.getId().equals(other.outputPe.getId())) return false;
    if (outputPortId == null) {
      if (other.outputPortId != null) return false;
    } else if (!outputPortId.equals(other.outputPortId)) return false;
    return true;
  }
}
