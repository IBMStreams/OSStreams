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

package com.ibm.streams.controller.k8s.utils;

import io.fabric8.kubernetes.api.model.*;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.time.Instant;
import java.util.concurrent.ConcurrentHashMap;
import lombok.var;

public class EventUtils {

  private static ConcurrentHashMap<String, Long> eventLog = new ConcurrentHashMap<>();

  public static void sendEvent(
      KubernetesClient client,
      String component,
      String ns,
      HasMetadata rsrc,
      String reason,
      String type,
      String message) {
    /*
     * Grab the next event name for the resource.
     */
    var rsrcName = rsrc.getMetadata().getName();
    Long id = eventLog.compute(rsrcName, (k, v) -> (v == null) ? 0 : v + 1);
    /*
     * Build the object reference.
     */
    var or = new ObjectReference();
    or.setApiVersion(rsrc.getApiVersion());
    or.setKind(rsrc.getKind());
    or.setName(rsrc.getMetadata().getName());
    or.setNamespace(rsrc.getMetadata().getNamespace());
    or.setResourceVersion(rsrc.getMetadata().getResourceVersion());
    or.setUid(rsrc.getMetadata().getUid());
    /*
     * Build the event source.
     */
    var source = new EventSource();
    source.setComponent(component);
    /*
     * Build the event time.
     */
    var time = TimeUtils.getInstance().toString(Instant.now());
    /*
     * Build the metadata.
     */
    ObjectMeta meta = new ObjectMeta();
    meta.setName(rsrcName + "." + id);
    meta.setNamespace(ns);
    /*
     * Build the event.
     */
    var event = new Event();
    event.setApiVersion("v1");
    event.setCount(1);
    event.setFirstTimestamp(time);
    event.setInvolvedObject(or);
    event.setKind("Event");
    event.setLastTimestamp(time);
    event.setMessage(message);
    event.setMetadata(meta);
    event.setReason(reason);
    event.setSource(source);
    event.setType(type);
    /*
     * Send the event.
     */
    client.events().inNamespace(ns).createOrReplace(event);
  }
}
