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

package com.ibm.streams.controller.executor;

import lombok.Getter;

public enum EExecutionCommand {
  /*
   * Resource addition.
   */
  ADD_CONFIGMAPS("addConfigMaps"),
  ADD_SERVICES("addServices"),
  ADD_HOST_POOLS("addHostpools"),
  ADD_EXPORTS("addExports"),
  ADD_EXPORT_TO_BROKER("addExportToBroker"),
  ADD_IMPORTS("addImports"),
  ADD_IMPORT_TO_BROKER("addImportToBroker"),
  ADD_CONSISTENT_REGIONS("addConsistentRegions"),
  ADD_PARALLEL_REGIONS("addParallelRegions"),
  ADD_PES("addPes"),
  ADD_POD("addPod"),
  /*
   * Resource update.
   */
  UPDATE_JOB("updateJob"),
  /*
   * Resource deletion.
   */
  DELETE_CONFIGMAPS("deleteConfigMaps"),
  DELETE_SERVICES("deleteServices"),
  DELETE_HOST_POOLS("deleteHostPools"),
  DELETE_EXPORTS("deleteExports"),
  DELETE_IMPORTS("deleteImports"),
  DELETE_CONSISTEMT_REGIONS("deleteConsistentRegions"),
  DELETE_PARALLEL_REGIONS("deleteParallelRegions"),
  DELETE_PES("deletePes");

  @Getter private final String name;

  private EExecutionCommand(String s) {
    name = s;
  }
}
