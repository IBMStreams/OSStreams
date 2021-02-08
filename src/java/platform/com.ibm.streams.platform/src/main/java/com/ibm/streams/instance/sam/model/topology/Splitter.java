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

import static com.google.common.base.Preconditions.checkArgument;
import static com.google.common.base.Preconditions.checkNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNullAndValidate;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsPositionIndex;

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.ParallelChannelType;
import com.ibm.streams.platform.services.PartitioningAttributeType;
import com.ibm.streams.platform.services.PartitioningAttributesType;
import com.ibm.streams.platform.services.SplitterType;
import java.math.BigInteger;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Objects;
import java.util.TreeMap;
import org.apache.commons.lang.builder.EqualsBuilder;

public class Splitter implements Validator, IXmlSerializer<SplitterType> {

  private BigInteger index;
  private final BigInteger iportIndex;
  private transient TopologyNodeOutputPort owningOutputPort;
  private final SplitterKind kind;
  private final Boolean dropWindowPuncts;
  private final BigInteger parallelRegionIndex;
  private final List<String> attributes;
  private final Map<BigInteger, ParallelChannel> parallelChannels;
  private final BigInteger portIndex;
  private BigInteger nodeIndex;
  private BigInteger appId;

  public Splitter(
      BigInteger index,
      BigInteger iPortIndex,
      TopologyNodeOutputPort owningOutputPort,
      SplitterKind splitterKind,
      Boolean dropWindowPuncts,
      BigInteger parallelRegionIndex,
      List<String> attributes,
      Map<BigInteger, ParallelChannel> parallelChannels) {
    this.index = index;
    this.iportIndex = iPortIndex;
    this.owningOutputPort = owningOutputPort;
    this.kind = splitterKind;
    this.dropWindowPuncts = dropWindowPuncts;
    this.parallelRegionIndex = parallelRegionIndex;
    this.attributes = attributes;
    this.parallelChannels = parallelChannels;
    this.nodeIndex = owningOutputPort.getNodeIndex();
    this.portIndex = owningOutputPort.getIndex();
    this.appId = owningOutputPort.getApplicationId();
  }

  public Map<BigInteger, ParallelChannel> getParallelChannels() {
    return parallelChannels;
  }

  // for recovery
  public void populateTransientFields() {
    this.owningOutputPort =
        SAMStaticContext.getTopologyTracker()
            .getTopologyNodeOutputPort(appId, nodeIndex, portIndex);
    ;
    if (parallelChannels != null) {
      for (ParallelChannel parallelChannel : parallelChannels.values()) {
        parallelChannel.populateTransientFields(this);
      }
    }
  }

  public List<String> getAttributes() {
    return attributes;
  }

  public SplitterKind getKind() {
    return kind;
  }

  public BigInteger getIndex() {
    return index;
  }

  public boolean isDropWindowPuncts() {
    return dropWindowPuncts;
  }

  public BigInteger getIportIndex() {
    return iportIndex;
  }

  public BigInteger getParallelRegionIndex() {
    return parallelRegionIndex;
  }

  public void transferOwnership(
      BigInteger appId, BigInteger nodeIndex, Map<BigInteger, BigInteger> splitterIndexFix) {
    if (getParallelChannels() != null) {
      for (ParallelChannel channel : getParallelChannels().values()) {
        if (channel.getSplitters() != null) {
          for (Splitter splitter : channel.getSplitters()) {
            splitter.transferOwnership(appId, nodeIndex, splitterIndexFix);
          }
        }
      }
    }

    if (splitterIndexFix != null && splitterIndexFix.containsKey(this.index)) {
      this.index = splitterIndexFix.get(this.index);
    }
    this.appId = appId;
    this.nodeIndex = nodeIndex;
  }

  public Map<BigInteger, Splitter> getAllSplitters() {
    Map<BigInteger, Splitter> allSplitters = new TreeMap<>();
    if (getParallelChannels() != null) {
      for (ParallelChannel channel : getParallelChannels().values()) {
        if (channel.getSplitters() != null) {
          for (Splitter nestedSplitter : channel.getSplitters()) {
            allSplitters.put(nestedSplitter.getIndex(), nestedSplitter);
            allSplitters.putAll(nestedSplitter.getAllSplitters());
          }
        }
      }
    }
    return allSplitters;
  }

  // Note: we do NOT count ourself.
  public int numberOfSplitters() {
    int count = 0;
    if (getParallelChannels() != null) {
      for (ParallelChannel channel : getParallelChannels().values()) {
        if (channel.getSplitters() != null) {
          for (Splitter nestedSplitter : channel.getSplitters()) {
            count += 1 + nestedSplitter.numberOfSplitters();
          }
        }
      }
    }
    return count;
  }

  public BigInteger maxSplitterIndex() {
    BigInteger maxIndex = this.index;
    if (getParallelChannels() != null) {
      for (ParallelChannel channel : getParallelChannels().values()) {
        if (channel.getSplitters() != null) {
          for (Splitter nestedSplitter : channel.getSplitters()) {
            maxIndex = nestedSplitter.maxSplitterIndex().max(maxIndex);
          }
        }
      }
    }
    return maxIndex;
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
    Splitter rhs = (Splitter) obj;
    return new EqualsBuilder()
        .append(getIndex(), rhs.getIndex())
        .append(getIportIndex(), rhs.getIportIndex())
        .append(getKind(), rhs.getKind())
        .append(isDropWindowPuncts(), rhs.isDropWindowPuncts())
        .append(getParallelRegionIndex(), rhs.getParallelRegionIndex())
        .append(getOwningOutputPort().getNodeIndex(), rhs.getOwningOutputPort().getNodeIndex())
        .append(getOwningOutputPort().getIndex(), rhs.getOwningOutputPort().getIndex())
        .append(
            getOwningOutputPort().getApplicationId(), rhs.getOwningOutputPort().getApplicationId())
        .isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        index,
        iportIndex,
        kind,
        dropWindowPuncts,
        parallelRegionIndex,
        nodeIndex,
        portIndex,
        appId);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<splitter index=\""
            + index
            + "\" iPortIndex=\""
            + iportIndex
            + "\" kind=\""
            + kind
            + "\" dropWindowPuncts=\""
            + dropWindowPuncts
            + "\" parallelRegionIndex=\""
            + parallelRegionIndex
            + "\">");
    dumper.indent();
    dumper.line("<parallelChannels>");
    dumper.indent();
    dumper.append(parallelChannels);
    dumper.outdent();
    dumper.line("</parallelChannels>");
    dumper.outdent();
    if (!attributes.isEmpty()) {
      dumper.indent();
      dumper.line("<attributes>");
      dumper.indent();
      for (String s : attributes) {
        dumper.line("<name=\"" + s + "\"/>");
      }
      dumper.outdent();
      dumper.line("</attributes>");
      dumper.outdent();
    }
    dumper.line("</splitter>");
    return dumper.toString();
  }

  public SplitterType toXml() {
    SplitterType xml = new SplitterType();
    // ParallelChannelType injected at the PE level when we know if it is an Inter or Intra PE
    // Connection
    xml.setKind(com.ibm.streams.platform.services.SplitterKind.fromValue(getKind().toString()));
    xml.setIndex(getIndex());
    xml.setIportIndex(iportIndex);
    xml.setRegionIndex(parallelRegionIndex);
    xml.setDropWindowPuncts(dropWindowPuncts);
    if (!attributes.isEmpty()) {
      PartitioningAttributesType attribs = new PartitioningAttributesType();
      for (String s : attributes) {
        PartitioningAttributeType attr = new PartitioningAttributeType();
        attr.setName(s);
        attribs.getAttribute().add(attr);
      }
      xml.setAttributes(attribs);
    }
    for (Entry<BigInteger, ParallelChannel> entry : parallelChannels.entrySet()) {
      ParallelChannel pc = entry.getValue();
      ParallelChannelType pct = pc.toXml();
      pct.setIndex(pc.getIndex());
      xml.getParallelChannel().add(pct);
    }
    return xml;
  }

  public void validate() throws Exception {
    checkArgument(
        parallelChannels.values().size() > 1, "At least one parallel channel entry is required");
    checkContentsNotNullAndValidate(parallelChannels.values());
    checkContentsPositionIndex(parallelChannels.keySet());

    checkContentsNotNull(attributes);
    checkNotNull(kind);
    checkNotNull(index);
    // Drop Window Puncts is optional
    // checkNotNull(iportIndex);
    // checkNotNull(regionIndex);
  }

  public TopologyNodeOutputPort getOwningOutputPort() {
    return owningOutputPort;
  }

  public BigInteger getApplicationId() {
    return appId;
  }
}
