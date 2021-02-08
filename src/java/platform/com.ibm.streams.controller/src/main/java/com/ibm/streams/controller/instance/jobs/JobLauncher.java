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

package com.ibm.streams.controller.instance.jobs;

import static com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus.Submitting;

import com.ibm.streams.control.internal.server.consistent.ConsistentRegion;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorFactory;
import com.ibm.streams.controller.crds.crs.ConsistentRegionFactory;
import com.ibm.streams.controller.crds.exports.ExportFactory;
import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.exports.IExportCoordinator;
import com.ibm.streams.controller.crds.hostpools.HostPoolFactory;
import com.ibm.streams.controller.crds.imports.IImportCoordinator;
import com.ibm.streams.controller.crds.imports.ImportFactory;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobFactory;
import com.ibm.streams.controller.crds.jobs.JobStatisticsSpec;
import com.ibm.streams.controller.crds.pes.PlacementSpec;
import com.ibm.streams.controller.crds.pes.ProcessingElementFactory;
import com.ibm.streams.controller.crds.pes.ProcessingElementSpec;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import com.ibm.streams.controller.crds.prs.IParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegionFactory;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.executor.EExecutionCommand;
import com.ibm.streams.controller.executor.IExecutor;
import com.ibm.streams.controller.instance.broker.ImportExportCommons;
import com.ibm.streams.controller.instance.sam.InputPortLabel;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapFactory;
import com.ibm.streams.controller.k8s.services.IServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceFactory;
import com.ibm.streams.controller.k8s.services.ServiceStore;
import com.ibm.streams.controller.k8s.utils.ResourceRequirementsUtils;
import com.ibm.streams.controller.security.Certificate;
import com.ibm.streams.controller.utils.CommonEnvironment;
import com.ibm.streams.instance.sam.model.InputPort;
import com.ibm.streams.instance.sam.model.OutputPort;
import io.fabric8.kubernetes.api.model.ResourceRequirements;
import java.io.StringWriter;
import java.math.BigInteger;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.cert.X509Certificate;
import java.util.AbstractMap;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.function.Function;
import java.util.stream.Collectors;
import lombok.var;
import org.apache.commons.codec.digest.DigestUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class JobLauncher {

  private static final Logger LOGGER = LoggerFactory.getLogger(JobLauncher.class);

  private final ConfigMapFactory cmFactory;
  private final ExportFactory expFactory;
  private final ExportStore expStore;
  private final IExportCoordinator expCoordinator;
  private final ImportFactory impFactory;
  private final ImportStore impStore;
  private final IImportCoordinator impCoordinator;
  private final HostPoolFactory hpFactory;
  private final ParallelRegionFactory prFactory;
  private final ParallelRegionStore prStore;
  private final IParallelRegionCoordinator prCoordinator;
  private final ProcessingElementFactory peFactory;
  private final ProcessingElementStore peStore;
  private final IProcessingElementCoordinator peCoordinator;
  private final ConsistentRegionFactory crFactory;
  private final ConsistentRegionOperatorFactory consistentRegionOperatorFactory;
  private final JobFactory jobFactory;
  private final ServiceFactory svcFactory;
  private final ServiceStore svcStore;
  private final IServiceCoordinator svcCoordinator;
  private final JobBridge jobBridge;
  private final IExecutor executor;
  private final Map<String, JobModelContext> transitionalContexts;
  private final CommonEnvironment env;

  private KeyPairGenerator keygen;

  public JobLauncher(
      JobFactory jobFactory,
      ExportFactory expFactory,
      ExportStore expStore,
      IExportCoordinator expCoordinator,
      ImportFactory impFactory,
      ImportStore impStore,
      IImportCoordinator impCoordinator,
      HostPoolFactory hpFactory,
      ParallelRegionFactory prFactory,
      ParallelRegionStore prStore,
      IParallelRegionCoordinator prCoordinator,
      ProcessingElementFactory peFactory,
      ProcessingElementStore peStore,
      IProcessingElementCoordinator peCoordinator,
      ConsistentRegionFactory crFactory,
      ConsistentRegionOperatorFactory consistentRegionOperatorFactory,
      ConfigMapFactory cmFactory,
      ServiceFactory svcFactory,
      ServiceStore svcStore,
      IServiceCoordinator svcCoordinator,
      IExecutor executor,
      CommonEnvironment env) {
    this.cmFactory = cmFactory;
    this.expFactory = expFactory;
    this.expStore = expStore;
    this.expCoordinator = expCoordinator;
    this.impFactory = impFactory;
    this.impStore = impStore;
    this.impCoordinator = impCoordinator;
    this.hpFactory = hpFactory;
    this.prFactory = prFactory;
    this.prStore = prStore;
    this.prCoordinator = prCoordinator;
    this.peFactory = peFactory;
    this.peStore = peStore;
    this.peCoordinator = peCoordinator;
    this.crFactory = crFactory;
    this.consistentRegionOperatorFactory = consistentRegionOperatorFactory;
    this.jobFactory = jobFactory;
    this.svcFactory = svcFactory;
    this.svcStore = svcStore;
    this.svcCoordinator = svcCoordinator;
    this.executor = executor;
    this.transitionalContexts = new HashMap<>();
    /*
     * Grab settings from our environment. Our environment object knows if we are in test or not.
     */
    this.env = env;
    /*
     * The JobBridge knows about both the Kube, CRD kind of "Job" and the SAM, Pipeline model
     * kind of "Job." On this side of the bridge, we only know about the CRD kind of "Job."
     */
    this.jobBridge = new JobBridge(env);
    /*
     * Create a RSA keypair generator.
     */
    try {
      this.keygen = KeyPairGenerator.getInstance("RSA");
      this.keygen.initialize(2048);
    } catch (NoSuchAlgorithmException e) {
      LOGGER.error(e.getMessage());
    }
  }

  private void addConfigMaps(Job job, JobModelContext context) {
    /*
     * Build the properties object.
     */
    var portLabels = context.getPortLabels();
    Properties props = new Properties();
    portLabels.forEach(
        (k, v) -> v.forEach(l -> props.setProperty(l.getOperatorPortLabel(), l.getPePortLabel())));
    /*
     * Try to create the config map.
     */
    try {
      StringWriter writer = new StringWriter();
      props.store(writer, "Operator port labels");
      /*
       * Build the config map.
       */
      HashMap<String, String> data = new HashMap<>();
      /*
       * Generate a certificate if there is any transport security.
       */
      if (job.getSpec().getProcessingElement().hasTransportSecurity()) {
        String dn = "CN=Streams, L=NewYork, C=US";
        KeyPair keys = keygen.generateKeyPair();
        X509Certificate cert =
            Certificate.generateCertificate(
                dn, keys, job.getSpec().getTransportCertificateValidityInDays());
        data.put("transport.cert", Certificate.certificateToPemString(cert));
        data.put("transport.key", Certificate.privateKeyToPemString(keys));
      }
      /*
       * Populate the config map.
       */
      data.put("operator_port_labels.properties", writer.toString());
      /*
       * Build owner reference.
       */
      cmFactory.addConfigMap(job, data);
    } catch (Exception e) {
      LOGGER.error(e.getMessage());
    }
    /*
     * Add per-pe config maps.
     */
    context
        .getAadls()
        .forEach(
            (id, aadl) -> {
              var hash = DigestUtils.sha256Hex(aadl);
              var name = JobUtils.getProcessingElementName(job, id);
              cmFactory.addConfigMap(job, name, id, aadl, hash);
            });
  }

  private void addServices(Job job, JobModelContext context) {
    context.getPortLabels().forEach((k, v) -> addService(job, k, v));
  }

  private void addService(Job job, BigInteger peId, List<InputPortLabel> ports) {
    var old = svcStore.getServiceWithJobPeIdAndPreviousGenerationId(job, peId);
    if (old.isPresent()) {
      this.svcCoordinator.updateService(job, old.get(), ports);
    } else {
      this.svcFactory.addService(job, peId, ports);
    }
  }

  private void addExports(Job job, JobModelContext context) {
    context.getExports().forEach((k, v) -> addExports(job, k, v));
  }

  private void addExports(Job job, BigInteger peId, List<OutputPort> exports) {
    /*
     * First, build a list of the new export names and get the set of existing export names.
     */
    var newExports =
        exports.stream()
            .collect(
                Collectors.toMap(
                    op -> ImportExportCommons.getExportName(job, peId, op), Function.identity()));
    var oldExports =
        expStore.getExportsWithJobAndPreviousGenerationId(job).stream()
            .collect(Collectors.toMap(exp -> exp.getMetadata().getName(), Function.identity()));
    /*
     * Then, touch the existing exports.
     */
    oldExports.entrySet().stream()
        .filter(e -> newExports.containsKey(e.getKey()))
        .forEach(e -> this.expCoordinator.updateExport(job, e.getValue()));
    /*
     * Finally, create new exports.
     */
    newExports.entrySet().stream()
        .filter(e -> !oldExports.containsKey(e.getKey()))
        .forEach(e -> this.expFactory.addExport(job, peId, e.getValue()));
  }

  private void addImports(Job job, JobModelContext context) {
    context.getImports().forEach((k, v) -> addImports(job, k, v));
  }

  private void addImports(Job job, BigInteger peId, List<InputPort> imports) {
    /*
     * First, build a list of the new export names and get the set of existing export names.
     */
    var newImports =
        imports.stream()
            .collect(
                Collectors.toMap(
                    ip -> ImportExportCommons.getImportName(job, peId, ip), Function.identity()));
    var oldImports =
        impStore.getImportsWithJobAndPreviousGenerationId(job).stream()
            .collect(Collectors.toMap(imp -> imp.getMetadata().getName(), Function.identity()));
    /*
     * Then, touch the existing exports.
     */
    oldImports.entrySet().stream()
        .filter(e -> newImports.containsKey(e.getKey()))
        .forEach(e -> this.impCoordinator.updateImport(job, e.getValue()));
    /*
     * Finally, create new exports.
     */
    newImports.entrySet().stream()
        .filter(e -> !oldImports.containsKey(e.getKey()))
        .forEach(e -> this.impFactory.addImport(job, peId, e.getValue()));
  }

  private void addHostPools(Job job, JobModelContext context) {
    context.getHostPools().forEach((k, v) -> this.hpFactory.addHostPool(job, k, v));
  }

  private void addParallelRegions(Job job, JobModelContext context) {
    context
        .getParallelRegions()
        .values()
        .forEach(
            v -> addParallelRegion(job, v, context.getParallelRegionWidths().get(v.getName())));
  }

  private void addParallelRegion(
      Job job, com.ibm.streams.instance.sam.model.topology.ParallelRegion pr, BigInteger width) {
    var old = this.prStore.getParallelRegionWithJobAndAlias(job, pr.getName());
    if (old.isPresent()) {
      this.prCoordinator.updateParallelRegion(job, old.get());
    } else {
      this.prFactory.addParallelRegion(job, pr, width);
    }
  }

  private void addPes(Job job, JobModelContext context) {
    /*
     * We want to sort our PEs in the order of most-restrictive placement policies first
     * before trying to create pods for them. If we don't, and create pods for them in
     * an arbitrary order, Kube may place pods with less restrictions evenly distributed
     * among the nodes, leaving no viable placement for pods with more restrictions.
     *
     * The most restrictive constraint is host isolation, so we sort PEs with host
     * isolations first. The next most restrictive constraint is the number of
     * exlocations.
     */
    var crRegions = getPeToRegionMap(context.getConsistentRegions());
    var mostRestrictiveFirst =
        Comparator.comparing(
                PlacementSpec::getIsolation, Comparator.nullsLast(Comparator.naturalOrder()))
            .thenComparingLong(p -> p.getExLocations().size());
    /*
     * For PEs that are not already running, we will create a new spec for them based off
     * of the PE spec given in the job spec. We do this operation first as it is asynchronous.
     */
    var peMap = peStore.getPreviousPesWithJob(job);
    context.getAadls().entrySet().stream()
        .filter(e -> !peMap.containsKey(e.getKey()))
        .map(
            e ->
                createProcessingElementSpec(
                    job, context, e.getKey(), e.getValue(), crRegions.get(e.getKey())))
        .sorted((e1, e2) -> mostRestrictiveFirst.compare(e1.getPlacement(), e2.getPlacement()))
        .forEach(e -> addPe(job, e));
    /*
     * If any of the PEs are already running and they have the same content ID, update their generation ID.
     * We use the processing element coordinator to do that synchronously.
     */
    context.getAadls().entrySet().stream()
        .filter(e -> peMap.containsKey(e.getKey()))
        .map(
            e ->
                new AbstractMap.SimpleEntry<>(
                    peMap.get(e.getKey()), DigestUtils.sha256Hex(e.getValue())))
        .filter(e -> e.getKey().getSpec().getContentId().equals(e.getValue()))
        .forEach(e -> peCoordinator.touchPe(job, e.getKey()));
    /*
     * If any of the PEs are already running and they have a differing content ID, update their specs.
     * We want to preserve their various states - in particular, launch count and connectivity. We use
     * the processing element coordinator to do that synchronously.
     */
    context.getAadls().entrySet().stream()
        .filter(e -> peMap.containsKey(e.getKey()))
        .map(
            e ->
                new AbstractMap.SimpleEntry<>(
                    peMap.get(e.getKey()), DigestUtils.sha256Hex(e.getValue())))
        .filter(e -> !e.getKey().getSpec().getContentId().equals(e.getValue()))
        .forEach(
            e ->
                peCoordinator.updatePeContentIdAndIncrementLaunchCount(
                    job, e.getKey(), e.getValue()));
  }

  /*
   * Job limits are divided among the PEs. But each PE gets the same requests. The rationale is that
   * the limits are upper limits for the job, but that requests are bare-minimum needed to run the
   * job, and each PE will need that much.
   */
  private ResourceRequirements allocatePeResources(ResourceRequirements resources, long totalPes) {
    var limits = ResourceRequirementsUtils.divideQuantities(resources.getLimits(), totalPes);
    var requests = new HashMap<>(resources.getRequests());
    return new ResourceRequirements(limits, requests);
  }

  private ProcessingElementSpec createProcessingElementSpec(
      Job job, JobModelContext context, BigInteger peId, String aadl, Set<Integer> crRegions) {
    /*
     * Build the PE spec.
     */
    ProcessingElementSpec spec = new ProcessingElementSpec(job.getSpec().getProcessingElement());
    spec.setId(peId);
    spec.setContentId(DigestUtils.sha256Hex(aadl));
    spec.setBundleName(job.getSpec().getBundle().getName());
    spec.setRestartFailedPod(
        spec.restartFailedPod().orElse(context.getPesIsRestartable().get(peId)));
    spec.setOperators(context.getPesOperatorNames().get(peId));
    /*
     * If there are resources at the job level, then we divide them among the PEs regardless of
     * anything specified anywhere else or any defaults. If we have no job resources, then we
     * use the PE resources. (Note that if the user did not not specify any PE resources in
     * the job spec, this will just re-apply the defaults.)
     */
    var jobResources = job.getSpec().getResources();
    var jobPeResources = job.getSpec().getProcessingElement().getResources();
    var totalPes = job.getSpec().getStatistics().getProcessingElementCount();
    if (jobResources != null) {
      spec.setResources(allocatePeResources(jobResources, totalPes));
    } else {
      spec.setResources(jobPeResources);
    }
    /*
     * Set host constraints.
     */
    spec.getPlacement().setHost(context.getPesHostPlacement().getOrDefault(peId, null));
    spec.getPlacement().setCoLocations(context.getPesHostCoLocations().get(peId));
    spec.getPlacement().setExLocations(context.getPesHostExLocations().get(peId));
    spec.getPlacement().setIsolation(context.getPesHostIsolation().getOrDefault(peId, null));
    spec.getPlacement().setIsolationLabels(context.getPesHostIsolationLabels().get(peId));
    spec.getPlacement()
        .setHostPoolLocation(context.getPesHostPoolLocation().getOrDefault(peId, null));
    /*
     * Set image parameters if empty.
     */
    if (spec.getImagePullPolicy() == null) {
      spec.setImagePullPolicy(job.getSpec().getImagePullPolicy());
    }
    if (spec.getImagePullSecret() == null) {
      spec.setImagePullSecret(job.getSpec().getImagePullSecret());
    }
    /*
     * Add the consistent regions if there are any.
     */
    if (crRegions != null) {
      spec.setRegions(crRegions);
    }
    return spec;
  }

  private void addPe(Job job, ProcessingElementSpec spec) {
    /*
     * Build the pod name.
     */
    String name = JobUtils.getProcessingElementName(job, spec.getId());
    /*
     * Build the pod.
     */
    peFactory.addPe(job, name, spec);
  }

  private void addConsistentRegion(Job job, JobModelContext context) {
    /*
     * Create the consistent region operator
     */
    var crMap = context.getConsistentRegions();
    if (crMap.size() > 0) {
      consistentRegionOperatorFactory.addConsistentRegionOperator(job, crMap.size(), env);
      crMap.forEach((k, v) -> crFactory.addConsistentRegion(job, k, v));
    }
  }

  private Map<BigInteger, Set<Integer>> getPeToRegionMap(Map<Integer, ConsistentRegion> crMap) {
    Map<BigInteger, Set<Integer>> peToRegionsMap = new HashMap<>();
    if (crMap.size() > 0) {
      crMap.forEach(
          (k, v) ->
              v.getPesInRegion()
                  .forEach(
                      p -> {
                        if (peToRegionsMap.containsKey(p)) {
                          peToRegionsMap.get(p).add(k);
                        } else {
                          Set<Integer> regions = new HashSet<>();
                          regions.add(k);
                          peToRegionsMap.put(p, regions);
                        }
                      }));
    }
    return peToRegionsMap;
  }

  /*
   * If the update method is not provided with an explicit job status, we use what is currently
   * present in the job spec. For newly constructed JobSpecs, the status is `Submitting`.
   */
  private void update(Job job, BigInteger jobId, BigInteger generationId, JobModelContext context)
      throws Exception {
    update(job, jobId, generationId, job.getSpec().getStatus(), context);
  }

  private void update(
      Job job,
      BigInteger jobId,
      BigInteger generationId,
      EJobSubmissionStatus jobStatus,
      JobModelContext context)
      throws Exception {
    /*
     * Create the transitional context.
     */
    transitionalContexts.put(job.getMetadata().getName(), context);
    /*
     * Initialize the job's statistics.
     */
    var jobStat = new JobStatisticsSpec();
    jobStat.setConfigMapCount(context.getAadls().size());
    jobStat.setConsistentRegionCount(context.getConsistentRegions().size());
    jobStat.setExportCount(context.getExports().values().stream().mapToInt(Collection::size).sum());
    jobStat.setHostPoolCount(context.getHostPools().size());
    jobStat.setImportCount(context.getImports().values().stream().mapToInt(Collection::size).sum());
    jobStat.setParallelRegionCount(context.getParallelRegions().size());
    jobStat.setProcessingElementCount(context.getAadls().size());
    jobStat.setServiceCount(
        context.getAadls().size() + (context.getConsistentRegions().size() > 0 ? 1 : 0));
    /*
     * Update the job.
     */
    executor.execute(
        EExecutionCommand.UPDATE_JOB,
        () ->
            jobFactory.updateJob(
                job,
                jobId,
                generationId,
                jobStatus,
                jobStat,
                context.getApplicationName(),
                context.getApplicationScope()));
  }

  public void addResources(Job job) {
    /*
     * Grab the context.
     */
    var ctx = transitionalContexts.get(job.getMetadata().getName());
    /*
     * Add the kubernetes resources.
     */
    executor.execute(EExecutionCommand.ADD_CONFIGMAPS, () -> addConfigMaps(job, ctx));
    executor.execute(EExecutionCommand.ADD_SERVICES, () -> addServices(job, ctx));
    executor.execute(EExecutionCommand.ADD_HOST_POOLS, () -> addHostPools(job, ctx));
    executor.execute(EExecutionCommand.ADD_EXPORTS, () -> addExports(job, ctx));
    executor.execute(EExecutionCommand.ADD_IMPORTS, () -> addImports(job, ctx));
    executor.execute(EExecutionCommand.ADD_CONSISTENT_REGIONS, () -> addConsistentRegion(job, ctx));
    executor.execute(EExecutionCommand.ADD_PARALLEL_REGIONS, () -> addParallelRegions(job, ctx));
    /*
     * Note that the order in which we add and delete PEs matters. Adding PEs
     * involves looking up the specs for the currently running (old, previous)
     * PEs. They must still exist for us to do so, so we must add the new PEs
     * first, then delete the old ones.
     */
    executor.execute(EExecutionCommand.ADD_PES, () -> addPes(job, ctx));
  }

  public void deleteResources(Job job) {
    executor.execute(EExecutionCommand.DELETE_PES, () -> peFactory.deletePes(job));
    executor.execute(
        EExecutionCommand.DELETE_PARALLEL_REGIONS, () -> prFactory.deleteParallelRegions(job));
    executor.execute(
        EExecutionCommand.DELETE_CONSISTEMT_REGIONS, () -> crFactory.deleteConsistentRegions(job));
    executor.execute(EExecutionCommand.DELETE_IMPORTS, () -> impFactory.deleteImports(job));
    executor.execute(EExecutionCommand.DELETE_EXPORTS, () -> expFactory.deleteExports(job));
    executor.execute(EExecutionCommand.DELETE_HOST_POOLS, () -> hpFactory.deleteHostPools(job));
    executor.execute(EExecutionCommand.DELETE_SERVICES, () -> svcFactory.deleteServices(job));
    executor.execute(EExecutionCommand.DELETE_CONFIGMAPS, () -> cmFactory.deleteConfigMaps(job));
  }

  public void removeJobFromTransitionalContext(String jobName) {
    transitionalContexts.remove(jobName);
  }

  public void launch(Job job, BigInteger jobId) throws Exception {
    /*
     * Create the job models (logical, topological and final fusion).
     */
    var jobName = job.getMetadata().getName();
    var jobNamespace = job.getMetadata().getNamespace();
    var jobSpec = job.getSpec();
    var context = jobBridge.create(jobName, jobNamespace, jobSpec, jobId);
    /*
     * Since this is the first launch of this job, its generation ID is 0.
     */
    update(job, jobId, BigInteger.valueOf(0), context);
  }

  /*
   * TODO: reject a parallel width change if the parallel region contains
   * a consistent region.
   */
  public void updateParallelWidth(
      Job job,
      String regionName,
      Map<BigInteger, Set<String>> peIdToOperators,
      Map<String, BigInteger> priorWidths,
      Map<String, BigInteger> currWidths)
      throws Exception {
    var jobName = job.getMetadata().getName();
    var jobNamespace = job.getMetadata().getNamespace();

    var context =
        jobBridge.updateParallelWidth(
            jobName,
            jobNamespace,
            job.getSpec(),
            regionName,
            peIdToOperators,
            priorWidths,
            currWidths);
    /*
     * We use the same job ID, but we insist that the job state is `Submitting` - if the job
     * had been fully submitted, this ensures that we add the resources again. We also
     * increment the generation ID.
     */
    var jobId = job.getSpec().getId();
    var generationId = job.getSpec().getGenerationId().add(BigInteger.ONE);
    update(job, jobId, generationId, Submitting, context);
  }

  public void delete(Job job) {
    transitionalContexts.remove(job.getMetadata().getName());
    jobFactory.deleteJob(job);
  }

  public void close() {
    jobBridge.close();
  }
}
