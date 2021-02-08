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
import com.ibm.streams.admin.internal.core.api.ApiProperties.PropertyType;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.ListOrSetType;
import java.math.BigInteger;
import java.util.Objects;

public abstract class TopologyListOrSetType extends TopologyCollectionType
    implements IXmlSerializer<ListOrSetType> {

  protected TopologyType elementType;

  public TopologyListOrSetType(BigInteger bound, TopologyType elementType) {
    super(bound);
    this.elementType = elementType;
  }

  public TopologyType getElementType() {
    return elementType;
  }

  public void setElementType(TopologyType elementType) {
    this.elementType = elementType;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.append(elementType.toString());
    return dumper.toString();
  }

  @Override
  public ListOrSetType toXml() {
    ListOrSetType xml = new ListOrSetType();
    if (elementType.isTopologyPrimitiveType()) {
      xml.setElementType(elementType.toString());
    } else if (elementType.isTopologyTupleType()) {
      xml.setTuple(elementType.asTopologyTupleType().toXml());
    } else if (elementType.isTopologyListType()) {
      xml.setList(elementType.asTopologyListType().toXml());
    } else if (elementType.isTopologySetType()) {
      xml.setSet(elementType.asTopologySetType().toXml());
    } else if (elementType.isTopologyMapType()) {
      xml.setMap(elementType.asTopologyMapType().toXml());
    } else if (elementType.isTopologyOptionalType()) {
      xml.setOptional(elementType.asTopologyOptionalType().toXml());
    } else {
      assert (false);
    }
    if (isBounded()) {
      xml.setBound(getBound().longValue());
    }
    return xml;
  }

  @Override
  public JSONObject toApi() {
    JSONObject typeObj = new JSONObject();
    JSONObject elementObj = getElementType().toApi();

    StringBuilder sb = new StringBuilder(isTopologySetType() ? "set" : "list");
    sb.append("<");
    sb.append(elementObj.remove(PropertyType.SPL_TYPE.toApiStr()));
    sb.append(">");
    if (isBounded()) {
      sb.append("[" + bound.toString() + "]");
    }

    typeObj.put(PropertyType.TYPE.toApiStr(), getApiType());
    typeObj.put(PropertyType.SPL_TYPE.toApiStr(), sb.toString());
    if (isBounded()) {
      typeObj.put(PropertyType.BOUNDED_SIZE.toApiStr(), getBound());
    }
    typeObj.put(PropertyType.ELEMENT_OBJECT.toApiStr(), elementObj);
    return typeObj;
  }

  public int hashCode() {
    return Objects.hash(bound, elementType);
  }
}
