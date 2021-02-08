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

package com.ibm.streams.controller.k8s.pods;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CONTENT_ID_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_LAUNCH_COUNT_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_ZOMBIE_LABEL_VALUE;

import com.ibm.streams.controller.crds.pes.ProcessingElement;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.api.model.Pod;
import io.fabric8.kubernetes.api.model.PodSpec;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.util.Collections;
import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;
import lombok.var;
import org.apache.commons.lang.RandomStringUtils;

public class PodFactory {

  private final KubernetesClient client;
  private final ConcurrentHashMap<String, Boolean> names;

  public PodFactory(KubernetesClient client, PodStore store) {
    this.client = client;
    this.names = new ConcurrentHashMap<>();
  }

  /**
   * Synchronously allocate a scope-wide unique name.
   *
   * @param name the name to allocate
   * @return true if the name already exists
   */
  private boolean allocateUniquePodName(String name) {
    return names.putIfAbsent(name, true) != null;
  }

  private String getUniquePodName(ProcessingElement pe) {
    String podName;
    do {
      var suffix = RandomStringUtils.random(5, true, true).toLowerCase();
      podName = pe.getMetadata().getName() + "-" + suffix;
    } while (allocateUniquePodName(podName));
    return podName;
  }

  /**
   * Register a pod name as a reaction to an onAddition() event.
   *
   * @param name the pod name
   */
  public void registerPodName(String name) {
    this.names.put(name, true);
  }

  /**
   * Unregister a pod name as a reaction to an onDelete() event.
   *
   * @param name the pod name
   */
  public void unregisterPodName(String name) {
    this.names.remove(name);
  }

  public void addPod(ProcessingElement pe, PodSpec spec) {
    /*
     * Allocate a unique pod name.
     */
    var name = getUniquePodName(pe);
    /*
     * Build owner reference.
     */
    var or = new OwnerReference();
    or.setApiVersion(STREAMS_CRD_GROUP + '/' + STREAMS_CRD_VERSION);
    or.setKind("ProcessingElement");
    or.setName(pe.getMetadata().getName());
    or.setUid(pe.getMetadata().getUid());
    or.setController(true);
    or.setBlockOwnerDeletion(true);
    /*
     * Build the annotations.
     */
    var annotations = new HashMap<String, String>();
    annotations.put(STREAMS_CONTENT_ID_ANNOTATION_KEY, pe.getSpec().getContentId());
    /*
     * Build the labels.
     */
    var labels = new HashMap<String, String>(pe.getMetadata().getLabels());
    labels.put(STREAMS_LAUNCH_COUNT_LABEL_KEY, Integer.toString(pe.getSpec().getLaunchCount()));
    /*
     * Build the pod metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(name);
    meta.setNamespace(pe.getMetadata().getNamespace());
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setAnnotations(annotations);
    meta.setLabels(labels);
    /*
     * Build the pod.
     */
    var pod = new Pod();
    pod.setMetadata(meta);
    pod.setSpec(spec);
    /*
     * Create the pod.
     */
    client.pods().inNamespace(pe.getMetadata().getNamespace()).createOrReplace(pod);
  }

  public void markFailedPodAsZombie(ProcessingElement pe, Pod pod) {
    /*
     * Update the labels.
     */
    var labels = new HashMap<String, String>(pod.getMetadata().getLabels());
    labels.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_ZOMBIE_LABEL_VALUE);
    /*
     * Build the pod metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(pod.getMetadata().getName());
    meta.setNamespace(pe.getMetadata().getNamespace());
    meta.setOwnerReferences(pod.getMetadata().getOwnerReferences());
    meta.setLabels(labels);
    meta.setAnnotations(pod.getMetadata().getAnnotations());
    /*
     * Build the pod.
     */
    var newPod = new Pod();
    newPod.setMetadata(meta);
    newPod.setSpec(pod.getSpec());
    /*
     * Create the pod.
     */
    client.pods().inNamespace(pe.getMetadata().getNamespace()).createOrReplace(newPod);
  }

  public void updatePodGenerationId(ProcessingElement pe, Pod pod) {
    /*
     * Update the labels.
     */
    var labels = new HashMap<String, String>(pod.getMetadata().getLabels());
    labels.put(
        STREAMS_GENERATION_LABEL_KEY,
        pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY));
    /*
     * Build the pod metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(pod.getMetadata().getName());
    meta.setNamespace(pe.getMetadata().getNamespace());
    meta.setOwnerReferences(pod.getMetadata().getOwnerReferences());
    meta.setLabels(labels);
    meta.setAnnotations(pod.getMetadata().getAnnotations());
    /*
     * Build the pod.
     */
    var newPod = new Pod();
    newPod.setMetadata(meta);
    newPod.setSpec(pod.getSpec());
    /*
     * Create the pod.
     */
    client.pods().inNamespace(pe.getMetadata().getNamespace()).createOrReplace(newPod);
  }
}
