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
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

public class HostPePlacements {
  private List<HostPePlacement> _pePlacements;
  private String _hostId;

  public HostPePlacements(String hostId) {
    _pePlacements = new ArrayList<>();
    _hostId = hostId;
  }

  public synchronized void removePePlacement(BigInteger oldPe) {
    Iterator<HostPePlacement> iter = _pePlacements.iterator();
    while (iter.hasNext()) {
      HostPePlacement hostPePlacement = iter.next();
      if (hostPePlacement.getPeId().equals(oldPe)) {
        iter.remove();
        return;
      }
    }
  }

  public synchronized ImmutableList<HostPePlacement> getHostPePlacements() {
    return ImmutableList.copyOf(_pePlacements);
  }

  public String getHostId() {
    return _hostId;
  }

  public synchronized Set<BigInteger> getPlacedPeIds() {
    Set<BigInteger> peIdSet = new TreeSet<>();
    for (HostPePlacement hostPePlacement : _pePlacements) {
      peIdSet.add(hostPePlacement.getPeId());
    }
    return peIdSet;
  }

  public synchronized void addPePlacement(BigInteger newPe, Set<String> tagSet, String buildId) {
    HostPePlacement newPePlcmt = new HostPePlacement(buildId, newPe, _hostId);
    for (String tag : tagSet) {
      newPePlcmt.addRequiredHostTag(tag);
    }
    _pePlacements.add(newPePlcmt);
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("hostId=" + _hostId);
    builder.append(" pePlacements=" + _pePlacements);
    return builder.toString();
  }
}
