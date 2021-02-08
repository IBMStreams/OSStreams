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
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import java.lang.reflect.Type;

public class PrimitiveOperatorBaseSerializer {
  public JsonObject serialize(
      PrimitiveOperatorBase oper, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    jsonObject.addProperty("definitionIndex", oper.getDefinitionIndex());
    jsonObject.addProperty("sourceIndex", oper.getSourceIndex());
    jsonObject.addProperty("index", oper.getIndex());
    try {
      jsonObject.addProperty("name", oper.getFullyQualifiedName());
      if (!oper.getFullyQualifiedName().equals(oper.getFullyQualifiedLogicalName())) {
        jsonObject.addProperty("logicalName", oper.getFullyQualifiedLogicalName());
      }
    } catch (SAMInternalErrorException e) {
      e.printStackTrace();
    }

    if (oper.getIndex() != oper.getLogicalIndex()) {
      jsonObject.addProperty("logicalIndex", oper.getLogicalIndex());
    }
    if (oper.getAnnotations() != null && oper.getAnnotations().size() > 0) {
      JsonArray annotations = new JsonArray();
      for (Annotation annotation : oper.getAnnotations()) {
        JsonElement element = context.serialize(annotation);
        annotations.add(element);
      }
      jsonObject.add("annotations", annotations);
    }
    return jsonObject;
  }
}
