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

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;
import java.util.Arrays;

public enum EFusionType {
  NoChannelInfluence("noChannelInfluence"),
  ChannelIsolation("channelIsolation"),
  ChannelExlocation("channelExlocation");

  private String value;

  EFusionType(String value) {
    this.value = value;
  }

  @JsonCreator
  public static EFusionType parse(String value) {
    return Arrays.stream(EFusionType.values())
        .filter(e -> e.value.equals(value))
        .findFirst()
        .orElseThrow(IllegalArgumentException::new);
  }

  @JsonValue
  public String getValue() {
    return value;
  }
}
