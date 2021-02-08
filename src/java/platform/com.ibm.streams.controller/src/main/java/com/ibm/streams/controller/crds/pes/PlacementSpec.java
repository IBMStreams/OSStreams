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
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import java.util.HashSet;
import java.util.Optional;
import java.util.Set;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class PlacementSpec implements KubernetesResource {
  @Getter @Setter private String host;
  @Getter @Setter private Set<String> coLocations;
  @Getter @Setter private Set<String> exLocations;

  /*
   * If present, the string value is the name of the operator that requested
   * host isolation. We use the name when generating labels and anti-affinities.
   * Null indicates no isolation.
   */
  @Getter @Setter private String isolation;

  /*
   * Our isolation labels are from isolation requests on OTHER PEs. That is, the
   * operator names in this set requested to be isolated, so we need to create
   * labels based on their name to make sure their anti-affinity spec excludes
   * us.
   */
  @Getter @Setter private Set<String> isolationLabels;

  /*
   * Null indicates no host pool location.
   */
  @Getter @Setter private PoolLocation hostPoolLocation;

  public PlacementSpec() {
    this.host = null;
    this.coLocations = new HashSet<>();
    this.exLocations = new HashSet<>();
    this.isolation = null;
    this.isolationLabels = new HashSet<>();
    this.hostPoolLocation = null;
  }

  public PlacementSpec(PlacementSpec spec) {
    this.host = spec.host;
    this.coLocations = new HashSet<>(spec.coLocations);
    this.exLocations = new HashSet<>(spec.exLocations);
    this.isolation = spec.isolation;
    this.isolationLabels = new HashSet<>(spec.isolationLabels);
    this.hostPoolLocation =
        spec.hostPoolLocation != null ? new PoolLocation(spec.hostPoolLocation) : null;
  }

  public Optional<String> host() {
    return Optional.ofNullable(this.host);
  }
}
