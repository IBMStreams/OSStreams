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

import com.google.common.collect.ImmutableSortedSet;
import com.ibm.streams.platform.services.AdditionalExceptionDataType;
import java.math.BigInteger;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

public class RuntimePe {

  private BigInteger jobId;
  private BigInteger id;
  private BigInteger index;

  private SortedSet<BigInteger> inputPortIds;
  private SortedSet<BigInteger> outputPortIds;
  private TracingEmployment currentTracing; // Optional
  private TracingEmployment pendingTracing; // Optional
  private BigInteger nextTrcUpdateSeqNum; // Optional Field
  private PeHealth health;
  private BigInteger samRetryCount;
  private BigInteger launchCount;
  private boolean isHostValid;
  private PeStateReason reason;
  private String host;
  private boolean isStateStale;
  private BigInteger pid;
  private Set<String> hostTags;
  private BigInteger lastRunTimestamp; // Optional Field
  private String currentWorkingDirectory; // Optional Field
  private BigInteger genId;

  // internal for pe placement
  private transient String directedHost;
  private transient String computedHostPlacement;
  private transient Set<String> tempHostTags;
  private transient AdditionalExceptionDataType computedFailureMessage;

  public RuntimePe(
      BigInteger jobId,
      BigInteger id,
      BigInteger index,
      BigInteger genId,
      boolean buildingJob,
      boolean pesStopped) {
    this.jobId = jobId;
    this.id = id;
    this.index = index;
    this.inputPortIds = Collections.synchronizedSortedSet(new TreeSet<BigInteger>());
    this.outputPortIds = Collections.synchronizedSortedSet(new TreeSet<BigInteger>());
    this.hostTags = Collections.synchronizedSet(new HashSet<String>());

    if (buildingJob) {
      this.reason = PeStateReason.NONE;
    } else {
      // fission/fusion
      if (pesStopped) {
        this.reason = PeStateReason.REQUEST;
      } else {
        this.reason = PeStateReason.AUTOMATIC;
      }
    }

    // need another way to get from K8
    this.host = "localhost";

    this.samRetryCount = BigInteger.ZERO;
    this.launchCount = BigInteger.ZERO;
    this.pid = BigInteger.ZERO;
    this.genId = genId;

    PeHealth peh = new PeHealth();
    peh.setHealthy(true);
    peh.setPeHealth(PeHealthSummary.UP);
    peh.setRequiredConnectionsSummary(OutgoingConnectionHealthSummary.UP);
    peh.setOptionalConnectionsSummary(OutgoingConnectionHealthSummary.UP);
    this.health = peh;

    // pe placement
    this.directedHost = "";
    this.computedHostPlacement = "";
    this.tempHostTags = new TreeSet<>();
  }

  // Final Getters
  public BigInteger getJobId() {
    return jobId;
  }

  public BigInteger getId() {
    return id;
  }

  public BigInteger getIndex() {
    return index;
  }

  public ImmutableSortedSet<BigInteger> getInputPortIds() {
    synchronized (inputPortIds) {
      return ImmutableSortedSet.copyOf(inputPortIds);
    }
  }

  public void setInputPortIds(Set<BigInteger> inputPortIds) {
    synchronized (this.inputPortIds) {
      this.inputPortIds.clear();
      this.inputPortIds.addAll(inputPortIds);
    }
  }

  public ImmutableSortedSet<BigInteger> getOutputPortIds() {
    synchronized (outputPortIds) {
      return ImmutableSortedSet.copyOf(outputPortIds);
    }
  }

  public void setOutputPortIds(Set<BigInteger> outputPortIds) {
    synchronized (this.outputPortIds) {
      this.outputPortIds.clear();
      this.outputPortIds.addAll(outputPortIds);
    }
  }

  public boolean hasNextTrcUpdateSeqNum() {
    return nextTrcUpdateSeqNum != null;
  }

  public BigInteger getNextTrcUpdateSeqNum() {
    return nextTrcUpdateSeqNum;
  }

  public void setNextTrcUpdateSeqNum(BigInteger nextTrcUpdateSeqNum) {
    this.nextTrcUpdateSeqNum = nextTrcUpdateSeqNum;
  }

  public PeHealth getHealth() {
    return health;
  }

  public void setHealth(PeHealth health) {
    this.health = health;
  }

  public BigInteger getSamRetryCount() {
    return samRetryCount;
  }

  public void setSamRetryCount(BigInteger samRetryCount) {
    this.samRetryCount = samRetryCount;
  }

  public BigInteger getLaunchCount() {
    return launchCount;
  }

  public void setLaunchCount(BigInteger launchCount) {
    this.launchCount = launchCount;
  }

  public BigInteger getPid() {
    return pid;
  }

  public void setPid(BigInteger pid) {
    this.pid = pid;
  }

  public BigInteger getGenId() {
    return genId;
  }

  public boolean isHostValid() {
    return isHostValid;
  }

  public void setHostValid(boolean isHostValid) {
    this.isHostValid = isHostValid;
  }

  public PeStateReason getReason() {
    return reason;
  }

  public void setReason(PeStateReason reason) {
    this.reason = reason;
  }

  public String getHost() {
    return host;
  }

  public void setHost(String host) {
    this.host = host;
  }

  public String getCurrentWorkingDirectory() {
    return currentWorkingDirectory;
  }

  public void setCurrentWorkingDirectory(String currentWorkingDirectory) {
    this.currentWorkingDirectory = currentWorkingDirectory;
  }

  public boolean isStateStale() {
    return isStateStale;
  }

  public void setStateStale(boolean isStateStale) {
    this.isStateStale = isStateStale;
  }

  public BigInteger getLastRunTimestamp() {
    return lastRunTimestamp;
  }

  public void setLastRunTimestamp(BigInteger lastRunTimestamp) {
    this.lastRunTimestamp = lastRunTimestamp;
  }

  public TracingEmployment getCurrentTracing() {
    return currentTracing;
  }

  public void setCurrentTracing(TracingEmployment currentTracing) {
    this.currentTracing = currentTracing;
  }

  public TracingEmployment getPendingTracing() {
    return pendingTracing;
  }

  public void setPendingTracing(TracingEmployment pendingTracing) {
    this.pendingTracing = pendingTracing;
  }

  public String getDirectedHost() {
    return directedHost;
  }

  public void setDirectedHost(String directedHost) {
    this.directedHost = directedHost;
  }

  public String getComputedHostPlacement() {
    return computedHostPlacement;
  }

  public void setComputedHostPlacement(String computedHost) {
    this.computedHostPlacement = computedHost;
  }

  public Set<String> getTempHostTags() {
    return tempHostTags;
  }

  public void setTempHostTags(Set<String> tagSet) {
    this.tempHostTags = tagSet;
  }

  public void clearTempHostTags() {
    this.tempHostTags.clear();
  }

  public AdditionalExceptionDataType getComputedFailureMessage() {
    return computedFailureMessage;
  }

  public void setComputedFailureMessage(AdditionalExceptionDataType computedFailureMessage) {
    this.computedFailureMessage = computedFailureMessage;
  }

  public Set<String> getHostTags() {
    return hostTags;
  }

  public void setHostTags(Set<String> tagSet) {
    this.hostTags = tagSet;
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        jobId,
        id,
        index,
        inputPortIds,
        outputPortIds,
        nextTrcUpdateSeqNum,
        health,
        samRetryCount,
        launchCount,
        isHostValid,
        reason,
        host,
        isStateStale,
        pid,
        hostTags,
        lastRunTimestamp,
        currentWorkingDirectory);
  }
}
