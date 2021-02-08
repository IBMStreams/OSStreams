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

package com.ibm.streams.controller.crds.cros;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.utils.CommonEnvironment;
import com.ibm.streams.controller.utils.ObjectUtils;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.dsl.base.CustomResourceDefinitionContext;
import java.util.Collections;
import java.util.HashMap;
import lombok.var;

public class ConsistentRegionOperatorFactory {

  public static final String STREAMS_CRO_CRD_NAME = "streamscros." + STREAMS_CRD_GROUP;

  private final KubernetesClient client;
  private final CustomResourceDefinitionContext context;

  public ConsistentRegionOperatorFactory(KubernetesClient client) {
    /*
     * Save the client handle.
     */
    this.client = client;

    /*
     * Look for the ConsistentRegionOperator CRD.
     */
    this.context =
        client.customResourceDefinitions().list().getItems().stream()
            .filter(e -> e.getMetadata().getName().equals(STREAMS_CRO_CRD_NAME))
            .findFirst()
            .map(CustomResourceDefinitionContext::fromCrd)
            .orElseThrow(RuntimeException::new);
  }

  public CustomResourceDefinitionContext getContext() {
    return context;
  }

  public void addConsistentRegionOperator(Job job, Integer numRegions, CommonEnvironment env) {
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
     * Create the consistent region operator spec
     */
    var spec = new ConsistentRegionOperatorSpec();
    spec.setLogLevel(env.getStreamsLogLevel());
    spec.setJobName(job.getMetadata().getName());
    spec.setNumRegions(numRegions);
    spec.setJobId(job.getMetadata().getUid());
    spec.setImagePullPolicy(job.getSpec().getImagePullPolicy());
    spec.setImagePullSecret(job.getSpec().getImagePullSecret());
    /*
     * Build the labels.
     */
    var labels = new HashMap<String, String>();
    labels.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    labels.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    labels.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    /*
     * Build annotations.
     */
    var annotations = new HashMap<String, String>();
    annotations.put(
        STREAMS_APP_NAME_ANNOTATION_KEY,
        job.getMetadata().getAnnotations().get(STREAMS_APP_NAME_ANNOTATION_KEY));
    annotations.put(
        STREAMS_APP_SCOPE_ANNOTATION_KEY,
        job.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY));
    /*
     * Build the metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(job.getMetadata().getName() + "-consistent-region-operator");
    meta.setNamespace(job.getMetadata().getNamespace());
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setLabels(labels);
    meta.setAnnotations(annotations);
    /*
     * Build the ConsistentRegionOperator custom resource.
     */
    var cro = new ConsistentRegionOperator();
    cro.setMetadata(meta);
    cro.setSpec(spec);
    /*
     * Create the ConsistentRegionOperator.
     */
    client
        .customResources(
            this.getContext(),
            ConsistentRegionOperator.class,
            ConsistentRegionOperatorList.class,
            DoneableConsistentRegionOperator.class)
        .inNamespace(job.getMetadata().getNamespace())
        .create(cro);
  }

  public void updateConsistentRegionOperator(ConsistentRegionOperator cro, boolean hasStarted) {
    /*
     * Duplicate the ConsistentRegionOperator.
     */
    ObjectUtils.deepCopy(cro, ConsistentRegionOperator.class)
        .ifPresent(
            target -> {
              /*
               * Update the started status.
               */
              target.getSpec().setHasStarted(hasStarted);
              /*
               * Update the new CRO.
               */
              client
                  .customResources(
                      this.context,
                      ConsistentRegionOperator.class,
                      ConsistentRegionOperatorList.class,
                      DoneableConsistentRegionOperator.class)
                  .inNamespace(cro.getMetadata().getNamespace())
                  .withName(cro.getMetadata().getName())
                  .patch(target);
            });
  }
}
