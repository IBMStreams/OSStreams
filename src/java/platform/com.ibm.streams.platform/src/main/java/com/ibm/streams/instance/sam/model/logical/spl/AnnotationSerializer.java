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
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import java.lang.reflect.Type;
import java.util.List;

public class AnnotationSerializer implements JsonSerializer<Annotation> {

  @Override
  public JsonElement serialize(Annotation annotation, Type type, JsonSerializationContext context) {
    JsonObject obj = new JsonObject();
    String tag = annotation.getTag();
    obj.add("tag", context.serialize(tag));
    Boolean isInternal = annotation.isInternal();
    obj.add("internal", context.serialize(isInternal));
    List<AnnotationKeyValue> keyValues = annotation.getKeyValues();
    JsonObject kvs = new JsonObject();
    for (AnnotationKeyValue keyValue : keyValues) {
      kvs.add(keyValue.getKey(), context.serialize(keyValue.getValue()));
    }
    obj.add("keyValues", kvs);
    return obj;
  }
}
