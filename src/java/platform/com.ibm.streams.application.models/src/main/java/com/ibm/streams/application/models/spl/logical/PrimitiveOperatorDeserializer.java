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
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;

public class PrimitiveOperatorDeserializer implements JsonDeserializer<PrimitiveOperator> {

  private String deserializeParamObject(JsonObject parameterValue) {
    // This should result in the form of a tuple literal
    StringBuilder sb = new StringBuilder();
    sb.append("{");
    boolean outerFirst = true;
    for (Entry<String, JsonElement> entry : parameterValue.entrySet()) {
      if (!outerFirst) {
        sb.append(",");
      }
      outerFirst = false;
      sb.append(entry.getKey() + ":");
      if (entry.getValue().isJsonArray()) {
        sb.append("[");
        JsonArray array = entry.getValue().getAsJsonArray();
        boolean first = true;
        for (int i = 0; i < array.size(); ++i) {
          if (!first) {
            sb.append(",");
          }
          first = false;
          sb.append(array.get(i).getAsString());
        }
        sb.append("]");
      } else {
        assert (entry.getValue().isJsonPrimitive());
        sb.append(entry.getValue().getAsString());
      }
    }
    sb.append("}");
    return sb.toString();
  }

  @Override
  public PrimitiveOperator deserialize(
      JsonElement json, Type type, JsonDeserializationContext context) throws JsonParseException {

    JsonObject obj = json.getAsJsonObject();
    BigInteger definitionIndex = obj.get("definitionIndex").getAsBigInteger();
    BigInteger sourceIndex = obj.get("sourceIndex").getAsBigInteger();
    BigInteger index = obj.get("index").getAsBigInteger();
    BigInteger logicalIndex = null;
    if (obj.has("logicalIndex")) {
      logicalIndex = obj.get("logicalIndex").getAsBigInteger();
    }
    String name = obj.get("name").getAsString();
    String logicalName = null;
    if (obj.has("logicalName")) {
      logicalName = obj.get("logicalName").getAsString();
    }

    List<PrimitiveInputPort> inputPorts = null;
    JsonElement iPorts = obj.get("inputPorts");
    if (iPorts != null) {
      inputPorts = new ArrayList<>();
      JsonArray ports = iPorts.getAsJsonArray();
      for (JsonElement port : ports) {
        PrimitiveInputPort iPort = context.deserialize(port, PrimitiveInputPort.class);
        inputPorts.add(iPort);
      }
    }

    JsonElement oPorts = obj.get("outputPorts");
    List<PrimitiveOutputPort> outputPorts = null;
    if (oPorts != null) {
      outputPorts = new ArrayList<>();
      JsonArray ports = oPorts.getAsJsonArray();
      for (JsonElement port : ports) {
        PrimitiveOutputPort oPort = context.deserialize(port, PrimitiveOutputPort.class);
        outputPorts.add(oPort);
      }
    }

    List<Parameter> params = null;
    JsonElement parameters = obj.get("params");
    if (parameters != null) {
      params = new ArrayList<>();
      assert (parameters.isJsonObject());
      JsonObject parms = parameters.getAsJsonObject();
      for (Entry<String, JsonElement> entry : parms.entrySet()) {
        String parameterName = entry.getKey();
        JsonElement parameterValue = entry.getValue();
        if (parameterValue.isJsonArray()) {
        } else if (parameterValue.isJsonObject()) {
          params.add(new Parameter(parameterName, parameterValue));
        } else {
          assert (parameterValue.isJsonPrimitive());
          params.add(new Parameter(parameterName, parameterValue));
        }
      }
    }

    List<Annotation> annotations = null;
    JsonElement annots = obj.get("annotations");
    if (annots != null) {
      annotations = new ArrayList<>();
      assert (annots.isJsonArray());
      for (JsonElement annot : annots.getAsJsonArray()) {
        Annotation annotation = context.deserialize(annot, Annotation.class);
        annotations.add(annotation);
      }
    }

    return new PrimitiveOperator(
        definitionIndex,
        sourceIndex,
        index,
        logicalIndex,
        name,
        logicalName,
        inputPorts,
        outputPorts,
        params,
        annotations);
  }
}
