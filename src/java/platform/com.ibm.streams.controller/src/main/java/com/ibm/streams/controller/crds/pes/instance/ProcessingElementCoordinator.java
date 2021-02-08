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

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.Connectivity;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementFactory;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.sync.Command;
import com.ibm.streams.controller.sync.Coordinator;
import java.math.BigInteger;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ProcessingElementCoordinator
    extends Coordinator<ProcessingElement, ProcessingElementCommandStatus>
    implements IProcessingElementCoordinator {

  private static final Logger LOGGER = LoggerFactory.getLogger(ProcessingElementCoordinator.class);

  private final JobStore jobStore;
  private final ProcessingElementStore peStore;
  private final ProcessingElementFactory peFactory;

  public ProcessingElementCoordinator(
      JobStore jobStore, ProcessingElementStore peStore, ProcessingElementFactory peFactory) {
    this.jobStore = jobStore;
    this.peStore = peStore;
    this.peFactory = peFactory;
  }

  private class UpdateConnectivityCommand
      extends Command<ProcessingElement, ProcessingElementCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String peName;
    private final Connectivity connectivity;

    UpdateConnectivityCommand(ProcessingElement pe, Connectivity connectivity) {
      super(ProcessingElementCommandStatus.Unknown);
      this.jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.peName = pe.getMetadata().getName();
      this.connectivity = connectivity;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ProcessingElementCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the PE.
       */
      var pe = peStore.getPeWithName(peName);
      if (!pe.isPresent()) {
        LOGGER.debug("PE {} not found", peName);
        set(ProcessingElementCommandStatus.ProcessingElementNotFound);
        return Action.Remove;
      }
      /*
       * Tell the executor to delete the command if connectivities are the same.
       */
      if (pe.get().getSpec().getConnectivity() == connectivity) {
        set(ProcessingElementCommandStatus.NoChangeNeeded);
        return Action.Remove;
      }
      /*
       * Update the connectivity.
       */
      LOGGER.debug("Updating PE {} connectivity", peName);
      peFactory.updatePe(pe.get(), connectivity);
      return Action.Wait;
    }

    @Override
    public boolean check(ProcessingElement pre, ProcessingElement cur) {
      var result =
          pre.getSpec().getConnectivity() != cur.getSpec().getConnectivity()
              && cur.getSpec().getConnectivity() == connectivity;
      set(result ? ProcessingElementCommandStatus.Success : ProcessingElementCommandStatus.Failure);
      return result;
    }
  }

  private class IncrementLaunchCountCommand
      extends Command<ProcessingElement, ProcessingElementCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String peName;

    IncrementLaunchCountCommand(ProcessingElement pe) {
      super(ProcessingElementCommandStatus.Unknown);
      this.jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.peName = pe.getMetadata().getName();
    }

    @Override
    public Action run() {
      /*
       * Find the job for the requested PE.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ProcessingElementCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the requeste PE for the job.
       */
      var pe = peStore.getPeWithName(peName);
      if (!pe.isPresent()) {
        LOGGER.debug("PE {} not found", peName);
        set(ProcessingElementCommandStatus.ProcessingElementNotFound);
        return Action.Remove;
      }
      /*
       * Increase the launch count.
       */
      LOGGER.debug("Incrementing PE {} launch count and resetting its connectivity", peName);
      peFactory.updatePe(pe.get(), pe.get().getSpec().getLaunchCount() + 1);
      return Action.Wait;
    }

    @Override
    public boolean check(ProcessingElement pre, ProcessingElement cur) {
      var result = cur.getSpec().getLaunchCount() == pre.getSpec().getLaunchCount() + 1;
      set(result ? ProcessingElementCommandStatus.Success : ProcessingElementCommandStatus.Failure);
      return result;
    }
  }

  private class TouchCommand extends Command<ProcessingElement, ProcessingElementCommandStatus> {

    private final String jobName;
    private final BigInteger jobGenerationId;
    private final String peName;

    TouchCommand(Job job, ProcessingElement pe) {
      super(ProcessingElementCommandStatus.Unknown);
      this.jobName = job.getMetadata().getName();
      this.jobGenerationId = job.getSpec().getGenerationId();
      this.peName = pe.getMetadata().getName();
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ProcessingElementCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the PE.
       */
      var pe = peStore.getPeWithName(peName);
      if (!pe.isPresent()) {
        LOGGER.debug("PE {} not found", peName);
        set(ProcessingElementCommandStatus.ProcessingElementNotFound);
        return Action.Remove;
      }
      /*
       * Simply touch the PE (update its generation ID).
       */
      LOGGER.debug("Updating PE {} generation ID", peName);
      peFactory.updatePe(job.get(), pe.get());
      return Action.Wait;
    }

    @Override
    public boolean check(ProcessingElement pre, ProcessingElement cur) {
      var curGenerationId = cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      var result = curGenerationId.equals(jobGenerationId.toString());
      set(result ? ProcessingElementCommandStatus.Success : ProcessingElementCommandStatus.Failure);
      return result;
    }
  }

  private class UpdateContentIdAndIncrementLaunchCountCommand
      extends Command<ProcessingElement, ProcessingElementCommandStatus> {

    private final String jobName;
    private final BigInteger jobGenerationId;
    private final String peName;
    private final String contentId;

    UpdateContentIdAndIncrementLaunchCountCommand(Job job, ProcessingElement pe, String contentId) {
      super(ProcessingElementCommandStatus.Unknown);
      this.jobName = job.getMetadata().getName();
      this.jobGenerationId = job.getSpec().getGenerationId();
      this.peName = pe.getMetadata().getName();
      this.contentId = contentId;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithName(jobName);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ProcessingElementCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the PE.
       */
      var pe = peStore.getPeWithName(peName);
      /*
       * Tell the executor to delete the command if neither Job nor PE exist.
       */
      if (!pe.isPresent()) {
        LOGGER.debug("PE {} not found", peName);
        set(ProcessingElementCommandStatus.ProcessingElementNotFound);
        return Action.Remove;
      }
      /*
       * Update the content ID and increase the launch count.
       */
      LOGGER.debug("Updating PE {} content ID and launch count", peName);
      peFactory.updatePe(job.get(), pe.get(), contentId, pe.get().getSpec().getLaunchCount() + 1);
      return Action.Wait;
    }

    @Override
    public boolean check(ProcessingElement pre, ProcessingElement cur) {
      var curGenerationId = cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      var result =
          curGenerationId.equals(jobGenerationId.toString())
              && cur.getSpec().getContentId().equals(this.contentId)
              && cur.getSpec().getLaunchCount() == pre.getSpec().getLaunchCount() + 1;
      set(result ? ProcessingElementCommandStatus.Success : ProcessingElementCommandStatus.Failure);
      return result;
    }
  }

  private void processStatus(ProcessingElementCommandStatus status) {
    switch (status) {
      case Failure:
        throw new IllegalArgumentException("Operation failed");
      case JobNotFound:
        throw new IllegalArgumentException("Job not found");
      case ProcessingElementNotFound:
        throw new IllegalArgumentException("PE not found");
      case NoChangeNeeded:
      case Success:
        break;
      case Unknown:
        throw new IllegalArgumentException("Invalid state");
    }
  }

  @Override
  public void updatePeConnectivity(ProcessingElement pe, Connectivity connectivity) {
    var cmd = new UpdateConnectivityCommand(pe, connectivity);
    apply(pe, cmd);
    processStatus(cmd.get());
  }

  /*
   * NOTE(xrg) The command below MUST be asynchronous as the PE controller invokes it. If we were
   * to make it synchronous, it would deadlock in onAddition().
   */
  @Override
  public void incrementPeLaunchCount(ProcessingElement pe) {
    var cmd = new IncrementLaunchCountCommand(pe);
    apply(pe, cmd);
  }

  @Override
  public void touchPe(Job job, ProcessingElement pe) {
    var cmd = new TouchCommand(job, pe);
    apply(pe, cmd);
    processStatus(cmd.get());
  }

  @Override
  public void updatePeContentIdAndIncrementLaunchCount(
      Job job, ProcessingElement pe, String contentId) {
    var cmd = new UpdateContentIdAndIncrementLaunchCountCommand(job, pe, contentId);
    apply(pe, cmd);
    processStatus(cmd.get());
  }
}
