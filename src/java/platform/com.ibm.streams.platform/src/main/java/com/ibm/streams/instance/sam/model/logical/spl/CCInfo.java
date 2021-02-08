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
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationPrimitiveValue;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

public class CCInfo {
  private boolean isOblivious = false;
  private BigInteger index = BigInteger.ZERO;
  private boolean isStartOfRegion = false;
  private boolean isEndOfRegion = false;
  private List<CCRegion> regions = new ArrayList<>();
  private List<AnnotationKeyValue> keyValues = new ArrayList<>();
  private final PrimitiveOperatorBase owningOperator;

  public void setRegionEntry(Annotation newAnnotation, SPLLogicalModel model)
      throws SAMInternalErrorException {
    isOblivious = false;
    List<AnnotationKeyValue> kvs = newAnnotation.getKeyValues();
    for (AnnotationKeyValue kv : kvs) {
      String key = kv.getKey();
      if (key.equals("index")) {
        try {
          assert (kv.getValue().isAnnotationPrimitiveValue());
          index = new BigInteger(kv.getValue().asAnnotationPrimitiveValue().getValue());
        } catch (NumberFormatException nfe) {
          throw SPLLogicalModel.traceAndReturnException(
              "Internal error: could not read region index");
        }
      } else if (key.equals("isStartOfRegion")) {
        isStartOfRegion = true;
      } else if (key.equals("isEndOfRegion")) {
        isEndOfRegion = true;
      } else {
        keyValues.add(kv);
      }
    }
  }

  public void print(ModelDumper dumper) {
    if (isStartOfRegion) dumper.line("Is start of consistent region");
    if (isEndOfRegion) dumper.line("Is end of consistent region");
    if (isOblivious) dumper.line("Is autonomous");
  }

  public CCInfo(PrimitiveOperatorBase owningOperator) {
    index = BigInteger.ZERO;
    isStartOfRegion = false;
    isEndOfRegion = false;
    isOblivious = true;
    this.owningOperator = owningOperator;
  }

  public CCInfo(CCInfo other, PrimitiveOperatorBase owningOperator) {
    index = other.index;
    isStartOfRegion = other.isStartOfRegion;
    isEndOfRegion = other.isEndOfRegion;
    isOblivious = other.isOblivious;
    keyValues.addAll(other.keyValues);
    this.owningOperator = owningOperator;
  }

  public boolean isStartCutRegion() {
    return isStartOfRegion;
  }

  public boolean isEndCutRegion() {
    return isEndOfRegion;
  }

  public void setOblivious() {
    isOblivious = true;
  }

  public BigInteger getIndex() {
    return index;
  }

  public int addedToRegion(CCRegion ccRegion) {
    regions.add(ccRegion);
    return regions.size();
  }

  public boolean isOblivious() {
    return isOblivious;
  }

  public List<CCRegion> getRegions() {
    return regions;
  }

  public void setMergedRegion(CCRegion ccRegion) {
    regions.clear();
    regions.add(ccRegion);
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<ccInfo index=\""
            + index
            + "\" isStartOfRegion=\""
            + isStartOfRegion
            + "\" isEndOfRegion=\""
            + isEndOfRegion
            + "\" isOblivious=\""
            + isOblivious
            + "\"/>");
    return dumper.toString();
  }

  public boolean isInCCRegion() {
    return regions.size() == 1;
  }

  public void injectCCAnnotations() throws SAMInternalErrorException {

    // If we are in a CC region we may have annotations to add
    if (isInCCRegion()) {
      CCRegion region = regions.get(0).getMergedRegion();
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace(
            "Injecting CC annotations for oper "
                + owningOperator.getFullyQualifiedName()
                + " for region "
                + region.getIndex().toString());
      }

      // We are in a CC region
      List<AnnotationKeyValue> operKeyValues = new ArrayList<>();
      AnnotationKeyValue indexKeyValue =
          new AnnotationKeyValue(
              "index", new AnnotationPrimitiveValue(region.getIndex().toString()));
      operKeyValues.add(indexKeyValue);
      AnnotationKeyValue logicalIndexKeyValue =
          new AnnotationKeyValue(
              "logicalIndex", new AnnotationPrimitiveValue(region.getLogicalIndex().toString()));
      operKeyValues.add(logicalIndexKeyValue);
      AnnotationKeyValue drainTimeoutKeyValue =
          new AnnotationKeyValue(
              "drainTimeout", new AnnotationPrimitiveValue(region.getDrainTimeout().toString()));
      operKeyValues.add(drainTimeoutKeyValue);
      AnnotationKeyValue resetTimeoutKeyValue =
          new AnnotationKeyValue(
              "resetTimeout", new AnnotationPrimitiveValue(region.getResetTimeout().toString()));
      operKeyValues.add(resetTimeoutKeyValue);
      if (isStartOfRegion) {
        AnnotationKeyValue isStartRegionKeyValue =
            new AnnotationKeyValue("isStartOfRegion", new AnnotationPrimitiveValue("true"));
        operKeyValues.add(isStartRegionKeyValue);
      }
      if (isEndOfRegion) {
        AnnotationKeyValue isEndRegionKeyValue =
            new AnnotationKeyValue("isEndOfRegion", new AnnotationPrimitiveValue("true"));
        operKeyValues.add(isEndRegionKeyValue);
      }
      for (AnnotationKeyValue keyValue : keyValues) {
        AnnotationKeyValue topKeyValue = keyValue;
        operKeyValues.add(topKeyValue);
      }

      Annotation operAnnotation = new Annotation("consistentRegionEntry", true, operKeyValues);
      owningOperator.getAnnotations().add(operAnnotation);
    }
  }
}
