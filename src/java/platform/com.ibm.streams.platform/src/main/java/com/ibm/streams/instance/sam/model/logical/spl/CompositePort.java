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
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public abstract class CompositePort extends Port {
  protected transient List<Connection> incomingConnections = new ArrayList<>();
  protected transient List<Connection> outgoingConnections = new ArrayList<>();
  protected transient CompositeOperator owningComposite;

  public List<Connection> getIncomingConnections() {
    return incomingConnections;
  }

  public List<Connection> getOutgoingConnections() {
    return outgoingConnections;
  }

  protected CompositePort(BigInteger index, CompositeOperator owningComposite) {
    super(index);
    this.owningComposite = owningComposite;
  }

  public void addIncomingConnection(Connection con) {
    System.err.println("Adding incoming connection");
    incomingConnections.add(con);
  }

  public void addOutgoingConnection(Connection con) {
    outgoingConnections.add(con);
  }

  public CompositePort(CompositePort other, CompositeOperator owningComposite) {
    super(other);
    this.owningComposite = owningComposite;

    for (Connection conn :
        other.incomingConnections.toArray(new Connection[incomingConnections.size()])) {
      Connection newConn = new Connection(conn);
      incomingConnections.add(newConn);
    }

    for (Connection conn :
        other.outgoingConnections.toArray(new Connection[outgoingConnections.size()])) {
      Connection newConn = new Connection(conn);
      outgoingConnections.add(newConn);
    }
  }

  public CompositeOperator getOwningOperator() {
    return owningComposite;
  }

  public void getOutgoingSplitters(List<SplitterOperator> splitterOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : outgoingConnections) {
      conn.getSplitters(splitterOps, model);
    }
  }

  public void getIncomingMergers(List<MergerOperator> mergerOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : incomingConnections) {
      conn.getMergers(mergerOps, model);
    }
  }

  public boolean fedByMerger(SPLLogicalModel model) throws SAMInternalErrorException {
    for (Connection conn : incomingConnections) {
      if (conn.fedByMerger(model)) {
        return true;
      }
    }
    return false;
  }

  public void computeCCReachabilityGraph(
      CCRegion ccRegion, Set<PrimitiveOperatorBase> operatorsSeen, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection mConn : outgoingConnections) {
      mConn.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<port index=\"" + getIndex() + "\">");
    dumper.indent();
    dumper.line("<incomingConnections>");
    dumper.indent();
    dumper.append(incomingConnections);
    dumper.outdent();
    dumper.line("</incomingConnections>");
    dumper.line("<outgoingConnections>");
    dumper.indent();
    dumper.append(outgoingConnections);
    dumper.outdent();
    dumper.line("</outgoingConnections>");
    dumper.outdent();
    dumper.line("</port>");
    return dumper.toString();
  }

  public void fixOutgoingConnections(ParallelTransformState state) {
    for (Connection conn : outgoingConnections) {
      conn.fixOperatorIndex(state);
    }
  }

  public void fixIncomingConnections(ParallelTransformState state) {
    for (Connection conn : incomingConnections) {
      conn.fixOperatorIndex(state);
    }
  }
}
