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

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.instance.sam.InputPortLabel;
import com.ibm.streams.controller.sync.Command;
import com.ibm.streams.controller.sync.Coordinator;
import io.fabric8.kubernetes.api.model.Service;
import io.fabric8.kubernetes.api.model.ServicePort;
import java.math.BigInteger;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ServiceCoordinator extends Coordinator<Service, ServiceCommandStatus>
    implements IServiceCoordinator {

  private static final Logger LOGGER = LoggerFactory.getLogger(ServiceCoordinator.class);

  private final JobStore jobStore;
  private final ServiceStore svcStore;
  private final ServiceFactory svcFactory;

  public ServiceCoordinator(JobStore jobStore, ServiceStore svcStore, ServiceFactory svcFactory) {
    this.jobStore = jobStore;
    this.svcStore = svcStore;
    this.svcFactory = svcFactory;
  }

  private class AddUserServicePortCommand extends Command<Service, ServiceCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final BigInteger peId;
    private final String svcName;
    private final String svcScope;
    private final ServicePort port;

    AddUserServicePortCommand(Service svc, ServicePort port) {
      super(ServiceCommandStatus.Unknown);
      this.jobName = svc.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = svc.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.peId = new BigInteger(svc.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY));
      this.svcName = svc.getMetadata().getName();
      this.svcScope = svc.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY);
      this.port = port;
    }

    @Override
    public Action run() {
      /*
       * Check if the port already exists.
       */
      var match =
          svcStore.values().stream()
              .filter(
                  e ->
                      e.getMetadata()
                          .getAnnotations()
                          .containsKey(STREAMS_APP_SCOPE_ANNOTATION_KEY))
              .filter(
                  e ->
                      e.getMetadata()
                          .getAnnotations()
                          .get(STREAMS_APP_SCOPE_ANNOTATION_KEY)
                          .equals(svcScope))
              .flatMap(e -> e.getSpec().getPorts().stream())
              .anyMatch(e -> e.getName().contains(port.getName()));
      /*
       * If so, cancel the execution.
       */
      if (match) {
        set(ServiceCommandStatus.PortNameAlreadyExists);
        return Action.Remove;
      }
      /*
       * Grab the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ServiceCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Grab the service.
       */
      var svc = svcStore.getServiceWithName(svcName);
      if (!svc.isPresent()) {
        LOGGER.debug("Service {} not found", svcName);
        set(ServiceCommandStatus.ServiceNotFound);
        return Action.Remove;
      }
      /*
       * Add the new port.
       */
      var ports = Optional.ofNullable(svc.get().getSpec().getPorts()).orElse(new LinkedList<>());
      ports.add(port);
      svcFactory.updateService(svc.get(), ports);
      return Action.Wait;
    }

    @Override
    public boolean check(Service pre, Service cur) {
      var result =
          cur.getSpec().getPorts().stream().anyMatch(e -> e.getName().equals(port.getName()));
      set(result ? ServiceCommandStatus.Success : ServiceCommandStatus.Failure);
      return result;
    }
  }

  private class DeleteUserServicePortCommand extends Command<Service, ServiceCommandStatus> {

    private final String jobName;
    private final String jobGenerationId;
    private final BigInteger peId;
    private final String svcName;
    private final String epName;

    DeleteUserServicePortCommand(Service svc, String epName) {
      super(ServiceCommandStatus.Unknown);
      this.jobName = svc.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      this.jobGenerationId = svc.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
      this.peId = new BigInteger(svc.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY));
      this.svcName = svc.getMetadata().getName();
      this.epName = epName;
    }

    @Override
    public Action run() {
      /*
       * Grab the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ServiceCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Grab the service.
       */
      var svc = svcStore.getServiceWithName(svcName);
      if (!svc.isPresent()) {
        LOGGER.debug("Service {} not found", svcName);
        set(ServiceCommandStatus.ServiceNotFound);
        return Action.Remove;
      }
      /*
       * Check if the port exists.
       */
      if (svc.get().getSpec().getPorts().stream().noneMatch(e -> e.getName().equals(epName))) {
        set(ServiceCommandStatus.PortNameNotFound);
        return Action.Remove;
      }
      /*
       * Remove the port.
       */
      var ports =
          Optional.ofNullable(svc.get().getSpec().getPorts()).orElse(new LinkedList<>()).stream()
              .filter(e -> !e.getName().equals(epName))
              .collect(Collectors.toList());
      svcFactory.updateService(svc.get(), ports);
      return Action.Wait;
    }

    @Override
    public boolean check(Service pre, Service cur) {
      var result = cur.getSpec().getPorts().stream().noneMatch(e -> e.getName().equals(epName));
      set(result ? ServiceCommandStatus.Success : ServiceCommandStatus.Failure);
      return result;
    }
  }

  private class UpdateServiceCommand extends Command<Service, ServiceCommandStatus> {

    private final String jobName;
    private final BigInteger jobGenerationId;
    private final BigInteger peId;
    private final String svcName;
    private final List<InputPortLabel> svcPorts;

    UpdateServiceCommand(Job job, Service svc, List<InputPortLabel> ports) {
      super(ServiceCommandStatus.Unknown);
      this.jobName = job.getMetadata().getName();
      this.jobGenerationId = job.getSpec().getGenerationId();
      this.peId = new BigInteger(svc.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY));
      this.svcName = svc.getMetadata().getName();
      this.svcPorts = ports;
    }

    @Override
    public Action run() {
      /*
       * Grab the job.
       */
      var job = jobStore.getJobWithNameAndGenerationId(jobName, jobGenerationId);
      if (!job.isPresent()) {
        LOGGER.debug("Job ({}, {}) not found", jobName, jobGenerationId);
        set(ServiceCommandStatus.JobNotFound);
        return Action.Remove;
      }
      /*
       * Grab the service.
       */
      var svc = svcStore.getServiceWithName(svcName);
      if (!svc.isPresent()) {
        LOGGER.debug("Service {} not found", svcName);
        set(ServiceCommandStatus.ServiceNotFound);
        return Action.Remove;
      }
      /*
       * Grab the user-defined ports. They should be carried through the update.
       */
      var ports =
          svcPorts.stream()
              .map(ServiceFactory::convertPortLabelToServicePort)
              .collect(Collectors.toList());
      var usr =
          svc.get().getSpec().getPorts().stream()
              .filter(p -> p.getName().startsWith("usr-"))
              .collect(Collectors.toList());
      ports.addAll(usr);
      /*
       * Update the service.
       */
      svcFactory.updateService(job.get(), svc.get(), ports);
      return Action.Wait;
    }

    @Override
    public boolean check(Service pre, Service cur) {
      var curGen = new BigInteger(cur.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY));
      var result = curGen.equals(jobGenerationId);
      set(result ? ServiceCommandStatus.Success : ServiceCommandStatus.Failure);
      return result;
    }
  }

  private void processStatus(ServiceCommandStatus status) {
    switch (status) {
      case Failure:
        throw new IllegalArgumentException("Operation failed");
      case JobNotFound:
        throw new IllegalArgumentException("Job not found");
      case PortNameAlreadyExists:
        throw new IllegalArgumentException("Port name already exists");
      case PortNameNotFound:
        throw new IllegalArgumentException("Port name not found");
      case ServiceNotFound:
        throw new IllegalArgumentException("Service not found");
      case Success:
        break;
      case Unknown:
        throw new IllegalArgumentException("Invalid state");
    }
  }

  @Override
  public void addUserServicePort(Service svc, ServicePort port) {
    var command = new AddUserServicePortCommand(svc, port);
    LOGGER.debug("Add port {} to service {}", port.getName(), svc.getMetadata().getName());
    apply(svc, command);
    processStatus(command.get());
  }

  @Override
  public void deleteUserServicePort(Service svc, String epName) {
    var command = new DeleteUserServicePortCommand(svc, epName);
    LOGGER.debug("Delete port {} from service {}", epName, svc.getMetadata().getName());
    apply(svc, command);
    processStatus(command.get());
  }

  @Override
  public void updateService(Job job, Service svc, List<InputPortLabel> ports) {
    var command = new UpdateServiceCommand(job, svc, ports);
    apply(svc, command);
    processStatus(command.get());
  }
}
