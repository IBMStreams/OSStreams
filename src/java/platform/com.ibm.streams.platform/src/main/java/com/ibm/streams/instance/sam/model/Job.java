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

package com.ibm.streams.instance.sam.model;

import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;
import com.google.common.collect.ImmutableBiMap;
import com.google.common.collect.ImmutableSortedMap;
import com.google.common.collect.ImmutableSortedSet;
import com.ibm.distillery.utils.DistilleryExceptionCode;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.config.PropertyDefinition.FusionScheme;
import com.ibm.streams.admin.internal.api.config.PropertyDefinition.JobResourceSharing;
import com.ibm.streams.admin.internal.api.config.PropertyDefinition.ThreadingModel;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.ObjectTracker;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.fuser.FusedApplication;
import com.ibm.streams.instance.sam.model.fuser.FusedContainer;
import com.ibm.streams.instance.sam.model.logical.spl.SubmissionTimeValue;
import com.ibm.streams.instance.sam.model.runtime.JobHealth;
import com.ibm.streams.instance.sam.model.runtime.RuntimeCustomMetric;
import com.ibm.streams.instance.sam.model.runtime.RuntimeJob;
import com.ibm.streams.instance.sam.model.runtime.RuntimeJobComposites;
import com.ibm.streams.instance.sam.model.runtime.RuntimeJobConfig;
import com.ibm.streams.instance.sam.model.runtime.RuntimeStream;
import com.ibm.streams.instance.sam.model.runtime.StreamId;
import com.ibm.streams.instance.sam.model.topology.CustomMetric;
import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.Library;
import com.ibm.streams.instance.sam.model.topology.RuntimeConstant;
import com.ibm.streams.instance.sam.model.topology.ToolkitMap;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleTypeType;
import com.ibm.streams.instance.sam.model.topology.YieldBehaviour;
import com.ibm.streams.sam.exc.InvalidJobNameException;
import java.math.BigInteger;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.Vector;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class Job extends SamObject {

  private FusedApplication fused;
  private RuntimeJob run;
  private BiMap<BigInteger, BigInteger>
      peIdToIndex; // can hold temporarily pes while fusion is progress
  private Map<BigInteger, BigInteger> fusedContainerIndexToPeId;
  private SortedMap<BigInteger, Pe> pes; // pe ID -> pe
  private boolean isConstructed;
  private RuntimeJobConfig config;
  private RuntimeJobComposites composites;
  private ReentrantReadWriteLock intraProcessLock;

  public Job(
      FusedApplication fused,
      String user,
      String jobName,
      String jobGroup,
      RuntimeJobConfig config,
      RuntimeJobComposites composites,
      boolean isPreviewOnly)
      throws InvalidJobNameException, StreamsException {
    this(
        fused,
        user,
        jobName,
        jobGroup,
        new HashMap<BigInteger, Set<String>>(),
        config,
        composites,
        isPreviewOnly);
  }

  /**
   * Constructor - New Job Submission
   *
   * @param fused - FusedApplication
   * @param user - String
   * @param jobName - String
   * @param jobGroup - String
   * @param sjp - SubmitJobParameters
   * @param isPreviewOnly - boolean
   * @throws InvalidJobNameException
   * @throws StreamsException
   */
  public Job(
      FusedApplication fused,
      String user,
      String jobName,
      String jobGroup,
      Map<BigInteger, Set<String>> peIdToOperators,
      RuntimeJobConfig config,
      RuntimeJobComposites composites,
      boolean isPreviewOnly)
      throws InvalidJobNameException, StreamsException {
    super(isPreviewOnly);
    RuntimeJob runtimeJob = createRuntimeJob(fused, user, jobName, jobGroup);
    try {
      initialize(fused, runtimeJob, config, composites);
      buildRuntimeOperators(isPreviewOnly());
      SortedSet<FusedContainer> fusedContainers =
          SAMStaticContext.getFusedTracker().getFusedContainers(fused.getApplicationId());

      // we don't do anything with the result anymore, as we have to set the pes field inside the
      // buildPes method
      buildPes(fusedContainers, peIdToOperators, true, false, isPreviewOnly());
      addPes(this.pes);
    } catch (Exception e) {
      Trace.logError(e.getMessage(), e);
      unregister(false);
      throw e;
    }
  }

  /**
   * Constructor - Recovery of existing Job
   *
   * @param fused - FusedApplication
   * @param runtime - The RuntimeJob object representing all runtime state from before the HA event
   * @param sjp - The SubmitJobParameters, these are recovered because they may continue to apply
   *     during fusion/fission events later
   * @throws StreamsException
   */
  public Job(
      FusedApplication fused,
      RuntimeJob runtimeJob,
      RuntimeJobConfig config,
      RuntimeJobComposites composites)
      throws StreamsException {
    super(false);
    throw new RuntimeException("called Job recovery constructor");
  }

  private void buildRuntimeOperators(boolean previewOnly) {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "ENTER Job.buildRuntimeOperators with "
              + Arrays.deepToString(new Object[] {previewOnly}));
    }
    for (BigInteger nodeIndex : getNodeIndexes()) {
      new Operator(getTopologyApplicationId(), nodeIndex, previewOnly);
    }
  }

  public SortedMap<BigInteger, Pe> buildPes(
      SortedSet<FusedContainer> fusedContainers,
      boolean buildingJob,
      boolean pesStopped,
      boolean previewOnly) {
    return buildPes(
        fusedContainers,
        new HashMap<BigInteger, Set<String>>(),
        buildingJob,
        pesStopped,
        previewOnly);
  }

  /**
   * Builds PEs from the FusedContainers provided. PEs will be a 1-to-1 mapping with
   * FusedContainers. This works for both partial and full PE set construction. The set returned
   * will contain all the newly created PE IDs in order. They will also be registered in the
   * ObjectManager
   *
   * @param fusedContainers - The containers that will be synthesized into PEs, the IDs will match
   *     the existing order
   * @param buildingJob - Indicates if we are conducting fusion/fission or submitting a Job
   * @param pesStopped - Used to set the PE state correctly based on recovery, fusion/fission, or
   *     job submission
   * @return The set of PE IDs representing the newly registered PEs, sorted in numerical order
   */
  public SortedMap<BigInteger, Pe> buildPes(
      SortedSet<FusedContainer> fusedContainers,
      Map<BigInteger, Set<String>> peIdToOperators,
      boolean buildingJob,
      boolean pesStopped,
      boolean previewOnly) {

    this.fusedContainerIndexToPeId =
        JobUtils.createFusedContainerIndexToPeId(fusedContainers, peIdToOperators);

    // we have to set this here because buildPeConnections will rely on aksing the job for PEs
    this.pes =
        JobUtils.createPes(
            getId(),
            fusedContainers,
            fusedContainerIndexToPeId,
            getGenId(),
            buildingJob,
            pesStopped,
            previewOnly);

    // Build the ID <-> Index Mappings
    ImmutableBiMap.Builder<BigInteger, BigInteger> builder = ImmutableBiMap.builder();
    for (Pe pe : this.pes.values()) {
      builder.put(pe.getId(), pe.getIndex());
    }

    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("buildPes peIdToIndexMap1: " + getPeIdToIndexMap());
    }

    ImmutableBiMap<BigInteger, BigInteger> peIdToIndexMap = builder.build();
    peIdToIndex.putAll(peIdToIndexMap);

    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("buildPes peIdToIndexMap2: " + getPeIdToIndexMap());
    }

    // build pe connections after peIdToIndex is populated
    JobUtils.buildPeConnections(this.pes.values(), buildingJob);

    return this.pes;
  }

  public void addPes(SortedMap<BigInteger, Pe> pes) {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("ENTER Job.addPes with " + Arrays.deepToString(new Object[] {pes}));
    }
    if (!pes.isEmpty()) {
      // add peIds to job
      run.addPeIds(pes.keySet());
      for (Map.Entry<BigInteger, Pe> entry : pes.entrySet()) {
        BigInteger peId = entry.getKey();
        Pe pe = entry.getValue();
        // update peIdToIndex map
        peIdToIndex.put(peId, pe.getIndex());
        // adds fused container index to fused application in memory
        fused.addContainerIndex(pe.getFusedContainer());
      }
    }
  }

  public void removePes(Set<BigInteger> peIds) {
    throw new RuntimeException("called Job.removePes(peIds)");
  }

  public void persistPes(Set<BigInteger> newPeIds) throws StreamsException {
    throw new RuntimeException("called Job.persistPes(peIds)");
  }

  public boolean isConstructed() {
    return isConstructed;
  }

  private void initialize(
      FusedApplication fused,
      RuntimeJob run,
      RuntimeJobConfig config,
      RuntimeJobComposites composites)
      throws StreamsException {
    peIdToIndex = HashBiMap.<BigInteger, BigInteger>create();
    this.composites = composites;
    this.fused = fused;
    this.run = run;
    this.config = config;
    SAMStaticContext.getObjectTracker().registerJob(this);
  }

  private static RuntimeJob createRuntimeJob(
      FusedApplication fused, String user, String jobName, String jobGroup)
      throws InvalidJobNameException {
    // topology application id = fused application id = job id
    RuntimeJob runtimeJob =
        new RuntimeJob(
            fused.getApplicationId(), user, fused.getTopologyApplication().getName(), jobGroup);
    setJobName(runtimeJob, jobName);
    runtimeJob.setCustomMetrics(fused.getTopologyApplication().getCustomMetrics());
    if (Trace.isEnabled(Level.INFO)) {
      Trace.logInfo("Done job name verification and setup with jobname: " + runtimeJob.getName());
    }
    return runtimeJob;
  }

  public RuntimeJob getRuntime() {
    return run;
  }

  public void setRuntime(RuntimeJob run) {
    this.run = run;
  }

  public BigInteger getId() {
    return run.getId();
  }

  public BigInteger getPeIdByIndex(BigInteger peIndex) {
    return peIdToIndex.inverse().get(peIndex);
  }

  public BigInteger getPeIdByFusedContainerIndex(BigInteger fusedIndex) {
    return fusedContainerIndexToPeId.get(fusedIndex);
  }

  public Map<BigInteger, BigInteger> getPeIdToIndexMap() {
    return peIdToIndex;
  }

  public String getDataDirectory() {
    return fused.getTopologyApplication().getDataDirectory();
  }

  public String getAdlPath() {
    return fused.getTopologyApplication().getAdlPath();
  }

  public BigInteger getSubmitTime() {
    return run.getSubmitTime();
  }

  public String getApplicationDirectory() {
    return fused.getTopologyApplication().getApplicationDirectory();
  }

  public String getOutputDirectory() {
    return fused.getTopologyApplication().getOutputDirectory();
  }

  public String getCheckpointDirectory() {
    return fused.getTopologyApplication().getCheckpointDirectory();
  }

  public String getName() {
    return run.getName();
  }

  public String getUser() {
    return run.getUser();
  }

  public String getApplicationScope() {
    return fused.getTopologyApplication().getApplicationScope();
  }

  public String getApplicationName() {
    return fused.getTopologyApplication().getName();
  }

  public String getApplicationVersion() {
    return fused.getTopologyApplication().getVersion();
  }

  public String getApplicationProductVersion() {
    return fused.getTopologyApplication().getProductVersion();
  }

  public String getBundleVersion() {
    return fused.getTopologyApplication().getBundleVersion();
  }

  public YieldBehaviour getYieldBehaviour() {
    return fused.getTopologyApplication().getYieldBehaviour();
  }

  public ImmutableSortedMap<BigInteger, Library> getLibraries() {
    return fused.getTopologyApplication().getLibraries();
  }

  public ImmutableSortedMap<BigInteger, Hostpool> getHostpools() {
    return fused.getTopologyApplication().getHostpools();
  }

  public ImmutableSortedSet<BigInteger> getPeIds() {
    return ImmutableSortedSet.copyOf(pes.keySet());
  }

  public ImmutableSortedMap<BigInteger, Pe> getPes() {
    return ImmutableSortedMap.copyOf(pes);
  }

  public Pe getPe(BigInteger peId) {
    return pes.get(peId);
  }

  public ImmutableSortedMap<BigInteger, CustomMetric> getCustomMetrics() {
    return run.getCustomMetrics();
  }

  public Set<RuntimeCustomMetric> getRuntimeCustomMetrics() {
    return run.getRuntimeCustomMetrics();
  } // internal

  public RuntimeConstant getDisplay() {
    return fused.getTopologyApplication().getDisplay();
  }

  public String getBuildId() {
    return fused.getTopologyApplication().getBuildId();
  }

  public String getBundlePath() {
    return fused.getTopologyApplication().getBundlePath();
  }

  public ImmutableSortedMap<BigInteger, TopologyTupleTypeType> getTupleTypes() {
    return fused.getTopologyApplication().getTupleTypes();
  }

  public BigInteger getFusedApplicationId() {
    return fused.getApplicationId();
  }

  public BigInteger getTopologyApplicationId() {
    return fused.getTopologyApplication().getId();
  }

  public TopologyApplication getTopologyApplication() {
    return fused.getTopologyApplication();
  }

  public FusedApplication getFusedApplication() {
    return fused;
  }

  public ImmutableSortedSet<BigInteger> getNodeIndexes() {
    return fused.getTopologyApplication().getNodeIndexes();
  }

  public ToolkitMap getToolkitMap() {
    return fused.getTopologyApplication().getToolkitMap();
  }

  public String getJobGroup() {
    return run.getJobGroup();
  }

  public Map<BigInteger, SubmissionTimeValue> getSubmissionTimeValues() {
    return fused.getTopologyApplication().getSubmissionTimeValues();
  }

  public boolean isForceCancelling() {
    return run.isForceCancelling();
  }

  public void setForceCancelling(boolean force) {
    run.setForceCancelling(force);
  }

  public void setHealth(JobHealth health) {}

  public JobHealth getHealth() {
    return run.getHealth();
  }

  public String getExecStreamsVersion() {
    return run.getExecStreamsVersion();
  }

  public void setExecStreamsVersion(String version) {
    run.setExecStreamsVersion(version);
  }

  public BigInteger getGenId() {
    return run.getGenId();
  }

  public void setGenId(BigInteger genId) {
    run.setGenId(genId);
  }

  public void unregister(boolean force) {
    ObjectTracker objectTracker = SAMStaticContext.getObjectTracker();
    objectTracker.unregisterJob(getId());
  }

  private static void setJobName(RuntimeJob runtimeJob, String jobName)
      throws InvalidJobNameException {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug(
          "ENTER Job.setJobName with "
              + Arrays.deepToString(new Object[] {runtimeJob.getId(), jobName}));
    }
    String jobname;
    if (jobName == null || jobName.isEmpty()) {
      StringBuilder jobnameSS = new StringBuilder();
      jobnameSS.append(runtimeJob.getName()).append("_").append(runtimeJob.getId());
      while (!verifyJobName(jobnameSS.toString())) {
        jobnameSS.append("_").append(runtimeJob.getId());
      }
      jobname = jobnameSS.toString();
    } else {
      if (verifyJobName(jobName)) {
        jobname = jobName;
      } else {
        throw new InvalidJobNameException(
            "User specified job name " + jobName + " is not unique.",
            new DistilleryExceptionCode("CDISR1167E"),
            new Vector<String>(Collections.singletonList(jobName)));
      }
    }

    runtimeJob.setName(jobname);
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug(
          "EXIT Job.setJobName with " + Arrays.deepToString(new Object[] {runtimeJob.getName()}));
    }
  }

  private static boolean verifyJobName(String jobName) {
    ObjectTracker objectTracker = SAMStaticContext.getObjectTracker();
    for (BigInteger jobId : objectTracker.getJobs()) {
      Job job = objectTracker.getJob(jobId);
      if (jobName.equals(job.getName())) {
        return false;
      }
    }
    return true;
  }

  public boolean hasViewableStream(StreamId streamId) {
    return run.hasViewableStream(streamId);
  }

  public RuntimeStream getViewableStream(StreamId streamId) {
    return run.getViewableStream(streamId);
  }

  public Map<StreamId, RuntimeStream> getViewableStreams() {
    return run.getViewableStreams();
  }

  private void addViewableStreams(Collection<RuntimeStream> streams) {
    run.addViewableStreams(streams);
  }

  private void clearViewableStreams() {
    run.clearViewableStreams();
  }

  public List<RuntimeStream> getPhysicalViewableStreams(StreamId streamId) {
    return run.getPhysicalViewableStreams(streamId);
  }

  public void addCustomMetrics(String name, String kind, String desc, String operName) {
    run.addCustomMetrics(name, kind, desc, operName);
  }

  public RuntimeJobConfig getConfig() {
    return config;
  }

  public void setConfig(RuntimeJobConfig config) {
    this.config = config;
  }

  public RuntimeJobComposites getComposites() {
    return composites;
  }

  public void setComposites(RuntimeJobComposites composites) {
    this.composites = composites;
  }

  @Override
  public boolean isJobConstructed() {
    return isConstructed();
  }

  @Override
  public int hashCode() {
    return Objects.hash(run, fused, fused.getTopologyApplication(), config, composites);
  }

  public JobResourceSharing getJobResourceSharing() {
    return run.getJobResourceSharing();
  }

  public FusionScheme getFusionScheme() {
    return run.getFusionScheme();
  }

  public ThreadingModel getThreadingModel() {
    return run.getThreadingModel();
  }

  public Integer getProposedOperatorsPerResource() {
    return run.getProposedOperatorsPerResource();
  }

  public Integer getDynamicThreadingThreadCount() {
    return run.getDynamicThreadingThreadCount();
  }

  public Boolean getDynamicThreadingElastic() {
    return run.getDynamicThreadingElastic();
  }

  public String getRuntimeConfigStr() {
    return run.getRuntimeConfigStr();
  }

  public boolean supportFusionFission() {
    return run.supportFusionFission();
  }
}
