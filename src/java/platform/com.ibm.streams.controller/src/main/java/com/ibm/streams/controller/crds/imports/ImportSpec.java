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

package com.ibm.streams.controller.crds.imports;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import java.math.BigInteger;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class ImportSpec implements KubernetesResource {

  @Getter @Setter private BigInteger jobId;
  @Getter @Setter private BigInteger peId;
  @Getter @Setter private BigInteger portId;
  @Getter @Setter private BigInteger portIndex;
  @Getter @Setter private ImportedStreams streams;

  public ImportSpec() {}

  public ImportSpec(ImportSpec spec) {
    jobId = spec.jobId;
    peId = spec.peId;
    portId = spec.portId;
    portIndex = spec.portIndex;
    streams = new ImportedStreams(spec.streams);
  }
}
