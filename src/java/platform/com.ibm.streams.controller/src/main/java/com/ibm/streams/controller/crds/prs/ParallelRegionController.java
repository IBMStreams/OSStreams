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

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceEvents.PARALLEL_REGION_WIDTH_CHANGE_FAILURE;

import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.instance.jobs.JobLauncher;
import com.ibm.streams.controller.instance.sam.Pipeline;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapCache;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapStore;
import com.ibm.streams.controller.k8s.utils.EventUtils;
import com.ibm.streams.controller.k8s.utils.ResourceUtils;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import com.ibm.streams.controller.utils.CommonEnvironment;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.math.BigInteger;
import java.time.Instant;
import java.util.stream.Collectors;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ParallelRegionController extends GenericEventQueueConsumer<ParallelRegion> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ParallelRegionController.class);
  private static final String COMPONENT_NAME = "streams-parallelregion-controller";

  private final Instant creationTime;
  private final KubernetesClient client;
  private final JobStore jobStore;
  private final JobLauncher jobLauncher;
  private final ParallelRegionStore prStore;
  private final ParallelRegionFactory prFactory;
  private final ConfigMapCache cmCache;
  private final ConfigMapStore cmStore;
  private final String ns;
  private final Controller<ParallelRegion> controller;
  private final CommonEnvironment env;

  public ParallelRegionController(
      Instant creationTime,
      KubernetesClient client,
      JobStore jobStore,
      JobLauncher jobLauncher,
      ParallelRegionStore prStore,
      ParallelRegionFactory prFactory,
      ConfigMapCache cmCache,
      ConfigMapStore cmStore,
      String ns,
      CommonEnvironment env) {
    /*
     * Super constructor.
     */
    super(prStore);

    this.creationTime = creationTime;
    this.client = client;
    this.jobStore = jobStore;
    this.jobLauncher = jobLauncher;
    this.prStore = prStore;
    this.prFactory = prFactory;
    this.cmCache = cmCache;
    this.cmStore = cmStore;
    this.ns = ns;
    this.env = env;
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .customResources(
                    prFactory.getContext(),
                    ParallelRegion.class,
                    ParallelRegionList.class,
                    DoneableParallelRegion.class)
                .inNamespace(ns),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends ParallelRegion> event) {
    /*
     * Ignore events from before our creation.
     */
    if (TimeUtils.happenedBefore(event.getResource(), this.creationTime)) {
      return;
    }
    /*
     * Print some information.
     */
    LOGGER.debug("ADD {}", event.getResource().getMetadata().getName());
  }

  @Override
  public void onModification(AbstractEvent<? extends ParallelRegion> event) {
    /*
     * Get the resources.
     */
    var curr = event.getResource();
    var prior = event.getPriorResource();
    /*
     * Ignore events if there is no width change.
     */
    if (curr.getSpec().getWidth().equals(prior.getSpec().getWidth())) {
      return;
    }
    LOGGER.debug("MOD {}", event.getResource().getMetadata().getName());
    /*
     * Check if the job is valid.
     */
    var jobName = curr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = curr.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var job = jobStore.getJobWithName(jobName);
    if (!job.isPresent()) {
      LOGGER.debug(
          "Job ({}, {}) for ParallelRegion {} not found",
          jobName,
          jobGenerationId,
          curr.getMetadata().getName());
      return;
    }
    /*
     * We have a parallel width change - process it.
     */
    LOGGER.info(
        "Starting width change for {} ({}): {} -> {}",
        curr.getMetadata().getName(),
        curr.getSpec().getAlias(),
        prior.getSpec().getWidth(),
        curr.getSpec().getWidth());
    try {
      /*
       * Get the widths for the running job (prior) and the new widths that we are going
       * to process now (curr).
       */
      var priorWidths = prStore.getParallelRegionWidthsForJob(jobName);
      priorWidths.put(prior.getSpec().getAlias(), prior.getSpec().getWidth());
      var currWidths = prStore.getParallelRegionWidthsForJob(jobName);
      /*
       * Create a mapping of PE ID to operator names so that the update process can
       * preserve PE IDs across the change.
       */
      var configMapUidToPeId =
          cmStore.getConfigMapsForJob(job.get()).stream()
              .collect(
                  Collectors.toMap(
                      cm -> cm.getMetadata().getUid(),
                      cm -> cm.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY)));
      var peIdToOperatorNames =
          cmCache.entrySet().stream()
              .filter(e -> configMapUidToPeId.containsKey(e.getKey()))
              .collect(
                  Collectors.toMap(
                      e -> new BigInteger(configMapUidToPeId.get(e.getKey())),
                      e -> Pipeline.getOperatorNames(e.getValue())));
      /*
       * Process the width change.
       */
      jobLauncher.updateParallelWidth(
          job.get(), curr.getSpec().getAlias(), peIdToOperatorNames, priorWidths, currWidths);
    } catch (Exception e) {
      /*
       * Print the stack trace.
       */
      e.printStackTrace();
      /*
       * Send a resource event.
       */
      EventUtils.sendEvent(
          client,
          COMPONENT_NAME,
          ns,
          job.get(),
          PARALLEL_REGION_WIDTH_CHANGE_FAILURE,
          "Normal",
          e.getMessage());
    }
  }

  @Override
  public void onDeletion(AbstractEvent<? extends ParallelRegion> event) {
    /*
     * Grab the resource.
     */
    var pr = event.getResource();
    /*
     * Log the event.
     */
    var name = pr.getMetadata().getName();
    LOGGER.debug("DEL: {})", name);
    /*
     * Check if the job is present.
     */
    var jobName = pr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = pr.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
    if (!job.isPresent()) {
      LOGGER.debug("Job ({}, {}) for ParallelRegion {} not found", jobName, jobGenerationId, name);
      return;
    }
    /*
     * Reload the parallel region if necessary.
     */
    if (ResourceUtils.resourceIsBeingGarbageCollected(job.get())) {
      LOGGER.debug(
          "Job {} is beging garbage collected, allowing deletion of ParallelRegion {}",
          job.get().getMetadata().getName(),
          name);
      return;
    }
    LOGGER.debug("Restoring ParallelRegion {}", name);
    prFactory.addParallelRegion(job.get(), name, pr.getSpec());
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
