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

package com.ibm.streams.controller.crds.hostpools;

import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class HostPoolController extends GenericEventQueueConsumer<HostPool> {

  private static final Logger LOGGER = LoggerFactory.getLogger(HostPoolController.class);

  private final Controller<HostPool> controller;

  public HostPoolController(
      KubernetesClient client, HostPoolStore store, HostPoolFactory factory, String ns) {
    /*
     * Super constructor.
     */
    super(store);
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .customResources(
                    factory.getContext(),
                    HostPool.class,
                    HostPoolList.class,
                    DoneableHostPool.class)
                .inNamespace(ns),
            this);
  }

  /*
   * TODO:
   *     - support checking the size of hostpools
   *     - support enforcing exclusive hostpools
   */
  @Override
  public void onAddition(AbstractEvent<? extends HostPool> event) {}

  @Override
  public void onModification(AbstractEvent<? extends HostPool> event) {}

  @Override
  public void onDeletion(AbstractEvent<? extends HostPool> event) {}

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
