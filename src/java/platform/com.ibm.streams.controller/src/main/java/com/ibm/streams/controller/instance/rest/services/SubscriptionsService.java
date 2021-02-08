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

import com.ibm.streams.controller.crds.exports.EExportedStreamType;
import com.ibm.streams.controller.crds.exports.Export;
import com.ibm.streams.controller.crds.exports.ExportSpec;
import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.exports.IExportCoordinator;
import com.ibm.streams.controller.crds.imports.IImportCoordinator;
import com.ibm.streams.controller.crds.imports.Import;
import com.ibm.streams.controller.crds.imports.ImportSpec;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.state.subscription.SubscriptionBoard;
import com.ibm.streams.controller.state.subscription.Subscriptions;
import com.ibm.streams.instance.sam.SamUtilFunctions;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.platform.services.ImportedStreamsType;
import com.ibm.streams.platform.services.StreamPropertiesType;
import com.ibm.streams.platform.services.StreamPropertyNameType;
import com.ibm.streams.platform.services.StreamPropertyNamesType;
import com.ibm.streams.platform.services.StreamPropertyType;
import java.math.BigInteger;
import java.util.LinkedList;
import java.util.Map;
import java.util.Optional;
import java.util.function.Function;
import java.util.stream.Collectors;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.NotFoundException;
import javax.ws.rs.PATCH;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Request;
import lombok.var;

@Path("/api/subscriptions")
public class SubscriptionsService {

  @Inject private SubscriptionBoard subscriptionBoard;
  @Inject private JobStore jobStore;
  @Inject private ExportStore exportStore;
  @Inject private IExportCoordinator exportCoordinator;
  @Inject private ImportStore importStore;
  @Inject private IImportCoordinator importCoordinator;

  /*
   * Helper functions.
   */

  private Job getJob(String jobName) {
    return jobStore.getJobWithName(jobName).orElseThrow(IllegalArgumentException::new);
  }

  private Export getExport(Job job, String peId, String portId) {
    return exportStore.getExportsWithJob(job).stream()
        .filter(e -> e.getSpec().getPeId().equals(new BigInteger(peId)))
        .filter(e -> e.getSpec().getPortId().equals(new BigInteger(portId)))
        .findAny()
        .orElseThrow(NotFoundException::new);
  }

  private Import getImport(Job job, String peId, String portId) {
    return importStore.getImportsWithJob(job).stream()
        .filter(e -> e.getSpec().getPeId().equals(new BigInteger(peId)))
        .filter(e -> e.getSpec().getPortId().equals(new BigInteger(portId)))
        .findAny()
        .orElseThrow(NotFoundException::new);
  }

  /*
   * Get the subscriptions for a given PE.
   */

  @GET
  @Path("/job/{jobName}/pe/{peId}")
  @Produces(MediaType.APPLICATION_JSON)
  public Subscriptions getSubscriptionsForId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId) {
    return Optional.ofNullable(subscriptionBoard.get(jobName + "-" + peId))
        .orElseThrow(NotFoundException::new);
  }

  /*
   * Get the specs for exports and imports.
   */

  @GET
  @Path("/job/{jobName}/pe/{peId}/export/{portId}")
  @Produces(MediaType.APPLICATION_JSON)
  public ExportSpec getExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId) {
    var job = getJob(jobName);
    return getExport(job, peId, portId).getSpec();
  }

  @GET
  @Path("/job/{jobName}/pe/{peId}/import/{portId}")
  @Produces(MediaType.APPLICATION_JSON)
  public ImportSpec getImportForId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId) {
    var job = getJob(jobName);
    return getImport(job, peId, portId).getSpec();
  }

  /*
   * Export properties manipulation.
   */

  @PUT
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/properties")
  @Consumes(MediaType.APPLICATION_XML)
  public void addPropertiesForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      StreamPropertiesType properties) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    exportCoordinator.addExportProperties(exp, properties);
  }

  @PUT
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/property/{pName}")
  @Consumes(MediaType.APPLICATION_XML)
  public void addPropertyForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      @PathParam("pName") String pName,
      StreamPropertyType property) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    exportCoordinator.addExportProperty(exp, property);
  }

  @PATCH
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/properties")
  @Consumes(MediaType.APPLICATION_XML)
  public void setPropertiesForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      StreamPropertiesType properties) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    exportCoordinator.setExportProperties(exp, properties);
  }

  @PATCH
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/property/{pName}")
  @Consumes(MediaType.APPLICATION_XML)
  public void setPropertyForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      @PathParam("pName") String pName,
      StreamPropertyType property) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    exportCoordinator.setExportProperty(exp, property);
  }

  @POST
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/properties")
  @Consumes(MediaType.APPLICATION_XML)
  @Produces(MediaType.APPLICATION_XML)
  public StreamPropertiesType getPropertiesForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      StreamPropertyNamesType propertyNames) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    /*
     * Check if the export has properties.
     */
    if (exp.getSpec().getType() != EExportedStreamType.ByProperties) {
      throw new NotFoundException();
    }
    /*
     * Check that the name is valid.
     */
    var names =
        Optional.ofNullable(propertyNames).orElseThrow(IllegalArgumentException::new).getName()
            .stream()
            .map(StreamPropertyNameType::getName)
            .collect(Collectors.toSet());
    /*
     * Check if there are any properties.
     */
    var properties =
        Optional.ofNullable(exp.getSpec().getStream().getProperties()).orElseGet(LinkedList::new);
    /*
     * Select the properties. NOTE(xrg): we go through a Map here for backward compatibility with v4.
     */
    Map<String, StreamProperty> propertyMap;
    if (names.isEmpty()) {
      propertyMap =
          properties.stream()
              .collect(Collectors.toMap(StreamProperty::getName, Function.identity()));
    } else {
      propertyMap =
          properties.stream()
              .filter(e -> names.contains(e.getName()))
              .collect(Collectors.toMap(StreamProperty::getName, Function.identity()));
    }
    var values = propertyMap.values();
    /*
     * Return the selection.
     */
    return SamUtilFunctions.getStreamPropertiesTypeFromStreamProperty(values);
  }

  @GET
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/property/{pName}")
  @Produces(MediaType.APPLICATION_XML)
  public StreamPropertyType getPropertyForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      @PathParam("pName") String pName) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    /*
     * Check if the export has properties.
     */
    if (exp.getSpec().getType() != EExportedStreamType.ByProperties) {
      throw new NotFoundException();
    }
    /*
     * Check if there are any properties.
     */
    var properties =
        Optional.ofNullable(exp.getSpec().getStream().getProperties()).orElseGet(LinkedList::new);
    /*
     * Look for the properties.
     */
    return properties.stream()
        .filter(e -> e.getName().equals(pName))
        .findAny()
        .orElseThrow(NotFoundException::new)
        .toXml();
  }

  @DELETE
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/properties")
  @Consumes(MediaType.APPLICATION_XML)
  public void removePropertiesForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      StreamPropertyNamesType names) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    exportCoordinator.removeExportProperties(exp, names);
  }

  @DELETE
  @Path("/job/{jobName}/pe/{peId}/export/{portId}/property/{pName}")
  @Consumes(MediaType.APPLICATION_XML)
  public void removePropertyForExportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      @PathParam("pName") String name) {
    var job = getJob(jobName);
    var exp = getExport(job, peId, portId);
    exportCoordinator.removeExportProperty(exp, name);
  }

  /*
   * Import fields manipulations.
   */

  @PUT
  @Path("/job/{jobName}/pe/{peId}/import/{portId}/filter")
  @Consumes(MediaType.TEXT_PLAIN)
  public void setFilterForImportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      String filter) {
    var job = getJob(jobName);
    var imp = getImport(job, peId, portId);
    importCoordinator.setImportFilter(imp, filter);
  }

  @GET
  @Path("/job/{jobName}/pe/{peId}/import/{portId}/filter")
  @Produces(MediaType.TEXT_PLAIN)
  public String getFilterForImportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId) {
    var job = getJob(jobName);
    var imp = getImport(job, peId, portId);
    return Optional.ofNullable(imp.getSpec().getStreams().getFilter())
        .orElseThrow(NotFoundException::new);
  }

  @PUT
  @Path("/job/{jobName}/pe/{peId}/import/{portId}/streams")
  @Consumes(MediaType.APPLICATION_XML)
  public void setStreamsForImportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId,
      ImportedStreamsType streams) {
    var job = getJob(jobName);
    var imp = getImport(job, peId, portId);
    importCoordinator.setImportImportedStreams(imp, streams);
  }

  @GET
  @Path("/job/{jobName}/pe/{peId}/import/{portId}/streams")
  @Produces(MediaType.APPLICATION_XML)
  public ImportedStreamsType getStreamsForImportWithId(
      @Context Request request,
      @PathParam("jobName") String jobName,
      @PathParam("peId") String peId,
      @PathParam("portId") String portId) {
    var job = getJob(jobName);
    var imp = getImport(job, peId, portId);
    return Optional.ofNullable(imp.getSpec().getStreams())
        .orElseThrow(NotFoundException::new)
        .toXml();
  }
}
