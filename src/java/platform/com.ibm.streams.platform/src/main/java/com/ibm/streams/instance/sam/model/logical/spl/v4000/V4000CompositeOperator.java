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

package com.ibm.streams.instance.sam.model.logical.spl.v4000;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.logical.spl.CCRegion;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperator;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOperatorBase;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.platform.services.v4000.CompositeOperInstancePortType;
import com.ibm.streams.platform.services.v4000.CompositeOperInstanceType;
import com.ibm.streams.platform.services.v4000.PrimitiveOperInstanceType;
import com.ibm.streams.platform.services.v4000.SplAnnotationType;
import com.ibm.streams.platform.services.v4000.SplAnnotationValueType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

public class V4000CompositeOperator extends CompositeOperator {

  private static List<Annotation> createAnnotations(
      CompositeOperInstanceType composite, SPLLogicalModel model) throws SAMInternalErrorException {
    if (!composite.getSplAnnotation().isEmpty()) {
      List<Annotation> annotations = new ArrayList<>();
      for (SplAnnotationType splAnnotation : composite.getSplAnnotation()) {
        String tag = splAnnotation.getTag();
        List<SplAnnotationValueType> splValues = splAnnotation.getValue();
        Annotation newAnnotation = AnnotationFactory.createAnnotation(tag, splValues, model);
        annotations.add(newAnnotation);
        // While we are walking the annotations check if any specify a CC region
        if (tag.equals("consistentRegion")) {
          CCRegion ccRegion = new CCRegion(newAnnotation);
          model.getCCRegions().addRegion(ccRegion, BigInteger.ZERO);
        }
      }
      return annotations;
    }
    return null;
  }

  public V4000CompositeOperator(
      CompositeOperInstanceType composite, CompositeOperator owningComp, SPLLogicalModel model)
      throws SAMInternalErrorException {

    super(
        composite.getName(),
        composite.getSourceIndex(),
        composite.getIndex(),
        (composite.getDescription() != null ? composite.getDescription() : ""),
        createAnnotations(composite, model),
        owningComp,
        composite.getCompositeOperIndex());

    model.getOperatorMap().put(composite.getIndex(), this);

    // allocate model input/output ports
    if (composite.getInputPorts() != null) {
      for (CompositeOperInstancePortType compositeOperInstancePortType :
          composite.getInputPorts().getPort()) {
        CompositeInputPort mp = new V4000CompositeInputPort(compositeOperInstancePortType, this);
        inputPorts.add(mp);
      }
    }

    if (composite.getOutputPorts() != null) {
      for (CompositeOperInstancePortType compositeOperInstancePortType :
          composite.getOutputPorts().getPort()) {
        CompositeOutputPort mp = new V4000CompositeOutputPort(compositeOperInstancePortType, this);
        outputPorts.add(mp);
      }
    }

    // create primitive operators
    for (PrimitiveOperInstanceType primitiveOperInstanceType :
        composite.getPrimitiveOperInstance()) {
      PrimitiveOperatorBase mpo = null;
      List<Annotation> annotations = null;
      List<SplAnnotationType> splAnnotations = primitiveOperInstanceType.getSplAnnotation();
      if (!splAnnotations.isEmpty()) {

        annotations = new ArrayList<>();
      }
      switch (primitiveOperInstanceType.getKind()) {
        case "spl.adapter::Import":
          mpo = new V4000ImportOperator(primitiveOperInstanceType, annotations, this, model);
          break;
        case "spl.adapter::Export":
          mpo = new V4000ExportOperator(primitiveOperInstanceType, annotations, this, model);
          break;
        default:
          mpo = new V4000PrimitiveOperator(primitiveOperInstanceType, annotations, this, model);
          break;
      }
      for (SplAnnotationType splAnnotation : splAnnotations) {
        String tag = splAnnotation.getTag();
        List<SplAnnotationValueType> splValues = splAnnotation.getValue();
        Annotation annotation = AnnotationFactory.createAnnotation(tag, splValues, model);
        if (annotation.getTag().compareTo("consistentRegionEntry") == 0) {
          mpo.getCCInfo().setRegionEntry(annotation, model);
        } else {
          annotations.add(annotation);
        }
      }
      primitiveOperators.add(mpo);
    }

    // composite operators
    for (CompositeOperInstanceType nestedCompositeOperInstanceType :
        composite.getCompositeOperInstance()) {
      CompositeOperator comp =
          new V4000CompositeOperator(nestedCompositeOperInstanceType, this, model);
      compositeOperators.add(comp);
    }
  }
}
