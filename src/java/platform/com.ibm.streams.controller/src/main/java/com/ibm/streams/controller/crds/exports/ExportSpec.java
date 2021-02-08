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

package com.ibm.streams.controller.crds.exports;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.ibm.streams.instance.sam.model.topology.ExportedStream;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import java.math.BigInteger;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class ExportSpec implements KubernetesResource {

  @Getter @Setter private BigInteger jobId;
  @Getter @Setter private BigInteger peId;
  @Getter @Setter private BigInteger portId;
  @Getter @Setter private EExportedStreamType type;
  @Getter @Setter private ExportedStream stream;

  public ExportSpec() {}

  public ExportSpec(ExportSpec spec) {
    this.jobId = spec.jobId;
    this.peId = spec.peId;
    this.portId = spec.portId;
    this.type = spec.type;
    this.stream = new ExportedStream(spec.stream);
  }
}
