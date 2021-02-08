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

package com.ibm.streams.controller.consistent.rest;

import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.consistent.ConsistentRegionController;
import java.util.concurrent.ConcurrentHashMap;
import org.glassfish.hk2.utilities.binding.AbstractBinder;

public class ConsistentRegionBinder extends AbstractBinder {

  private final ConsistentRegionNotificationBoard crNotificationBoard;
  private final ConsistentRegionController crController;
  private final String jobName;
  private final ConsistentRegionStore crStore;
  private final Integer numRegions;

  public ConsistentRegionBinder(
      ConsistentRegionController crController,
      ConsistentRegionNotificationBoard crNotificationBoard,
      String jobName,
      ConsistentRegionStore crStore,
      Integer numRegions) {
    this.crController = crController;
    this.crNotificationBoard = crNotificationBoard;
    this.jobName = jobName;
    this.crStore = crStore;
    this.numRegions = numRegions;
  }

  @Override
  protected void configure() {
    bind(crNotificationBoard).to(ConcurrentHashMap.class);
    bind(crController).to(ConsistentRegionController.class);
    bind(jobName).to(String.class);
    bind(crStore).to(ConsistentRegionStore.class);
    bind(numRegions).to(Integer.class);
  }
}
