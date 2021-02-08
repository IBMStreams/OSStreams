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
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class Connection {

  private BigInteger operIndex = BigInteger.ZERO;
  private BigInteger portIndex = BigInteger.ZERO;
  private PortKind portKind;

  public Connection(Connection other) {
    operIndex = other.operIndex;
    portIndex = other.portIndex;
    portKind = other.portKind;
  }

  public Connection(BigInteger operIndex, BigInteger portIndex, PortKind portKind) {
    this.operIndex = operIndex;
    this.portIndex = portIndex;
    this.portKind = portKind;
  }

  public BigInteger getTupleTypeIndex(SPLLogicalModel model) throws SAMInternalErrorException {
    Operator op = model.getModelOperator(operIndex);
    if (op.isPrimitive()) {
      assert (portKind == PortKind.Output);
      PrimitiveOutputPort oPort = op.asPrimitive().getOutputPorts().get(portIndex.intValue());
      return oPort.getTupleTypeIndex(model);
    }
    if (op.isSplitterOperator()) {
      assert (portKind == PortKind.Output);
      PrimitiveOutputPort oPort =
          op.asSplitterOperator().getOutputPorts().get(portIndex.intValue());
      return oPort.getTupleTypeIndex(model);
    }
    if (op.isCompositeOperator()) {
      if (portKind == PortKind.Input) {
        CompositePort port = op.asCompositeOperator().getInputPorts().get(portIndex.intValue());
        Connection conn = port.getIncomingConnections().get(0);
        return conn.getTupleTypeIndex(model);
      } else {
        CompositePort port = op.asCompositeOperator().getOutputPorts().get(portIndex.intValue());
        Connection conn = port.getIncomingConnections().get(0);
        return conn.getTupleTypeIndex(model);
      }
    }
    if (op.isMergerOperator()) {
      assert (portKind == PortKind.Output);
      PrimitiveOutputPort oPort = op.asMergerOperator().getOutputPorts().get(0);
      return oPort.getTupleTypeIndex(model);
    }
    assert (false);
    return null;
  }

  public String getStreamName(SPLLogicalModel model) throws SAMInternalErrorException {
    Operator op = model.getModelOperator(operIndex);
    if (op.isPrimitive()) {
      assert (portKind == PortKind.Output);
      PrimitiveOutputPort oPort = op.asPrimitive().getOutputPorts().get(portIndex.intValue());
      return oPort.getStreamName();
    } else if (op.isMergerOperator()) {
      assert (portKind == PortKind.Output);
      PrimitiveOutputPort oPort = op.asMergerOperator().getOutputPorts().get(0);
      return oPort.getStreamName();
    } else if (op.isCompositeOperator()) {
      if (portKind == PortKind.Input) {
        // This is kind of non-sensical: an arbitrary number of
        // primitive output ports can
        // feed into a composite input port - composite input ports can
        // even feed into composite
        // input ports. So what does it mean to have a name? Not much.
        // So we're just going to pick
        // the 0th connection, ask its name, and assume that's
        // meaningful.
        CompositePort port = op.asCompositeOperator().getInputPorts().get(portIndex.intValue());
        Connection conn = port.getIncomingConnections().get(0);
        return conn.getStreamName(model);
      } else {
        CompositePort port = op.asCompositeOperator().getOutputPorts().get(portIndex.intValue());
        Connection conn = port.getIncomingConnections().get(0);
        return conn.getStreamName(model);
      }
    }
    assert (false)
        : "Operator is not Primitive, Merger or Composite, but is " + op.getClass().getSimpleName();
    return null;
  }

  public String getLogicalStreamName(SPLLogicalModel model) throws SAMInternalErrorException {
    Operator op = model.getModelOperator(operIndex);
    if (op.isPrimitive()) {
      assert (portKind == PortKind.Output);
      PrimitiveOutputPort oPort = op.asPrimitive().getOutputPorts().get(portIndex.intValue());
      return oPort.getLogicalStreamName();
    }
    if (op.isCompositeOperator()) {
      if (portKind.equals(PortKind.Input)) {
        assert (false);
      } else {
        CompositeOutputPort oPort =
            op.asCompositeOperator().getOutputPorts().get(portIndex.intValue());
        Connection conn = oPort.getIncomingConnections().get(0);
        return conn.getLogicalStreamName(model);
      }
    }
    if (op.isMergerOperator()) {
      assert (portKind == PortKind.Output);
      PrimitiveOutputPort oPort = op.asMergerOperator().getOutputPorts().get(0);
      return oPort.getLogicalStreamName();
    }
    assert (false);
    return null;
  }

  public List<TopologyNodeInputPort> getDestinationInputPort(
      Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {
    List<TopologyNodeInputPort> l = new ArrayList<>();
    // Get the operator index and see if it is a composite or a primitive
    Operator op = model.getModelOperator(operIndex);
    if (op.isCompositeOperator()) {
      if (portKind.equals(PortKind.Input)) {
        CompositeInputPort iPort =
            op.asCompositeOperator().getInputPorts().get(portIndex.intValue());
        List<Connection> outgoingConnections = iPort.getOutgoingConnections();
        for (Connection con : outgoingConnections) {
          List<TopologyNodeInputPort> newList = con.getDestinationInputPort(nodeMap, model);
          l.addAll(newList);
        }
      } else {
        CompositeOutputPort oPort =
            op.asCompositeOperator().getOutputPorts().get(portIndex.intValue());
        List<Connection> outgoingConnections = oPort.getOutgoingConnections();
        for (Connection con : outgoingConnections) {
          List<TopologyNodeInputPort> newList = con.getDestinationInputPort(nodeMap, model);
          l.addAll(newList);
        }
      }
      return l;
    } else if (op.isExportOperator()) {
      return l;
    } else {
      if (op.isSplitterOperator()) {
        SplitterOperator split = op.asSplitterOperator();
        if (split.isBroadcastSplitter()) {
          // If this is a broadcast splitter we look through it to find its downstream connections
          List<PrimitiveOutputPort> oPorts = split.getOutputPorts();
          for (PrimitiveOutputPort oPort : oPorts) {
            List<Connection> connections = oPort.getConnections();
            for (Connection con : connections) {
              List<TopologyNodeInputPort> newList = con.getDestinationInputPort(nodeMap, model);
              l.addAll(newList);
            }
          }
        }
      } else {
        if (op.isMergerOperator()) {
          // We need to look through the Merger
          List<Connection> connections =
              op.asMergerOperator().getOutputPorts().get(0).getConnections();
          for (Connection con : connections) {
            List<TopologyNodeInputPort> newList = con.getDestinationInputPort(nodeMap, model);
            l.addAll(newList);
          }

        } else {
          // This is a primitive. Get the input port.
          TopologyNode dstNode = nodeMap.get(operIndex);
          TopologyNodeInputPort dstIPort = dstNode.getInputPorts().get(portIndex);
          l.add(dstIPort);
        }
      }
      return l;
    }
  }

  public List<LogicalExportedStream> getExportedStream(SPLLogicalModel model)
      throws SAMInternalErrorException {
    List<LogicalExportedStream> exportedStreams = new ArrayList<>();
    Operator op = model.getModelOperator(operIndex);
    if (op.isCompositeOperator()) {
      List<Connection> outgoingConnections = null;
      if (portKind.equals(PortKind.Input)) {
        CompositeInputPort iPort =
            op.asCompositeOperator().getInputPorts().get(portIndex.intValue());
        outgoingConnections = iPort.getOutgoingConnections();

      } else {
        CompositeOutputPort oPort =
            op.asCompositeOperator().getOutputPorts().get(portIndex.intValue());
        outgoingConnections = oPort.getOutgoingConnections();
      }
      for (Connection con : outgoingConnections) {
        List<LogicalExportedStream> exps = con.getExportedStream(model);
        exportedStreams.addAll(exps);
      }

    } else if (op.isExportOperator()) {
      exportedStreams.add(op.asExportOperator().getExportedStream());
    }
    return exportedStreams;
  }

  public void setPortIndex(BigInteger portIndex) {
    this.portIndex = portIndex;
  }

  public void addConnection(Connection connection, SPLLogicalModel model)
      throws SAMInternalErrorException {
    Operator toModelOper = model.getModelOperator(operIndex);
    if (portKind == PortKind.Input) {
      toModelOper.addInputPortConnection(portIndex, connection);
    } else {
      toModelOper.addOutputPortConnection(portIndex, connection);
    }
  }

  public void setOperIndex(BigInteger operIndex) {
    this.operIndex = operIndex;
  }

  public void setPortKind(PortKind portKind) {
    this.portKind = portKind;
  }

  public void fixReverseConnection(
      BigInteger oldToOperIndex,
      BigInteger oldToPortIndex,
      BigInteger newToOperIndex,
      BigInteger newToPortIndex,
      PortKind toPortKind,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    Operator toModelOper = model.getModelOperator(operIndex);
    toModelOper.fixReverseConnection(
        portIndex,
        portKind,
        oldToOperIndex,
        oldToPortIndex,
        newToOperIndex,
        newToPortIndex,
        toPortKind);
  }

  public BigInteger getOperIndex() {
    return operIndex;
  }

  public BigInteger getPortIndex() {
    return portIndex;
  }

  public PortKind getPortKind() {
    return portKind;
  }

  public void computeCCReachabilityGraph(
      CCRegion ccRegion, Set<PrimitiveOperatorBase> operatorsSeen, SPLLogicalModel model)
      throws SAMInternalErrorException {
    Operator toModelOper = model.getModelOperator(operIndex);
    if (toModelOper.isCompositeOperator()) {
      CompositeOperator cOper = (CompositeOperator) toModelOper;
      cOper.computeCCReachabilityGraph(portIndex, portKind, ccRegion, operatorsSeen, model);
    } else {
      if (portKind == PortKind.Output) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
      }
      PrimitiveOperatorBase pOper = (PrimitiveOperatorBase) toModelOper;
      pOper.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<connection operIndex=\""
            + getOperIndex().toString()
            + "\" portIndex=\""
            + getPortIndex().toString()
            + "\" portKind=\""
            + portKind.toString()
            + "\"/>");
    return dumper.toString();
  }

  public void getSplitters(List<SplitterOperator> splitterOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    Operator op = model.getModelOperator(operIndex);
    if (op.isSplitterOperator()) {
      SplitterOperator splitter = op.asSplitterOperator();
      if (splitter.isBroadcastSplitter()) {
        splitter.getOutputPortSplitters(splitterOps, model);
      } else {
        splitterOps.add(splitter);
      }
    } else if (op.isCompositeOperator()) {
      // We have a call through from an output port looking for splitters.  We need to pass it
      // through
      // If this connection points to an input port we are coming from outside, otherwise we are
      // coming from inside
      CompositeOperator cOper = op.asCompositeOperator();
      if (portKind == PortKind.Input) {
        cOper.getInputPortOutgoingSplitters(portIndex, splitterOps, model);
      } else {
        cOper.getOutputPortOutgoingSplitters(portIndex, splitterOps, model);
      }
    } else if (op.isMergerOperator()) {
      // Pass the request on to the merger operator
      op.asMergerOperator().getOutputPortSplitters(splitterOps, model);
    }
    // Otherwise we dead end here because whatever type of operator it can't forward to a splitter
  }

  public void getMergers(List<MergerOperator> mergerOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    Operator op = model.getModelOperator(operIndex);
    if (op.isMergerOperator()) {
      mergerOps.add(op.asMergerOperator());

      // mergers can feed mergers! happens when parallel regions end on nested composite operator
      // output boundary
      op.asMergerOperator().getInputPortMergers(mergerOps, model);
    } else if (op.isCompositeOperator()) {
      CompositeOperator cOper = op.asCompositeOperator();
      if (portKind == PortKind.Input) {
        cOper.getInputPortIncomingMergers(portIndex, mergerOps, model);
      } else {
        cOper.getOutputPortIncomingMergers(portIndex, mergerOps, model);
      }
    } else if (op.isSplitterOperator()) {
      op.asSplitterOperator().getInputPortMergers(mergerOps, model);
    }
  }

  public boolean fedByMerger(SPLLogicalModel model) throws SAMInternalErrorException {
    Operator op = model.getModelOperator(operIndex);
    if (op.isMergerOperator()) {
      return true;
    } else if (op.isCompositeOperator()) {
      CompositeOperator cOper = op.asCompositeOperator();
      if (portKind == PortKind.Input) {
        return cOper.fedByMergerThroughInput(portIndex, model);
      } else {
        return cOper.fedByMergerThroughOutput(portIndex, model);
      }
    }
    return false;
  }

  public void fixOperatorIndex(ParallelTransformState state) {
    operIndex = state.getOperatorIndexMap().get(operIndex);
  }
}
