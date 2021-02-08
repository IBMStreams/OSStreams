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
import com.ibm.streams.platform.services.OperInstanceIncomingConnectionType;
import com.ibm.streams.platform.services.OperInstanceOutgoingConnectionType;
import java.math.BigInteger;
import java.util.Objects;

public class TopologyNodeConnection extends TopologyObject implements Validator {

  TopologyNodeConnectionId id;
  private BigInteger splitterIndex;
  private final BigInteger channelIndex;

  public TopologyNodeConnection(TopologyNodeOutputPort srcOPort, TopologyNodeInputPort dstIPort) {
    this(srcOPort, dstIPort, null, null);
  }

  public TopologyNodeConnection(
      TopologyNodeOutputPort srcOPort,
      TopologyNodeInputPort dstIPort,
      BigInteger splitterIndex,
      BigInteger channelIndex) {
    this.tracker = srcOPort.getTopologyTracker();
    this.splitterIndex = splitterIndex;
    this.channelIndex = channelIndex;
    id =
        new TopologyNodeConnectionId(
            srcOPort.getOutputPort().getApplicationId(), srcOPort, dstIPort);
    build();
  }

  @Override
  public void build() {
    tracker.registerTopologyNodeConnection(id.getApplicationId(), this);
  }

  public void unbuild() {
    tracker.unregisterTopologyNodeConnection(id.getApplicationId(), id);
  }

  // for recovery
  public void populateTransientFields() {
    this.tracker = SAMStaticContext.getTopologyTracker();
  }

  public BigInteger getSplitterIndex() {
    return splitterIndex;
  }

  public void setSplitterIndex(BigInteger splitterIndex) {
    this.splitterIndex = splitterIndex;
  }

  public BigInteger getChannelIndex() {
    return channelIndex;
  }

  public ConnectionSource getSource() {
    return id.getSource(tracker);
  }

  public ConnectionDestination getDestination() {
    return id.getDestination(tracker);
  }

  public TopologyNodeConnectionId getId() {
    return id;
  }

  @Override
  public int hashCode() {
    return Objects.hash(id, splitterIndex, channelIndex);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj != null && (obj instanceof TopologyNodeConnection)) {
      TopologyNodeConnection conn = (TopologyNodeConnection) obj;
      if (id.equals(conn.getId())) {
        if ((splitterIndex == null && conn.getSplitterIndex() == null)
            || (splitterIndex != null && splitterIndex.equals(conn.getSplitterIndex()))) {
          if ((channelIndex == null && conn.getChannelIndex() == null)
              || (channelIndex != null && channelIndex.equals(conn.getChannelIndex()))) {
            return true;
          }
        }
      }
    }
    return false;
  }

  @Override
  public String toString() {
    BigInteger oPortIndex = id.getSrcPortIndex();
    BigInteger srcOperIndex = id.getSrcNodeIndex();
    BigInteger iPortIndex = id.getDstPortIndex();
    BigInteger dstOperIndex = id.getDstNodeIndex();
    ModelDumper dumper = new ModelDumper();
    dumper.startLine(
        "<connection srcOperIndex=\""
            + srcOperIndex
            + "\" srcOPortIndex=\""
            + oPortIndex
            + "\" dstOperIndex=\""
            + dstOperIndex
            + "\" dstIPortIndex=\""
            + iPortIndex
            + "\"");
    if (splitterIndex != null) {
      dumper.append(
          " splitterIndex=\"" + splitterIndex + "\" channelIndex=\"" + channelIndex + "\"");
    }
    dumper.append("/>");
    dumper.end();
    return dumper.toString();
  }

  public OperInstanceIncomingConnectionType toXml(OperInstanceIncomingConnectionType xml) {
    xml.setOperInstanceIndex(id.getSrcNodeIndex());
    xml.setOportIndex(id.getSrcPortIndex());
    if (splitterIndex != null) {
      xml.setSplitterIndex(splitterIndex);
      xml.setChannelIndex(channelIndex);
    }
    // xml.setPePortIndex(BigInteger.ZERO); // To be fixed by the container
    return xml;
  }

  public OperInstanceOutgoingConnectionType toXml(OperInstanceOutgoingConnectionType xml) {
    xml.setOperInstanceIndex(id.getDstNodeIndex());
    xml.setIportIndex(id.getDstPortIndex());
    // xml.setPePortIndex(BigInteger.ZERO); // To be fixed by the container
    return xml;
  }

  public void validate() throws Exception {
    // Avoid validating because of recursion concerns
    checkNotNull(id);
  }
}
