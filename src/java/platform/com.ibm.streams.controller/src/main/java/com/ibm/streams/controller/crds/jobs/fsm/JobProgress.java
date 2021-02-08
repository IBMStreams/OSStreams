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

package com.ibm.streams.controller.crds.jobs.fsm;

import java.time.Duration;
import java.time.Instant;

class JobProgress {

  private final Instant creationTime;
  private final Duration jobTimeOut;
  private EJobState state;

  JobProgress(Duration jobTimeOut, EJobState state) {
    this.creationTime = Instant.now();
    this.jobTimeOut = jobTimeOut;
    this.state = state;
  }

  boolean hasTimedOut() {
    if (state == EJobState.SUBMITTED) {
      return false;
    }
    return Duration.between(this.creationTime, Instant.now()).compareTo(jobTimeOut) >= 0;
  }

  public void reset() {
    this.state = EJobState.SUBMITTING;
  }

  EJobState next() {
    switch (this.state) {
      case SUBMITTING:
        this.state = EJobState.WAIT_FOR_CONFIG_MAPS;
        break;
      case WAIT_FOR_CONFIG_MAPS:
        this.state = EJobState.WAIT_FOR_SERVICES;
        break;
      case WAIT_FOR_SERVICES:
        this.state = EJobState.WAIT_FOR_HOSTPOOLS;
        break;
      case WAIT_FOR_HOSTPOOLS:
        this.state = EJobState.WAIT_FOR_EXPORTS;
        break;
      case WAIT_FOR_EXPORTS:
        this.state = EJobState.WAIT_FOR_IMPORTS;
        break;
      case WAIT_FOR_IMPORTS:
        this.state = EJobState.WAIT_FOR_CONSISTENT_REGION_OPERATOR;
        break;
      case WAIT_FOR_CONSISTENT_REGION_OPERATOR:
        this.state = EJobState.WAIT_FOR_CONSISTENT_REGIONS;
        break;
      case WAIT_FOR_CONSISTENT_REGIONS:
        this.state = EJobState.WAIT_FOR_PARALLEL_REGIONS;
        break;
      case WAIT_FOR_PARALLEL_REGIONS:
        this.state = EJobState.WAIT_FOR_PES;
        break;
      case WAIT_FOR_PES:
        this.state = EJobState.WAIT_FOR_PODS;
        break;
      case WAIT_FOR_PODS:
        this.state = EJobState.DELETE_PES;
        break;
      case DELETE_PES:
        this.state = EJobState.WAIT_FOR_DELETED_PES;
        break;
      case WAIT_FOR_DELETED_PES:
        this.state = EJobState.DELETE_IMPORTS;
        break;
      case DELETE_IMPORTS:
        this.state = EJobState.WAIT_FOR_DELETED_IMPORTS;
        break;
      case WAIT_FOR_DELETED_IMPORTS:
        this.state = EJobState.DELETE_EXPORTS;
        break;
      case DELETE_EXPORTS:
        this.state = EJobState.WAIT_FOR_DELETED_EXPORTS;
        break;
      case WAIT_FOR_DELETED_EXPORTS:
        this.state = EJobState.DELETE_SERVICES;
        break;
      case DELETE_SERVICES:
        this.state = EJobState.WAIT_FOR_DELETED_SERVICES;
        break;
      case WAIT_FOR_DELETED_SERVICES:
        this.state = EJobState.DELETE_CONFIG_MAPS;
        break;
      case DELETE_CONFIG_MAPS:
        this.state = EJobState.WAIT_FOR_DELETED_CONFIG_MAPS;
        break;
      case WAIT_FOR_DELETED_CONFIG_MAPS:
        this.state = EJobState.SUBMITTED;
        break;
      case SUBMITTED:
        break;
    }
    return this.state;
  }

  public EJobState getState() {
    return this.state;
  }
}
