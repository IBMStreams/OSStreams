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
import java.lang.reflect.Type;

public class CompositePortSerializer implements JsonSerializer<CompositePort> {

  public JsonElement serialize(CompositePort port, Type type, JsonSerializationContext context) {
    JsonObject jsonObject = new JsonObject();
    jsonObject.addProperty("index", port.getIndex());
    JsonArray incomingConnections = new JsonArray();
    for (Connection con : port.getIncomingConnections()) {
      JsonElement element = context.serialize(con);
      incomingConnections.add(element);
    }
    jsonObject.add("incomingConnections", incomingConnections);
    JsonArray outgoingConnections = new JsonArray();
    for (Connection con : port.getOutgoingConnections()) {
      JsonElement element = context.serialize(con);
      outgoingConnections.add(element);
    }
    jsonObject.add("outgoingConnections", outgoingConnections);
    return jsonObject;
  }
}
