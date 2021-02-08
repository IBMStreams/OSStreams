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

public class TupleTypeSerializer implements JsonSerializer<TupleType> {

  @Override
  public JsonElement serialize(
      TupleType tuple, java.lang.reflect.Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    JsonArray attributes = new JsonArray();
    for (Attribute attr : tuple.getAttributes()) {
      JsonObject attribute = new JsonObject();
      attribute.add(attr.getName(), context.serialize(attr.getType()));
      attributes.add(attribute);
    }
    jsonObject.add("tuple", attributes);
    return jsonObject;
  }
}
