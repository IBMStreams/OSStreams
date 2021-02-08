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
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.instance.utils.EndpointInfo;
import com.ibm.streams.controller.k8s.services.IServiceCoordinator;
import com.ibm.streams.controller.k8s.services.ServiceStore;
import io.fabric8.kubernetes.api.model.Service;
import io.fabric8.kubernetes.api.model.ServicePort;
import java.math.BigInteger;
import java.util.Collection;
import java.util.Map;
import java.util.stream.Collectors;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
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

@Path("/api/endpoints")
public class EndpointsService {

  @Inject private JobStore jobStore;
  @Inject private ProcessingElementStore peStore;
  @Inject private ServiceStore svcStore;
  @Inject private IServiceCoordinator svcCoordinator;

  /*
   * Helper functions.
   */

  private Job getJob(String jobName) {
    return jobStore.getJobWithName(jobName).orElseThrow(IllegalArgumentException::new);
  }

  private ProcessingElement getPe(Job job, BigInteger peId) {
    return peStore.getPeWithJobAndId(job, peId).orElseThrow(NotFoundException::new);
  }

  private Collection<ProcessingElement> getProcessingElements(Job job) {
    return peStore.getPesWithJob(job).values();
  }

  private Service getService(ProcessingElement pe) {
    return svcStore.getServiceForProcessingElement(pe).orElseThrow(NotFoundException::new);
  }

  /*
   * Get endpoints.
   */

  @GET
  @Path("/job/{jobName}/pe/{peId}")
  @Produces(MediaType.APPLICATION_JSON)
  public Map<String, ServicePort> getEndpoints(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") BigInteger peId) {
    var job = getJob(jobName);
    var pe = getPe(job, peId);
    var svc = getService(pe);
    return svc.getSpec().getPorts().stream()
        .collect(Collectors.toMap(ServicePort::getName, e -> e));
  }

  /*
   * Get/set endpoint.
   */

  @GET
  @Path("/job/{jobName}/endpoint/{epName}")
  @Produces(MediaType.APPLICATION_JSON)
  public EndpointInfo getEndpoint(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("epName") String epName) {
    var job = getJob(jobName);
    var pes = getProcessingElements(job);
    var svcs = pes.stream().map(this::getService);
    var name = "usr-" + epName;
    /*
     * Grab the matching service.
     */
    var svc =
        svcs.filter(e -> e.getSpec().getPorts().stream().anyMatch(f -> f.getName().equals(name)))
            .findFirst()
            .orElseThrow(NotFoundException::new);
    /*
     * Grab the matching endpoint.
     */
    var ep =
        svc.getSpec().getPorts().stream()
            .filter(e -> e.getName().equals(name))
            .findFirst()
            .orElseThrow(NotFoundException::new);
    /*
     * Return the endpoint info.
     */
    var canonicalName = svc.getMetadata().getName() + "." + svc.getMetadata().getNamespace();
    return new EndpointInfo(canonicalName, ep.getPort(), ep.getProtocol());
  }

  @PUT
  @Path("/job/{jobName}/pe/{peId}/endpoint/{epName}")
  @Consumes(MediaType.APPLICATION_JSON)
  public void addEndpoint(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") BigInteger peId,
      @PathParam("epName") String epName,
      EndpointInfo info) {
    var job = getJob(jobName);
    var pe = getPe(job, peId);
    var svc = getService(pe);
    var name = "usr-" + epName;
    var port = info.getPort();
    var sp = new ServicePort(null, name, port, port, info.getProtocol(), null);
    svcCoordinator.addUserServicePort(svc, sp);
  }

  @DELETE
  @Path("/job/{jobName}/pe/{peId}/endpoint/{epName}")
  public void deleteEndpoint(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") BigInteger peId,
      @PathParam("epName") String epName) {
    var job = getJob(jobName);
    var pe = getPe(job, peId);
    var svc = getService(pe);
    var name = "usr-" + epName;
    svcCoordinator.deleteUserServicePort(svc, name);
  }
}
