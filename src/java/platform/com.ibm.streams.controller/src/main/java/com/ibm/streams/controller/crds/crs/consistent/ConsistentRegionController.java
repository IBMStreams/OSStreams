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

package com.ibm.streams.controller.crds.crs.consistent;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.consistent.ConsistentRegionStateMachine;
import com.ibm.streams.controller.consistent.rest.ConsistentRegionNotificationBoard;
import com.ibm.streams.controller.consistent.utils.ConsistentRegionOperatorMetric;
import com.ibm.streams.controller.consistent.utils.EMetric;
import com.ibm.streams.controller.consistent.utils.Notification;
import com.ibm.streams.controller.crds.crs.ConsistentRegion;
import com.ibm.streams.controller.crds.crs.ConsistentRegionFactory;
import com.ibm.streams.controller.crds.crs.ConsistentRegionList;
import com.ibm.streams.controller.crds.crs.ConsistentRegionSpec;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.DoneableConsistentRegion;
import com.ibm.streams.controller.crds.crs.EConsistentRegionState;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.k8s.pods.PodStore;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.IOException;
import java.util.concurrent.ConcurrentLinkedDeque;
import java.util.concurrent.atomic.AtomicBoolean;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConsistentRegionController extends GenericEventQueueConsumer<ConsistentRegion> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ConsistentRegionController.class);

  private final Controller<ConsistentRegion> controller;
  private final ConcurrentLinkedDeque<Notification> notificationQueue;
  private final AtomicBoolean fsmSignal;
  private final AtomicBoolean waitOnModification;
  private final ConsistentRegionStateMachine crFsm;
  private final ConsistentRegionOperatorMetric crMetrics;
  private final ConsistentRegionFactory crFactory;
  private final boolean isRestart;
  private final Integer numRegions;
  private Integer numAddedRegions;
  private boolean startUp;

  public ConsistentRegionController(
      boolean isRestart,
      KubernetesClient client,
      ConsistentRegionFactory crFactory,
      ConsistentRegionStore crStore,
      ProcessingElementStore peStore,
      PodStore podStore,
      ConsistentRegionNotificationBoard crNotificationBoard,
      ConsistentRegionOperatorMetric crMetrics,
      String ns,
      String jobName,
      String jobId,
      Integer numRegions) {
    /*
     * Super constructor.
     */
    super(crStore);
    /*
     * Initialize internal state.
     */
    this.numRegions = numRegions;
    this.isRestart = isRestart;
    startUp = true;
    numAddedRegions = 0;
    this.crMetrics = crMetrics;
    /*
     * Initialize and run the consistent region FSM
     */
    this.notificationQueue = new ConcurrentLinkedDeque<>();
    this.fsmSignal = new AtomicBoolean(false);
    this.waitOnModification = new AtomicBoolean(true);
    this.crFsm =
        new ConsistentRegionStateMachine(
            this,
            fsmSignal,
            waitOnModification,
            jobName,
            jobId,
            crStore,
            crFactory,
            peStore,
            podStore,
            crNotificationBoard,
            notificationQueue,
            numRegions,
            ns);
    Thread fsmThread = new Thread(this.crFsm);
    fsmThread.start();

    this.crFactory = crFactory;

    if (isRestart) {
      LOGGER.info("consistent region restarted");
    } else {
      LOGGER.info("consistent region fresh start");
    }

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
                .inNamespace(ns)
                .withLabel(STREAMS_JOB_LABEL_KEY, jobName),
            this);
  }

  @Override
  public void onAddition(AbstractEvent<? extends ConsistentRegion> event) {
    /*
     * Get the event resource.
     */
    ConsistentRegion cr = event.getResource();
    updateMetric(cr.getSpec());
    numAddedRegions++;

    if (isRestart) {
      crFactory.updateConsistentRegion(cr, EConsistentRegionState.STARTED, false);
    } else {
      if (numAddedRegions == numRegions) {
        synchronized (fsmSignal) {
          startUp = false;
          waitOnModification.set(false);
          if (!notificationQueue.isEmpty()) {
            if (fsmSignal.compareAndSet(false, true)) {
              fsmSignal.notify();
            }
          }
        }
      }
    }
  }

  @Override
  public void onModification(AbstractEvent<? extends ConsistentRegion> event) {
    /*
     * Update Metric
     * 1. Do we really need to check the difference in metric value between the current CR and previous CR CRD, the
     * update is simple, while the comparison may just overweigh the update cost.
     * 2. Also we only update metric after CR CRD has been securely updated for consistency concern.
     * 3. The notification to runtime PE also happens here.
     */
    ConsistentRegion cur = event.getResource();
    updateMetric(cur.getSpec());
    if (startUp) {
      numAddedRegions++;
      if (numAddedRegions == 2 * numRegions) {
        /*
         * first time trigger the crFsm
         */
        synchronized (fsmSignal) {
          startUp = false;
          waitOnModification.set(false);
          if (!notificationQueue.isEmpty()) {
            if (fsmSignal.compareAndSet(false, true)) {
              fsmSignal.notify();
            }
          }
        }
      }
    } else {
      crFsm.sendPendingNotification(cur.getSpec());
      synchronized (fsmSignal) {
        waitOnModification.set(false);
        if (!notificationQueue.isEmpty()) {
          if (fsmSignal.compareAndSet(false, true)) {
            fsmSignal.notify();
          }
        }
      }
    }
  }

  @Override
  public void onDeletion(AbstractEvent<? extends ConsistentRegion> event) {}

  public void addNotification(Notification n) {
    synchronized (fsmSignal) {
      notificationQueue.add(n);
      if (!waitOnModification.get()) {
        if (fsmSignal.compareAndSet(false, true)) {
          fsmSignal.notify();
        }
      }
    }
  }

  public boolean hasRestarted() {
    return isRestart;
  }

  private void updateMetric(ConsistentRegionSpec spec) {
    Integer regionIndex = spec.getRegionIndex();
    crMetrics.updateMetric(regionIndex, EMetric.STATE, Long.valueOf(spec.getState().ordinal()));
    crMetrics.updateMetric(regionIndex, EMetric.AVERAGE_DRAIN_TIME, spec.getAvgDrainTimeMetric());
    crMetrics.updateMetric(regionIndex, EMetric.AVERAGE_RESET_TIME, spec.getAvgResetTimeMetric());
    crMetrics.updateMetric(
        regionIndex, EMetric.LAST_COMPLETED_DRAIN_SEQ_ID, spec.getLastCompletedDrain());
    crMetrics.updateMetric(
        regionIndex, EMetric.LAST_COMPLETED_RESET_SEQ_ID, spec.getLastCompletedReset());
    crMetrics.updateMetric(
        regionIndex, EMetric.LAST_CONSISTENT_STATE_TIME, spec.getLastConsistentStateTime());
    crMetrics.updateMetric(regionIndex, EMetric.LAST_RESET_TIME, spec.getLastResetTime());
    crMetrics.updateMetric(
        regionIndex, EMetric.CURRENT_RESET_ATTEMPT, spec.getCurrentResetAttempt());
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    /*
     * terminate the crFsm
     */
    crFsm.terminate();
    synchronized (fsmSignal) {
      if (fsmSignal.compareAndSet(false, true)) {
        fsmSignal.notify();
      }
    }
    controller.close();
  }
}
