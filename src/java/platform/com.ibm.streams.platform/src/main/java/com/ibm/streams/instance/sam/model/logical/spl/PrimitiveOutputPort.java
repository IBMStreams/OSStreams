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

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.exception.SubmissionParameterError;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.Encoding;
import com.ibm.streams.instance.sam.model.topology.ExportedStream;
import com.ibm.streams.instance.sam.model.topology.ParallelChannel;
import com.ibm.streams.instance.sam.model.topology.Splitter;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import com.ibm.streams.instance.sam.model.topology.Transport;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;

public class PrimitiveOutputPort extends PrimitivePort {
  private String logicalStreamName = "";
  private Boolean viewable = true;
  private Boolean singleThreadedOnOutput = true;

  public void setLogicalStreamName(String streamName) {
    this.logicalStreamName = streamName;
  }

  public String getLogicalStreamName() {
    return logicalStreamName;
  }

  public void setViewable(boolean viewable) {
    this.viewable = viewable;
  }

  public boolean getViewable() {
    return viewable;
  }

  public void setSingleThreadedOnOutput(boolean singleThreadedOnOutput) {
    this.singleThreadedOnOutput = singleThreadedOnOutput;
  }

  public boolean getSingleThreadedOnOutput() {
    return singleThreadedOnOutput;
  }

  public PrimitiveOutputPort(
      BigInteger index, String logicalStreamName, Operator owningPrimitiveOp, SPLLogicalModel model)
      throws SAMInternalErrorException {
    super(index, owningPrimitiveOp, model);
    this.logicalStreamName = logicalStreamName;
  }

  public PrimitiveOutputPort(
      BigInteger index,
      String logicalStreamName,
      Operator owningPrimitiveOp,
      String portName,
      Encoding encoding,
      Transport transport,
      BigInteger tupleTypeIndex,
      Boolean isMutable,
      Boolean isViewable,
      SPLLogicalModel model) {
    super(
        index, owningPrimitiveOp, portName, encoding, transport, tupleTypeIndex, isMutable, model);
    this.logicalStreamName = logicalStreamName;
    this.viewable = isViewable;
  }

  public PrimitiveOutputPort(PrimitiveOutputPort other, Operator owningPrimitiveOp) {
    super(other, owningPrimitiveOp);
    logicalStreamName = other.logicalStreamName;
    viewable = other.viewable;
    singleThreadedOnOutput = other.singleThreadedOnOutput;
  }

  public void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : getConnections()) {
      conn.setPortIndex(channelIndex);
      Connection mc = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Output);
      conn.addConnection(mc, model);
    }
  }

  public void createMergers(ParallelRegion parallelRegion, SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    for (Connection conn : getConnections()) {
      MergerOperator newMerger =
          new MergerOperator(
              getOwningOperator().getOwningComposite(),
              getLogicalStreamName(),
              parallelRegion,
              model);
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Connection.createMergers: create merger " + newMerger.getIndex());
      }
      Connection mc1 = new Connection(conn);
      newMerger.addOutputPortConnection(BigInteger.ZERO, mc1);

      Connection mc2 = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Output);
      newMerger.addInputPortConnection(BigInteger.ZERO, mc2);

      conn.fixReverseConnection(
          getOwningOperator().getIndex(),
          getIndex(),
          newMerger.getIndex(),
          BigInteger.ZERO,
          PortKind.Output,
          model);

      conn.setOperIndex(newMerger.getIndex());
      conn.setPortIndex(BigInteger.ZERO);
      conn.setPortKind(PortKind.Input);
    } // end for
  }

  private static void splitStreamName(
      String fullyQualifiedStreamName, StringBuilder compositeName, StringBuilder streamName) {

    int pos = fullyQualifiedStreamName.lastIndexOf('.');
    if (pos != -1) {
      compositeName.append(fullyQualifiedStreamName.substring(0, pos));
      streamName.append(fullyQualifiedStreamName.substring(pos + 1));
      return;
    }
    compositeName.append("");
    streamName.append(fullyQualifiedStreamName);
  }

  public String getStreamName() throws SAMInternalErrorException {
    // compute stream name
    // given the logical stream name we know the logical composite in which
    // the stream is declared
    StringBuilder compositeNameBuilder = new StringBuilder();
    StringBuilder streamNameBuilder = new StringBuilder();
    StringBuilder fullyQualifiedStreamNameBuilder = new StringBuilder();

    splitStreamName(logicalStreamName, compositeNameBuilder, streamNameBuilder);
    List<String> subscripts = new ArrayList<>();
    if (getOwningOperator().isSplitterOperator()) {
      StringBuilder s = new StringBuilder();
      s.append("[").append(getIndex()).append("]");
      subscripts.add(s.toString());
    }
    if (getOwningOperator().isParallelRegion()) {
      StringBuilder s = new StringBuilder();
      s.append("[").append(getOwningOperator().getLocalChannelIndex()).append("]");
      subscripts.add(s.toString());
    }

    CompositeOperator comp = getOwningOperator().getOwningComposite();
    while (comp != null) {
      if (comp.getFullyQualifiedOwningLogicalCompositeName()
          .equals(compositeNameBuilder.toString())) {
        fullyQualifiedStreamNameBuilder.append(comp.getFullyQualifiedOwningCompositeName());
        break;
      }
      if (comp.isMainComposite()) {
        comp = null;
        throw SPLLogicalModel.traceAndReturnException(
            "Internal error: could not find owning composite");
      } else {
        if (comp.isParallelRegion()) {
          StringBuilder s = new StringBuilder();
          s.append("[").append(comp.getLocalChannelIndex()).append("]");
          subscripts.add(s.toString());
        }
        comp = comp.getOwningComposite();
      }
    } // end while

    if (fullyQualifiedStreamNameBuilder.length() != 0) {
      fullyQualifiedStreamNameBuilder.append(".");
    }
    fullyQualifiedStreamNameBuilder.append(streamNameBuilder.toString());

    ListIterator<String> li = subscripts.listIterator(subscripts.size());
    while (li.hasPrevious()) {
      fullyQualifiedStreamNameBuilder.append(li.previous());
    }
    return fullyQualifiedStreamNameBuilder.toString();
  }

  public void populatePort(TopologyNodeOutputPort port) throws SAMInternalErrorException {
    super.populatePort(port);
    port.setStreamName(getStreamName());
    port.setLogicalStreamName(getLogicalStreamName());
    port.setViewable(viewable);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    try {
      dumper.line(
          "<outputPort viewable=\""
              + viewable.toString()
              + "\" streamName=\""
              + getStreamName()
              + "\" logicalStreamName=\""
              + getLogicalStreamName()
              + "\">");
    } catch (SAMInternalErrorException e) {
      e.printStackTrace();
    }
    dumper.indent();
    dumper.append(super.toString());
    dumper.outdent();
    dumper.line("</outputPort>");
    return dumper.toString();
  }

  public void populateConnections(Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {
    BigInteger srcOperIndex = getOwningOperator().getIndex();
    BigInteger srcOPortIndex = getIndex();
    TopologyNode srcNode = nodeMap.get(srcOperIndex);
    TopologyNodeOutputPort srcOPort = srcNode.getOutputPorts().get(srcOPortIndex);
    List<Connection> connections = getConnections();
    Map<BigInteger, Splitter> portSplittersMap = new HashMap<>();
    srcOPort.setPortSplittersMap(portSplittersMap);
    List<Splitter> splitters = new ArrayList<>();
    srcOPort.setSplitters(splitters);
    for (Connection conn : connections) {
      List<TopologyNodeInputPort> dstIPorts = conn.getDestinationInputPort(nodeMap, model);
      if (dstIPorts.size() > 0) {
        for (TopologyNodeInputPort dstIPort : dstIPorts) {
          TopologyNodeConnection topologyConnection =
              new TopologyNodeConnection(srcOPort, dstIPort);
          srcOPort.addOperConnection(topologyConnection);
          dstIPort.addOperConnection(topologyConnection);
        }
      }
      // We have a logical connection but got back no physical
      // connection. That means we have an export operator
      List<LogicalExportedStream> exportedStreams = conn.getExportedStream(model);
      if (exportedStreams.size() > 0) {
        assert (exportedStreams.size() == 1);
        LogicalExportedStream les = exportedStreams.get(0);
        ExportedStream es =
            new ExportedStream(
                les.getExportOperName(),
                les.getName(),
                les.isAllowFilter(),
                les.getCongestionPolicy(),
                les.getProperties());
        srcOPort.setExport(es);
      }
      // Get a list of splitter operators owned directly by this port
      List<SplitterOperator> splitterOps = new ArrayList<>();
      conn.getSplitters(splitterOps, model);

      for (SplitterOperator op : splitterOps) {
        Splitter splitter =
            op.asSplitterOperator()
                .createTopologySplitter(this, srcOPort, portSplittersMap, nodeMap, model);
        splitters.add(splitter);
      }
    }
  }

  public ParallelChannel createParallelChannel(
      PrimitiveOutputPort owningPrimitiveOutputPort,
      Splitter splitter,
      Map<BigInteger, Splitter> portSplittersMap,
      Map<BigInteger, TopologyNode> nodeMap,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    assert (getOwningOperator().isSplitterOperator());
    BigInteger srcOperIndex = owningPrimitiveOutputPort.getOwningOperator().getIndex();
    BigInteger srcOPortIndex = owningPrimitiveOutputPort.getIndex();
    TopologyNode srcNode = nodeMap.get(srcOperIndex);
    TopologyNodeOutputPort srcOPort = srcNode.getOutputPorts().get(srcOPortIndex);

    ParallelChannel pChannel = new ParallelChannel(getIndex(), splitter);
    for (Connection conn : getConnections()) {
      List<TopologyNodeInputPort> dstIPorts = conn.getDestinationInputPort(nodeMap, model);
      if (dstIPorts.size() > 0) {
        for (TopologyNodeInputPort dstIPort : dstIPorts) {
          TopologyNodeConnection topologyConnection =
              new TopologyNodeConnection(srcOPort, dstIPort, splitter.getIndex(), getIndex());
          srcOPort.addOperConnection(topologyConnection);
          dstIPort.addOperConnection(topologyConnection);
        }
      }
      // Find any nested splitters
      List<SplitterOperator> splitterOps = new ArrayList<>();

      conn.getSplitters(splitterOps, model);
      if (!splitterOps.isEmpty()) {
        List<Splitter> splitters = new ArrayList<>();
        for (SplitterOperator op : splitterOps) {
          Splitter nestedSplitter =
              op.asSplitterOperator()
                  .createTopologySplitter(
                      owningPrimitiveOutputPort, srcOPort, portSplittersMap, nodeMap, model);
          splitters.add(nestedSplitter);
        }
        pChannel.setSplitters(splitters);
      }
    }
    return pChannel;
  }
}
