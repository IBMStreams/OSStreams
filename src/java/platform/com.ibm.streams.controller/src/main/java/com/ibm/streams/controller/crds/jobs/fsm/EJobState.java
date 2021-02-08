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

import lombok.Getter;

enum EJobState {
  SUBMITTING("submitting state"),
  SUBMITTED("submitted state"),

  DELETE_PES("deleting processing element"),
  DELETE_IMPORTS("deleting import"),
  DELETE_EXPORTS("deleting export"),
  DELETE_SERVICES("deleting service"),
  DELETE_CONFIG_MAPS("deleting config map"),

  WAIT_FOR_CONFIG_MAPS("config map"),
  WAIT_FOR_SERVICES("service"),
  WAIT_FOR_HOSTPOOLS("host pool"),
  WAIT_FOR_EXPORTS("export"),
  WAIT_FOR_IMPORTS("import"),
  WAIT_FOR_CONSISTENT_REGION_OPERATOR("consistent region operator"),
  WAIT_FOR_CONSISTENT_REGIONS("consistent region"),
  WAIT_FOR_PARALLEL_REGIONS("parallel region"),
  WAIT_FOR_PES("processing element"),
  WAIT_FOR_PODS("pod"),
  WAIT_FOR_DELETED_PES("deleted processing element"),
  WAIT_FOR_DELETED_IMPORTS("deleted import"),
  WAIT_FOR_DELETED_EXPORTS("deleted export"),
  WAIT_FOR_DELETED_SERVICES("deleted service"),
  WAIT_FOR_DELETED_CONFIG_MAPS("deleted config map");

  @Getter private final String description;

  EJobState(String description) {
    this.description = description;
  }
}
