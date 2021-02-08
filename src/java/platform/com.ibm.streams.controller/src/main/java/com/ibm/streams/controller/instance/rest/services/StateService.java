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

package com.ibm.streams.controller.instance.rest.services;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.pes.Connectivity;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import com.ibm.streams.controller.crds.prs.IParallelRegionCoordinator;
import com.ibm.streams.controller.crds.prs.ParallelRegion;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.state.capture.CaptureConverter;
import com.ibm.xmlns.prod.streams.api.state.Instance;
import java.math.BigInteger;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.NotFoundException;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Request;
import lombok.var;

@Path("/api/state")
public class StateService {

  @Inject private String namespace;
  @Inject private CaptureConverter captureConverter;
  @Inject private JobStore jobStore;
  @Inject private ProcessingElementStore peStore;
  @Inject private IProcessingElementCoordinator peCoordinator;
  @Inject private ParallelRegionStore prStore;
  @Inject private IParallelRegionCoordinator prCoordinator;

  /*
   * Helper functions.
   */

  private Job getJob(String jobName) {
    return jobStore.getJobWithName(jobName).orElseThrow(IllegalArgumentException::new);
  }

  private Job getJob(BigInteger id) {
    return jobStore.getJobWithId(id).orElseThrow(IllegalArgumentException::new);
  }

  private ProcessingElement getPe(Job job, String peId) {
    return peStore.getPeWithJobAndId(job, new BigInteger(peId)).orElseThrow(NotFoundException::new);
  }

  private ParallelRegion getParallelRegion(Job job, String prAlias) {
    return prStore
        .getParallelRegionWithJobAndAlias(job, prAlias)
        .orElseThrow(NotFoundException::new);
  }

  /*
   * Capture state.
   */

  @GET
  @Produces(MediaType.APPLICATION_XML)
  public Instance captureInstanceState(@Context Request request) {
    return captureConverter.captureInstanceState(namespace);
  }

  @GET
  @Path("/job/by-id/{jobId}/name")
  @Produces(MediaType.TEXT_PLAIN)
  public String getJobId(@Context Request request, @PathParam("jobId") BigInteger jobId) {
    var job = getJob(jobId);
    return job.getMetadata().getName();
  }

  /*
   * PE health management.
   */

  @GET
  @Path("/job/{jobName}/pe/{peId}/connectivity")
  @Produces(MediaType.APPLICATION_JSON)
  public Connectivity getHealthForPe(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId) {
    var job = getJob(jobName);
    var pe = getPe(job, peId);
    return pe.getSpec().getConnectivity();
  }

  @PUT
  @Path("/job/{jobName}/pe/{peId}/connectivity")
  @Consumes(MediaType.APPLICATION_JSON)
  public void setHealthForPe(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      Connectivity connectivity) {
    var job = getJob(jobName);
    var pe = getPe(job, peId);
    peCoordinator.updatePeConnectivity(pe, connectivity);
  }

  /*
   * Parallel region management.
   */

  @GET
  @Path("/job/{jobName}/pr/{prId}/width")
  @Produces(MediaType.APPLICATION_JSON)
  public BigInteger getWidthForParallelRegion(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("prId") String prId) {
    var job = getJob(jobName);
    var pr = getParallelRegion(job, prId);
    return pr.getSpec().getWidth();
  }

  @PUT
  @Path("/job/{jobName}/pr/{prAlias}/width")
  @Consumes(MediaType.APPLICATION_JSON)
  public void setWidthForParallelRegion(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("prAlias") String prAlias,
      BigInteger width) {
    var job = getJob(jobName);
    var pr = getParallelRegion(job, prAlias);
    prCoordinator.setParallelRegionWidth(pr, width);
  }
}
