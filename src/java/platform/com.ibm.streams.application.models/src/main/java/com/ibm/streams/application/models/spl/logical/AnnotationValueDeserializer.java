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
import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;

public class AnnotationValueDeserializer implements JsonDeserializer<AnnotationValue> {

  @Override
  public AnnotationValue deserialize(
      JsonElement value, Type type, JsonDeserializationContext context) throws JsonParseException {
    if (value.isJsonPrimitive()) {
      return new PrimitiveAnnotationValue(value.getAsJsonPrimitive().getAsString());
    } else if (value.isJsonObject()) {
      JsonObject jsonObject = value.getAsJsonObject();
      for (Entry<String, JsonElement> entry : jsonObject.entrySet()) {
        String key = entry.getKey();
        if (key.compareTo("list") == 0) {
          List<AnnotationValue> values = new ArrayList<>();
          assert (entry.getValue().isJsonArray());
          JsonArray array = entry.getValue().getAsJsonArray();
          for (int i = 0; i < array.size(); ++i) {
            JsonElement element = array.get(i);
            AnnotationValue v = context.deserialize(element, AnnotationValue.class);
            values.add(v);
          }
          return new ListAnnotationValue(values);
        } else if (key.compareTo("tuple") == 0) {
          List<AnnotationKeyValue> keyValues = new ArrayList<>();
          assert (entry.getValue().isJsonArray());
          JsonArray array = entry.getValue().getAsJsonArray();
          for (int i = 0; i < array.size(); ++i) {
            JsonElement element = array.get(i);
            assert (element.isJsonObject());
            for (Entry<String, JsonElement> e : element.getAsJsonObject().entrySet()) {
              AnnotationValue v = context.deserialize(e.getValue(), AnnotationValue.class);
              keyValues.add(new AnnotationKeyValue(e.getKey(), v));
            }
          }
          return new TupleAnnotationValue(keyValues);
        } else {
          assert (false);
        }
      }
    } else {
      assert (false);
    }
    return null;
  }
}
