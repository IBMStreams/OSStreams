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
import com.ibm.streams.instance.sam.exception.SubmissionParameterError;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.Merger;
import com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class MergerOperator extends PrimitiveOperatorBase {
  private final ParallelRegion injectingParallelRegion;
  private final List<PrimitiveOutputPort> outputPorts = new ArrayList<>();
  private final List<PrimitiveInputPort> inputPorts = new ArrayList<>();

  public MergerOperator(
      CompositeOperator owningComposite,
      String logicalStreamName,
      ParallelRegion parallelRegion,
      SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    super(
        model.allocateMergerName(),
        BigInteger.valueOf(-1),
        BigInteger.valueOf(model.getOperatorMap().size()),
        "",
        null,
        owningComposite,
        false,
        model.findOrCreatePrimitiveOperatorDefinitionIndex(
            model.getMergerOperatorKind(),
            OperatorRuntimeType.SPL_CPlusPlus,
            model.getSplToolkitIndex()),
        model);

    this.injectingParallelRegion = parallelRegion;
    outputPorts.add(new PrimitiveOutputPort(BigInteger.ZERO, logicalStreamName, this, model));

    model.getOperatorMap().put(getIndex(), this);
    owningComposite.addPrimitiveOperator(this);
    BigInteger width = parallelRegion.getWidth();
    for (long i = 0; i < width.longValue(); i++) {
      PrimitiveInputPort newPort = new PrimitiveInputPort(BigInteger.valueOf(i), this, model);
      inputPorts.add(newPort);
    }
  }

  public MergerOperator(
      MergerOperator other,
      BigInteger localChannelIndex,
      CompositeOperator owningComp,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    super(
        other,
        BigInteger.valueOf(model.getOperatorMap().size()),
        localChannelIndex,
        owningComp,
        model);
    SPLLogicalModel.trace(
        "MergerOperator ctor: replicating merger index  "
            + other.getIndex().toString()
            + " to index "
            + getIndex().toString());
    this.injectingParallelRegion = other.injectingParallelRegion;

    // replicate the input and output ports
    for (PrimitiveInputPort modelPrimitiveInputPort : other.inputPorts) {
      PrimitiveInputPort newPort = new PrimitiveInputPort(modelPrimitiveInputPort, this);
      inputPorts.add(newPort);
    }

    for (PrimitiveOutputPort modelPrimitiveOutputPort : other.outputPorts) {
      PrimitiveOutputPort newPort = new PrimitiveOutputPort(modelPrimitiveOutputPort, this);
      outputPorts.add(newPort);
    }
    model.getOperatorMap().put(getIndex(), this);
  }

  public List<PrimitiveInputPort> getInputPorts() {
    return inputPorts;
  }

  public List<PrimitiveOutputPort> getOutputPorts() {
    return outputPorts;
  }

  @Override
  public boolean isMergerOperator() {
    return true;
  }

  @Override
  public MergerOperator asMergerOperator() {
    return this;
  }

  public void getOutputPortSplitters(List<SplitterOperator> splitterOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    PrimitiveOutputPort oPort = outputPorts.get(0);
    oPort.getSplitters(splitterOps, model);
  }

  public void getInputPortMergers(List<MergerOperator> mergerOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    PrimitiveInputPort iPort = inputPorts.get(0);
    iPort.getMergers(mergerOps, model);
  }

  @Override
  public PrimitiveOperatorBase replicate(
      CompositeOperator owningComp, BigInteger localChannelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    PrimitiveOperatorBase replica = new MergerOperator(this, localChannelIndex, owningComp, model);
    return replica;
  }

  public Merger createTopologyMerger(int mergerIndex, TopologyNodeInputPort owningInputPort)
      throws SAMInternalErrorException {
    return new Merger(
        BigInteger.valueOf(mergerIndex), owningInputPort, injectingParallelRegion.getIndex());
  }

  @Override
  public void addInputPortConnection(BigInteger portIndex, Connection conn)
      throws SAMInternalErrorException {
    if (portIndex.compareTo(BigInteger.valueOf(inputPorts.size())) >= 0) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
    }
    if (portIndex.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) >= 0) {
      throw SPLLogicalModel.traceAndReturnException(
          "Internal error - portIndex value exceeds Integer.MAX_VALUE-1: " + portIndex);
    }
    PrimitivePort port = inputPorts.get(portIndex.intValue());
    port.addConnection(conn);
  }

  @Override
  public void addOutputPortConnection(BigInteger portIndex, Connection conn)
      throws SAMInternalErrorException {
    if (portIndex.compareTo(BigInteger.ZERO) != 0) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
    }
    outputPorts.get(0).addConnection(conn);
  }

  @Override
  public void createSplittersAndMergers(SPLLogicalModel model) throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  @Override
  public void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  @Override
  public void fixReverseConnection(
      BigInteger fromPortIndex,
      PortKind fromPortKind,
      BigInteger oldToOperIndex,
      BigInteger oldToPortIndex,
      BigInteger newToOperIndex,
      BigInteger newToPortIndex,
      PortKind toPortKind)
      throws SAMInternalErrorException {

    if (fromPortKind == PortKind.Output) {
      if (fromPortIndex.compareTo(BigInteger.ZERO) != 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      outputPorts
          .get(0)
          .fixConnection(
              oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
    } else {
      if (fromPortIndex.compareTo(BigInteger.valueOf(inputPorts.size())) >= 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      if (fromPortIndex.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) >= 0) {
        throw SPLLogicalModel.traceAndReturnException(
            "Internal error - fromPortIndex value exceeds Integer.MAX_VALUE-1: " + fromPortIndex);
      }
      PrimitivePort port = inputPorts.get(fromPortIndex.intValue());
      port.fixConnection(
          oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
    }
  }

  @Override
  public void computeCCReachabilityGraph(
      CCRegion ccRegion, Set<PrimitiveOperatorBase> operatorsSeen, SPLLogicalModel model)
      throws SAMInternalErrorException {
    if (operatorsSeen.contains(this)) {
      return;
    }
    operatorsSeen.add(this);
    ccRegion.addRegionEntry(this, model);
    if (getCCInfo().isEndCutRegion()) {
      return;
    }
    outputPorts.get(0).computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
  }

  @Override
  public void populateNode(TopologyNode node, SPLLogicalModel model) {
    // This operator exists in the logical model only
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    try {
      dumper.line(
          "<primitiveOperInstance name=\""
              + getFullyQualifiedName()
              + "\" kind=\"spl.adapter::Merger\" index =\""
              + getIndex().toString()
              + "\" logicalIndex=\""
              + getLogicalIndex()
              + "\" localChannelIndex=\""
              + getLocalChannelIndex().toString()
              + "\">");
    } catch (SAMInternalErrorException e) {
      e.printStackTrace();
    }
    dumper.indent();
    dumper.line("<inputPorts>");
    dumper.indent();
    dumper.append(inputPorts);
    dumper.outdent();
    dumper.line("</inputPorts>");
    dumper.line("<outputPorts>");
    dumper.indent();
    dumper.append(outputPorts);
    dumper.outdent();
    dumper.line("</outputPorts>");
    dumper.outdent();
    dumper.line("</primitiveOperInstance>");
    return dumper.toString();
  }

  @Override
  public void fixConnections(ParallelTransformState state) throws SAMInternalErrorException {
    for (PrimitiveInputPort port : inputPorts) {
      port.fixConnections(state);
    }
    for (PrimitiveOutputPort port : outputPorts) {
      port.fixConnections(state);
    }
  }
}
