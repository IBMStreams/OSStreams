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

package com.ibm.streams.sch.composer;

import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.Set;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

public class HostSpec {
  public Set<String> getDeclaredHosts() {
    return (_declaredHosts);
  }

  public Set<String> getTags() {
    return _tags;
  }

  public boolean isIndexed() {
    return ((getIndexedPoolLocations() != null) ? (getIndexedPoolLocations().size() > 0) : false);
  }

  public void clearIndexing() {
    _indexesInPool = null;
  }

  public Set<PoolLocation> getIndexedPoolLocations() {
    return _indexesInPool;
  }
  //    public void addIndexedPoolLocation(PoolLocation pLoc) { _poolIndexes.add(pLoc); }
  public void setTags(Set<String> hostpoolTags) {
    _tags = new HashSet<>(hostpoolTags);
  }

  public boolean isDefault() {
    return ((_declaredHosts == null) && (_tags.size() == 0) && (_indexesInPool == null));
  }
  // return element if exactly 1 hp index in set.
  public BigInteger getPrimaryHostpoolIndex() {
    BigInteger primHpIndex = null;
    if (_hpIndexes.size() == 1) {
      primHpIndex = _hpIndexes.iterator().next();
    }
    return primHpIndex;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == null) {
      return false;
    }
    if (obj == this) {
      return true;
    }
    if (!obj.getClass().equals(getClass())) {
      return false;
    }
    HostSpec rhs = (HostSpec) obj;
    return new EqualsBuilder()
        .append(_declaredHosts, rhs._declaredHosts)
        .append(_tags, rhs._tags)
        .append(_indexesInPool, rhs._indexesInPool)
        .append(_hpIndexes, rhs._hpIndexes)
        .isEquals();
  }

  @Override
  public int hashCode() {
    return new HashCodeBuilder()
        .append(_declaredHosts)
        .append(_tags)
        .append(_indexesInPool)
        .append(_hpIndexes)
        .toHashCode();
  }

  public void addHostName(String hostName) {
    if (_declaredHosts == null) {
      _declaredHosts = new HashSet<>();
    }
    _declaredHosts.add(hostName);
  }

  public Set<BigInteger> getHostpoolIndexes() {
    return _hpIndexes;
  }

  private String poolIndexesToString() {
    StringBuilder str = new StringBuilder();
    str.append("indexes{");
    boolean first = true;
    for (PoolLocation pLoc : _indexesInPool) {
      if (!first) str.append(";");
      first = false;
      str.append("[" + pLoc.getPoolIndex() + "]");
      if (pLoc.getInpoolIndex() != null) {
        str.append("(" + pLoc.getInpoolIndex() + ")");
      }
    }
    str.append("}");
    return str.toString();
  }

  public String toString() {
    return ("HS:"
        + ((_declaredHosts != null) ? "declaredHost:" + _declaredHosts.toString() : "")
        + ((_tags.size() > 0) ? " tags:" + _tags.toString() : "")
        + ((_indexesInPool != null) ? poolIndexesToString() : ""));
  }

  public HostSpec(
      Set<String> declaredHostNames,
      Set<String> tags,
      Set<PoolLocation> indexedPoolLocs,
      Set<BigInteger> hpIndexes) {
    _tags = new HashSet<>(tags);
    _declaredHosts = (declaredHostNames != null) ? new HashSet<>(declaredHostNames) : null;
    _indexesInPool = (indexedPoolLocs != null) ? new HashSet<>(indexedPoolLocs) : null;
    _hpIndexes = hpIndexes;
  }

  private Set<String>
      _declaredHosts; // null - not specified.   empty set means no hosts possible (intersection
  // processing).
  private Set<String> _tags;
  private Set<PoolLocation> _indexesInPool;
  private Set<BigInteger> _hpIndexes; // indexes for all hostpool that contribute to this hostspec.
}
