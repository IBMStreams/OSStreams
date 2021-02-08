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

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleAttributeType;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleType;
import java.lang.reflect.Type;
import java.util.List;

public class TopologyTupleTypeSerializer implements JsonSerializer<TopologyTupleType> {

  @Override
  public JsonElement serialize(
      TopologyTupleType topTuple, Type type, JsonSerializationContext context) {
    JsonObject tuple = new JsonObject();
    JsonArray tupleType = new JsonArray();
    List<TopologyTupleAttributeType> attrs = topTuple.getAttributes();
    for (TopologyTupleAttributeType attr : attrs) {
      JsonObject jsonAttr = new JsonObject();
      jsonAttr.add(attr.getName(), context.serialize(attr.getType()));
      tupleType.add(jsonAttr);
    }
    tuple.add("tuple", tupleType);
    return tuple;
  }
}
