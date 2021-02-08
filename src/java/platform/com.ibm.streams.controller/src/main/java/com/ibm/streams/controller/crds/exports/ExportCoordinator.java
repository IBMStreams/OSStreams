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

package com.ibm.streams.controller.crds.exports;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.sync.Command;
import com.ibm.streams.controller.sync.Coordinator;
import com.ibm.streams.instance.sam.SamUtilFunctions;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.platform.services.StreamPropertiesType;
import com.ibm.streams.platform.services.StreamPropertyNameType;
import com.ibm.streams.platform.services.StreamPropertyNamesType;
import com.ibm.streams.platform.services.StreamPropertyType;
import java.math.BigInteger;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ExportCoordinator extends Coordinator<Export, ExportCommandStatus>
    implements IExportCoordinator {

  private static final Logger LOGGER = LoggerFactory.getLogger(ExportCoordinator.class);

  private final ExportStore exportStore;
  private final ExportFactory exportFactory;
  private final JobStore jobStore;

  public ExportCoordinator(ExportStore expStore, ExportFactory expFactory, JobStore jobStore) {
    this.exportStore = expStore;
    this.exportFactory = expFactory;
    this.jobStore = jobStore;
  }

  private class AddExportPropertiesCommand extends Command<Export, ExportCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String exportName;
    private final List<StreamProperty> properties;

    AddExportPropertiesCommand(Export exp, StreamProperty property) {
      this(exp, Collections.singletonList(property));
    }

    AddExportPropertiesCommand(Export exp, List<StreamProperty> properties) {
      super(ExportCommandStatus.Unknown);
      this.jobName = exp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = exp.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.exportName = exp.getMetadata().getName();
      this.properties = properties;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ExportCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var exp = exportStore.getExportWithNameAndGenerationId(exportName, jobGenerationId);
      if (!exp.isPresent()) {
        LOGGER.debug("Export {} not found", exportName);
        set(ExportCommandStatus.ExportNotFound);
        return Action.Remove;
      }
      /*
       * Check if the export is by properties.
       */
      if (exp.get().getSpec().getType() != EExportedStreamType.ByProperties) {
        LOGGER.debug("Export {} has no properties", exportName);
        set(ExportCommandStatus.ExportHasNoProperties);
        return Action.Remove;
      }
      /*
       * Get the Export's properties.
       */
      var origin =
          Optional.ofNullable(exp.get().getSpec().getStream().getProperties())
              .orElseGet(LinkedList::new);
      /*
       * Check that no new property exists and add the value.
       */
      var keys = properties.stream().map(StreamProperty::getName).collect(Collectors.toList());
      var orgn = origin.stream().map(StreamProperty::getName).collect(Collectors.toList());
      if (keys.stream().anyMatch(orgn::contains)) {
        LOGGER.debug("A property already exist in Export {}", exportName);
        set(ExportCommandStatus.PropertyAlreadyExists);
        return Action.Remove;
      }
      /*
       * Update the export property.
       */
      var update = new LinkedList<>(origin);
      update.addAll(properties);
      /*
       * Make sure the update differs from the original.
       */
      if (update.equals(origin)) {
        set(ExportCommandStatus.NoChangeNeeded);
        return Action.Remove;
      }
      /*
       * Submit the update.
       */
      LOGGER.debug("Update export {} properties {}", exportName, update);
      exportFactory.updateExport(exp.get(), update);
      return Action.Wait;
    }

    @Override
    public boolean check(Export pre, Export cur) {
      var keys = properties.stream().map(StreamProperty::getName).collect(Collectors.toList());
      var prps =
          cur.getSpec().getStream().getProperties().stream()
              .map(StreamProperty::getName)
              .collect(Collectors.toList());
      var result = prps.containsAll(keys);
      set(result ? ExportCommandStatus.Success : ExportCommandStatus.Failure);
      return result;
    }
  }

  private class SetExportPropertiesCommand extends Command<Export, ExportCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String exportName;
    private final List<StreamProperty> properties;

    SetExportPropertiesCommand(Export exp, StreamProperty property) {
      this(exp, Collections.singletonList(property));
    }

    SetExportPropertiesCommand(Export exp, List<StreamProperty> properties) {
      super(ExportCommandStatus.Unknown);
      this.jobName = exp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = exp.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.exportName = exp.getMetadata().getName();
      this.properties = properties;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ExportCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var exp = exportStore.getExportWithNameAndGenerationId(exportName, jobGenerationId);
      if (!exp.isPresent()) {
        LOGGER.debug("Export {} not found", exportName);
        set(ExportCommandStatus.ExportNotFound);
        return Action.Remove;
      }
      /*
       * Check if the export is by properties.
       */
      if (exp.get().getSpec().getType() != EExportedStreamType.ByProperties) {
        LOGGER.debug("Export {} has no properties", exportName);
        set(ExportCommandStatus.ExportHasNoProperties);
        return Action.Remove;
      }
      /*
       * Get the Export's properties.
       */
      var origin =
          Optional.ofNullable(exp.get().getSpec().getStream().getProperties())
              .orElseGet(LinkedList::new);
      /*
       * Check that no new property exists and add the value.
       */
      var keys = properties.stream().map(StreamProperty::getName).collect(Collectors.toList());
      var orgn = origin.stream().map(StreamProperty::getName).collect(Collectors.toList());
      if (!orgn.containsAll(keys)) {
        LOGGER.debug("A property is missing in Export {}", exportName);
        set(ExportCommandStatus.PropertyNotFound);
        return Action.Remove;
      }
      /*
       * Set the export properties.
       */
      var update = new LinkedList<StreamProperty>();
      for (var p : origin) {
        var n = properties.stream().filter(e -> e.getName().equals(p.getName())).findFirst();
        update.add(n.orElse(p));
      }
      /*
       * Make sure the update differs from the original.
       */
      if (update.equals(origin)) {
        set(ExportCommandStatus.NoChangeNeeded);
        return Action.Remove;
      }
      /*
       * Submit the update.
       */
      LOGGER.debug("Update export {} properties {}", exportName, update);
      exportFactory.updateExport(exp.get(), update);
      return Action.Wait;
    }

    @Override
    public boolean check(Export pre, Export cur) {
      var keys = properties.stream().map(StreamProperty::getName).collect(Collectors.toList());
      var prps =
          cur.getSpec().getStream().getProperties().stream()
              .map(StreamProperty::getName)
              .collect(Collectors.toList());
      var result = prps.containsAll(keys);
      set(result ? ExportCommandStatus.Success : ExportCommandStatus.Failure);
      return result;
    }
  }

  private class RemoveExportPropertyCommand extends Command<Export, ExportCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final String exportName;
    private final List<String> propertyNames;

    RemoveExportPropertyCommand(Export exp, String name) {
      this(exp, Collections.singletonList(name));
    }

    RemoveExportPropertyCommand(Export exp, List<String> names) {
      super(ExportCommandStatus.Unknown);
      this.jobName = exp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = exp.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.exportName = exp.getMetadata().getName();
      this.propertyNames = names;
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ExportCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var exp = exportStore.getExportWithNameAndGenerationId(exportName, jobGenerationId);
      if (!exp.isPresent()) {
        LOGGER.debug("Export {} not found", exportName);
        set(ExportCommandStatus.ExportNotFound);
        return Action.Remove;
      }
      /*
       * Check if the export is by properties.
       */
      if (exp.get().getSpec().getType() != EExportedStreamType.ByProperties) {
        LOGGER.debug("Export {} has no properties", exportName);
        set(ExportCommandStatus.ExportHasNoProperties);
        return Action.Remove;
      }
      /*
       * Get the Export's properties.
       */
      var origin =
          Optional.ofNullable(exp.get().getSpec().getStream().getProperties())
              .orElseGet(LinkedList::new);
      /*
       * Check that the export has all the properties we want removed.
       */
      var keys = origin.stream().map(StreamProperty::getName).collect(Collectors.toList());
      if (!keys.containsAll(propertyNames)) {
        LOGGER.debug("A property is missing in Export {}", exportName);
        set(ExportCommandStatus.PropertyNotFound);
        return Action.Remove;
      }
      /*
       * Set the export properties.
       */
      var update = new LinkedList<>(origin);
      update.removeIf(e -> propertyNames.contains(e.getName()));
      /*
       * Make sure the update differs from the original.
       */
      if (update.equals(origin)) {
        set(ExportCommandStatus.NoChangeNeeded);
        return Action.Remove;
      }
      /*
       * Submit the update.
       */
      LOGGER.debug("Update export {} properties {}", exportName, update);
      exportFactory.updateExport(exp.get(), update.isEmpty() ? null : update);
      return Action.Wait;
    }

    @Override
    public boolean check(Export pre, Export cur) {
      var props = cur.getSpec().getStream().getProperties();
      var result =
          props == null
              || props.stream().map(StreamProperty::getName).noneMatch(propertyNames::contains);
      set(result ? ExportCommandStatus.Success : ExportCommandStatus.Failure);
      return result;
    }
  }

  private class UpdateExportCommand extends Command<Export, ExportCommandStatus> {

    private final String jobName;
    private final BigInteger jobGenerationId;
    private final String exportName;

    UpdateExportCommand(Job job, Export exp) {
      super(ExportCommandStatus.Unknown);
      this.jobName = job.getMetadata().getName();
      this.jobGenerationId = job.getSpec().getGenerationId();
      this.exportName = exp.getMetadata().getName();
    }

    @Override
    public Action run() {
      /*
       * Find the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ExportCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Find the Export.
       */
      var exp = exportStore.getExportWithName(exportName);
      if (!exp.isPresent()) {
        LOGGER.debug("Export {} not found", exportName);
        set(ExportCommandStatus.ExportNotFound);
        return Action.Remove;
      }
      /*
       * Update the export.
       *
       * NOTE(xrg) Parallel width update should not change the nature of the export. Therefore, we keep
       * the existing spec to carry any user modifications through the update.
       */
      exportFactory.updateExport(job.get(), exp.get());
      return Action.Wait;
    }

    @Override
    public boolean check(Export pre, Export cur) {
      var curGen = new BigInteger(cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY));
      var result = curGen.equals(jobGenerationId);
      set(result ? ExportCommandStatus.Success : ExportCommandStatus.Failure);
      return result;
    }
  }

  private void processStatus(ExportCommandStatus status) {
    switch (status) {
      case Failure:
        throw new IllegalArgumentException("Operation failed");
      case JobNotFound:
        throw new IllegalArgumentException("Job not found");
      case ExportNotFound:
        throw new IllegalArgumentException("Export not found");
      case ExportHasNoProperties:
        throw new IllegalArgumentException("Export has no properties");
      case PropertyAlreadyExists:
        throw new IllegalArgumentException("Property already exists");
      case PropertyNotFound:
        throw new IllegalArgumentException("Property not found");
      case NoChangeNeeded:
      case Success:
        break;
      case Unknown:
        throw new IllegalArgumentException("Invalid state");
    }
  }

  @Override
  public void addExportProperty(Export exp, StreamPropertyType property) {
    var value = SamUtilFunctions.getStreamPropertyFromStreamPropertyType(property);
    var command = new AddExportPropertiesCommand(exp, value);
    LOGGER.debug("Export {} ADD property {}", exp.getMetadata().getName(), value);
    apply(exp, command);
    processStatus(command.get());
  }

  @Override
  public void addExportProperties(Export exp, StreamPropertiesType properties) {
    var values = SamUtilFunctions.getStreamPropertyFromStreamPropertiesType(properties);
    var command = new AddExportPropertiesCommand(exp, values);
    LOGGER.debug("Export {} ADD properties {}", exp.getMetadata().getName(), values);
    apply(exp, command);
    processStatus(command.get());
  }

  @Override
  public void setExportProperty(Export exp, StreamPropertyType property) {
    var value = SamUtilFunctions.getStreamPropertyFromStreamPropertyType(property);
    var command = new SetExportPropertiesCommand(exp, value);
    LOGGER.debug("Export {} SET property {}", exp.getMetadata().getName(), value);
    apply(exp, command);
    processStatus(command.get());
  }

  @Override
  public void setExportProperties(Export exp, StreamPropertiesType properties) {
    var values = SamUtilFunctions.getStreamPropertyFromStreamPropertiesType(properties);
    var command = new SetExportPropertiesCommand(exp, values);
    LOGGER.debug("Export {} SET properties {}", exp.getMetadata().getName(), values);
    apply(exp, command);
    processStatus(command.get());
  }

  @Override
  public void removeExportProperty(Export exp, String name) {
    var command = new RemoveExportPropertyCommand(exp, name);
    LOGGER.debug("Export {} REM property {}", exp.getMetadata().getName(), name);
    apply(exp, command);
    processStatus(command.get());
  }

  @Override
  public void removeExportProperties(Export exp, StreamPropertyNamesType names) {
    var nameList =
        names.getName().stream().map(StreamPropertyNameType::getName).collect(Collectors.toList());
    var command = new RemoveExportPropertyCommand(exp, nameList);
    LOGGER.debug("Export {} REM property {}", exp.getMetadata().getName(), nameList);
    apply(exp, command);
    processStatus(command.get());
  }

  @Override
  public void updateExport(Job job, Export exp) {
    var command = new UpdateExportCommand(job, exp);
    apply(exp, command);
    processStatus(command.get());
  }
}
