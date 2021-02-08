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
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import io.fabric8.mockwebserver.crud.AttributeSet;
import io.fabric8.zjsonpatch.JsonDiff;
import io.fabric8.zjsonpatch.JsonPatch;
import java.io.IOException;
import java.time.Instant;
import java.util.LinkedList;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.Delayed;
import java.util.concurrent.TimeUnit;
import lombok.Getter;
import lombok.var;

class ResourceLog implements Delayed {
  private static final Integer GRACE_PERIOD_IN_SECS = 1;
  private static final Integer RETAIN_PERIOD_IN_SECS = 1;

  private final ObjectMapper mapper;
  private final LinkedList<ResourceEntry> resourceEntries;

  private long deletionTime;
  @Getter private String propagationPolicy;

  private String updateVersion(Integer resourceVersion, String s) {
    try {
      var root = mapper.readTree(s);
      var meta = (ObjectNode) root.get("metadata");
      meta.put("resourceVersion", resourceVersion);
      return mapper.writeValueAsString(root);
    } catch (IOException ignored) {
      return s;
    }
  }

  private String updateCreationTimestampAndVersionAndUuid(
      UUID uuid, Integer resourceVersion, String s) {
    try {
      var root = mapper.readTree(s);
      var meta = (ObjectNode) root.get("metadata");
      meta.put("creationTimestamp", TimeUtils.getInstance().toString(Instant.now()));
      meta.put("resourceVersion", resourceVersion);
      meta.put("uid", uuid.toString());
      return mapper.writeValueAsString(root);
    } catch (IOException ignored) {
      return s;
    }
  }

  private String updateDeletionTimestampGracePeriodAndVersion(Integer resourceVersion, String s) {
    try {
      var root = mapper.readTree(s);
      var meta = (ObjectNode) root.get("metadata");
      meta.put("deletionTimestamp", TimeUtils.getInstance().toString(Instant.now()));
      meta.put("deletionGracePeriodSeconds", GRACE_PERIOD_IN_SECS);
      meta.put("resourceVersion", resourceVersion);
      return mapper.writeValueAsString(root);
    } catch (IOException ignored) {
      return s;
    }
  }

  ResourceLog(
      UUID uuid, String kind, Integer resourceVersion, AttributeSet attributeSet, String content) {
    mapper = new ObjectMapper();
    deletionTime = 0;
    propagationPolicy = "Background";
    var next = updateCreationTimestampAndVersionAndUuid(uuid, resourceVersion, content);
    resourceEntries =
        new LinkedList<ResourceEntry>() {
          {
            add(
                new ResourceEntry(
                    uuid, kind, resourceVersion, EResourceAction.ADDITION, attributeSet, next));
          }
        };
  }

  UUID getUuid() {
    return resourceEntries.getFirst().getUuid();
  }

  Integer getVersion() {
    return resourceEntries.getFirst().getVersion();
  }

  String getKind() {
    return resourceEntries.getFirst().getKind();
  }

  String getContent() {
    return resourceEntries.getFirst().getContent();
  }

  JsonNode getContentAsTree() {
    return resourceEntries.get(0).getContentAsTree(mapper);
  }

  AttributeSet getFeatures() {
    return resourceEntries.getFirst().getFeatures();
  }

  EResourceAction getAction() {
    return resourceEntries.getFirst().getAction();
  }

  List<ResourceEntry> getLog(Integer resourceVersion) {
    var result = new LinkedList<ResourceEntry>();
    var iterator = resourceEntries.descendingIterator();
    while (iterator.hasNext()) {
      var next = iterator.next();
      if (next.getVersion() > resourceVersion) {
        result.add(next);
      }
    }
    return result;
  }

  private JsonNode computeUpdate(Integer version, JsonNode from, JsonNode patch) {
    var fromMeta = ((ObjectNode) from.get("metadata"));
    var patchMeta = ((ObjectNode) patch.get("metadata"));
    patchMeta.put("creationTimestamp", fromMeta.get("creationTimestamp").asText());
    patchMeta.put("resourceVersion", version.toString());
    patchMeta.put("uid", fromMeta.get("uid").asText());
    return JsonDiff.asJson(from, patch);
  }

  void patch(Integer resourceVersion, AttributeSet attributeSet, String payload)
      throws IOException {
    var head = resourceEntries.getFirst();
    var uuid = head.getUuid();
    var content = mapper.readTree(head.getContent());
    var patch = mapper.readTree(payload);
    var result = JsonPatch.apply(patch, content);
    ((ObjectNode) result.get("metadata")).put("resourceVersion", resourceVersion);
    var update = mapper.writerWithDefaultPrettyPrinter().writeValueAsString(result);
    var next =
        new ResourceEntry(
            uuid,
            head.getKind(),
            resourceVersion,
            EResourceAction.MODIFICATION,
            attributeSet,
            update);
    resourceEntries.addFirst(next);
  }

  void update(Integer resourceVersion, AttributeSet attributeSet, String payload)
      throws IOException {
    var head = resourceEntries.getFirst();
    var uuid = head.getUuid();
    var content = mapper.readTree(head.getContent());
    var patch = mapper.readTree(payload);
    var update =
        mapper.writeValueAsString(
            JsonPatch.apply(computeUpdate(resourceVersion, content, patch), content));
    var next =
        new ResourceEntry(
            uuid,
            head.getKind(),
            resourceVersion,
            EResourceAction.MODIFICATION,
            attributeSet,
            update);
    resourceEntries.addFirst(next);
  }

  void prepareForDeletion(Integer resourceVersion, String propagationPolicy) {
    var head = resourceEntries.getFirst();
    var uuid = head.getUuid();
    var content = updateDeletionTimestampGracePeriodAndVersion(resourceVersion, head.getContent());
    var next =
        new ResourceEntry(
            uuid,
            head.getKind(),
            resourceVersion,
            EResourceAction.MODIFICATION,
            head.getFeatures(),
            content);
    deletionTime = System.currentTimeMillis() + GRACE_PERIOD_IN_SECS * 1000;
    this.propagationPolicy = propagationPolicy;
    resourceEntries.addFirst(next);
  }

  void delete(Integer resourceVersion) {
    var head = resourceEntries.getFirst();
    var uuid = head.getUuid();
    var content = updateVersion(resourceVersion, head.getContent());
    var next =
        new ResourceEntry(
            uuid,
            head.getKind(),
            resourceVersion,
            EResourceAction.DELETION,
            head.getFeatures(),
            content);
    deletionTime = System.currentTimeMillis() + RETAIN_PERIOD_IN_SECS * 1000;
    resourceEntries.addFirst(next);
  }

  @Override
  public long getDelay(TimeUnit unit) {
    long diff = deletionTime - System.currentTimeMillis();
    return unit.convert(diff, TimeUnit.MILLISECONDS);
  }

  @Override
  public int compareTo(Delayed o) {
    return (int) (getDelay(TimeUnit.MILLISECONDS) - o.getDelay(TimeUnit.MILLISECONDS));
  }
}
