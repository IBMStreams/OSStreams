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

public class CompositeOperatorSerializer implements JsonSerializer<CompositeOperator> {

  @Override
  public JsonElement serialize(
      CompositeOperator oper, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    jsonObject.addProperty("definitionIndex", oper.getDefinitionIndex());
    jsonObject.addProperty("sourceIndex", oper.getSourceIndex());
    jsonObject.addProperty("index", oper.getIndex());
    jsonObject.addProperty("name", oper.getName());
    String logicalName = oper.getLogicalName();
    if (logicalName != null) {
      jsonObject.addProperty("logicalName", logicalName);
    }
    JsonElement inputPorts = context.serialize(oper.getInputPorts());
    jsonObject.add("inputPorts", inputPorts);
    JsonElement outputPorts = context.serialize(oper.getOutputPorts());
    jsonObject.add("outputPorts", outputPorts);
    JsonElement compositeOpers = context.serialize(oper.getCompositeOperators());
    jsonObject.add("compositeOperators", compositeOpers);
    JsonElement primitiveOpers = context.serialize(oper.getPrimitiveOperators());
    jsonObject.add("primitiveOperators", primitiveOpers);
    JsonArray jsonAnnotations = new JsonArray();
    if (oper.getAnnotations() != null) {
      for (Annotation annotation : oper.getAnnotations()) {
        JsonElement element = context.serialize(annotation);
        jsonAnnotations.add(element);
      }
    }
    jsonObject.add("annotations", jsonAnnotations);
    return jsonObject;
  }
}
