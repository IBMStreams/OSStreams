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

package com.ibm.streams.controller.instance.rest;

import static org.junit.Assert.assertEquals;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.pes.Connectivity;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.controller.instance.utils.ParallelRegionUtils;
import com.ibm.streams.controller.instance.utils.ProcessingElementUtils;
import com.ibm.streams.mock.instance.MockStreamsInstance;
import com.ibm.xmlns.prod.streams.api.state.Instance;
import java.math.BigInteger;
import java.util.logging.Level;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.Invocation;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriBuilder;
import lombok.var;
import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.jdkhttp.JdkHttpServerFactory;
import org.glassfish.jersey.logging.LoggingFeature;
import org.junit.ClassRule;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExternalResource;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class StateTest {

  private static final Logger LOGGER = LoggerFactory.getLogger(StateTest.class);
  private static final Level logLevel = Level.OFF; /* Set to OFF for server-side logs */
  private static final Integer httpPort =
      20000 + Integer.parseInt(System.getProperty("surefire.fork.number", "0"));

  private static MockStreamsInstance mockInstance;

  @Rule
  public ExternalResource storeRules =
      new ExternalResource() {
        @Override
        protected void before() throws Throwable {
          super.before();
          mockInstance.clear();
        }

        @Override
        protected void after() {
          super.after();
        }
      };

  @ClassRule
  public static ExternalResource serverRule =
      new ExternalResource() {
        @Override
        protected void before() throws Throwable {
          super.before();
          mockInstance = new MockStreamsInstance();
          /*
           * Create the dummy web server.
           */
          LOGGER.info("HTTP server listening on port: {}", httpPort);
          var baseUri = UriBuilder.fromUri("http://0.0.0.0/").port(httpPort).build();
          var config = new StreamsResourceConfig(mockInstance, "test");
          config.register(
              new LoggingFeature(
                  java.util.logging.Logger.getLogger(getClass().getName()),
                  logLevel,
                  LoggingFeature.Verbosity.PAYLOAD_TEXT,
                  8192));
          JdkHttpServerFactory.createHttpServer(baseUri, config);
        }

        @Override
        protected void after() {
          super.after();
        }
      };

  @Test
  public void captureInstanceState() {
    /*
     * Create a client.
     */
    Client client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    WebTarget webTarget = client.target("http://localhost:" + httpPort + "/api").path("state");
    /*
     * Get when the Job does not exist.
     */
    Response response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    Job job = JobUtils.makeJob("srcjob");
    ProcessingElement pe = ProcessingElementUtils.makeProcessingElement("srcjob", BigInteger.ZERO);
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    /*
     * Get the one-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    Instance instance = response.readEntity(Instance.class);
    assertEquals("test", instance.getId());
  }

  @Test
  public void getJobNameById() {
    /*
     * Create a client.
     */
    Client client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    WebTarget webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("state")
            .path("job")
            .path("by-id")
            .path("1")
            .path("name");
    /*
     * Get when the Job does not exist.
     */
    Response response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    Job job = JobUtils.makeJob("srcjob");
    mockInstance.getJobStore().put(job);
    /*
     * Get the one-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    String jobName = response.readEntity(String.class);
    assertEquals("srcjob", jobName);
  }

  @Test
  public void getPeConnectivity() {
    /*
     * Create a client.
     */
    Client client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    WebTarget webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("state")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("0")
            .path("connectivity");
    Invocation.Builder invocationBuilder = webTarget.request();
    /*
     * Get when the Job does not exist.
     */
    Response response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    Job job = JobUtils.makeJob("srcjob");
    ProcessingElement pe = ProcessingElementUtils.makeProcessingElement("srcjob", BigInteger.ZERO);
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    Connectivity connectivity = response.readEntity(Connectivity.class);
    assertEquals(Connectivity.None, connectivity);
  }

  @Test
  public void setPeConnectivityToFull() {
    /*
     * Create a client.
     */
    Client client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    WebTarget webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("state")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("0")
            .path("connectivity");
    /*
     * Get when the Job does not exist.
     */
    Response response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    Job job = JobUtils.makeJob("srcjob");
    ProcessingElement pe = ProcessingElementUtils.makeProcessingElement("srcjob", BigInteger.ZERO);
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    /*
     * Update the PE health.
     */
    response =
        webTarget.request().put(Entity.entity(Connectivity.Full, MediaType.APPLICATION_JSON_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the one-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    Connectivity connectivity = response.readEntity(Connectivity.class);
    assertEquals(Connectivity.Full, connectivity);
  }

  @Test
  public void setPeConnectivityToPartial() {
    /*
     * Create a client.
     */
    Client client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    WebTarget webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("state")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("0")
            .path("connectivity");
    /*
     * Get when the Job does not exist.
     */
    Response response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    Job job = JobUtils.makeJob("srcjob");
    ProcessingElement pe = ProcessingElementUtils.makeProcessingElement("srcjob", BigInteger.ZERO);
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    /*
     * Update the PE health.
     */
    response =
        webTarget
            .request()
            .put(Entity.entity(Connectivity.Partial, MediaType.APPLICATION_JSON_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the one-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    Connectivity connectivity = response.readEntity(Connectivity.class);
    assertEquals(Connectivity.Partial, connectivity);
  }

  @Test
  public void getParallelRegionWidth() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("state")
            .path("job")
            .path("srcjob")
            .path("pr")
            .path("NestedPar.Nest.Nest")
            .path("width");
    var invocationBuilder = webTarget.request();
    /*
     * Get when the Job does not exist.
     */
    var response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var pr =
        ParallelRegionUtils.makeParallelRegion(
            job, "srcjob-0", "NestedPar.Nest.Nest", BigInteger.valueOf(5));
    mockInstance.getJobStore().put(job);
    mockInstance.getParallelRegionStore().put(pr);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    var width = response.readEntity(BigInteger.class);
    assertEquals(BigInteger.valueOf(5), width);
  }

  @Test
  public void setParallelRegionWidth() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("state")
            .path("job")
            .path("srcjob")
            .path("pr")
            .path("NestedPar.Nest.Nest")
            .path("width");
    var invocationBuilder = webTarget.request();
    /*
     * Get when the Job does not exist.
     */
    var response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var pr =
        ParallelRegionUtils.makeParallelRegion(
            job, "srcjob-0", "NestedPar.Nest.Nest", BigInteger.valueOf(5));
    mockInstance.getJobStore().put(job);
    mockInstance.getParallelRegionStore().put(pr);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    var width = response.readEntity(BigInteger.class);
    assertEquals(BigInteger.valueOf(5), width);
    /*
     * Update the PR width.
     */
    response =
        webTarget
            .request()
            .put(Entity.entity(BigInteger.valueOf(1), MediaType.APPLICATION_JSON_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    width = response.readEntity(BigInteger.class);
    assertEquals(BigInteger.valueOf(1), width);
  }
}
