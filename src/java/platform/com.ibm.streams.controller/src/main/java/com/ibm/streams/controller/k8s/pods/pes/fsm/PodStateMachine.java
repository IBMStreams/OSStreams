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

package com.ibm.streams.controller.k8s.pods.pes.fsm;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;

import com.ibm.streams.controller.crds.hostpools.HostPool;
import com.ibm.streams.controller.crds.hostpools.HostPoolStore;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.events.IEventConsumerDelegate;
import com.ibm.streams.controller.executor.EExecutionCommand;
import com.ibm.streams.controller.executor.IExecutor;
import com.ibm.streams.controller.instance.jobs.JobUtils;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapStore;
import com.ibm.streams.controller.k8s.pods.EPodPhase;
import com.ibm.streams.controller.k8s.pods.PodFactory;
import com.ibm.streams.controller.k8s.pods.PodStore;
import com.ibm.streams.controller.k8s.pods.pes.PodSpecBuilder;
import com.ibm.streams.controller.utils.CommonEnvironment;
import com.ibm.streams.controller.utils.OperationInProgressException;
import io.fabric8.kubernetes.api.model.ConfigMap;
import io.fabric8.kubernetes.api.model.HasMetadata;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.text.MessageFormat;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Supplier;
import lombok.Getter;
import lombok.Setter;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class PodStateMachine implements Runnable, IEventConsumerDelegate<HasMetadata> {
  private static final Logger LOGGER = LoggerFactory.getLogger(PodStateMachine.class);

  private static final class PodDependencies {
    private final String jobName;
    private final String peId;
    @Getter private boolean configMapReady;
    @Getter private boolean peReady;
    @Getter private PodProgress progress;

    PodDependencies(String jobName, String peId) {
      this.jobName = jobName;
      this.peId = peId;
      this.configMapReady = false;
      this.peReady = false;
      this.progress = new PodProgress();
    }

    void setConfigMapReady(boolean value) {
      LOGGER.debug(
          "ConfigMap for ProcessingElement {}-{} {} ready", jobName, peId, value ? "is" : "is not");
      configMapReady = value;
    }

    void setPeReady(boolean value) {
      LOGGER.debug("ProcessingElement {}-{} {} ready", jobName, peId, value ? "is" : "is not");
      peReady = value;
    }
  }

  private static final class JobDependencies {
    private final String name;
    @Getter private final String generationId;
    @Getter @Setter private long hostPoolCount;
    @Getter private int hostPoolAdded;
    @Getter private Map<String, PodDependencies> podDependencies;

    JobDependencies(String name, String generationId) {
      this.name = name;
      this.generationId = generationId;
      this.hostPoolCount = -1;
      this.hostPoolAdded = 0;
      this.podDependencies = new ConcurrentHashMap<>();
    }

    void incrementHostPool() {
      hostPoolAdded++;
    }

    void decrementHostPool() {
      hostPoolAdded--;
    }

    PodDependencies getPodDependenciesForPeWithId(String peId) {
      return podDependencies.computeIfAbsent(peId, k -> new PodDependencies(name, peId));
    }
  }

  private Map<String, JobDependencies> states;
  private LinkedBlockingQueue<HasMetadata> eventQueue;
  private AtomicBoolean keepRunning;

  private final KubernetesClient client;
  private final ProcessingElementStore peStore;
  private final JobStore jobStore;
  private final ConfigMapStore cmStore;
  private final HostPoolStore hpStore;
  private final PodStore podStore;
  private final PodFactory podFactory;
  private final IExecutor executor;
  private final CommonEnvironment env;

  public PodStateMachine(
      KubernetesClient client,
      ProcessingElementStore peStore,
      JobStore jobStore,
      HostPoolStore hpStore,
      ConfigMapStore cmStore,
      PodStore podStore,
      PodFactory podFactory,
      IExecutor executor,
      CommonEnvironment env) {
    states = new ConcurrentHashMap<>();
    eventQueue = new LinkedBlockingQueue<>();
    keepRunning = new AtomicBoolean(true);
    this.client = client;
    this.peStore = peStore;
    this.jobStore = jobStore;
    this.cmStore = cmStore;
    this.hpStore = hpStore;
    this.podStore = podStore;
    this.podFactory = podFactory;
    this.executor = executor;
    this.env = env;
  }

  @Override
  public void run() {
    while (keepRunning.get()) {
      try {
        var resource = eventQueue.poll(500, TimeUnit.MILLISECONDS);
        if (resource == null) {
          continue;
        }
        if (resource instanceof Job) {
          handle((Job) resource);
        } else if (resource instanceof HostPool) {
          handle((HostPool) resource);
        } else if (resource instanceof ProcessingElement) {
          handle((ProcessingElement) resource);
        } else if (resource instanceof ConfigMap) {
          handle((ConfigMap) resource);
        }
      } catch (InterruptedException | OperationInProgressException ignored) {
      }
    }
  }

  public void terminate() {
    keepRunning.set(false);
  }

  private void handle(Job job) throws OperationInProgressException {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    /*
     * Create a new entry for the Job when we see an event for it. This applies for
     * both the first time we see this job, and when we see updates on it.
     */
    states.put(jobName, new JobDependencies(jobName, jobGenerationId));
    LOGGER.debug("Job {} has generation ID {}", jobName, jobGenerationId);
    /*
     * Set the HostPool count for the job. We compare this count against the number of
     * HostPool events we see.
     */
    var hostPoolCount = job.getSpec().getStatistics().getHostPoolCount();
    states.get(jobName).setHostPoolCount(hostPoolCount);
    process(jobName, jobGenerationId);
  }

  private void handle(HostPool hp) throws OperationInProgressException {
    var jobName = hp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = hp.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    /*
     * Handle a HostPool, which bumps a counter. This counter must eventually equal the
     * hostPoolCount set in the Job case.
     */
    states.get(jobName).incrementHostPool();
    process(jobName, jobGenerationId);
  }

  private void handle(ConfigMap cm) throws OperationInProgressException {
    var jobName = cm.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = cm.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var peId = cm.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    /*
     * Handle the ProcessingElement and sets its state to ready.
     */
    LOGGER.debug("ProcessingElement {} sets that it has its ConfigMap", peId);
    states.get(jobName).getPodDependenciesForPeWithId(peId).setConfigMapReady(true);
    process(jobName, jobGenerationId, peId);
  }

  private void handle(ProcessingElement pe) throws OperationInProgressException {
    var jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var peId = pe.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    /*
     * Handle the ProcessingElement and sets its state to ready.
     */
    LOGGER.debug("ProcessingElement {} is set to ready", peId);
    states.get(jobName).getPodDependenciesForPeWithId(peId).setPeReady(true);
    process(jobName, jobGenerationId, peId);
  }

  /*
   * State operation methods.
   */

  private void waitForCondition(PodDependencies podState, String header, Supplier<Boolean> fun)
      throws OperationInProgressException {
    if (!fun.get()) {
      throw new OperationInProgressException();
    }
    var next = podState.getProgress().next();
    LOGGER.debug("{} - Done, switching to {}", header, next.name());
  }

  private void process(String jobName, String jobGenerationId) throws OperationInProgressException {
    for (String peId : states.get(jobName).getPodDependencies().keySet()) {
      process(jobName, jobGenerationId, peId);
    }
  }

  private void process(String jobName, String jobGenerationId, String peId)
      throws OperationInProgressException {
    var jobState = states.get(jobName);
    var podState = states.get(jobName).getPodDependencies().get(peId);
    var state = podState.getProgress().getState();
    var header = MessageFormat.format("({0}, {1}, {2})", jobName, peId, jobGenerationId);
    LOGGER.debug("{} - Pod in state: {}", header, state);
    /*
     * Process the current machine state.
     */
    switch (state) {
      case WAITING_FOR_HOSTPOOLS:
        waitForCondition(
            podState, header, () -> jobState.getHostPoolAdded() == jobState.getHostPoolCount());
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAITING_FOR_CONFIGMAP:
        waitForCondition(podState, header, podState::isConfigMapReady);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case WAITING_FOR_PROCESSINGELEMENT:
        waitForCondition(podState, header, podState::isPeReady);
        /*
         * NOTE(xrg) the fall-through is intended.
         */
      case CREATED:
        String peName = JobUtils.getProcessingElementName(jobName, peId);
        var pe = peStore.getPeWithNameAndGenerationId(peName, jobGenerationId);
        if (!pe.isPresent()) {
          break;
        }
        /*
         * Check that the job exists.
         */
        var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
        if (!job.isPresent()) {
          LOGGER.error("Job {} not found", jobName);
          return;
        }
        /*
         * Check that the config map exists.
         */
        var cm = cmStore.getConfigMapForProcessingElement(pe.get());
        if (!cm.isPresent()) {
          LOGGER.error("Cannot find current ConfigMap for PE " + pe.get().getMetadata().getName());
          return;
        }
        /*
         * Grab the host pool.
         */
        HostPool hp = null;
        if (pe.get().getSpec().getPlacement().getHostPoolLocation() != null) {
          var poolIndex = pe.get().getSpec().getPlacement().getHostPoolLocation().getPoolIndex();
          var opt = hpStore.getHostPoolWithIdAndGenerationId(poolIndex, jobGenerationId);
          if (opt.isPresent()) {
            hp = opt.get();
          } else {
            LOGGER.error("Cannot find current HostPool for PE " + pe.get().getMetadata().getName());
            break;
          }
        }
        /*
         * Build the pod.
         */
        var builder = new PodSpecBuilder(client, job.get(), pe.get(), cm.get(), hp, env);
        executor.execute(
            EExecutionCommand.ADD_POD, () -> podFactory.addPod(pe.get(), builder.build()));
        break;
    }
  }

  /*
   * Helper methods.
   */

  private boolean jobIsUnknown(String name) {
    return !states.containsKey(name);
  }

  private boolean jobOrPeIsUnknown(String jobName, String peId) {
    return !states.containsKey(jobName)
        || !states.get(jobName).getPodDependencies().containsKey(peId);
  }

  private void enqueue(HasMetadata resource) {
    try {
      eventQueue.put(resource);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }

  /*
   * Addition methods.
   */

  @Override
  public void onAddition(AbstractEvent<? extends HasMetadata> event) {
    var resource = event.getResource();
    if (resource instanceof ConfigMap) {
      onAddition((ConfigMap) resource);
    } else if (resource instanceof HostPool) {
      onAddition((HostPool) resource);
    }
  }

  private void onAddition(ConfigMap cm) {
    enqueue(cm);
  }

  private void onAddition(HostPool hp) {
    enqueue(hp);
  }

  /*
   * Modification methods.
   */

  @Override
  public void onModification(AbstractEvent<? extends HasMetadata> event) {
    var pre = event.getPriorResource();
    var cur = event.getResource();
    if (pre instanceof Job) {
      onModification((Job) pre, (Job) cur);
    } else if (pre instanceof ProcessingElement) {
      onModification((ProcessingElement) pre, (ProcessingElement) cur);
    } else if (pre instanceof ConfigMap) {
      onModification((ConfigMap) pre, (ConfigMap) cur);
    } else if (pre instanceof HostPool) {
      onModification((HostPool) pre, (HostPool) cur);
    }
  }

  private void onModification(Job pre, Job cur) {
    var preId = pre.getSpec().getId();
    var curId = cur.getSpec().getId();
    var preGenerationId = pre.getSpec().getGenerationId();
    var curGenerationId = cur.getSpec().getGenerationId();
    /*
     * We only enqueue a Job event when it has either transitioned from NOT having
     * an ID to HAVING an ID, or when the generation ID has changed.
     */
    if ((preId == null && curId != null)
        || (preGenerationId != null && !preGenerationId.equals(curGenerationId))) {
      enqueue(cur);
    }
  }

  private void onModification(ProcessingElement pre, ProcessingElement cur) {
    /*
     * Grab the job name and check that it still exists.
     */
    var jobName = cur.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
    if (!job.isPresent()) {
      /*
       * If the current PE does NOT contain the service label, then we are
       * deleting it, and we expect that we cannot find a job with a matching
       * generation ID. Only if it does have the service label do we report
       * an error.
       */
      if (cur.getMetadata().getLabels().containsKey(STREAMS_SVC_LABEL_KEY)) {
        LOGGER.error("Job {} with generation ID {} not found", jobName, jobGenerationId);
      }
      return;
    }
    /*
     * Check the launch count, the content ID and the generation ID. If all are the same, then we have nothing
     * to do. If only the generation ID differs, we update the generation ID of the owned pod.
     */
    int preLaunchCount = pre.getSpec().getLaunchCount();
    int curLaunchCount = cur.getSpec().getLaunchCount();
    var preContentId = pre.getSpec().getContentId();
    var curContentId = cur.getSpec().getContentId();
    var preGenerationId = pre.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var curGenerationId = cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    if (preLaunchCount == curLaunchCount && preContentId.equals(curContentId)) {
      if (!preGenerationId.equals(curGenerationId)) {
        podStore
            .getPodForProcessingElement(pre)
            .ifPresent(pod -> podFactory.updatePodGenerationId(cur, pod));
      }
      return;
    }
    /*
     * Check if the previous pod needs to be deleted. We also check here if the pod needs
     * to be marked as zombie.
     */
    podStore
        .getPodForProcessingElement(pre)
        .ifPresent(
            pod -> {
              var phase = pod.getStatus().getPhase();
              if (EPodPhase.valueOf(phase) == EPodPhase.Failed
                  && !pre.getSpec().isDeleteFailedPod()) {
                podFactory.markFailedPodAsZombie(pre, pod);
              } else {
                client.pods().delete(pod);
              }
            });
    /*
     * Request to create new pod.
     */
    enqueue(cur);
  }

  private void onModification(ConfigMap pre, ConfigMap cur) {
    var preGenerationId = pre.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var curGenerationId = cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    /*
     * If the generation ID has changed, add the ConfigMap modification to the event queue.
     */
    if (!preGenerationId.equals(curGenerationId)) {
      enqueue(cur);
    }
  }

  private void onModification(HostPool pre, HostPool cur) {
    enqueue(cur);
  }

  /*
   * Deletion methods.
   */

  @Override
  public void onDeletion(AbstractEvent<? extends HasMetadata> event) {
    var resource = event.getResource();
    if (resource instanceof Job) {
      onDeletion((Job) resource);
    } else if (resource instanceof HostPool) {
      onDeletion((HostPool) resource);
    } else if (resource instanceof ConfigMap) {
      onDeletion((ConfigMap) resource);
    } else if (resource instanceof ProcessingElement) {
      onDeletion((ProcessingElement) resource);
    }
  }

  private void onDeletion(Job job) {
    states.remove(job.getMetadata().getName());
  }

  private void onDeletion(HostPool hp) {
    var name = hp.getMetadata().getName();
    var jobName = hp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    if (jobIsUnknown(jobName)) {
      LOGGER.debug("No such Job {}", jobName);
      return;
    }
    /*
     * The deletion of a HostPool _always_ triggers a reset as there is no control
     * path in the controller where such a deletion is acceptable.
     */
    LOGGER.debug(
        "Reset progress for {} Job because of HostPool {} has been deleted", jobName, name);
    states.get(jobName).decrementHostPool();
    states.get(jobName).getPodDependencies().values().forEach(v -> v.getProgress().reset());
  }

  private void onDeletion(ConfigMap cm) {
    var name = cm.getMetadata().getName();
    var jobName = cm.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var peId = cm.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    if (jobOrPeIsUnknown(jobName, peId)) {
      LOGGER.debug("No such Job {} or ProcessingElement {}", jobName, peId);
      return;
    }
    /*
     * The deletion of a ConfigMap triggers a reset only if its generation ID is the same
     * as the owning job. Otherwise, it can be safely ignored as PE delete their old CMs.
     */
    var cmGenerationId = cm.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    jobStore
        .getJobWithNameAndGenerationId(jobName, cmGenerationId)
        .ifPresent(
            job -> {
              LOGGER.debug(
                  "Reset progress for PE {}-{} because of a deleted ConfigMap {}",
                  jobName,
                  peId,
                  name);
              states.get(jobName).getPodDependencies().get(peId).setConfigMapReady(false);
              states.get(jobName).getPodDependencies().get(peId).getProgress().reset();
            });
  }

  private void onDeletion(ProcessingElement pe) {
    var jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var peId = pe.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
    if (jobOrPeIsUnknown(jobName, peId)) {
      LOGGER.debug("No such ProcessingElement {}-{}", jobName, peId);
      podStore.getPodForProcessingElement(pe).ifPresent(client.pods()::delete);
      return;
    }
    /*
     * Check that the job exists.
     */
    LOGGER.debug("Reset progress for PE {}-{} because it has been deleted", jobName, peId);
    states.get(jobName).getPodDependencies().get(peId).setPeReady(false);
    states.get(jobName).getPodDependencies().get(peId).getProgress().reset();
  }
}
