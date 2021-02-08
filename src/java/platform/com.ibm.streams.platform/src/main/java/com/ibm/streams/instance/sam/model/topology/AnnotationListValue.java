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

import com.ibm.streams.platform.services.AnnotationListEntryValueType;
import com.ibm.streams.platform.services.AnnotationListValueType;
import java.util.List;

public class AnnotationListValue extends AnnotationValue {
  private final List<AnnotationValue> values;

  public AnnotationListValue(List<AnnotationValue> values) {
    super();
    this.values = values;
  }

  public List<AnnotationValue> getValues() {
    return values;
  }

  @Override
  public boolean isAnnotationListValue() {
    return true;
  }

  @Override
  public AnnotationListValue asAnnotationListValue() {
    return this;
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();
    sb.append("[");
    boolean first = true;
    for (AnnotationValue value : values) {
      if (!first) {
        sb.append(",");
      }
      first = false;
      sb.append(value.toString());
    }
    sb.append("]");
    return sb.toString();
  }

  public AnnotationListValueType toXml() {
    AnnotationListValueType alvt = new AnnotationListValueType();
    for (AnnotationValue value : values) {
      AnnotationListEntryValueType alevt = new AnnotationListEntryValueType();
      if (value.isAnnotationPrimitiveValue()) {
        alevt.setValue(value.asAnnotationPrimitiveValue().getValue());
      } else if (value.isAnnotationListValue()) {
        alevt.setList(value.asAnnotationListValue().toXml());
      } else if (value.isAnnotationTupleValue()) {
        alevt.setTuple(value.asAnnotationTupleValue().toXml());
      }
      alvt.getListEntry().add(alevt);
    }
    return alvt;
  }
}
