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

package com.ibm.streams.controller.k8s.pods.pes;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CONTENT_ID_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_PE_LABEL_VALUE;
import static com.ibm.streams.controller.k8s.pods.EPodPhase.Failed;
import static com.ibm.streams.controller.k8s.pods.EPodPhase.Succeeded;

import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.instance.jobs.JobUtils;
import com.ibm.streams.controller.k8s.pods.EPodPhase;
import com.ibm.streams.controller.k8s.pods.PodFactory;
import com.ibm.streams.controller.k8s.pods.PodStore;
import com.ibm.streams.controller.k8s.utils.ResourceUtils;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import io.fabric8.kubernetes.api.model.Pod;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.time.Instant;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class PodController extends GenericEventQueueConsumer<Pod> {

  private static final Logger LOGGER = LoggerFactory.getLogger(PodController.class);
  private final Instant creationTime;
  private final PodFactory factory;
  private final JobStore jobStore;
  private final ProcessingElementStore peStore;
  private final IProcessingElementCoordinator peCoordinator;
  private final Controller<Pod> controller;

  /*
   * Pod phases:
   *
   * - Pending: The Pod has been accepted by the Kubernetes system, but one or more of the Container images has not
   *       been created. This includes time before being scheduled as well as time spent downloading images over
   *       the network, which could take a while.
   * - Running: The Pod has been bound to a node, and all of the Containers have been created. At least one Container
   *       is still running, or is in the process of starting or restarting.
   * - Succeeded: All Containers in the Pod have terminated in success, and will not be restarted.
   * - Failed: All Containers in the Pod have terminated, and at least one Container has terminated in failure.
   *       That is, the Container either exited with non-zero status or was terminated by the system.
   * - Unknown: For some reason the state of the Pod could not be obtained, typically due to an error in communicating
   *       with the host of the Pod.
   */

  public PodController(
      Instant creationTime,
      KubernetesClient client,
      PodFactory factory,
      PodStore podStore,
      JobStore jobStore,
      ProcessingElementStore peStore,
      IProcessingElementCoordinator peCoordinator,
      String ns) {
    /*
     * Super constructor.
     */
    super(podStore);
    /*
     * Save the factory and the store.
     */
    this.creationTime = creationTime;
    this.factory = factory;
    this.jobStore = jobStore;
    this.peStore = peStore;
    this.peCoordinator = peCoordinator;
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .pods()
                .inNamespace(ns)
                .withLabel(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE)
                .withLabel(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_PE_LABEL_VALUE),
            this);
  }

  private boolean peAndPodHashDiffer(ProcessingElement pe, Pod pod) {
    var peHash = pe.getSpec().getContentId();
    var podHash = pod.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY);
    return !peHash.equals(podHash);
  }

  /*
   * NOTE: if any action is eventually taken here, the creation time much be checked
   * and compared with the creation time of the controller. Events that are anterior to
   * the creation of the controller should be ignored.
   */

  @Override
  public void onAddition(AbstractEvent<? extends Pod> event) {
    /*
     * Register the pod name.
     */
    Pod pod = event.getResource();
    LOGGER.debug(
        "ADD: "
            + pod.getMetadata().getName()
            + "/"
            + pod.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY));
    if (TimeUtils.happenedBefore(pod, this.creationTime)) {
      return;
    }
    factory.registerPodName(pod.getMetadata().getName());
  }

  /*
   * NOTE(xrg): for the PE:POD relationship, since we are using random naming for Pods, we need
   * to match Pods and PEs not only with the name of the PE but also its UID. The reason is that
   * by the time the Pod controller gets a deletion event after a PE has been deleted manually,
   * the PE controller might have recreated the object. So if we only check by name, we recreate
   * the deleted Pod even though it is not necessary as the PE would have created a new one.
   *
   * NOTE(xrg): the actual pod deletion task required when a pod must be restarted has been
   * moved to the processing element controller. It conflates with the task of the PE controller
   * to delete previously existing pods when the UUID has changed.
   */

  @Override
  public void onModification(AbstractEvent<? extends Pod> event) {
    /*
     * Get prior and current resources.
     */
    Pod pri = event.getPriorResource();
    Pod cur = event.getResource();
    /*
     * Log the event.
     */
    LOGGER.debug(
        "MOD: "
            + pri.getMetadata().getName()
            + "/"
            + pri.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY)
            + "/"
            + cur.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY));
    /*
     * Grab the phases.
     */
    var priPhase = pri.getStatus().getPhase();
    var curPhase = cur.getStatus().getPhase();
    /*
     * Ignore the event if the phase is the same.
     */
    if (priPhase.equals(curPhase)) {
      return;
    }
    /*
     * Print some debugging information.
     */
    LOGGER.debug("MOD: {}/{}/{}", cur.getMetadata().getName(), priPhase, curPhase);
    /*
     * Check if the job is still valid.
     */
    var jobName = cur.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    if (!jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId).isPresent()) {
      LOGGER.debug(
          "Job ({}, {}) for Pod {} not found",
          jobName,
          jobGenerationId,
          cur.getMetadata().getName());
    }
    /*
     * Grab some PE information.
     */
    var peId = cur.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    var peName = JobUtils.getProcessingElementName(jobName, peId);
    var peUid = cur.getMetadata().getOwnerReferences().get(0).getUid();
    /*
     * Grab the PE and check the state.
     */
    peStore
        .getPeWithNameAndUid(peName, peUid)
        .ifPresent(
            pe -> {
              /*
               * Make sure the PE and the pod have the same AADL hash before proceeding.
               */
              if (peAndPodHashDiffer(pe, cur)) {
                LOGGER.debug(
                    "PE {} and Pod {} are out of sync, skipping",
                    pe.getMetadata().getName(),
                    cur.getMetadata().getName());
                return;
              }
              /*
               * Process the phase change.
               */
              switch (EPodPhase.valueOf(curPhase)) {
                  /*
                   * The phase might be in an undecided state. In that case, we consider it failed.
                   */
                case Pending:
                  /*
                   * Check if the pod has failed and we should restart it (RC != 0).
                   */
                case Failed:
                  {
                    if (pe.getSpec().getRestartFailedPod()) {
                      LOGGER.debug("Restart failed Pod {}", cur.getMetadata().getName());
                      peCoordinator.incrementPeLaunchCount(pe);
                    }
                    break;
                  }
                  /*
                   * Check if the pod has terminated cleanly and we should restart it. This stage is entered whenever
                   * all containers terminate cleanly (RC=0).
                   */
                case Succeeded:
                  {
                    /*
                     * NOTE(scott): the nomenclature is strange here. Maybe we should add a
                     * `restartDeletedPod` option instead, which would also be informed
                     * by the SPL-level restartable config. See onDeletion().
                     *
                     * NOTE(xrg): we need to discriminate in the runtime whether or not we have
                     * been terminated by an external signal. We may need to conflate _failed_
                     * and _deleted by user_.
                     */
                    if (pe.getSpec().isRestartCompletedPod()) {
                      LOGGER.debug("Restart deleted Pod {}", cur.getMetadata().getName());
                      peCoordinator.incrementPeLaunchCount(pe);
                    }
                    break;
                  }
              }
            });
  }

  /*
   * NOTE(xrg): when a pod has failed, it is deleted by the function above before being restarted.
   * Therefore, we cannot also restart it here if `restartDeletedPod` is set.
   */

  @Override
  public void onDeletion(AbstractEvent<? extends Pod> event) {
    /*
     * Get current resource.
     */
    Pod pod = event.getResource();
    /*
     * Log the event.
     */
    LOGGER.debug(
        "DEL: {}/{}",
        pod.getMetadata().getName(),
        pod.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY));
    /*
     * Remove its name from the factory.
     */
    factory.unregisterPodName(pod.getMetadata().getName());
    /*
     * Grab the phase.
     */
    var phase = pod.getStatus().getPhase();
    /*
     * Check if the job is still valid.
     */
    var jobName = pod.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = pod.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    if (!jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId).isPresent()) {
      LOGGER.debug(
          "Job ({}, {}) for Pod {} not found",
          jobName,
          jobGenerationId,
          pod.getMetadata().getName());
    }
    /*
     * Grab some PE information.
     */
    var peId = pod.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    var peName = JobUtils.getProcessingElementName(jobName, peId);
    var peUid = pod.getMetadata().getOwnerReferences().get(0).getUid();
    /*
     * Grab the PE and check the state.
     */
    peStore
        .getPeWithNameAndUid(peName, peUid)
        .ifPresent(
            pe -> {
              /*
               * Check if pod is being foreground deleted
               */
              if (ResourceUtils.resourceIsBeingGarbageCollected(pod)) {
                LOGGER.debug("Pod {} is being garbage collected", pod.getMetadata().getName());
                return;
              }
              /*
               * Check if PE and Pod AADL UUID match.
               */
              if (peAndPodHashDiffer(pe, pod)) {
                LOGGER.debug(
                    "PE {} and Pod {} are out of sync",
                    pe.getMetadata().getName(),
                    pod.getMetadata().getName());
                return;
              }
              /*
               * Check if the pod has failed and if it was restarted.
               * The field getRestartFailedPod is always assigned a value by the Job launcher.
               */
              if (EPodPhase.valueOf(phase) == Failed && pe.getSpec().getRestartFailedPod()) {
                return;
              }
              /*
               * Check if the pod has completed and if it was restarted.
               * The field getRestartCompletedPod is always assigned a value by the Job launcher.
               */
              if (EPodPhase.valueOf(phase) == Succeeded && pe.getSpec().isRestartCompletedPod()) {
                return;
              }
              /*
               * Restart the pod if none of the previous cases apply.
               * The field getRestartDeletedPod is always assigned a value by the Job launcher.
               */
              if (pe.getSpec().isRestartDeletedPod()) {
                LOGGER.debug("Increment PE {} launch count", pod.getMetadata().getName());
                peCoordinator.incrementPeLaunchCount(pe);
              }
            });
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
