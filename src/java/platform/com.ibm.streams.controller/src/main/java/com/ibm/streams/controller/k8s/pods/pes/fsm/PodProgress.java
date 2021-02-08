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

package com.ibm.streams.controller.k8s.pods.pes.fsm;

import lombok.Getter;

class PodProgress {
  @Getter EPodState state;

  PodProgress() {
    state = EPodState.WAITING_FOR_HOSTPOOLS;
  }

  EPodState next() {
    switch (state) {
      case WAITING_FOR_HOSTPOOLS:
        state = EPodState.WAITING_FOR_CONFIGMAP;
        break;
      case WAITING_FOR_CONFIGMAP:
        state = EPodState.WAITING_FOR_PROCESSINGELEMENT;
        break;
      case WAITING_FOR_PROCESSINGELEMENT:
        state = EPodState.CREATED;
        break;
      case CREATED:
        break;
    }
    return state;
  }

  public void reset() {
    state = EPodState.WAITING_FOR_HOSTPOOLS;
  }
}
