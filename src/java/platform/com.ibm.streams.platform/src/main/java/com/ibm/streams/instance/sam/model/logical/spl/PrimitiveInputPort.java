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
import com.ibm.streams.instance.sam.model.topology.Merger;
import com.ibm.streams.instance.sam.model.topology.ThreadedPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.Transport;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class PrimitiveInputPort extends PrimitivePort {
  private boolean isControl = false;
  private ThreadedPort threadedPort = null;

  public void setIsControl(boolean isControl) {
    this.isControl = isControl;
  }

  public void setThreadedPort(ThreadedPort threadedPort) {
    this.threadedPort = threadedPort;
  }

  public PrimitiveInputPort(BigInteger index, Operator owningPrimitiveOp, SPLLogicalModel model)
      throws SAMInternalErrorException {
    super(index, owningPrimitiveOp, model);
  }

  public PrimitiveInputPort(
      BigInteger index,
      Operator owningPrimitiveOp,
      String portName,
      Encoding encoding,
      Transport transport,
      BigInteger tupleTypeIndex,
      Boolean isMutable,
      Boolean isControl,
      ThreadedPort threadedPort,
      SPLLogicalModel model) {
    super(
        index, owningPrimitiveOp, portName, encoding, transport, tupleTypeIndex, isMutable, model);
    this.isControl = isControl.booleanValue();
    this.threadedPort = threadedPort;
  }

  public PrimitiveInputPort(PrimitiveInputPort other, Operator owningPrimitiveOp) {
    super(other, owningPrimitiveOp);
    isControl = other.isControl;
    threadedPort = ((other.threadedPort != null) ? new ThreadedPort(other.threadedPort) : null);
  }

  public void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : getConnections()) {
      conn.setPortIndex(channelIndex);
      Connection mc = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Input);
      conn.addConnection(mc, model);
    }
  }

  public void createSplitters(ParallelRegion parallelRegion, SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    for (Connection conn : getConnections()) {
      String streamName = conn.getStreamName(model);
      SplitterOperator newSplitter =
          new SplitterOperator(
              getIndex(),
              getOwningOperator().getOwningComposite(),
              streamName,
              parallelRegion,
              model);
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace(
            "PrimitiveInputPort.createSplitters: create splitter " + newSplitter.getIndex());
      }
      Connection mc1 = new Connection(conn);
      newSplitter.addInputPortConnection(BigInteger.ZERO, mc1);

      Connection mc2 = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Input);
      newSplitter.addOutputPortConnection(BigInteger.ZERO, mc2);

      // Fix the connection from the operator that is now feeding the splitter to the splitter
      conn.fixReverseConnection(
          getOwningOperator().getIndex(),
          getIndex(),
          newSplitter.getIndex(),
          BigInteger.ZERO,
          PortKind.Input,
          model);

      conn.setOperIndex(newSplitter.getIndex());
      conn.setPortIndex(BigInteger.ZERO);
      conn.setPortKind(PortKind.Output);
    } // end for
  }

  public boolean isControl() {
    return isControl;
  }

  public boolean hasThreadedPort() {
    return (threadedPort != null);
  }

  public ThreadedPort getThreadedPort() {
    return threadedPort;
  }

  public void populatePort(TopologyNodeInputPort port) {
    super.populatePort(port);
    port.setIsControl(isControl);
    port.setThreadedPort(threadedPort);
  }

  public void populateConnections(Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {
    BigInteger srcOperIndex = getOwningOperator().getIndex();
    BigInteger srcIPortIndex = getIndex();
    TopologyNode srcNode = nodeMap.get(srcOperIndex);
    TopologyNodeInputPort srcIPort = srcNode.getInputPorts().get(srcIPortIndex);

    List<Merger> mergers = new ArrayList<>();
    for (Connection conn : getConnections()) {
      List<MergerOperator> mergerOps = new ArrayList<>();
      conn.getMergers(mergerOps, model);

      int index = 0;
      for (MergerOperator merger : mergerOps) {
        mergers.add(merger.createTopologyMerger(index, srcIPort));
        ++index;
      }
    }
    srcIPort.setMergers(mergers);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<inputPort isControl=\"" + isControl() + "\">");
    dumper.indent();
    if (threadedPort != null) {
      dumper.line(threadedPort.toString());
    }
    dumper.append(super.toString());
    dumper.outdent();
    dumper.line("</inputPort>");
    return dumper.toString();
  }
}
