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

import com.ibm.streams.controller.crds.exports.ExportSpec;
import com.ibm.streams.controller.crds.imports.ImportSpec;
import com.ibm.streams.controller.instance.rest.providers.ImportedStreamsTypeWriter;
import com.ibm.streams.controller.instance.rest.providers.StreamPropertiesTypeWriter;
import com.ibm.streams.controller.instance.rest.providers.StreamPropertyNamesTypeWriter;
import com.ibm.streams.controller.instance.rest.providers.StreamPropertyTypeWriter;
import com.ibm.streams.controller.instance.utils.ExportUtils;
import com.ibm.streams.controller.instance.utils.ImportUtils;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.controller.state.subscription.Subscriptions;
import com.ibm.streams.instance.sam.SamUtilFunctions;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.mock.instance.MockStreamsInstance;
import com.ibm.streams.platform.services.ImportedStreamsType;
import com.ibm.streams.platform.services.StreamPropertiesType;
import com.ibm.streams.platform.services.StreamPropertyNamesType;
import com.ibm.streams.platform.services.StreamPropertyType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.logging.Level;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriBuilder;
import lombok.var;
import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.client.HttpUrlConnectorProvider;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.jdkhttp.JdkHttpServerFactory;
import org.glassfish.jersey.logging.LoggingFeature;
import org.junit.ClassRule;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExternalResource;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SubscriptionsTest {

  private static final Logger LOGGER = LoggerFactory.getLogger(SubscriptionsTest.class);
  private static final java.util.logging.Level logLevel =
      Level.ALL; /* Set to OFF for server-side logs */
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
    var config =
        new ClientConfig()
            .register(LoggingFeature.class)
            .register(JacksonFeature.class)
            .register(ImportedStreamsTypeWriter.class)
            .register(StreamPropertyNamesTypeWriter.class)
            .register(StreamPropertiesTypeWriter.class)
            .register(StreamPropertyTypeWriter.class);
    config.property(HttpUrlConnectorProvider.SET_METHOD_WORKAROUND, true);
    return ClientBuilder.newClient(config);
  }

  private static List<StreamProperty> defaultProperties() {
    var prop = new StreamProperty("prop0", "someType", Arrays.asList("a", "b"));
    var props = new ArrayList<StreamProperty>();
    props.add(prop);
    return props;
  }

  @Test
  public void getAllPeSubscriptions() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("0");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var exp =
        ExportUtils.makeNamedExport("Export", "Stream", "srcjob", BigInteger.ZERO, BigInteger.ZERO);
    var imp =
        ImportUtils.makeNamedImport(
            "dstjob", "Import", "Stream", "dstjob", BigInteger.ZERO, BigInteger.ZERO);
    mockInstance.getExportStore().put(exp);
    mockInstance.getImportStore().put(imp);
    mockInstance.getSubscriptionBoard().add(exp, imp);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var subs = response.readEntity(Subscriptions.class);
    assertEquals(1, subs.getExports().size());
    assertEquals(0, subs.getImports().size());
  }

  @Test
  public void getExportSpec() throws Exception {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.OK.getStatusCode());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    ExportSpec spec = response.readEntity(ExportSpec.class);
    assertEquals(1, spec.getStream().getProperties().size());
    StreamProperty p =
        spec.getStream().getProperties().stream()
            .filter(e -> e.getName().equals("prop0"))
            .findAny()
            .orElseThrow(Exception::new);
    assertEquals("a", p.getValues().get(0));
  }

  @Test
  public void addExistingExportProperties() throws Exception {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("properties");
    /*
     * Get an empty set.
     */
    var response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Get the one-element set.
     */
    response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var props = response.readEntity(StreamPropertiesType.class);
    var p =
        props.getProperty().stream()
            .filter(e -> e.getName().equals("prop0"))
            .findAny()
            .orElseThrow(Exception::new);
    assertEquals("a", p.getValue().get(0));
    /*
     * Replace the properties.
     */
    var nlist =
        Collections.singletonList(
            new StreamProperty("prop0", "type1", Arrays.asList("hello", "world")));
    var update = SamUtilFunctions.getStreamPropertiesTypeFromStreamProperty(nlist);
    response = webTarget.request().put(Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void addExportProperties() throws Exception {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("properties");
    /*
     * Get an empty set.
     */
    var response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Get the one-element set.
     */
    response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info("Response: {}", response.readEntity(StreamPropertiesType.class));
    var props = response.readEntity(StreamPropertiesType.class);
    var p =
        props.getProperty().stream()
            .filter(e -> e.getName().equals("prop0"))
            .findAny()
            .orElseThrow(Exception::new);
    assertEquals("a", p.getValue().get(0));
    /*
     * Replace the properties.
     */
    var nlist =
        Arrays.asList(
            new StreamProperty("foo", "type1", Arrays.asList("hello", "world")),
            new StreamProperty("bar", "type2", Collections.singletonList("ola")));
    var update = SamUtilFunctions.getStreamPropertiesTypeFromStreamProperty(nlist);
    response = webTarget.request().put(Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the two-element set.
     */
    response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    props = response.readEntity(StreamPropertiesType.class);
    assertEquals(3, props.getProperty().size());
  }

  @Test
  public void addExportProperty() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop1");
    /*
     * Get an empty set.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Check that the prop we want is not there.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
    /*
     * Add that property.
     */
    var prop = new StreamProperty("prop1", "type1", Arrays.asList("hello", "world"));
    var update = SamUtilFunctions.getStreamPropertyTypeFromStreamProperty(prop);
    response = webTarget.request().put(Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the two-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var entity = response.readEntity(StreamPropertyType.class);
    assertEquals("hello", entity.getValue().get(0));
  }

  @Test
  public void addExportPropertyToNamedExport() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop1");
    /*
     * Get an empty set.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makeNamedExport("Export", "stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Check that the prop we want is not there.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
    /*
     * Add that property.
     */
    var prop = new StreamProperty("prop1", "type1", Arrays.asList("hello", "world"));
    var update = SamUtilFunctions.getStreamPropertyTypeFromStreamProperty(prop);
    response = webTarget.request().put(Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void addExportPropertiesToNamedExport() throws Exception {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("properties");
    /*
     * Get an empty set.
     */
    var response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makeNamedExport("Export", "stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Get the one-element set.
     */
    response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
    /*
     * Replace the properties.
     */
    var nlist =
        Arrays.asList(
            new StreamProperty("foo", "type1", Arrays.asList("hello", "world")),
            new StreamProperty("bar", "type2", Collections.singletonList("ola")));
    var update = SamUtilFunctions.getStreamPropertiesTypeFromStreamProperty(nlist);
    response = webTarget.request().put(Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void addExistingExportProperty() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop0");
    /*
     * Get an empty set.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Add that property.
     */
    var prop = new StreamProperty("prop0", "type1", Arrays.asList("hello", "world"));
    var update = SamUtilFunctions.getStreamPropertyTypeFromStreamProperty(prop);
    response = webTarget.request().put(Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void addExportPropertiesWithInvalidJob() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("properties");
    /*
     * Create an export, without a job.
     */
    var exp =
        ExportUtils.makeNamedExport("Export", "Stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getExportStore().put(exp);
    /*
     * Replace the properties.
     */
    var nlist =
        Arrays.asList(
            new StreamProperty("foo", "type1", Arrays.asList("hello", "world")),
            new StreamProperty("bar", "type2", Collections.singletonList("ola")));
    var update = SamUtilFunctions.getStreamPropertiesTypeFromStreamProperty(nlist);
    var response = webTarget.request().put(Entity.entity(update, MediaType.APPLICATION_XML));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void getExportProperties() throws Exception {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("properties");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response =
        invocationBuilder.post(
            Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Get the one-element set.
     */
    response =
        invocationBuilder.post(
            Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML));
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var props = response.readEntity(StreamPropertiesType.class);
    assertEquals(1, props.getProperty().size());
    var p =
        props.getProperty().stream()
            .filter(e -> e.getName().equals("prop0"))
            .findAny()
            .orElseThrow(Exception::new);
    assertEquals("a", p.getValue().get(0));
  }

  @Test
  public void setExportProperties() throws Exception {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("properties");
    /*
     * Get an empty set.
     */
    var response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Get the one-element set.
     */
    response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var props = response.readEntity(StreamPropertiesType.class);
    assertEquals(1, props.getProperty().size());
    /*
     * Replace the properties.
     */
    var nlist =
        Collections.singletonList(
            new StreamProperty("prop0", "type0", Arrays.asList("hello", "world")));
    var update = SamUtilFunctions.getStreamPropertiesTypeFromStreamProperty(nlist);
    response =
        webTarget.request().method("PATCH", Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the two-element set.
     */
    response =
        webTarget
            .request()
            .post(Entity.entity(new StreamPropertyNamesType(), MediaType.APPLICATION_XML));
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    props = response.readEntity(StreamPropertiesType.class);
    assertEquals(1, props.getProperty().size());
  }

  @Test
  public void setExportProperty() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop0");
    /*
     * Get an empty set.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Add that property.
     */
    var nlist = new StreamProperty("prop0", "type1", Arrays.asList("hello", "world"));
    var update = SamUtilFunctions.getStreamPropertyTypeFromStreamProperty(nlist);
    response =
        webTarget.request().method("PATCH", Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the two-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var prop = response.readEntity(StreamPropertyType.class);
    assertEquals("hello", prop.getValue().get(0));
  }

  @Test
  public void setUnknownExportProperty() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop2");
    /*
     * Get an empty set.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Check that the prop we want is not there.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
    /*
     * Add that property.
     */
    var nlist = new StreamProperty("prop2", "type1", Arrays.asList("hello", "world"));
    var update = SamUtilFunctions.getStreamPropertyTypeFromStreamProperty(nlist);
    response =
        webTarget.request().method("PATCH", Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void setExportPropertiesWithInvalidJob() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("properties");
    /*
     * Create an export, without a job.
     */
    var exp =
        ExportUtils.makeNamedExport("Export", "Stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getExportStore().put(exp);
    /*
     * Replace the properties.
     */
    var nlist =
        Arrays.asList(
            new StreamProperty("foo", "type1", Arrays.asList("hello", "world")),
            new StreamProperty("foo", "type2", Collections.singletonList("ola")));
    var update = SamUtilFunctions.getStreamPropertiesTypeFromStreamProperty(nlist);
    var response =
        webTarget.request().method("PATCH", Entity.entity(update, MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void removeExportProperty() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop0");
    /*
     * Get an empty set.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Get the one-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var prop = response.readEntity(StreamPropertyType.class);
    assertEquals("a", prop.getValue().get(0));
    /*
     * Delete the property.
     */
    response = webTarget.request().delete();
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the two-element set.
     */
    response = webTarget.request().get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
  }

  @Test
  public void removeUnknownExportProperty() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop1");
    /*
     * Get an empty set.
     */
    var response = webTarget.request().get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var exp =
        ExportUtils.makePropertyExport(
            "Export", defaultProperties(), "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getExportStore().put(exp);
    /*
     * Delete the property.
     */
    response = webTarget.request().delete();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void removeExportPropertyWithInvalidJob() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("export")
            .path("10")
            .path("property")
            .path("prop0");
    /*
     * Add an export, without a valid job .
     */
    var exp =
        ExportUtils.makeNamedExport("Export", "Stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getExportStore().put(exp);
    /*
     * Delete the property.
     */
    var response = webTarget.request().delete();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void getImportSpecWithNamedImports() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var imp =
        ImportUtils.makeNamedImport(
            "srcjob", "Import", "Stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getImportStore().put(imp);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    ImportSpec spec = response.readEntity(ImportSpec.class);
    assertEquals(BigInteger.ONE, spec.getPeId());
    assertEquals(BigInteger.TEN, spec.getPortId());
  }

  @Test
  public void getImportSpecWithPropertyImports() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var imp =
        ImportUtils.makeSubscriptionImport(
            "some == 1", "Import", BigInteger.ZERO, "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getImportStore().put(imp);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    ImportSpec spec = response.readEntity(ImportSpec.class);
    assertEquals(BigInteger.ONE, spec.getPeId());
    assertEquals(BigInteger.TEN, spec.getPortId());
  }

  @Test
  public void getImportFilter() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10")
            .path("filter");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.BAD_REQUEST.getStatusCode());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var imp =
        ImportUtils.makeNamedImportWithFilter(
            "srcjob", "Import", "Stream", "some == 1", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getImportStore().put(imp);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    String filter = response.readEntity(String.class);
    assertEquals("some == 1", filter);
  }

  @Test
  public void getImportFilterWithNoFilter() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10")
            .path("filter");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.BAD_REQUEST.getStatusCode());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var imp =
        ImportUtils.makeNamedImport(
            "srcjob", "Import", "Stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getImportStore().put(imp);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
  }

  @Test
  public void setImportFilter() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10")
            .path("filter");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.BAD_REQUEST.getStatusCode());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var imp =
        ImportUtils.makeNamedImport(
            "srcjob", "Import", "Stream", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getImportStore().put(imp);
    /*
     * Get the filter with an error.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.NOT_FOUND.getStatusCode(), response.getStatus());
    /*
     * Set the filter.
     */
    response = webTarget.request().put(Entity.entity("some == 1", MediaType.TEXT_PLAIN_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the filter.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    String filter = response.readEntity(String.class);
    assertEquals("some == 1", filter);
  }

  @Test
  public void setImportFilterWithInvalidJob() {
    /*
     * Create a client.
     */
    var client = ClientBuilder.newClient(new ClientConfig().register(LoggingFeature.class));
    client.register(JacksonFeature.class);
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10")
            .path("filter");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.BAD_REQUEST.getStatusCode());
    /*
     * Add a one element in the set.
     */
    var imp =
        ImportUtils.makeNamedImportWithFilter(
            "srcjob", "Import", "Stream", "some == 1", "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getImportStore().put(imp);
    /*
     * Get the filter with an error.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Set the filter.
     */
    response = webTarget.request().put(Entity.entity("some == 1", MediaType.TEXT_PLAIN_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Get the filter.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }

  @Test
  public void getImportStreams() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10")
            .path("streams");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.BAD_REQUEST.getStatusCode());
    /*
     * Add a one element in the set.
     */
    var job = JobUtils.makeJob("srcjob");
    var imp =
        ImportUtils.makeSubscriptionImport(
            "some == 1 && subscription == 2",
            "Import",
            BigInteger.ZERO,
            "srcjob",
            BigInteger.ONE,
            BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getImportStore().put(imp);
    /*
     * Get the one-element set.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var is = response.readEntity(ImportedStreamsType.class);
    assertEquals("some == 1 && subscription == 2", is.getPropertyBasedImport().getSubscription());
  }

  @Test
  public void setImportStreams() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10")
            .path("streams");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.BAD_REQUEST.getStatusCode());
    /*
     * Add a job and create a subscription import.
     */
    var job = JobUtils.makeJob("srcjob");
    var imp =
        ImportUtils.makeSubscriptionImport(
            "some == 1", "Import", BigInteger.ZERO, "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getJobStore().put(job);
    mockInstance.getImportStore().put(imp);
    /*
     * Create a new subscription.
     */
    var ns = new ImportedStreams(imp.getSpec().getStreams());
    ns.getPropertyBasedImport().setSubscription("some == 2");
    /*
     * Set the streams.
     */
    response = webTarget.request().put(Entity.entity(ns.toXml(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.NO_CONTENT.getStatusCode(), response.getStatus());
    /*
     * Get the filter.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.OK.getStatusCode(), response.getStatus());
    response.bufferEntity();
    LOGGER.info(response.readEntity(String.class));
    var is = response.readEntity(ImportedStreamsType.class);
    assertEquals("some == 2", is.getPropertyBasedImport().getSubscription());
  }

  @Test
  public void setImportStreamsWithInvalidJob() {
    /*
     * Create a client.
     */
    var client = buildClient();
    var webTarget =
        client
            .target("http://localhost:" + httpPort + "/api")
            .path("subscriptions")
            .path("job")
            .path("srcjob")
            .path("pe")
            .path("1")
            .path("import")
            .path("10")
            .path("streams");
    var invocationBuilder = webTarget.request();
    /*
     * Get an empty set.
     */
    var response = invocationBuilder.get();
    assertEquals(response.getStatus(), Response.Status.BAD_REQUEST.getStatusCode());
    /*
     * Add a one element in the set.
     */
    var imp =
        ImportUtils.makeSubscriptionImport(
            "some == 1", "Import", BigInteger.ZERO, "srcjob", BigInteger.ONE, BigInteger.TEN);
    mockInstance.getImportStore().put(imp);
    /*
     * Create a new subscription.
     */
    var ns = new ImportedStreams(imp.getSpec().getStreams());
    ns.getPropertyBasedImport().setSubscription("some == 2");
    /*
     * Get the filter with an error.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Set the filter.
     */
    response = webTarget.request().put(Entity.entity(ns.toXml(), MediaType.APPLICATION_XML_TYPE));
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
    /*
     * Get the streams.
     */
    response = invocationBuilder.get();
    assertEquals(Response.Status.BAD_REQUEST.getStatusCode(), response.getStatus());
  }
}
