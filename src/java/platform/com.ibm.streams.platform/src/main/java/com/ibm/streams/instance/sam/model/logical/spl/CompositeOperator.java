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

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.exception.SubmissionParameterError;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class CompositeOperator extends Operator {
  protected final List<CompositeInputPort> inputPorts = new ArrayList<>();
  protected final List<CompositeOutputPort> outputPorts = new ArrayList<>();
  protected final List<PrimitiveOperatorBase> primitiveOperators = new ArrayList<>();
  protected final List<CompositeOperator> compositeOperators = new ArrayList<>();
  protected ThreadingModel threadingModel = null;

  public void setThreadingModel(ThreadingModel threadingModel) {
    this.threadingModel = threadingModel;
  }

  public ThreadingModel getThreadingModel(SPLLogicalModel model) throws SAMInternalErrorException {
    // If this instance of a composite operator has a threading model, return that
    if (threadingModel != null) {
      return threadingModel;
    }
    // Otherwise check if this composite's definition has a threading model
    CompositeOperatorDefinition def =
        model.getCompositeOperDefs().get(getDefinitionIndex().intValue());
    ThreadingModel tm = def.getThreadingModel();
    if (tm != null) {
      return tm;
    }
    // Otherwise check if the owning composite has a threading model
    if (hasOwningComposite()) {
      return getOwningComposite().getThreadingModel(model);
    }
    // Otherwise null indicates the default model
    return null;
  }

  public List<PrimitiveOperatorBase> getPrimitiveOperators() {
    return primitiveOperators;
  }

  public List<CompositeOperator> getCompositeOperators() {
    return compositeOperators;
  }

  @Override
  public CompositeOperator asCompositeOperator() {
    return this;
  }

  public List<CompositeInputPort> getInputPorts() {
    return inputPorts;
  }

  public List<CompositeOutputPort> getOutputPorts() {
    return outputPorts;
  }

  protected CompositeOperator(
      String name,
      BigInteger sourceIndex,
      BigInteger index,
      String description,
      List<Annotation> annotations,
      CompositeOperator owningComp,
      BigInteger definitionIndex) {
    super(name, sourceIndex, index, description, annotations, owningComp, false, definitionIndex);
  }

  public CompositeOperator(
      CompositeOperator other,
      BigInteger localChannelIndex,
      CompositeOperator owningComp,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    super(other, BigInteger.valueOf(model.getOperatorMap().size()), localChannelIndex, owningComp);
    SPLLogicalModel.trace(
        "CompositeOperator ctor: replicating composite index  "
            + other.getIndex().toString()
            + " to index "
            + getIndex().toString());

    // put in the map to secure the index
    model.getOperatorMap().put(getIndex(), this);
    ParallelTransformState state = new ParallelTransformState();
    state.getOperatorIndexMap().put(other.getIndex(), getIndex());

    // replicate the input ports
    for (CompositeInputPort port : other.inputPorts) {
      CompositeInputPort newPort = new CompositeInputPort(port, this);
      inputPorts.add(newPort);
    }

    // replicate the output ports
    for (CompositeOutputPort port : other.outputPorts) {
      CompositeOutputPort newPort = new CompositeOutputPort(port, this);
      outputPorts.add(newPort);
    }

    // replicate the contained composite operators
    for (CompositeOperator oper : other.compositeOperators) {
      CompositeOperator cReplica = oper.replicate(this, oper.getLocalChannelIndex(), model);
      state.getOperatorIndexMap().put(oper.getIndex(), cReplica.getIndex());
      compositeOperators.add(cReplica);
      if (oper.hasParallelRegion()) {
        oper.getParallelRegion().addReplica(cReplica.getIndex());
      }
    }

    // replicate the contained primitive operators
    for (PrimitiveOperatorBase oper : other.primitiveOperators) {
      PrimitiveOperatorBase pReplica = oper.replicate(this, oper.getLocalChannelIndex(), model);
      state.getOperatorIndexMap().put(oper.getIndex(), pReplica.getIndex());
      primitiveOperators.add(pReplica);
      if (oper.hasParallelRegion()) {
        oper.getParallelRegion().addReplica(pReplica.getIndex());
      }
    }

    // fix all incoming and outgoing connections
    for (CompositeInputPort port : inputPorts) {
      port.fixOutgoingConnections(state);
    }
    for (CompositeOutputPort port : outputPorts) {
      port.fixIncomingConnections(state);
    }

    // Fix all the connections between contained composite and primitive operators
    for (CompositeOperator oper : compositeOperators) {
      oper.fixConnections(state);
    }
    for (PrimitiveOperatorBase oper : primitiveOperators) {
      oper.fixConnections(state);
    }
  }

  private void fixConnections(ParallelTransformState state) {
    for (CompositeInputPort port : inputPorts) {
      port.fixIncomingConnections(state);
    }
    for (CompositeOutputPort port : outputPorts) {
      port.fixOutgoingConnections(state);
    }
  }

  @Override
  public boolean isCompositeOperator() {
    return true;
  }

  private void checkPortIndex(BigInteger portIndex, int size) throws SAMInternalErrorException {
    if (portIndex.compareTo(BigInteger.valueOf(size)) >= 0) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
    }
  }

  private void getPortOutgoingSplitters(
      List<? extends CompositePort> ports,
      BigInteger portIndex,
      List<SplitterOperator> splitterOps,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    checkPortIndex(portIndex, ports.size());
    CompositePort port = ports.get(portIndex.intValue());
    port.getOutgoingSplitters(splitterOps, model);
  }

  public void getInputPortOutgoingSplitters(
      BigInteger portIndex, List<SplitterOperator> splitterOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    getPortOutgoingSplitters(inputPorts, portIndex, splitterOps, model);
  }

  public void getOutputPortOutgoingSplitters(
      BigInteger portIndex, List<SplitterOperator> splitterOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    getPortOutgoingSplitters(outputPorts, portIndex, splitterOps, model);
  }

  private void getPortIncomingMergers(
      List<? extends CompositePort> ports,
      BigInteger portIndex,
      List<MergerOperator> mergerOps,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    checkPortIndex(portIndex, ports.size());
    CompositePort port = ports.get(portIndex.intValue());
    port.getIncomingMergers(mergerOps, model);
  }

  public void getInputPortIncomingMergers(
      BigInteger portIndex, List<MergerOperator> mergerOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    getPortIncomingMergers(inputPorts, portIndex, mergerOps, model);
  }

  public void getOutputPortIncomingMergers(
      BigInteger portIndex, List<MergerOperator> mergerOps, SPLLogicalModel model)
      throws SAMInternalErrorException {
    getPortIncomingMergers(outputPorts, portIndex, mergerOps, model);
  }

  private boolean fedByMerger(
      List<? extends CompositePort> ports, BigInteger portIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    checkPortIndex(portIndex, ports.size());
    CompositePort port = ports.get(portIndex.intValue());
    return port.fedByMerger(model);
  }

  public boolean fedByMergerThroughInput(BigInteger portIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    return fedByMerger(inputPorts, portIndex, model);
  }

  public boolean fedByMergerThroughOutput(BigInteger portIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    return fedByMerger(outputPorts, portIndex, model);
  }

  public void performParallelTransform(SPLLogicalModel model)
      throws SAMInternalErrorException, StreamsException, SubmissionParameterError {
    performParallelTransformHelper(model);
    Set<String> nonParallelCoLocations = findNonParallelCoLocations();
    performParallelResourceTransforms(model, nonParallelCoLocations);
  }

  private Set<String> findNonParallelCoLocations() {
    Set<String> nonParallelCoLocations = new HashSet<>();

    for (CompositeOperator composite : compositeOperators) {
      nonParallelCoLocations.addAll(composite.findNonParallelCoLocations());
    }

    for (PrimitiveOperatorBase primitive : primitiveOperators) {
      if (!primitive.isInParallelRegion() && primitive instanceof PrimitiveOperator) {
        nonParallelCoLocations.addAll(((PrimitiveOperator) primitive).getCoLocations());
      }
    }

    return nonParallelCoLocations;
  }

  private void performParallelTransformHelper(SPLLogicalModel model)
      throws SAMInternalErrorException, StreamsException, SubmissionParameterError {
    SPLLogicalModel.trace(
        "CompositeOperator performParallelTransformHelper: processing composite "
            + getFullyQualifiedLogicalName()
            + " index "
            + getIndex().toString());
    ParallelRegions parallelRegions = model.getParallelRegions();

    // walk the contained primitives and perform any parallel
    // transformations
    for (PrimitiveOperatorBase primitive :
        primitiveOperators.toArray(new PrimitiveOperatorBase[primitiveOperators.size()])) {
      if (parallelRegions.isParallelOperator(primitive.getIndex())) {
        ParallelRegion pr = parallelRegions.getParallelRegion(primitive.getIndex());
        BigInteger regionWidth = pr.getWidth();
        if (regionWidth.longValue() < 1) {
          throw new StreamsException(
              StreamsRuntimeMessagesKey.Key.SAM_InvalidParallelWidth, regionWidth);
        }
        primitive.setParallelRegion(pr);
        primitive.createSplittersAndMergers(model);
        for (long i = 1; i < regionWidth.longValue(); i++) {
          PrimitiveOperatorBase replica = primitive.replicate(this, BigInteger.valueOf(i), model);
          //                    replica.setParallelRegionInfo(pr);
          replica.addReverseConnections(BigInteger.valueOf(i), model);
          primitiveOperators.add(replica);
          pr.addReplica(replica.getIndex());
        }
      }
    } // end for

    // walk any contained composites and generate the parallel model for
    // them
    for (CompositeOperator composite :
        compositeOperators.toArray(new CompositeOperator[compositeOperators.size()])) {
      composite.performParallelTransformHelper(model);
      if (parallelRegions.isParallelOperator(composite.getIndex())) {
        ParallelRegion pr = parallelRegions.getParallelRegion(composite.getIndex());
        BigInteger regionWidth = pr.getWidth();
        if (regionWidth.longValue() < 1) {
          throw new StreamsException(
              StreamsRuntimeMessagesKey.Key.SAM_InvalidParallelWidth, regionWidth);
        }
        composite.setParallelRegion(pr);
        composite.createSplittersAndMergers(model);
        for (long i = 1; i < regionWidth.longValue(); i++) {
          CompositeOperator replica = composite.replicate(this, BigInteger.valueOf(i), model);
          replica.addReverseConnections(BigInteger.valueOf(i), model);
          compositeOperators.add(replica);
          pr.addReplica(replica.getIndex());
        }
      }
    } // end for
  }

  private void performParallelResourceTransforms(
      SPLLogicalModel model, Set<String> nonParallelCoLocations)
      throws SAMInternalErrorException, SubmissionParameterError {
    for (CompositeOperator comp : compositeOperators) {
      comp.performParallelResourceTransforms(model, nonParallelCoLocations);
    }
    for (PrimitiveOperatorBase prim : primitiveOperators) {
      prim.performParallelResourceTransforms(model, nonParallelCoLocations);
    }
  }

  public void performCCResourceTransforms() {
    for (CompositeOperator comp : compositeOperators) {
      comp.performCCResourceTransforms();
    }
    for (PrimitiveOperatorBase prim : primitiveOperators) {
      prim.performCCResourceTransforms();
    }
  }

  public void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (CompositeInputPort modelCompositeInputPort : inputPorts) {
      modelCompositeInputPort.addIncomingReverseConnections(channelIndex, model);
    }

    for (CompositeOutputPort modelCompositeOutputPort : outputPorts) {
      modelCompositeOutputPort.addOutgoingReverseConnections(channelIndex, model);
    }
  }

  private CompositeOperator replicate(
      CompositeOperator owningComp, BigInteger localChannelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    CompositeOperator replica = new CompositeOperator(this, localChannelIndex, owningComp, model);
    return replica;
  }

  public void createSplittersAndMergers(SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    for (CompositeInputPort modelCompositeInputPort : inputPorts) {
      modelCompositeInputPort.createSplitters(getParallelRegion(), model);
    }

    for (CompositeOutputPort modelCompositeOutputPort : outputPorts) {
      modelCompositeOutputPort.createMergers(getParallelRegion(), model);
    }
  }

  @Override
  public void addInputPortConnection(BigInteger portIndex, Connection conn)
      throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  @Override
  public void fixReverseConnection(
      BigInteger portIndex,
      PortKind portKind,
      BigInteger oldToOperIndex,
      BigInteger oldToPortIndex,
      BigInteger newToOperIndex,
      BigInteger newToPortIndex,
      PortKind toPortKind)
      throws SAMInternalErrorException {

    if (portKind == PortKind.Input) {
      if (portIndex.compareTo(BigInteger.valueOf(inputPorts.size())) >= 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      if (portIndex.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) >= 0) {
        throw SPLLogicalModel.traceAndReturnException(
            "Internal error - portIndex value exceeds Integer.MAX_VALUE-1: " + portIndex);
      }
      CompositeInputPort port = inputPorts.get(portIndex.intValue());
      if (toPortKind == PortKind.Input) {
        port.fixOutgoingReverseConnection(
            oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
      } else {
        port.fixIncomingReverseConnection(
            oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
      }
    } else {
      if (portIndex.compareTo(BigInteger.valueOf(outputPorts.size())) >= 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      if (portIndex.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) >= 0) {
        throw SPLLogicalModel.traceAndReturnException(
            "Internal error - portIndex value exceeds Integer.MAX_VALUE-1: " + portIndex);
      }
      CompositeOutputPort port = outputPorts.get(portIndex.intValue());
      if (toPortKind == PortKind.Input) {
        port.fixOutgoingReverseConnection(
            oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
      } else {
        port.fixIncomingReverseConnection(
            oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
      }
    }
  }

  public void addPrimitiveOperator(PrimitiveOperatorBase oper) {
    primitiveOperators.add(oper);
  }

  public void computeCCReachabilityGraph(
      BigInteger portIndex,
      PortKind portKind,
      CCRegion ccRegion,
      Set<PrimitiveOperatorBase> operatorsSeen,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("computeCCReachabilityGraph: in composite " + getFullyQualifiedName());
    }
    CompositePort port;
    if (portIndex.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) >= 0) {
      throw SPLLogicalModel.traceAndReturnException(
          "Internal error - portIndex value exceeds Integer.MAX_VALUE-1: " + portIndex);
    }

    if (portKind == PortKind.Input) {
      if (portIndex.compareTo(BigInteger.valueOf(inputPorts.size())) >= 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      port = inputPorts.get(portIndex.intValue());
    } else {
      if (portIndex.compareTo(BigInteger.valueOf(outputPorts.size())) >= 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      port = outputPorts.get(portIndex.intValue());
    }
    port.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    try {
      dumper.line(
          "<compositeOperInstance compositeOperIndex=\""
              + getDefinitionIndex().toString()
              + "\" index=\""
              + getIndex().toString()
              + "\" name=\""
              + getName()
              + "\">");
    } catch (SAMInternalErrorException e) {
      e.printStackTrace();
    }
    dumper.indent();
    List<Annotation> annotations = getAnnotations();
    if (annotations != null && !annotations.isEmpty()) {
      dumper.line("<annotations>");
      dumper.indent();
      dumper.append(annotations);
      dumper.outdent();
      dumper.line("</annotations>");
    }
    if (inputPorts.size() > 0) {
      dumper.line("<inputPorts>");
      dumper.indent();
      dumper.append(inputPorts);
      dumper.outdent();
      dumper.line("</inputPorts>");
    }
    if (outputPorts.size() > 0) {
      dumper.line("<outputPorts>");
      dumper.indent();
      dumper.append(outputPorts);
      dumper.outdent();
      dumper.line("</outputPorts>");
    }
    dumper.line("<compositeOperatorInstances>");
    dumper.indent();
    dumper.append(compositeOperators);
    dumper.outdent();
    dumper.line("</compositeOperatorInstances>");
    dumper.line("<primitiveOperatorInstances>");
    dumper.indent();
    dumper.append(primitiveOperators);
    dumper.outdent();
    dumper.line("</primitiveOperatorInstances>");
    dumper.outdent();
    dumper.line("</compositeOperInstance>");
    return dumper.toString();
  }

  public void populateNodes(
      TopologyApplication ta,
      List<TopologyNode> nodes,
      Map<BigInteger, TopologyNode> nodeMap,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (CompositeOperator comp : compositeOperators) {
      comp.populateNodes(ta, nodes, nodeMap, model);
    }
    for (Operator prim : primitiveOperators) {
      if (prim.isExportOperator()) {
        // We need to do this prior to populate connections is called
        prim.asExportOperator().evaluateExportedStreamIntrinsics();
        continue;
      }
      if (prim.isImportOperator() || prim.isSplitterOperator() || prim.isMergerOperator()) {
        continue;
      }
      TopologyNode node = new TopologyNode(ta, null, model.isForFormalSubmitjob());
      nodes.add(node);
      prim.populateNode(node, model);
      nodeMap.put(prim.getIndex(), node);
    }
  }

  public void populateConnections(Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (CompositeOperator comp : compositeOperators) {
      comp.populateConnections(nodeMap, model);
    }
    for (PrimitiveOperatorBase prim : primitiveOperators) {
      prim.populateConnections(nodeMap, model);
    }
  }

  public void injectCCAnnotations() throws SAMInternalErrorException {
    for (CompositeOperator comp : compositeOperators) {
      comp.injectCCAnnotations();
    }
    for (PrimitiveOperatorBase oper : primitiveOperators) {
      oper.injectCCAnnotations();
    }
  }

  public void findStartOperators(SPLLogicalModel model) throws SAMInternalErrorException {
    for (CompositeOperator op : compositeOperators) {
      op.findStartOperators(model);
    }
    for (PrimitiveOperatorBase op : primitiveOperators) {
      op.findStartOperators(model);
    }
  }
}
