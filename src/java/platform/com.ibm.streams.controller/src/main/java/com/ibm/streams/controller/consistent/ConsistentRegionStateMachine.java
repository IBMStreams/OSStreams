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

package com.ibm.streams.controller.consistent;

import static com.ibm.streams.controller.consistent.utils.ConsistentRegionCommons.REGION_DRAINED;
import static com.ibm.streams.controller.consistent.utils.ConsistentRegionCommons.RESET_FAILED;
import static com.ibm.streams.controller.consistent.utils.ConsistentRegionCommons.RESUME_SUBMISSION;
import static com.ibm.streams.controller.consistent.utils.ConsistentRegionCommons.TRIGGER_DRAIN;
import static com.ibm.streams.controller.consistent.utils.ConsistentRegionCommons.TRIGGER_RESET;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_CONSISENT_REGION_PORT;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.ibm.streams.controller.consistent.rest.ConsistentRegionNotification;
import com.ibm.streams.controller.consistent.rest.ConsistentRegionNotificationBoard;
import com.ibm.streams.controller.consistent.utils.ConsistentRegionTimerTask;
import com.ibm.streams.controller.consistent.utils.ETimerEvent;
import com.ibm.streams.controller.consistent.utils.Notification;
import com.ibm.streams.controller.consistent.utils.Progress;
import com.ibm.streams.controller.consistent.utils.RegionInfo;
import com.ibm.streams.controller.consistent.utils.Timeout;
import com.ibm.streams.controller.consistent.utils.Utils;
import com.ibm.streams.controller.crds.crs.ConsistentRegion;
import com.ibm.streams.controller.crds.crs.ConsistentRegionFactory;
import com.ibm.streams.controller.crds.crs.ConsistentRegionSpec;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.EConsistentRegionState;
import com.ibm.streams.controller.crds.crs.EPeStatus;
import com.ibm.streams.controller.crds.crs.consistent.ConsistentRegionController;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.k8s.pods.PodStore;
import java.io.IOException;
import java.math.BigInteger;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.concurrent.ConcurrentLinkedDeque;
import java.util.concurrent.atomic.AtomicBoolean;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConsistentRegionStateMachine implements Runnable {
  private static final Logger logger = LoggerFactory.getLogger(ConsistentRegionStateMachine.class);

  private final ConsistentRegionController crController;
  private final AtomicBoolean fsmSignal;
  private final AtomicBoolean waitOnModification;
  private final ConsistentRegionNotificationBoard crNotificationBoard;
  private final ConcurrentLinkedDeque<Notification> notificationQueue;
  private final String jobName;
  private final String jobId;
  private final ConsistentRegionStore crStore;
  private final ConsistentRegionFactory crFactory;
  private final PodStore podStore;
  private final ProcessingElementStore peStore;
  private ConsistentRegion curCR;
  private ConsistentRegionSpec spec;
  private volatile boolean terminated;
  private Timer[] timers;
  private DatagramSocket socket;
  private Map<Integer, ConsistentRegionNotification> pendingNotifications;
  private String ns;

  public ConsistentRegionStateMachine(
      ConsistentRegionController crController,
      AtomicBoolean fsmSignal,
      AtomicBoolean waitOnModification,
      String jobName,
      String jobId,
      ConsistentRegionStore crStore,
      ConsistentRegionFactory crFactory,
      ProcessingElementStore peStore,
      PodStore podStore,
      ConsistentRegionNotificationBoard crNotificationBoard,
      ConcurrentLinkedDeque<Notification> notificationQueue,
      Integer numRegions,
      String ns) {
    this.crController = crController;
    this.fsmSignal = fsmSignal;
    this.waitOnModification = waitOnModification;
    this.jobName = jobName;
    this.jobId = jobId;
    this.crStore = crStore;
    this.crFactory = crFactory;
    this.peStore = peStore;
    this.podStore = podStore;
    this.crNotificationBoard = crNotificationBoard;
    this.notificationQueue = notificationQueue;
    this.terminated = false;
    this.ns = ns;
    /*
     * Create the one timer per region.
     */
    this.timers = new Timer[numRegions];

    for (Integer i = 0; i < numRegions; i++) {
      timers[i] = new Timer();
    }

    /*
     * pendingNotifications is used to store notification awaiting update of CRCRD
     */
    pendingNotifications = new HashMap<>();

    /*
     * Create the notification socket.
     */
    try {
      this.socket = new DatagramSocket();
    } catch (SocketException ex) {
      logger.error("Cannot create a UDP socket, subscription notifications will be disabled.");
      this.socket = null;
    }
  }

  public void run() {
    while (!terminated) {
      Notification n;
      synchronized (fsmSignal) {
        if (fsmSignal.get()) {
          if (notificationQueue.isEmpty()) {
            fsmSignal.set(false);
          }
        }
        while (!fsmSignal.get()) {
          try {
            fsmSignal.wait();
          } catch (InterruptedException e) {
            e.printStackTrace();
          }
        }
      }
      n = notificationQueue.poll();

      if (n != null) {
        Integer regionIndex = n.getRegionIndex();
        crStore
            .getWithJobNameAndRegionId(jobName, regionIndex)
            .ifPresent(
                cr -> {
                  curCR = cr;
                  spec = null;
                  handleNotification(n);
                  // set signal
                  if (spec != null) {
                    synchronized (fsmSignal) {
                      waitOnModification.set(true);
                      fsmSignal.set(false);
                    }
                    crFactory.addConsistentRegion(cr, spec);
                  }
                });
      } else {
        break;
      }
    }
  }

  public void terminate() {
    terminated = true;
  }

  private void handleNotification(Notification notification) {
    switch (notification.getType()) {
      case PodPhase:
      case PeConnectivity:
        RegionInfo regionInfo = Utils.checkRegion(curCR, peStore, podStore, jobId);
        logger.info(
            "health "
                + regionInfo.isHealthUnknown()
                + " "
                + regionInfo.isRegionHealthy()
                + " "
                + regionInfo.isCleanStart());
        if (regionInfo.isHealthUnknown()) {
          return;
        }
        if (regionInfo.isRegionHealthy() != curCR.getSpec().isRegionHealthy()) {
          spec = new ConsistentRegionSpec(curCR.getSpec());
          spec.setRegionHealthy(regionInfo.isRegionHealthy());
          spec.setCleanStart(regionInfo.isCleanStart());
          handleHealthNotification(regionInfo.isRegionHealthy());
        }
        break;
      case Progress:
        Progress progress = notification.getProgress();
        handleProgressNotification(progress);
        break;
      case Timeout:
        Timeout timeout = notification.getTimeout();
        if (timeout.getTimerSeqId() < curCR.getSpec().getCurrentTimerSeqId()) {
          logger.info("Ignoring " + timeout.getEvent() + " notification");
        } else {
          handleTimeoutNotification(timeout.getEvent());
        }
        break;
    }
  }

  private void handleHealthNotification(boolean health) {
    EConsistentRegionState state = curCR.getSpec().getState();
    if (health) {
      switch (state) {
        case UNHEALTHY:
          resetConsistentRegion();
          break;
        case MAXIMUM_RESET_ATTEMPTS_REACHED:
          logger.error(
              "Job "
                  + jobName
                  + " Region "
                  + curCR.getSpec().getRegionIndex().toString()
                  + " is healthy, but "
                  + "maximum reset "
                  + "attempts "
                  + "reached: not "
                  + "resetting region.");
          break;
        case DRAINING:
        case CHECKPOINT_PENDING:
        case PROCESSING:
        case RESETTING:
          break;
        case STARTED:
          // First health after starting!
          logger.info(
              "Region " + curCR.getSpec().getRegionIndex() + " is healthy for the first time.");
          // If the region is not seen to have had a clean start
          // (all PEs reached Healthy with no issues) then force a reset.
          boolean isCleanStart = spec.isCleanStart();
          if (!isCleanStart) {
            logger.info(
                "Job "
                    + jobName
                    + " Region "
                    + curCR.getSpec().getRegionIndex().toString()
                    + " did not have a clean start.");
            spec.setMustReset(true);
          }
          if (crController.hasRestarted()) {
            logger.info("consistent region operator has been restarted, force a reset");
            spec.setMustReset(true);
          }
          spec.setHealthyFirstTime(true);
          moveToProcessingOrResetOnBootup();
          break;
      }
    } else {
      /*
       * clear the notification board
       */
      curCR
          .getSpec()
          .getPesInRegion()
          .forEach(
              p -> {
                logger.info("clear consistent region notification board");
                crNotificationBoard.clear(p, curCR.getSpec().getRegionIndex());
              });
      switch (state) {
        case MAXIMUM_RESET_ATTEMPTS_REACHED:
        case UNHEALTHY:
        case STARTED: // wait until the region becomes HEALTHY
          break;
        case DRAINING:
        case CHECKPOINT_PENDING:
        case PROCESSING:
        case RESETTING:
          logger.info(
              "Job "
                  + jobName
                  + " Region "
                  + curCR.getSpec().getRegionIndex()
                  + " is unhealthy. Waiting for region to become "
                  + "healthy.");
          spec.setState(EConsistentRegionState.UNHEALTHY);
          clearTimerNotifications(curCR.getSpec().getRegionIndex());
          break;
      }
    }
  }

  private void handleProgressNotification(Progress progress) {
    switch (progress.getProgressType()) {
      case CheckpointDone:
        spec = new ConsistentRegionSpec(curCR.getSpec());
        handleCheckpointDone(progress);
        break;
      case BlockingCheckpointDone:
        spec = new ConsistentRegionSpec(curCR.getSpec());
        handleBlockingCheckpointDone(progress);
        break;
      case ResetDone:
        spec = new ConsistentRegionSpec(curCR.getSpec());
        handleResetDone(progress);
        break;
      case Drain:
        drain(progress.getOpName(), progress.getSequenceId());
        break;
      case Reset:
        reset(progress.getForce());
        break;
    }
  }

  private void handleTimeoutNotification(ETimerEvent event) {
    switch (event) {
      case DrainTimeout:
        logger.trace(
            "Timeout expired for region " + curCR.getSpec().getRegionIndex() + " Drain event");
        if (curCR.getSpec().getState() != EConsistentRegionState.UNHEALTHY) {
          spec = new ConsistentRegionSpec(curCR.getSpec());
          resetConsistentRegion();
        }
        break;
      case ResetTimeout:
        logger.trace(
            "Timeout expired for region " + curCR.getSpec().getRegionIndex() + " Reset event");
        if (curCR.getSpec().getState() != EConsistentRegionState.UNHEALTHY) {
          spec = new ConsistentRegionSpec(curCR.getSpec());
          resetConsistentRegion();
        }
        break;
      case DrainPeriod:
        handleDrainPeriod();
        break;
    }
  }

  private void resetConsistentRegion() {
    Integer regionIndex = spec.getRegionIndex();
    logger.info("Job " + jobName + " Resetting a consistent region " + regionIndex.toString());
    // Cancel all timers
    clearTimerNotifications(regionIndex);

    long currentResetAttempt = spec.getCurrentResetAttempt();
    long currentMaxResetAttempts = spec.getCurrentMaxResetAttempts();
    long lastCompletedSeqID = spec.getLastCompletedSeqID();
    logger.info("reset attemps " + currentResetAttempt + " " + currentMaxResetAttempts);
    if (currentResetAttempt == currentMaxResetAttempts) {
      spec.setState(EConsistentRegionState.MAXIMUM_RESET_ATTEMPTS_REACHED);
      spec.resetPeToCompletion();
      logger.error(
          "Job "
              + jobName
              + " state set to MAXIMUM_RESET_ATTEMPTS_REACHED ("
              + currentMaxResetAttempts
              + ") for region "
              + regionIndex.toString());

      String failMessage = "seqId=" + lastCompletedSeqID;
      sendNotification(RESET_FAILED, failMessage);
      return;
    }
    spec.setState(EConsistentRegionState.RESETTING);
    spec.resetPeToCompletion();
    spec.setCurrentResetAttempt(spec.getCurrentResetAttempt() + 1);

    String restoreMessage = "seqId=" + lastCompletedSeqID + ";resetAttempt=" + currentResetAttempt;
    sendNotification(TRIGGER_RESET, restoreMessage);

    createTimerNotification(
        ETimerEvent.ResetTimeout,
        spec.getResetTimeout(),
        spec.getRegionIndex(),
        spec.getCurrentTimerSeqId());
    spec.setResetStartTimestamp(System.currentTimeMillis());
  }

  private void moveToProcessingOrResetOnBootup() {
    boolean mustReset = spec.isMustReset();
    boolean healthyFirstTime = spec.isHealthyFirstTime();
    String trigger = spec.getTrigger();
    if (healthyFirstTime) {
      if (mustReset) {
        // Honoring request for a reset or recovering from a drain request when the region
        // was not setup yet.
        spec.setMustReset(false);
        resetConsistentRegion();
      } else {
        spec.setState(EConsistentRegionState.PROCESSING);
        if (trigger.equals("periodic")) {
          createTimerNotification(
              ETimerEvent.DrainPeriod,
              spec.getPeriod(),
              spec.getRegionIndex(),
              spec.getCurrentTimerSeqId());
        }
      }
    }
  }

  private void handleCheckpointDone(Progress progress) {
    BigInteger peId = progress.getPeId();
    long sequenceId = progress.getSequenceId();

    logger.debug("checkpointCompleted invoked. peId: " + peId + " seqId: " + sequenceId);

    // Error checks
    if (!spec.getPesInRegion().contains(peId)) {
      return;
    }

    EConsistentRegionState state = spec.getState();
    long currentSeqID = spec.getCurrentSeqID();
    long pendingSeqID = spec.getPendingSeqID();
    if (state == EConsistentRegionState.DRAINING) {
      if (sequenceId != currentSeqID) {
        logger.error("Unexpected seqId processed: " + sequenceId);
        throw new IllegalStateException("Unexpected seqId processed: " + sequenceId);
      }
    } else if (state == EConsistentRegionState.CHECKPOINT_PENDING) {
      if (sequenceId != pendingSeqID) {
        logger.error("Unexpected seqId processed: " + sequenceId);
        throw new IllegalStateException("Unexpected seqId processed: " + sequenceId);
      }
    } else {
      return;
    }

    Map<BigInteger, EPeStatus> peToCompletion = spec.getPeToCompletion();
    if (peToCompletion.get(peId) == EPeStatus.CheckpointDone) {
      logger.error("Duplicate completion notification from PE " + peId);
      throw new IllegalStateException("Duplicate completion notification from PE " + peId);
    }
    // Add PE to completed and remove from the set of PEs that have completed
    // only blocking checkpoint
    peToCompletion.put(peId, EPeStatus.CheckpointDone);

    if (!isCheckpointComplete()) {
      if (isBlockingCheckpointComplete() && state == EConsistentRegionState.DRAINING) {
        finishBlockingCheckpoint(sequenceId);
      }
      return;
    }

    spec.resetPeToCompletion();
    finishCheckpoint();
  }

  private boolean isCheckpointComplete() {
    long numCompletedPEs =
        spec.getPeToCompletion().values().stream()
            .filter(v -> v.equals(EPeStatus.CheckpointDone))
            .count();

    if (numCompletedPEs == spec.getPesInRegion().size()) {
      return true;
    }
    return false;
  }

  private boolean isResetComplete() {
    long numCompletedPEs =
        spec.getPeToCompletion().values().stream()
            .filter(v -> v.equals(EPeStatus.ResetDone))
            .count();

    if (numCompletedPEs == spec.getPesInRegion().size()) {
      return true;
    }
    return false;
  }

  private boolean isBlockingCheckpointComplete() {
    long numCompletedPEs =
        spec.getPeToCompletion().values().stream()
            .filter(
                v ->
                    v.equals(EPeStatus.CheckpointDone)
                        || v.equals(EPeStatus.BlockingCheckpointDone))
            .count();

    if (numCompletedPEs == spec.getPesInRegion().size()) {
      return true;
    }
    return false;
  }

  private void finishBlockingCheckpoint(long sequenceId) {
    spec.setPendingSeqID(sequenceId);
    spec.setCurrentSeqID(spec.getCurrentSeqID() + 1);
    spec.setState(EConsistentRegionState.CHECKPOINT_PENDING);
    String resumeMessage =
        "seqId=" + spec.getCurrentSeqID() + ";retId=" + spec.getToRetireSeqID() + ";pendCkpt=1";
    sendNotification(RESUME_SUBMISSION, resumeMessage);
  }

  public void finishCheckpoint() {
    if (spec.getState() == EConsistentRegionState.DRAINING) {
      spec.setToRetireSeqID(spec.getLastCompletedSeqID());
      spec.setLastCompletedSeqID(spec.getCurrentSeqID());
      spec.setCurrentSeqID(spec.getCurrentSeqID() + 1);
    } else if (spec.getState() == EConsistentRegionState.CHECKPOINT_PENDING) {
      spec.setToRetireSeqID(spec.getLastCompletedSeqID());
      spec.setLastCompletedSeqID(spec.getPendingSeqID());
    }
    spec.setCurrentResetAttempt(0);
    spec.setCurrentMaxResetAttempts(spec.getMaxConsecutiveResetAttempts());

    if (spec.getState() == EConsistentRegionState.DRAINING) {
      // Send notifications
      String resumeMessage =
          "seqId=" + spec.getCurrentSeqID() + ";retId=" + spec.getToRetireSeqID() + ";pendCkpt=0";
      sendNotification(RESUME_SUBMISSION, resumeMessage);
    } else if (spec.getState() == EConsistentRegionState.CHECKPOINT_PENDING) {
      // Send notifications
      String drainedMessage = "seqId=" + spec.getPendingSeqID();
      sendNotification(REGION_DRAINED, drainedMessage);
    }

    spec.setState(EConsistentRegionState.PROCESSING);
    clearTimerNotifications(spec.getRegionIndex());

    spec.setPendingSeqID(spec.getCurrentSeqID());
    if (spec.getTrigger().equals("periodic")) {
      createTimerNotification(
          ETimerEvent.DrainPeriod,
          spec.getPeriod(),
          spec.getRegionIndex(),
          spec.getCurrentTimerSeqId());
    }

    // update all metrics
    final long now = System.currentTimeMillis();
    spec.setLastCompletedDrain(spec.getLastCompletedSeqID());
    spec.setLastConsistentStateTime(now);
    spec.getAvgDrainTime().addSample(now - spec.getDrainStartTimestamp());
    logger.debug("Updating drain avg time: " + spec.getAvgDrainTime().calcAverage());
    spec.setAvgDrainTimeMetric((long) spec.getAvgDrainTime().calcAverage());
  }

  private void handleResetDone(Progress progress) {
    BigInteger peId = progress.getPeId();
    long sequenceId = progress.getSequenceId();
    int resetAttempt = progress.getResetAttempt();
    logger.info("restoreCompleted invoked. PE ID: " + peId + " sequenceId: " + sequenceId);

    // Error checks
    if (!spec.getPesInRegion().contains(peId)) {
      return;
    }
    if (spec.getState() != EConsistentRegionState.RESETTING) {
      return;
    }
    if (sequenceId != spec.getLastCompletedSeqID()
        || resetAttempt != spec.getCurrentResetAttempt() - 1) {
      return;
    }

    if (spec.getPeToCompletion().get(peId).equals(EPeStatus.ResetDone)) {
      logger.error("Duplicate completion notification from PE " + peId);
      throw new IllegalStateException("Duplicate completion notification from PE " + peId);
    }
    spec.getPeToCompletion().put(peId, EPeStatus.ResetDone);

    if (!isResetComplete()) {
      return;
    }

    spec.resetPeToCompletion();
    finishReset();
  }

  private void finishReset() {
    spec.setCurrentSeqID(spec.getCurrentSeqID() + 1);
    clearTimerNotifications(spec.getRegionIndex());
    spec.setState(EConsistentRegionState.PROCESSING);
    String resumeMessage =
        "seqId=" + spec.getCurrentSeqID() + ";retId=" + spec.getToRetireSeqID() + ";pendCkpt=0";
    sendNotification(RESUME_SUBMISSION, resumeMessage);

    if (spec.getTrigger().equals("periodic")) {
      createTimerNotification(
          ETimerEvent.DrainPeriod,
          spec.getPeriod(),
          spec.getRegionIndex(),
          spec.getCurrentTimerSeqId());
    }
    final long now = System.currentTimeMillis();
    spec.setLastCompletedReset(spec.getLastCompletedSeqID());
    spec.setLastResetTime(now);
    spec.getAvgResetTime().addSample(now - spec.getResetStartTimestamp());
    logger.debug("Avg restore time: " + spec.getAvgResetTime().calcAverage());
    spec.setAvgResetTimeMetric((long) spec.getAvgResetTime().calcAverage());
  }

  private void handleBlockingCheckpointDone(Progress progress) {
    BigInteger peId = progress.getPeId();
    long sequenceId = progress.getSequenceId();
    EConsistentRegionState state = spec.getState();
    long currentSeqID = spec.getCurrentSeqID();
    long pendingSeqID = spec.getPendingSeqID();
    logger.debug("blockingCheckpointCompleted invoked. peId: " + peId + " seqId: " + sequenceId);

    if (state == EConsistentRegionState.DRAINING) {
      if (sequenceId != currentSeqID) {
        logger.error("Unexpected seqId processed: " + sequenceId);
        throw new IllegalStateException("Unexpected seqId processed: " + sequenceId);
      }
    } else if (state == EConsistentRegionState.CHECKPOINT_PENDING) {
      if (sequenceId != pendingSeqID) {
        logger.error("Unexpected seqId processed: " + sequenceId);
        throw new IllegalStateException("Unexpected seqId processed: " + sequenceId);
      }
    } else {
      return;
    }

    if (spec.getPeToCompletion().get(peId).equals(EPeStatus.BlockingCheckpointDone)) {
      logger.error("Duplicate completion notification from PE " + peId);
      throw new IllegalStateException("Duplicate completion notification from PE " + peId);
    }

    spec.getPeToCompletion().put(peId, EPeStatus.BlockingCheckpointDone);

    if (!isBlockingCheckpointComplete()) {
      return;
    }

    finishBlockingCheckpoint(sequenceId);
  }

  private void handleDrainPeriod() {
    if (curCR.getSpec().getState() == EConsistentRegionState.PROCESSING) {
      spec = new ConsistentRegionSpec(curCR.getSpec());
      logger.trace("Starting drain");
      String drainMessage = "seqId=" + spec.getCurrentSeqID();
      spec.setState(EConsistentRegionState.DRAINING);
      sendNotification(TRIGGER_DRAIN, drainMessage);
      createTimerNotification(
          ETimerEvent.DrainTimeout,
          spec.getDrainTimeout(),
          spec.getRegionIndex(),
          spec.getCurrentTimerSeqId());
      spec.setDrainStartTimestamp(System.currentTimeMillis());
    } else {
      logger.trace("Ignoring " + ETimerEvent.DrainPeriod + " notification");
    }
  }

  public void drain(String opInstanceName, long sequenceID) {
    logger.info("drain invoked. Operator name: " + opInstanceName + " sequenceId: " + sequenceID);

    // Error checks
    if (curCR.getSpec().getOperatorsToStartRegionMap().get(opInstanceName) == null) {
      return;
    }
    // If request is not coming from the trigger operator, ignore.
    if (!curCR.getSpec().getOperatorsToTriggerMap().get(opInstanceName)) {
      return;
    }
    if (!curCR.getSpec().getTrigger().equals("operator driven")) {
      return;
    }

    if (curCR.getSpec().getState() == EConsistentRegionState.STARTED) {
      // If the region is not yet ready to start processing DrainMarkers, mark the
      // region for a reset once it is stabilized
      spec = new ConsistentRegionSpec(curCR.getSpec());
      spec.setMustReset(true);
      return;
    }
    if (curCR.getSpec().getState() != EConsistentRegionState.PROCESSING) {
      return;
    }
    if (sequenceID != curCR.getSpec().getCurrentSeqID()) {
      logger.error("Unexpected sequenceId started by trigger operator: " + sequenceID);
      throw new IllegalStateException("Unexpected sequenceId started by trigger operator");
    }

    spec = new ConsistentRegionSpec(curCR.getSpec());
    spec.setState(EConsistentRegionState.DRAINING);
    spec.setDrainStartTimestamp(System.currentTimeMillis());
    createTimerNotification(
        ETimerEvent.DrainTimeout,
        spec.getDrainTimeout(),
        spec.getRegionIndex(),
        spec.getCurrentTimerSeqId());
  }

  public void reset(boolean force) {
    logger.info("Reset of a consistent region requested:force=" + force);

    if (force) {
      spec = new ConsistentRegionSpec(curCR.getSpec());
      spec.setCurrentMaxResetAttempts(
          spec.getCurrentResetAttempt() + spec.getMaxConsecutiveResetAttempts());
    }
    switch (curCR.getSpec().getState()) {
      case STARTED:
        if (!force) {
          spec = new ConsistentRegionSpec(curCR.getSpec());
        }
        spec.setMustReset(true);
        break;
      case UNHEALTHY:
        // Region will reset anyway once it becomes healthy.
        break;
      case MAXIMUM_RESET_ATTEMPTS_REACHED:
        if (!force) {
          break;
        }
        RegionInfo regionInfo = Utils.checkRegion(curCR, peStore, podStore, jobId);
        logger.info(
            "health "
                + regionInfo.isHealthUnknown()
                + " "
                + regionInfo.isRegionHealthy()
                + " "
                + regionInfo.isCleanStart());
        if (regionInfo.isHealthUnknown() || !regionInfo.isRegionHealthy()) {
          spec.setState(EConsistentRegionState.UNHEALTHY);
          break;
        }
        spec.setRegionHealthy(regionInfo.isRegionHealthy());
        // If region healthy, fall through
      case DRAINING:
      case CHECKPOINT_PENDING:
      case PROCESSING:
      case RESETTING:
        if (!force) {
          spec = new ConsistentRegionSpec(curCR.getSpec());
        }
        resetConsistentRegion();
        break;
    }
  }

  private void sendNotification(String type, String msg) {
    Integer regionIndex = curCR.getSpec().getRegionIndex();
    ConsistentRegionNotification n = new ConsistentRegionNotification(type, msg);
    pendingNotifications.put(regionIndex, n);
  }

  public void sendPendingNotification(ConsistentRegionSpec spec) {
    Integer regionIndex = spec.getRegionIndex();
    if (pendingNotifications.containsKey(regionIndex)) {
      ConsistentRegionNotification n = pendingNotifications.get(regionIndex);
      spec.getPesInRegion()
          .forEach(
              p -> {
                logger.info(
                    "send out notification for region "
                        + regionIndex
                        + " type "
                        + n.getType()
                        + " message "
                        + n.getMessage());
                crNotificationBoard.add(p, regionIndex, n);
                sendNotification(p, pendingNotifications);
              });
      pendingNotifications.remove(regionIndex);
    }
  }

  private void createTimerNotification(
      ETimerEvent event, Double delay, Integer regionIndex, Integer timerSeqId) {
    ConsistentRegionTimerTask task =
        new ConsistentRegionTimerTask(crController, event, regionIndex, timerSeqId);
    logger.info("create timeout notification " + event + " for region " + regionIndex);
    timers[regionIndex].schedule(task, (new Double(delay * 1000)).longValue());
  }

  public void clearTimerNotifications(Integer regionIndex) {
    logger.info("cancel timeout notification for region " + regionIndex);
    timers[regionIndex].cancel();
    timers[regionIndex] = new Timer();
    spec.setCurrentTimerSeqId(spec.getCurrentTimerSeqId() + 1);
  }

  public String getPeName(BigInteger peId) {
    return jobName + "-" + peId;
  }

  public void sendNotification(BigInteger peId, Map<Integer, ConsistentRegionNotification> n) {
    sendNotification(socket, getPeName(peId) + "." + ns, n);
  }

  public void sendNotification(
      DatagramSocket socket, String target, Map<Integer, ConsistentRegionNotification> n) {
    /*
     * Check if the socket is valid.
     */
    if (socket == null) {
      return;
    }
    /*
     * Send the datagram notification.
     */
    try {
      ObjectMapper mapper = new ObjectMapper();
      String json = mapper.writeValueAsString(n);

      byte[] serializedMessage = json.getBytes();
      InetAddress address = InetAddress.getByName(target);

      DatagramPacket packet =
          new DatagramPacket(
              serializedMessage,
              serializedMessage.length,
              address,
              STREAMS_PE_CONSISENT_REGION_PORT);
      socket.send(packet);
      logger.debug("Routing notification sent to " + target + "message " + serializedMessage);
    } catch (IOException ex) {
      logger.warn("Error sending a notification to " + target);
    }
  }
}
