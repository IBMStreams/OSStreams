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

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonPrimitive;
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;
import com.ibm.streams.instance.sam.model.topology.TopologyType;
import java.lang.reflect.Type;

public class TopologyTypeSerializer implements JsonSerializer<TopologyType> {

  @Override
  public JsonElement serialize(TopologyType topType, Type type, JsonSerializationContext context) {
    if (topType.isTopologyListType()) {
      JsonObject list = new JsonObject();
      JsonObject listType = new JsonObject();
      listType.add("elementType", context.serialize(topType.asTopologyListType().getElementType()));
      if (topType.asTopologyListType().getBound() != null) {
        listType.add("bound", context.serialize(topType.asTopologyListType().getBound()));
      }
      list.add("list", listType);
      return list;
    } else if (topType.isTopologyMapType()) {
      JsonObject map = new JsonObject();
      JsonObject mapType = new JsonObject();
      mapType.add("keyType", context.serialize(topType.asTopologyMapType().getKeyType()));
      mapType.add("valueType", context.serialize(topType.asTopologyMapType().getValueType()));
      if (topType.asTopologyMapType().getBound() != null) {
        mapType.add("bound", context.serialize(topType.asTopologyMapType().getBound()));
      }
      map.add("map", mapType);
      return map;
    } else if (topType.isTopologySetType()) {
      JsonObject set = new JsonObject();
      JsonObject setType = new JsonObject();
      setType.add("elementType", context.serialize(topType.asTopologySetType().getElementType()));
      if (topType.asTopologySetType().getBound() != null) {
        setType.add("bound", context.serialize(topType.asTopologySetType().getBound()));
      }
      set.add("set", setType);
      return set;
    } else if (topType.isTopologyOptionalType()) {
      JsonObject optionalType = new JsonObject();
      optionalType.add(
          "underlyingType",
          context.serialize(topType.asTopologyOptionalType().getUnderlyingType()));
      return optionalType;
    } else if (topType.isTopologyTupleType()) {
      assert (false); // Handled elsewhere
      return null;
    }
    assert (topType.isTopologyPrimitiveType());
    return new JsonPrimitive(topType.asTopologyPrimitiveType().getTypeName());
  }
}
