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

import com.ibm.streams.controller.instance.utils.EndpointInfo;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.controller.instance.utils.ProcessingElementUtils;
import com.ibm.streams.controller.instance.utils.ServiceUtils;
import com.ibm.streams.mock.instance.MockStreamsInstance;
import io.fabric8.kubernetes.api.model.ServicePort;
import java.math.BigInteger;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.GenericType;
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

public class EndpointsTest {

  private static final Logger LOGGER = LoggerFactory.getLogger(EndpointsTest.class);
  private static final Level logLevel = Level.ALL; /* Set to OFF for server-side logs */
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

  private static Client buildClient() {
    var config = new ClientConfig().register(LoggingFeature.class).register(JacksonFeature.class);
    return ClientBuilder.newClient(config);
  }

  private static List<ServicePort> buildDefaultPorts() {
    var ports = new LinkedList<ServicePort>();
    ports.add(new ServicePort(null, "sys-web", 80, 80, "TCP", null));
    ports.add(new ServicePort(null, "usr-test", 1000, 1000, "TCP", null));
    return ports;
  }

  @Test
  public void getEndpoints() {
    /*
     * Create a client.
     */
    var client = buildClient();
    WebTarget webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("pe")
            .path("0");
    /*
     * Get when the Job does not exist.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("job-0");
    var pe = ProcessingElementUtils.makeProcessingElement("job-0", BigInteger.ZERO);
    var svc = ServiceUtils.makeService("job-0", BigInteger.ZERO, buildDefaultPorts());
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    mockInstance.getServiceStore().put(svc);
    /*
     * Get the one-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var service = response.readEntity(new GenericType<Map<String, ServicePort>>() {});
    assertEquals(2, service.size());
  }

  @Test
  public void addEndpoint() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var target0 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("endpoint")
            .path("test");
    var target1 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("pe")
            .path("0")
            .path("endpoint")
            .path("test");
    /*
     * Get when the Job does not exist.
     */
    var response = target0.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("job-0");
    var pe = ProcessingElementUtils.makeProcessingElement("job-0", BigInteger.ZERO);
    var svc = ServiceUtils.makeService("job-0", BigInteger.ZERO, new LinkedList<>());
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    mockInstance.getServiceStore().put(svc);
    /*
     * Add the endpoint.
     */
    response =
        target1
            .request()
            .put(
                Entity.entity(
                    new EndpointInfo("fqdn", 80, "TCP"), MediaType.APPLICATION_JSON_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the endpoint.
     */
    response = target0.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var info = response.readEntity(EndpointInfo.class);
    assertEquals(80, info.getPort().longValue());
  }

  @Test
  public void addEndpointExisting() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var target0 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("endpoint")
            .path("test");
    var target1 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("pe")
            .path("0")
            .path("endpoint")
            .path("test");
    /*
     * Get when the Job does not exist.
     */
    var response = target0.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("job-0");
    var pe = ProcessingElementUtils.makeProcessingElement("job-0", BigInteger.ZERO);
    var svc = ServiceUtils.makeService("job-0", BigInteger.ZERO, buildDefaultPorts());
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    mockInstance.getServiceStore().put(svc);
    /*
     * Add the endpoint.
     */
    response =
        target1
            .request()
            .put(
                Entity.entity(
                    new EndpointInfo("fqdn", 80, "TCP"), MediaType.APPLICATION_JSON_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void removeEndpoint() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var target0 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("endpoint")
            .path("test");
    var target1 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("pe")
            .path("0")
            .path("endpoint")
            .path("test");
    /*
     * Get when the Job does not exist.
     */
    var response = target0.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("job-0");
    var pe = ProcessingElementUtils.makeProcessingElement("job-0", BigInteger.ZERO);
    var svc = ServiceUtils.makeService("job-0", BigInteger.ZERO, new LinkedList<>());
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    mockInstance.getServiceStore().put(svc);
    /*
     * Add the endpoint.
     */
    response =
        target1
            .request()
            .put(
                Entity.entity(
                    new EndpointInfo("fqdn", 80, "TCP"), MediaType.APPLICATION_JSON_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the endpoint.
     */
    response = target0.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    /*
     * Remove the endpoint.
     */
    response = target1.request().delete();
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the endpoint.
     */
    response = target0.request().get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
  }

  @Test
  public void removeEndpointMissing() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var target0 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("endpoint")
            .path("test");
    var target1 =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("endpoints")
            .path("job")
            .path("job-0")
            .path("pe")
            .path("0")
            .path("endpoint")
            .path("test");
    /*
     * Get when the Job does not exist.
     */
    var response = target0.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("job-0");
    var pe = ProcessingElementUtils.makeProcessingElement("job-0", BigInteger.ZERO);
    var svc = ServiceUtils.makeService("job-0", BigInteger.ZERO, new LinkedList<>());
    mockInstance.getJobStore().put(job);
    mockInstance.getProcessingElementStore().put(pe);
    mockInstance.getServiceStore().put(svc);
    /*
     * Get the endpoint.
     */
    response = target0.request().get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
    /*
     * Remove the endpoint.
     */
    response = target1.request().delete();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }
}
