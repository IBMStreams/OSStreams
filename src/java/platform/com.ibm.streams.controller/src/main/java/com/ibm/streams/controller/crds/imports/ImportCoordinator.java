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

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.instance.broker.ImportExportCommons;
import com.ibm.streams.controller.sync.Command;
import com.ibm.streams.controller.sync.Coordinator;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.platform.services.ImportedStreamsType;
import java.math.BigInteger;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ImportCoordinator extends Coordinator<Import, ImportCommandStatus>
    implements IImportCoordinator {

  private static final Logger LOGGER = LoggerFactory.getLogger(ImportCoordinator.class);

  private final ImportStore importStore;
  private final ImportFactory importFactory;
  private final JobStore jobStore;

  public ImportCoordinator(
      ImportStore importStore, ImportFactory importFactory, JobStore jobStore) {
    this.importStore = importStore;
    this.importFactory = importFactory;
    this.jobStore = jobStore;
  }

  private class SetImportFilterCommand extends Command<Import, ImportCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String importName;
    private final String filter;

    SetImportFilterCommand(Import imp, String filter) {
      super(ImportCommandStatus.Unknown);
      this.jobName = imp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = imp.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.importName = imp.getMetadata().getName();
      this.filter = filter.isEmpty() ? null : filter;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ImportCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var imp = importStore.getImportWithNameAndGenerationId(importName, jobGenerationId);
      if (!imp.isPresent()) {
        LOGGER.debug("Import {} not found", importName);
        set(ImportCommandStatus.ImportNotFound);
        return Action.Remove;
      }
      /*
       * Get the original filter.
       */
      var origin = imp.get().getSpec().getStreams().getFilter();
      /*
       * Check the filter combination.
       */
      if (this.filter == null && origin == null) {
        set(ImportCommandStatus.NoChangeNeeded);
        return Action.Remove;
      }
      if (this.filter != null && origin != null) {
        if (this.filter.equals(origin)) {
          set(ImportCommandStatus.NoChangeNeeded);
          return Action.Remove;
        }
      }
      /*
       * Update the filter.
       */
      importFactory.updateImport(imp.get(), filter);
      return Action.Wait;
    }

    @Override
    public boolean check(Import pre, Import cur) {
      var update = cur.getSpec().getStreams().getFilter();
      var result = update == null || update.equals(filter);
      set(result ? ImportCommandStatus.Success : ImportCommandStatus.Failure);
      return result;
    }
  }

  private class SetImportImportedStreamCommand extends Command<Import, ImportCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String importName;
    private final ImportedStreams streams;

    SetImportImportedStreamCommand(Import imp, ImportedStreams streams) {
      super(ImportCommandStatus.Unknown);
      this.jobName = imp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = imp.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.importName = imp.getMetadata().getName();
      this.streams = streams;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ImportCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var imp = importStore.getImportWithNameAndGenerationId(importName, jobGenerationId);
      if (!imp.isPresent()) {
        LOGGER.debug("Import {} not found", importName);
        set(ImportCommandStatus.ImportNotFound);
        return Action.Remove;
      }
      /*
       * Check if the imported streams are different.
       */
      if (imp.get().getSpec().getStreams().equals(streams)) {
        set(ImportCommandStatus.NoChangeNeeded);
        return Action.Remove;
      }
      /*
       * Update the imported streams.
       */
      importFactory.updateImport(imp.get(), streams);
      return Action.Wait;
    }

    @Override
    public boolean check(Import pre, Import cur) {
      var result = cur.getSpec().getStreams().toString().equals(streams.toString());
      set(result ? ImportCommandStatus.Success : ImportCommandStatus.Failure);
      return result;
    }
  }

  private class UpdateImportCommand extends Command<Import, ImportCommandStatus> {

    private final String jobName;
    private final BigInteger jobGenerationId;
    private final String importName;

    UpdateImportCommand(Job job, Import imp) {
      super(ImportCommandStatus.Unknown);
      this.jobName = job.getMetadata().getName();
      this.jobGenerationId = job.getSpec().getGenerationId();
      this.importName = imp.getMetadata().getName();
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ImportCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var imp = importStore.getImportWithName(importName);
      if (!imp.isPresent()) {
        LOGGER.debug("Import {} not found", importName);
        set(ImportCommandStatus.ImportNotFound);
        return Action.Remove;
      }
      /*
       * Update the import.
       */
      importFactory.updateImport(job.get(), imp.get());
      return Action.Wait;
    }

    @Override
    public boolean check(Import pre, Import cur) {
      var curGen = new BigInteger(cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY));
      var result = curGen.equals(jobGenerationId);
      set(result ? ImportCommandStatus.Success : ImportCommandStatus.Failure);
      return result;
    }
  }

  private void processStatus(ImportCommandStatus status) {
    switch (status) {
      case Failure:
        throw new IllegalArgumentException("Operation failed");
      case JobNotFound:
        throw new IllegalArgumentException("Job not found");
      case ImportNotFound:
        throw new IllegalArgumentException("Import not found");
      case NoChangeNeeded:
      case Success:
        break;
      case Unknown:
        throw new IllegalArgumentException("Invalid state");
    }
  }

  @Override
  public void setImportFilter(Import imp, String filter) {
    var command = new SetImportFilterCommand(imp, filter);
    LOGGER.debug("Import {} SET filter {}", imp.getMetadata().getName(), filter);
    apply(imp, command);
    processStatus(command.get());
    LOGGER.debug("Import {} SET filter -- DONE", imp.getMetadata().getName());
  }

  @Override
  public void setImportImportedStreams(Import imp, ImportedStreamsType streams) {
    var ip = ImportExportCommons.getImportedStreams(streams);
    LOGGER.debug("Import {} SET imported streams {}", imp.getMetadata().getName(), ip);
    var command = new SetImportImportedStreamCommand(imp, ip);
    apply(imp, command);
    processStatus(command.get());
    LOGGER.debug("Import {} SET imported streams -- DONE", imp.getMetadata().getName());
  }

  @Override
  public void updateImport(Job job, Import imp) {
    var command = new UpdateImportCommand(job, imp);
    apply(imp, command);
    processStatus(command.get());
  }
}
