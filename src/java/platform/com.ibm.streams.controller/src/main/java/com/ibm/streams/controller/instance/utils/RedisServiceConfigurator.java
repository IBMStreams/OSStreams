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

package com.ibm.streams.controller.instance.utils;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import lombok.Getter;
import lombok.Setter;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xbill.DNS.ARecord;
import org.xbill.DNS.Lookup;
import org.xbill.DNS.Record;
import org.xbill.DNS.TextParseException;
import org.xbill.DNS.Type;

public class RedisServiceConfigurator {

  private static final Logger LOGGER = LoggerFactory.getLogger(RedisServiceConfigurator.class);

  private final String service;
  private final Integer replicas;
  private final Integer shards;
  private final String restrictedProperty;
  private final boolean shuffle;

  @JsonSerialize
  private static class ReplicaGroup {
    @Getter @Setter private List<String> servers;
    @Getter @Setter private String description;

    ReplicaGroup(List<String> servers, String description) {
      this.servers = servers;
      this.description = description;
    }
  }

  @JsonSerialize
  private static class RedisConfiguration {
    @Getter @Setter private Integer replicas;
    @Getter @Setter private Integer shards;
    @Getter @Setter private List<ReplicaGroup> replicaGroups;

    RedisConfiguration(Integer replicas, Integer shards, List<ReplicaGroup> replicaGroups) {
      this.replicas = replicas;
      this.shards = shards;
      this.replicaGroups = replicaGroups;
    }
  }

  public RedisServiceConfigurator(
      String service,
      Integer replicas,
      Integer shards,
      String restrictedProperty,
      boolean shuffle) {
    this.service = service;
    this.replicas = replicas;
    this.shards = shards;
    this.restrictedProperty = restrictedProperty;
    this.shuffle = shuffle;
  }

  public boolean validate() {
    /*
     * Make sure that the redis parameters are valid.
     */
    if (service == null || replicas == null || shards == null) {
      LOGGER.error("Invalid checkpoint configuration");
      return false;
    }
    /*
     * Check if the parameters check out.
     */
    if (replicas == 0 || shards == 0) {
      LOGGER.error("Replica or shard count cannot be 0");
      return false;
    }
    if (replicas % 2 == 0) {
      LOGGER.error("Replica count must be an odd number");
      return false;
    }
    /*
     * Look-up the service.
     */
    try {
      Record[] records = new Lookup(service, Type.A).run();
      if (records.length != replicas * shards) {
        LOGGER.error("Available Redis server != replicaCount * shardCount");
        return false;
      }
    } catch (TextParseException e) {
      e.printStackTrace();
      return false;
    }
    /*
     * The configuration is valid.
     */
    return true;
  }

  /*
   * NOTE(xrg) This function was originally recomputing the full list upon each invocation. Although
   * nice, it was actually incorrect as only one version of that list must exist per job creation. Also,
   * it needs to be stable across pod restarts.
   */

  public String getReplicaServers() {
    /*
     * Compute the server list if it does not exist.
     */
    var replicaGroups = new ArrayList<ReplicaGroup>();
    try {
      /*
       * Collect all the A records.
       */
      var records = new Lookup(service, Type.A).run();
      var data =
          Arrays.stream(records)
              .map(r -> (ARecord) r)
              .map(r -> r.getAddress().getCanonicalHostName() + ":6379")
              .map(r -> restrictedProperty == null ? r : r + ":" + restrictedProperty)
              .collect(Collectors.toList());
      /*
       * Shuffle the collection if requested.
       */
      if (shuffle) {
        Collections.shuffle(data);
      } else {
        Collections.sort(data);
      }
      /*
       * Split into multiple lists.
       */
      for (int i = 0; i < replicas; i += 1) {
        var description = this.service + "#" + i;
        var replicaGroup =
            new ReplicaGroup(data.subList(i * shards, i * shards + shards), description);
        replicaGroups.add(replicaGroup);
      }
      /*
       * Format the server list.
       */
      var mapper = new ObjectMapper();
      var redisConfig = new RedisConfiguration(this.replicas, this.shards, replicaGroups);
      return mapper.writeValueAsString(redisConfig);
    } catch (TextParseException | JsonProcessingException e) {
      e.printStackTrace();
      return "{}";
    }
  }
}
