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

package com.ibm.streams.instance.sam.model.composer;

import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.sch.composer.ComposedContainer;
import java.util.Collection;
import java.util.SortedSet;
import java.util.TreeSet;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

public class ContainerSpecification implements Comparable<ContainerSpecification> {

  //  protected void init() {
  //	  _nodes = new TreeSet<>();
  //	  _id = _nextId++;
  //  }

  public String getId() {
    return _id;
  }

  public SortedSet<TopologyNode> getNodes() {
    return _nodes;
  }

  public void addNodes(Collection<TopologyNode> nodes) {
    _nodes.addAll(nodes);
  }

  public String getAbstract() {
    StringBuilder str = new StringBuilder();
    str.append("ContSpec(" + _id + ")[");
    for (TopologyNode node : _nodes) {
      str.append(" " + node.getName() + "(" + node.getIndex() + ")" + ";");
    }
    str.append("]\n");
    return str.toString();
  }

  public String toString() {
    StringBuilder str = new StringBuilder();
    str.append(getAbstract());
    /*
    for (TopologyNode node : _nodes) {
     str.append("******* node *******\n");
     str.append(node.toString());
     str.append("====================\n");
    }
    */
    return str.toString();
  }

  //  public ContainerSpecification() {
  //	  init();
  //  }
  //
  //  public ContainerSpecification(TopologyNode node) {
  //	  init();
  //	  _nodes.add(node);
  //  }
  //
  //  public ContainerSpecification(List<TopologyNode> nodes) {
  //	  init();
  //	  _nodes.addAll(nodes);
  //  }
  //
  //  public ContainerSpecification(FusableUnit fusUnit) {
  //	  init();
  //	  addNodes(fusUnit.getTopologyNodes());
  //  }

  public ContainerSpecification(ComposedContainer container) {
    _nodes = new TreeSet<>();
    _id = container.getId();
    addNodes(container.getTopologyNodes());
  }

  // for recovery
  public ContainerSpecification(String id, SortedSet<TopologyNode> nodes) {
    _nodes = new TreeSet<>();
    _id = id;
    addNodes(nodes);
  }

  public ContainerSpecification copy() {
    return new ContainerSpecification(getId(), new TreeSet<>(getNodes()));
  }

  @Override
  public int compareTo(ContainerSpecification cs) {
    return getNodes().first().compareTo(cs.getNodes().first());
  }

  @Override
  public int hashCode() {
    return new HashCodeBuilder()
        .append(_nodes)
        //      .append(_id)
        .toHashCode();
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
    ContainerSpecification rhs = (ContainerSpecification) obj;
    return new EqualsBuilder()
        .append(_nodes, rhs._nodes)
        .
        //        append(_id, rhs._id).
        isEquals();
  }

  private SortedSet<TopologyNode> _nodes;
  private String _id;
}
