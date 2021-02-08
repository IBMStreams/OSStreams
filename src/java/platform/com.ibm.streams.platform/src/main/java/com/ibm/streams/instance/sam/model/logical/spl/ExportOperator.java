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
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class ExportOperator extends PrimitiveOperatorBase {
  private List<PrimitiveExportInputPort> inputPorts = new ArrayList<>();
  private LogicalExportedStream exportedStream;

  @Override
  public ExportOperator asExportOperator() {
    return this;
  }

  public LogicalExportedStream getExportedStream() {
    return exportedStream;
  }

  public void setExportedStream(LogicalExportedStream exportedStream) {
    this.exportedStream = exportedStream;
  }

  public List<PrimitiveExportInputPort> getInputPorts() {
    return inputPorts;
  }

  public ExportOperator(
      String name,
      BigInteger sourceIndex,
      BigInteger index,
      String description,
      List<Annotation> annotations,
      CompositeOperator owningComp,
      BigInteger definitionIndex,
      SPLLogicalModel model)
      throws SAMInternalErrorException {

    super(
        name,
        sourceIndex,
        index,
        description,
        annotations,
        owningComp,
        false,
        definitionIndex,
        model);
    model.getOperatorMap().put(index, this);
    model.getExportOperators().put(name, this);
  }

  public ExportOperator(
      ExportOperator other,
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

    for (PrimitiveExportInputPort modelPrimitiveExportInputPort : other.inputPorts) {
      PrimitiveExportInputPort newPort =
          new PrimitiveExportInputPort(modelPrimitiveExportInputPort, this);
      inputPorts.add(newPort);
    }
    // We need to copy the properties because they may get updated when the topology model is
    // populated.
    List<StreamProperty> properties = null;
    if (other.exportedStream.getProperties() != null) {
      properties = new ArrayList<StreamProperty>();
      for (StreamProperty prop : other.exportedStream.getProperties()) {
        properties.add(new StreamProperty(prop));
      }
    }
    exportedStream =
        new LogicalExportedStream(
            this,
            other.exportedStream.getName(),
            other.exportedStream.isAllowFilter(),
            other.exportedStream.getCongestionPolicy(),
            properties);
    model.getOperatorMap().put(getIndex(), this);
  }

  @Override
  public boolean isExportOperator() {
    return true;
  }

  @Override
  public PrimitiveOperatorBase replicate(
      CompositeOperator owningComp, BigInteger localChannelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    ExportOperator replica = new ExportOperator(this, localChannelIndex, owningComp, model);
    return replica;
  }

  @Override
  public void addInputPortConnection(BigInteger portIndex, Connection conn)
      throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  @Override
  public void createSplittersAndMergers(SPLLogicalModel model) throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  @Override
  public void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (PrimitiveExportInputPort modelPrimitiveExportInputPort : inputPorts) {
      modelPrimitiveExportInputPort.addReverseConnections(channelIndex);
    }
  }

  @Override
  public void computeCCReachabilityGraph(
      CCRegion ccRegion, Set<PrimitiveOperatorBase> operatorsSeen, SPLLogicalModel model)
      throws SAMInternalErrorException {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("computeCCReachabilityGraph: in node " + getFullyQualifiedName());
    }
    if (operatorsSeen.contains(this)) {
      return;
    }
    operatorsSeen.add(this);
    if (getCCInfo().isOblivious()) {
      return;
    }
    // The compiler disallows an Export in a consistent region so it must be marked autonomous
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  @Override
  public void populateNode(TopologyNode node, SPLLogicalModel model) {
    // We don't do anything here as there is no equivalent physical operator
  }

  public void evaluateExportedStreamIntrinsics() throws SAMInternalErrorException {
    exportedStream.evaluate(
        getLocalChannelIndex(),
        getLocalMaxChannels(),
        getGlobalChannelIndex(),
        getGlobalMaxChannels());
  }

  @Override
  public void populateConnections(Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {
    exportedStream.evaluate(
        getLocalChannelIndex(),
        getLocalMaxChannels(),
        getGlobalChannelIndex(),
        getGlobalMaxChannels());
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    try {
      dumper.line(
          "<primitiveOperInstance kind=\"spl.adapter::Export\" index =\""
              + getIndex()
              + "\" name=\""
              + getFullyQualifiedName()
              + "\">");
    } catch (SAMInternalErrorException e) {
      e.printStackTrace();
    }
    dumper.indent();
    if (inputPorts.size() > 0) {
      dumper.indent();
      dumper.line("<inputPorts>");
      dumper.indent();
      dumper.append(inputPorts);
      dumper.append(inputPorts);
      dumper.outdent();
      dumper.line("</inputPorts>");
      dumper.append(exportedStream.toString());
      dumper.outdent();
    }
    dumper.outdent();
    dumper.append(getCCInfo().toString());
    dumper.line("</primitiveOperInstance>");
    return dumper.toString();
  }

  @Override
  public void fixConnections(ParallelTransformState state) throws SAMInternalErrorException {
    for (PrimitiveExportInputPort port : inputPorts) {
      port.fixConnections(state);
    }
  }
}
