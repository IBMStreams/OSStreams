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
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class JobStatisticsSpec implements KubernetesResource {
  @Getter @Setter private long configMapCount;
  @Getter @Setter private long consistentRegionCount;
  @Getter @Setter private long exportCount;
  @Getter @Setter private long hostPoolCount;
  @Getter @Setter private long importCount;
  @Getter @Setter private long parallelRegionCount;
  @Getter @Setter private long processingElementCount;
  @Getter @Setter private long serviceCount;

  public JobStatisticsSpec() {
    configMapCount = 0;
    consistentRegionCount = 0;
    exportCount = 0;
    hostPoolCount = 0;
    importCount = 0;
    parallelRegionCount = 0;
    processingElementCount = 0;
    serviceCount = 0;
  }

  public JobStatisticsSpec(JobStatisticsSpec spec) {
    this.configMapCount = spec.configMapCount;
    this.consistentRegionCount = spec.consistentRegionCount;
    this.exportCount = spec.exportCount;
    this.hostPoolCount = spec.hostPoolCount;
    this.importCount = spec.importCount;
    this.parallelRegionCount = spec.parallelRegionCount;
    this.processingElementCount = spec.processingElementCount;
    this.serviceCount = spec.serviceCount;
  }
}
