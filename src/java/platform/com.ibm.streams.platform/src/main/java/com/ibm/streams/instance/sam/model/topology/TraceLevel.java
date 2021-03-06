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

public enum TraceLevel {
  OFF("off"),
  ERROR("error"),
  WARN("warn"),
  INFO("info"),
  DEBUG("debug"),
  TRACE("trace");

  private final String value;

  TraceLevel(String v) {
    value = v;
  }

  public static TraceLevel fromValue(String value) {
    for (TraceLevel lev : TraceLevel.values()) {
      if (lev.value.equals(value)) return lev;
    }
    throw new IllegalArgumentException(value);
  }

  public String getValue() {
    return value;
  }

  @Override
  public String toString() {
    return getValue();
  }
}
