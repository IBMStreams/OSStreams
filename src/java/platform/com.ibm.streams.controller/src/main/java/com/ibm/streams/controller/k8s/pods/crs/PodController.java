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

package com.ibm.streams.controller.k8s.pods.crs;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CR_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_LAUNCH_COUNT_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_PE_LABEL_VALUE;

import com.ibm.streams.controller.consistent.utils.ENotificationType;
import com.ibm.streams.controller.consistent.utils.Notification;
import com.ibm.streams.controller.crds.crs.consistent.ConsistentRegionController;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.instance.jobs.JobUtils;
import com.ibm.streams.controller.k8s.pods.PodStore;
import io.fabric8.kubernetes.api.model.Pod;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.util.Set;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class PodController extends GenericEventQueueConsumer<Pod> {

  private static final Logger LOGGER = LoggerFactory.getLogger(PodController.class);

  private final ProcessingElementStore peStore;
  private final ConsistentRegionController crController;
  private final Controller<Pod> controller;

  public PodController(
      KubernetesClient client,
      ConsistentRegionController crController,
      PodStore podStore,
      ProcessingElementStore peStore,
      String ns,
      String jobName) {
    /*
     * Super constructor.
     */
    super(podStore);
    /*
     * Save the client and the store.
     */
    this.crController = crController;
    this.peStore = peStore;
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .pods()
                .inNamespace(ns)
                .withLabel(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE)
                .withLabel(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_PE_LABEL_VALUE)
                .withLabel(STREAMS_JOB_LABEL_KEY, jobName)
                .withLabel(STREAMS_CR_LABEL_KEY, "true"),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends Pod> event) {
    /*
     * The monitor is created after pe crd created, we would like to listen to all events
     * Return if the PE is not in consistent region
     */

    Pod pod = event.getResource();
    String phase = pod.getStatus().getPhase();
    /*
     * Grab some PE information.
     */
    String jobName = pod.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    String peId = pod.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    String peName = JobUtils.getProcessingElementName(jobName, peId);
    /*
     * Grab the PE and update pod phase. If PE not present, it means the PE has been deleted, hence the region
     * becomes unhealthy because of the PE event
     */
    Integer launchCount =
        Integer.parseInt(pod.getMetadata().getLabels().get(STREAMS_LAUNCH_COUNT_LABEL_KEY));
    LOGGER.info("Addition event " + peName + " phase " + phase + " restart " + launchCount);
    peStore.getPeWithName(peName).ifPresent(pe -> updatePodPhase(pe, launchCount));
  }

  @Override
  public void onModification(AbstractEvent<? extends Pod> event) {
    /*
     * Get prior and current resources.
     */
    Pod pri = event.getPriorResource();
    Pod cur = event.getResource();
    /*
     * Grab the phases.
     */
    String priPhase = pri.getStatus().getPhase();
    String curPhase = cur.getStatus().getPhase();
    /*
     * Ignore the event if the phase is the same.
     */
    if (priPhase.equals(curPhase)) {
      return;
    }
    /*
     * Print some debugging information.
     */
    LOGGER.debug(cur.getMetadata().getName() + ": " + priPhase + " -> " + curPhase);
    /*
     * Grab some PE information.
     */
    String jobName = cur.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    String peId = cur.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    String peName = JobUtils.getProcessingElementName(jobName, peId);
    Integer launchCount =
        Integer.parseInt(cur.getMetadata().getLabels().get(STREAMS_LAUNCH_COUNT_LABEL_KEY));
    /*
     * Grab the PE and update pod phase
     * If PE not present, it means the PE has been deleted, hence the region
     * becomes unhealthy because of the PE event
     */
    LOGGER.info("Modification event " + peName + " phase " + curPhase + " restart " + launchCount);
    peStore.getPeWithName(peName).ifPresent(pe -> updatePodPhase(pe, launchCount));
  }

  @Override
  public void onDeletion(AbstractEvent<? extends Pod> event) {
    /*
     * Get current resource.
     */
    Pod pod = event.getResource();
    /*
     * Grab the phase.
     */
    String phase = pod.getStatus().getPhase();
    /*
     * Grab some PE information.
     */
    String jobName = pod.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    String peId = pod.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    String peName = JobUtils.getProcessingElementName(jobName, peId);
    /*
     * Grab the PE and update pod phase. If PE not present, it means the PE has been deleted, hence the region
     * becomes unhealthy because of the PE event
     */
    Integer launchCount =
        Integer.parseInt(pod.getMetadata().getLabels().get(STREAMS_LAUNCH_COUNT_LABEL_KEY));
    LOGGER.info("deletion event " + peName + " phase " + phase + " restart " + launchCount);
    peStore.getPeWithName(peName).ifPresent(pe -> updatePodPhase(pe, launchCount));
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }

  private void updatePodPhase(ProcessingElement pe, Integer launchCount) {
    if (pe.getSpec().getLaunchCount() == launchCount) {
      LOGGER.info("pod update pe health");
      Set<Integer> regions = pe.getSpec().getRegions();
      regions.forEach(this::updatePodPhaseForRegion);
    }
  }

  private void updatePodPhaseForRegion(Integer regionIndex) {
    Notification n = new Notification();
    n.setType(ENotificationType.PodPhase);
    n.setRegionIndex(regionIndex);
    crController.addNotification(n);
  }
}
