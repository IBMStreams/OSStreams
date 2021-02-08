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

import lombok.Getter;
import lombok.Setter;

public class RedisDataStoreSpec {

  @Getter @Setter boolean shuffle;
  @Getter @Setter Integer replicaCount;
  @Getter @Setter Integer shardCount;
  @Getter @Setter String service;
  @Getter @Setter String restrictedProperty;

  public RedisDataStoreSpec() {
    this.shuffle = false;
  }

  public RedisDataStoreSpec(RedisDataStoreSpec spec) {
    this.replicaCount = spec.replicaCount;
    this.shardCount = spec.shardCount;
    this.service = spec.service;
    this.restrictedProperty = spec.restrictedProperty;
  }
}
