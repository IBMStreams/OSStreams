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

package com.ibm.streams.instance.sam.model.logical.spl.v4200;

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.logical.spl.CCRegion;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperator;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOperatorBase;
import com.ibm.streams.instance.sam.model.logical.spl.ThreadingModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.platform.services.v4200.CompositeOperInstancePortType;
import com.ibm.streams.platform.services.v4200.CompositeOperInstanceType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperInstanceBaseType;
import com.ibm.streams.platform.services.v4200.SplAnnotationType;
import com.ibm.streams.platform.services.v4200.SplAnnotationValueType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

public class V4200CompositeOperator extends CompositeOperator {

  private static void createAnnotations(
      V4200CompositeOperator v4200CompositeOperator,
      CompositeOperInstanceType composite,
      V4200SPLLogicalModel model)
      throws SAMInternalErrorException {
    boolean isStandalone = SAMStaticContext.isStandaloneMode();
    if (!composite.getSplAnnotation().isEmpty()) {
      List<Annotation> annotations = v4200CompositeOperator.getAnnotations();
      for (SplAnnotationType splAnnotation : composite.getSplAnnotation()) {
        String tag = splAnnotation.getTag();
        Boolean isInternal = splAnnotation.isInternal();
        if (isStandalone && tag.equals("consistentRegion")) {
          model.getUnsupportedAnnotationsWarning().add("consistent");
          continue;
        }
        List<SplAnnotationValueType> splValues = splAnnotation.getValue();
        Annotation newAnnotation =
            AnnotationFactory.createAnnotation(
                tag, (isInternal != null) ? isInternal : false, splValues, model);
        if (newAnnotation.getTag().equals("threading")) {
          v4200CompositeOperator.setThreadingModel(new ThreadingModel(newAnnotation));
        }
        annotations.add(newAnnotation);
        // While we are walking the annotations check if any specify a CC region
        if (tag.equals("consistentRegion")) {
          CCRegion ccRegion = new CCRegion(newAnnotation);
          model.getCCRegions().addRegion(ccRegion, BigInteger.ZERO);
        }
      }
    }
  }

  public V4200CompositeOperator(
      CompositeOperInstanceType composite, CompositeOperator owningComp, V4200SPLLogicalModel model)
      throws SAMInternalErrorException {

    super(
        composite.getName(),
        composite.getSourceIndex(),
        composite.getIndex(),
        (composite.getDescription() != null ? composite.getDescription() : ""),
        null,
        owningComp,
        composite.getDefinitionIndex());

    createAnnotations(this, composite, model);

    model.getOperatorMap().put(composite.getIndex(), this);

    // allocate model input/output ports
    if (composite.getInputPorts() != null) {
      for (CompositeOperInstancePortType compositeOperInstancePortType :
          composite.getInputPorts().getPort()) {
        CompositeInputPort mp = new V4200CompositeInputPort(compositeOperInstancePortType, this);
        inputPorts.add(mp);
      }
    }

    if (composite.getOutputPorts() != null) {
      for (CompositeOperInstancePortType compositeOperInstancePortType :
          composite.getOutputPorts().getPort()) {
        CompositeOutputPort mp = new V4200CompositeOutputPort(compositeOperInstancePortType, this);
        outputPorts.add(mp);
      }
    }
    // create primitive operators
    for (PrimitiveOperInstanceBaseType primitiveOperInstanceType :
        composite
            .getPrimitiveOperInstanceOrPrimitiveImportOperInstanceOrPrimitiveExportOperInstance()) {
      PrimitiveOperatorBase mpo = null;
      List<Annotation> annotations = null;
      List<SplAnnotationType> splAnnotations = primitiveOperInstanceType.getSplAnnotation();
      if (!splAnnotations.isEmpty()) {
        annotations = new ArrayList<>();
      }

      boolean isStandalone = SAMStaticContext.isStandaloneMode();
      switch (primitiveOperInstanceType.getClass().getName()) {
        case "com.ibm.streams.platform.services.v4200.PrimitiveImportOperInstanceType":
          mpo =
              new V4200ImportOperator(
                  (com.ibm.streams.platform.services.v4200.PrimitiveImportOperInstanceType)
                      primitiveOperInstanceType,
                  annotations,
                  this,
                  model);
          if (isStandalone) {
            model.emitUnsupportedImportExport();
          }
          break;
        case "com.ibm.streams.platform.services.v4200.PrimitiveExportOperInstanceType":
          mpo =
              new V4200ExportOperator(
                  (com.ibm.streams.platform.services.v4200.PrimitiveExportOperInstanceType)
                      primitiveOperInstanceType,
                  annotations,
                  this,
                  model);
          if (isStandalone) {
            model.emitUnsupportedImportExport();
          }
          break;
        default:
          mpo =
              new V4200PrimitiveOperator(
                  (com.ibm.streams.platform.services.v4200.PrimitiveOperInstanceType)
                      primitiveOperInstanceType,
                  annotations,
                  this,
                  model);
          break;
      }
      for (SplAnnotationType splAnnotation : splAnnotations) {
        String tag = splAnnotation.getTag();
        Boolean isInternal = splAnnotation.isInternal();
        List<SplAnnotationValueType> splValues = splAnnotation.getValue();
        Annotation annotation =
            AnnotationFactory.createAnnotation(
                tag, (isInternal != null) ? isInternal : false, splValues, model);
        if (!isStandalone && annotation.getTag().equals("consistentRegionEntry")) {
          mpo.getCCInfo().setRegionEntry(annotation, model);
        } else if (isStandalone && annotation.getTag().equals("autonomous")) {
          // Just consume this one
        } else if (isStandalone && annotation.getTag().equals("view")) {
          model.getUnsupportedAnnotationsWarning().add("view");
        } else if (annotation.getTag().equals("threading")) {
          threadingModel = new ThreadingModel(annotation);
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
          new V4200CompositeOperator(nestedCompositeOperInstanceType, this, model);
      compositeOperators.add(comp);
    }
  }
}
