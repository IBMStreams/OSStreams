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

package com.ibm.streams.controller.crds.jobs;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import java.util.Optional;
import java.util.Properties;
import lombok.Getter;
import lombok.Setter;
import lombok.var;

@JsonDeserialize
public class FusionSpec implements KubernetesResource {

  @Getter @Setter private EFusionType type;
  @Getter private Boolean automatic;
  @Getter private Boolean legacy;
  @Getter private Integer manual;

  FusionSpec() {
    this.automatic = true;
    this.legacy = null;
    this.manual = null;
  }

  FusionSpec(FusionSpec spec) {
    this.type = spec.type;
    this.automatic = spec.automatic;
    this.legacy = spec.legacy;
    this.manual = spec.manual;
  }

  public Optional<Boolean> automatic() {
    return Optional.ofNullable(this.automatic);
  }

  public Optional<Boolean> legacy() {
    return Optional.ofNullable(this.legacy);
  }

  public Optional<Integer> manual() {
    return Optional.ofNullable(this.manual);
  }

  /*
   * We implement our own setters to ensure mutual exclusion.
   */
  public void setAutomatic(Boolean automatic) {
    this.automatic = automatic;
    if (automatic != null) {
      this.legacy = null;
      this.manual = null;
    }
  }

  public void setLegacy(Boolean legacy) {
    this.legacy = legacy;
    if (legacy != null) {
      this.automatic = null;
      this.manual = null;
    }
  }

  public void setManual(Integer manual) {
    this.manual = manual;
    if (manual != null) {
      this.automatic = null;
      this.legacy = null;
    }
  }

  public Properties configurationProperties() {
    var result = new Properties();
    if (this.type != null && this.legacy == null) {
      result.put("parallelRegionConfig", type.getValue());
    }
    return result;
  }
}
