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

package com.ibm.streams.controller.state.subscription;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.ibm.streams.controller.crds.imports.Import;
import java.math.BigInteger;
import java.util.Optional;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class ImportSubscriptionEntry {

  @Getter @Setter private BigInteger jobId;
  @Getter @Setter private BigInteger peId;
  @Getter @Setter private BigInteger portId;
  @Getter @Setter private BigInteger portIndex;
  @Getter @Setter private String filter;

  public ImportSubscriptionEntry() {}

  public ImportSubscriptionEntry(Import imp) {
    this.jobId = imp.getSpec().getJobId();
    this.peId = imp.getSpec().getPeId();
    this.portId = imp.getSpec().getPortId();
    this.portIndex = imp.getSpec().getPortIndex();
    this.filter = Optional.ofNullable(imp.getSpec().getStreams().getFilter()).orElse("");
  }
}
