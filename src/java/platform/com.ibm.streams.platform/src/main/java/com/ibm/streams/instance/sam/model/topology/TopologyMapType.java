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
import com.ibm.streams.platform.services.MapType;
import com.ibm.streams.platform.services.TypeType;
import java.math.BigInteger;
import java.util.Objects;

public class TopologyMapType extends TopologyCollectionType implements IXmlSerializer<MapType> {

  private TopologyType keyType;
  private TopologyType valueType;

  public TopologyMapType(BigInteger bound, TopologyType keyType, TopologyType valueType) {
    super(bound);
    this.keyType = keyType;
    this.valueType = valueType;
  }

  public TopologyMapType copy() {
    return new TopologyMapType(bound, keyType.copy(), valueType.copy());
  }

  public TopologyType getKeyType() {
    return keyType;
  }

  public void setKeyType(TopologyType keyType) {
    this.keyType = keyType;
  }

  public TopologyType getValueType() {
    return valueType;
  }

  public void setValueType(TopologyType valueType) {
    this.valueType = valueType;
  }

  @Override
  public boolean isTopologyMapType() {
    return true;
  }

  @Override
  public TopologyMapType asTopologyMapType() {
    return this;
  }

  @Override
  public MapType toXml() {
    MapType xml = new MapType();
    if (keyType.isTopologyPrimitiveType()) {
      xml.setKeyType(keyType.toString());
    } else {
      TypeType tt = new TypeType();
      if (keyType.isTopologyTupleType()) {
        tt.setTuple(keyType.asTopologyTupleType().toXml());
      } else if (keyType.isTopologyListType()) {
        tt.setList(keyType.asTopologyListType().toXml());
      } else if (keyType.isTopologySetType()) {
        tt.setSet(keyType.asTopologySetType().toXml());
      } else if (keyType.isTopologyMapType()) {
        tt.setMap(keyType.asTopologyMapType().toXml());
      } else if (keyType.isTopologyOptionalType()) {
        tt.setOptional(keyType.asTopologyOptionalType().toXml());
      } else {
        assert (false);
      }
      xml.setKey(tt);
    }
    if (valueType.isTopologyPrimitiveType()) {
      xml.setValueType(valueType.toString());
    } else {
      TypeType tt = new TypeType();
      if (valueType.isTopologyTupleType()) {
        tt.setTuple(valueType.asTopologyTupleType().toXml());
      } else if (valueType.isTopologyListType()) {
        tt.setList(valueType.asTopologyListType().toXml());
      } else if (valueType.isTopologySetType()) {
        tt.setSet(valueType.asTopologySetType().toXml());
      } else if (valueType.isTopologyMapType()) {
        tt.setMap(valueType.asTopologyMapType().toXml());
      } else if (valueType.isTopologyOptionalType()) {
        tt.setOptional(valueType.asTopologyOptionalType().toXml());
      } else {
        assert (false);
      }
      xml.setValue(tt);
    }

    if (isBounded()) {
      xml.setBound(getBound().longValue());
    }
    return xml;
  }

  @Override
  public JSONObject toApi() {
    JSONObject typeObj = new JSONObject();
    JSONObject keyObj = getKeyType().toApi();
    JSONObject valueObj = getValueType().toApi();

    StringBuilder sb = new StringBuilder("map<");
    sb.append(keyObj.remove(PropertyType.SPL_TYPE.toApiStr()));
    sb.append(",");
    sb.append(valueObj.remove(PropertyType.SPL_TYPE.toApiStr()));
    sb.append(">");
    if (isBounded()) {
      sb.append("[" + bound.toString() + "]");
    }

    typeObj.put(ApiProperties.PropertyType.TYPE.toApiStr(), getApiType());
    typeObj.put(ApiProperties.PropertyType.SPL_TYPE.toApiStr(), sb.toString());
    if (isBounded()) {
      typeObj.put(ApiProperties.PropertyType.BOUNDED_SIZE.toApiStr(), getBound());
    }
    typeObj.put(ApiProperties.PropertyType.KEY_OBJECT.toApiStr(), keyObj);
    typeObj.put(ApiProperties.PropertyType.VALUE_OBJECT.toApiStr(), valueObj);
    return typeObj;
  }

  @Override
  public String getApiType() {
    return isBounded() ? ApiEnum.SPL_TYPE_BMAP : ApiEnum.SPL_TYPE_MAP;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.append("map<" + keyType.toString() + "," + valueType.toString() + ">");
    if (isBounded()) {
      dumper.append("[" + getBound().toString() + "]");
    }
    return dumper.toString();
  }

  @Override
  public int hashCode() {
    return Objects.hash(bound, keyType, valueType);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof TopologyMapType) {
      TopologyMapType t = (TopologyMapType) obj;
      return (getKeyType().equals(t.getKeyType())
          && getValueType().equals(t.getValueType())
          && ((!isBounded() && !t.isBounded())
              || (isBounded() && t.isBounded() && getBound().equals(t.getBound()))));
    }
    return false;
  }
}
