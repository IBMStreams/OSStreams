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

import com.api.json.JSONObject;
import com.ibm.streams.admin.internal.core.api.ApiProperties;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.AttrType;
import java.util.Objects;

public class TopologyTupleAttributeType extends TopologyType implements IXmlSerializer<AttrType> {
  String name;
  TopologyType type;

  public TopologyTupleAttributeType(String name, TopologyType type) {
    this.name = name;
    this.type = type;
  }

  @Override
  public TopologyTupleAttributeType copy() {
    return new TopologyTupleAttributeType(name, type.copy());
  }

  // @Override
  // public TopologyType copy() {
  // return new TopologyTupleAttributeType(name, type.copy());
  // }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public TopologyType getType() {
    return type;
  }

  public void setType(TopologyType type) {
    this.type = type;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.append(type.toString());
    dumper.append(" ");
    dumper.append(name);
    return dumper.toString();
  }

  @Override
  public AttrType toXml() {
    AttrType xml = new AttrType();
    xml.setName(name);
    if (type.isTopologyPrimitiveType()) {
      xml.setType(type.toString());
    } else if (type.isTopologyTupleType()) {
      xml.setTuple(type.asTopologyTupleType().toXml());
    } else if (type.isTopologyListType()) {
      xml.setList(type.asTopologyListType().toXml());
    } else if (type.isTopologySetType()) {
      xml.setSet(type.asTopologySetType().toXml());
    } else if (type.isTopologyMapType()) {
      xml.setMap(type.asTopologyMapType().toXml());
    } else if (type.isTopologyOptionalType()) {
      xml.setOptional(type.asTopologyOptionalType().toXml());
    } else {
      assert (false);
    }
    return xml;
  }

  @Override
  public JSONObject toApi() {
    JSONObject attrObj = new JSONObject();
    attrObj.put(ApiProperties.PropertyType.NAME.toApiStr(), name);

    if (type.isTopologyPrimitiveType()) {
      attrObj.putAll(type.asTopologyPrimitiveType().toApi());
    } else if (type.isTopologyTupleType()) {
      attrObj.putAll(type.asTopologyTupleType().toApi());
    } else if (type.isTopologyListType()) {
      attrObj.putAll(type.asTopologyListType().toApi());
    } else if (type.isTopologySetType()) {
      attrObj.putAll(type.asTopologySetType().toApi());
    } else if (type.isTopologyMapType()) {
      attrObj.putAll(type.asTopologyMapType().toApi());
    } else if (type.isTopologyOptionalType()) {
      attrObj.putAll(type.asTopologyOptionalType().toApi());
    } else {
      assert (false);
    }
    return attrObj;
  }

  @Override
  public String getApiType() {
    return null;
  }

  public int hashCode() {
    return Objects.hash(name, type);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof TopologyTupleAttributeType) {
      TopologyTupleAttributeType t = (TopologyTupleAttributeType) obj;
      return (getName().equals(t.getName()) && getType().equals(t.getType()));
    }
    return false;
  }
}
