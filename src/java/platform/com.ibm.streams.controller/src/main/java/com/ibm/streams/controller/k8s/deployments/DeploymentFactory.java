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

package com.ibm.streams.controller.k8s.deployments;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_CR_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;

import com.ibm.streams.controller.crds.cros.ConsistentRegionOperator;
import com.ibm.streams.controller.utils.CommonEnvironment;
import io.fabric8.kubernetes.api.model.Container;
import io.fabric8.kubernetes.api.model.EnvVar;
import io.fabric8.kubernetes.api.model.LabelSelector;
import io.fabric8.kubernetes.api.model.LocalObjectReference;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.OwnerReference;
import io.fabric8.kubernetes.api.model.PodSpec;
import io.fabric8.kubernetes.api.model.PodTemplateSpec;
import io.fabric8.kubernetes.api.model.apps.Deployment;
import io.fabric8.kubernetes.api.model.apps.DeploymentSpec;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.stream.Collectors;
import lombok.var;

public class DeploymentFactory {

  private final KubernetesClient client;
  private final CommonEnvironment env;

  public DeploymentFactory(KubernetesClient client, CommonEnvironment env) {
    this.client = client;
    this.env = env;
  }

  public void addDeployment(ConsistentRegionOperator cro) {
    /*
     * Grab the namespace;
     */
    var ns = cro.getMetadata().getNamespace();
    /*
     * Build the container environment.
     */
    var containerEnv =
        Arrays.stream(
                new String[][] {
                  {"STREAMS_JOB_NAME", cro.getSpec().getJobName()},
                  {"STREAMS_LOG_LEVEL", cro.getSpec().getLogLevel()},
                  {"MY_POD_NAMESPACE", ns},
                  {"NUM_CONSISTENT_REGIONS", cro.getSpec().getNumRegions().toString()},
                  {"STREAMS_JOB_ID", cro.getSpec().getJobId()},
                  {
                    "STREAMS_GENERATION_ID",
                    cro.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY)
                  }
                })
            .map(
                e -> {
                  EnvVar var = new EnvVar();
                  var.setName(e[0]);
                  var.setValue(e[1]);
                  return var;
                })
            .collect(Collectors.toList());
    /*
     * Build the container definition.
     */
    var type = env.getBuildType().toLowerCase();
    var c0 = new Container();
    c0.setName("consistent-region-operator");
    c0.setImage(
        String.format(
            "%s/%s/streams-runtime:6.%s", env.getRegistry(), env.getRegistryNameSpace(), type));
    c0.setImagePullPolicy(cro.getSpec().getImagePullPolicy());
    c0.setEnv(containerEnv);
    c0.setArgs(
        Arrays.asList(
            "java",
            "-cp",
            "/usr/local/streams/system/impl/lib/com.ibm.streams.controller.jar",
            "com.ibm.streams.controller.consistent.Main"));
    /*
     * Build the cr operator pod spec.
     */
    var spec = new PodSpec();
    spec.setContainers(Collections.singletonList(c0));
    /*
     * Image pull secret.
     */
    if (cro.getSpec().getImagePullSecret() != null) {
      LocalObjectReference s0 = new LocalObjectReference();
      s0.setName(cro.getSpec().getImagePullSecret());
      spec.setImagePullSecrets(Collections.singletonList(s0));
    }
    /*
     * Grab the job name and a unique pod name.
     */
    var jobName = cro.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = cro.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var deploymentName = jobName + "-consistent-region-operator";
    /*
     * Build the labels.
     */
    var labels = new HashMap<String, String>();
    labels.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    labels.put(STREAMS_JOB_LABEL_KEY, jobName);
    labels.put(STREAMS_GENERATION_LABEL_KEY, jobGenerationId);
    labels.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_CR_LABEL_VALUE);
    /*
     * Build the pod template metadata.
     */
    var podTemplateMeta = new ObjectMeta();
    podTemplateMeta.setName(deploymentName);
    podTemplateMeta.setNamespace(cro.getMetadata().getNamespace());
    podTemplateMeta.setLabels(labels);
    /*
     * Build the pod template spec.
     */
    var podTemplateSpec = new PodTemplateSpec();
    podTemplateSpec.setSpec(spec);
    podTemplateSpec.setMetadata(podTemplateMeta);
    /*
     * Build the label selector.
     */
    var selector = new HashMap<String, String>();
    selector.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
    selector.put(STREAMS_JOB_LABEL_KEY, jobName);
    selector.put(STREAMS_GENERATION_LABEL_KEY, jobGenerationId);
    selector.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_CR_LABEL_VALUE);
    var labelSelector = new LabelSelector();
    labelSelector.setMatchLabels(selector);
    /*
     * Build the deployment spec.
     */
    var depSpec = new DeploymentSpec();
    depSpec.setReplicas(1);
    depSpec.setSelector(labelSelector);
    depSpec.setTemplate(podTemplateSpec);
    /*
     * Build owner reference.
     */
    var or = new OwnerReference();
    or.setApiVersion(STREAMS_CRD_GROUP + '/' + STREAMS_CRD_VERSION);
    or.setKind("ConsistentRegionOperator");
    or.setName(cro.getMetadata().getName());
    or.setUid(cro.getMetadata().getUid());
    or.setController(true);
    or.setBlockOwnerDeletion(true);
    /*
     * Build the deployment metadata.
     */
    var meta = new ObjectMeta();
    meta.setName(deploymentName);
    meta.setNamespace(cro.getMetadata().getNamespace());
    meta.setOwnerReferences(Collections.singletonList(or));
    meta.setLabels(labels);
    /*
     * Build the deployment.
     */
    var dep = new Deployment();
    dep.setMetadata(meta);
    dep.setSpec(depSpec);
    /*
     * Create the pod.
     */
    client.apps().deployments().inNamespace(cro.getMetadata().getNamespace()).create(dep);
  }
}
