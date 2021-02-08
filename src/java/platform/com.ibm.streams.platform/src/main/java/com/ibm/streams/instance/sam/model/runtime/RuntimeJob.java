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

package com.ibm.streams.instance.sam.model.runtime;

import com.google.common.collect.ImmutableSet;
import com.google.common.collect.ImmutableSortedMap;
import com.google.common.collect.ImmutableSortedSet;
import com.ibm.streams.admin.internal.api.VersionInfo;
import com.ibm.streams.admin.internal.api.config.PropertyDefinition.FusionScheme;
import com.ibm.streams.admin.internal.api.config.PropertyDefinition.JobResourceSharing;
import com.ibm.streams.admin.internal.api.config.PropertyDefinition.ThreadingModel;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.SamConstants;
import com.ibm.streams.instance.sam.model.HostpoolAllocation;
import com.ibm.streams.instance.sam.model.topology.CustomMetric;
import com.ibm.streams.instance.sam.model.topology.CustomMetricKind;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Properties;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;

public class RuntimeJob {

  private SortedSet<BigInteger> peIds;
  private JobHealth health;
  private SortedMap<BigInteger, HostpoolAllocation> hostpoolAllocations;
  private BigInteger submitTime;
  private BigInteger id;
  private transient Map<StreamId, RuntimeStream> viewableStreams;
  private String user;
  private String name;
  private String applicationName;
  private String jobGroup;
  private boolean forceCancelling;
  private SortedMap<BigInteger, CustomMetric> customMetrics;
  private transient Set<RuntimeCustomMetric>
      runtimeCustomMetics; // internal; prevents adding duplicate customMetrics
  private String execStreamsVersion;
  private BigInteger genId;

  // Some job related properties that can be updated with updateoperators.
  private FusionScheme fusionScheme;
  private ThreadingModel threadingModel;
  private JobResourceSharing jobResourceSharing;
  private Integer proposedOperatorsPerResource;
  private Integer dynamicThreadingThreadCount;
  private Boolean dynamicThreadingElastic;

  // Some booleans that track job supported feature release by release
  private boolean supportFusionFission;

  public RuntimeJob(BigInteger id, String user, String appName, String jobGroup) {
    this.id = id;
    submitTime = BigInteger.valueOf(System.currentTimeMillis() / 1000);
    this.user = user;
    applicationName = appName;
    name = applicationName; // set name to applicationName initially.

    JobHealth jht = new JobHealth();
    jht.setHealthy(false);
    jht.setPeHealth(PeHealthSummary.DOWN);
    health = jht;
    this.jobGroup = jobGroup;
    this.peIds = new TreeSet<>();

    hostpoolAllocations =
        Collections.synchronizedSortedMap(new TreeMap<BigInteger, HostpoolAllocation>());
    viewableStreams = new HashMap<StreamId, RuntimeStream>();

    forceCancelling = false;
    customMetrics = new TreeMap<>();
    runtimeCustomMetics = new HashSet<>();

    execStreamsVersion = VersionInfo.VERSION_424;

    genId = BigInteger.ZERO;

    supportFusionFission = true;

    initRuntimeConfigs();
  }

  public BigInteger getId() {
    return id;
  }

  public ImmutableSortedSet<BigInteger> getPeIds() {
    return ImmutableSortedSet.copyOf(peIds);
  }

  public void addPeIds(Set<BigInteger> peIds) {
    this.peIds.addAll(peIds);
  }

  public void removePeIds(Set<BigInteger> peIds) {
    this.peIds.removeAll(peIds);
  }

  public String getUser() {
    return user;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public String getApplicationName() {
    return applicationName;
  }

  public String getJobGroup() {
    return jobGroup;
  }

  public boolean isForceCancelling() {
    return forceCancelling;
  }

  public void setForceCancelling(boolean force) {
    this.forceCancelling = force;
  }

  public JobHealth getHealth() {
    return health;
  }

  public void setHealth(JobHealth health) {
    this.health = health;
  }

  public BigInteger getSubmitTime() {
    return submitTime;
  }

  public ImmutableSet<HostpoolAllocation> getHostpoolAllocations() {
    synchronized (hostpoolAllocations) {
      return ImmutableSet.copyOf(hostpoolAllocations.values());
    }
  }

  public void clearHostpoolAllocations() {
    hostpoolAllocations.clear();
  }

  public void addHostpoolAllocation(HostpoolAllocation hostpoolAllocation) {
    hostpoolAllocations.put(hostpoolAllocation.getIndex(), hostpoolAllocation);
  }

  public HostpoolAllocation removeHostpoolAllocation(BigInteger hostpoolAllocationIndex) {
    return hostpoolAllocations.remove(hostpoolAllocationIndex);
  }

  public boolean hasViewableStream(StreamId streamId) {
    return viewableStreams.containsKey(streamId);
  }

  public RuntimeStream getViewableStream(StreamId streamId) {
    return viewableStreams.get(streamId);
  }

  public Map<StreamId, RuntimeStream> getViewableStreams() {
    return viewableStreams;
  }

  public void addViewableStreams(Collection<RuntimeStream> streams) {
    for (RuntimeStream stream : streams) {
      if (!stream.isLogical()) {
        // physical stream, direct add them.
        viewableStreams.put(stream.getStreamId(), stream);
      } else {
        // logic stream
        if (viewableStreams.containsKey(stream.getStreamId())) {
          int numChannels =
              stream.getNumChannels() + viewableStreams.get(stream.getStreamId()).getNumChannels();
          viewableStreams.get(stream.getStreamId()).setNumChannels(numChannels);
        } else {
          viewableStreams.put(stream.getStreamId(), stream);
        }
      }
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace(
            "Added/updated viewable stream for " + stream.getStreamId().toJson().toString());
      }
    }
  }

  public void removeViewableStreams(Collection<RuntimeStream> streams) {
    for (RuntimeStream stream : streams) {
      viewableStreams.remove(stream.getStreamId());
    }
  }

  public void clearViewableStreams() {
    viewableStreams.clear();
  }

  public int hashCode() {
    return Objects.hash(
        peIds,
        health,
        hostpoolAllocations,
        submitTime,
        id,
        user,
        name,
        forceCancelling,
        customMetrics);
  }

  public List<RuntimeStream> getPhysicalViewableStreams(StreamId streamId) {
    List<RuntimeStream> phyStreams = new ArrayList<RuntimeStream>();
    for (RuntimeStream stream : viewableStreams.values()) {
      // skip logical streams
      if (!stream.isLogical()) {
        // check if within parallel region
        if (stream.getLogicalOperInstanceName() != null) {
          // Get the physical stream within the parallel region for the streamId
          if (stream.getLogicalOperInstanceName().equals(streamId.getOperInstanceName())) {
            // found a physical stream that belongs to logical stream -- single layer UDP behavior
            // Note: Keep this code here to speed up the single-layer UDP process.
            phyStreams.add(stream);
          } else {
            boolean findmatch = true;
            String[] phyNames = stream.getOperInstanceName().split("\\.");
            String[] logNames = streamId.getOperInstanceName().split("\\.");
            if (phyNames.length == logNames.length) {
              for (int i = 0; i < phyNames.length; i++) {
                if (phyNames[i].indexOf(logNames[i]) != 0) {
                  findmatch = false;
                  break;
                }
              }
            } else {
              findmatch = false;
            }
            if (findmatch) {
              phyStreams.add(stream);
            }
          }
        } else {
          // outside of parallel region
          if (stream.getOperInstanceName().equals(streamId.getOperInstanceName())) {
            // found a non-parallel stream.
            phyStreams.add(stream);
            break;
          }
        }
      }
    }
    return phyStreams;
  }

  public ImmutableSortedMap<BigInteger, CustomMetric> getCustomMetrics() {
    return ImmutableSortedMap.copyOf(customMetrics);
  }

  public Set<RuntimeCustomMetric> getRuntimeCustomMetrics() {
    return Collections.unmodifiableSet(runtimeCustomMetics);
  }

  public void setCustomMetrics(ImmutableSortedMap<BigInteger, CustomMetric> customMetrics) {
    for (BigInteger key : customMetrics.keySet()) {
      CustomMetric customMetric = customMetrics.get(key).copy();
      this.customMetrics.put(key, customMetric);
      RuntimeCustomMetric runtimeCustomMetric =
          new RuntimeCustomMetric(
              customMetric.getName(), customMetric.getKind(), customMetric.getDescription());
      runtimeCustomMetics.add(runtimeCustomMetric);
    }
  }

  // for recovery
  public void rebuildRuntimeCustomMetrics() {
    for (BigInteger key : customMetrics.keySet()) {
      CustomMetric customMetric = customMetrics.get(key);
      RuntimeCustomMetric runtimeCustomMetric =
          new RuntimeCustomMetric(
              customMetric.getName(), customMetric.getKind(), customMetric.getDescription());
      runtimeCustomMetics.add(runtimeCustomMetric);
    }
  }

  public void addCustomMetrics(String name, String kind, String desc, String operName) {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug(
          "Enter, with name:"
              + name
              + ", kind:"
              + kind
              + ", desc:"
              + desc
              + ", operName:"
              + operName);
    }
    RuntimeCustomMetric runtimeCustomMetric =
        new RuntimeCustomMetric(name, CustomMetricKind.getEnum(kind), desc);
    BigInteger thisKey = null;
    // check for duplicates
    if (!runtimeCustomMetics.contains(runtimeCustomMetric)) {
      runtimeCustomMetics.add(runtimeCustomMetric);
      if (!this.customMetrics.isEmpty()) {
        thisKey = this.customMetrics.lastKey().add(BigInteger.ONE);
      } else {
        thisKey = BigInteger.ZERO;
      }
      CustomMetricKind thisKind = CustomMetricKind.getEnum(kind);
      CustomMetric thisMetric = new CustomMetric(thisKey, name, thisKind, desc);
      this.customMetrics.put(thisKey, thisMetric);
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Added new CustomMetric with key:" + thisKey.toString());
      }
    } else {
      // customMetric already exist, add its index to the operator.
      CustomMetricKind thisKind = CustomMetricKind.getEnum(kind);
      for (BigInteger key : this.customMetrics.keySet()) {
        CustomMetric m = this.customMetrics.get(key);
        if (name.equals(m.getName())
            && thisKind.equals(m.getKind())
            && desc.equals(m.getDescription())) {
          thisKey = m.getIndex();
          if (Trace.isEnabled(Level.DEBUG)) {
            Trace.logDebug("Got existing CustomMetric object with key:" + thisKey.toString());
          }
          break;
        }
      }
    }

    if (thisKey != null && operName != null) {
      // Adding this metric to the related operator.
      // If no operatorName set, add to all operators.
      BigInteger appId = SAMStaticContext.getObjectTracker().getJob(id).getTopologyApplicationId();
      TopologyApplication ta = SAMStaticContext.getTopologyTracker().getTopologyApplication(appId);
      SortedSet<TopologyNode> nodes = ta.getNodes();
      if (nodes != null) {
        for (TopologyNode node : nodes) {
          if (node.getName().equals(operName) || node.getLogicalName().equals(operName)) {
            node.getCustomMetrics().add(thisKey);
            if (Trace.isEnabled(Level.DEBUG)) {
              Trace.logDebug(
                  "Added this custome metric index:"
                      + thisKey.toString()
                      + " to operator:"
                      + operName);
            }
            break;
          }
        }
      }
    }
  }

  public String getExecStreamsVersion() {
    return execStreamsVersion;
  }

  public void setExecStreamsVersion(String version) {
    execStreamsVersion = version;
  }

  public BigInteger getGenId() {
    return genId;
  }

  public void setGenId(BigInteger genId) {
    this.genId = genId;
  }

  public JobResourceSharing getJobResourceSharing() {
    return jobResourceSharing;
  }

  public void setJobResourceSharing(JobResourceSharing sharing) {
    jobResourceSharing = sharing;
  }

  public FusionScheme getFusionScheme() {
    return fusionScheme;
  }

  public void setFusionScheme(FusionScheme scheme) {
    fusionScheme = scheme;
  }

  public ThreadingModel getThreadingModel() {
    return threadingModel;
  }

  public void setThreadingModel(ThreadingModel model) {
    threadingModel = model;
  }

  public Integer getProposedOperatorsPerResource() {
    return proposedOperatorsPerResource;
  }

  public void setProposedOperatorsPerResource(Integer ratio) {
    proposedOperatorsPerResource = ratio;
  }

  public Integer getDynamicThreadingThreadCount() {
    return dynamicThreadingThreadCount;
  }

  public void setDynamicThreadingThreadCount(Integer count) {
    dynamicThreadingThreadCount = count;
  }

  public Boolean getDynamicThreadingElastic() {
    return dynamicThreadingElastic;
  }

  public void setDynamicThreadingElastic(Boolean elastic) {
    dynamicThreadingElastic = elastic;
  }

  private void initRuntimeConfigs() {
    if (jobResourceSharing == null) {
      jobResourceSharing = JobResourceSharing.SAME_INSTANCE;
    }
    if (fusionScheme == null) {
      fusionScheme = FusionScheme.AUTOMATIC;
    }
    if (threadingModel == null) {
      threadingModel = ThreadingModel.NOT_SPECIFIED;
    }
    if (proposedOperatorsPerResource == null) {
      proposedOperatorsPerResource = Integer.valueOf(8);
    }
  }

  public Properties getRuntimeConfigs() {
    Properties cFig = new Properties();
    if (jobResourceSharing != null) {
      cFig.put(SamConstants.jobResourceSharing, jobResourceSharing.toString());
    }
    if (fusionScheme != null) {
      cFig.put(SamConstants.fusionScheme, fusionScheme.toString());
    }
    if (threadingModel != null) {
      cFig.put(SamConstants.threadingModel, threadingModel.toString());
    }
    if (proposedOperatorsPerResource != null) {
      cFig.put(SamConstants.proposedOperatorsPerResource, proposedOperatorsPerResource.toString());
    }
    if (dynamicThreadingThreadCount != null) {
      cFig.put(SamConstants.dynamicThreadingThreadCount, dynamicThreadingThreadCount.toString());
    }
    if (dynamicThreadingElastic != null) {
      cFig.put(SamConstants.dynamicThreadingElastic, dynamicThreadingElastic.toString());
    }
    return cFig;
  }

  public String getRuntimeConfigStr() {
    StringBuilder outStrStrm = new StringBuilder();
    if (jobResourceSharing != null) {
      outStrStrm.append("jobResourceSharing=" + jobResourceSharing.toString());
    }
    if (fusionScheme != null) {
      outStrStrm.append(", fusionScheme=" + fusionScheme.toString());
    }
    if (threadingModel != null) {
      outStrStrm.append(", threadingModel=" + threadingModel.toString());
    }
    if (proposedOperatorsPerResource != null) {
      outStrStrm.append(
          ", proposedOperatorsPerResource=" + proposedOperatorsPerResource.toString());
    }
    if (dynamicThreadingThreadCount != null) {
      outStrStrm.append(", dynamicThreadingThreadCount=" + dynamicThreadingThreadCount.toString());
    }
    if (dynamicThreadingElastic != null) {
      outStrStrm.append(", dynamicThreadingElastic=" + dynamicThreadingElastic.toString());
    }
    return (outStrStrm.toString());
  }

  // public methods for checking if a feature supported or not
  public boolean supportFusionFission() {
    return this.supportFusionFission;
  }
}
