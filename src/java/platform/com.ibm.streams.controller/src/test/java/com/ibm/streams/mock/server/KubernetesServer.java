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

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import io.fabric8.kubernetes.api.model.DeleteOptions;
import io.fabric8.kubernetes.api.model.HasMetadata;
import io.fabric8.kubernetes.api.model.Pod;
import io.fabric8.kubernetes.api.model.PodStatus;
import io.fabric8.kubernetes.client.ConfigBuilder;
import io.fabric8.kubernetes.client.DefaultKubernetesClient;
import io.fabric8.kubernetes.client.NamespacedKubernetesClient;
import io.fabric8.mockwebserver.crud.AttributeSet;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.UUID;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.DelayQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class KubernetesServer implements HttpHandler {

  private static final Logger LOGGER = LoggerFactory.getLogger(KubernetesServer.class);

  /*
   * The DeletionBroken class handles resource deletion transition steps.
   */
  private final class DeletionBroker {

    /*
     * The FinalizationRunner class handles the finalization upon deletion.
     */
    private final class FinalizationRunner implements Runnable {
      public void run() {
        while (keepRunning.get()) {
          try {
            var log = finalizationQueue.poll(250, TimeUnit.MILLISECONDS);
            if (log == null) {
              continue;
            }
            /*
             * Delete the resource.
             */
            LOGGER.debug("Delete {} with UUID {}", log.getKind(), log.getUuid().toString());
            var version = getNextResourceVersion(log.getKind());
            log.delete(version);
            deletionQueue.add(log);
            /*
             * Propagate deletion to dependents.
             */
            for (var dep : getDependents(log.getUuid())) {
              prepareForDeletion(dep, dep.getPropagationPolicy());
            }
          } catch (InterruptedException | IOException ignored) {
          }
        }
      }
    }

    /*
     * The DeletionRunner class handles the actual deletion of the resource from the in-memory store.
     */
    private final class DeletionRunner implements Runnable {
      public void run() {
        while (keepRunning.get()) {
          try {
            var log = deletionQueue.poll(250, TimeUnit.MILLISECONDS);
            if (log != null) {
              LOGGER.debug("Removing resource {}", log.getUuid());
              resources.remove(log.getUuid());
            }
          } catch (InterruptedException ignored) {
          }
        }
      }
    }

    private final AtomicBoolean keepRunning;
    private final BlockingQueue<ResourceLog> finalizationQueue;
    private final BlockingQueue<ResourceLog> deletionQueue;
    private final Thread finalizationThread;
    private final Thread deletionThread;

    private DeletionBroker() {
      this.keepRunning = new AtomicBoolean(true);
      this.finalizationQueue = new DelayQueue<>();
      this.deletionQueue = new DelayQueue<>();
      this.finalizationThread = new Thread(new FinalizationRunner());
      this.deletionThread = new Thread(new DeletionRunner());
    }

    void enqueue(ResourceLog log) {
      LOGGER.debug("Enqueue {} for finalization", log.getUuid());
      finalizationQueue.add(log);
    }

    void start() {
      this.finalizationThread.start();
      this.deletionThread.start();
    }

    void stop() {
      try {
        keepRunning.set(false);
        deletionThread.join();
        finalizationThread.join();
      } catch (InterruptedException ignored) {
      }
    }
  }

  private final Integer port;
  private final ObjectMapper mapper;
  private final AttributesExtractor attributesExtractor;
  private final HttpServer httpServer;
  private final Map<UUID, ResourceLog> resources;
  private final Map<String, AtomicInteger> versions;
  private final DeletionBroker deletionBroker;

  public KubernetesServer() throws IOException {
    this(new Random().nextInt(5000) + 10000);
  }

  public KubernetesServer(Integer port) throws IOException {
    LOGGER.info("Mock Kubernetes server listening on port: {}", port);
    this.port = port;
    this.mapper = new ObjectMapper();
    this.attributesExtractor = new AttributesExtractor();
    this.httpServer = HttpServer.create(new InetSocketAddress(port), 0);
    this.httpServer.createContext("/", this);
    this.httpServer.setExecutor(Executors.newFixedThreadPool(2));
    this.resources = new ConcurrentHashMap<>();
    this.versions = new ConcurrentHashMap<>();
    this.deletionBroker = new DeletionBroker();
  }

  private String getBody(HttpExchange ex) {
    var reader = new BufferedReader(new InputStreamReader(ex.getRequestBody()));
    return reader.lines().collect(Collectors.joining());
  }

  private Map<String, String> parseQuery(HttpExchange ex) {
    var result = new HashMap<String, String>();
    var query = ex.getRequestURI().getQuery();
    if (query != null) {
      for (var e : query.split("&")) {
        var kv = e.split("=");
        result.put(kv[0], kv[1]);
      }
    }
    return result;
  }

  private Integer getNextResourceVersion(String resourceName) {
    var version = versions.computeIfAbsent(resourceName, k -> new AtomicInteger(0));
    return version.addAndGet(1);
  }

  @SuppressWarnings("deprecation")
  private String composeWatchResponse(List<ResourceEntry> collection)
      throws JsonProcessingException {
    var result = new StringBuilder();
    for (var e : collection) {
      var node = mapper.createObjectNode();
      node.put("type", e.getAction().getValue());
      node.put("object", e.getContentAsTree(mapper));
      result.append(mapper.writeValueAsString(node));
      result.append("\n");
    }
    return result.toString();
  }

  private String composeListResponse(List<ResourceLog> collection) throws JsonProcessingException {
    var node = mapper.createObjectNode();
    node.put("apiVersion", "v1");
    node.put("kind", "List");
    var meta = node.putObject("metadata");
    var version =
        collection.stream()
            .min(Comparator.comparing(ResourceLog::getVersion))
            .map(ResourceLog::getVersion)
            .orElse(0);
    meta.put("resourceVersion", version);
    var items = node.putArray("items");
    collection.stream().map(ResourceLog::getContentAsTree).forEach(items::add);
    return mapper.writeValueAsString(node);
  }

  private boolean isWatch(Map<String, String> fields) {
    return fields.containsKey("watch") && fields.get("watch").equals("true");
  }

  private boolean returnSingletonForQuery(AttributeSet query) {
    return query.containsKey("uuid")
        || (query.containsKey("name") && query.containsKey("namespace"));
  }

  private List<ResourceLog> getDependents(UUID uuid) {
    return resources.values().stream()
        .filter(
            log -> {
              try {
                var meta = mapper.readValue(log.getContent(), HasMetadata.class);
                var refs = meta.getMetadata().getOwnerReferences();
                if (refs.isEmpty()) {
                  return false;
                }
                return refs.get(0).getUid().equals(uuid.toString());
              } catch (IOException ignored) {
                return false;
              }
            })
        .collect(Collectors.toList());
  }

  private void prepareForDeletion(ResourceLog log, String propagationPolicy) throws IOException {
    LOGGER.debug("Preparing {} with UUID {} for deletion", log.getKind(), log.getUuid().toString());
    /*
     * Grab the metadata and check if the resource is already being deleted.
     */
    var meta = mapper.readValue(log.getContent(), HasMetadata.class);
    if (meta.getMetadata().getDeletionTimestamp() != null) {
      return;
    }
    /*
     * If it's a pod, switch it to Succeeded.
     */
    if (meta.getKind().equals("Pod")) {
      var pod = mapper.readValue(log.getContent(), Pod.class);
      pod.getStatus().setPhase("Succeeded");
      var version = getNextResourceVersion(meta.getKind());
      var rsrc = mapper.writeValueAsString(pod);
      log.update(version, log.getFeatures(), rsrc);
    }
    /*
     * Delete the resource.
     */
    var version = getNextResourceVersion(log.getKind());
    log.prepareForDeletion(version, propagationPolicy);
    deletionBroker.enqueue(log);
  }

  private List<ResourceLog> doGet(AttributeSet query) {
    return resources.values().stream()
        .filter(log -> log.getFeatures().matches(query))
        .filter(log -> log.getAction() != EResourceAction.DELETION)
        .collect(Collectors.toList());
  }

  private List<ResourceEntry> doWatch(AttributeSet query, Integer resourceVersion) {
    return resources.values().stream()
        .filter(log -> log.getFeatures().matches(query))
        .flatMap(log -> log.getLog(resourceVersion).stream())
        .sorted(Comparator.comparing(ResourceEntry::getVersion))
        .collect(Collectors.toList());
  }

  public NamespacedKubernetesClient createClient() {
    /*
     * NOTE(xrg) the withWatchReconnectInterval(8) is EXTREMELY important. It is used to
     * reduce the exponential connection back-off mechanism in WatchHTTPManager to 128 millisecond.
     */
    var config =
        new ConfigBuilder()
            .withMasterUrl("http://localhost:" + port)
            .withNamespace("test")
            .withWatchReconnectInterval(4)
            .build();
    return new DefaultKubernetesClient(config);
  }

  public void start() {
    this.deletionBroker.start();
    this.httpServer.start();
  }

  public void stop() {
    this.httpServer.stop(0);
    this.deletionBroker.stop();
    this.resources.clear();
    this.versions.clear();
  }

  public void handle(HttpExchange ex) throws IOException {
    var path = ex.getRequestURI().toString();
    switch (ex.getRequestMethod().toUpperCase()) {
      case "DELETE":
        handleDelete(ex);
        break;
      case "GET":
        var fields = parseQuery(ex);
        if (isWatch(fields)) {
          handleWatch(ex, fields);
        } else {
          handleGet(ex);
        }
        break;
      case "PATCH":
        handlePatch(ex);
      case "PUT":
        handleUpdate(ex);
        break;
      case "POST":
        handleCreate(ex);
        break;
    }
    LOGGER.trace("RESP {} - {}", path, ex.getResponseCode());
  }

  private void handleCreate(HttpExchange ex) throws IOException {
    var path = ex.getRequestURI().toString();
    var rsrc = getBody(ex);
    /*
     * Unpack the resource.
     */
    var meta = mapper.readValue(rsrc, HasMetadata.class);
    LOGGER.trace("CREATE {} - {} {}", path, meta.getKind(), meta.getMetadata().getName());
    /*
     * Buit the attributes.
     */
    var attr0 = attributesExtractor.fromPath(path);
    var attr1 = attributesExtractor.fromResource(rsrc);
    var feat = AttributeSet.merge(attr0, attr1);
    /*
     * Check if there is any conflict.
     */
    if (!doGet(feat).isEmpty()) {
      LOGGER.debug("Cannot create {} {}, conflict", meta.getKind(), meta.getMetadata().getName());
      ex.sendResponseHeaders(HttpURLConnection.HTTP_CONFLICT, -1);
      return;
    }
    /*
     * If we are dealing with a pod, we need to add some kind of status.
     */
    if (meta.getKind().equals("Pod")) {
      var pod = mapper.readValue(rsrc, Pod.class);
      var status = new PodStatus();
      status.setPhase("Pending");
      pod.setStatus(status);
      rsrc = mapper.writeValueAsString(pod);
    }
    /*
     * Build the log for the resource.
     */
    var uuid = UUID.randomUUID();
    var version = getNextResourceVersion(meta.getKind());
    var log = new ResourceLog(uuid, meta.getKind(), version, feat, rsrc);
    /*
     * If it's a pod, switch it immediately to Running.
     */
    if (meta.getKind().equals("Pod")) {
      var pod = mapper.readValue(log.getContent(), Pod.class);
      pod.getStatus().setPhase("Running");
      version = getNextResourceVersion(meta.getKind());
      rsrc = mapper.writeValueAsString(pod);
      log.update(version, feat, rsrc);
    }
    /*
     * Add the log in the resources.
     */
    LOGGER.debug("Create resource {}", uuid);
    this.resources.put(uuid, log);
    /*
     * Handle the response.
     */
    ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, log.getContent().length());
    var os = ex.getResponseBody();
    os.write(log.getContent().getBytes());
    os.close();
  }

  private void handlePatch(HttpExchange ex) throws IOException {
    var path = ex.getRequestURI().toString();
    var payload = getBody(ex);
    /*
     * Parse and execute the query.
     */
    var query = attributesExtractor.extract(path);
    var items = doGet(query);
    if (items.isEmpty()) {
      ex.sendResponseHeaders(HttpURLConnection.HTTP_NOT_FOUND, -1);
      return;
    }
    /*
     * Perform the update.
     */
    try {
      var log = items.get(0);
      var content = log.getContent();
      /*
       * Update the feature.
       */
      var attr0 = attributesExtractor.fromPath(path);
      var attr1 = attributesExtractor.fromResource(content);
      var feats = AttributeSet.merge(attr0, attr1);
      /*
       * Get the metadata object.
       */
      var meta = mapper.readValue(content, HasMetadata.class);
      /*
       * If we are dealing with pods, we need to carry the status through.
       */
      if (meta.getKind().equals("Pod")) {
        var pre = mapper.readValue(content, Pod.class);
        var cur = mapper.readValue(payload, Pod.class);
        cur.setStatus(pre.getStatus());
        payload = mapper.writeValueAsString(cur);
      }
      /*
       * Update the resource.
       */
      var version = getNextResourceVersion(log.getKind());
      LOGGER.debug("Patch resource {} to version {}", log.getUuid(), version);
      log.patch(version, feats, payload);
      ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, log.getContent().length());
      var os = ex.getResponseBody();
      os.write(log.getContent().getBytes());
      os.close();
    } catch (Exception e) {
      e.printStackTrace();
      throw new RuntimeException(e);
    }
  }

  private void handleUpdate(HttpExchange ex) throws IOException {
    var path = ex.getRequestURI().toString();
    var payload = getBody(ex);
    /*
     * Parse and execute the query.
     */
    var query = attributesExtractor.extract(path);
    var items = doGet(query);
    if (items.isEmpty()) {
      ex.sendResponseHeaders(HttpURLConnection.HTTP_NOT_FOUND, -1);
      return;
    }
    /*
     * Perform the update.
     */
    try {
      var log = items.get(0);
      var content = log.getContent();
      /*
       * Update the feature.
       */
      var attr0 = attributesExtractor.fromPath(path);
      var attr1 = attributesExtractor.fromResource(content);
      var feats = AttributeSet.merge(attr0, attr1);
      /*
       * Get the metadata object.
       */
      var meta = mapper.readValue(content, HasMetadata.class);
      /*
       * If we are dealing with pods, we need to carry the status through.
       */
      if (meta.getKind().equals("Pod")) {
        var pre = mapper.readValue(content, Pod.class);
        var cur = mapper.readValue(payload, Pod.class);
        cur.setStatus(pre.getStatus());
        payload = mapper.writeValueAsString(cur);
      }
      /*
       * Update the resource.
       */
      var version = getNextResourceVersion(log.getKind());
      LOGGER.debug("Update resource {} to version {}", log.getUuid(), version);
      log.update(version, feats, payload);
      ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, log.getContent().length());
      var os = ex.getResponseBody();
      os.write(log.getContent().getBytes());
      os.close();
    } catch (Exception e) {
      e.printStackTrace();
      throw new RuntimeException(e);
    }
  }

  private void handleGet(HttpExchange ex) throws IOException {
    var path = ex.getRequestURI().toString();
    var query = attributesExtractor.extract(path);
    var items = doGet(query);
    LOGGER.trace("GET {} - {}", path, items.size());
    /*
     * If there is no match and we were asked for an exact match, return 404.
     */
    if (items.isEmpty() && returnSingletonForQuery(query)) {
      ex.sendResponseHeaders(HttpURLConnection.HTTP_NOT_FOUND, -1);
      return;
    }
    /*
     * Return the response.
     */
    var body =
        returnSingletonForQuery(query) ? items.get(0).getContent() : composeListResponse(items);
    ex.getResponseHeaders().set("Content-Type", "application/json");
    ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, body.length());
    /*
     * Write the response body.
     */
    var os = ex.getResponseBody();
    os.write(body.getBytes());
    os.close();
  }

  private void handleWatch(HttpExchange ex, Map<String, String> fields) throws IOException {
    var path = ex.getRequestURI().toString();
    var query = attributesExtractor.extract(path);
    var version =
        fields.containsKey("resourceVersion") ? Integer.parseInt(fields.get("resourceVersion")) : 0;
    var items = doWatch(query, version);
    LOGGER.trace("WATCH {} - {}", path, items.size());
    /*
     * If there is no history, return an empty payload.
     */
    if (items.isEmpty()) {
      ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, -1);
      return;
    }
    /*
     * Compute the response.
     */
    var body = composeWatchResponse(items);
    ex.getResponseHeaders().set("Content-Type", "application/json");
    if (body.isEmpty()) {
      ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, -1);
    } else {
      ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, body.length());
    }
    /*
     * Write the response body.
     */
    var os = ex.getResponseBody();
    os.write(body.getBytes());
    os.close();
  }

  private void handleDelete(HttpExchange ex) throws IOException {
    var path = ex.getRequestURI().toString();
    /*
     * Grab the propagation policy.
     */
    var propagationPolicy = "";
    try {
      var options = mapper.readValue(getBody(ex), DeleteOptions.class);
      propagationPolicy =
          options.getPropagationPolicy() == null ? "Background" : options.getPropagationPolicy();
    } catch (IOException ignored) {
      propagationPolicy = "Background";
    }
    /*
     * Perform the query.
     */
    var query = attributesExtractor.extract(path);
    var items = doGet(query);
    if (items.isEmpty()) {
      ex.sendResponseHeaders(HttpURLConnection.HTTP_NOT_FOUND, -1);
      return;
    }
    /*
     * Prepare the resource for deletion.
     */
    for (var item : items) {
      prepareForDeletion(item, propagationPolicy);
    }
    /*
     * Send the response.
     */
    ex.sendResponseHeaders(HttpURLConnection.HTTP_OK, -1);
  }
}
