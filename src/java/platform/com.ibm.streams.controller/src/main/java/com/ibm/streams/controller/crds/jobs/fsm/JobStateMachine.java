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

package com.ibm.streams.controller.crds.jobs.fsm;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus.Submitted;
import static com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus.Submitting;

import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorStore;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.exports.ExportFactory;
import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.hostpools.HostPoolStore;
import com.ibm.streams.controller.crds.imports.ImportFactory;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobFactory;
import com.ibm.streams.controller.crds.jobs.JobStatusTransitions;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.ProcessingElementFactory;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.prs.ParallelRegionFactory;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.events.IEventConsumerDelegate;
import com.ibm.streams.controller.executor.EExecutionCommand;
import com.ibm.streams.controller.executor.IExecutor;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapFactory;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapStore;
import com.ibm.streams.controller.k8s.pods.PodStore;
import com.ibm.streams.controller.k8s.services.ServiceFactory;
import com.ibm.streams.controller.k8s.services.ServiceStore;
import com.ibm.streams.controller.utils.OperationInProgressException;
import io.fabric8.kubernetes.api.model.HasMetadata;
import java.math.BigInteger;
import java.time.Duration;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Function;
import java.util.function.Supplier;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class JobStateMachine implements Runnable, IEventConsumerDelegate<HasMetadata> {

  private static final Logger LOGGER = LoggerFactory.getLogger(JobStateMachine.class);

  private final JobStore jobStore;
  private final JobFactory jobFactory;
  private final ConfigMapStore cmStore;
  private final ConfigMapFactory cmFactory;
  private final ServiceStore svcStore;
  private final ServiceFactory svcFactory;
  private final PodStore podStore;
  private final ProcessingElementStore peStore;
  private final ProcessingElementFactory peFactory;
  private final ImportStore impStore;
  private final ImportFactory impFactory;
  private final ExportStore expStore;
  private final ExportFactory expFactory;
  private final HostPoolStore hpStore;
  private final ConsistentRegionOperatorStore croStore;
  private final ConsistentRegionStore crStore;
  private final ParallelRegionStore prStore;
  private final ParallelRegionFactory prFactory;
  private final IExecutor executor;

  private AtomicBoolean keepRunning;
  private Map<String, JobProgress> states;
  private LinkedBlockingQueue<HasMetadata> eventQueue;

  public JobStateMachine(
      JobStore jobStore,
      JobFactory jobFactory,
      ConfigMapStore cmStore,
      ConfigMapFactory cmFactory,
      ServiceStore svcStore,
      ServiceFactory svcFactory,
      PodStore podStore,
      ProcessingElementStore peStore,
      ProcessingElementFactory peFactory,
      ImportStore impStore,
      ImportFactory impFactory,
      ExportStore expStore,
      ExportFactory expFactory,
      HostPoolStore hpStore,
      ConsistentRegionOperatorStore croStore,
      ConsistentRegionStore crStore,
      ParallelRegionStore prStore,
      ParallelRegionFactory prFactory,
      IExecutor executor) {
    /*
     * Save the stores and factories.
     */
    this.jobStore = jobStore;
    this.jobFactory = jobFactory;
    this.cmStore = cmStore;
    this.cmFactory = cmFactory;
    this.svcStore = svcStore;
    this.svcFactory = svcFactory;
    this.podStore = podStore;
    this.peStore = peStore;
    this.peFactory = peFactory;
    this.impStore = impStore;
    this.impFactory = impFactory;
    this.expStore = expStore;
    this.expFactory = expFactory;
    this.hpStore = hpStore;
    this.croStore = croStore;
    this.crStore = crStore;
    this.prStore = prStore;
    this.prFactory = prFactory;
    this.executor = executor;
    /*
     * Initialize the local state.
     */
    this.keepRunning = new AtomicBoolean(true);
    this.states = new ConcurrentHashMap<>();
    this.eventQueue = new LinkedBlockingQueue<>();
  }

  @Override
  public void run() {
    while (keepRunning.get()) {
      try {
        /*
         * Grab a resource and process the timeouts.
         */
        var resource = eventQueue.poll(5, TimeUnit.SECONDS);
        if (resource == null) {
          states.entrySet().stream()
              .filter(e -> e.getValue().hasTimedOut())
              .map(e -> jobStore.getJobWithName(e.getKey()))
              .filter(Optional::isPresent)
              .map(Optional::get)
              .forEach(
                  job -> {
                    /*
                     * NOTE(xrg) we need to delete the job.
                     */
                    var name = job.getMetadata().getName();
                    var generationId = job.getSpec().getGenerationId();
                    LOGGER.warn("Submission for Job ({}, {}) is timing out", name, generationId);
                  });
          continue;
        }
        /*
         * Check if the job is known to the FSM. The resource MUST HAVE the STREAMS_JOB_LABEL_KEY label.
         */
        var name = resource.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
        if (!states.containsKey(name)) {
          LOGGER.trace("Job {} not registered in the FSM", name);
          continue;
        }
        /*
         * Grab the job.
         */
        var job = jobStore.getJobWithName(name);
        if (!job.isPresent()) {
          LOGGER.trace("Job {} not found", name);
          continue;
        }
        /*
         * Process the event.
         */
        process(job.get(), resource);
      } catch (InterruptedException | OperationInProgressException ignored) {
      }
    }
  }

  public void terminate() {
    keepRunning.set(false);
  }

  /*
   * Helper methods.
   */

  private void resetStates(String name) {
    this.states.computeIfPresent(
        name,
        (k, v) -> {
          v.reset();
          return v;
        });
  }

  /*
   * State operation methods.
   */

  private void next(String jobName, BigInteger generationId) {
    var next = this.states.get(jobName).next();
    LOGGER.debug("({}, {}) - Done, switching to {}", jobName, generationId, next.name());
  }

  private void waitForCondition(Job job, Function<Job, Long> cur, Supplier<Long> tgt)
      throws OperationInProgressException {
    var name = job.getMetadata().getName();
    var generationId = job.getSpec().getGenerationId();
    var progress = this.states.get(name);
    var value = cur.apply(job);
    var target = tgt.get();
    /*
     * Check the resource constraints.
     */
    if (!value.equals(target)) {
      LOGGER.debug(
          "({}, {}) - Waiting for {} {}(s)",
          name,
          generationId,
          Math.abs(tgt.get() - value),
          progress.getState().getDescription());
      throw new OperationInProgressException();
    }
    /*
     * Go to the next state.
     */
    next(name, generationId);
  }

  /*
   * Job State Machine
   *
   * 1. Jobs start in the SUBMITTING state
   * 2. The first transition SUBMITTING -> WAIT_FOR_CONFIG_MAPS is triggered by the assignment of the JobID
   * 3. All other transitions are sequential and triggered by resource events
   * 4. The final transition is triggered by the job's status changing from SUBMITTING to SUBMITTED
   */

  private void process(Job job, HasMetadata rsrc) throws OperationInProgressException {
    var name = job.getMetadata().getName();
    var generationId = job.getSpec().getGenerationId();
    var stats = job.getSpec().getStatistics();
    var progress = this.states.get(name);
    /*
     * Check if the job is in the right state.
     */
    if (job.getSpec().getStatus() == Submitted) {
      return;
    }
    /*
     * Process the current state. NOTE(xrg) the fall-throughs are intended.
     */
    switch (progress.getState()) {
      case SUBMITTING:
        if (!(rsrc instanceof Job)) {
          LOGGER.debug("Waiting for the first Job update");
          throw new OperationInProgressException();
        }
        next(name, generationId);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_CONFIG_MAPS:
        waitForCondition(job, cmStore::countConfigMapsWithJob, stats::getConfigMapCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_SERVICES:
        waitForCondition(job, svcStore::countServicesWithJob, stats::getServiceCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_HOSTPOOLS:
        waitForCondition(job, hpStore::countHostPoolsWithJob, stats::getHostPoolCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_EXPORTS:
        waitForCondition(job, expStore::countExportsWithJob, stats::getExportCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_IMPORTS:
        waitForCondition(job, impStore::countImportsWithJob, stats::getImportCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_CONSISTENT_REGION_OPERATOR:
        long croCount = stats.getConsistentRegionCount() == 0 ? 0L : 1L;
        waitForCondition(job, croStore::countCrosWithJob, () -> croCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_CONSISTENT_REGIONS:
        waitForCondition(job, crStore::countCrsWithJob, stats::getConsistentRegionCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_PARALLEL_REGIONS:
        waitForCondition(job, prStore::countParallelRegionsWithJob, stats::getParallelRegionCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_PES:
        waitForCondition(job, peStore::countPesWithJob, stats::getProcessingElementCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_PODS:
        waitForCondition(job, podStore::countPodsWithJob, stats::getProcessingElementCount);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case DELETE_PES:
        executor.execute(
            EExecutionCommand.DELETE_PES,
            () -> {
              peStore
                  .getPesWithJobAndPreviousGenerationId(job)
                  .forEach((k, v) -> peFactory.deletePe(v));
            });
        next(name, generationId);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_DELETED_PES:
        waitForCondition(job, peStore::countPesWithJobAndPreviousGenerationId, () -> 0L);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case DELETE_IMPORTS:
        executor.execute(
            EExecutionCommand.DELETE_IMPORTS,
            () -> {
              impStore
                  .getImportsWithJobAndPreviousGenerationId(job)
                  .forEach(impFactory::deleteImport);
            });
        next(name, generationId);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_DELETED_IMPORTS:
        waitForCondition(job, impStore::countImportsWithJobAndPreviousGenerationId, () -> 0L);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case DELETE_EXPORTS:
        executor.execute(
            EExecutionCommand.DELETE_EXPORTS,
            () -> {
              expStore
                  .getExportsWithJobAndPreviousGenerationId(job)
                  .forEach(expFactory::deleteExport);
            });
        next(name, generationId);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_DELETED_EXPORTS:
        waitForCondition(job, expStore::countExportsWithJobAndPreviousGenerationId, () -> 0L);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case DELETE_SERVICES:
        executor.execute(
            EExecutionCommand.DELETE_SERVICES,
            () -> {
              svcStore
                  .getServicesWithJobAndPreviousGenerationId(job)
                  .forEach(svcFactory::deleteService);
            });
        next(name, generationId);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_DELETED_SERVICES:
        waitForCondition(job, svcStore::countServicesWithJobAndPreviousGenerationId, () -> 0L);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case DELETE_CONFIG_MAPS:
        executor.execute(
            EExecutionCommand.DELETE_CONFIGMAPS,
            () -> {
              cmStore
                  .getConfigMapsWithJobAndPreviousGenerationId(job)
                  .forEach(cmFactory::deleteConfigMap);
            });
        next(name, generationId);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAIT_FOR_DELETED_CONFIG_MAPS:
        waitForCondition(job, cmStore::countConfigMapsWithJobAndPreviousGenerationId, () -> 0L);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case SUBMITTED:
        jobFactory.updateJob(job, Submitted);
        break;
    }
  }

  /*
   * Addition method.
   */

  @Override
  public void onAddition(AbstractEvent<? extends HasMetadata> event) {
    var resource = event.getResource();
    if (resource instanceof Job) {
      var name = resource.getMetadata().getName();
      var job = (Job) resource;
      var timeOut = Duration.ofSeconds(job.getSpec().getSubmissionTimeOutInSeconds());
      var state =
          job.getSpec().getStatus() == Submitting ? EJobState.SUBMITTING : EJobState.SUBMITTED;
      this.states.put(name, new JobProgress(timeOut, state));
    } else {
      try {
        eventQueue.put(resource);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
  }

  /*
   * Modification method.
   */

  @Override
  public void onModification(AbstractEvent<? extends HasMetadata> event) {
    /*
     * Only check modifications for Jobs.
     */
    if (!(event.getResource() instanceof Job)) {
      return;
    }
    var pri = (Job) event.getPriorResource();
    var cur = (Job) event.getResource();
    /*
     * Notify the state machine if the new event is a new job (gained ID) or
     * a job status reversion which may require changing resources.
     */
    try {
      if (JobStatusTransitions.revertedToSubmitting(pri, cur)) {
        resetStates(cur.getMetadata().getName());
        eventQueue.put(cur);
      } else if (JobStatusTransitions.gainedId(pri, cur)) {
        eventQueue.put(cur);
      }
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }

  /*
   * Deletion method.
   */

  @Override
  public void onDeletion(AbstractEvent<? extends HasMetadata> event) {
    var resource = event.getResource();
    var name = resource.getMetadata().getName();
    if (resource instanceof Job) {
      this.states.remove(name);
      return;
    }
    /*
     * NOTE(xrg) we reset the FSM state here if a resource for the job has been deleted.
     * This will force the FSM to recheck each resource for completion. In the highly likely
     * event of a failure, the timeout check will know exaclty which state failed.
     */
    resetStates(name);
    /*
     * Enqueue the event.
     */
    try {
      eventQueue.put(resource);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }
}
