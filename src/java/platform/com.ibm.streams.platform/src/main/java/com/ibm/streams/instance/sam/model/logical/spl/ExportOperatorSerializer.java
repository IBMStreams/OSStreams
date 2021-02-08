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
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import java.lang.reflect.Type;
import java.util.List;

public class ExportOperatorSerializer extends PrimitiveOperatorBaseSerializer
    implements JsonSerializer<ExportOperator> {
  public JsonElement serialize(ExportOperator oper, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = super.serialize(oper, type, context);
    JsonElement inputPorts = context.serialize(oper.getInputPorts());
    jsonObject.add("inputPorts", inputPorts);
    LogicalExportedStream expStream = oper.getExportedStream();
    JsonObject params = new JsonObject();
    params.add("allowFilter", context.serialize(expStream.isAllowFilter()));
    if (expStream.getName() != null) {
      params.add("streamId", context.serialize(expStream.getName()));
    }
    List<StreamProperty> properties = expStream.getProperties();
    if (properties != null) {
      JsonObject props = new JsonObject();
      for (StreamProperty prop : properties) {
        String name = prop.getName();
        String typeName = prop.getType();
        List<String> values = prop.getValues();
        if (typeName.startsWith("list")) {
          JsonArray list = new JsonArray();
          for (String s : values) {
            list.add(context.serialize(s));
          }
          props.add(name, list);
        } else {
          assert (values.size() == 1);
          props.add(name, context.serialize(values.get(0)));
        }
      }
      params.add("properties", props);
    }
    jsonObject.add("params", params);
    return jsonObject;
  }
}
