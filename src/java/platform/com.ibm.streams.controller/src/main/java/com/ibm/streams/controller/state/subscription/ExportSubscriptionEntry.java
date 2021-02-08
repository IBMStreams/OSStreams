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
import com.ibm.streams.controller.crds.exports.Export;
import java.math.BigInteger;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class ExportSubscriptionEntry {

  @Getter @Setter private BigInteger jobId;
  @Getter @Setter private BigInteger peId;
  @Getter @Setter private BigInteger portId;

  ExportSubscriptionEntry(Export exp) {
    this.jobId = exp.getSpec().getJobId();
    this.peId = exp.getSpec().getPeId();
    this.portId = exp.getSpec().getPortId();
  }
}
