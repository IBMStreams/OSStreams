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

package com.ibm.streams.controller.crds.pes.instance;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.Connectivity;
import com.ibm.streams.controller.crds.pes.DoneableProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementFactory;
import com.ibm.streams.controller.crds.pes.ProcessingElementList;
import com.ibm.streams.controller.crds.pes.ProcessingElementSpec;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.k8s.utils.ResourceUtils;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.time.Instant;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ProcessingElementController extends GenericEventQueueConsumer<ProcessingElement> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ProcessingElementController.class);

  private final Instant creationTime;
  private final ProcessingElementFactory factory;
  private final IProcessingElementCoordinator coordinator;
  private final JobStore jobStore;
  private final Controller<ProcessingElement> controller;

  public ProcessingElementController(
      Instant creationTime,
      KubernetesClient client,
      ProcessingElementFactory peFactory,
      ProcessingElementStore peStore,
      IProcessingElementCoordinator coordinator,
      JobStore jobStore,
      String ns) {
    /*
     * Super constructor.
     */
    super(peStore);
    /*
     * Save the parameters.
     */
    this.creationTime = creationTime;
    this.factory = peFactory;
    this.coordinator = coordinator;
    this.jobStore = jobStore;
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .customResources(
                    this.factory.getContext(),
                    ProcessingElement.class,
                    ProcessingElementList.class,
                    DoneableProcessingElement.class)
                .inNamespace(ns),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends ProcessingElement> event) {
    /*
     * Grab the PE resource.
     */
    ProcessingElement pe = event.getResource();
    /*
     * Log the event.
     */
    LOGGER.debug(
        "ADD: {}/{}:{}",
        pe.getMetadata().getName(),
        pe.getSpec().getLaunchCount(),
        pe.getSpec().getContentId());
    /*
     * If the event happened before our creation time, skip it.
     */
    if (TimeUtils.happenedBefore(pe, this.creationTime)) {
      return;
    }
    /*
     * Increase the launch count.
     */
    coordinator.incrementPeLaunchCount(pe);
  }

  @Override
  public void onModification(AbstractEvent<? extends ProcessingElement> event) {
    /*
     * Grab the prior and current resource.
     */
    var pre = event.getPriorResource();
    var cur = event.getResource();
    /*
     * Log the event.
     */
    LOGGER.debug(
        "MOD: {}/{}:{}/{}:{}",
        pre.getMetadata().getName(),
        pre.getSpec().getLaunchCount(),
        pre.getSpec().getContentId(),
        cur.getSpec().getLaunchCount(),
        cur.getSpec().getContentId());
  }

  @Override
  public void onDeletion(AbstractEvent<? extends ProcessingElement> event) {
    /*
     * Grab the resource.
     */
    ProcessingElement pe = event.getResource();
    /*
     * Log the event.
     */
    LOGGER.debug("DEL: {}/{})", pe.getMetadata().getName(), pe.getSpec().getContentId());
    /*
     * Check if the job is present.
     */
    var jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
    if (!job.isPresent()) {
      LOGGER.debug(
          "Job ({}, {}) for PE {} not found", jobName, jobGenerationId, pe.getMetadata().getName());
      return;
    }
    /*
     * Reload the PE if necessary.
     */
    if (ResourceUtils.resourceIsBeingGarbageCollected(job.get())) {
      LOGGER.debug(
          "Job {} is beging garbage collected, allowing deletion of PE {}",
          job.get().getMetadata().getName(),
          pe.getMetadata().getName());
      return;
    }
    var name = pe.getMetadata().getName();
    LOGGER.debug("Restoring PE {}", name);
    var spec = new ProcessingElementSpec(pe.getSpec());
    spec.setConnectivity(Connectivity.None);
    factory.addPe(job.get(), name, pe.getSpec());
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
