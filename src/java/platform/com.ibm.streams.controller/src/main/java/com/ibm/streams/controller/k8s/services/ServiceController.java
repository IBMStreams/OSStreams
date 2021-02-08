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

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.k8s.utils.ResourceUtils;
import io.fabric8.kubernetes.api.model.Service;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.math.BigInteger;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ServiceController extends GenericEventQueueConsumer<Service> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ServiceController.class);

  private ServiceFactory factory;
  private JobStore jobStore;
  private Controller<Service> controller;

  public ServiceController(
      KubernetesClient client,
      ServiceStore svcStore,
      ServiceFactory svcFactory,
      JobStore jobStore,
      String ns) {
    /*
     * Super constructor.
     */
    super(svcStore);
    /*
     * Save the client and the store.
     */
    this.factory = svcFactory;
    this.jobStore = jobStore;
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .services()
                .inNamespace(ns)
                .withLabel(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE)
                .withLabel(STREAMS_JOB_LABEL_KEY),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends Service> event) {}

  @Override
  public void onModification(AbstractEvent<? extends Service> event) {}

  @Override
  public void onDeletion(AbstractEvent<? extends Service> event) {
    /*
     * Grab the resource.
     */
    var svc = event.getResource();
    /*
     * Check if the job for this service is valid.
     */
    var jobName = svc.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = svc.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
    if (!job.isPresent()) {
      LOGGER.debug(
          "Job ({}, {}) for Service {} not found",
          jobName,
          jobGenerationId,
          svc.getMetadata().getName());
      return;
    }
    /*
     * Check if the service needs to be recreated.
     */
    if (ResourceUtils.resourceIsBeingGarbageCollected(job.get())) {
      LOGGER.debug(
          "Job {} is being garbage collected, allowing deletion of Service {}",
          job.get().getMetadata().getName(),
          svc.getMetadata().getName());
      return;
    }
    var name = svc.getMetadata().getName();
    var peId = new BigInteger(svc.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY));
    LOGGER.debug("Recreating Service {}", name);
    factory.addService(job.get(), name, peId, svc.getSpec());
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
