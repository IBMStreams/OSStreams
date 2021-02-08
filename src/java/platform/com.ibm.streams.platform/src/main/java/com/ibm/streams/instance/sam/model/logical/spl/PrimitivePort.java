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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.Encoding;
import com.ibm.streams.instance.sam.model.topology.EventTimeContext;
import com.ibm.streams.instance.sam.model.topology.TopologyNodePort;
import com.ibm.streams.instance.sam.model.topology.Transport;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class PrimitivePort extends PrimitivePortBase {
  private String name;
  private Transport transport = Transport.TCP;
  private Encoding encoding = Encoding.LE;
  private boolean mutable = false;
  private BigInteger tupleTypeIndex = BigInteger.ZERO;
  private List<Connection> connections = new ArrayList<>();
  private final Operator owningPrimitiveOp;
  private EventTimeContext eventTimeContext = null;

  public PrimitivePort(BigInteger index, Operator owningPrimitiveOp, SPLLogicalModel model) {
    super(index);
    this.owningPrimitiveOp = owningPrimitiveOp;
  }

  public PrimitivePort(
      BigInteger index,
      Operator owningPrimitiveOp,
      String portName,
      Encoding encoding,
      Transport transport,
      BigInteger tupleTypeIndex,
      Boolean isMutable,
      SPLLogicalModel model) {
    super(index);
    this.owningPrimitiveOp = owningPrimitiveOp;
    this.name = portName;
    this.encoding = encoding;
    this.transport = transport;
    this.tupleTypeIndex = tupleTypeIndex;
    this.mutable = isMutable;
  }

  public PrimitivePort(PrimitivePort other, Operator owningPrimitiveOp) {
    super(other);
    this.name = other.name;
    this.transport = other.transport;
    this.encoding = other.encoding;
    this.mutable = other.mutable;
    this.tupleTypeIndex = other.tupleTypeIndex;
    this.owningPrimitiveOp = owningPrimitiveOp;
    for (Connection modelConnection : other.connections) {
      Connection newConn = new Connection(modelConnection);
      this.connections.add(newConn);
    }
    this.eventTimeContext = other.eventTimeContext;
  }

  public void setName(String name) {
    this.name = name;
  }

  public void setTransport(Transport transport) {
    this.transport = transport;
  }

  public void setEncoding(Encoding encoding) {
    this.encoding = encoding;
  }

  public void setMutable(boolean mutable) {
    this.mutable = mutable;
  }

  public void setTupleTypeIndex(BigInteger tupleTypeIndex) {
    this.tupleTypeIndex = tupleTypeIndex;
  }

  public Operator getOwningOperator() {
    return owningPrimitiveOp;
  }

  public List<Connection> getConnections() {
    return connections;
  }

  public void addConnection(Connection conn) {
    connections.add(conn);
  }

  public void fixConnection(
      BigInteger oldToOperIndex,
      BigInteger oldToPortIndex,
      BigInteger newToOperIndex,
      BigInteger newToPortIndex,
      PortKind newPortKind)
      throws SAMInternalErrorException {
    for (Connection conn : connections) {
      if (conn.getOperIndex().equals(oldToOperIndex)
          && conn.getPortIndex().equals(oldToPortIndex)) {
        conn.setOperIndex(newToOperIndex);
        conn.setPortIndex(newToPortIndex);
        conn.setPortKind(newPortKind);
        return;
      }
    } // end for
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  public void getSplitters(List<SplitterOperator> splitterOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : connections) {
      conn.getSplitters(splitterOps, model);
    }
  }

  public void getMergers(List<MergerOperator> mergerOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : connections) {
      conn.getMergers(mergerOps, model);
    }
  }

  public boolean fedByMerger(SPLLogicalModel model) throws SAMInternalErrorException {
    for (Connection conn : connections) {
      if (conn.fedByMerger(model)) {
        return true;
      }
    }
    return false;
  }

  public void computeCCReachabilityGraph(
      CCRegion ccRegion, Set<PrimitiveOperatorBase> operatorsSeen, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : connections) {
      conn.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
  }

  public String getName() {
    return name;
  }

  public Transport getTransport() {
    return transport;
  }

  public Encoding getEncoding() {
    return encoding;
  }

  public boolean isMutable() {
    return mutable;
  }

  @Override
  public BigInteger getTupleTypeIndex(SPLLogicalModel model) throws SAMInternalErrorException {
    return tupleTypeIndex;
  }

  public void setEventTimeContext(EventTimeContext context) {
    eventTimeContext = context;
  }

  public EventTimeContext getEventTimeContext() {
    return eventTimeContext;
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<port name=\"" + name + "\" index=\"" + getIndex() + "\">");
    if (connections.size() > 0) {
      dumper.indent();
      dumper.line("<connections>");
      dumper.indent();
      dumper.append(connections);
      dumper.outdent();
      dumper.line("</connections>");
      dumper.outdent();
    }
    if (eventTimeContext != null) {
      dumper.append(eventTimeContext.toString());
    }
    dumper.line("</port>");
    return dumper.toString();
  }

  @Override
  public void populatePort(TopologyNodePort port) {
    super.populatePort(port);
    port.setName(name);
    port.setEncoding(encoding);
    port.setTransport(transport);
    port.setTupleTypeIndex(tupleTypeIndex);
    port.setMutable(isMutable());
    if (eventTimeContext != null) {
      port.setEventTimeContext(eventTimeContext);
    }
  }

  public void fixConnections(ParallelTransformState state) {
    for (Connection conn : connections) {
      conn.fixOperatorIndex(state);
    }
  }
}
