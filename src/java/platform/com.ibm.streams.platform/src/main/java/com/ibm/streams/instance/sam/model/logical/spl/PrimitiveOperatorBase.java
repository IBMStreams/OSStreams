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
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import java.math.BigInteger;
import java.util.List;
import java.util.Map;
import java.util.Set;

public abstract class PrimitiveOperatorBase extends Operator {

  private final CCInfo ccInfo;

  public PrimitiveOperatorBase(
      String name,
      BigInteger sourceIndex,
      BigInteger index,
      String description,
      List<Annotation> annotations,
      CompositeOperator owningComp,
      boolean isRep,
      BigInteger definitionIndex,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    super(name, sourceIndex, index, description, annotations, owningComp, isRep, definitionIndex);

    ccInfo = new CCInfo(this);
  }

  public PrimitiveOperatorBase(
      PrimitiveOperatorBase other,
      BigInteger index,
      BigInteger localChannelIndex,
      CompositeOperator owningComp,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    super(other, index, localChannelIndex, owningComp);
    ccInfo = new CCInfo(other.ccInfo, this);
  }

  public void findStartOperators(SPLLogicalModel model) throws SAMInternalErrorException {
    if (ccInfo.isStartCutRegion()) {
      BigInteger globalChannelIndex = getGlobalChannelIndex();
      CCRegion region =
          model
              .getCCRegions()
              .findOrCreateRegion(
                  ccInfo.getIndex(),
                  globalChannelIndex.compareTo(BigInteger.valueOf(-1)) == 0
                      ? BigInteger.ZERO
                      : globalChannelIndex);
      region.addRegionEntry(this, model);
      model.getCCStartOperators().add(this);
    }
  }

  public abstract PrimitiveOperatorBase replicate(
      CompositeOperator owningComp, BigInteger localChannelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException;

  public CCInfo getCCInfo() {
    return ccInfo;
  }

  public abstract void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException;

  public abstract void computeCCReachabilityGraph(
      CCRegion ccRegion, Set<PrimitiveOperatorBase> operatorsSeen, SPLLogicalModel model)
      throws SAMInternalErrorException;

  public void populateNode(TopologyNode node, SPLLogicalModel model)
      throws SAMInternalErrorException {
    super.populateNode(node, model);
    node.setOperDefinitionIndex(getDefinitionIndex());
    PrimitiveOperatorDefinition operDef =
        model.getPrimitiveOperatorDefs().get(getDefinitionIndex());
    node.setOperRuntimeType(operDef.getRuntimeType());
    node.setOperKind(operDef.getKind());
    node.setInCCRegion(getCCInfo().isInCCRegion());
  }

  public void populateConnections(Map<BigInteger, TopologyNode> nodeMap, SPLLogicalModel model)
      throws SAMInternalErrorException {}

  public abstract void fixConnections(ParallelTransformState state)
      throws SAMInternalErrorException;

  public void injectCCAnnotations() throws SAMInternalErrorException {
    ccInfo.injectCCAnnotations();
  }

  public void performParallelResourceTransforms(
      SPLLogicalModel model, Set<String> nonParallelCoLocations)
      throws SAMInternalErrorException, SubmissionParameterError {
    // This class does nothing. Derived classes may override it.
  }

  public void performCCResourceTransforms() {
    // This class does nothing. Derived classes may override it.
  }
}
