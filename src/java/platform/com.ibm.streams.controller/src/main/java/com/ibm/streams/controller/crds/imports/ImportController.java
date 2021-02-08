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

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.k8s.utils.ResourceUtils;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ImportController extends GenericEventQueueConsumer<Import> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ImportController.class);

  private final ImportFactory factory;
  private final JobStore jobStore;
  private final Controller<Import> controller;

  public ImportController(
      KubernetesClient client,
      ImportStore store,
      ImportFactory factory,
      JobStore jobStore,
      String ns) {
    /*
     * Super constructor and members.
     */
    super(store);
    this.factory = factory;
    this.jobStore = jobStore;
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .customResources(
                    factory.getContext(), Import.class, ImportList.class, DoneableImport.class)
                .inNamespace(ns),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends Import> event) {
    LOGGER.debug("ADD {}", event.getResource().getMetadata().getName());
  }

  @Override
  public void onModification(AbstractEvent<? extends Import> event) {
    LOGGER.debug("MOD {}", event.getResource().getMetadata().getName());
  }

  @Override
  public void onDeletion(AbstractEvent<? extends Import> event) {
    /*
     * Grab the resource.
     */
    var imp = event.getResource();
    LOGGER.debug("DEL {}", imp.getMetadata().getName());
    /*
     * Grab the import's job.
     */
    var jobName = imp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = imp.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    jobStore
        .getJobWithNameAndGenerationId(jobName, jobGenerationId)
        .ifPresent(
            job -> {
              if (ResourceUtils.resourceIsBeingGarbageCollected(job)) {
                return;
              }
              /*
               * Restore the export.
               */
              LOGGER.debug(
                  "Restoring Import {} for Job ({}, {})",
                  imp.getMetadata().getName(),
                  jobName,
                  jobGenerationId);
              factory.addImport(job, imp.getMetadata().getName(), imp.getSpec());
            });
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
