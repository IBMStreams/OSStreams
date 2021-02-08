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

package com.ibm.streams.controller.crds.crs;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class ConsistentRegionSpec implements KubernetesResource {
  @Getter @Setter private Integer regionIndex;
  @Getter @Setter private Integer logicalIndex;
  @Getter @Setter private Integer numStartOperators;
  @Getter @Setter private String trigger;
  @Getter @Setter private double period;
  @Getter @Setter private double drainTimeout;
  @Getter @Setter private double resetTimeout;
  @Getter @Setter private long maxConsecutiveResetAttempts;
  @Getter @Setter private Set<BigInteger> pesInRegion;
  @Getter @Setter private Map<String, Boolean> operatorsToStartRegionMap;
  @Getter @Setter private Map<String, Boolean> operatorsToTriggerMap;
  @Getter @Setter private boolean isRegionHealthy;
  @Getter @Setter private boolean isCleanStart;
  @Getter @Setter private EConsistentRegionState state;
  @Getter @Setter private long currentResetAttempt;
  @Getter @Setter private long currentMaxResetAttempts;
  @Getter @Setter private long currentSeqID;
  @Getter @Setter private long lastCompletedSeqID;
  @Getter @Setter private long pendingSeqID;
  @Getter @Setter private long toRetireSeqID;
  @Getter @Setter private boolean isMustReset;
  @Getter @Setter private boolean isHealthyFirstTime;
  @Getter @Setter private Map<BigInteger, EPeStatus> peToCompletion;
  @Getter @Setter private Integer currentTimerSeqId;

  // metrics helper
  @Getter @Setter private MovingAverage avgDrainTime;
  @Getter @Setter private MovingAverage avgResetTime;
  @Getter @Setter private long drainStartTimestamp;
  @Getter @Setter private long resetStartTimestamp;
  // metric
  @Getter @Setter private long avgDrainTimeMetric;
  @Getter @Setter private long avgResetTimeMetric;
  @Getter @Setter private long lastConsistentStateTime;
  @Getter @Setter private long lastResetTime;
  @Getter @Setter private long lastCompletedDrain;
  @Getter @Setter private long lastCompletedReset;

  public ConsistentRegionSpec() {
    this.isRegionHealthy = false;
    this.isCleanStart = false;
    this.state = EConsistentRegionState.STARTED;
    this.currentResetAttempt = 0;
    this.currentSeqID = 1;
    this.lastCompletedSeqID = 0;
    this.pendingSeqID = 0;
    this.toRetireSeqID = 0;
    this.isMustReset = false;
    this.isHealthyFirstTime = false;
    this.peToCompletion = new HashMap<>();
    this.currentTimerSeqId = 0;
    this.avgDrainTime = new MovingAverage(10);
    this.avgResetTime = new MovingAverage(10);
    this.avgDrainTimeMetric = 0;
    this.avgResetTimeMetric = 0;
    this.lastConsistentStateTime = 0;
    this.lastResetTime = 0;
    this.lastCompletedDrain = -1;
    this.lastCompletedReset = -1;
  }

  public ConsistentRegionSpec(ConsistentRegionSpec spec) {
    this.regionIndex = spec.regionIndex;
    this.logicalIndex = spec.logicalIndex;
    this.numStartOperators = spec.numStartOperators;
    this.trigger = spec.trigger;
    this.period = spec.period;
    this.drainTimeout = spec.drainTimeout;
    this.resetTimeout = spec.resetTimeout;
    this.maxConsecutiveResetAttempts = spec.maxConsecutiveResetAttempts;
    this.pesInRegion = new HashSet<>(spec.pesInRegion);
    this.operatorsToStartRegionMap = new HashMap<>(spec.operatorsToStartRegionMap);
    this.operatorsToTriggerMap = new HashMap<>(spec.operatorsToTriggerMap);
    this.isRegionHealthy = spec.isRegionHealthy;
    this.isCleanStart = spec.isCleanStart;
    this.state = spec.state;
    this.currentResetAttempt = spec.currentResetAttempt;
    this.currentMaxResetAttempts = spec.currentMaxResetAttempts;
    this.currentSeqID = spec.currentSeqID;
    this.lastCompletedSeqID = spec.lastCompletedSeqID;
    this.pendingSeqID = spec.pendingSeqID;
    this.toRetireSeqID = spec.toRetireSeqID;
    this.isMustReset = spec.isMustReset;
    this.isHealthyFirstTime = spec.isHealthyFirstTime;
    this.peToCompletion = new HashMap<>(spec.peToCompletion);
    this.currentTimerSeqId = spec.currentTimerSeqId;
    this.avgDrainTime = new MovingAverage(spec.avgDrainTime);
    this.avgResetTime = new MovingAverage(spec.avgResetTime);
    this.drainStartTimestamp = spec.drainStartTimestamp;
    this.resetStartTimestamp = spec.resetStartTimestamp;
    this.avgDrainTimeMetric = spec.avgDrainTimeMetric;
    this.avgResetTimeMetric = spec.avgResetTimeMetric;
    this.lastConsistentStateTime = spec.lastConsistentStateTime;
    this.lastResetTime = spec.lastResetTime;
    this.lastCompletedDrain = spec.lastCompletedDrain;
    this.lastCompletedReset = spec.lastCompletedReset;
  }

  public void resetPeToCompletion() {
    pesInRegion.forEach(p -> peToCompletion.put(p, EPeStatus.None));
  }
}
