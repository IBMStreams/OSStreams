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

import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorController;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorFactory;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorStore;
import com.ibm.streams.controller.crds.crs.ConsistentRegionFactory;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.instance.ConsistentRegionController;
import com.ibm.streams.controller.crds.exports.ExportController;
import com.ibm.streams.controller.crds.exports.ExportCoordinator;
import com.ibm.streams.controller.crds.exports.ExportFactory;
import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.exports.IExportCoordinator;
import com.ibm.streams.controller.crds.hostpools.HostPoolController;
import com.ibm.streams.controller.crds.hostpools.HostPoolFactory;
import com.ibm.streams.controller.crds.hostpools.HostPoolStore;
import com.ibm.streams.controller.crds.imports.IImportCoordinator;
import com.ibm.streams.controller.crds.imports.ImportController;
import com.ibm.streams.controller.crds.imports.ImportCoordinator;
import com.ibm.streams.controller.crds.imports.ImportFactory;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.jobs.JobController;
import com.ibm.streams.controller.crds.jobs.JobFactory;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.jobs.fsm.JobStateMachine;
import com.ibm.streams.controller.crds.pes.ProcessingElementFactory;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import com.ibm.streams.controller.crds.pes.instance.ProcessingElementController;
import com.ibm.streams.controller.crds.pes.instance.ProcessingElementCoordinator;
import com.ibm.streams.controller.crds.prs.IParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegionController;
import com.ibm.streams.controller.crds.prs.ParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegionFactory;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.executor.IExecutor;
import com.ibm.streams.controller.instance.broker.ImportExportBroker;
import com.ibm.streams.controller.instance.jobs.JobLauncher;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapCache;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapController;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapFactory;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapStore;
import com.ibm.streams.controller.k8s.deployments.DeploymentFactory;
import com.ibm.streams.controller.k8s.pods.PodFactory;
import com.ibm.streams.controller.k8s.pods.PodStore;
import com.ibm.streams.controller.k8s.pods.pes.PodController;
import com.ibm.streams.controller.k8s.pods.pes.fsm.PodStateMachine;
import com.ibm.streams.controller.k8s.services.IServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceController;
import com.ibm.streams.controller.k8s.services.ServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceFactory;
import com.ibm.streams.controller.k8s.services.ServiceStore;
import com.ibm.streams.controller.state.capture.CaptureConverter;
import com.ibm.streams.controller.state.subscription.SubscriptionBoard;
import com.ibm.streams.controller.utils.CommonEnvironment;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.time.Instant;
import lombok.Getter;

public class StreamsInstance implements IStreamsInstance {

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

  @Getter private final ConfigMapFactory configMapFactory;
  @Getter private final ConsistentRegionFactory consistentRegionFactory;
  @Getter private final ConsistentRegionOperatorFactory consistentRegionOperatorFactory;
  @Getter private final DeploymentFactory deploymentFactory;
  @Getter private final ExportFactory exportFactory;
  @Getter private final HostPoolFactory hostPoolFactory;
  @Getter private final ImportFactory importFactory;
  @Getter private final JobFactory jobFactory;
  @Getter private final ProcessingElementFactory processingElementFactory;
  @Getter private final PodFactory podFactory;
  @Getter private final ParallelRegionFactory parallelRegionFactory;
  @Getter private final ServiceFactory serviceFactory;

  @Getter private final IExportCoordinator exportCoordinator;
  @Getter private final IImportCoordinator importCoordinator;
  @Getter private final IParallelRegionCoordinator parallelRegionCoordinator;
  @Getter private final IProcessingElementCoordinator processingElementCoordinator;
  @Getter private final IServiceCoordinator serviceCoordinator;

  @Getter private final CaptureConverter captureConverter;
  @Getter private final SubscriptionBoard subscriptionBoard;
  @Getter private final ImportExportBroker importExportBroker;
  @Getter private final JobStateMachine jobStateMachine;
  @Getter private final Thread jobStateMachineThread;
  @Getter private final JobLauncher jobLauncher;

  @Getter private final PodStateMachine podStateMachine;
  @Getter private final Thread podStateMachineThread;

  @Getter private final ConfigMapController configMapController;
  @Getter private final ConsistentRegionController consistentRegionController;
  @Getter private final ConsistentRegionOperatorController consistentRegionOperatorController;
  @Getter private final ExportController exportController;
  @Getter private final HostPoolController hostPoolController;
  @Getter private final ImportController importController;
  @Getter private final JobController jobController;
  @Getter private final ParallelRegionController parallelRegionController;
  @Getter private final PodController podController;
  @Getter private final ProcessingElementController processingElementController;
  @Getter private final ServiceController serviceController;

  public StreamsInstance(
      KubernetesClient client,
      IExecutor executor,
      String ns,
      CommonEnvironment env,
      Instant creationTime) {
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
     * Create the factories.
     */
    this.configMapFactory = new ConfigMapFactory(client, configMapStore);
    this.consistentRegionFactory = new ConsistentRegionFactory(client);
    this.consistentRegionOperatorFactory = new ConsistentRegionOperatorFactory(client);
    this.deploymentFactory = new DeploymentFactory(client, env);
    this.exportFactory = new ExportFactory(client);
    this.hostPoolFactory = new HostPoolFactory(client);
    this.importFactory = new ImportFactory(client);
    this.jobFactory = new JobFactory(client);
    this.processingElementFactory = new ProcessingElementFactory(client);
    this.podFactory = new PodFactory(client, podStore);
    this.parallelRegionFactory = new ParallelRegionFactory(client);
    this.serviceFactory = new ServiceFactory(client);
    /*
     * Create the coordinators.
     */
    this.exportCoordinator = new ExportCoordinator(exportStore, exportFactory, jobStore);
    this.importCoordinator = new ImportCoordinator(importStore, importFactory, jobStore);
    this.parallelRegionCoordinator =
        new ParallelRegionCoordinator(parallelRegionStore, parallelRegionFactory, jobStore);
    this.processingElementCoordinator =
        new ProcessingElementCoordinator(
            jobStore, processingElementStore, processingElementFactory);
    this.serviceCoordinator = new ServiceCoordinator(jobStore, serviceStore, serviceFactory);
    /*
     * Create the internal state objects.
     */
    this.captureConverter =
        new CaptureConverter(jobStore, configMapStore, configMapCache, parallelRegionStore);
    this.subscriptionBoard = new SubscriptionBoard();
    this.importExportBroker =
        new ImportExportBroker(
            subscriptionBoard, jobStore, importStore, exportStore, executor, ns, env);
    this.jobStateMachine =
        new JobStateMachine(
            jobStore,
            jobFactory,
            configMapStore,
            configMapFactory,
            serviceStore,
            serviceFactory,
            podStore,
            processingElementStore,
            processingElementFactory,
            importStore,
            importFactory,
            exportStore,
            exportFactory,
            hostPoolStore,
            consistentRegionOperatorStore,
            consistentRegionStore,
            parallelRegionStore,
            parallelRegionFactory,
            executor);
    this.jobStateMachineThread = new Thread(jobStateMachine);
    this.jobLauncher =
        new JobLauncher(
            jobFactory,
            exportFactory,
            exportStore,
            exportCoordinator,
            importFactory,
            importStore,
            importCoordinator,
            hostPoolFactory,
            parallelRegionFactory,
            parallelRegionStore,
            parallelRegionCoordinator,
            processingElementFactory,
            processingElementStore,
            processingElementCoordinator,
            consistentRegionFactory,
            consistentRegionOperatorFactory,
            configMapFactory,
            serviceFactory,
            serviceStore,
            serviceCoordinator,
            executor,
            env);

    this.podStateMachine =
        new PodStateMachine(
            client,
            processingElementStore,
            jobStore,
            hostPoolStore,
            configMapStore,
            podStore,
            podFactory,
            executor,
            env);
    this.podStateMachineThread = new Thread(podStateMachine);
    /*
     * Create the controllers.
     */
    this.configMapController =
        new ConfigMapController(client, configMapStore, configMapFactory, configMapCache, ns);
    this.consistentRegionController =
        new ConsistentRegionController(
            client, consistentRegionStore,
            consistentRegionFactory, ns);
    this.consistentRegionOperatorController =
        new ConsistentRegionOperatorController(
            creationTime,
            client,
            consistentRegionOperatorFactory,
            consistentRegionOperatorStore,
            serviceFactory,
            deploymentFactory,
            ns,
            env);
    this.exportController = new ExportController(client, exportStore, exportFactory, jobStore, ns);
    this.hostPoolController = new HostPoolController(client, hostPoolStore, hostPoolFactory, ns);
    this.importController = new ImportController(client, importStore, importFactory, jobStore, ns);
    this.jobController =
        new JobController(creationTime, client, jobFactory, jobStore, jobLauncher, ns, env);
    this.processingElementController =
        new ProcessingElementController(
            creationTime,
            client,
            processingElementFactory,
            processingElementStore,
            processingElementCoordinator,
            jobStore,
            ns);
    this.podController =
        new PodController(
            creationTime,
            client,
            podFactory,
            podStore,
            jobStore,
            processingElementStore,
            processingElementCoordinator,
            ns);
    this.parallelRegionController =
        new ParallelRegionController(
            creationTime,
            client,
            jobStore,
            jobLauncher,
            parallelRegionStore,
            parallelRegionFactory,
            configMapCache,
            configMapStore,
            ns,
            env);
    this.serviceController =
        new ServiceController(client, serviceStore, serviceFactory, jobStore, ns);
    /*
     * Register the job state machine listener.
     */
    this.configMapController.addGenericListener(jobStateMachine);
    this.consistentRegionController.addGenericListener(jobStateMachine);
    this.consistentRegionOperatorController.addGenericListener(jobStateMachine);
    this.exportController.addGenericListener(jobStateMachine);
    this.hostPoolController.addGenericListener(jobStateMachine);
    this.importController.addGenericListener(jobStateMachine);
    this.jobController.addGenericListener(jobStateMachine);
    this.processingElementController.addGenericListener(jobStateMachine);
    this.podController.addGenericListener(jobStateMachine);
    this.parallelRegionController.addGenericListener(jobStateMachine);
    this.serviceController.addGenericListener(jobStateMachine);
    /*
     * Register the pod state machine listener.
     */
    this.configMapController.addGenericListener(podStateMachine);
    this.hostPoolController.addGenericListener(podStateMachine);
    this.jobController.addGenericListener(podStateMachine);
    this.processingElementController.addGenericListener(podStateMachine);
    /*
     * Register the import/export broker.
     */
    importController.addGenericListener(importExportBroker);
    exportController.addGenericListener(importExportBroker);
    /*
     * Register the instance listeners.
     */
    this.exportController.addListener(exportCoordinator);
    this.importController.addListener(importCoordinator);
    this.parallelRegionController.addListener(parallelRegionCoordinator);
    this.processingElementController.addListener(processingElementCoordinator);
    this.serviceController.addListener(serviceCoordinator);
  }

  public void start() throws IOException {
    configMapController.start();
    consistentRegionController.start();
    consistentRegionOperatorController.start();
    exportController.start();
    hostPoolController.start();
    importController.start();
    jobController.start();
    jobStateMachineThread.start();
    podStateMachineThread.start();
    processingElementController.start();
    podController.start();
    parallelRegionController.start();
    serviceController.start();
  }

  public void close() throws IOException {
    /*
     * Close all controllers and terminate state machines.
     */
    configMapController.close();
    consistentRegionController.close();
    consistentRegionOperatorController.close();
    exportController.close();
    hostPoolController.close();
    importController.close();
    jobController.close();
    jobStateMachine.terminate();
    jobLauncher.close();
    podStateMachine.terminate();
    processingElementController.close();
    podController.close();
    parallelRegionController.close();
    serviceController.close();
    /*
     * Clear all stores to help the JVM garbage-collect between test runs.
     */
    configMapCache.clear();
    configMapStore.clear();
    consistentRegionOperatorStore.clear();
    consistentRegionStore.clear();
    exportStore.clear();
    hostPoolStore.clear();
    importStore.clear();
    jobStore.clear();
    parallelRegionStore.clear();
    podStore.clear();
    processingElementStore.clear();
    serviceStore.clear();
  }
}
