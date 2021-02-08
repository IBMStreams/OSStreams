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

package com.ibm.streams.mock.instance;

import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorStore;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.exports.IExportCoordinator;
import com.ibm.streams.controller.crds.exports.MockExportCoordinator;
import com.ibm.streams.controller.crds.hostpools.HostPoolStore;
import com.ibm.streams.controller.crds.imports.IImportCoordinator;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.imports.MockImportCoordinator;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.MockProcessingElementCoordinator;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import com.ibm.streams.controller.crds.prs.IParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.MockParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.instance.IStreamsInstance;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapCache;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapStore;
import com.ibm.streams.controller.k8s.pods.PodStore;
import com.ibm.streams.controller.k8s.services.IServiceCoordinator;
import com.ibm.streams.controller.k8s.services.MockServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceStore;
import com.ibm.streams.controller.state.capture.CaptureConverter;
import com.ibm.streams.controller.state.subscription.SubscriptionBoard;
import lombok.Getter;

public class MockStreamsInstance implements IStreamsInstance {

  @Getter private final ConfigMapCache configMapCache;
  @Getter private final ConfigMapStore configMapStore;
  @Getter private final ConsistentRegionOperatorStore consistentRegionOperatorStore;
  @Getter private final ConsistentRegionStore consistentRegionStore;
  @Getter private final ExportStore exportStore;
  @Getter private final HostPoolStore hostPoolStore;
  @Getter private final ImportStore importStore;
  @Getter private final JobStore jobStore;
  @Getter private final ParallelRegionStore parallelRegionStore;
  @Getter private final PodStore podStore;
  @Getter private final ProcessingElementStore processingElementStore;
  @Getter private final ServiceStore serviceStore;

  @Getter private final IExportCoordinator exportCoordinator;
  @Getter private final IImportCoordinator importCoordinator;
  @Getter private final IParallelRegionCoordinator parallelRegionCoordinator;
  @Getter private final IProcessingElementCoordinator processingElementCoordinator;
  @Getter private final IServiceCoordinator serviceCoordinator;

  @Getter private final CaptureConverter captureConverter;
  @Getter private final SubscriptionBoard subscriptionBoard;

  public MockStreamsInstance() {
    /*
     * Create the stores.
     */
    this.configMapCache = new ConfigMapCache();
    this.configMapStore = new ConfigMapStore();
    this.consistentRegionStore = new ConsistentRegionStore();
    this.consistentRegionOperatorStore = new ConsistentRegionOperatorStore();
    this.exportStore = new ExportStore();
    this.hostPoolStore = new HostPoolStore();
    this.importStore = new ImportStore();
    this.jobStore = new JobStore();
    this.processingElementStore = new ProcessingElementStore();
    this.podStore = new PodStore();
    this.parallelRegionStore = new ParallelRegionStore();
    this.serviceStore = new ServiceStore();
    /*
     * Create the coordinators.
     */
    this.exportCoordinator = new MockExportCoordinator();
    this.importCoordinator = new MockImportCoordinator();
    this.parallelRegionCoordinator = new MockParallelRegionCoordinator();
    this.processingElementCoordinator = new MockProcessingElementCoordinator();
    this.serviceCoordinator = new MockServiceCoordinator(this.serviceStore);
    /*
     * Create the internal state objects.
     */
    this.captureConverter =
        new CaptureConverter(jobStore, configMapStore, configMapCache, parallelRegionStore);
    this.subscriptionBoard = new SubscriptionBoard();
  }

  public void clear() {
    jobStore.clear();
    processingElementStore.clear();
    subscriptionBoard.clear();
  }
}
