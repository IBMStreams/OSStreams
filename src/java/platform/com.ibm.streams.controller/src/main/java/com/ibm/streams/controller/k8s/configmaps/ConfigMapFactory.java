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

package com.ibm.streams.controller.k8s.configmaps;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CONTENT_ID_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_PE_LABEL_VALUE;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.k8s.utils.ConfigMapUtils;
import io.fabric8.kubernetes.api.model.ConfigMap;
import io.fabric8.kubernetes.api.model.DeletionPropagation;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.math.BigInteger;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import lombok.var;
import org.apache.commons.lang.RandomStringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConfigMapFactory {

  private static final Logger LOGGER = LoggerFactory.getLogger(ConfigMapFactory.class);

  private final KubernetesClient client;
  private final ConfigMapStore store;
  private final ConcurrentHashMap<String, Boolean> names;

  public ConfigMapFactory(KubernetesClient client, ConfigMapStore store) {
    this.client = client;
    this.store = store;
    this.names = new ConcurrentHashMap<>();
  }

  private boolean allocateUniqueName(String name) {
    return names.putIfAbsent(name, true) != null;
  }

  private String getUniqueConfigMapName(String peName) {
    String podName;
    do {
      var suffix = RandomStringUtils.random(5, true, true).toLowerCase();
      podName = peName + "-" + suffix;
    } while (allocateUniqueName(podName));
    return podName;
  }

  void registerConfigMapName(String name) {
    this.names.put(name, true);
  }

  void unregisterConfigMapName(String name) {
    this.names.remove(name);
  }

  public void addConfigMap(Job job, Map<String, String> data) {
    /*
     * Build owner reference.
     */
    var or = new OwnerReference();
    or.setApiVersion(STREAMS_CRD_GROUP + '/' + STREAMS_CRD_VERSION);
    or.setKind("Job");
    or.setName(job.getMetadata().getName());
    or.setUid(job.getMetadata().getUid());
    or.setController(true);
    or.setBlockOwnerDeletion(true);
    /*
     * Build the labels.
     */
    var labels = new HashMap<String, String>();
    labels.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    labels.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    labels.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    /*
     * Build the config map.
     */
    var annotations = new HashMap<String, String>();
    addConfigMap(
        job.getMetadata().getName(),
        job.getMetadata().getNamespace(),
        or,
        annotations,
        labels,
        data);
  }

  public void addConfigMap(Job job, String peName, BigInteger peId, String aadl, String hash) {
    /*
     * Build the data map.
     */
    var data = new HashMap<String, String>();
    ConfigMapUtils.setAadl(data, aadl);
    /*
     * Build owner reference.
     */
    var or = new OwnerReference();
    or.setApiVersion(STREAMS_CRD_GROUP + '/' + STREAMS_CRD_VERSION);
    or.setKind("Job");
    or.setName(job.getMetadata().getName());
    or.setUid(job.getMetadata().getUid());
    or.setController(true);
    or.setBlockOwnerDeletion(true);
    /*
     * Build the annotations.
     */
    var annotations = new HashMap<String, String>();
    annotations.put(STREAMS_CONTENT_ID_ANNOTATION_KEY, hash);
    /*
     * Build the labels.
     */
    var labels = new HashMap<String, String>();
    labels.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    labels.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    labels.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    labels.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_PE_LABEL_VALUE);
    labels.put(STREAMS_PE_LABEL_KEY, peId.toString());
    /*
     * Check if a ConfigMap with this hash already exists. If it does, make sure we re-use
     * the existing name for that ConfigMap. Otherwise, generate a new unique name.
     */
    var name =
        store.getConfigMapNameForContentId(hash).orElseGet(() -> getUniqueConfigMapName(peName));
    /*
     * Build the config map.
     */
    addConfigMap(name, job.getMetadata().getNamespace(), or, annotations, labels, data);
  }

  private void addConfigMap(
      String name,
      String ns,
      OwnerReference owner,
      Map<String, String> annotations,
      Map<String, String> labels,
      Map<String, String> data) {
    /*
     * Build the config map metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(name);
    meta.setNamespace(ns);
    meta.setOwnerReferences(Collections.singletonList(owner));
    meta.setAnnotations(annotations);
    meta.setLabels(labels);
    /*
     * Build the config map.
     */
    var map = new ConfigMap();
    map.setMetadata(meta);
    map.setData(data);
    /*
     * Create the configmap.
     */
    LOGGER.debug("ADD {}", meta.getName());
    client.configMaps().inNamespace(ns).createOrReplace(map);
  }

  public void deleteConfigMap(ConfigMap cm) {
    LOGGER.debug("DEL {}", cm.getMetadata().getName());
    client.configMaps().delete(cm);
  }

  public void deleteConfigMaps(Job job) {
    /* FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745 */
    var list =
        client
            .configMaps()
            .inNamespace(job.getMetadata().getNamespace())
            .withLabel(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName())
            .list();
    client.resourceList(list).withPropagationPolicy(DeletionPropagation.BACKGROUND).delete();
  }
}
