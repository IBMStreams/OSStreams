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

package com.ibm.streams.controller.crds.pes.consistent;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CR_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.consistent.utils.ENotificationType;
import com.ibm.streams.controller.consistent.utils.Notification;
import com.ibm.streams.controller.crds.crs.consistent.ConsistentRegionController;
import com.ibm.streams.controller.crds.pes.Connectivity;
import com.ibm.streams.controller.crds.pes.DoneableProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementFactory;
import com.ibm.streams.controller.crds.pes.ProcessingElementList;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.util.Set;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ProcessingElementController extends GenericEventQueueConsumer<ProcessingElement> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ProcessingElementController.class);

  private final ConsistentRegionController crController;
  private final Controller<ProcessingElement> controller;

  public ProcessingElementController(
      KubernetesClient client,
      ConsistentRegionController crController,
      ProcessingElementStore peStore,
      String ns,
      String jobName) {
    /*
     * Super constructor.
     */
    super(peStore);
    /*
     * Save the parameters.
     */
    this.crController = crController;
    /*
     * Create the controller.
     */
    controller =
        new Controller<>(
            client
                .customResources(
                    ProcessingElementFactory.getContext(client),
                    ProcessingElement.class,
                    ProcessingElementList.class,
                    DoneableProcessingElement.class)
                .inNamespace(ns)
                .withLabel(STREAMS_CR_LABEL_KEY, "true")
                .withLabel(STREAMS_JOB_LABEL_KEY, jobName),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends ProcessingElement> event) {
    /*
     * The monitor is created after pe crd created, we would like to listen to all events
     * Return if the PE is not in consistent region
     */
    ProcessingElement cur = event.getResource();
    /*
     * Check PE connectivity
     */
    updatePeConnectivity(cur);
  }

  @Override
  public void onModification(AbstractEvent<? extends ProcessingElement> event) {
    ProcessingElement pri = event.getPriorResource();
    ProcessingElement cur = event.getResource();
    /*
     * Check PE connectivity
     */
    Connectivity priConn = pri.getSpec().getConnectivity();
    Connectivity curConn = cur.getSpec().getConnectivity();
    if (!priConn.equals(curConn)) {
      updatePeConnectivity(cur);
    }
  }

  @Override
  public void onDeletion(AbstractEvent<? extends ProcessingElement> event) {
    /*
     * Grab the resource
     */
    ProcessingElement pe = event.getResource();
    updatePeConnectivity(pe);
  }

  private void updatePeConnectivity(ProcessingElement pe) {
    Set<Integer> regions = pe.getSpec().getRegions();
    if (regions != null) {
      LOGGER.info(
          "update regions pe " + pe.getSpec().getId() + " conn " + pe.getSpec().getConnectivity());
      regions.forEach(this::updatePeConnForRegion);
    }
  }

  private void updatePeConnForRegion(Integer regionIndex) {
    Notification n = new Notification();
    n.setType(ENotificationType.PeConnectivity);
    n.setRegionIndex(regionIndex);
    crController.addNotification(n);
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
