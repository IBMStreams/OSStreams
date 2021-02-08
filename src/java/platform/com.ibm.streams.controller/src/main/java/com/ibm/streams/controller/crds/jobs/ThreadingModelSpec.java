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

import java.util.Properties;
import lombok.Getter;
import lombok.Setter;

public class ThreadingModelSpec {

  @Getter @Setter private boolean automatic;
  @Getter @Setter private boolean dedicated;
  @Getter @Setter private DynamicThreadingModelSpec dynamic;
  @Getter @Setter private boolean manual;

  public ThreadingModelSpec() {
    this.automatic = false;
    this.dedicated = false;
    this.manual = false;
  }

  public ThreadingModelSpec(ThreadingModelSpec spec) {
    this.automatic = spec.automatic;
    this.dedicated = spec.dedicated;
    this.manual = spec.manual;
  }

  Properties configurationProperties() {
    Properties cfgs = new Properties();
    if (this.automatic) {
      cfgs.put("threadingModel", "automatic");
    } else if (this.dedicated) {
      cfgs.put("threadingModel", "dedicated");
    } else if (this.dynamic != null) {
      cfgs.put("threadingModel", "dynamic");
      this.dynamic.fillConfigurationProperties(cfgs);
    } else if (this.manual) {
      cfgs.put("threadingModel", "manual");
    }
    return cfgs;
  }
}
