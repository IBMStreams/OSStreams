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

import com.api.json.JSONArray;
import com.api.json.JSONObject;
import com.ibm.streams.admin.internal.core.api.ApiEnum;
import com.ibm.streams.admin.internal.core.api.ApiProperties.PropertyType;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.TupleType;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class TopologyTupleType extends TopologyType implements IXmlSerializer<TupleType> {

  private List<TopologyTupleAttributeType> attributes = new ArrayList<>();

  public TopologyTupleType() {}

  public TopologyTupleType(TopologyTupleType other) {
    for (TopologyTupleAttributeType attr : other.attributes) {
      TopologyTupleAttributeType copy = attr.copy();
      attributes.add(copy);
    }
  }

  public TopologyType copy() {
    return new TopologyTupleType(this);
  }

  public List<TopologyTupleAttributeType> getAttributes() {
    return attributes;
  }

  public void setAttributes(List<TopologyTupleAttributeType> attributes) {
    this.attributes = attributes;
  }

  @Override
  public boolean isTopologyTupleType() {
    return true;
  }

  @Override
  public TopologyTupleType asTopologyTupleType() {
    return this;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.append("tuple<");
    int i = 0;
    for (TopologyTupleAttributeType attr : attributes) {
      if (i > 0) dumper.append(",");
      dumper.append(attr.toString());
      ++i;
    }
    dumper.append(">");
    return dumper.toString();
  }

  public TupleType toXml() {
    TupleType xml = new TupleType();
    for (TopologyTupleAttributeType ttat : attributes) xml.getAttr().add(ttat.toXml());
    return xml;
  }

  @Override
  public JSONObject toApi() {
    JSONObject typeObj = new JSONObject();
    typeObj.put(PropertyType.TYPE.toApiStr(), getApiType());
    JSONArray tupleAttrs = new JSONArray();
    StringBuffer splType = new StringBuffer("tuple<");
    boolean firstAttr = true;
    for (TopologyTupleAttributeType ttat : getAttributes()) {
      JSONObject attrObj = ttat.toApi();
      tupleAttrs.add(attrObj);

      if (firstAttr) {
        firstAttr = false;
      } else {
        splType.append(",");
      }
      splType.append(attrObj.get(PropertyType.SPL_TYPE.toApiStr()));
      splType.append(" ");
      splType.append(attrObj.get(PropertyType.NAME.toApiStr()));
    }
    splType.append(">");
    typeObj.put(PropertyType.SPL_TYPE.toApiStr(), splType.toString());
    typeObj.put(PropertyType.TUPLE_ATTRIBUTES.toApiStr(), tupleAttrs);
    return typeObj;
  }

  @Override
  public String getApiType() {
    return ApiEnum.SPL_TYPE_TUPLE;
  }

  public int hashCode() {
    return Objects.hash(attributes);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof TopologyTupleType) {
      TopologyTupleType t = (TopologyTupleType) obj;
      if (getAttributes().size() == t.getAttributes().size()) {
        // Make sure both TopologyTupleType have same TopologyTupleAttributeType
        // objects, but sequence does not matter
        for (TopologyTupleAttributeType attr : getAttributes()) {
          if (!t.getAttributes().contains(attr)) {
            return false;
          }
        }
        return true;
      }
    }
    return false;
  }
}
