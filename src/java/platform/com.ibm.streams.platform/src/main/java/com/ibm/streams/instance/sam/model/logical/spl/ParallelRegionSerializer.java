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
import com.google.gson.JsonSerializer;
import com.ibm.streams.admin.internal.api.StreamsException;
import java.lang.reflect.Type;
import java.math.BigInteger;
import java.util.Map.Entry;

public class ParallelRegionSerializer implements JsonSerializer<ParallelRegion> {

  public JsonElement serialize(
      ParallelRegion parallelRegion, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    jsonObject.addProperty("index", parallelRegion.getIndex());
    try {
      jsonObject.addProperty("width", parallelRegion.getWidth());
    } catch (StreamsException e) {
      // Should never happen
      e.printStackTrace();
    }
    jsonObject.addProperty("operIndex", parallelRegion.getOperIndex());
    JsonElement replicas = context.serialize(parallelRegion.getReplicas());
    jsonObject.add("replicas", replicas);
    JsonArray jsonSplitters = new JsonArray();
    for (Entry<BigInteger, ParallelRegionSplitter> entry :
        parallelRegion.getSplitterMap().entrySet()) {
      JsonElement jsonSplitter = context.serialize(entry.getValue());
      jsonSplitters.add(jsonSplitter);
    }
    jsonObject.add("splitters", jsonSplitters);
    return jsonObject;
  }
}
