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

package com.ibm.streams.mock.server;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import io.fabric8.mockwebserver.crud.AttributeSet;
import java.io.IOException;
import java.util.UUID;
import lombok.Getter;

class ResourceEntry {
  @Getter private UUID uuid;
  @Getter private String kind;
  @Getter private Integer version;
  @Getter private EResourceAction action;
  @Getter private AttributeSet features;
  @Getter private String content;

  ResourceEntry(
      UUID uuid,
      String kind,
      Integer version,
      EResourceAction action,
      AttributeSet features,
      String content) {
    this.uuid = uuid;
    this.kind = kind;
    this.version = version;
    this.action = action;
    this.features = features;
    this.content = content;
  }

  JsonNode getContentAsTree(ObjectMapper mapper) {
    try {
      return mapper.readTree(content);
    } catch (IOException e) {
      return mapper.createObjectNode();
    }
  }
}
