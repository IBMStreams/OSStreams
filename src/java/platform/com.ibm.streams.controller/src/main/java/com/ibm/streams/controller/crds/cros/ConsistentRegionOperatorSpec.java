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

package com.ibm.streams.controller.crds.cros;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class ConsistentRegionOperatorSpec implements KubernetesResource {

  @Getter @Setter private String jobName;
  @Getter @Setter private String logLevel;
  @Getter @Setter private String imagePullPolicy;
  @Getter @Setter private String imagePullSecret;
  @Getter @Setter private Integer numRegions;
  @Getter @Setter private boolean hasStarted;
  @Getter @Setter private String jobId;

  public ConsistentRegionOperatorSpec() {
    this.imagePullPolicy = "Always";
    this.hasStarted = false;
  }

  public ConsistentRegionOperatorSpec(ConsistentRegionOperatorSpec spec) {
    this.jobName = spec.jobName;
    this.logLevel = "INFO";
    this.imagePullPolicy = spec.imagePullPolicy;
    this.imagePullSecret = spec.imagePullSecret;
    this.numRegions = spec.numRegions;
    this.hasStarted = spec.hasStarted;
    this.jobId = spec.jobId;
  }
}
