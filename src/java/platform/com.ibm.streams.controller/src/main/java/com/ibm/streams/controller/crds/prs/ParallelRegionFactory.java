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

package com.ibm.streams.controller.crds.prs;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_API_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.utils.ObjectUtils;
import io.fabric8.kubernetes.api.model.DeletionPropagation;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.dsl.base.CustomResourceDefinitionContext;
import io.fabric8.kubernetes.internal.KubernetesDeserializer;
import java.math.BigInteger;
import java.util.Collections;
import java.util.HashMap;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ParallelRegionFactory {

  public static final String STREAMS_HOSTPOOL_CRD_NAME = "streamsprs." + STREAMS_CRD_GROUP;
  private static final Logger LOGGER = LoggerFactory.getLogger(ParallelRegionFactory.class);

  private final KubernetesClient client;
  private final CustomResourceDefinitionContext context;

  public ParallelRegionFactory(KubernetesClient client) {
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
        STREAMS_API_VERSION, "ParallelRegion", ParallelRegion.class);
    /*
     * Look for the ParallelRegion CRD.
     */
    this.context =
        client.customResourceDefinitions().list().getItems().stream()
            .filter(e -> e.getMetadata().getName().equals(STREAMS_HOSTPOOL_CRD_NAME))
            .findFirst()
            .map(CustomResourceDefinitionContext::fromCrd)
            .orElseThrow(RuntimeException::new);
  }

  public CustomResourceDefinitionContext getContext() {
    return context;
  }

  /*
   * Add/delete ParallelRegion, called by the Job launcher and the Job state machine.
   */

  public void addParallelRegion(
      Job job, com.ibm.streams.instance.sam.model.topology.ParallelRegion pr, BigInteger width) {
    var spec = new ParallelRegionSpec();
    spec.setIndex(pr.getRegionIndex());
    spec.setAlias(pr.getName());
    spec.setWidth(width);
    addParallelRegion(job, job.getMetadata().getName() + "-" + pr.getRegionIndex(), spec);
  }

  void addParallelRegion(Job job, String name, ParallelRegionSpec spec) {
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
    labels.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    labels.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    labels.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
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
     * Build the parallel region metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(name);
    meta.setNamespace(job.getMetadata().getNamespace());
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setLabels(labels);
    meta.setAnnotations(annotations);
    /*
     * Build the parallel region.
     */
    var pr = new ParallelRegion();
    pr.setMetadata(meta);
    pr.setSpec(spec);
    /*
     * Create the parallel region.
     */
    LOGGER.debug("ADD {}", pr.getMetadata().getName());
    client
        .customResources(
            context, ParallelRegion.class, ParallelRegionList.class, DoneableParallelRegion.class)
        .inNamespace(job.getMetadata().getNamespace())
        .createOrReplace(pr);
  }

  public void deleteParallelRegion(ParallelRegion pr) {
    LOGGER.debug("DEL {}", pr.getMetadata().getName());
    client
        .customResources(
            context, ParallelRegion.class, ParallelRegionList.class, DoneableParallelRegion.class)
        .delete(pr);
  }

  public void deleteParallelRegions(Job job) {
    /* FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745 */
    var list =
        client
            .customResources(
                context,
                ParallelRegion.class,
                ParallelRegionList.class,
                DoneableParallelRegion.class)
            .inNamespace(job.getMetadata().getNamespace())
            .withLabel(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName())
            .list();
    client.resourceList(list).withPropagationPolicy(DeletionPropagation.BACKGROUND).delete();
  }

  /*
   * Update ParallelRegion, called by the ParallelRegion coordinator.
   */

  void updateParallelRegion(ParallelRegion pr, BigInteger width) {
    /*
     * Duplicate the PR.
     */
    ObjectUtils.deepCopy(pr, ParallelRegion.class)
        .ifPresent(
            target -> {
              /*
               * Update the width.
               */
              target.getSpec().setWidth(width);
              /*
               * Patch the PR.
               */
              LOGGER.debug("UPD {}", pr.getMetadata().getName());
              client
                  .customResources(
                      context,
                      ParallelRegion.class,
                      ParallelRegionList.class,
                      DoneableParallelRegion.class)
                  .inNamespace(pr.getMetadata().getNamespace())
                  .withName(pr.getMetadata().getName())
                  .patch(target);
            });
  }

  void updateParallelRegion(Job job, ParallelRegion pr) {
    /*
     * Duplicate the PR.
     */
    ObjectUtils.deepCopy(pr, ParallelRegion.class)
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
               * Patch the PR.
               */
              LOGGER.debug("UPD {}", pr.getMetadata().getName());
              client
                  .customResources(
                      context,
                      ParallelRegion.class,
                      ParallelRegionList.class,
                      DoneableParallelRegion.class)
                  .inNamespace(pr.getMetadata().getNamespace())
                  .withName(pr.getMetadata().getName())
                  .patch(target);
            });
  }
}
