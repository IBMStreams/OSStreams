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
import com.ibm.streams.admin.internal.core.api.ApiEnum;
import com.ibm.streams.admin.internal.core.api.ApiProperties;
import com.ibm.streams.admin.internal.core.api.ApiProperties.PropertyType;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.OptionalType;
import java.util.Objects;

public class TopologyOptionalType extends TopologyType implements IXmlSerializer<OptionalType> {

  private TopologyType underlyingType;

  public TopologyOptionalType(TopologyType underlyingType) {
    this.underlyingType = underlyingType;
  }

  public TopologyOptionalType copy() {
    return new TopologyOptionalType(underlyingType.copy());
  }

  public TopologyType getUnderlyingType() {
    return underlyingType;
  }

  public void setUnderlyingType(TopologyType underlyingType) {
    this.underlyingType = underlyingType;
  }

  @Override
  public boolean isTopologyOptionalType() {
    return true;
  }

  @Override
  public TopologyOptionalType asTopologyOptionalType() {
    return this;
  }

  @Override
  public OptionalType toXml() {
    OptionalType xml = new OptionalType();
    if (underlyingType.isTopologyPrimitiveType()) {
      xml.setType(underlyingType.toString());
    } else {
      if (underlyingType.isTopologyTupleType()) {
        xml.setTuple(underlyingType.asTopologyTupleType().toXml());
      } else if (underlyingType.isTopologyListType()) {
        xml.setList(underlyingType.asTopologyListType().toXml());
      } else if (underlyingType.isTopologySetType()) {
        xml.setSet(underlyingType.asTopologySetType().toXml());
      } else if (underlyingType.isTopologyMapType()) {
        xml.setMap(underlyingType.asTopologyMapType().toXml());
      } else if (underlyingType.isTopologyOptionalType()) {
        // Actually should never happen
        assert (false);
      } else {
        assert (false);
      }
    }
    return xml;
  }

  @Override
  public JSONObject toApi() {
    JSONObject typeObj = new JSONObject();
    JSONObject underlyingObj = getUnderlyingType().toApi();

    StringBuilder sb = new StringBuilder("optional<");
    sb.append(underlyingObj.remove(PropertyType.SPL_TYPE.toApiStr()));
    sb.append(">");

    typeObj.put(ApiProperties.PropertyType.TYPE.toApiStr(), getApiType());
    typeObj.put(ApiProperties.PropertyType.SPL_TYPE.toApiStr(), sb.toString());

    typeObj.put(ApiProperties.PropertyType.UNDERLYING_OBJECT.toApiStr(), underlyingObj);
    return typeObj;
  }

  @Override
  public String getApiType() {
    return ApiEnum.SPL_TYPE_OPTIONAL;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.append("optional<" + underlyingType.toString() + ">");
    return dumper.toString();
  }

  public int hashCode() {
    return Objects.hash(underlyingType);
  }

  public boolean equals(Object obj) {
    if (obj instanceof TopologyOptionalType) {
      TopologyOptionalType t = (TopologyOptionalType) obj;
      return (getUnderlyingType().equals(t.getUnderlyingType()));
    }
    return false;
  }
}
