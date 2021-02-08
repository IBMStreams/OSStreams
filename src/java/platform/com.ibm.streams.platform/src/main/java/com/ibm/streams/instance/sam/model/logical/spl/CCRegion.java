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
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.Set;

public class CCRegion {
  private BigInteger index = BigInteger.ZERO;
  private BigInteger logicalIndex = BigInteger.ZERO;
  private Float drainTimeout;
  private Float resetTimeout;
  private boolean isOperatorDriven;
  private CCRegion mergedWith;
  private Set<PrimitiveOperatorBase> operators = new HashSet<>();

  public Float getDrainTimeout() {
    return drainTimeout;
  }

  public Float getResetTimeout() {
    return resetTimeout;
  }

  public CCRegion(Annotation annotation) throws SAMInternalErrorException {
    mergedWith = null;
    isOperatorDriven = false;

    for (AnnotationKeyValue keyValue : annotation.getKeyValues()) {
      String key = keyValue.getKey();
      if (key.equals("index")) {
        try {
          assert (keyValue.getValue().isAnnotationPrimitiveValue());
          index = new BigInteger(keyValue.getValue().asAnnotationPrimitiveValue().getValue());
        } catch (NumberFormatException nfe) {
          throw SPLLogicalModel.traceAndReturnException(
              "Internal error: could not read region index");
        }
        logicalIndex = index;
      } else if (key.equals("trigger")) {
        if (keyValue.getValue().equals("operatorDriven")) {
          isOperatorDriven = true;
        }
      } else if (key.equals("drainTimeout")) {
        assert (keyValue.getValue().isAnnotationPrimitiveValue());
        drainTimeout =
            Float.parseFloat(keyValue.getValue().asAnnotationPrimitiveValue().getValue());
      } else if (key.equals("resetTimeout")) {
        assert (keyValue.getValue().isAnnotationPrimitiveValue());
        resetTimeout =
            Float.parseFloat(keyValue.getValue().asAnnotationPrimitiveValue().getValue());
      }
    }
  }

  public CCRegion(BigInteger index, CCRegion other) {
    this.logicalIndex = other.logicalIndex;
    this.index = index;
    this.isOperatorDriven = other.isOperatorDriven;
    this.drainTimeout = other.drainTimeout;
    this.resetTimeout = other.resetTimeout;
    this.mergedWith = null;
  }

  public BigInteger getLogicalIndex() {
    return logicalIndex;
  }

  public void addRegionEntry(PrimitiveOperatorBase oper, SPLLogicalModel model) {
    if (!operators.contains(oper)) {
      operators.add(oper);
      if (oper.getCCInfo().addedToRegion(this) > 1) {
        model.getMultiRegionOperators().add(oper);
      }
    }
  }

  public boolean wasMerged() {
    return (mergedWith != null);
  }

  public CCRegion mergedWith() throws SAMInternalErrorException {
    if (mergedWith == null) {
      throw SPLLogicalModel.traceAndReturnException(
          "Internal error: _mergedWith should not be null");
    }
    return mergedWith;
  }

  public void merge(CCRegion other) throws SAMInternalErrorException {
    for (PrimitiveOperatorBase op : other.getOperators()) {
      operators.add(op);
    }
    other.setMergedWith(this);
  }

  private void setMergedWith(CCRegion region) throws SAMInternalErrorException {
    if (mergedWith != null) {
      throw SPLLogicalModel.traceAndReturnException("Internal error: _mergedWith should be null");
    }
    mergedWith = region;
  }

  public Set<PrimitiveOperatorBase> getOperators() {
    return operators;
  }

  public boolean isOperatorDriven() {
    return isOperatorDriven;
  }

  public CCRegion getMergedRegion() {
    if (mergedWith != null) {
      return mergedWith.getMergedRegion();
    }
    return this;
  }

  public BigInteger getIndex() {
    return index;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((index == null) ? 0 : index.hashCode());
    result = prime * result + ((logicalIndex == null) ? 0 : logicalIndex.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    CCRegion other = (CCRegion) obj;
    if (index == null) {
      if (other.index != null) return false;
    } else if (!index.equals(other.index)) return false;
    if (logicalIndex == null) {
      if (other.logicalIndex != null) return false;
    } else if (!logicalIndex.equals(other.logicalIndex)) return false;
    return true;
  }
}
