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

public enum PeStateReason {
  AUTOMATIC(1),
  CRASH(2),
  FAILURE(3),
  HOSTFAILURE(4),
  NONE(5),
  REQUEST(6),
  VOLUNTARY(7),
  NOFEASIBLEHOST(8),
  SETCAPABILITIESFAILURE(9),
  NOTRESTARTABLEFAILURE(10);

  private final int value;

  private PeStateReason(final int newValue) {
    value = newValue;
  }

  public int getValue() {
    return value;
  }

  public static PeStateReason getEnum(int val) {
    PeStateReason[] reasons = PeStateReason.values();
    for (PeStateReason reason : reasons) {
      if (reason.getValue() == val) return reason;
    }
    return reasons[0];
  }
}
