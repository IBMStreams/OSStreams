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

package com.ibm.streams.instance.sam.model.fuser;

import java.math.BigInteger;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

public class FusedContainerPort implements Comparable<FusedContainerPort> {

  private BigInteger index;
  private BigInteger containerIndex;
  private BigInteger appId;
  protected transient Set<FusedContainerConnectionId> containerConnections;

  public FusedContainerPort(BigInteger index, BigInteger containerIndex, BigInteger appId) {
    this.index = index;
    this.containerIndex = containerIndex;
    this.appId = appId;
    containerConnections = new HashSet<>();
  }

  // for recovery - initialize transient fields
  FusedContainerPort() {
    containerConnections = new HashSet<>();
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((appId == null) ? 0 : appId.hashCode());
    result = prime * result + ((containerIndex == null) ? 0 : containerIndex.hashCode());
    result = prime * result + ((index == null) ? 0 : index.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    FusedContainerPort other = (FusedContainerPort) obj;
    if (appId == null) {
      if (other.appId != null) return false;
    } else if (!appId.equals(other.appId)) return false;
    if (containerIndex == null) {
      if (other.containerIndex != null) return false;
    } else if (!containerIndex.equals(other.containerIndex)) return false;
    if (index == null) {
      if (other.index != null) return false;
    } else if (!index.equals(other.index)) return false;
    return true;
  }

  @Override
  public int compareTo(FusedContainerPort port) {
    if (this.getClass().equals(port.getClass())) return this.index.compareTo(port.getIndex());
    else return this.getClass().toString().compareTo(port.getClass().toString());
  }

  public BigInteger getIndex() {
    return index;
  }

  public BigInteger getContainerIndex() {
    return containerIndex;
  }

  public BigInteger getFusedApplicationId() {
    return appId;
  }

  public Set<FusedContainerConnectionId> getContainerConnections() {
    return Collections.unmodifiableSet(containerConnections);
  }

  public void addContainerConnection(FusedContainerConnectionId connId) {
    containerConnections.add(connId);
  }

  public void removeContainerConnection(FusedContainerConnectionId connId) {
    containerConnections.remove(connId);
  }
}
