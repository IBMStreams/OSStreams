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

package com.ibm.streams.controller.crds.jobs;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_API_VERSION;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_REDIS_CONFIG_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_JOB_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;

import com.ibm.streams.controller.instance.utils.RedisServiceConfigurator;
import com.ibm.streams.controller.utils.ObjectUtils;
import io.fabric8.kubernetes.api.model.DeletionPropagation;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.dsl.base.CustomResourceDefinitionContext;
import io.fabric8.kubernetes.internal.KubernetesDeserializer;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Optional;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class JobFactory {

  private static final Logger LOGGER = LoggerFactory.getLogger(JobFactory.class);

  private static final String STREAMS_JOB_PLURAL_NAME = "streamsjobs";
  public static final String STREAMS_JOB_CRD_NAME =
      STREAMS_JOB_PLURAL_NAME + "." + STREAMS_CRD_GROUP;

  private final KubernetesClient client;
  private final CustomResourceDefinitionContext context;

  public JobFactory(KubernetesClient client) {
    /*
     * Save the client handle.
     */
    this.client = client;
    /*
     * Pre-register our CRD signature with the embedded JSON deserializer. This is a required step.
     *
     * See: fabric8io/kubernetes-client#1099
     */
    KubernetesDeserializer.registerCustomKind(STREAMS_API_VERSION, "Job", Job.class);
    /*
     * Look for the Job CRD.
     */
    this.context =
        client.customResourceDefinitions().list().getItems().stream()
            .filter(e -> e.getMetadata().getName().equals(STREAMS_JOB_CRD_NAME))
            .findFirst()
            .map(CustomResourceDefinitionContext::fromCrd)
            .orElseThrow(RuntimeException::new);
  }

  public CustomResourceDefinitionContext getContext() {
    return context;
  }

  /*
   * Add/delete job, called from the Job launcher.
   */

  public void deleteJob(Job job) {
    LOGGER.debug("DEL {}", job.getMetadata().getName());
    /*
     * Execute a Foreground deletion request.
     *
     * We need to make sure that the deletion uses cascading garbage collection.
     *
     * See https://kubernetes.io/docs/concepts/workloads/controllers/garbage-collection/#controlling-how-the-garbage-collector-deletes-dependents
     * for more information about garbage collection.
     *
     * FIXME(regression) https://github.com/fabric8io/kubernetes-client/issues/2745
     */
    var rsrc =
        client
            .customResources(context, Job.class, JobList.class, DoneableJob.class)
            .inNamespace(job.getMetadata().getNamespace())
            .withName(job.getMetadata().getName())
            .get();
    client.resource(rsrc).withPropagationPolicy(DeletionPropagation.FOREGROUND).delete();
  }

  /*
   * Update Job, called from the Job launcher and the Job state machine.
   */

  public void updateJob(Job job, EJobSubmissionStatus status) {
    /*
     * Duplicate the Job.
     */
    ObjectUtils.deepCopy(job, Job.class)
        .ifPresent(
            target -> {
              /*
               * Update the width.
               */
              target.getSpec().setStatus(status);
              /*
               * Patch the Job.
               */
              LOGGER.debug("UPD {}", job.getMetadata().getName());
              client
                  .customResources(context, Job.class, JobList.class, DoneableJob.class)
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(job.getMetadata().getName())
                  .patch(target);
            });
  }

  public void updateJob(
      Job job,
      BigInteger id,
      BigInteger generationId,
      EJobSubmissionStatus status,
      JobStatisticsSpec stats,
      String appName,
      String appScope) {
    /*
     * Duplicate the Job.
     */
    ObjectUtils.deepCopy(job, Job.class)
        .ifPresent(
            target -> {
              /*
               * Prepare the annotations.
               */
              var annotations =
                  Optional.ofNullable(target.getMetadata().getAnnotations())
                      .orElse(new HashMap<>());
              annotations.put(STREAMS_APP_NAME_ANNOTATION_KEY, appName);
              annotations.put(STREAMS_APP_SCOPE_ANNOTATION_KEY, appScope);
              /*
               * If we are using a Redis cluster for checkpointing, make sure it's valid.
               */
              if (target.getSpec().getProcessingElement().getCheckpointDataStore() != null
                  && target.getSpec().getProcessingElement().getCheckpointDataStore().getRedis()
                      != null) {
                var redis =
                    target.getSpec().getProcessingElement().getCheckpointDataStore().getRedis();
                var configurator =
                    new RedisServiceConfigurator(
                        redis.getService(),
                        redis.getReplicaCount(),
                        redis.getShardCount(),
                        redis.getRestrictedProperty(),
                        redis.isShuffle());
                /*
                 * Validate the configuration. It will throw an exception if it fails.
                 */
                if (configurator.validate()) {
                  annotations.put(
                      STREAMS_REDIS_CONFIG_ANNOTATION_KEY, configurator.getReplicaServers());
                }
              }
              /*
               * Update the annotations.
               */
              target.getMetadata().setAnnotations(annotations);
              /*
               * Update the labels.
               */
              var labels =
                  Optional.ofNullable(target.getMetadata().getLabels()).orElse(new HashMap<>());
              labels.put(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE);
              labels.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_JOB_LABEL_VALUE);
              labels.put(STREAMS_JOB_LABEL_KEY, target.getMetadata().getName());
              labels.put(STREAMS_GENERATION_LABEL_KEY, generationId.toString());
              target.getMetadata().setLabels(labels);
              /*
               * Update the spec.
               */
              target.getSpec().setId(id);
              target.getSpec().setGenerationId(generationId);
              target.getSpec().setStatus(status);
              target.getSpec().setStatistics(stats);
              /*
               * Replace the Job. Not sure why but patching fails on Kubernetes.
               */
              LOGGER.debug("UPD {}", job.getMetadata().getName());
              client
                  .customResources(context, Job.class, JobList.class, DoneableJob.class)
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(job.getMetadata().getName())
                  .replace(target);
            });
  }

  /*
   * Reset Job, called from the Job controller.
   */

  void resetJob(Job job) {
    /*
     * Duplicate the Job.
     */
    ObjectUtils.deepCopy(job, Job.class)
        .ifPresent(
            target -> {
              /*
               * Update the width.
               */
              target.getSpec().setId(null);
              /*
               * Patch the Job.
               */
              LOGGER.debug("RST {}", job.getMetadata().getName());
              client
                  .customResources(context, Job.class, JobList.class, DoneableJob.class)
                  .inNamespace(job.getMetadata().getNamespace())
                  .withName(job.getMetadata().getName())
                  .create(target);
            });
  }
}
