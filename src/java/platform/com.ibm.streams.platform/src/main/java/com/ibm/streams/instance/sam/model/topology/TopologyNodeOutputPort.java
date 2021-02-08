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
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNullAndValidate;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.validateIfPresent;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.PhysOperInstanceOutputPortType;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.TreeMap;
import java.util.stream.Collectors;
import org.apache.commons.lang.builder.EqualsBuilder;

public class TopologyNodeOutputPort extends TopologyNodePort
    implements ConnectionSource, Validator, IXmlSerializer<PhysOperInstanceOutputPortType> {

  private String streamName;
  private String logicalStreamName;
  private Boolean viewable;
  private Boolean singleThreadedOnOutput;
  private Map<BigInteger, Splitter> portSplittersMap;
  private List<Splitter> splitters;
  private ExportedStream export;

  public void setSplitters(List<Splitter> splitters) {
    this.splitters = splitters;
  }

  public TopologyNodeOutputPort(String nodeName, TopologyNode owningOp) {
    super(nodeName, owningOp);
    viewable = Boolean.TRUE;
    singleThreadedOnOutput = Boolean.FALSE;
  }

  public void transferOwnership(
      BigInteger appId, BigInteger nodeIndex, Map<BigInteger, BigInteger> splitterIndexFix) {
    Set<TopologyNodeConnectionId> mutated = new HashSet<>();
    for (TopologyNodeConnectionId id : operConnectionIds) {
      id.setSrcNodeIndex(nodeIndex);
      mutated.add(id);
    }
    operConnectionIds = mutated;

    if (splitters != null) {
      for (Splitter splitter : splitters) {
        splitter.transferOwnership(appId, nodeIndex, splitterIndexFix);
      }

      if (splitterIndexFix != null) {
        // Fix our splitter indexes - we can't change them one-by-one; assume splitterIndexFix
        // is {(x,y), (y,z)} and changed the portSplittersMap x to y, then changed its y to z,
        // we would have lost y. So we must copy out all relevant entries, remove them, then
        // re-add them while fixing up their indexes.
        Map<BigInteger, Splitter> toFix =
            portSplittersMap.entrySet().stream()
                .filter(e -> splitterIndexFix.containsKey(e.getKey()))
                .collect(Collectors.toMap(e -> e.getKey(), e -> e.getValue()));
        portSplittersMap.entrySet().removeAll(toFix.entrySet());
        portSplittersMap.putAll(
            toFix.entrySet().stream()
                .collect(
                    Collectors.toMap(e -> splitterIndexFix.get(e.getKey()), e -> e.getValue())));
      }
    }

    super.transferOwnership(appId, nodeIndex);
  }

  public String getLogicalStreamName() {
    return logicalStreamName;
  }

  public void setLogicalStreamName(String logicalStreamName) {
    this.logicalStreamName = logicalStreamName;
  }

  public String getStreamName() {
    return streamName;
  }

  public void setStreamName(String streamName) {
    this.streamName = streamName;
  }

  public Boolean isViewable() {
    return viewable;
  }

  public void setViewable(Boolean viewable) {
    this.viewable = viewable;
  }

  public boolean isSingleThreadedOnOutput() {
    return singleThreadedOnOutput.booleanValue();
  }

  public void setSingleThreadedOnOutput(boolean singleThreadedOnOutput) {
    this.singleThreadedOnOutput = Boolean.valueOf(singleThreadedOnOutput);
  }

  public List<Splitter> getSplitters() {
    return splitters;
  }

  public Map<BigInteger, Splitter> getAllSplitters() {
    Map<BigInteger, Splitter> allSplitters = new TreeMap<>();
    if (splitters != null) {
      allSplitters.putAll(splitters.stream().collect(Collectors.toMap(s -> s.getIndex(), s -> s)));
      for (Splitter splitter : splitters) {
        allSplitters.putAll(splitter.getAllSplitters());
      }
    }
    return allSplitters;
  }

  // This counts all splitters, including nested splitters, so it's not just the size of
  // the splitters list.
  public int numberOfSplitters() {
    int count = 0;
    if (splitters != null) {
      for (Splitter splitter : splitters) {
        count += 1 + splitter.numberOfSplitters();
      }
    }
    return count;
  }

  // Note: this is recursive, and finds the max among all nested splittes.
  public BigInteger maxSplitterIndex() {
    BigInteger maxIndex = BigInteger.valueOf(-1);
    if (splitters != null) {
      for (Splitter splitter : splitters) {
        maxIndex = splitter.maxSplitterIndex().max(maxIndex);
      }
    }
    return maxIndex;
  }

  public void setPortSplittersMap(Map<BigInteger, Splitter> portSplittersMap) {
    this.portSplittersMap = portSplittersMap;
  }

  public Map<BigInteger, Splitter> getPortSplittersMap() {
    return portSplittersMap;
  }

  // for recovery
  @Override
  public void populateTransientFields() {
    super.populateTransientFields();
    if (portSplittersMap != null) {
      for (Splitter splitter : portSplittersMap.values()) {
        splitter.populateTransientFields();
      }
    }
    if (splitters != null) {
      for (Splitter splitter : splitters) {
        splitter.populateTransientFields();
      }
    }
  }

  public ExportedStream getExport() {
    return export;
  }

  public void setExport(ExportedStream export) {
    this.export = export;
  }

  @Override
  public void build() {
    tracker.registerTopologyNodeOutputPort(appId, nodeIndex, this);
  }

  @Override
  protected void unbuild() {
    tracker.unregisterTopologyNodeOutputPort(appId, nodeIndex, index);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<outputPort index=\""
            + index
            + "\" portName=\""
            + portName
            + "\" streamName=\""
            + streamName
            + "\" logicalStreamName=\""
            + logicalStreamName
            + "\" tupleTypeIndex=\""
            + tupleTypeIndex
            + "\" encoding=\""
            + encoding
            + "\" transport=\""
            + transport
            + "\" mutable=\""
            + mutable.toString()
            + "\" viewable=\""
            + viewable.toString()
            + "\">");
    dumper.indent();
    dumper.append(super.toString());
    if (export != null) {
      dumper.append(export.toString());
    }
    if (splitters != null && splitters.size() > 0) {
      dumper.line("<splitters>");
      dumper.indent();
      dumper.append(splitters);
      dumper.outdent();
      dumper.line("</splitters>");
    }
    dumper.outdent();
    dumper.line("</outputPort>");
    return dumper.toString();
  }

  public PhysOperInstanceOutputPortType toXml() {
    PhysOperInstanceOutputPortType xml = new PhysOperInstanceOutputPortType();

    // Base Class Fields
    xml.setIndex(getIndex());
    xml.setName(getName());
    xml.setIsMutable(isMutable());
    xml.setTupleTypeIndex(getTupleTypeIndex());
    xml.setViewable(isViewable());
    // End Base Class

    // We don't have knowledge of what's inter-pe and intra-pe at
    // this level, so it's excluded entirely and added back in at the PE level
    // for(TopologyNodeConnection tnc : getOperConnections())
    //  xml.getInterPeConnection().add(tnc.toXml(new
    // com.ibm.streams.platform.services.OperInstanceOutgoingConnectionType()));

    if (getSplitters() != null && getSplitters().size() > 0)
      for (Splitter entry : getSplitters())
        xml.getSplitter().add(entry.getIndex().intValue(), entry.toXml());

    xml.setStreamName(getStreamName());
    xml.setLogicalStreamName(getLogicalStreamName());

    return xml;
  }

  public void validate() throws Exception {
    super.validate();
    checkNotNull(streamName);
    // viewable is an optional field
    checkNotNull(singleThreadedOnOutput);

    checkContentsNotNull(portSplittersMap.keySet());
    checkContentsNotNullAndValidate(portSplittersMap.values());

    validateIfPresent(export);
  }

  @Override
  public boolean isOutputPort() {
    return true;
  }

  @Override
  public boolean isParallelChannel() {
    return false;
  }

  @Override
  public TopologyNodeOutputPort getOutputPort() {
    return this;
  }

  @Override
  public BigInteger getPortIndex() {
    return getIndex();
  }

  @Override
  public BigInteger getOperatorIndex() {
    return nodeIndex;
  }

  @Override
  public BigInteger getApplicationId() {
    return appId;
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
    TopologyNodeOutputPort rhs = (TopologyNodeOutputPort) obj;
    return new EqualsBuilder()
        .appendSuper(super.equals(obj))
        .append(getPortSplittersMap(), rhs.getPortSplittersMap())
        .append(getSplitters(), rhs.getSplitters())
        .isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        super.hashCode(), portSplittersMap, splitters, TopologyNodeOutputPort.class);
  }
}
