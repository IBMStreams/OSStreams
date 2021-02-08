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

package com.ibm.streams.controller.crds.imports;

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
import com.ibm.streams.instance.sam.model.InputPort;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
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

public class ImportFactory {

  public static final String STREAMS_IMPORT_CRD_NAME = "streamsimports." + STREAMS_CRD_GROUP;
  private static final Logger LOGGER = LoggerFactory.getLogger(ImportFactory.class);

  private final KubernetesClient client;
  private final CustomResourceDefinitionContext context;

  public ImportFactory(KubernetesClient client) {
    /*
     * Save the client handle.
     */
    this.client = client;
    /*
     * Pre-register our CRD signature with the embedded JSON deserializer. This is a required step.
     *
     * See: fabric8io/kubernetes-client#1099
     */
    KubernetesDeserializer.registerCustomKind(STREAMS_API_VERSION, "Import", Import.class);
    /*
     * Look for the Import CRD.
     */
    this.context =
        client.customResourceDefinitions().list().getItems().stream()
            .filter(e -> e.getMetadata().getName().equals(STREAMS_IMPORT_CRD_NAME))
            .findFirst()
            .map(CustomResourceDefinitionContext::fromCrd)
            .orElseThrow(RuntimeException::new);
  }

  public CustomResourceDefinitionContext getContext() {
    return context;
  }

  /*
   * Add/delete Import, called by the Job launcher and the Job state machine.
   */

  public void addImport(Job job, BigInteger peId, InputPort op) {
    /*
     * Grab the imported stream.
     */
    var is = op.getImportedStreams();
    /*
     * Build spec.
     */
    var spec = new ImportSpec();
    spec.setJobId(job.getSpec().getId());
    spec.setPeId(peId);
    spec.setPortId(op.getId());
    spec.setPortIndex(op.getIndex());
    spec.setStreams(is);
    /*
     * Create the import.
     */
    addImport(job, job.getMetadata().getName() + "-" + peId + "-" + op.getId(), spec);
  }

  public void deleteImport(Import imp) {
    LOGGER.debug("DEL {}", imp.getMetadata().getName());
    client
        .customResources(this.context, Import.class, ImportList.class, DoneableImport.class)
        .delete(imp);
  }

  public void deleteImports(Job job) {
    /* FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745 */
    var list =
        client
            .customResources(this.context, Import.class, ImportList.class, DoneableImport.class)
            .inNamespace(job.getMetadata().getNamespace())
            .withLabel(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName())
            .list();
    client.resourceList(list).withPropagationPolicy(DeletionPropagation.BACKGROUND).delete();
  }

  /*
   * Add Import, called by the import controller on deletion and the addImport above.
   */

  void addImport(Job job, String name, ImportSpec spec) {
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
     * Build the config map metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(name);
    meta.setNamespace(job.getMetadata().getNamespace());
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setLabels(labels);
    meta.setAnnotations(annotations);
    /*
     * Build the config map.
     */
    var imp = new Import();
    imp.setMetadata(meta);
    imp.setSpec(spec);
    /*
     * Create the import.
     */
    LOGGER.debug("ADD {}", imp.getMetadata().getName());
    client
        .customResources(this.context, Import.class, ImportList.class, DoneableImport.class)
        .inNamespace(job.getMetadata().getNamespace())
        .create(imp);
  }

  /*
   * Update Import, called by the import coordinator.
   */

  void updateImport(Import imp, String filter) {
    /*
     * Duplicate the Import.
     */
    ObjectUtils.deepCopy(imp, Import.class)
        .ifPresent(
            target -> {
              /*
               * Update the imported streams.
               */
              target.getSpec().getStreams().setFilter(filter);
              /*
               * Patch the Import.
               */
              LOGGER.debug("UPD {}", imp.getMetadata().getName());
              client
                  .customResources(context, Import.class, ImportList.class, DoneableImport.class)
                  .inNamespace(imp.getMetadata().getNamespace())
                  .withName(imp.getMetadata().getName())
                  .patch(target);
            });
  }

  void updateImport(Import imp, ImportedStreams streams) {
    /*
     * Duplicate the Import.
     */
    ObjectUtils.deepCopy(imp, Import.class)
        .ifPresent(
            target -> {
              /*
               * Update the imported streams.
               */
              target.getSpec().setStreams(streams);
              /*
               * Patch the Import.
               */
              LOGGER.debug("UPD {}", imp.getMetadata().getName());
              client
                  .customResources(context, Import.class, ImportList.class, DoneableImport.class)
                  .inNamespace(imp.getMetadata().getNamespace())
                  .withName(imp.getMetadata().getName())
                  .patch(target);
            });
  }

  void updateImport(Job job, Import imp) {
    /*
     * Duplicate the Import.
     */
    ObjectUtils.deepCopy(imp, Import.class)
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
               * Patch the Import.
               */
              LOGGER.debug("UPD {}", imp.getMetadata().getName());
              client
                  .customResources(context, Import.class, ImportList.class, DoneableImport.class)
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(imp.getMetadata().getName())
                  .patch(target);
            });
  }
}
