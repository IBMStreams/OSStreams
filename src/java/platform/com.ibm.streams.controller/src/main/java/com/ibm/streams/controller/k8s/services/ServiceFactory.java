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

package com.ibm.streams.controller.k8s.services;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_API_PORT;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_METRIC_PORT;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_BASE_PORT;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_CONSISENT_REGION_PORT;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_CR_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;

import com.ibm.streams.controller.crds.cros.ConsistentRegionOperator;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.instance.jobs.JobUtils;
import com.ibm.streams.controller.instance.sam.InputPortLabel;
import com.ibm.streams.controller.utils.ObjectUtils;
import io.fabric8.kubernetes.api.model.DeletionPropagation;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.api.model.Service;
import io.fabric8.kubernetes.api.model.ServicePort;
import io.fabric8.kubernetes.api.model.ServiceSpec;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.math.BigInteger;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ServiceFactory {

  private static final Logger LOGGER = LoggerFactory.getLogger(ServiceFactory.class);

  private final KubernetesClient client;

  public ServiceFactory(KubernetesClient client) {
    this.client = client;
  }

  /*
   * Helper functions.
   */

  private static List<ServicePort> getDefaultProcessingElementServicePorts() {
    /*
     * Add the prometheus metrics port.
     */
    var prom = new ServicePort();
    prom.setName("sys-metrics");
    prom.setPort(STREAMS_METRIC_PORT);
    prom.setProtocol("TCP");
    /*
     * Add the Import/Export notification port.
     */
    var subs = new ServicePort();
    subs.setName("sys-notifications");
    subs.setPort(10000);
    subs.setProtocol("UDP");
    /*
     * Add the consistent region notification port.
     */
    var cregion = new ServicePort();
    cregion.setName("sys-consistentregion");
    cregion.setPort(STREAMS_PE_CONSISENT_REGION_PORT);
    cregion.setProtocol("UDP");
    /*
     * Return the result.
     */
    return Arrays.asList(prom, subs, cregion);
  }

  private static List<ServicePort> getDefaultConsistentRegionOperatorServicePorts() {
    /*
     * Add the API port.
     */
    var api = new ServicePort();
    api.setName("sys-api");
    api.setPort(STREAMS_API_PORT);
    api.setProtocol("TCP");
    /*
     * Add the prometheus metrics port.
     */
    var prom = new ServicePort();
    prom.setName("sys-metrics");
    prom.setPort(STREAMS_METRIC_PORT);
    prom.setProtocol("TCP");
    /*
     * Return the result.
     */
    return Arrays.asList(api, prom);
  }

  static ServicePort convertPortLabelToServicePort(InputPortLabel p) {
    var port = new ServicePort();
    var label = p.getPePortLabel();
    var parts = label.split("[.@]");
    port.setName("sys-" + parts[1]);
    port.setPort(STREAMS_PE_BASE_PORT + Integer.parseInt(parts[1]));
    port.setProtocol("TCP");
    return port;
  }

  /*
   * Add/delete Service, called by the Job launcher and the Job state machine.
   */

  public void addService(Job job, BigInteger peId, List<InputPortLabel> portLabels) {
    /*
     * Build the service name.
     */
    var name = JobUtils.getProcessingElementName(job, peId);
    /*
     * Build the selector.
     */
    var selector = new HashMap<String, String>();
    selector.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    selector.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    selector.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    selector.put(STREAMS_PE_LABEL_KEY, peId.toString());
    /*
     * Build the port list.
     */
    var ports =
        portLabels.stream()
            .map(ServiceFactory::convertPortLabelToServicePort)
            .collect(Collectors.toList());
    ports.addAll(getDefaultProcessingElementServicePorts());
    /*
     * Build the service spec.
     */
    var spec = new ServiceSpec();
    spec.setClusterIP("None");
    spec.setPorts(ports);
    spec.setSelector(selector);
    /*
     * Create the service.
     */
    addService(job, name, peId, spec);
  }

  public void addService(ConsistentRegionOperator cro) {
    /*
     * Get the job name and generation ID.
     */
    var jobName = cro.getSpec().getJobName();
    var jobGenerationId = cro.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    /*
     * Create the service name.
     */
    var name = "streams-" + jobName + "-consistentregion-api";
    /*
     * Build the selector.
     */
    var selector = new HashMap<String, String>();
    selector.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    selector.put(STREAMS_JOB_LABEL_KEY, jobName);
    selector.put(STREAMS_GENERATION_LABEL_KEY, jobGenerationId);
    selector.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_CR_LABEL_VALUE);
    /*
     * Build the port list.
     */
    var ports = getDefaultConsistentRegionOperatorServicePorts();
    /*
     * Build the service spec.
     */
    var spec = new ServiceSpec();
    spec.setClusterIP("None");
    spec.setPorts(ports);
    spec.setSelector(selector);
    /*
     * Create the owner reference.
     */
    var or = new OwnerReference();
    or.setApiVersion(STREAMS_CRD_GROUP + '/' + STREAMS_CRD_VERSION);
    or.setKind("ConsistentRegionOperator");
    or.setName(cro.getMetadata().getName());
    or.setUid(cro.getMetadata().getUid());
    or.setController(true);
    or.setBlockOwnerDeletion(true);
    /*
     * Create the annotations.
     */
    var annotations = new HashMap<String, String>();
    annotations.put(
        STREAMS_APP_NAME_ANNOTATION_KEY,
        cro.getMetadata().getAnnotations().get(STREAMS_APP_NAME_ANNOTATION_KEY));
    annotations.put(
        STREAMS_APP_SCOPE_ANNOTATION_KEY,
        cro.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY));
    /*
     * Create the service.
     */
    addService(name, or, annotations, selector, spec, cro.getMetadata().getNamespace());
  }

  public void deleteService(Service svc) {
    LOGGER.debug("DEL {}", svc.getMetadata().getName());
    client.services().delete(svc);
  }

  public void deleteServices(Job job) {
    /* FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745 */
    var list =
        client
            .services()
            .inNamespace(job.getMetadata().getNamespace())
            .withLabel(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName())
            .list();
    client.resourceList(list).withPropagationPolicy(DeletionPropagation.BACKGROUND).delete();
  }

  /*
   * Add Service, called by the service controller on deletion and the on addService above.
   */

  void addService(Job job, String name, BigInteger peId, ServiceSpec spec) {
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
    labels.put(STREAMS_SVC_LABEL_KEY, "pe");
    labels.put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
    labels.put(STREAMS_PE_LABEL_KEY, peId.toString());
    labels.put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
    /*
     * Build the service.
     */
    addService(name, or, annotations, labels, spec, job.getMetadata().getNamespace());
  }

  private void addService(
      String name,
      OwnerReference or,
      Map<String, String> annotations,
      Map<String, String> labels,
      ServiceSpec spec,
      String ns) {
    /*
     * Build the service metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(name);
    meta.setNamespace(ns);
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setAnnotations(annotations);
    meta.setLabels(labels);
    /*
     * Build the service.
     */
    var svc = new Service();
    svc.setMetadata(meta);
    svc.setSpec(spec);
    /*
     * Create the pod.
     */
    LOGGER.debug("ADD {}", meta.getName());
    client.services().inNamespace(ns).create(svc);
  }

  /*
   * Update service, called by the service coordinator.
   */

  void updateService(Service svc, List<ServicePort> ports) {
    /*
     * Duplicate the Service.
     */
    ObjectUtils.deepCopy(svc, Service.class)
        .ifPresent(
            target -> {
              /*
               * Convert and patch the ports.
               */
              target.getSpec().setPorts(ports);
              /*
               * Patch the Service.
               */
              LOGGER.debug("UPD {}", svc.getMetadata().getName());
              client
                  .services()
                  .inNamespace(svc.getMetadata().getNamespace())
                  .withName(svc.getMetadata().getName())
                  .patch(target);
            });
  }

  void updateService(Job job, Service svc, List<ServicePort> ports) {
    /*
     * Duplicate the Service.
     */
    ObjectUtils.deepCopy(svc, Service.class)
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
               * Update the generation selector.
               */
              target
                  .getSpec()
                  .getSelector()
                  .put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
              /*
               * Convert and patch the ports.
               */
              ports.addAll(getDefaultProcessingElementServicePorts());
              target.getSpec().setPorts(ports);
              /*
               * Patch the Service.
               */
              LOGGER.debug("UPD {}", svc.getMetadata().getName());
              client
                  .services()
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(svc.getMetadata().getName())
                  .patch(target);
            });
  }
}
