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

package com.ibm.streams.controller.crds.crs.instance;

import com.ibm.streams.controller.crds.crs.ConsistentRegion;
import com.ibm.streams.controller.crds.crs.ConsistentRegionFactory;
import com.ibm.streams.controller.crds.crs.ConsistentRegionList;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.DoneableConsistentRegion;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConsistentRegionController extends GenericEventQueueConsumer<ConsistentRegion> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ConsistentRegionController.class);

  private final Controller<ConsistentRegion> controller;

  public ConsistentRegionController(
      KubernetesClient client,
      ConsistentRegionStore crStore,
      ConsistentRegionFactory crFactory,
      String ns) {
    /*
     * Super constructor.
     */
    super(crStore);
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .customResources(
                    crFactory.getContext(),
                    ConsistentRegion.class,
                    ConsistentRegionList.class,
                    DoneableConsistentRegion.class)
                .inNamespace(ns),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends ConsistentRegion> event) {}

  @Override
  public void onModification(AbstractEvent<? extends ConsistentRegion> event) {}

  @Override
  public void onDeletion(AbstractEvent<? extends ConsistentRegion> event) {}

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
