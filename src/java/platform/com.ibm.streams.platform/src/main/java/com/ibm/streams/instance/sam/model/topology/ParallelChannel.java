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

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.platform.services.ParallelChannelType;
import java.math.BigInteger;
import java.util.List;

public final class ParallelChannel implements Validator, ConnectionSource {

  private BigInteger index;
  private transient Splitter owningSplitter;
  private List<Splitter> splitters;

  public List<Splitter> getSplitters() {
    return splitters;
  }

  public void setSplitters(List<Splitter> splitters) {
    this.splitters = splitters;
  }

  public ParallelChannel(BigInteger index, Splitter owningSplitter) {
    this.index = index;
    this.owningSplitter = owningSplitter;
  }

  // for recovery
  public void populateTransientFields(Splitter owningSplitter) {
    this.owningSplitter = owningSplitter;
    if (splitters != null) {
      for (Splitter splitter : splitters) {
        splitter.populateTransientFields();
      }
    }
  }

  public BigInteger getIndex() {
    return index;
  }

  public void setIndex(BigInteger index) {
    this.index = index;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((index == null) ? 0 : index.hashCode());
    result = prime * result + ((owningSplitter == null) ? 0 : owningSplitter.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    ParallelChannel other = (ParallelChannel) obj;
    if (index == null) {
      if (other.index != null) return false;
    } else if (!index.equals(other.index)) return false;
    if (owningSplitter == null) {
      if (other.owningSplitter != null) return false;
    } else if (!owningSplitter.equals(other.owningSplitter)) return false;
    return true;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<parallelChannel index=\"" + index + "\"");
    if (splitters != null) {
      dumper.append(">").end();
      dumper.indent();
      dumper.append(splitters);
      dumper.outdent();
      dumper.line("</parallelChannel>");
    } else {
      dumper.append("/>").end();
    }
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNull(index);
    //        checkArgument(operConnections.size() > 0, "At least one connection required");
    //        checkContentsNotNullAndValidate(operConnections);
  }

  @Override
  public boolean isOutputPort() {
    return false;
  }

  @Override
  public boolean isParallelChannel() {
    return true;
  }

  @Override
  public TopologyNode getOwningOperator() {
    return null;
  }

  @Override
  public TopologyNodeOutputPort getOutputPort() {
    return owningSplitter.getOwningOutputPort();
  }

  @Override
  public BigInteger getPortIndex() {
    return getOutputPort().getIndex();
  }

  @Override
  public BigInteger getOperatorIndex() {
    return owningSplitter.getOwningOutputPort().getOperatorIndex();
  }

  @Override
  public BigInteger getApplicationId() {
    return owningSplitter.getOwningOutputPort().getApplicationId();
  }

  public ParallelChannelType toXml() {
    ParallelChannelType xml = new ParallelChannelType();
    if (splitters != null) {
      for (Splitter splitter : splitters) {
        xml.getSplitter().add(splitter.toXml());
      }
    }
    xml.setIndex(index);
    return xml;
  }
}
