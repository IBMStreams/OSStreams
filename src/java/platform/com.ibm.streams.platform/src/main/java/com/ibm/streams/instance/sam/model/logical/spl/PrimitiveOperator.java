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
import com.ibm.streams.instance.sam.model.topology.*;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class PrimitiveOperator extends PrimitiveOperatorBase {
  protected final List<PrimitiveInputPort> inputPorts = new ArrayList<>();
  protected final List<PrimitiveOutputPort> outputPorts = new ArrayList<>();
  private final BigInteger physicalIndex;
  private boolean relocatable = true;
  private boolean restartable = true;
  private String _class = "";
  private String sharedObject = "";
  private String wrapper = "";
  private boolean singleThreadedOnInputs = true;
  private boolean singleThreadedOnOutputs = true;
  private List<RuntimeConstant> runtimeConstants;
  private Resources resources = null;
  private final Set<String> coLocations = new HashSet<>();
  private final Set<String> exLocations = new HashSet<>();
  private final Map<String, Set<String>> softSegregatedLocations = new HashMap<>();
  private Boolean isolation = false;
  private List<String> jvmArguments = null;
  private TraceLevel traceLevel;
  private boolean optimized = false;
  private Checkpointing checkpointing;

  public PrimitiveOperator(
      String name,
      BigInteger sourceIndex,
      BigInteger index,
      String description,
      List<Annotation> annotations,
      CompositeOperator owningComp,
      BigInteger definitionIndex,
      Boolean optimized,
      Boolean restartable,
      Boolean relocatable,
      String sharedObject,
      String wrapper,
      TraceLevel traceLevel,
      Boolean singleThreadedOnInputs,
      Boolean singleThreadedOnOutputs,
      String _class,
      Resources resources,
      Checkpointing checkpointing,
      List<RuntimeConstant> runtimeConstants,
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
    model.getPrimitiveOperatorMap().put(index, this);
    physicalIndex = index;
    model.setMaxPrimitiveOperIndex(getIndex());
    model.getPrimitiveOperatorMap().put(physicalIndex, this);
    model.getOperatorMap().put(getIndex(), this);
    this.optimized = optimized;
    this.restartable = restartable;
    this.relocatable = relocatable;
    this.sharedObject = sharedObject;
    this.wrapper = wrapper;
    this.traceLevel = traceLevel;
    this.singleThreadedOnInputs = singleThreadedOnInputs;
    this.singleThreadedOnOutputs = singleThreadedOnOutputs;
    this._class = _class;
    this.resources = resources;
    this.checkpointing = checkpointing;
    this.runtimeConstants = runtimeConstants;
  }

  public PrimitiveOperator(
      PrimitiveOperator other,
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
        "PrimitiveOperator ctor: replicating primitive index  "
            + other.getIndex().toString()
            + " to index "
            + getIndex().toString()
            + " for local channel "
            + localChannelIndex);

    physicalIndex = BigInteger.valueOf(model.getPrimitiveOperatorMap().size());
    relocatable = other.isRelocatable();
    restartable = other.isRestartable();
    _class = other.get_class();
    sharedObject = other.getSharedObject();
    wrapper = other.getWrapper();
    checkpointing = other.getCheckpointing();
    singleThreadedOnInputs = other.isSingleThreadedOnInputs();
    singleThreadedOnOutputs = other.isSingleThreadedOnOutputs();
    runtimeConstants = new ArrayList<RuntimeConstant>();
    runtimeConstants.addAll(other.getRuntimeConstants());
    traceLevel = other.getTraceLevel();
    optimized = other.isOptimized();
    if (other.hasJVMArguments()) {
      getJvmArguments().addAll(other.getJvmArguments());
    }

    if (other.hasResources()) {
      // We are replicating an operator. We need to copy over any resource constraints.
      // Copy the resource for now.
      resources = other.getResources();
    }

    coLocations.addAll(other.getCoLocations());
    exLocations.addAll(other.getExLocations());
    softSegregatedLocations.putAll(other.softSegregatedLocations);
    isolation = other.getIsolation();

    // replicate the input and output ports
    for (PrimitiveInputPort modelPrimitiveInputPort : other.inputPorts) {
      PrimitiveInputPort newPort = new PrimitiveInputPort(modelPrimitiveInputPort, this);
      inputPorts.add(newPort);
    }

    for (PrimitiveOutputPort modelPrimitiveOutputPort : other.outputPorts) {
      PrimitiveOutputPort newPort = new PrimitiveOutputPort(modelPrimitiveOutputPort, this);
      outputPorts.add(newPort);
    }

    model.getPrimitiveOperatorMap().put(physicalIndex, this);
    model.getOperatorMap().put(getIndex(), this);
  }

  private void performIntrinsicFunctionSubstitutions(
      Set<String> locations,
      List<com.ibm.streams.instance.sam.model.topology.ParallelRegion> parallelRegions) {
    Set<String> tempLocations = new HashSet<>();
    for (String str : locations) {
      tempLocations.add(
          SPLLogicalModel.performIntrinsicFunctionSubstitutions(str, parallelRegions));
    }
    locations.clear();
    locations.addAll(tempLocations);
  }

  @Override
  public void performParallelResourceTransforms(
      SPLLogicalModel model, Set<String> nonParallelCoLocations)
      throws SAMInternalErrorException, SubmissionParameterError {
    List<com.ibm.streams.instance.sam.model.topology.ParallelRegion> parallelRegions =
        new ArrayList<>();
    if (isInParallelRegion()) {
      gatherParallelRegions(parallelRegions);
    }

    // Patch any host hostname/colocation/exlocation strings
    String hostLocation = resources.getHostLocation();
    if (hostLocation != null) {
      hostLocation =
          SPLLogicalModel.performIntrinsicFunctionSubstitutions(
              resources.getHostLocation(), parallelRegions);
    }
    Set<String> hostCoLocations = new HashSet<>();
    for (String str : resources.getCoLocations()) {
      hostCoLocations.add(
          SPLLogicalModel.performIntrinsicFunctionSubstitutions(str, parallelRegions));
    }
    Set<String> hostExLocations = new HashSet<>();
    for (String str : resources.getExLocations()) {
      hostExLocations.add(
          SPLLogicalModel.performIntrinsicFunctionSubstitutions(str, parallelRegions));
    }

    resources =
        new Resources(
            hostLocation,
            resources.getPoolLocation(),
            hostCoLocations,
            hostExLocations,
            resources.isIsolation());

    performIntrinsicFunctionSubstitutions(coLocations, parallelRegions);
    performIntrinsicFunctionSubstitutions(exLocations, parallelRegions);

    if (isInParallelRegion()) {

      List<BigInteger> channelIndexes = new ArrayList<>();
      for (com.ibm.streams.instance.sam.model.topology.ParallelRegion pr : parallelRegions) {
        channelIndexes.add(pr.getGlobalChannelIndex());
      }

      ParallelRegion containingParallelRegion = getContainingParallelRegion();
      if (!containingParallelRegion.getReplicateTags().isEmpty()) {
        final PoolLocation poolLocation = resources.getPoolLocation();
        if (poolLocation != null) {
          // A hostpool was specified.  Is this a pool that needs to be replicated?
          BigInteger originalPoolIndex = poolLocation.getPoolIndex();
          if (model.getHostpoolMap().containsKey(originalPoolIndex)) {
            LogicalHostpool originalHostPool = model.getHostpoolMap().get(originalPoolIndex);
            Set<String> intersectTags = new HashSet<String>(originalHostPool.getTags());
            intersectTags.retainAll(containingParallelRegion.getReplicateTags());
            if (!intersectTags.isEmpty()) {
              LogicalHostpool replicaHostPool =
                  originalHostPool.findOrCreateReplica(
                      containingParallelRegion.getIndex(), channelIndexes, intersectTags, model);
              resources.setPoolLocation(
                  new PoolLocation(replicaHostPool.getIndex(), poolLocation.getInpoolIndex()));
            }
          } else {
            throw SPLLogicalModel.traceAndReturnException(
                "Internal error: invalid hostpool index " + originalPoolIndex.toString());
          }
        }
      }
    }
  }

  @Override
  public void performCCResourceTransforms() {
    if (getCCInfo().isInCCRegion()) {
      BigInteger regionId = getCCInfo().getRegions().get(0).getIndex();
      Set<String> set = new HashSet<>();
      set.add("region" + regionId.toString());
      softSegregatedLocations.put("consistentRegionId", set);
    }
  }

  public TraceLevel getTraceLevel() {
    return traceLevel;
  }

  public boolean hasJVMArguments() {
    return jvmArguments != null;
  }

  public List<String> getJvmArguments() {
    if (jvmArguments == null) {
      jvmArguments = new ArrayList<>();
    }
    return jvmArguments;
  }

  public boolean isSingleThreadedOnInputs() {
    return singleThreadedOnInputs;
  }

  public void setSingleThreadedOnInputs(boolean singleThreadedOnInputs) {
    this.singleThreadedOnInputs = singleThreadedOnInputs;
  }

  public void setTraceLevel(TraceLevel traceLevel) {
    this.traceLevel = traceLevel;
  }

  public void setResources(Resources resources) {
    this.resources = resources;
  }

  public List<RuntimeConstant> getRuntimeConstants() {
    return runtimeConstants;
  }

  public boolean isSingleThreadedOnOutputs() {
    return singleThreadedOnOutputs;
  }

  public void setSingleThreadedOnOutputs(boolean singleThreadedOnOutputs) {
    this.singleThreadedOnOutputs = singleThreadedOnOutputs;
  }

  public boolean isRestartable() {
    return restartable;
  }

  public void setRestartable(boolean restartable) {
    this.restartable = restartable;
  }

  public boolean isRelocatable() {
    return relocatable;
  }

  public void setRelocatable(boolean isRelocatable) {
    this.relocatable = isRelocatable;
  }

  public String get_Class() {
    return _class;
  }

  public void set_Class(String _class) {
    this._class = _class;
  }

  @Override
  public PrimitiveOperatorBase replicate(
      CompositeOperator owningComp, BigInteger localChannelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    PrimitiveOperatorBase replica =
        new PrimitiveOperator(this, localChannelIndex, owningComp, model);
    return replica;
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
  public void createSplittersAndMergers(SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    for (PrimitiveInputPort modelPrimitiveInputPort : inputPorts) {
      modelPrimitiveInputPort.createSplitters(getParallelRegion(), model);
    }
    for (PrimitiveOutputPort modelPrimitiveOutputPort : outputPorts) {
      modelPrimitiveOutputPort.createMergers(getParallelRegion(), model);
    }
  }

  @Override
  public void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (PrimitiveInputPort modelPrimitiveInputPort : inputPorts) {
      modelPrimitiveInputPort.addReverseConnections(channelIndex, model);
    }
    for (PrimitiveOutputPort modelPrimitiveOutputPort : outputPorts) {
      modelPrimitiveOutputPort.addReverseConnections(channelIndex, model);
    }
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
      PrimitiveInputPort port = inputPorts.get(portIndex.intValue());
      port.fixConnection(
          oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
    } else {
      if (portIndex.compareTo(BigInteger.valueOf(outputPorts.size())) >= 0) {
        throw SPLLogicalModel.traceAndReturnException("Internal error: invalid port index");
      }
      if (portIndex.compareTo(BigInteger.valueOf(Integer.MAX_VALUE)) >= 0) {
        throw SPLLogicalModel.traceAndReturnException(
            "Internal error - portIndex value exceeds Integer.MAX_VALUE-1: " + portIndex);
      }
      PrimitivePort port = outputPorts.get(portIndex.intValue());
      port.fixConnection(
          oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex, toPortKind);
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
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("Adding " + getFullyQualifiedName() + " to region " + ccRegion.getIndex());
    }
    ccRegion.addRegionEntry(this, model);
    if (getCCInfo().isEndCutRegion()) {
      return;
    }
    for (PrimitiveOutputPort port : outputPorts) {
      port.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
  }

  @Override
  public BigInteger getPhysicalIndex() throws SAMInternalErrorException {
    return physicalIndex;
  }

  public List<PrimitiveInputPort> getInputPorts() {
    return this.inputPorts;
  }

  public List<PrimitiveOutputPort> getOutputPorts() {
    return this.outputPorts;
  }

  @Override
  public boolean isPrimitive() {
    return true;
  }

  @Override
  public PrimitiveOperator asPrimitive() {
    return this;
  }

  @Override
  public void populateNode(TopologyNode node, SPLLogicalModel model)
      throws SAMInternalErrorException {
    super.populateNode(node, model);

    // traceLevel set
    if (model.getConfigTraceLevel() == null) {
      node.setTraceLevel(traceLevel);
    } else {
      // Set "tracing" with submission config set.
      // By design, this will overwrite whatever setting of tracing from original spl source.
      node.setTraceLevel(model.getConfigTraceLevel());
    }

    // wrapper
    node.setWrapper(wrapper);

    List<com.ibm.streams.instance.sam.model.topology.RuntimeConstant> topRuntimeConstants = null;
    if (runtimeConstants.size() > 0) {
      topRuntimeConstants = new ArrayList<>();
      for (RuntimeConstant rc : runtimeConstants) {
        com.ibm.streams.instance.sam.model.topology.RuntimeConstant topRC =
            new com.ibm.streams.instance.sam.model.topology.RuntimeConstant(
                rc.getName(), rc.getValue());
        topRuntimeConstants.add(topRC);
      }
    }
    node.setIndex(physicalIndex);
    PrimitiveOperatorDefinition operDef =
        model.getPrimitiveOperatorDefs().get(getDefinitionIndex());
    BigInteger toolkitIndex = operDef.getToolkitIndex();
    DeploymentDescriptor dDesc =
        new DeploymentDescriptor(
            new SharedObject(sharedObject, wrapper),
            topRuntimeConstants,
            toolkitIndex,
            isOptimized(),
            isSingleThreadedOnInputs(),
            isSingleThreadedOnOutputs(),
            checkpointing);
    if (jvmArguments != null) {
      dDesc.setJvmArguments(jvmArguments);
    }
    ThreadingModel tm = getThreadingModel(model);
    if (tm != null) {
      node.getAnnotations().add(tm.getAnnotation());
    }

    node.setDeploymentDesc(dDesc);
    node.setRelocatable(relocatable);
    node.setRestartable(restartable);
    node.set_class(_class);
    node.setResources(resources);

    Partitions partitions = new Partitions();
    partitions.getCoLocations().addAll(coLocations);
    partitions.getExLocations().addAll(exLocations);
    partitions.setIsolation(isolation);
    partitions.getSoftSegregatedLocations().putAll(softSegregatedLocations);
    if (jvmArguments != null) {
      Map<String, Set<String>> provisionalLocations = partitions.getProvisionalLocations();
      Set<String> args = new HashSet<>();
      // A java op with no args cannot be fused with a java op with args
      if (jvmArguments.isEmpty()) {
        args.add("");
      } else {
        for (String arg : jvmArguments) {
          args.add(arg);
        }
      }
      provisionalLocations.put("jvmArgs", args);
    }
    node.setPartitions(partitions);

    if (operDef.getCustomMetricIndices().size() > 0) {
      List<BigInteger> topOperMetricIndices = node.getCustomMetrics();
      for (BigInteger index : operDef.getCustomMetricIndices()) {
        topOperMetricIndices.add(index);
      }
    }
    Map<BigInteger, TopologyNodeInputPort> iPorts = node.getInputPorts();
    for (PrimitiveInputPort iPort : inputPorts) {
      TopologyNodeInputPort port = new TopologyNodeInputPort(getName(), node);
      iPort.populatePort(port);
      iPorts.put(iPort.getIndex(), port);
    }
    Map<BigInteger, TopologyNodeOutputPort> oPorts = node.getOutputPorts();
    for (PrimitiveOutputPort oPort : outputPorts) {
      TopologyNodeOutputPort port = new TopologyNodeOutputPort(getName(), node);
      oPort.populatePort(port);
      oPorts.put(oPort.getIndex(), port);
    }
    // If the node is in a parallel region, populate the parallelRegion
    if (getIsInParallelRegion()) {
      List<com.ibm.streams.instance.sam.model.topology.ParallelRegion> parallelRegions =
          new ArrayList<>();
      gatherParallelRegions(parallelRegions);
      node.setParallelRegions(parallelRegions);
    }
    node.getCapabilities().addAll(operDef.getCapabilities());
  }

  private ThreadingModel getThreadingModel(SPLLogicalModel model) throws SAMInternalErrorException {
    return getOwningComposite().getThreadingModel(model);
  }

  @Override
  public void populateConnections(Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (PrimitiveOutputPort oPort : outputPorts) {
      oPort.populateConnections(nodeMap, model);
    }
    for (PrimitiveInputPort iPort : inputPorts) {
      iPort.populateConnections(nodeMap, model);
    }
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    try {
      dumper.line(
          "<primitiveOperInstance index =\""
              + getIndex()
              + "\" logicalIndex=\""
              + getLogicalIndex()
              + "\" physicalIndex=\""
              + physicalIndex
              + "\" definitionIndex=\""
              + getDefinitionIndex().toString()
              + "\" localChannelIndex=\""
              + getLocalChannelIndex().toString()
              + "\" name=\""
              + getFullyQualifiedName()
              + "\" sharedObject=\""
              + sharedObject
              + "\" traceLevel=\""
              + traceLevel.toString()
              + "\" restartable=\""
              + restartable
              + "\" relocatable=\""
              + relocatable
              + "\" isolation=\""
              + isolation.toString()
              + "\" optimized=\""
              + optimized
              + "\">");
    } catch (SAMInternalErrorException e) {
      e.printStackTrace();
    }
    dumper.indent();
    dumper.append(resources.toString());
    if (!coLocations.isEmpty()) {
      dumper.line("<coLocations>");
      dumper.indent();
      for (String coloc : coLocations) {
        dumper.line("<coLocation colocId=\"" + coloc + "\"/>");
      }
      dumper.outdent();
      dumper.line("</coLocations>");
    }
    if (!exLocations.isEmpty()) {
      dumper.line("<exLocations>");
      dumper.indent();
      if (exLocations.size() > 0) {
        for (String exloc : exLocations) {
          dumper.line("<exLocation exlocId=\"" + exloc + "\"/>");
        }
      }
      dumper.outdent();
      dumper.line("</exLocations>");
    }
    if (!softSegregatedLocations.isEmpty()) {
      dumper.line("<softSegregatedLocations>");
      dumper.indent();
      for (Map.Entry<String, Set<String>> entry : softSegregatedLocations.entrySet()) {
        dumper.line("<" + entry.getKey() + ">");
        dumper.indent();
        for (String soft : entry.getValue()) {
          dumper.line("<softSegregatedLocation locId=\"" + soft + "\"/>");
        }
        dumper.outdent();
        dumper.line("</" + entry.getKey() + ">");
      }
      dumper.outdent();
      dumper.line("</softSegregatedLocations>");
    }

    List<Annotation> annotations = getAnnotations();
    if (annotations != null && !annotations.isEmpty()) {
      dumper.line("<annotations>");
      dumper.indent();
      dumper.append(annotations);
      dumper.outdent();
      dumper.line("</annotations>");
    }
    dumper.line("<runtimeConstants>");
    dumper.indent();
    for (RuntimeConstant rc : runtimeConstants) {
      dumper.append(rc.toString());
    }
    dumper.outdent();
    dumper.line("</runtimeConstants>");
    if (jvmArguments != null) {
      dumper.line("<jvmArguments>");
      dumper.indent();
      for (String arg : jvmArguments) {
        dumper.line("<jvmArgument>" + arg + "</jvmArgument>");
      }
      dumper.outdent();
      dumper.line("</jvmArguments>");
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
    dumper.append(getCCInfo().toString());
    dumper.outdent();
    dumper.line("</primitiveOperInstance>");
    return dumper.toString();
  }

  public void setWrapper(String wrapper) {
    this.wrapper = wrapper;
  }

  public void setSharedObject(String uri) {
    this.sharedObject = uri;
  }

  public void fixConnections(ParallelTransformState state) {
    for (PrimitiveInputPort port : inputPorts) {
      port.fixConnections(state);
    }
    for (PrimitiveOutputPort port : outputPorts) {
      port.fixConnections(state);
    }
  }

  public boolean isOptimized() {
    return optimized;
  }

  public void setOptimized(boolean optimized) {
    this.optimized = optimized;
  }

  public String getWrapper() {
    return wrapper;
  }

  public String getSharedObject() {
    return sharedObject;
  }

  public String get_class() {
    return _class;
  }

  public boolean hasResources() {
    return resources != null;
  }

  public Resources getResources() {
    return resources;
  }

  public Boolean getIsolation() {
    return isolation;
  }

  public void setIsolation(Boolean isolation) {
    this.isolation = isolation;
  }

  public Set<String> getCoLocations() {
    return coLocations;
  }

  public Set<String> getExLocations() {
    return exLocations;
  }

  public void setCheckpointing(Checkpointing checkpointing) {
    this.checkpointing = checkpointing;
  }

  public Checkpointing getCheckpointing() {
    return checkpointing;
  }

  public void setRuntimeConstants(List<RuntimeConstant> runtimeConstants) {
    this.runtimeConstants = runtimeConstants;
  }
}
