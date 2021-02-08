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
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationPrimitiveValue;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

public abstract class Operator {

  private final String name;
  private final BigInteger sourceIndex;
  private final BigInteger index;
  private final BigInteger logicalIndex;
  private final CompositeOperator owningComposite;
  private final boolean isReplica;
  private ParallelRegion parallelRegion;
  private final BigInteger localChannelIndex;
  private List<Annotation> annotations = null;
  private BigInteger definitionIndex;

  public Operator(
      String name,
      BigInteger sourceIndex,
      BigInteger index,
      String description,
      List<Annotation> newAnnotations,
      CompositeOperator owningComp,
      boolean isRep,
      BigInteger definitionIndex) {
    this.name = getRootOperatorName(name);
    this.sourceIndex = sourceIndex;
    this.index = index;
    this.logicalIndex = index;
    this.annotations = newAnnotations;
    this.owningComposite = owningComp;
    this.isReplica = isRep;
    this.definitionIndex = definitionIndex;
    this.parallelRegion = null;
    this.localChannelIndex = BigInteger.ZERO;
  }

  public Operator(
      Operator other, BigInteger index, BigInteger localChannelIndex, CompositeOperator owningComp)
      throws SAMInternalErrorException {
    this.name = getRootOperatorName(other.name);
    this.sourceIndex = other.getSourceIndex();
    this.index = index;
    this.logicalIndex = other.getLogicalIndex();
    this.owningComposite = owningComp;
    this.isReplica = true;
    this.parallelRegion = other.hasParallelRegion() ? other.getParallelRegion() : null;
    this.localChannelIndex = localChannelIndex;
    this.definitionIndex = other.getDefinitionIndex();
    if (!other.getAnnotations().isEmpty()) {
      getAnnotations().addAll(other.getAnnotations());
    }
  }

  public BigInteger getSourceIndex() {
    return sourceIndex;
  }

  public boolean hasParallelRegion() {
    return parallelRegion != null;
  }

  public boolean hasOwningComposite() {
    return this.owningComposite != null;
  }

  public BigInteger getDefinitionIndex() {
    assert (definitionIndex != null);
    return definitionIndex;
  }

  public void setDefinitionIndex(BigInteger definitionIndex) {
    this.definitionIndex = definitionIndex;
  }

  public boolean hasAnnotations() {
    return annotations != null;
  }

  public List<Annotation> getAnnotations() {
    if (annotations == null) {
      annotations = new ArrayList<>();
    }
    return annotations;
  }

  public BigInteger getIndex() {
    return index;
  }

  public boolean isImportOperator() {
    return false;
  }

  public ImportOperator asImportOperator() {
    return null;
  }

  public String getFullyQualifiedName() throws SAMInternalErrorException {
    String fqn = "";
    if (owningComposite != null) {
      fqn = owningComposite.getFullyQualifiedOwningCompositeName();
    }
    if (!fqn.isEmpty()) {
      fqn += ".";
    }
    fqn += getName();
    return fqn;
  }

  public String getFullyQualifiedLogicalName() {
    String fqn = "";
    if (owningComposite != null) {
      fqn = owningComposite.getFullyQualifiedOwningLogicalCompositeName();
    }
    if (!fqn.isEmpty()) {
      fqn += ".";
    }
    fqn += getLogicalName();
    return fqn;
  }

  public String getFullyQualifiedOwningLogicalCompositeName() {
    String fqn = "";
    if (owningComposite != null) {
      fqn = owningComposite.getFullyQualifiedOwningLogicalCompositeName();
      if (!fqn.isEmpty()) {
        fqn += ".";
      }
      fqn += getLogicalName();
    }
    return fqn;
  }

  public String getName() throws SAMInternalErrorException {
    String n = getLogicalName();
    if (parallelRegion != null) {
      StringBuilder sb = new StringBuilder();
      sb.append("[").append(String.valueOf(getGlobalChannelIndex())).append("]");
      n += sb.toString();
    }
    return n;
  }

  public String getFullyQualifiedOwningCompositeName() throws SAMInternalErrorException {
    String fqn = "";
    if (owningComposite != null) {
      fqn = owningComposite.getFullyQualifiedOwningCompositeName();
      if (!fqn.isEmpty()) {
        fqn += ".";
      }
      fqn += getName();
    }
    return fqn;
  }

  public com.ibm.streams.instance.sam.model.topology.ParallelRegion createTopologyParallelRegion()
      throws SAMInternalErrorException {
    return new com.ibm.streams.instance.sam.model.topology.ParallelRegion(
        getGlobalChannelIndex(),
        getLocalChannelIndex(),
        getGlobalMaxChannels(),
        getLocalMaxChannels(),
        parallelRegion.getIndex(),
        getFullyQualifiedLogicalName());
  }

  public void gatherParallelRegions(
      List<com.ibm.streams.instance.sam.model.topology.ParallelRegion> parallelRegions)
      throws SAMInternalErrorException {
    // If this operator is a parallel region, then add it to the array
    if (parallelRegion != null) {
      parallelRegions.add(createTopologyParallelRegion());
    }
    // Call up the containing composite instance hierarchy and add any enclosing parallel regions
    if (owningComposite != null) {
      owningComposite.gatherParallelRegions(parallelRegions);
    }
  }

  public void gatherParallelRegionInfos(List<ParallelRegionInfo> infos)
      throws SAMInternalErrorException {
    // If this operator is a parallel region, then add it to the array
    if (parallelRegion != null) {
      ParallelRegionInfo pri =
          new ParallelRegionInfo(
              getLocalChannelIndex(),
              getLocalMaxChannels(),
              parallelRegion.getIndex(),
              getFullyQualifiedLogicalName());
      infos.add(pri);
    }
    // Call up the containing composite instance hierarchy and add any enclosing parallel regions
    if (owningComposite != null) {
      owningComposite.gatherParallelRegionInfos(infos);
    }
  }

  public boolean getIsInParallelRegion() {
    if (parallelRegion != null) {
      return true;
    }
    if (owningComposite != null) {
      return owningComposite.getIsInParallelRegion();
    }
    return false;
  }

  public ParallelRegion getContainingParallelRegion() throws SAMInternalErrorException {
    if (parallelRegion != null) {
      return parallelRegion;
    }
    if (owningComposite == null) {
      throw SPLLogicalModel.traceAndReturnException(
          "Internal error: containingParallelRegion called on main composite");
    }
    return owningComposite.getContainingParallelRegion();
  }

  public BigInteger getGlobalChannelIndex() throws SAMInternalErrorException {
    List<ParallelRegionInfo> infos = new ArrayList<>();
    gatherParallelRegionInfos(infos);
    return SPLLogicalModel.computeGlobalChannelIndex(infos);
  }

  public BigInteger getGlobalMaxChannels() throws SAMInternalErrorException {
    List<ParallelRegionInfo> infos = new ArrayList<>();
    gatherParallelRegionInfos(infos);
    return SPLLogicalModel.computeGlobalMaxChannels(infos);
  }

  public BigInteger getLocalChannelIndex() {
    if (parallelRegion != null) {
      return localChannelIndex;
    }
    if (owningComposite == null) {
      return BigInteger.valueOf(-1); // getChannel() outside of a parallel region always returns -1
    }
    return owningComposite.getLocalChannelIndex();
  }

  public BigInteger getLocalMaxChannels() throws SAMInternalErrorException {
    if (parallelRegion != null) {
      BigInteger width = null;
      try {
        width = parallelRegion.getWidth();
      } catch (StreamsException e) {
        throw SPLLogicalModel.traceAndReturnException(
            "Internal error: parallel region width unknown; " + e.toString());
      }
      return width;
    }
    if (owningComposite == null) {
      return BigInteger.valueOf(
          0); // getMaxChannels() outside of a parallel region always returns 0
    }
    return owningComposite.getLocalMaxChannels();
  }

  private String getRootOperatorName(String operName) {
    int pos = operName.lastIndexOf('.');
    if (pos != -1) {
      return operName.substring(pos + 1);
    }
    return operName;
  }

  private String getLogicalName() {
    return name;
  }

  public boolean isCompositeOperator() {
    return false;
  }

  public CompositeOperator asCompositeOperator() {
    return null;
  }

  public boolean isExportOperator() {
    return false;
  }

  public ExportOperator asExportOperator() {
    return null;
  }

  public boolean isSplitterOperator() {
    return false;
  }

  public SplitterOperator asSplitterOperator() {
    return null;
  }

  public boolean isMergerOperator() {
    return false;
  }

  public MergerOperator asMergerOperator() {
    return null;
  }

  public void setParallelRegion(ParallelRegion region) {
    parallelRegion = region;
  }

  public ParallelRegion getParallelRegion() throws SAMInternalErrorException {
    if (parallelRegion == null) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: parallel region null");
    }
    return parallelRegion;
  }

  public abstract void addInputPortConnection(BigInteger portIndex, Connection conn)
      throws SAMInternalErrorException;

  public void addOutputPortConnection(BigInteger portIndex, Connection connection)
      throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  public abstract void createSplittersAndMergers(SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError;

  public CompositeOperator getOwningComposite() throws SAMInternalErrorException {
    if (owningComposite == null) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: owning composite null");
    }
    return owningComposite;
  }

  public void fixReverseConnection(
      BigInteger fromPortIndex,
      PortKind fromPortKind,
      BigInteger oldToOperIndex,
      BigInteger oldToPortIndex,
      BigInteger newToOperIndex,
      BigInteger newToPortIndex,
      PortKind toPortKind)
      throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  public boolean isInParallelRegion() {
    if (parallelRegion != null) {
      return true;
    }
    if (owningComposite != null) {
      return owningComposite.isInParallelRegion();
    }
    return false;
  }

  public boolean isParallelRegion() {
    return (parallelRegion != null);
  }

  public boolean isMainComposite() {
    return (owningComposite == null);
  }

  public boolean isPrimitive() {
    return false;
  }

  public PrimitiveOperator asPrimitive() {
    return null;
  }

  public BigInteger getPhysicalIndex() throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException(
        "Internal error: physicalIndex called on non-primitive operator");
  }

  public boolean isReplica() {
    return isReplica;
  }

  public abstract String toString();

  public void populateNode(TopologyNode node, SPLLogicalModel splLogicalModel)
      throws SAMInternalErrorException {
    node.setLogicalIndex(logicalIndex);
    node.setName(getFullyQualifiedName());
    node.setLogicalName(getFullyQualifiedLogicalName());

    if (annotations != null) {
      List<Annotation> topAnnotations = node.getAnnotations();
      for (Annotation annotation : annotations) {
        if (annotation.getTag().compareTo("view") == 0) {
          List<AnnotationKeyValue> newKeyValues = new ArrayList<>();
          List<AnnotationKeyValue> keyValues = annotation.getKeyValues();
          for (AnnotationKeyValue kv : keyValues) {
            if (kv.getKey().compareTo("name") == 0) {
              StringBuilder viewName = new StringBuilder();
              StringBuilder viewLogicalName = new StringBuilder();
              if (hasOwningComposite()) {
                viewName.append(getOwningComposite().getFullyQualifiedOwningCompositeName());
                viewLogicalName.append(
                    getOwningComposite().getFullyQualifiedOwningLogicalCompositeName());
              }
              if (viewName.length() != 0) {
                viewName.append('.');
                viewLogicalName.append('.');
              }
              viewName.append(kv.getValue().toString());
              viewLogicalName.append(kv.getValue().toString());
              if (Trace.isEnabled(Level.DEBUG)) {
                Trace.logDebug("View name= " + viewName + ", logicalName=" + viewLogicalName);
              }
              AnnotationKeyValue name =
                  new AnnotationKeyValue("name", new AnnotationPrimitiveValue(viewName.toString()));
              newKeyValues.add(name);
              AnnotationKeyValue logicalName =
                  new AnnotationKeyValue(
                      "logicalName", new AnnotationPrimitiveValue(viewLogicalName.toString()));
              newKeyValues.add(logicalName);
            } else {
              System.err.println(
                  "key is " + kv.getKey() + ", value is " + kv.getValue().toString());
              newKeyValues.add(kv);
            }
          }
          Annotation newAnnotation = new Annotation("view", true, newKeyValues);
          topAnnotations.add(newAnnotation);
        } else {
          topAnnotations.add(annotation);
        }
      }
    }
  }

  public BigInteger getLogicalIndex() {
    return logicalIndex;
  }
}
