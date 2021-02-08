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
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNullAndValidate;

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IDotGraph;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.IdentityHashMap;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import org.apache.commons.lang.builder.EqualsBuilder;

public abstract class TopologyNodePort extends TopologyObject
    implements IDotGraph, Validator, Comparable<TopologyNodePort> {

  protected BigInteger index;
  protected BigInteger tupleTypeIndex;
  protected Encoding encoding;
  protected Transport transport;
  protected Set<TopologyNodeConnectionId> operConnectionIds;
  protected String portName;
  protected Boolean mutable;
  protected BigInteger appId;
  protected BigInteger nodeIndex;
  protected EventTimeContext eventTimeContext;

  private transient Set<TopologyNodeConnection> operConnections;
  private transient TopologyNode node;

  public TopologyNodePort(String name, TopologyNode owningOp) {
    tracker = owningOp.getTopologyTracker();
    portName = name;
    appId = owningOp.getApplicationId();
    nodeIndex = owningOp.getIndex();
    operConnectionIds = new HashSet<>();
    node = owningOp;
    // TODO: Is this the default value?
    mutable = false;
    if (index != null) build();
  }

  protected abstract void unbuild();

  public void deepUnbuild() {
    for (TopologyNodeConnection conn : getOperConnections()) {
      // Ordinarily, our connections should never be null. But this may
      // happen when we're calling deepUnbuild on multiple TopologyNodes
      // which may connect to each other, as we do in transferParallelRegionDifference.
      if (conn != null) {
        conn.unbuild();
      }
    }
    unbuild();
  }

  public BigInteger getNodeIndex() {
    return nodeIndex;
  }

  // for recovery
  public void populateTransientFields() {
    this.tracker = SAMStaticContext.getTopologyTracker();
    node = null;
    getOwningOperator();
    operConnections = null;
    getOperConnections();
  }

  public TopologyNode getOwningOperator() {
    if (node != null && node.getIndex() == nodeIndex) {
      return node;
    }

    node = tracker.getTopologyNode(appId, nodeIndex);
    return node;
  }

  public String getName() {
    return portName;
  }

  public void setName(String name) {
    portName = name;
  }

  public BigInteger getIndex() {
    return index;
  }

  public void setIndex(BigInteger index) {
    this.index = index;
    if (index != null) build();
  }

  public void transferOwnership(BigInteger appId, BigInteger nodeIndex) {
    unbuild();
    this.appId = appId;
    this.nodeIndex = nodeIndex;
    build();
  }

  public void fixConnections(
      BigInteger appId,
      Map<BigInteger, BigInteger> nodeIndexFix,
      IdentityHashMap<TopologyNodeConnectionId, Boolean> fixed) {
    // We can't modify the ConnectionIds in place, because modifying them changes their hashCode()
    // result, which makes basic operations like Set.contains() not work. So we have to create a
    // new set, modify a ConnectionId, add it to new mutated set, then replace the old set with the
    // mutated one.
    Set<TopologyNodeConnectionId> mutated = new HashSet<>();
    for (TopologyNodeConnectionId connId : operConnectionIds) {
      if (!fixed.containsKey(connId)) {
        connId.setAppId(appId);
        connId.fixConnection(nodeIndexFix);
        fixed.put(connId, Boolean.TRUE);
      }
      mutated.add(connId);
    }
    operConnectionIds = mutated;
  }

  public BigInteger getTupleTypeIndex() {
    return tupleTypeIndex;
  }

  public void setTupleTypeIndex(BigInteger tupleTypeIndex) {
    this.tupleTypeIndex = tupleTypeIndex;
  }

  public Encoding getEncoding() {
    return encoding;
  }

  public void setEncoding(Encoding encoding) {
    this.encoding = encoding;
  }

  public Transport getTransport() {
    return transport;
  }

  public void setTransport(Transport transport) {
    this.transport = transport;
  }

  public void addOperConnection(TopologyNodeConnection tnc) {
    operConnectionIds.add(tnc.getId());
  }

  public Set<TopologyNodeConnectionId> getOperConnectionIds() {
    return operConnectionIds;
  }

  public Set<TopologyNodeConnection> getOperConnections() {
    if (operConnections != null && operConnections.size() == operConnectionIds.size()) {
      return operConnections;
    }
    operConnections = new HashSet<>();
    for (TopologyNodeConnectionId id : operConnectionIds) {
      operConnections.add(tracker.getTopologyNodeConnection(appId, id));
    }
    return operConnections;
  }

  public String getNodeName() {
    return getOwningOperator().getName();
  }

  public boolean isMutable() {
    return mutable.booleanValue();
  }

  public void setMutable(boolean mutable) {
    this.mutable = Boolean.valueOf(mutable);
  }

  public boolean hasEventTimeContext() {
    return eventTimeContext != null;
  }

  public EventTimeContext getEventTimeContext() {
    return eventTimeContext;
  }

  public void setEventTimeContext(EventTimeContext context) {
    eventTimeContext = context;
  }

  public abstract void build();

  public String toDotGraph() {
    return getIndex().toString();
  }

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
    TopologyNodePort rhs = (TopologyNodePort) obj;
    return new EqualsBuilder()
        .append(appId, rhs.appId)
        .append(getIndex(), rhs.getIndex())
        .append(getNodeIndex(), rhs.getNodeIndex())
        .append(getName(), rhs.getName())
        .append(getOperConnectionIds(), rhs.getOperConnectionIds())
        .append(eventTimeContext, rhs.eventTimeContext)
        .isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(appId, index, nodeIndex, portName, operConnectionIds, eventTimeContext);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<connections>");
    dumper.indent();
    for (TopologyNodeConnection conn : getOperConnections()) {
      dumper.append(conn.toString());
    }
    dumper.outdent();
    dumper.line("</connections>");
    if (eventTimeContext != null) {
      dumper.append(eventTimeContext.toString());
    }
    return dumper.toString();
  }

  @Override
  public int compareTo(TopologyNodePort tnp) {
    if (this.getClass().equals(tnp.getClass())) {
      int i = this.nodeIndex.compareTo(tnp.getNodeIndex());
      if (i != 0) {
        return i;
      }
      return this.index.compareTo(tnp.getIndex());
    } else {
      return this.getClass().toString().compareTo(tnp.getClass().toString());
    }
  }

  public void validate() throws Exception {
    checkNotNull(index);
    checkNotNull(tupleTypeIndex);
    checkNotNull(encoding); // Enums are null or valid
    checkNotNull(transport);
    checkContentsNotNullAndValidate(getOperConnections());
    checkNotNull(portName);
    checkNotNull(node); // Don't validate, recursion
    checkNotNull(mutable);
  }
}
