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

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;
import java.lang.reflect.Type;

public class AnnnotationSerializer implements JsonSerializer<Annotation> {

  @Override
  public JsonElement serialize(Annotation annotation, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    jsonObject.add("tag", context.serialize(annotation.getTag()));
    jsonObject.add("internal", context.serialize(annotation.getIsInternal()));
    JsonObject kvs = new JsonObject();
    for (AnnotationKeyValue kv : annotation.getKeyValues()) {
      kvs.add(kv.getKey(), context.serialize(kv.getValue()));
    }
    jsonObject.add("keyValues", kvs);

    return jsonObject;
  }
}
