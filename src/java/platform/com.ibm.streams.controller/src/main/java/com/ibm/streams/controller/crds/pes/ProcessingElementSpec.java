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

package com.ibm.streams.controller.crds.pes;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import io.fabric8.kubernetes.api.model.Quantity;
import io.fabric8.kubernetes.api.model.ResourceRequirements;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class ProcessingElementSpec implements KubernetesResource {

  /*
   * Misc.
   */
  @Getter @Setter private BigInteger id;
  @Getter @Setter private String contentId;
  @Getter @Setter private String appTraceLevel;
  @Getter @Setter private String image;
  @Getter @Setter private String imagePullPolicy;
  @Getter @Setter private String imagePullSecret;
  @Getter @Setter private String runtimeTraceLevel;
  @Getter @Setter private String bundleName;
  @Getter @Setter private String transportSecurityType;
  @Getter @Setter private ResourceRequirements resources;
  /*
   * Debug options.
   */
  @Getter @Setter private boolean allowPtrace;
  @Getter @Setter private boolean runWithValgrind;
  @Getter @Setter private String buildType;
  /*
   * Liveness probe configuration.
   */
  @Getter @Setter private Integer initialProbeDelayInSeconds;
  @Getter @Setter private Integer probeFailureThreshold;
  @Getter @Setter private Integer probeIntervalInSeconds;
  /*
   * Restart/delete policies.
   */
  @Getter @Setter private boolean restartCompletedPod;
  @Getter @Setter private boolean restartDeletedPod;
  @Getter @Setter private Boolean restartFailedPod;
  @Getter @Setter private boolean deleteFailedPod;
  /*
   * PE volumes.
   */
  @Getter @Setter private String dataPath;
  @Getter @Setter private DataVolumeClaimSpec dataVolumeClaim;
  /*
   * Checkpointing data store.
   */
  @Getter @Setter private CheckpointDataStoreSpec checkpointDataStore;
  /*
   * Properties
   */
  @Getter @Setter private String appPropertiesSecret;
  @Getter @Setter private String restrictedPropertiesSecret;
  /*
   * Host placement.
   */
  @Getter @Setter private PlacementSpec placement;
  /*
   * Processing element state
   */
  @Getter @Setter private Connectivity connectivity;
  @Getter @Setter private int launchCount;
  @Getter @Setter private Set<String> operators;
  @Getter @Setter private Set<Integer> regions;

  private static final Map<String, Quantity> defaultRequests =
      new HashMap<String, Quantity>() {
        {
          put("cpu", new Quantity("100m"));
          put("memory", new Quantity("128Mi"));
        }
      };

  public ProcessingElementSpec() {
    /*
     * Misc.
     */
    this.imagePullPolicy = "Always";
    this.runtimeTraceLevel = "INFO";
    this.transportSecurityType = "none";
    this.resources = new ResourceRequirements(new HashMap<String, Quantity>(), defaultRequests);
    /*
     * Debug.
     */
    this.allowPtrace = false;
    this.runWithValgrind = false;
    /*
     * Probe.
     */
    this.initialProbeDelayInSeconds = 30;
    this.probeFailureThreshold = 1;
    this.probeIntervalInSeconds = 10;
    /*
     * Policies.
     */
    this.deleteFailedPod = true;
    this.restartCompletedPod = false;
    this.restartDeletedPod = false;
    this.restartFailedPod = null; // null indicates the option was not specified
    /*
     * Host placement.
     */
    this.placement = new PlacementSpec();
    /*
     * State.
     */
    this.connectivity = Connectivity.None;
    this.launchCount = 0;
    this.operators = new HashSet<>();
  }

  public ProcessingElementSpec(ProcessingElementSpec spec) {
    /*
     * Misc.
     */
    this.id = spec.id;
    this.contentId = spec.contentId;
    this.appTraceLevel = spec.appTraceLevel;
    this.image = spec.image;
    this.imagePullPolicy = spec.imagePullPolicy;
    this.imagePullSecret = spec.imagePullSecret;
    this.runtimeTraceLevel = spec.runtimeTraceLevel;
    this.bundleName = spec.bundleName;
    this.transportSecurityType = spec.transportSecurityType;
    this.resources =
        new ResourceRequirements(spec.resources.getLimits(), spec.resources.getRequests());
    /*
     * Debug.
     */
    this.allowPtrace = spec.allowPtrace;
    this.runWithValgrind = spec.runWithValgrind;
    this.buildType = spec.buildType;
    /*
     * Probe.
     */
    this.initialProbeDelayInSeconds = spec.initialProbeDelayInSeconds;
    this.probeIntervalInSeconds = spec.probeIntervalInSeconds;
    this.probeFailureThreshold = spec.probeFailureThreshold;
    /*
     * Policies.
     */
    this.deleteFailedPod = spec.deleteFailedPod;
    this.restartCompletedPod = spec.restartCompletedPod;
    this.restartDeletedPod = spec.restartDeletedPod;
    this.restartFailedPod = spec.restartFailedPod;
    /*
     * Volumes.
     */
    this.dataPath = spec.dataPath;
    this.dataVolumeClaim =
        spec.dataVolumeClaim != null ? new DataVolumeClaimSpec(spec.dataVolumeClaim) : null;
    /*
     * Checkpointing.
     */
    this.checkpointDataStore =
        spec.checkpointDataStore != null
            ? new CheckpointDataStoreSpec(spec.checkpointDataStore)
            : null;
    /*
     * Properties.
     */
    this.appPropertiesSecret = spec.appPropertiesSecret;
    this.restrictedPropertiesSecret = spec.restrictedPropertiesSecret;
    /*
     * Host placement.
     */
    this.placement = new PlacementSpec(spec.placement);
    /*
     * State.
     */
    this.connectivity = spec.connectivity;
    this.launchCount = spec.launchCount;
    this.operators = new HashSet<>(spec.getOperators());
    this.regions = spec.regions != null ? new HashSet<>(spec.regions) : null;
  }

  public Optional<Boolean> restartFailedPod() {
    return Optional.ofNullable(this.restartFailedPod);
  }

  public boolean hasTransportSecurity() {
    return !this.transportSecurityType.equals("none");
  }

  public Optional<String> buildType() {
    return Optional.ofNullable(this.buildType);
  }
}
