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
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.PropertyBasedImport;
import java.lang.reflect.Type;
import java.math.BigInteger;
import java.util.Map;
import java.util.Map.Entry;

public class ImportOperatorSerializer extends PrimitiveOperatorBaseSerializer
    implements JsonSerializer<ImportOperator> {
  public JsonElement serialize(ImportOperator oper, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = super.serialize(oper, type, context);
    jsonObject.addProperty("inputPortCount", 0);
    JsonElement outputPorts = context.serialize(oper.getOutputPorts());
    jsonObject.add("outputPorts", outputPorts);
    JsonObject params = new JsonObject();
    ImportedStreams iStreams = oper.getImportedStreams();
    PropertyBasedImport prop = iStreams.getPropertyBasedImport();
    if (prop != null) {
      assert (prop.getSubscription() != null);
      params.add("subscription", context.serialize(prop.getSubscription()));
      if (prop.getApplicationScope() != null) {
        params.add("applicationScope", context.serialize(prop.getApplicationScope()));
      }
    } else {
      Map<BigInteger, NameBasedImport> named = iStreams.getNameBasedImports();
      assert (named.size() == 1);
      for (Entry<BigInteger, NameBasedImport> entry : named.entrySet()) {
        NameBasedImport imp = entry.getValue();
        assert (imp.getApplicationName() != null);
        params.add("applicationName", context.serialize(imp.getApplicationName()));
        assert (imp.getStreamName() != null);
        params.add("streamId", context.serialize(imp.getStreamName()));
        if (imp.getApplicationScope() != null) {
          params.add("applicationScope", context.serialize(imp.getApplicationScope()));
        }
      }
    }
    jsonObject.add("params", params);
    return jsonObject;
  }
}
