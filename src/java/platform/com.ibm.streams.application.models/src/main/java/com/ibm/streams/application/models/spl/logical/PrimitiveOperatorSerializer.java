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

package com.ibm.streams.application.models.spl.logical;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;
import java.lang.reflect.Type;

public class PrimitiveOperatorSerializer implements JsonSerializer<PrimitiveOperator> {

  @Override
  public JsonElement serialize(
      PrimitiveOperator oper, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    jsonObject.addProperty("definitionIndex", oper.getDefinitionIndex());
    jsonObject.addProperty("sourceIndex", oper.getSourceIndex());
    jsonObject.addProperty("index", oper.getIndex());
    jsonObject.addProperty("name", oper.getName());
    if (oper.getLogicalName() != null) {
      jsonObject.addProperty("logicalName", oper.getLogicalName());
    }
    if (oper.getLogicalIndex() != null) {
      jsonObject.addProperty("logicalIndex", oper.getLogicalIndex());
    }
    if (oper.getAnnotations() != null) {
      JsonArray annotations = new JsonArray();
      for (Annotation annot : oper.getAnnotations()) {
        annotations.add(context.serialize(annot));
      }
      jsonObject.add("annotations", annotations);
    }
    if (oper.getInputPorts() != null) {
      jsonObject.add("inputPorts", context.serialize(oper.getInputPorts()));
    }

    if (oper.getOutputPorts() != null) {
      jsonObject.add("outputPorts", context.serialize(oper.getOutputPorts()));
    }
    if (oper.getParams() != null) {
      JsonObject parameters = new JsonObject();
      for (Parameter param : oper.getParams()) {
        parameters.add(param.getName(), param.getValue());
      }
      jsonObject.add("params", parameters);
    }

    return jsonObject;
  }
}
