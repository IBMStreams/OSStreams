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
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;

public class TypeDeserializer implements JsonDeserializer<Type> {

  @Override
  public Type deserialize(
      JsonElement element, java.lang.reflect.Type type, JsonDeserializationContext context)
      throws JsonParseException {
    if (element.isJsonObject()) {
      JsonObject obj = element.getAsJsonObject();
      if (obj.has("tuple")) {
        List<Attribute> attrs = new ArrayList<>();
        JsonArray tuple = obj.get("tuple").getAsJsonArray();
        for (int i = 0; i < tuple.size(); ++i) {
          JsonObject attr = tuple.get(i).getAsJsonObject();
          for (Entry<String, JsonElement> entry : attr.entrySet()) {
            String attrName = entry.getKey();
            Type attrType = context.deserialize(entry.getValue(), Type.class);
            Attribute newAttr = new Attribute(attrName, attrType);
            attrs.add(newAttr);
          }
        }
        return new TupleType(attrs);
      } else if (obj.has("map")) {
        JsonObject map = obj.get("map").getAsJsonObject();
        Type keyType = context.deserialize(map.get("keyType"), Type.class);
        Type valueType = context.deserialize(map.get("valueType"), Type.class);
        BigInteger bound = null;
        if (map.has("bound")) {
          bound = map.get("bound").getAsBigInteger();
        }
        return new MapType(keyType, valueType, bound);
      } else if (obj.has("set")) {
        JsonObject set = obj.get("set").getAsJsonObject();
        Type elementType = context.deserialize(set.get("elementType"), Type.class);
        BigInteger bound = null;
        if (set.has("bound")) {
          bound = set.get("bound").getAsBigInteger();
        }
        return new SetType(elementType, bound);
      } else if (obj.has("list")) {
        JsonObject list = obj.get("list").getAsJsonObject();
        Type elementType = context.deserialize(list.get("elementType"), Type.class);
        BigInteger bound = null;
        if (list.has("bound")) {
          bound = list.get("bound").getAsBigInteger();
        }
        return new ListType(elementType, bound);
      } else if (obj.has("underlyingType")) {
        Type underlyingType = context.deserialize(obj.get("underlyingType"), Type.class);
        return new OptionalType(underlyingType);
      }
    } else if (element.isJsonPrimitive()) {
      return new PrimitiveType(element.getAsJsonPrimitive().getAsString());
    }
    return null;
  }
}
