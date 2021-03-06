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

package com.ibm.streams.instance.sam.model.runtime;

public enum RoutingNotificationType {
  ADD(0),
  DEL(1),
  UPDATE(2);

  private final int value;

  private RoutingNotificationType(final int newValue) {
    value = newValue;
  }

  public int getValue() {
    return value;
  }

  public static RoutingNotificationType getEnum(int val) {
    RoutingNotificationType[] states = RoutingNotificationType.values();
    for (RoutingNotificationType state : states) {
      if (state.getValue() == val) return state;
    }
    return states[0];
  }
}
