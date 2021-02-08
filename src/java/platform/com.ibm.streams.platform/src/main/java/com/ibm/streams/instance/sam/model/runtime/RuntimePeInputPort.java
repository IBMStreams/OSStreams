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
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import java.math.BigInteger;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

public class RuntimePeInputPort {

  private BigInteger jobId;
  private BigInteger peId;
  private BigInteger id;
  private BigInteger index;
  private Set<ConnectionId> incomingDynamicConnectionIds;
  private Set<ConnectionId> incomingStaticConnectionIds;
  private ImportedStreams importedStreams;

  public RuntimePeInputPort(BigInteger jobId, BigInteger peId, BigInteger id, BigInteger index) {
    this.jobId = jobId;
    this.peId = peId;
    this.id = id;
    this.index = index;
    incomingDynamicConnectionIds = Collections.synchronizedSet(new HashSet<ConnectionId>());
    incomingStaticConnectionIds = Collections.synchronizedSet(new HashSet<ConnectionId>());
  }

  // for recovery - initialize transient fields
  RuntimePeInputPort() {
    // incomingStaticConnectionIds = Collections.synchronizedSet(new HashSet<ConnectionId>());
  }

  public int hashCode() {
    return Objects.hash(
        jobId,
        peId,
        id,
        index,
        incomingDynamicConnectionIds,
        incomingStaticConnectionIds,
        importedStreams);
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

  public ImportedStreams getImports() {
    return importedStreams;
  }

  public void setImports(ImportedStreams imports) {
    this.importedStreams = imports;
  }

  // Dynamic Connection Getter / Setter / Adder / Remover
  public ImmutableSet<ConnectionId> getIncomingDynamicConnectionIds() {
    synchronized (incomingDynamicConnectionIds) {
      return ImmutableSet.copyOf(incomingDynamicConnectionIds);
    }
  }

  public void setIncomingDynamicConnectionIds(Set<ConnectionId> incomingDynamicConnectionIds) {
    synchronized (this.incomingDynamicConnectionIds) {
      this.incomingDynamicConnectionIds.clear();
      this.incomingDynamicConnectionIds.addAll(incomingDynamicConnectionIds);
    }
  }

  public boolean addIncomingDynamicConnectionIds(Set<ConnectionId> connIds) {
    return incomingDynamicConnectionIds.addAll(connIds);
  }

  public boolean removeIncomingDynamicConnectionIds(Set<ConnectionId> connIds) {
    return incomingDynamicConnectionIds.removeAll(connIds);
  }

  // Static Connection Getter / Setter / Adder / Remover
  public ImmutableSet<ConnectionId> getIncomingStaticConnectionIds() {
    synchronized (incomingStaticConnectionIds) {
      return ImmutableSet.copyOf(incomingStaticConnectionIds);
    }
  }

  public void setIncomingStaticConnectionIds(Set<ConnectionId> incomingStaticConnectionIds) {
    synchronized (this.incomingStaticConnectionIds) {
      this.incomingStaticConnectionIds.clear();
      this.incomingStaticConnectionIds.addAll(incomingStaticConnectionIds);
    }
  }

  public boolean addIncomingStaticConnectionIds(Set<ConnectionId> connIds) {
    return this.incomingStaticConnectionIds.addAll(connIds);
  }

  public boolean removeIncomingStaticConnectionIds(Set<ConnectionId> connIds) {
    return this.incomingStaticConnectionIds.removeAll(connIds);
  }
}
