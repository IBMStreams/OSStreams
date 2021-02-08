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

package com.ibm.streams.instance.sam.model.topology;

import static com.google.common.base.Preconditions.checkNotNull;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.platform.services.CheckpointPolicyType;
import com.ibm.streams.platform.services.CheckpointingType;

public class Checkpointing implements Validator {

  private CheckpointPolicy policy;
  private Double period;

  public Checkpointing() {}

  public Checkpointing(CheckpointPolicy policy, Double period) {
    this.policy = policy;
    this.period = period;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<checkpointing policy=\"" + policy.getValue() + "\"");
    if (policy == CheckpointPolicy.PERIODIC) {
      dumper.append(" period=\"" + period.toString() + "\"");
    }
    dumper.append("/>");
    dumper.end();
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNull(policy);
    // Period is optional
  }

  public CheckpointingType toXml() {
    CheckpointingType xml = new CheckpointingType();
    xml.setPolicy(CheckpointPolicyType.valueOf(policy.name()));
    if (policy == CheckpointPolicy.PERIODIC) {
      assert (period != null);
      xml.setPeriod(period);
    }
    return xml;
  }

  public CheckpointPolicy getPolicy() {
    return policy;
  }

  public Double getPeriod() {
    return period;
  }
}
