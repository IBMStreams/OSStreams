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
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class ImportOperator extends PrimitiveOperatorBase {
  private final List<PrimitiveImportOutputPort> outputPorts = new ArrayList<>();
  private final LogicalImportedStreams importedStreams;
  private boolean initialized = false;

  public ImportedStreams getImportedStreams() {
    return importedStreams;
  }

  public List<PrimitiveImportOutputPort> getOutputPorts() {
    return outputPorts;
  }

  @Override
  public boolean isImportOperator() {
    return true;
  }

  @Override
  public ImportOperator asImportOperator() {
    return this;
  }

  public ImportOperator(
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
    importedStreams = new LogicalImportedStreams();
    model.getOperatorMap().put(index, this);
    model.getImportOperators().put(name, this);
  }

  public ImportOperator(
      ImportOperator other,
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

    for (PrimitiveImportOutputPort modelPrimitiveImportOutputPort : other.outputPorts) {
      PrimitiveImportOutputPort newPort =
          new PrimitiveImportOutputPort(modelPrimitiveImportOutputPort, this);
      outputPorts.add(newPort);
    }

    importedStreams = new LogicalImportedStreams(other.importedStreams);
    model.getOperatorMap().put(getIndex(), this);
  }

  @Override
  public PrimitiveOperatorBase replicate(
      CompositeOperator owningComp, BigInteger localChannelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    ImportOperator replica = new ImportOperator(this, localChannelIndex, owningComp, model);
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
    for (PrimitiveImportOutputPort modelPrimitiveImportOutputPort : outputPorts) {
      modelPrimitiveImportOutputPort.addReverseConnections(channelIndex, model);
    }
  }

  @Override
  public void computeCCReachabilityGraph(
      CCRegion ccRegion, Set<PrimitiveOperatorBase> operatorsSeen, SPLLogicalModel model)
      throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  @Override
  public void populateNode(TopologyNode node, SPLLogicalModel model) {
    // We don't do anything here as there is no equivalent physical operator
  }

  @Override
  public void populateConnections(Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {
    // We need to patch the name in case this import operator was in a parallel region
    importedStreams.setImportOperName(getFullyQualifiedName());
    importedStreams.evaluate(
        getLocalChannelIndex(),
        getLocalMaxChannels(),
        getGlobalChannelIndex(),
        getGlobalMaxChannels());
    for (PrimitiveImportOutputPort oPort : outputPorts) {
      oPort.populateConnections(importedStreams, nodeMap, model);
    }
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    try {
      dumper.line(
          "<primitiveOperInstance kind=\"spl.adapter::Import\" index =\""
              + getIndex()
              + "\" name=\""
              + getFullyQualifiedName()
              + "\">");
    } catch (SAMInternalErrorException e) {
      e.printStackTrace();
    }
    dumper.indent();
    if (outputPorts.size() > 0) {
      dumper.line("<outputPorts>");
      dumper.indent();
      dumper.append(outputPorts);
      dumper.outdent();
      dumper.line("</outputPorts>");
    }
    dumper.append(importedStreams.toString());
    dumper.append(getCCInfo().toString());
    dumper.outdent();
    dumper.line("</primitiveOperInstance>");
    return dumper.toString();
  }

  @Override
  public void fixConnections(ParallelTransformState state) throws SAMInternalErrorException {
    for (PrimitiveImportOutputPort port : outputPorts) {
      port.fixConnections(state);
    }
  }

  public boolean isInitialized() {
    return initialized;
  }

  public void setInitialized() {
    this.initialized = true;
  }
}
