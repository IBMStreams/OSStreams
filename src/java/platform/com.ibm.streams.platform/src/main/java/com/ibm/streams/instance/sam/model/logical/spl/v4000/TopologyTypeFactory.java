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

package com.ibm.streams.instance.sam.model.logical.spl.v4000;

import com.ibm.streams.instance.sam.model.topology.TopologyListType;
import com.ibm.streams.instance.sam.model.topology.TopologyMapType;
import com.ibm.streams.instance.sam.model.topology.TopologyPrimitiveType;
import com.ibm.streams.instance.sam.model.topology.TopologySetType;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleAttributeType;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleType;
import com.ibm.streams.instance.sam.model.topology.TopologyType;
import com.ibm.streams.platform.services.v4000.AttrType;
import com.ibm.streams.platform.services.v4000.ListOrSetType;
import com.ibm.streams.platform.services.v4000.MapType;
import com.ibm.streams.platform.services.v4000.TupleType;
import com.ibm.streams.platform.services.v4000.TypeType;
import java.math.BigInteger;
import java.util.List;

public class TopologyTypeFactory {
  public static TopologyTupleType createType(TupleType adlType) {
    return (TopologyTupleType) getTopologyType(adlType);
  }

  private static TopologyType getTopologyType(TupleType adlType) {
    TopologyTupleType topTupleType = new TopologyTupleType();
    List<TopologyTupleAttributeType> topAttributes = topTupleType.getAttributes();
    List<AttrType> adlAttrs = adlType.getAttr();
    for (AttrType adlAttr : adlAttrs) {
      TopologyTupleAttributeType topAttribute =
          new TopologyTupleAttributeType(adlAttr.getName(), getTopologyType(adlAttr));
      topAttributes.add(topAttribute);
    }
    return topTupleType;
  }

  private static TopologyType getTopologyType(AttrType adlType) {
    if (adlType.getType() != null) {
      return getTopologyType(adlType.getType());
    }
    if (adlType.getTuple() != null) {
      return getTopologyType(adlType.getTuple());
    }
    if (adlType.getList() != null) {
      return getTopologyListType(adlType.getList());
    }
    if (adlType.getMap() != null) {
      return getTopologyType(adlType.getMap());
    }
    assert (adlType.getSet() != null);
    return getTopologySetType(adlType.getSet());
  }

  private static TopologyType getTopologyListType(ListOrSetType adlType) {
    TopologyType elementType = null;
    if (adlType.getElementType() != null) {
      elementType = getTopologyType(adlType.getElementType());
    } else if (adlType.getTuple() != null) {
      elementType = getTopologyType(adlType.getTuple());
    } else if (adlType.getList() != null) {
      elementType = getTopologyListType(adlType.getList());
    } else if (adlType.getMap() != null) {
      elementType = getTopologyType(adlType.getMap());
    } else {
      assert (adlType.getSet() != null);
      elementType = getTopologySetType(adlType.getSet());
    }
    BigInteger bound = null;
    if (adlType.getBound() != null) {
      bound = new BigInteger(adlType.getBound().toString());
    }
    return new TopologyListType(bound, elementType);
  }

  private static TopologyType getTopologySetType(ListOrSetType adlType) {
    TopologyType elementType = null;
    if (adlType.getElementType() != null) {
      elementType = getTopologyType(adlType.getElementType());
    } else if (adlType.getTuple() != null) {
      elementType = getTopologyType(adlType.getTuple());
    } else if (adlType.getList() != null) {
      elementType = getTopologyListType(adlType.getList());
    } else if (adlType.getMap() != null) {
      elementType = getTopologyType(adlType.getMap());
    } else {
      assert (adlType.getSet() != null);
      elementType = getTopologySetType(adlType.getSet());
    }
    BigInteger bound = null;
    if (adlType.getBound() != null) {
      bound = new BigInteger(adlType.getBound().toString());
    }
    return new TopologySetType(bound, elementType);
  }

  private static TopologyType getTopologyType(TypeType adlType) {
    if (adlType.getTuple() != null) {
      return getTopologyType(adlType.getTuple());
    }
    if (adlType.getList() != null) {
      return getTopologyListType(adlType.getList());
    }
    if (adlType.getMap() != null) {
      return getTopologyType(adlType.getMap());
    }
    assert (adlType.getSet() != null);
    return getTopologySetType(adlType.getSet());
  }

  private static TopologyType getTopologyType(MapType adlType) {
    TopologyType keyType, valueType;
    if (adlType.getKeyType() != null) {
      keyType = getTopologyType(adlType.getKeyType());
    } else {
      keyType = getTopologyType(adlType.getKey());
    }
    if (adlType.getValueType() != null) {
      valueType = getTopologyType(adlType.getValueType());
    } else {
      valueType = getTopologyType(adlType.getValue());
    }
    BigInteger bound = null;
    if (adlType.getBound() != null) {
      bound = new BigInteger(adlType.getBound().toString()); // Is this correct??
    }
    return new TopologyMapType(bound, keyType, valueType);
  }

  private static TopologyType getTopologyType(String adlType) {
    TopologyPrimitiveType t = new TopologyPrimitiveType(adlType);
    return t;
  }
}
