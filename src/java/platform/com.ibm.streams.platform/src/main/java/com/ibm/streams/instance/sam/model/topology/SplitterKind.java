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

public enum SplitterKind {
  Hash(1),
  RoundRobin(2),
  Broadcast(3);

  private final int value;

  private SplitterKind(final int newValue) {
    value = newValue;
  }

  public int getValue() {
    return value;
  }

  public static SplitterKind getEnum(int val) {
    SplitterKind[] states = SplitterKind.values();
    for (SplitterKind state : states) {
      if (state.getValue() == val) return state;
    }
    return states[0]; // Never should occur
  }
}
