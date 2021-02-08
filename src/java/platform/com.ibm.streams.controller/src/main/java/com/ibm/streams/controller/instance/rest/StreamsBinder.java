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

package com.ibm.streams.controller.instance.rest;

import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.exports.IExportCoordinator;
import com.ibm.streams.controller.crds.imports.IImportCoordinator;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import com.ibm.streams.controller.crds.prs.IParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.instance.IStreamsInstance;
import com.ibm.streams.controller.k8s.services.IServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceStore;
import com.ibm.streams.controller.state.capture.CaptureConverter;
import com.ibm.streams.controller.state.subscription.SubscriptionBoard;
import org.glassfish.hk2.utilities.binding.AbstractBinder;

public class StreamsBinder extends AbstractBinder {

  private final IStreamsInstance instance;
  private final String namespace;

  StreamsBinder(IStreamsInstance instance, String ns) {
    this.instance = instance;
    this.namespace = ns;
  }

  @Override
  protected void configure() {
    bind(instance.getCaptureConverter()).to(CaptureConverter.class);
    bind(instance.getExportCoordinator()).to(IExportCoordinator.class);
    bind(instance.getExportStore()).to(ExportStore.class);
    bind(instance.getImportCoordinator()).to(IImportCoordinator.class);
    bind(instance.getImportStore()).to(ImportStore.class);
    bind(instance.getJobStore()).to(JobStore.class);
    bind(instance.getParallelRegionCoordinator()).to(IParallelRegionCoordinator.class);
    bind(instance.getParallelRegionStore()).to(ParallelRegionStore.class);
    bind(instance.getProcessingElementCoordinator()).to(IProcessingElementCoordinator.class);
    bind(instance.getProcessingElementStore()).to(ProcessingElementStore.class);
    bind(instance.getServiceCoordinator()).to(IServiceCoordinator.class);
    bind(instance.getServiceStore()).to(ServiceStore.class);
    bind(instance.getSubscriptionBoard()).to(SubscriptionBoard.class);
    bind(namespace).to(String.class).named("namespace");
  }
}
