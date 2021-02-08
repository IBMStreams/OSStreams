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

package com.ibm.streams.controller.crds.pes;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_API_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CR_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_PE_LABEL_VALUE;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.utils.ObjectUtils;
import io.fabric8.kubernetes.api.model.DeletionPropagation;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.dsl.base.CustomResourceDefinitionContext;
import io.fabric8.kubernetes.internal.KubernetesDeserializer;
import java.util.Collections;
import java.util.HashMap;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ProcessingElementFactory {

  private static final Logger LOGGER = LoggerFactory.getLogger(ProcessingElementFactory.class);

  private static final String STREAMS_PE_PLURAL_NAME = "streamspes";
  public static final String STREAMS_PE_CRD_NAME = STREAMS_PE_PLURAL_NAME + "." + STREAMS_CRD_GROUP;

  private final KubernetesClient client;
  private final CustomResourceDefinitionContext context;

  public ProcessingElementFactory(KubernetesClient client) {
    /*
     * Save the client handle.
     */
    this.client = client;
    /*
     * Pre-register our CRD signature with the embedded JSON deserializer. This is a required step.
     *
     * See: fabric8io/kubernetes-client#1099
     */
    KubernetesDeserializer.registerCustomKind(
        STREAMS_API_VERSION, "ProcessingElement", ProcessingElement.class);
    /*
     * Look for the Pe CRD.
     */
    this.context =
        client.customResourceDefinitions().list().getItems().stream()
            .filter(e -> e.getMetadata().getName().equals(STREAMS_PE_CRD_NAME))
            .findFirst()
            .map(CustomResourceDefinitionContext::fromCrd)
            .orElseThrow(RuntimeException::new);
  }

  public static CustomResourceDefinitionContext getContext(KubernetesClient client) {
    /*
     * Pre-register our CRD signature with the embedded JSON deserializer. This is a required step.
     *
     * See: fabric8io/kubernetes-client#1099
     */
    KubernetesDeserializer.registerCustomKind(
        STREAMS_API_VERSION, "ProcessingElement", ProcessingElement.class);
    /*
     * Look for the Pe CRD.
     */
    return client.customResourceDefinitions().list().getItems().stream()
        .filter(e -> e.getMetadata().getName().equals(STREAMS_PE_CRD_NAME))
        .findFirst()
        .map(CustomResourceDefinitionContext::fromCrd)
        .orElseThrow(RuntimeException::new);
  }

  public CustomResourceDefinitionContext getContext() {
    return context;
  }

  /*
   * Add/delete PEs, called by the Job launcher and the Job state machine.
   */

  public void addPe(Job job, String name, ProcessingElementSpec spec) {
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
    annotations.put(
        STREAMS_APP_NAME_ANNOTATION_KEY,
        job.getMetadata().getAnnotations().get(STREAMS_APP_NAME_ANNOTATION_KEY));
    annotations.put(
        STREAMS_APP_SCOPE_ANNOTATION_KEY,
        job.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY));
    /*
     * Build the labels.
     */
    var labels = new HashMap<String, String>();
    labels.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    labels.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    labels.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    labels.put(STREAMS_PE_LABEL_KEY, spec.getId().toString());
    labels.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_PE_LABEL_VALUE);
    labels.put(STREAMS_CR_LABEL_KEY, spec.getRegions() == null ? "false" : "true");
    /*
     * Build the PE metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(name);
    meta.setNamespace(job.getMetadata().getNamespace());
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setAnnotations(annotations);
    meta.setLabels(labels);
    /*
     * Build the PE.
     */
    var pe = new ProcessingElement();
    pe.setMetadata(meta);
    pe.setSpec(spec);
    /*
     * Create the PE.
     */
    LOGGER.debug("ADD {}", pe.getMetadata().getName());
    client
        .customResources(
            this.context,
            ProcessingElement.class,
            ProcessingElementList.class,
            DoneableProcessingElement.class)
        .inNamespace(job.getMetadata().getNamespace())
        .createOrReplace(pe);
  }

  public void deletePe(ProcessingElement pe) {
    LOGGER.debug("DEL {}", pe.getMetadata().getName());
    /*
     * We need to make sure that the deletion uses cascading garbage collection.
     *
     * See https://kubernetes.io/docs/concepts/workloads/controllers/garbage-collection/#controlling-how-the-garbage-collector-deletes-dependents
     * for more information about garbage collection.
     *
     * NOTE(xrg) It is possible, albeit not required, to use the Foreground propagation policy here.
     */
    client
        .customResources(
            this.context,
            ProcessingElement.class,
            ProcessingElementList.class,
            DoneableProcessingElement.class)
        .inNamespace(pe.getMetadata().getNamespace())
        .withName(pe.getMetadata().getName())
        .withPropagationPolicy(DeletionPropagation.BACKGROUND)
        .delete();
  }

  public void deletePes(Job job) {
    /* FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745 */
    var list =
        client
            .customResources(
                this.context,
                ProcessingElement.class,
                ProcessingElementList.class,
                DoneableProcessingElement.class)
            .inNamespace(job.getMetadata().getNamespace())
            .withLabel(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName())
            .list();
    client.resourceList(list).withPropagationPolicy(DeletionPropagation.BACKGROUND).delete();
  }

  /*
   * Update PEs, called by the PE coordinator.
   */

  public void updatePe(ProcessingElement pe, int launchCount) {
    /*
     * Duplicate the PE.
     */
    ObjectUtils.deepCopy(pe, ProcessingElement.class)
        .ifPresent(
            target -> {
              /*
               * Update the spec.
               */
              target.getSpec().setLaunchCount(launchCount);
              /*
               * Patch the PE.
               */
              LOGGER.debug("UPD {}", pe.getMetadata().getName());
              client
                  .customResources(
                      this.context,
                      ProcessingElement.class,
                      ProcessingElementList.class,
                      DoneableProcessingElement.class)
                  .inNamespace(pe.getMetadata().getNamespace())
                  .withName(pe.getMetadata().getName())
                  .patch(target);
            });
  }

  public void updatePe(ProcessingElement pe, Connectivity connectivity) {
    /*
     * Duplicate the PE.
     */
    ObjectUtils.deepCopy(pe, ProcessingElement.class)
        .ifPresent(
            target -> {
              /*
               * Update the spec.
               */
              target.getSpec().setConnectivity(connectivity);
              /*
               * Patch the PE.
               */
              LOGGER.debug("UPD {}", pe.getMetadata().getName());
              client
                  .customResources(
                      this.context,
                      ProcessingElement.class,
                      ProcessingElementList.class,
                      DoneableProcessingElement.class)
                  .inNamespace(pe.getMetadata().getNamespace())
                  .withName(pe.getMetadata().getName())
                  .patch(target);
            });
  }

  public void updatePe(Job job, ProcessingElement pe) {
    /*
     * Duplicate the PE.
     */
    ObjectUtils.deepCopy(pe, ProcessingElement.class)
        .ifPresent(
            target -> {
              /*
               * Update the generation ID.
               */
              target
                  .getMetadata()
                  .getLabels()
                  .put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
              /*
               * Patch the PE.
               */
              LOGGER.debug("UPD {}", pe.getMetadata().getName());
              client
                  .customResources(
                      this.context,
                      ProcessingElement.class,
                      ProcessingElementList.class,
                      DoneableProcessingElement.class)
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(pe.getMetadata().getName())
                  .patch(target);
            });
  }

  public void updatePe(Job job, ProcessingElement pe, String contentId, int launchCount) {
    /*
     * Duplicate the PE.
     */
    ObjectUtils.deepCopy(pe, ProcessingElement.class)
        .ifPresent(
            target -> {
              /*
               * Update the generation ID.
               */
              target
                  .getMetadata()
                  .getLabels()
                  .put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
              /*
               * Update the spec.
               */
              target.getSpec().setContentId(contentId);
              target.getSpec().setLaunchCount(launchCount);
              /*
               * Patch the PE.
               */
              LOGGER.debug("UPD {}", pe.getMetadata().getName());
              client
                  .customResources(
                      this.context,
                      ProcessingElement.class,
                      ProcessingElementList.class,
                      DoneableProcessingElement.class)
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(pe.getMetadata().getName())
                  .patch(target);
            });
  }
}
