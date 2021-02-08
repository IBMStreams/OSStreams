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
import com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType;
import com.ibm.streams.instance.sam.model.topology.ParallelChannel;
import com.ibm.streams.instance.sam.model.topology.Splitter;
import com.ibm.streams.instance.sam.model.topology.SplitterKind;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class SplitterOperator extends PrimitiveOperatorBase {
  private final BigInteger injectingIPortIndex;
  private final ParallelRegion injectingParallelRegion;
  private final List<PrimitiveOutputPort> outputPorts = new ArrayList<>();
  private final List<PrimitiveInputPort> inputPorts = new ArrayList<>(); // Can only have 1

  public List<PrimitiveOutputPort> getOutputPorts() {
    return outputPorts;
  }

  public List<PrimitiveInputPort> getInputPorts() {
    return inputPorts;
  }

  public SplitterOperator(
      BigInteger injectingIPortIndex,
      CompositeOperator owningComposite,
      String logicalStreamName,
      ParallelRegion injectingParallelRegion,
      SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    super(
        model.allocateSplitterName(),
        BigInteger.valueOf(-1),
        BigInteger.valueOf(model.getOperatorMap().size()),
        "",
        null,
        owningComposite,
        false,
        model.findOrCreatePrimitiveOperatorDefinitionIndex(
            model.getSplitterOperatorKind(),
            OperatorRuntimeType.SPL_CPlusPlus,
            model.getSplToolkitIndex()),
        model);
    this.injectingIPortIndex = injectingIPortIndex;
    this.injectingParallelRegion = injectingParallelRegion;
    this.inputPorts.add(new PrimitiveSplitterInputPort(BigInteger.ZERO, this, model));

    model.getOperatorMap().put(getIndex(), this);
    owningComposite.addPrimitiveOperator(this);
    BigInteger width = injectingParallelRegion.getWidth();
    for (long i = 0; i < width.longValue(); i++) {
      PrimitiveOutputPort newPort =
          new PrimitiveSplitterOutputPort(BigInteger.valueOf(i), logicalStreamName, this, model);
      outputPorts.add(newPort);
    }
  }

  public SplitterOperator(
      SplitterOperator other,
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
        "SplitterOperator ctor: replicating splitter index  "
            + other.getIndex().toString()
            + " to index "
            + getIndex().toString());

    SPLLogicalModel.trace(
        "SplitterOperator ctor: replicating splitter index  " + other.getIndex().toString());

    this.injectingIPortIndex = other.getInjectingIPortIndex();
    this.injectingParallelRegion = other.getInjectingParallelRegion();

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

  @Override
  public boolean isSplitterOperator() {
    return true;
  }

  @Override
  public SplitterOperator asSplitterOperator() {
    return this;
  }

  public SplitterKind getSplitterKind() {
    return injectingParallelRegion.getSplitterMap().get(injectingIPortIndex).getKind();
  }

  private List<String> getAttributes() {
    return injectingParallelRegion.getSplitterMap().get(injectingIPortIndex).getAttributes();
  }

  private boolean hasAdjacentParallelRegion(SPLLogicalModel model)
      throws SAMInternalErrorException {
    return inputPorts.get(0).fedByMerger(model);
  }

  public Splitter createTopologySplitter(
      PrimitiveOutputPort owningPrimitiveOutputPort,
      TopologyNodeOutputPort owningOutputPort,
      Map<BigInteger, Splitter> portSplittersMap,
      Map<BigInteger, TopologyNode> nodeMap,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    Boolean dropWindowPuncts = hasAdjacentParallelRegion(model);
    Map<BigInteger, ParallelChannel> parallelChannels = new HashMap<>();
    BigInteger splitterIndex = new BigInteger(String.valueOf(portSplittersMap.size()));
    Splitter splitter =
        new Splitter(
            splitterIndex,
            injectingIPortIndex,
            owningOutputPort,
            getSplitterKind(),
            dropWindowPuncts,
            injectingParallelRegion.getIndex(),
            getAttributes(),
            parallelChannels);
    portSplittersMap.put(splitterIndex, splitter);

    // For each output port create a parallel channel
    for (PrimitiveOutputPort oPort : outputPorts) {
      ParallelChannel pChannel =
          oPort.createParallelChannel(
              owningPrimitiveOutputPort, splitter, portSplittersMap, nodeMap, model);
      parallelChannels.put(pChannel.getIndex(), pChannel);
    }
    return splitter;
  }

  @Override
  public PrimitiveOperatorBase replicate(
      CompositeOperator owningComp, BigInteger localChannelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    PrimitiveOperatorBase replica =
        new SplitterOperator(this, localChannelIndex, owningComp, model);
    return replica;
  }

  @Override
  public void addInputPortConnection(BigInteger portIndex, Connection conn)
      throws SAMInternalErrorException {
    if (portIndex.compareTo(BigInteger.ZERO) != 0) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
    }
    inputPorts.get(0).addConnection(conn);
  }

  @Override
  public void addOutputPortConnection(BigInteger portIndex, Connection conn)
      throws SAMInternalErrorException {
    if (portIndex.compareTo(BigInteger.valueOf(outputPorts.size())) >= 0) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
    }
    if (portIndex.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) >= 0) {
      throw SPLLogicalModel.traceAndReturnException(
          "Internal error - portIndex value exceeds Integer.MAX_VALUE-1: " + portIndex);
    }
    PrimitivePort port = outputPorts.get(portIndex.intValue());
    port.addConnection(conn);
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

    if (fromPortKind == PortKind.Input) {
      if (fromPortIndex.compareTo(BigInteger.ZERO) != 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      inputPorts
          .get(0)
          .fixConnection(
              oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
    } else {
      throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
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
    for (PrimitivePort oPort : outputPorts) {
      oPort.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    try {
      dumper.line(
          "<primitiveOperInstance name=\""
              + getFullyQualifiedName()
              + "\" kind=\"spl.adapter::Splitter\" index =\""
              + getIndex().toString()
              + "\" logicalIndex=\""
              + getLogicalIndex()
              + "\" localChannelIndex=\""
              + getLocalChannelIndex().toString()
              + "\" splitterKind=\""
              + getSplitterKind().toString()
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
  public void populateNode(TopologyNode node, SPLLogicalModel model) {
    // This operator exists in the logical model only.
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

  public BigInteger getInjectingIPortIndex() {
    return injectingIPortIndex;
  }

  public ParallelRegion getInjectingParallelRegion() {
    return injectingParallelRegion;
  }

  public boolean isBroadcastSplitter() {
    return getSplitterKind().compareTo(SplitterKind.Broadcast) == 0;
  }

  public void getOutputPortSplitters(List<SplitterOperator> splitterOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (PrimitiveOutputPort oPort : outputPorts) {
      oPort.getSplitters(splitterOps, model);
    }
  }

  public BigInteger getTupleTypeIndex(SPLLogicalModel model) throws SAMInternalErrorException {
    return inputPorts.get(0).getTupleTypeIndex(model);
  }

  public void getInputPortMergers(List<MergerOperator> mergerOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (PrimitiveInputPort iPort : inputPorts) {
      iPort.getMergers(mergerOps, model);
    }
  }
}
