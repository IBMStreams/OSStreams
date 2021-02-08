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

package com.ibm.streams.controller.consistent.rest.services;

import com.ibm.streams.controller.consistent.rest.ConsistentRegionNotification;
import com.ibm.streams.controller.consistent.rest.ConsistentRegionNotificationBoard;
import com.ibm.streams.controller.consistent.rest.ConsistentRegionNotifications;
import com.ibm.streams.controller.consistent.utils.ENotificationType;
import com.ibm.streams.controller.consistent.utils.EProgressType;
import com.ibm.streams.controller.consistent.utils.Notification;
import com.ibm.streams.controller.consistent.utils.Progress;
import com.ibm.streams.controller.crds.crs.ConsistentRegion;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.EConsistentRegionState;
import com.ibm.streams.controller.crds.crs.consistent.ConsistentRegionController;
import java.math.BigInteger;
import java.util.Map;
import java.util.Optional;
import javax.inject.Inject;
import javax.ws.rs.GET;
import javax.ws.rs.NotFoundException;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Request;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@Path("/api/consistentregion")
public class ConsistentRegionService {

  private static final Logger logger = LoggerFactory.getLogger(ConsistentRegionService.class);

  @Inject private ConsistentRegionNotificationBoard crNotificationBoard;
  @Inject private ConsistentRegionController crController;
  @Inject private String jobName;
  @Inject private ConsistentRegionStore crStore;
  @Inject private Integer numRegions;

  private ConsistentRegion getCR(String jobName, Integer regionIndex) {
    return crStore
        .getWithJobNameAndRegionId(jobName, regionIndex)
        .orElseThrow(IllegalArgumentException::new);
  }

  /*
   * Get the message for a certain region
   */
  @GET
  @Path("/pe/{peId}")
  @Produces(MediaType.APPLICATION_JSON)
  public ConsistentRegionNotifications getNotification(
      @Context Request request, @PathParam("peId") String peId) {
    return Optional.ofNullable(crNotificationBoard.get(new BigInteger(peId)))
        .orElseThrow(NotFoundException::new);
  }

  @GET
  @Path("/pe/{peId}/region/{regionId}")
  @Produces(MediaType.APPLICATION_JSON)
  public ConsistentRegionNotification getNotificationForRegion(
      @Context Request request,
      @PathParam("peId") String peId,
      @PathParam("regionId") String regionId) {
    return Optional.ofNullable(
            crNotificationBoard
                .getOrDefault(new BigInteger(peId), new ConsistentRegionNotifications())
                .get(Integer.parseInt(regionId)))
        .orElseThrow(NotFoundException::new);
  }

  /*
   * Get the state for a certain region
   */
  @GET
  @Path("/region/{regionId}/state")
  @Produces(MediaType.APPLICATION_JSON)
  public EConsistentRegionState getState(
      @Context Request request, @PathParam("regionId") String regionId) {
    ConsistentRegion cr = getCR(jobName, Integer.parseInt(regionId));
    return cr.getSpec().getState();
  }

  /*
   * Get lastCompletedSeqID for a certain region
   */
  @GET
  @Path("/region/{regionId}/lastcompletedseqid")
  @Produces(MediaType.APPLICATION_JSON)
  public Long getLastCompletedSeqID(
      @Context Request request, @PathParam("regionId") String regionId) {
    ConsistentRegion cr = getCR(jobName, Integer.parseInt(regionId));
    return cr.getSpec().getLastCompletedSeqID();
  }

  /*
   * Get trigger for a certain region
   */
  @GET
  @Path("/region/{regionId}/trigger")
  @Produces(MediaType.APPLICATION_JSON)
  public String getTrigger(@Context Request request, @PathParam("regionId") String regionId) {
    ConsistentRegion cr = getCR(jobName, Integer.parseInt(regionId));
    return cr.getSpec().getTrigger();
  }

  /*
   * Get period for a certain region
   */
  @GET
  @Path("/region/{regionId}/period")
  @Produces(MediaType.APPLICATION_JSON)
  public double getPeriod(@Context Request request, @PathParam("regionId") String regionId) {
    ConsistentRegion cr = getCR(jobName, Integer.parseInt(regionId));
    return cr.getSpec().getPeriod();
  }

  /*
   * Get drain timeout for a certain region
   */
  @GET
  @Path("/region/{regionId}/draintimeout")
  @Produces(MediaType.APPLICATION_JSON)
  public double getDrainTimeout(@Context Request request, @PathParam("regionId") String regionId) {
    ConsistentRegion cr = getCR(jobName, Integer.parseInt(regionId));
    return cr.getSpec().getDrainTimeout();
  }

  /*
   * Get reset timeout for a certain region
   */
  @GET
  @Path("/region/{regionId}/resettimeout")
  @Produces(MediaType.APPLICATION_JSON)
  public double getResetTimeout(@Context Request request, @PathParam("regionId") String regionId) {
    ConsistentRegion cr = getCR(jobName, Integer.parseInt(regionId));
    return cr.getSpec().getResetTimeout();
  }

  /*
   * Get reset timeout for a certain region
   */
  @GET
  @Path("/region/{regionId}/maxconsecutiveresetattempts")
  @Produces(MediaType.APPLICATION_JSON)
  public long getMaxConsecutiveResetAttempts(
      @Context Request request, @PathParam("regionId") String regionId) {
    ConsistentRegion cr = getCR(jobName, Integer.parseInt(regionId));
    return cr.getSpec().getMaxConsecutiveResetAttempts();
  }

  /*
   * PE checkpoint completion notifiction (checkpoint done)
   */
  @PUT
  @Path("/region/{regionId}/pe/{peId}/checkpoint")
  public void handlePECheckpointCompletion(
      @Context Request request,
      @PathParam("peId") String peId,
      @PathParam("regionId") String regionId,
      String checkpoint) {

    logger.debug("Checkpoint done " + checkpoint + " pe " + peId);
    Notification n = new Notification();
    n.setType(ENotificationType.Progress);
    Progress p = new Progress();
    p.setPeId(new BigInteger(peId));
    p.setProgressType(EProgressType.CheckpointDone);
    p.setSequenceId(Long.parseLong(checkpoint));
    n.setProgress(p);
    n.setRegionIndex(Integer.parseInt(regionId));
    crController.addNotification(n);
  }

  /*
   * PE checkpoint completion notifiction (block checkpoint done)
   */
  @PUT
  @Path("/region/{regionId}/pe/{peId}/blockingcheckpoint")
  public void handlePEBlockingCheckpointCompletion(
      @Context Request request,
      @PathParam("peId") String peId,
      @PathParam("regionId") String regionId,
      String seqId) {

    Notification n = new Notification();
    n.setType(ENotificationType.Progress);
    Progress p = new Progress();
    p.setPeId(new BigInteger(peId));
    p.setProgressType(EProgressType.BlockingCheckpointDone);
    p.setSequenceId(Long.parseLong(seqId));
    n.setProgress(p);
    n.setRegionIndex(Integer.parseInt(regionId));
    crController.addNotification(n);
  }

  /*
   * PE reset completion notifiction (reset done)
   */
  @PUT
  @Path("/region/{regionId}/pe/{peId}/reset")
  public void handlePEResetCompletion(
      @Context Request request,
      @PathParam("peId") String peId,
      @PathParam("regionId") String regionId,
      Map<String, Map<String, String>> progress) {

    Notification n = new Notification();
    n.setType(ENotificationType.Progress);
    Progress p = new Progress();
    p.setPeId(new BigInteger(peId));
    p.setProgressType(EProgressType.ResetDone);
    logger.debug("Invoke reset done");
    Map<String, String> data =
        Optional.ofNullable(progress.get("progress")).orElseThrow(IllegalArgumentException::new);
    String seqId =
        Optional.ofNullable(data.get("seqId")).orElseThrow(IllegalArgumentException::new);
    String resetAttempt =
        Optional.ofNullable(data.get("resetAttempt")).orElseThrow(IllegalArgumentException::new);
    logger.debug("Reset done " + seqId + " " + resetAttempt);
    p.setSequenceId(Long.parseLong(seqId));
    p.setResetAttempt(Integer.parseInt(resetAttempt));
    n.setProgress(p);
    n.setRegionIndex(Integer.parseInt(regionId));
    crController.addNotification(n);
  }

  /*
   * Operator action (drain)
   */
  @PUT
  @Path("/region/{regionId}/drain")
  public void handleOperatorDrainAction(
      @Context Request request,
      @PathParam("regionId") String regionId,
      Map<String, Map<String, String>> requestInfo) {
    Notification n = new Notification();
    n.setType(ENotificationType.Progress);
    Progress p = new Progress();
    Map<String, String> data =
        Optional.ofNullable(requestInfo.get("request")).orElseThrow(IllegalArgumentException::new);
    String seqId =
        Optional.ofNullable(data.get("seqId")).orElseThrow(IllegalArgumentException::new);
    String opName =
        Optional.ofNullable(data.get("operator")).orElseThrow(IllegalArgumentException::new);
    logger.debug("Invoke drain done");
    p.setOpName(opName);
    p.setSequenceId(Long.parseLong(seqId));
    p.setProgressType(EProgressType.Drain);
    n.setProgress(p);
    n.setRegionIndex(Integer.parseInt(regionId));
    crController.addNotification(n);
  }

  /*
   * Operator action (reset)
   */
  @PUT
  @Path("/region/{regionId}/reset")
  public void handleOperatorResetAction(
      @Context Request request,
      @PathParam("regionId") String regionId,
      Map<String, Map<String, String>> requestInfo) {
    Notification n = new Notification();
    n.setType(ENotificationType.Progress);
    Map<String, String> data =
        Optional.ofNullable(requestInfo.get("request")).orElseThrow(IllegalArgumentException::new);
    Boolean force = Boolean.valueOf(data.get("force"));
    logger.debug("invoke reset " + force);
    Progress p = new Progress();
    p.setForce(force);
    p.setProgressType(EProgressType.Reset);
    n.setProgress(p);
    n.setRegionIndex(Integer.parseInt(regionId));
    crController.addNotification(n);
  }
}
