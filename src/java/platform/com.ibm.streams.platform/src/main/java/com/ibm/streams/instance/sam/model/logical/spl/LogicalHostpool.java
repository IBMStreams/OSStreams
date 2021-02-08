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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.MembershipMode;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class LogicalHostpool extends Hostpool {

  private Map<ParallelRegionAndChannel, LogicalHostpool> replicaMap = new HashMap<>();

  public LogicalHostpool(
      String name,
      BigInteger index,
      MembershipMode membershipMode,
      List<String> hosts,
      Set<String> tags,
      BigInteger size) {
    super(name, index, membershipMode, hosts, tags, size);
  }

  public Hostpool createHostpool() {
    Hostpool hostpool = new Hostpool(this);
    return hostpool;
  }

  public LogicalHostpool findOrCreateReplica(
      BigInteger regionIndex,
      List<BigInteger> channelIndexes,
      Set<String> intersectTags,
      SPLLogicalModel model) {
    ParallelRegionAndChannel uid = new ParallelRegionAndChannel(regionIndex, channelIndexes);
    if (replicaMap.containsKey(uid)) {
      return replicaMap.get(uid);
    }

    StringBuilder replicaName = new StringBuilder();
    replicaName.append(getName()).append('_').append(regionIndex);
    for (BigInteger i : channelIndexes) {
      replicaName.append("_").append(i);
    }

    Set<String> replicaTags = new HashSet<>();
    for (String tag : getTags()) {
      if (intersectTags.contains(tag)) {
        StringBuilder replicaTag = new StringBuilder();
        replicaTag.append(tag);
        for (BigInteger i : channelIndexes) {
          replicaTag.append("_").append(i);
        }
        replicaTags.add(replicaTag.toString());
      } else {
        replicaTags.add(tag);
      }
    } // end for

    LogicalHostpool replica =
        new LogicalHostpool(
            replicaName.toString(),
            BigInteger.valueOf(model.getHostpoolMap().size()),
            getMembershipMode(),
            getHosts(),
            replicaTags,
            getSize());
    replicaMap.put(uid, replica);
    model.getHostpoolMap().put(replica.getIndex(), replica);
    return replica;
  }
}
