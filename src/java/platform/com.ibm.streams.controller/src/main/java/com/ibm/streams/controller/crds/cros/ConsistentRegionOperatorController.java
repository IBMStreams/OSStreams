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

package com.ibm.streams.controller.crds.cros;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_GROUP;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CRD_VERSION;

import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.k8s.deployments.DeploymentFactory;
import com.ibm.streams.controller.k8s.services.ServiceFactory;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import com.ibm.streams.controller.utils.CommonEnvironment;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.internal.KubernetesDeserializer;
import java.io.IOException;
import java.time.Instant;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConsistentRegionOperatorController
    extends GenericEventQueueConsumer<ConsistentRegionOperator> {

  private static final Logger LOGGER =
      LoggerFactory.getLogger(ConsistentRegionOperatorController.class);

  private final Instant creationTime;
  private final KubernetesClient client;
  private final ServiceFactory svcFactory;
  private final DeploymentFactory depFactory;
  private final String namespace;
  private final CommonEnvironment env;
  private final Controller<ConsistentRegionOperator> controller;

  public ConsistentRegionOperatorController(
      Instant creationTime,
      KubernetesClient client,
      ConsistentRegionOperatorFactory factory,
      ConsistentRegionOperatorStore croStore,
      ServiceFactory svcFactory,
      DeploymentFactory depFactory,
      String ns,
      CommonEnvironment env) {
    /*
     * Super constructor.
     */
    super(croStore);
    /*
     * Save the client handle.
     */
    this.creationTime = creationTime;
    this.client = client;
    this.svcFactory = svcFactory;
    this.depFactory = depFactory;
    this.namespace = ns;
    this.env = env;
    /*
     * Register the custom kind with the deserializer.
     */
    KubernetesDeserializer.registerCustomKind(
        STREAMS_CRD_GROUP + "/" + STREAMS_CRD_VERSION + "#ConsistentRegionOperator",
        ConsistentRegionOperator.class);
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .customResources(
                    factory.getContext(),
                    ConsistentRegionOperator.class,
                    ConsistentRegionOperatorList.class,
                    DoneableConsistentRegionOperator.class)
                .inNamespace(ns),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends ConsistentRegionOperator> event) {
    /*
     * Get the event resource.
     */
    var cro = event.getResource();
    /*
     * If the event happenend before our creation time, skip.
     */
    if (TimeUtils.happenedBefore(cro, this.creationTime)) {
      return;
    }
    LOGGER.debug("ADD {}", cro.getMetadata().getName());
    /*
     * Add the deployment and service.
     */
    svcFactory.addService(cro);
    depFactory.addDeployment(cro);
  }

  @Override
  public void onModification(AbstractEvent<? extends ConsistentRegionOperator> event) {
    var cro = event.getResource();
    LOGGER.debug("MOD {}", cro.getMetadata().getName());
  }

  @Override
  public void onDeletion(AbstractEvent<? extends ConsistentRegionOperator> event) {
    var cro = event.getResource();
    LOGGER.debug("DEL {}", cro.getMetadata().getName());
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
