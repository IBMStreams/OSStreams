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

public class MapTypeSerializer implements JsonSerializer<MapType> {

  @Override
  public JsonElement serialize(MapType mapType, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    JsonObject map = new JsonObject();
    map.add("keyType", context.serialize(mapType.getKeyType()));
    map.add("valueType", context.serialize(mapType.getValueType()));
    if (mapType.getBound() != null) {
      map.add("bound", context.serialize(mapType.getBound()));
    }
    jsonObject.add("map", map);
    return jsonObject;
  }
}
