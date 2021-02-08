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
import com.ibm.streams.instance.sam.model.ConnectionId;
import com.ibm.streams.instance.sam.model.topology.ExportedStream;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

public class RuntimePeOutputPort {

  // TODO: Port Ref?
  private BigInteger jobId;
  private BigInteger peId;
  private BigInteger id;
  private BigInteger index;
  private BigInteger IUO_currentNotificationCount;
  private BigInteger IUO_maxNotificationCount;
  private Set<ConnectionId> outgoingDynamicConnectionIds;
  private Set<ConnectionId> outgoingStaticConnectionIds;
  private ExportedStream exportedStream;

  public RuntimePeOutputPort(BigInteger jobId, BigInteger peId, BigInteger id, BigInteger index) {
    this.jobId = jobId;
    this.peId = peId;
    this.id = id;
    this.index = index;
    outgoingDynamicConnectionIds = Collections.synchronizedSet(new HashSet<ConnectionId>());
    outgoingStaticConnectionIds = Collections.synchronizedSet(new HashSet<ConnectionId>());
    IUO_currentNotificationCount = BigInteger.ZERO;
    IUO_maxNotificationCount = BigInteger.ZERO;
  }

  // for recovery - initialize transient fields
  RuntimePeOutputPort() {
    // outgoingStaticConnectionIds = Collections.synchronizedSet(new HashSet<ConnectionId>());
  }

  public int hashCode() {
    return Objects.hash(
        jobId,
        peId,
        id,
        index,
        IUO_currentNotificationCount,
        IUO_maxNotificationCount,
        outgoingDynamicConnectionIds,
        outgoingStaticConnectionIds,
        exportedStream);
  }

  // Final getters
  public BigInteger getJobId() {
    return jobId;
  }

  public BigInteger getPeId() {
    return peId;
  }

  public BigInteger getId() {
    return id;
  }

  public BigInteger getIndex() {
    return index;
  }

  // Mutable Getters / Setters

  public ExportedStream getExport() {
    return exportedStream;
  }

  public void setExport(ExportedStream export) {
    this.exportedStream = export;
  }

  public BigInteger getCurrentNotificationCount() {
    return IUO_currentNotificationCount;
  }

  public void setCurrentNotificationCount(BigInteger IUO_currentNotificationCount) {
    this.IUO_currentNotificationCount = IUO_currentNotificationCount;
  }

  public List<BigInteger> newMaxNotificationCounts(int blockSize) {
    synchronized (this) {
      List<BigInteger> rval = new ArrayList<>();
      for (int i = 0; i < blockSize; i++) {
        IUO_maxNotificationCount = IUO_maxNotificationCount.add(BigInteger.ONE);
        rval.add(IUO_maxNotificationCount);
      }
      return rval;
    }
  }

  // for recovery
  public BigInteger getMaxNotificationCount() {
    return IUO_maxNotificationCount;
  }

  // Dynamic Connection Getter / Setter / Adder / Remover
  public ImmutableSet<ConnectionId> getOutgoingDynamicConnectionIds() {
    synchronized (outgoingDynamicConnectionIds) {
      return ImmutableSet.copyOf(outgoingDynamicConnectionIds);
    }
  }

  public void setOutgoingDynamicConnectionIds(Set<ConnectionId> outgoingDynamicConnectionIds) {
    synchronized (this.outgoingDynamicConnectionIds) {
      this.outgoingDynamicConnectionIds.clear();
      this.outgoingDynamicConnectionIds.addAll(outgoingDynamicConnectionIds);
    }
  }

  public boolean addOutgoingDynamicConnectionIds(Set<ConnectionId> connIds) {
    return outgoingDynamicConnectionIds.addAll(connIds);
  }

  public boolean removeOutgoingDynamicConnectionIds(Set<ConnectionId> connIds) {
    return outgoingDynamicConnectionIds.removeAll(connIds);
  }

  // Static Connection Getter / Setter / Adder / Remover
  public ImmutableSet<ConnectionId> getOutgoingStaticConnectionIds() {
    synchronized (outgoingStaticConnectionIds) {
      return ImmutableSet.copyOf(outgoingStaticConnectionIds);
    }
  }

  public void setOutgoingStaticConnectionIds(Set<ConnectionId> outgoingStaticConnectionIds) {
    synchronized (this.outgoingStaticConnectionIds) {
      this.outgoingStaticConnectionIds.clear();
      this.outgoingStaticConnectionIds.addAll(outgoingStaticConnectionIds);
    }
  }

  public boolean addOutgoingStaticConnectionIds(Set<ConnectionId> connIds) {
    return this.outgoingStaticConnectionIds.addAll(connIds);
  }

  public boolean removeOutgoingStaticConnectionIds(Set<ConnectionId> connIds) {
    return this.outgoingStaticConnectionIds.removeAll(connIds);
  }
}
