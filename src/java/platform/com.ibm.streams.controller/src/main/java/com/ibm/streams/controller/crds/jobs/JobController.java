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

import static com.ibm.streams.controller.crds.ICustomResourceEvents.JOB_SUBMISSION_FAILURE;
import static com.ibm.streams.controller.crds.ICustomResourceEvents.JOB_SUBMISSION_SUCCESS;
import static com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus.Submitting;

import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.instance.jobs.JobLauncher;
import com.ibm.streams.controller.k8s.utils.EventUtils;
import com.ibm.streams.controller.k8s.utils.ResourceUtils;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import com.ibm.streams.controller.utils.CommonEnvironment;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.math.BigInteger;
import java.text.MessageFormat;
import java.time.Instant;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class JobController extends GenericEventQueueConsumer<Job> {

  private static final Logger LOGGER = LoggerFactory.getLogger(JobController.class);
  private static final String COMPONENT_NAME = "streams-job-controller";

  private final Instant creationTime;
  private final KubernetesClient client;
  private final JobFactory jobFactory;
  private final JobLauncher jobLauncher;
  private final String ns;
  private final CommonEnvironment env;
  private final Controller<Job> controller;

  private BigInteger currentJobId;

  public JobController(
      Instant creationTime,
      KubernetesClient client,
      JobFactory jobFactory,
      JobStore jobStore,
      JobLauncher jobLauncher,
      String ns,
      CommonEnvironment env) {
    /*
     * Super constructor.
     */
    super(jobStore);
    /*
     * Save the parameters.
     */
    this.creationTime = creationTime;
    this.client = client;
    this.jobFactory = jobFactory;
    this.jobLauncher = jobLauncher;
    this.ns = ns;
    this.env = env;
    /*
     * Create the controller.
     */
    this.controller =
        new Controller<>(
            client
                .customResources(
                    jobFactory.getContext(), Job.class, JobList.class, DoneableJob.class)
                .inNamespace(ns),
            this);
    this.currentJobId = BigInteger.ZERO;
  }

  @Override
  public void onAddition(AbstractEvent<? extends Job> event) {
    var job = event.getResource();
    /*
     * If the event happenened before our creation time, skip it but catch-up with the job ID.
     */
    var id = job.getSpec().getId();
    if (id != null && TimeUtils.happenedBefore(job, this.creationTime)) {
      currentJobId = currentJobId.compareTo(id) < 0 ? id.add(BigInteger.ONE) : currentJobId;
      /*
       * Ignore already submitted jobs.
       */
      var status = job.getSpec().getStatus();
      if (status == EJobSubmissionStatus.Submitted) {
        return;
      }
      /*
       * Delete partially-submitted jobs.
       */
      jobLauncher.delete(job);
      return;
    }
    /*
     * Process the new job.
     */
    try {
      jobLauncher.launch(job, currentJobId);
      currentJobId = currentJobId.add(BigInteger.ONE);
    } catch (Exception e) {
      /*
       * Print the stack trace.
       */
      e.printStackTrace();
      /*
       * Send a resource event.
       */
      EventUtils.sendEvent(
          client, COMPONENT_NAME, ns, job, JOB_SUBMISSION_FAILURE, "Normal", e.getMessage());
    }
  }

  @Override
  public void onModification(AbstractEvent<? extends Job> event) {
    /*
     * Grab the resources.
     */
    var pre = event.getPriorResource();
    var cur = event.getResource();
    /*
     * Only add the resources if we observe a new job (gained an ID) or some of the
     * job resources may have changed (reverted to `Submitting` status).
     */
    if (JobStatusTransitions.gainedId(pre, cur)
        || JobStatusTransitions.revertedToSubmitting(pre, cur)) {
      jobLauncher.addResources(cur);
    }
    /*
     * Send a notification if the status of the job has become subnitted.
     */
    if (JobStatusTransitions.becameSubmitted(pre, cur)) {
      jobLauncher.removeJobFromTransitionalContext(cur.getMetadata().getName());
      if (env.enableNotifications()) {
        var message = MessageFormat.format("Running job {0}", cur.getMetadata().getName());
        EventUtils.sendEvent(
            client, COMPONENT_NAME, ns, cur, JOB_SUBMISSION_SUCCESS, "Normal", message);
      }
    }
  }

  @Override
  public void onDeletion(AbstractEvent<? extends Job> event) {
    /*
     * We differentiate between voluntary deletion and deletion in order to handle failures during job submission
     * by looking at the finalizer.
     */
    Job job = event.getResource();
    if (job.getSpec().getStatus() == Submitting
        && ResourceUtils.resourceIsBeingGarbageCollected(job)) {
      LOGGER.debug("Recreate job that failed during submission");
      jobFactory.resetJob(job);
    } else {
      jobLauncher.deleteResources(job);
    }
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
