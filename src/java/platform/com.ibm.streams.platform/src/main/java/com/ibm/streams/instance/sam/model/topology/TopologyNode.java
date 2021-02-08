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

package com.ibm.streams.instance.sam.model.topology;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNullAndValidate;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsPositionIndex;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkNotNullAndValidate;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IDotGraph;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.OperInstanceInputPortsType;
import com.ibm.streams.platform.services.OperInstanceOutputPortsType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.streams.platform.services.PhysParallelRegionType;
import com.ibm.streams.platform.services.PhysParallelRegionsType;
import com.ibm.streams.platform.services.RuntimeConstantsType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.SortedSet;
import org.apache.commons.lang.builder.EqualsBuilder;

public class TopologyNode extends TopologyObject
    implements Comparable<TopologyNode>,
        IDotGraph,
        Validator,
        IXmlSerializer<PhysOperInstanceType> {

  private BigInteger appId;

  private BigInteger index;
  private BigInteger logicalIndex;

  private String _class;
  private String name;
  private String logicalName;
  private Boolean restartable;
  private Boolean relocatable;
  private BigInteger operDefinitionIndex;
  private OperatorRuntimeType operRuntimeType;
  private String operKind;
  private TraceLevel traceLevel = TraceLevel.ERROR;
  private String wrapper = "";

  private List<BigInteger> customMetricIndices = new ArrayList<>();

  private List<Annotation> annotations = new ArrayList<>();
  private List<String> capabilities = new ArrayList<>();
  private boolean isInCCRegion;

  // Parallel Reference Information
  private List<ParallelRegion> parallelRegions = null;

  // Partition & Resources Information
  private Resources resources;
  private Partitions partitions;

  // "Black Box" Data
  private DeploymentDescriptor deploymentDesc;

  @SuppressWarnings("unused")
  private TopologyNode() {}

  public TopologyNode(TopologyApplication ta, BigInteger index, boolean genId) {
    this.tracker = ta.getTopologyTracker();
    this.index = index;
    this.appId = ta.getId();
    if (this.index != null) {
      build();
    }
  }

  @Override
  public void build() {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("appId=" + appId.toString() + ", index=" + index.toString());
    }
    tracker.registerTopologyNode(appId, this);
  }

  private void unbuild() {
    tracker.unregisterTopologyNode(appId, index);
  }

  public void deepUnbuild() {
    for (TopologyNodeInputPort iport : getInputPorts().values()) {
      iport.deepUnbuild();
    }
    for (TopologyNodeOutputPort oport : getOutputPorts().values()) {
      oport.deepUnbuild();
    }

    unbuild();
  }

  // for recovery
  public void populateTransientFields() {
    this.tracker = SAMStaticContext.getTopologyTracker();
  }

  public boolean isInCCRegion() {
    return isInCCRegion;
  }

  public void setInCCRegion(boolean isInCCRegion) {
    this.isInCCRegion = isInCCRegion;
  }

  public String getLogicalName() {
    return logicalName;
  }

  public void setLogicalName(String logicalName) {
    this.logicalName = logicalName;
  }

  public List<BigInteger> getCustomMetrics() {
    return customMetricIndices;
  }

  public BigInteger getIndex() {
    return index;
  }

  public void setIndex(BigInteger index) {
    this.index = index;
    if (this.index != null) build();
  }

  public void transferOwnership(
      BigInteger appId, BigInteger index, Map<BigInteger, BigInteger> hostpoolIndexFix) {
    unbuild();
    this.appId = appId;
    this.index = index;
    resources.transferOwnership(hostpoolIndexFix);
    build();
  }

  public BigInteger getLogicalIndex() {
    return logicalIndex;
  }

  public void setLogicalIndex(BigInteger logicalIndex) {
    this.logicalIndex = logicalIndex;
  }

  public String get_class() {
    return _class;
  }

  public void set_class(String _class) {
    this._class = _class;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public boolean isRestartable() {
    return restartable.booleanValue();
  }

  public void setRestartable(boolean restartable) {
    this.restartable = Boolean.valueOf(restartable);
  }

  public boolean isRelocatable() {
    return relocatable.booleanValue();
  }

  public void setRelocatable(boolean relocatable) {
    this.relocatable = Boolean.valueOf(relocatable);
  }

  public String getTracing() {
    return traceLevel.toString();
  }

  public Map<BigInteger, TopologyNodeOutputPort> getOutputPorts() {
    SortedSet<TopologyNodeOutputPort> tnops =
        tracker.getTopologyNodeOutputPorts(getApplicationId(), getIndex());
    Map<BigInteger, TopologyNodeOutputPort> map = new HashMap<>();
    for (TopologyNodeOutputPort tnop : tnops) {
      map.put(tnop.getIndex(), tnop);
    }
    return map;
  }

  public Map<BigInteger, TopologyNodeInputPort> getInputPorts() {
    SortedSet<TopologyNodeInputPort> tnips =
        tracker.getTopologyNodeInputPorts(getApplicationId(), getIndex());
    Map<BigInteger, TopologyNodeInputPort> map = new HashMap<>();
    for (TopologyNodeInputPort tnip : tnips) {
      map.put(tnip.getIndex(), tnip);
    }
    return map;
  }

  public List<ParallelRegion> getParallelRegions() {
    return this.parallelRegions;
  }

  public void setParallelRegions(List<ParallelRegion> parallelRegions) {
    this.parallelRegions = parallelRegions;
  }

  public boolean isInParallelRegion(String name) {
    if (parallelRegions == null) {
      return false;
    }
    for (ParallelRegion pr : parallelRegions) {
      if (pr.getName().equals(name)) {
        return true;
      }
    }
    return false;
  }

  public Resources getResources() {
    return resources;
  }

  public void setResources(Resources resources) {
    this.resources = resources;
  }

  public Partitions getPartitions() {
    return partitions;
  }

  public void setPartitions(Partitions partitions) {
    this.partitions = partitions;
  }

  public DeploymentDescriptor getDeploymentDesc() {
    return deploymentDesc;
  }

  public void setDeploymentDesc(DeploymentDescriptor deploymentDesc) {
    this.deploymentDesc = deploymentDesc;
  }

  public List<Annotation> getAnnotations() {
    return annotations;
  }

  public BigInteger getApplicationId() {
    return appId;
  }

  public String toDotGraph() {
    String graph = name + "[color=black] " + "[label=\"{ { ";
    // ((resources != null) ? resources.toDotGraph() : "") +
    // ((partitions != null) ? partitions.toDotGraph() : "") +
    // "[[";
    Iterator<TopologyNodeInputPort> iterIp = getInputPorts().values().iterator();
    while (iterIp.hasNext()) {
      TopologyNodeInputPort tnip = iterIp.next();
      graph += "<i" + tnip.getIndex() + "/>";
      graph += tnip.toDotGraph();
      if (iterIp.hasNext()) graph += " | ";
    }
    graph += " } | { \\N " + getIndex() + " } | { ";
    Iterator<TopologyNodeOutputPort> iterOp = getOutputPorts().values().iterator();
    while (iterOp.hasNext()) {
      TopologyNodeOutputPort tnop = iterOp.next();
      graph += "<o" + tnop.getIndex() + "/>";
      graph += tnop.toDotGraph();
      if (iterOp.hasNext()) graph += " | ";
    }
    graph += " } }\"]\n";

    iterIp = getInputPorts().values().iterator();
    iterOp = getOutputPorts().values().iterator();

    // Connections are direction based OP->IP
    while (iterOp.hasNext()) {
      for (TopologyNodeConnection tnc : iterOp.next().getOperConnections())
        graph +=
            tnc.getSource().getOutputPort().getNodeName()
                + ":"
                + tnc.getSource().getOutputPort().getIndex()
                + " -> "
                + tnc.getDestination().getInputPort().getNodeName()
                + ":"
                + tnc.getDestination().getInputPort().getIndex()
                + ";\n";
    }

    return graph;
  }

  static <T> void print(Map<BigInteger, T> m, ModelDumper dumper) {
    Set<BigInteger> keys = m.keySet();
    for (BigInteger i : keys) {
      dumper.append(m.get(i).toString());
    }
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine(
        "<node index=\""
            + index
            + "\" logicalIndex=\""
            + logicalIndex
            + "\" name=\""
            + name
            + "\" logicalName=\""
            + logicalName
            + "\" class=\""
            + _class);
    dumper.append(
        "\" restartable=\""
            + restartable
            + "\" relocatable=\""
            + relocatable
            + "\" traceLevel=\""
            + traceLevel.toString()
            + "\"");
    dumper.append(">").end();
    if (parallelRegions != null) {
      dumper.line("<parallelRegions>");
      dumper.indent();
      dumper.append(parallelRegions);
      dumper.outdent();
      dumper.line("</parallelRegions>");
    }
    dumper.append(resources.toString());
    if (partitions != null) {
      dumper.append(partitions.toString());
    }
    dumper.indent();
    dumper.append(deploymentDesc.toString());
    if (customMetricIndices.size() > 0) {
      dumper.line("<customMetricIndices>");
      dumper.indent();
      for (BigInteger i : customMetricIndices) {
        dumper.line("<customMetricIndex>" + i + "</customMetricIndex>");
      }
      dumper.outdent();
      dumper.line("</customMetricIndices>");
    }
    dumper.line("<inputPorts>");
    dumper.indent();
    dumper.append(getInputPorts());
    dumper.outdent();
    dumper.line("</inputPorts>");
    dumper.line("<outputPorts>");
    dumper.indent();
    dumper.append(getOutputPorts());
    dumper.outdent();
    dumper.line("</outputPorts>");
    if (annotations != null) {
      dumper.line("<annotations>");
      dumper.indent();
      dumper.append(annotations);
      dumper.outdent();
      dumper.line("</annotations>");
    }
    if (!capabilities.isEmpty()) {
      dumper.line("<capabilities>");
      dumper.indent();
      for (String s : capabilities) {
        dumper.line("<capability>" + s + "</capability>");
      }
      dumper.outdent();
      dumper.line("</capabilities>");
    }
    dumper.outdent();
    dumper.line("</node>");
    return dumper.toString();
  }

  public PhysOperInstanceType toXml() {
    PhysOperInstanceType xml = new PhysOperInstanceType();

    xml.setSharedObject(getDeploymentDesc().getSharedObject().toXml());

    xml.setResources(getResources().toXml());
    // TODO: Handle Annotations
    // TODO: Implement Optional Fields
    // xml.setProfiling(getDeploymentDesc().getProfiling().toXml());
    if (getDeploymentDesc().getCheckpoint() != null) {
      xml.setCheckpointing(getDeploymentDesc().getCheckpoint().toXml());
    }

    RuntimeConstantsType rcsXml = new RuntimeConstantsType();
    if (getDeploymentDesc().getRuntimeConstants() != null) {
      List<RuntimeConstant> runtimeConstants = getDeploymentDesc().getRuntimeConstants();
      for (RuntimeConstant rc : runtimeConstants) {
        rcsXml.getRuntimeConstant().add(rc.toXml());
      }
    }
    xml.setRuntimeConstants(rcsXml);

    OperInstanceInputPortsType ipsXml = new OperInstanceInputPortsType();
    for (Map.Entry<BigInteger, TopologyNodeInputPort> entry : getInputPorts().entrySet()) {
      ipsXml.getInputPort().add(entry.getKey().intValue(), entry.getValue().toXml());
    }
    xml.setInputPorts(ipsXml);

    OperInstanceOutputPortsType opsXml = new OperInstanceOutputPortsType();
    for (Map.Entry<BigInteger, TopologyNodeOutputPort> entry : getOutputPorts().entrySet()) {
      opsXml.getOutputPort().add(entry.getKey().intValue(), entry.getValue().toXml());
    }
    xml.setOutputPorts(opsXml);

    xml.setIndex(getIndex());
    xml.setClazz(get_class());
    xml.setName(getName());
    xml.setDefinitionIndex(getOperDefinitionIndex());
    xml.setSingleThreadedOnInputs(getDeploymentDesc().isSingleThreadedOnInputs());
    xml.setSingleThreadedOnOutputs(getDeploymentDesc().isSingleThreadedOnOutputs());
    xml.setRestartable(isRestartable());
    xml.setRelocatable(isRelocatable());

    // PhysOperInstanceType Fields
    xml.setLogicalName(getLogicalName());
    xml.setLogicalIndex(
        getLogicalIndex()); // TODO: Do we use the one out of ParallelRegion or the one here?
    if (getParallelRegions() != null) {
      List<ParallelRegion> prs = getParallelRegions();
      PhysParallelRegionsType pRegions = new PhysParallelRegionsType();
      for (ParallelRegion pr : prs) {

        PhysParallelRegionType pRegion = new PhysParallelRegionType();
        pRegion.setLocalChannelIndex(pr.getLocalChannelIndex().intValue());
        pRegion.setGlobalChannelIndex(pr.getGlobalChannelIndex().intValue());
        pRegion.setLocalMaxChannels(pr.getLocalMaxChannels());
        pRegion.setGlobalMaxChannels(pr.getGlobalMaxChannels());
        pRegion.setRegionIndex(pr.getRegionIndex());
        pRegions.getParallelRegion().add(pRegion);
      }
      xml.setParallelRegions(pRegions);
    }
    xml.setToolkitIndex(getDeploymentDesc().getToolkitIndex());

    for (Annotation a : getAnnotations()) {
      xml.getAnnotation().add(a.toXml());
    }

    List<String> jvmArgs = getDeploymentDesc().getJvmArguments();
    if (jvmArgs != null) {
      com.ibm.streams.platform.services.JvmArgumentsType xmlArgs =
          new com.ibm.streams.platform.services.JvmArgumentsType();
      for (String arg : jvmArgs) {
        xmlArgs.getJvmArgument().add(arg);
      }
      xml.setJvmArguments(xmlArgs);
    }

    return xml;
  }

  public void validate() throws Exception {
    checkNotNull(index);
    checkNotNull(logicalIndex);
    checkNotNull(_class);
    checkNotNull(name);
    checkNotNull(restartable);
    checkNotNull(relocatable);

    checkContentsPositionIndex(getOutputPorts().keySet());
    checkContentsNotNullAndValidate(getOutputPorts().values());

    checkContentsPositionIndex(getInputPorts().keySet());
    checkContentsNotNullAndValidate(getInputPorts().values());

    checkContentsNotNull(customMetricIndices);

    //      validateIfPresent(parallelRegions);
    checkNotNullAndValidate(resources);
    checkNotNullAndValidate(deploymentDesc);
  }

  public int compareTo(TopologyNode tn) {
    if (this.equals(tn)) return 0;

    return getIndex().compareTo(tn.getIndex());
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == null) {
      return false;
    }
    if (obj == this) {
      return true;
    }
    if (obj.getClass() != getClass()) {
      return false;
    }
    TopologyNode rhs = (TopologyNode) obj;
    return new EqualsBuilder()
        .append(getIndex(), rhs.getIndex())
        .append(getName(), rhs.getName())
        .append(getResources(), rhs.getResources())
        .append(getPartitions(), rhs.getPartitions())
        .append(isRelocatable(), rhs.isRelocatable())
        .append(isRestartable(), rhs.isRestartable())
        .append(getParallelRegions(), rhs.getParallelRegions())
        .isEquals();
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        getIndex(),
        getName(),
        getResources(),
        getPartitions(),
        isRelocatable(),
        isRestartable(),
        getParallelRegions());
  }

  public TraceLevel getTraceLevel() {
    return traceLevel;
  }

  public void setTraceLevel(TraceLevel traceLevel) {
    this.traceLevel = traceLevel;
  }

  public BigInteger getOperDefinitionIndex() {
    return operDefinitionIndex;
  }

  public void setOperDefinitionIndex(BigInteger operDefinitionIndex) {
    this.operDefinitionIndex = operDefinitionIndex;
  }

  public OperatorRuntimeType getOperRuntimeType() {
    return operRuntimeType;
  }

  public void setOperRuntimeType(OperatorRuntimeType operRuntimeType) {
    this.operRuntimeType = operRuntimeType;
  }

  public String getOperKind() {
    return operKind;
  }

  public void setOperKind(String operKind) {
    this.operKind = operKind;
  }

  public List<String> getCapabilities() {
    return capabilities;
  }

  public String getWrapper() {
    return wrapper;
  }

  public void setWrapper(String wrapper) {
    this.wrapper = wrapper;
  }
}
