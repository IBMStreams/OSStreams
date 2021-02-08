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

import static com.ibm.streams.instance.sam.model.ValidatorHelper.validateIfPresent;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.OperInstanceInputPortType;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import org.apache.commons.lang.builder.EqualsBuilder;

public class TopologyNodeInputPort extends TopologyNodePort
    implements ConnectionDestination, Validator, IXmlSerializer<OperInstanceInputPortType> {

  private ThreadedPort threadedPort;
  private Boolean isControl;
  private final ImportedStreams imports = new ImportedStreams();
  private List<Merger> mergers;

  public TopologyNodeInputPort(String nodeName, TopologyNode owningOp) {
    super(nodeName, owningOp);
  }

  public ThreadedPort getThreadedPort() {
    return threadedPort;
  }

  public void setThreadedPort(ThreadedPort threadedPort) {
    this.threadedPort = threadedPort;
  }

  public ImportedStreams getImports() {
    return imports;
  }

  public List<Merger> getMergers() {
    return mergers;
  }

  public void setMergers(List<Merger> mergers) {
    this.mergers = mergers;
  }

  @Override
  public void transferOwnership(BigInteger appId, BigInteger nodeIndex) {
    Set<TopologyNodeConnectionId> mutated = new HashSet<>();
    for (TopologyNodeConnectionId id : operConnectionIds) {
      id.setDstNodeIndex(nodeIndex);
      mutated.add(id);
    }
    operConnectionIds = mutated;

    if (mergers != null) {
      for (Merger merger : mergers) {
        merger.transferOwnership(appId, nodeIndex);
      }
    }

    super.transferOwnership(appId, nodeIndex);
  }

  @Override
  public void build() {
    tracker.registerTopologyNodeInputPort(appId, nodeIndex, this);
  }

  @Override
  protected void unbuild() {
    tracker.unregisterTopologyNodeInputPort(appId, nodeIndex, index);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<inputPort name=\""
            + portName
            + "\" index=\""
            + index
            + "\" portName=\""
            + portName
            + "\" tupleTypeIndex=\""
            + tupleTypeIndex
            + "\" encoding=\""
            + encoding
            + "\" transport=\""
            + transport
            + "\" mutable=\""
            + mutable.toString()
            + "\" isControl=\""
            + isControl
            + "\">");
    dumper.indent();
    if (threadedPort != null) {
      dumper.line(threadedPort.toString());
    }
    if (mergers != null) {
      dumper.line("<mergers>");
      dumper.indent();
      dumper.append(mergers);
      dumper.outdent();
      dumper.line("</mergers>");
    }
    dumper.append(super.toString());
    dumper.append(imports.toString());
    dumper.outdent();
    dumper.line("</inputPort>");
    return dumper.toString();
  }

  public OperInstanceInputPortType toXml() {
    OperInstanceInputPortType xml = new OperInstanceInputPortType();

    // Base Class Fields
    xml.setIndex(getIndex());
    xml.setName(getName());
    xml.setIsMutable(isMutable());
    xml.setTupleTypeIndex(getTupleTypeIndex());
    // End Base Class

    // We don't have knowledge of what's inter-pe and intra-pe at
    // this level, so it's excluded entirely and added back in at the PE level
    // for(TopologyNodeConnection tnc : getOperConnections())
    //  xml.getIntraPeConnection().add(tnc.toXml(new
    // com.ibm.streams.platform.services.OperInstanceIncomingConnectionType()));

    if (threadedPort != null) xml.setThreadedPort(threadedPort.toXml());
    xml.setIsControl(isControl);

    return xml;
  }

  public void validate() throws Exception {
    validateIfPresent(threadedPort);
    validateIfPresent(imports);
  }

  @Override
  public TopologyNodeInputPort getInputPort() {
    return this;
  }

  @Override
  public BigInteger getOperatorIndex() {
    return nodeIndex;
  }

  @Override
  public BigInteger getPortIndex() {
    return getIndex();
  }

  @Override
  public BigInteger getApplicationId() {
    return appId;
  }

  public Boolean isControl() {
    return isControl;
  }

  public void setIsControl(Boolean isControl) {
    this.isControl = isControl;
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
    TopologyNodeInputPort rhs = (TopologyNodeInputPort) obj;
    return new EqualsBuilder()
        .appendSuper(super.equals(obj))
        .append(getThreadedPort(), rhs.getThreadedPort())
        .append(getMergers(), rhs.getMergers())
        .isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), threadedPort, mergers, TopologyNodeInputPort.class);
  }
}
