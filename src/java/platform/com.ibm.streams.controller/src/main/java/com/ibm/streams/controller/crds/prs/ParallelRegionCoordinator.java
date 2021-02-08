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

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.sync.Command;
import com.ibm.streams.controller.sync.Coordinator;
import java.math.BigInteger;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ParallelRegionCoordinator
    extends Coordinator<ParallelRegion, ParallelRegionCommandStatus>
    implements IParallelRegionCoordinator {

  private static final Logger LOGGER = LoggerFactory.getLogger(ParallelRegionCoordinator.class);

  private final ParallelRegionStore parallelRegionStore;
  private final ParallelRegionFactory parallelRegionFactory;
  private final JobStore jobStore;

  public ParallelRegionCoordinator(
      ParallelRegionStore prStore, ParallelRegionFactory prFactory, JobStore jobStore) {
    this.parallelRegionStore = prStore;
    this.parallelRegionFactory = prFactory;
    this.jobStore = jobStore;
  }

  private class SetParallelRegionWidthCommand
      extends Command<ParallelRegion, ParallelRegionCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String parallelRegionName;
    private final BigInteger width;

    SetParallelRegionWidthCommand(ParallelRegion parallelRegion, BigInteger width) {
      super(ParallelRegionCommandStatus.Unknown);
      this.jobName = parallelRegion.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId =
          parallelRegion.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.parallelRegionName = parallelRegion.getMetadata().getName();
      this.width = width;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ParallelRegionCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var pr = parallelRegionStore.getParallelRegionWithName(parallelRegionName);
      if (!pr.isPresent()) {
        LOGGER.debug("ParallelRegion {} not found", parallelRegionName);
        set(ParallelRegionCommandStatus.ParallelRegionNotFound);
        return Action.Remove;
      }
      /*
       * Check if the width is identical.
       */
      if (pr.get().getSpec().getWidth().equals(width)) {
        set(ParallelRegionCommandStatus.NoChangeNeeded);
        return Action.Remove;
      }
      /*
       * Update the width.
       */
      parallelRegionFactory.updateParallelRegion(pr.get(), width);
      return Action.Wait;
    }

    @Override
    public boolean check(ParallelRegion pre, ParallelRegion cur) {
      var result = cur.getSpec().getWidth().equals(width);
      set(result ? ParallelRegionCommandStatus.Success : ParallelRegionCommandStatus.Failure);
      return result;
    }
  }

  private class UpdateParallelRegionCommand
      extends Command<ParallelRegion, ParallelRegionCommandStatus> {

    private final String jobName;
    private final BigInteger jobGenerationId;
    private final String parallelRegionName;

    UpdateParallelRegionCommand(Job job, ParallelRegion parallelRegion) {
      super(ParallelRegionCommandStatus.Unknown);
      this.jobName = job.getMetadata().getName();
      this.jobGenerationId = job.getSpec().getGenerationId();
      this.parallelRegionName = parallelRegion.getMetadata().getName();
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ParallelRegionCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var pr = parallelRegionStore.getParallelRegionWithName(parallelRegionName);
      if (!pr.isPresent()) {
        LOGGER.debug("ParallelRegion {} not found", parallelRegionName);
        set(ParallelRegionCommandStatus.ParallelRegionNotFound);
        return Action.Remove;
      }
      /*
       * Update the width.
       */
      parallelRegionFactory.updateParallelRegion(job.get(), pr.get());
      return Action.Wait;
    }

    @Override
    public boolean check(ParallelRegion pre, ParallelRegion cur) {
      var curGenId =
          new BigInteger(cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY));
      var result = curGenId.equals(jobGenerationId);
      set(result ? ParallelRegionCommandStatus.Success : ParallelRegionCommandStatus.Failure);
      return result;
    }
  }

  private void processStatus(ParallelRegionCommandStatus status) {
    switch (status) {
      case Failure:
        throw new IllegalArgumentException("Operation failed");
      case JobNotFound:
        throw new IllegalArgumentException("Job not found");
      case ParallelRegionNotFound:
        throw new IllegalArgumentException("ParallelRegionNotFound not found");
      case NoChangeNeeded:
      case Success:
        break;
      case Unknown:
        throw new IllegalArgumentException("Invalid state");
    }
  }

  @Override
  public void setParallelRegionWidth(ParallelRegion pr, BigInteger width) {
    var command = new SetParallelRegionWidthCommand(pr, width);
    apply(pr, command);
    processStatus(command.get());
  }

  @Override
  public void updateParallelRegion(Job job, ParallelRegion pr) {
    var command = new UpdateParallelRegionCommand(job, pr);
    apply(pr, command);
    processStatus(command.get());
  }
}
