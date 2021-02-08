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

package com.ibm.streams.controller.crds.crs;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_API_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CR_LABEL_KEY;
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
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConsistentRegionFactory {

  public static final String STREAMS_CR_CRD_NAME = "streamscrs." + STREAMS_CRD_GROUP;

  @SuppressWarnings("unused")
  private static final Logger LOGGER = LoggerFactory.getLogger(ConsistentRegionFactory.class);

  private final KubernetesClient client;
  private final CustomResourceDefinitionContext context;

  public ConsistentRegionFactory(KubernetesClient client) {
    /*
     * Save the client handle.
     */
    this.client = client;

    KubernetesDeserializer.registerCustomKind(
        STREAMS_API_VERSION, "ConsistentRegion", ConsistentRegion.class);

    /*
     * Look for the ConsistentRegion CRD.
     */
    this.context =
        client.customResourceDefinitions().list().getItems().stream()
            .filter(e -> e.getMetadata().getName().equals(STREAMS_CR_CRD_NAME))
            .findFirst()
            .map(CustomResourceDefinitionContext::fromCrd)
            .orElseThrow(RuntimeException::new);
  }

  public CustomResourceDefinitionContext getContext() {
    return context;
  }

  /*
   * Add ConsistentRegion, called from the Job launcher.
   */

  public void addConsistentRegion(
      Job job,
      Integer regionIndex,
      com.ibm.streams.control.internal.server.consistent.ConsistentRegion crMeta) {
    /*
     * create the consistent region spec (to be extended).
     */
    ConsistentRegionSpec spec = new ConsistentRegionSpec();
    spec.setRegionIndex(crMeta.getIndex());
    spec.setLogicalIndex(crMeta.getLogicalIndex());
    spec.setNumStartOperators(crMeta.getNumStartOperators());
    spec.setTrigger(crMeta.getTriggerString());
    spec.setPeriod(crMeta.getPeriod());
    spec.setDrainTimeout(crMeta.getDrainTimeout());
    spec.setResetTimeout(crMeta.getResetTimeout());
    spec.setMaxConsecutiveResetAttempts(crMeta.getMaxConsecutiveResetAttempts());
    spec.setCurrentMaxResetAttempts(crMeta.getMaxConsecutiveResetAttempts());
    spec.setPesInRegion(crMeta.getPesInRegion());
    spec.setOperatorsToStartRegionMap(crMeta.getOperatorsToStartRegionMap());
    spec.setOperatorsToTriggerMap(crMeta.getOperatorsToTriggerMap());
    spec.getPesInRegion().forEach(p -> spec.getPeToCompletion().put(p, EPeStatus.None));
    /*
     * Build owner reference.
     */
    OwnerReference or = new OwnerReference();
    or.setApiVersion(STREAMS_CRD_GROUP + '/' + STREAMS_CRD_VERSION);
    or.setKind("Job");
    or.setName(job.getMetadata().getName());
    or.setUid(job.getMetadata().getUid());
    or.setController(true);
    or.setBlockOwnerDeletion(true);

    /*
     * Build the labels.
     */
    String name = job.getMetadata().getName() + "-cr" + regionIndex.toString();
    Map<String, String> labels = new HashMap<>();
    labels.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    labels.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    labels.put(STREAMS_CR_LABEL_KEY, name);
    /*
     * Build the metadata.
     */
    ObjectMeta meta = new ObjectMeta();
    meta.setName(name);
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setLabels(labels);
    /*
     * Add the consistent region.
     */

    addConsistentRegion(meta, spec);
  }

  private void addConsistentRegion(ObjectMeta meta, ConsistentRegionSpec spec) {
    /*
     * Build the consistent region custom resource.
     */
    ConsistentRegion cr = new ConsistentRegion();
    cr.setMetadata(meta);
    cr.setSpec(spec);
    /*
     * Create the consistent region.
     */
    client
        .customResources(
            this.context,
            ConsistentRegion.class,
            ConsistentRegionList.class,
            DoneableConsistentRegion.class)
        .inNamespace(meta.getNamespace())
        .createOrReplace(cr);
  }

  public void deleteConsistentRegions(Job job) {
    /* FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745 */
    var list =
        client
            .customResources(
                this.context,
                ConsistentRegion.class,
                ConsistentRegionList.class,
                DoneableConsistentRegion.class)
            .inNamespace(job.getMetadata().getNamespace())
            .withLabel(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName())
            .list();
    client.resourceList(list).withPropagationPolicy(DeletionPropagation.BACKGROUND).delete();
  }

  /*
   * Update ConsistentRegion, called from the ConsistentRegion controller and the ConsistentRegion state machine.
   */

  public void addConsistentRegion(ConsistentRegion cr, ConsistentRegionSpec spec) {
    /*
     * Build owner reference.
     */
    OwnerReference or = new OwnerReference();
    or.setApiVersion(STREAMS_CRD_GROUP + '/' + STREAMS_CRD_VERSION);
    or.setKind("Job");
    or.setName(cr.getMetadata().getOwnerReferences().get(0).getName());
    or.setUid(cr.getMetadata().getOwnerReferences().get(0).getUid());
    or.setController(true);
    or.setBlockOwnerDeletion(true);

    /*
     * Build the labels.
     */
    String name = cr.getMetadata().getName();
    Map<String, String> labels = new HashMap<>();
    labels.put(STREAMS_JOB_LABEL_KEY, cr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY));
    labels.put(
        STREAMS_GENERATION_LABEL_KEY,
        cr.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY));
    labels.put(STREAMS_CR_LABEL_KEY, name);
    /*
     * Build the metadata.
     */
    ObjectMeta meta = new ObjectMeta();
    meta.setName(name);
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setLabels(labels);
    addConsistentRegion(meta, spec);
  }

  public void updateConsistentRegion(
      ConsistentRegion cr, EConsistentRegionState state, boolean healthy) {
    /*
     * Duplicate the ConsistentRegion.
     */
    ObjectUtils.deepCopy(cr, ConsistentRegion.class)
        .ifPresent(
            target -> {
              /*
               * Update the width.
               */
              target.getSpec().setState(state);
              target.getSpec().setRegionHealthy(healthy);
              /*
               * Update the consistent region.
               */
              client
                  .customResources(
                      this.context,
                      ConsistentRegion.class,
                      ConsistentRegionList.class,
                      DoneableConsistentRegion.class)
                  .inNamespace(cr.getMetadata().getNamespace())
                  .withName(cr.getMetadata().getName())
                  .patch(target);
            });
  }
}
