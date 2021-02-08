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

package com.ibm.streams.controller.crds.exports;

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
import com.ibm.streams.controller.instance.broker.ImportExportCommons;
import com.ibm.streams.controller.utils.ObjectUtils;
import com.ibm.streams.instance.sam.model.OutputPort;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import io.fabric8.kubernetes.api.model.DeletionPropagation;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.dsl.base.CustomResourceDefinitionContext;
import io.fabric8.kubernetes.internal.KubernetesDeserializer;
import java.math.BigInteger;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ExportFactory {

  public static final String STREAMS_EXPORT_CRD_NAME = "streamsexports." + STREAMS_CRD_GROUP;
  private static final Logger LOGGER = LoggerFactory.getLogger(ExportFactory.class);

  private final KubernetesClient client;
  private final CustomResourceDefinitionContext context;

  public ExportFactory(KubernetesClient client) {
    /*
     * Save the client handle.
     */
    this.client = client;
    /*
     * Pre-register our CRD signature with the embedded JSON deserializer. This is a required step.
     *
     * See: fabric8io/kubernetes-client#1099
     */
    KubernetesDeserializer.registerCustomKind(STREAMS_API_VERSION, "Export", Export.class);
    /*
     * Look for the Export CRD.
     */
    this.context =
        client.customResourceDefinitions().list().getItems().stream()
            .filter(e -> e.getMetadata().getName().equals(STREAMS_EXPORT_CRD_NAME))
            .findFirst()
            .map(CustomResourceDefinitionContext::fromCrd)
            .orElseThrow(RuntimeException::new);
  }

  public CustomResourceDefinitionContext getContext() {
    return context;
  }

  /*
   * Add/delete Export. Called by the JobLauncher and the Job state machine.
   */

  public void addExport(Job job, BigInteger peId, OutputPort op) {
    /*
     * Grab the export name and the exported stream.
     */
    var name = ImportExportCommons.getExportName(job, peId, op);
    var es = op.getExportedStream();
    /*
     * Determine the export type.
     * See: https://www.ibm.com/support/knowledgecenter/en/SSCRJU_4.0.0/com.ibm.streams.ref.doc/doc/export.html
     */
    var type = es.getName() == null ? EExportedStreamType.ByProperties : EExportedStreamType.ByName;
    /*
     * Build spec.
     */
    var spec = new ExportSpec();
    spec.setJobId(job.getSpec().getId());
    spec.setPeId(peId);
    spec.setPortId(op.getId());
    spec.setType(type);
    spec.setStream(es);
    /*
     * Create the export.
     */
    addExport(job, name, spec);
  }

  public void deleteExport(Export exp) {
    LOGGER.debug("UPD {}", exp.getMetadata().getName());
    client
        .customResources(context, Export.class, ExportList.class, DoneableExport.class)
        .delete(exp);
  }

  public void deleteExports(Job job) {
    /* FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745 */
    var list =
        client
            .customResources(context, Export.class, ExportList.class, DoneableExport.class)
            .inNamespace(job.getMetadata().getNamespace())
            .withLabel(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName())
            .list();
    client.resourceList(list).withPropagationPolicy(DeletionPropagation.BACKGROUND).delete();
  }

  /*
   * Add Export, called by the export controller on deletion and the addExport above.
   */

  void addExport(Job job, String name, ExportSpec spec) {
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
     * Build the export metadata.
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
    var exp = new Export();
    exp.setMetadata(meta);
    exp.setSpec(spec);
    /*
     * Create the export.
     */
    LOGGER.debug("ADD {}", exp.getMetadata().getName());
    client
        .customResources(context, Export.class, ExportList.class, DoneableExport.class)
        .inNamespace(job.getMetadata().getNamespace())
        .create(exp);
  }

  /*
   * Update Export, called by the export coordinator.
   */

  void updateExport(Export exp, List<StreamProperty> properties) {
    /*
     * Duplicate the Export.
     */
    ObjectUtils.deepCopy(exp, Export.class)
        .ifPresent(
            target -> {
              /*
               * Update the stream properties.
               */
              target.getSpec().getStream().setProperties(properties);
              /*
               * Patch the Export.
               */
              LOGGER.debug("UPD {}", exp.getMetadata().getName());
              client
                  .customResources(context, Export.class, ExportList.class, DoneableExport.class)
                  .inNamespace(exp.getMetadata().getNamespace())
                  .withName(exp.getMetadata().getName())
                  .patch(target);
            });
  }

  void updateExport(Job job, Export exp) {
    /*
     * Duplicate the Export.
     */
    ObjectUtils.deepCopy(exp, Export.class)
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
               * Patch the Export.
               */
              LOGGER.debug("UPD {}", exp.getMetadata().getName());
              client
                  .customResources(context, Export.class, ExportList.class, DoneableExport.class)
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(exp.getMetadata().getName())
                  .patch(target);
            });
  }
}
