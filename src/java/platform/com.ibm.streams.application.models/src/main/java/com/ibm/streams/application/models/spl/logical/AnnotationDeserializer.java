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

import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;

public class AnnotationDeserializer implements JsonDeserializer<Annotation> {

  @Override
  public Annotation deserialize(
      JsonElement annotation, Type type, JsonDeserializationContext context)
      throws JsonParseException {
    JsonObject jsonObject = annotation.getAsJsonObject();
    Annotation a = null;
    String tag = jsonObject.get("tag").getAsString();
    Boolean isInternal = jsonObject.get("internal").getAsBoolean();
    JsonObject tuple = jsonObject.get("keyValues").getAsJsonObject();
    List<AnnotationKeyValue> keyValues = new ArrayList<>();
    for (Entry<String, JsonElement> keyValue : tuple.entrySet()) {
      String key = keyValue.getKey();
      AnnotationValue val = context.deserialize(keyValue.getValue(), AnnotationValue.class);
      keyValues.add(new AnnotationKeyValue(key, val));
    }
    a = new Annotation(tag, isInternal, keyValues);
    return a;
  }
}
