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
import com.ibm.streams.admin.internal.core.api.ApiProperties.PropertyType;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.AttributeType;
import com.ibm.streams.platform.services.TupleTypeType;
import java.math.BigInteger;

public class TopologyTupleTypeType implements IXmlSerializer<TupleTypeType> {
  private TopologyTupleType tuple;
  private BigInteger index;

  public TopologyTupleTypeType(BigInteger index, TopologyTupleType tuple) {
    this.index = index;
    this.tuple = tuple;
  }

  public BigInteger getIndex() {
    return index;
  }

  public TopologyTupleType getTuple() {
    return tuple;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("tupleType<index=\"" + index + "\" type=\"" + tuple.toString() + "\"/>");
    return dumper.toString();
  }

  @Override
  public TupleTypeType toXml() {
    TupleTypeType xml = new TupleTypeType();
    for (TopologyTupleAttributeType ttat : tuple.getAttributes()) {
      AttributeType attrXml = new AttributeType();
      attrXml.setName(ttat.getName());
      attrXml.setType(ttat.getType().toString());
      xml.getAttribute().add(attrXml);
      xml.setTuple(tuple.toXml());
    }
    return xml;
  }

  public JSONObject toApi() {
    JSONObject tupleObj = new JSONObject();
    JSONArray tupleAttrs = new JSONArray();
    StringBuffer splType = new StringBuffer("tuple<");
    boolean firstAttr = true;
    for (TopologyTupleAttributeType ttat : tuple.getAttributes()) {
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
    tupleObj.put(PropertyType.SPL_TYPE.toApiStr(), splType.toString());
    tupleObj.put(PropertyType.TUPLE_ATTRIBUTES.toApiStr(), tupleAttrs);
    return tupleObj;
  }
}
