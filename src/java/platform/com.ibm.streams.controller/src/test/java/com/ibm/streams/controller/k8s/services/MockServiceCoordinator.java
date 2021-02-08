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

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.instance.sam.InputPortLabel;
import io.fabric8.kubernetes.api.model.Service;
import io.fabric8.kubernetes.api.model.ServicePort;
import java.util.List;
import java.util.stream.Collectors;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;

public class MockServiceCoordinator implements IServiceCoordinator {

  private ServiceStore serviceStore;

  public MockServiceCoordinator(ServiceStore serviceStore) {
    this.serviceStore = serviceStore;
  }

  @Override
  public void addUserServicePort(Service svc, ServicePort port) {
    var match =
        serviceStore.values().stream()
            .flatMap(e -> e.getSpec().getPorts().stream())
            .anyMatch(e -> e.getName().contains(port.getName()));
    if (match) {
      throw new IllegalArgumentException();
    }
    svc.getSpec().getPorts().add(port);
  }

  @Override
  public void deleteUserServicePort(Service svc, String epName) {
    if (svc.getSpec().getPorts().stream().noneMatch(e -> e.getName().equals(epName))) {
      throw new IllegalArgumentException();
    }
    svc.getSpec().getPorts().removeIf(p -> p.getName().equals(epName));
  }

  @Override
  public void updateService(Job job, Service svc, List<InputPortLabel> ports) {
    var newPorts =
        ports.stream()
            .map(ServiceFactory::convertPortLabelToServicePort)
            .collect(Collectors.toList());
    svc.getSpec().setPorts(newPorts);
  }

  @Override
  public void onAddition(AbstractEvent<? extends Service> event) {}

  @Override
  public void onModification(AbstractEvent<? extends Service> event) {}

  @Override
  public void onDeletion(AbstractEvent<? extends Service> event) {}
}
