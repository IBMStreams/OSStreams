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

package com.ibm.streams.controller.instance;

import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.exports.IExportCoordinator;
import com.ibm.streams.controller.crds.imports.IImportCoordinator;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import com.ibm.streams.controller.crds.prs.IParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.k8s.services.IServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceStore;
import com.ibm.streams.controller.state.capture.CaptureConverter;
import com.ibm.streams.controller.state.subscription.SubscriptionBoard;

public interface IStreamsInstance {

  CaptureConverter getCaptureConverter();

  IExportCoordinator getExportCoordinator();

  ExportStore getExportStore();

  IImportCoordinator getImportCoordinator();

  ImportStore getImportStore();

  JobStore getJobStore();

  IParallelRegionCoordinator getParallelRegionCoordinator();

  ParallelRegionStore getParallelRegionStore();

  IProcessingElementCoordinator getProcessingElementCoordinator();

  ProcessingElementStore getProcessingElementStore();

  IServiceCoordinator getServiceCoordinator();

  ServiceStore getServiceStore();

  SubscriptionBoard getSubscriptionBoard();
}
