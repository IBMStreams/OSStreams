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

import com.ibm.streams.controller.consistent.rest.services.ConsistentRegionService;
import com.ibm.streams.controller.consistent.rest.services.ConsistentRegionTestService;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.consistent.ConsistentRegionController;
import com.ibm.streams.controller.instance.rest.NotFoundExceptionProvider;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.server.ResourceConfig;

public class ConsistentRegionResourceConfig extends ResourceConfig {

  public ConsistentRegionResourceConfig(
      ConsistentRegionController crController,
      ConsistentRegionNotificationBoard crNotificationBoard,
      String jobName,
      ConsistentRegionStore crStore,
      Integer numRegions) {
    register(JacksonFeature.class);
    register(ConsistentRegionService.class);
    register(ConsistentRegionTestService.class);
    register(
        new ConsistentRegionBinder(
            crController, crNotificationBoard, jobName, crStore, numRegions));
    register(NotFoundExceptionProvider.class);
  }
}
