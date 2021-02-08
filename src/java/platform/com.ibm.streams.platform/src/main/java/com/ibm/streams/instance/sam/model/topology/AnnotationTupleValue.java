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

import com.ibm.streams.platform.services.AnnotationAttributeValueType;
import com.ibm.streams.platform.services.AnnotationTupleValueType;
import java.util.List;

public class AnnotationTupleValue extends AnnotationValue {
  private final List<AnnotationKeyValue> attributes;

  public List<AnnotationKeyValue> getAttributes() {
    return attributes;
  }

  public AnnotationTupleValue(List<AnnotationKeyValue> attributes) {
    super();
    this.attributes = attributes;
  }

  @Override
  public boolean isAnnotationTupleValue() {
    return true;
  }

  @Override
  public AnnotationTupleValue asAnnotationTupleValue() {
    return this;
  }

  public AnnotationTupleValueType toXml() {
    AnnotationTupleValueType atvt = new AnnotationTupleValueType();
    for (AnnotationKeyValue kv : attributes) {
      AnnotationAttributeValueType aavt = new AnnotationAttributeValueType();
      aavt.setKey(kv.getKey());
      AnnotationValue val = kv.getValue();
      if (val.isAnnotationPrimitiveValue()) {
        aavt.setValue(val.asAnnotationPrimitiveValue().getValue());
      } else if (val.isAnnotationListValue()) {
        aavt.setList(val.asAnnotationListValue().toXml());
      } else if (val.isAnnotationTupleValue()) {
        aavt.setTuple(val.asAnnotationTupleValue().toXml());
      } else {
        assert (false);
      }
      atvt.getAttr().add(aavt);
    }
    return atvt;
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();
    sb.append("{");
    boolean first = true;
    for (AnnotationKeyValue keyValue : attributes) {
      if (!first) {
        sb.append(",");
      }
      first = false;
      sb.append(keyValue.toString());
    }
    sb.append("}");
    return sb.toString();
  }
}
