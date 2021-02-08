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

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;

import com.ibm.streams.controller.crds.cros.ConsistentRegionOperator;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorFactory;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorList;
import com.ibm.streams.controller.crds.cros.DoneableConsistentRegionOperator;
import com.ibm.streams.controller.crds.crs.ConsistentRegion;
import com.ibm.streams.controller.crds.crs.ConsistentRegionFactory;
import com.ibm.streams.controller.crds.crs.ConsistentRegionList;
import com.ibm.streams.controller.crds.crs.DoneableConsistentRegion;
import com.ibm.streams.controller.crds.exports.DoneableExport;
import com.ibm.streams.controller.crds.exports.Export;
import com.ibm.streams.controller.crds.exports.ExportController;
import com.ibm.streams.controller.crds.exports.ExportCoordinator;
import com.ibm.streams.controller.crds.exports.ExportFactory;
import com.ibm.streams.controller.crds.exports.ExportList;
import com.ibm.streams.controller.crds.hostpools.DoneableHostPool;
import com.ibm.streams.controller.crds.hostpools.HostPool;
import com.ibm.streams.controller.crds.hostpools.HostPoolFactory;
import com.ibm.streams.controller.crds.hostpools.HostPoolList;
import com.ibm.streams.controller.crds.hostpools.HostPoolStore;
import com.ibm.streams.controller.crds.imports.DoneableImport;
import com.ibm.streams.controller.crds.imports.Import;
import com.ibm.streams.controller.crds.imports.ImportController;
import com.ibm.streams.controller.crds.imports.ImportCoordinator;
import com.ibm.streams.controller.crds.imports.ImportFactory;
import com.ibm.streams.controller.crds.imports.ImportList;
import com.ibm.streams.controller.crds.jobs.DoneableJob;
import com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobController;
import com.ibm.streams.controller.crds.jobs.JobFactory;
import com.ibm.streams.controller.crds.jobs.JobList;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.jobs.fsm.JobStateMachine;
import com.ibm.streams.controller.crds.pes.DoneableProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementFactory;
import com.ibm.streams.controller.crds.pes.ProcessingElementList;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.ProcessingElementController;
import com.ibm.streams.controller.crds.pes.instance.ProcessingElementCoordinator;
import com.ibm.streams.controller.crds.prs.DoneableParallelRegion;
import com.ibm.streams.controller.crds.prs.IParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegion;
import com.ibm.streams.controller.crds.prs.ParallelRegionController;
import com.ibm.streams.controller.crds.prs.ParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegionFactory;
import com.ibm.streams.controller.crds.prs.ParallelRegionList;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.executor.DefaultExecutor;
import com.ibm.streams.controller.executor.IExecutor;
import com.ibm.streams.controller.executor.Utils;
import com.ibm.streams.controller.instance.StreamsInstance;
import com.ibm.streams.controller.instance.jobs.JobLauncher;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapController;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapFactory;
import com.ibm.streams.controller.k8s.pods.PodStore;
import com.ibm.streams.controller.k8s.pods.pes.PodController;
import com.ibm.streams.controller.k8s.pods.pes.fsm.PodStateMachine;
import com.ibm.streams.controller.k8s.services.ServiceController;
import com.ibm.streams.controller.k8s.services.ServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceFactory;
import com.ibm.streams.controller.state.capture.CaptureConverter;
import com.ibm.streams.controller.state.subscription.SubscriptionBoard;
import com.ibm.streams.mock.MockEnvironment;
import com.ibm.streams.mock.MockResourceBase;
import com.ibm.streams.mock.server.KubernetesServer;
import com.ibm.streams.utils.Probe;
import io.fabric8.kubernetes.api.model.ConfigMap;
import io.fabric8.kubernetes.api.model.ConfigMapList;
import io.fabric8.kubernetes.api.model.DoneableConfigMap;
import io.fabric8.kubernetes.api.model.DoneablePod;
import io.fabric8.kubernetes.api.model.DoneableService;
import io.fabric8.kubernetes.api.model.Pod;
import io.fabric8.kubernetes.api.model.PodList;
import io.fabric8.kubernetes.api.model.Service;
import io.fabric8.kubernetes.api.model.ServiceList;
import io.fabric8.kubernetes.api.model.apps.Deployment;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.dsl.NonNamespaceOperation;
import io.fabric8.kubernetes.client.dsl.PodResource;
import io.fabric8.kubernetes.client.dsl.Resource;
import io.fabric8.kubernetes.client.dsl.ServiceResource;
import io.fabric8.kubernetes.client.dsl.base.CustomResourceDefinitionContext;
import io.fabric8.kubernetes.client.dsl.internal.WatchHTTPManager;
import io.fabric8.kubernetes.client.internal.VersionUsageUtils;
import io.fabric8.kubernetes.client.server.mock.KubernetesAttributesExtractor;
import java.io.IOException;
import java.math.BigInteger;
import java.time.Instant;
import java.util.concurrent.TimeUnit;
import lombok.var;
import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.bridge.SLF4JBridgeHandler;

public class MockResource extends MockResourceBase {

  static {
    /*
     * Send JUL log events to SLF4J.
     */
    SLF4JBridgeHandler.removeHandlersForRootLogger();
    SLF4JBridgeHandler.install();
    /*
     * Set the JUL loggers' to the minimum verbosity.
     */
    var loggers = java.util.logging.LogManager.getLogManager().getLoggerNames();
    while (loggers.hasMoreElements()) {
      var logger = java.util.logging.Logger.getLogger(loggers.nextElement());
      logger.setLevel(java.util.logging.Level.OFF);
    }
    /*
     * Set the logging preferences.
     */
    LogManager.getLogger(ConfigMapController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ConfigMapFactory.class).setLevel(Level.DEBUG);
    LogManager.getLogger(Controller.class).setLevel(Level.OFF);
    LogManager.getLogger(DefaultExecutor.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ExportController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ExportCoordinator.class).setLevel(Level.DEBUG);
    LogManager.getLogger(GenericEventQueueConsumer.class).setLevel(Level.OFF);
    LogManager.getLogger(ImportController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ImportCoordinator.class).setLevel(Level.DEBUG);
    LogManager.getLogger(JobController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(JobLauncher.class).setLevel(Level.DEBUG);
    LogManager.getLogger(JobStateMachine.class).setLevel(Level.DEBUG);
    LogManager.getLogger(KubernetesAttributesExtractor.class).setLevel(Level.OFF);
    LogManager.getLogger(KubernetesServer.class).setLevel(Level.INFO);
    LogManager.getLogger(ParallelRegionController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ParallelRegionCoordinator.class).setLevel(Level.DEBUG);
    LogManager.getLogger(PodController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(PodStateMachine.class).setLevel(Level.DEBUG);
    LogManager.getLogger(Probe.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ProcessingElementController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ProcessingElementCoordinator.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ServiceController.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ServiceCoordinator.class).setLevel(Level.DEBUG);
    LogManager.getLogger(ServiceFactory.class).setLevel(Level.DEBUG);
    LogManager.getLogger(VersionUsageUtils.class).setLevel(Level.OFF);
    LogManager.getLogger(WatchHTTPManager.class).setLevel(Level.OFF);
  }

  private KubernetesServer server;
  private KubernetesClient client;

  private CustomResourceDefinitionContext crContext;
  private CustomResourceDefinitionContext croContext;
  private CustomResourceDefinitionContext expContext;
  private CustomResourceDefinitionContext hpContext;
  private CustomResourceDefinitionContext impContext;
  private CustomResourceDefinitionContext jobContext;
  private CustomResourceDefinitionContext peContext;
  private CustomResourceDefinitionContext prContext;

  private StreamsInstance instance;

  private CustomResourceDefinitionContext loadAndCreateCustomResourceContext(
      String fn, String name) {
    client.customResourceDefinitions().load(CRDS_PATH + fn).create();
    return client.customResourceDefinitions().list().getItems().stream()
        .filter(e -> e.getMetadata().getName().equals(name))
        .findAny()
        .map(CustomResourceDefinitionContext::fromCrd)
        .orElseThrow(AssertionError::new);
  }

  @Override
  protected void before() throws IOException {
    /*
     * Creathe the mock environment.
     */
    var cTime = Instant.now();
    var env = new MockEnvironment(APPS_PATH);
    /*
     * Create the mock client and server.
     */
    var forkNumber = Integer.parseInt(System.getProperty("surefire.fork.number", "0"));
    server = new KubernetesServer(30000 + forkNumber);
    server.start();
    client = server.createClient();
    /*
     * Add the CRDs.
     */
    crContext =
        loadAndCreateCustomResourceContext(
            "streams-cr.yaml", ConsistentRegionFactory.STREAMS_CR_CRD_NAME);
    croContext =
        loadAndCreateCustomResourceContext(
            "streams-cro.yaml", ConsistentRegionOperatorFactory.STREAMS_CRO_CRD_NAME);
    expContext =
        loadAndCreateCustomResourceContext(
            "streams-export.yaml", ExportFactory.STREAMS_EXPORT_CRD_NAME);
    hpContext =
        loadAndCreateCustomResourceContext(
            "streams-hostpool.yaml", HostPoolFactory.STREAMS_HOSTPOOL_CRD_NAME);
    impContext =
        loadAndCreateCustomResourceContext(
            "streams-import.yaml", ImportFactory.STREAMS_IMPORT_CRD_NAME);
    jobContext =
        loadAndCreateCustomResourceContext("streams-job.yaml", JobFactory.STREAMS_JOB_CRD_NAME);
    peContext =
        loadAndCreateCustomResourceContext(
            "streams-pe.yaml", ProcessingElementFactory.STREAMS_PE_CRD_NAME);
    prContext =
        loadAndCreateCustomResourceContext(
            "streams-pr.yaml", ParallelRegionFactory.STREAMS_HOSTPOOL_CRD_NAME);
    /*
     * Create the instance.
     */
    var executor =
        Utils.instantiate("com.ibm.streams.controller.executor.DefaultExecutor", IExecutor.class);
    instance = new StreamsInstance(client, executor, KUBE_NAMESPACE, env, cTime);
    /*
     * Start the controllers. They may throw an exception.
     */
    try {
      instance.start();
    } catch (IOException e) {
      fail("Exception on instance start: " + e);
    }
    /*
     * We sleep for one second because sometimes the JobController creation time ends up being the same.
     */
    try {
      TimeUnit.SECONDS.sleep(1);
    } catch (InterruptedException ignored) {
    }
  }

  @Override
  protected void after() {
    /*
     * Close the instance and stop the server.
     */
    try {
      instance.close();
      server.stop();
    } catch (IOException e) {
      fail("Exception on instance stop: " + e);
    }
    /*
     * Hint the JVM to start a garbage collection.
     */
    System.gc();
  }

  public NonNamespaceOperation<Export, ExportList, DoneableExport, Resource<Export, DoneableExport>>
      getExportClient() {
    assertNotNull(client);
    return client
        .customResources(expContext, Export.class, ExportList.class, DoneableExport.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<Import, ImportList, DoneableImport, Resource<Import, DoneableImport>>
      getImportClient() {
    assertNotNull(client);
    return client
        .customResources(impContext, Import.class, ImportList.class, DoneableImport.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<
          HostPool, HostPoolList, DoneableHostPool, Resource<HostPool, DoneableHostPool>>
      getHostPoolClient() {
    assertNotNull(client);
    return client
        .customResources(hpContext, HostPool.class, HostPoolList.class, DoneableHostPool.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<
          ParallelRegion,
          ParallelRegionList,
          DoneableParallelRegion,
          Resource<ParallelRegion, DoneableParallelRegion>>
      getParallelRegionClient() {
    assertNotNull(client);
    return client
        .customResources(
            prContext, ParallelRegion.class, ParallelRegionList.class, DoneableParallelRegion.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<Job, JobList, DoneableJob, Resource<Job, DoneableJob>>
      getJobClient() {
    assertNotNull(client);
    return client
        .customResources(jobContext, Job.class, JobList.class, DoneableJob.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<
          ProcessingElement,
          ProcessingElementList,
          DoneableProcessingElement,
          Resource<ProcessingElement, DoneableProcessingElement>>
      getPeClient() {
    assertNotNull(client);
    return client
        .customResources(
            peContext,
            ProcessingElement.class,
            ProcessingElementList.class,
            DoneableProcessingElement.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<
          ConsistentRegion,
          ConsistentRegionList,
          DoneableConsistentRegion,
          Resource<ConsistentRegion, DoneableConsistentRegion>>
      getCRClient() {
    assertNotNull(client);
    return client
        .customResources(
            crContext,
            ConsistentRegion.class,
            ConsistentRegionList.class,
            DoneableConsistentRegion.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<
          ConsistentRegionOperator,
          ConsistentRegionOperatorList,
          DoneableConsistentRegionOperator,
          Resource<ConsistentRegionOperator, DoneableConsistentRegionOperator>>
      getCROClient() {
    assertNotNull(client);
    return client
        .customResources(
            croContext,
            ConsistentRegionOperator.class,
            ConsistentRegionOperatorList.class,
            DoneableConsistentRegionOperator.class)
        .inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<Pod, PodList, DoneablePod, PodResource<Pod, DoneablePod>>
      getPodClient() {
    assertNotNull(client);
    return client.pods().inNamespace(KUBE_NAMESPACE);
  }

  private NonNamespaceOperation<
          ConfigMap, ConfigMapList, DoneableConfigMap, Resource<ConfigMap, DoneableConfigMap>>
      getConfigMapClient() {
    assertNotNull(client);
    return client.configMaps().inNamespace(KUBE_NAMESPACE);
  }

  public NonNamespaceOperation<
          Service, ServiceList, DoneableService, ServiceResource<Service, DoneableService>>
      getServiceClient() {
    assertNotNull(client);
    return client.services().inNamespace(KUBE_NAMESPACE);
  }

  public Deployment getDeployment(String name) {
    assertNotNull(client);
    return client.apps().deployments().inNamespace(KUBE_NAMESPACE).withName(name).get();
  }

  public void assertParallelRegionWidth(String jobName, String alias, int width) {
    Probe.watchUntil(
        "ParallelRegionWidth",
        60,
        () ->
            getParallelRegionStore()
                .getParallelRegionWithAlias(jobName, alias)
                .map(p -> p.getSpec().getWidth().intValueExact())
                .orElse(-1),
        width);
  }

  public void assertParallelRegionSize(int size) {
    Probe.watchUntil(
        "ParallelRegionSize", 60, () -> getParallelRegionClient().list().getItems().size(), size);
    Probe.watchUntil("ParallelRegionSize", 60, () -> getParallelRegionStore().size(), size);
  }

  public void assertJobSubmitted(String name, int delayInSec) {
    Probe.watchUntil(
        "JobSubmitted",
        delayInSec,
        () ->
            getJobStore()
                .getJobWithName(name)
                .map(e -> e.getSpec().getStatus())
                .orElse(EJobSubmissionStatus.Unknown),
        EJobSubmissionStatus.Submitted);
  }

  public void assertJobSubmitted(String name, BigInteger generationId, int delayInSec) {
    Probe.watchUntil(
        "JobSubmitted",
        delayInSec,
        () ->
            getJobStore()
                .getJobWithNameAndGenerationId(name, generationId)
                .map(e -> e.getSpec().getStatus())
                .orElse(EJobSubmissionStatus.Unknown),
        EJobSubmissionStatus.Submitted);
  }

  public void assertJobSize(int size) {
    Probe.watchUntil("JobSize", 60, () -> getJobClient().list().getItems().size(), size);
  }

  public void assertPeLaunchCount(String name, int launchCount, int delayInSec) {
    Probe.watchUntil(
        "PeLaunchCount",
        delayInSec,
        () ->
            getPeClient().list().getItems().stream()
                .filter(e -> e.getMetadata().getName().equals(name))
                .findFirst()
                .map(e -> e.getSpec().getLaunchCount())
                .orElse(-1),
        launchCount);
  }

  public void assertPeSize(int size) {
    Probe.watchUntil("PeSize", 60, () -> getPeClient().list().getItems().size(), size);
  }

  public void assertPodSize(int size) {
    Probe.watchUntil("PodSize", 60, () -> getPodClient().list().getItems().size(), size);
  }

  public void assertConfigMapSize(int size) {
    Probe.watchUntil(
        "ConfigMapSize", 60, () -> getConfigMapClient().list().getItems().size(), size);
  }

  public void assertServiceSize(int size) {
    Probe.watchUntil("ServiceSize", 60, () -> getServiceClient().list().getItems().size(), size);
  }

  public void assertImportSize(int size) {
    Probe.watchUntil("ImportSize", 60, () -> getImportClient().list().getItems().size(), size);
  }

  public void assertExportSize(int size) {
    Probe.watchUntil("ExportSize", 60, () -> getExportClient().list().getItems().size(), size);
  }

  public IParallelRegionCoordinator getParallelRegionCoordinator() {
    assertNotNull(this.instance.getParallelRegionCoordinator());
    return this.instance.getParallelRegionCoordinator();
  }

  public SubscriptionBoard getSubscriptionBoard() {
    return this.instance.getSubscriptionBoard();
  }

  public HostPoolStore getHostPoolStore() {
    return this.instance.getHostPoolStore();
  }

  public CaptureConverter getCaptureConverter() {
    return this.instance.getCaptureConverter();
  }

  public ParallelRegionStore getParallelRegionStore() {
    assertNotNull(this.instance.getParallelRegionStore());
    return this.instance.getParallelRegionStore();
  }

  public JobStore getJobStore() {
    assertNotNull(this.instance.getJobStore());
    return this.instance.getJobStore();
  }

  public ProcessingElementStore getProcessingElementStore() {
    assertNotNull(this.instance.getProcessingElementStore());
    return this.instance.getProcessingElementStore();
  }

  public PodStore getPodStore() {
    assertNotNull(this.instance.getPodStore());
    return this.instance.getPodStore();
  }
}
