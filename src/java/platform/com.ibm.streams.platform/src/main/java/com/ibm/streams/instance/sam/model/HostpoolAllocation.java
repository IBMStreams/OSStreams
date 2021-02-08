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

import com.google.common.collect.ImmutableList;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

public class HostpoolAllocation {

  // Private Members
  private List<HostAllocation> hostAllocations;
  private BigInteger index;

  public HostpoolAllocation(BigInteger index) {
    this.index = index;
    hostAllocations = new ArrayList<HostAllocation>();
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((hostAllocations == null) ? 0 : hostAllocations.hashCode());
    result = prime * result + ((index == null) ? 0 : index.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    HostpoolAllocation other = (HostpoolAllocation) obj;
    if (hostAllocations == null) {
      if (other.hostAllocations != null) return false;
    } else if (!hostAllocations.equals(other.hostAllocations)) return false;
    if (index == null) {
      if (other.index != null) return false;
    } else if (!index.equals(other.index)) return false;
    return true;
  }

  /**
   * Retrieve the collection of host allocation information
   *
   * @return The HostAllocation objects if set, otherwise an empty collection
   */
  public ImmutableList<HostAllocation> getHostAllocations() {
    return ImmutableList.copyOf(hostAllocations);
  }

  /**
   * Retrieve the hostpool index
   *
   * @return Index value
   */
  public BigInteger getIndex() {
    return index;
  }

  public void addHostAllocation(HostAllocation hostAllocation) {
    hostAllocations.add(hostAllocation);
  }

  public void removeHostAllocation(HostAllocation hostAllocation) {
    hostAllocations.remove(hostAllocation);
  }

  public static class HostAllocation {
    // Private Members
    private BigInteger hostIndexInPool;
    private String hostName;

    public HostAllocation(String hostName) {
      this.hostName = hostName;
    }

    public void setHostIndexInPool(BigInteger hostIndexInPool) {
      this.hostIndexInPool = hostIndexInPool;
    }

    /**
     * Retrieve the host index within the pool of hosts
     *
     * @return BigInteger index value of host in the pool, null if not set
     */
    public BigInteger getHostIndexInPool() {
      return hostIndexInPool;
    }

    /**
     * Retrieve the host's name
     *
     * @return Host Name
     */
    public String getHostName() {
      return hostName;
    }

    @Override
    public int hashCode() {
      final int prime = 31;
      int result = 1;
      result = prime * result + ((hostIndexInPool == null) ? 0 : hostIndexInPool.hashCode());
      result = prime * result + ((hostName == null) ? 0 : hostName.hashCode());
      return result;
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj) return true;
      if (obj == null) return false;
      if (getClass() != obj.getClass()) return false;
      HostAllocation other = (HostAllocation) obj;
      if (hostIndexInPool == null) {
        if (other.hostIndexInPool != null) return false;
      } else if (!hostIndexInPool.equals(other.hostIndexInPool)) return false;
      if (hostName == null) {
        if (other.hostName != null) return false;
      } else if (!hostName.equals(other.hostName)) return false;
      return true;
    }
  }
}
