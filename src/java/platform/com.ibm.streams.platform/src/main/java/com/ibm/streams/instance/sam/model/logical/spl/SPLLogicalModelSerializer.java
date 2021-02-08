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
import com.ibm.streams.instance.sam.model.topology.Toolkit;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleType;
import java.lang.reflect.Type;
import java.math.BigInteger;
import java.util.Map.Entry;

public class SPLLogicalModelSerializer implements JsonSerializer<SPLLogicalModel> {

  public JsonElement serialize(
      SPLLogicalModel splLogicalModel, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();

    JsonArray tupleTypeDefs = new JsonArray();
    for (Entry<BigInteger, TopologyTupleType> entry : splLogicalModel.getTupleTypes().entrySet()) {
      JsonObject tupleTypeDef = new JsonObject();
      tupleTypeDef.add("index", context.serialize(entry.getKey()));
      tupleTypeDef.add("type", context.serialize(entry.getValue()));
      tupleTypeDefs.add(tupleTypeDef);
    }
    jsonObject.add("tupleTypeDefs", tupleTypeDefs);

    jsonObject.add("compositeOperDefs", context.serialize(splLogicalModel.getCompositeOperDefs()));
    JsonArray jsonPrimOperDefs = new JsonArray();
    for (Entry<BigInteger, PrimitiveOperatorDefinition> entry :
        splLogicalModel.getPrimitiveOperDefs().entrySet()) {
      JsonElement element = context.serialize(entry.getValue());
      jsonPrimOperDefs.add(element);
    }
    jsonObject.add("primitiveOperDefs", jsonPrimOperDefs);

    JsonArray jsonParallelRegions = new JsonArray();
    for (Entry<BigInteger, ParallelRegion> entry :
        splLogicalModel.getParallelRegions().getParallelRegionMap().entrySet()) {
      JsonElement jsonParallelRegion = context.serialize(entry.getValue());
      jsonParallelRegions.add(jsonParallelRegion);
    }
    jsonObject.add("parallelRegions", jsonParallelRegions);

    JsonArray jsonToolkits = new JsonArray();
    for (Entry<BigInteger, Toolkit> entry : splLogicalModel.getToolkits().entrySet()) {
      JsonElement jsonToolkit = context.serialize(entry.getValue());
      jsonToolkits.add(jsonToolkit);
    }
    jsonObject.add("toolkits", jsonToolkits);

    JsonArray jsonSourceUris = new JsonArray();
    for (Entry<BigInteger, URI> entry : splLogicalModel.getSourceURIs().entrySet()) {
      JsonElement jsonUri = context.serialize(entry.getValue());
      jsonSourceUris.add(jsonUri);
    }
    jsonObject.add("sourceUris", jsonSourceUris);

    jsonObject.add("mainComposite", context.serialize(splLogicalModel.getMainComposite()));
    return jsonObject;
  }
}
