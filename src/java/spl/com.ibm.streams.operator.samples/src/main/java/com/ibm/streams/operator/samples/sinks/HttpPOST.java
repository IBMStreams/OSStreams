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

package com.ibm.streams.operator.samples.sinks;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.Metric.Kind;
import com.ibm.streams.operator.model.CustomMetric;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.operator.samples.patterns.TupleConsumer;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLEncoder;
import java.util.Iterator;
import java.util.Queue;
import java.util.logging.Logger;

/**
 * Sink operator where incoming tuples are sent as an HTTP POST requests to the URL specified in the
 * operator specific {@code url} parameter.
 *
 * <p>The POST request contains a query string of all the attribute names with their values obtained
 * using Tuple.getString(). This mimics how an HTML Form with the same attributes and values would
 * be sent. <br>
 * Multiple tuples may be represented in the POST request, a single tuple per POST request is the
 * default though.
 *
 * <p><B>Parameters</B>
 *
 * <UL>
 *   <LI><code>url</code> - HTTP URL of the server to POST data to.
 *   <LI><code>batchSize</code> - Number of tuples to include in each POST request, defaults to 1.
 * </UL>
 *
 * <p><B>Example SPL invocation</B> <br>
 * A Java primitive operator model {@code com.ibm.streams.javaprimitivesamples.sinks::HTTPPost} is
 * provided in the sample toolkit <br>
 * {@code $STREAMS_INSTALL/samples/spl/feature/JavaOperators} <code>
 * <pre>
 *  use com.ibm.streams.javaprimitivesamples.sinks::HTTPPost;
 *
 *  ...
 *
 *  () as TestPost = HTTPPost(input-streams)
 *   {
 *     param:
 *       // Example Jetty - show request info
 *       url: "http://localhost:8080/dump/info";
 *       batchSize: 3;
 *   }
 * </pre>
 * </code>
 */
@PrimitiveOperator(
    name = "HTTPPost",
    description =
        "Sink operator where incoming tuples are sent as an HTTP POST requests to the URL specified in the operator specific `url` parameter.")
@InputPortSet
public class HttpPOST extends TupleConsumer {

  private static final Logger trace = Logger.getLogger(HttpPOST.class.getName());

  /** URL of the HTTP server we will be posting data to. */
  private URL url;

  /** Path of the URL from the operator parameter. */
  private String urlPath;

  /** Initialize by setting the URL and the batch size. */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);

    url = getURL();

    setBatchSize(batchSize());
  }

  /**
   * Set the path of the URL as a String, from the operator parameter.
   *
   * @param urlPath Path of the URL.
   */
  @Parameter(name = "url", description = "HTTP URL of the server to POST data to.")
  public final void setUrlPath(String urlPath) {
    this.urlPath = urlPath;
  }

  /** Get the path of the URL as a String. */
  public final String getUrlPath() {
    return urlPath;
  }

  /**
   * Get the URL for the POST requests from the required parameter url. Sub-classes may override
   * this to set the URL another way.
   *
   * @return URL for POST requests
   * @throws MalformedURLException
   */
  protected URL getURL() throws MalformedURLException {
    return new URL(getUrlPath());
  }

  /**
   * Get the batch size to use from the parameter batchSize using 1 if that is not set. Sub-classes
   * may override this to set the batchSize another way.
   *
   * @return batchSize to use
   */
  protected int batchSize() {
    return getBatchSize();
  }

  /*
   * Metrics for POST requests
   */
  private Metric nPostRequests;
  private Metric nPostErrorCodeResponses;

  @CustomMetric(description = "Number of HTTP POST requests", kind = Kind.COUNTER)
  public void setnPostRequests(Metric nPostRequests) {
    this.nPostRequests = nPostRequests;
  }

  public Metric getnPostRequests() {
    return nPostRequests;
  }

  @CustomMetric(
      description = "Number of HTTP POST requests with error response",
      kind = Kind.COUNTER)
  public void setnPostErrorCodeResponses(Metric nPostErrorCodeResponses) {
    this.nPostErrorCodeResponses = nPostErrorCodeResponses;
  }

  public Metric getnPostErrorCodeResponses() {
    return nPostErrorCodeResponses;
  }

  /**
   * Process the batch of incoming tuples by creating a POST request from them of the form
   * name=value. Note that multiple values of the same name can be supplied and are supplied when
   * multiple tuples of the same type are present in the batch.
   *
   * <p>Since this is an example of extracting data to an external system, exception handling is not
   * covered and exceptions will cause the processing element containing the operator to terminate.
   * If the HTTP Response code is greater or equal to 400 then output from the request error stream
   * will be traced at {@code debug} level, otherwise the request output will be traced at {@code
   * debug} level. <br>
   * A functional HTTP POST operator has to consider a variety of exceptions and how to handle them.
   * The correct approach will depend on the functionality being implemented, options for handling
   * exceptions include:
   *
   * <UL>
   *   <LI>re-trying to submit the tuples, e.g. in an ordering system
   *   <LI>the incoming tuples being discarded until a valid connection can be made, e.g. for a
   *       visual dash board
   *   <LI>re-trying tuples for a limited time and discarding them after that, e.g. to support
   *       temporary connection issues but not submitting data that may be irrelevant due to its age
   *   <LI>submitting tuples to an optional output port to allow a different processing path for
   *       tuples that could not be submitted
   * </UL>
   */
  @Override
  protected final boolean processBatch(Queue<BatchedTuple> batch) throws Exception {

    StringBuilder data = new StringBuilder(1024);

    int tuplesInRequest = 0;
    for (Iterator<BatchedTuple> iter = batch.iterator(); iter.hasNext(); ) {

      if (tuplesInRequest++ == getBatchSize()) break;

      BatchedTuple item = iter.next();
      iter.remove();

      StreamSchema schema = item.getStream().getStreamSchema();

      for (Attribute attribute : schema) {
        if (data.length() != 0) data.append('&');

        data.append(URLEncoder.encode(attribute.getName(), "UTF-8"));
        data.append('=');
        data.append(URLEncoder.encode(item.getTuple().getString(attribute.getName()), "UTF-8"));
      }
    }

    final HttpURLConnection conn = (HttpURLConnection) url.openConnection();
    conn.setDoOutput(true);
    conn.setRequestMethod("POST");
    final OutputStream requestStream = conn.getOutputStream();
    final OutputStreamWriter wr = new OutputStreamWriter(requestStream, "UTF-8");
    wr.write(data.toString());
    wr.flush();
    wr.close();
    requestStream.close();
    getnPostRequests().incrementValue(1L);

    // Get the response
    final InputStream responseStream;
    final int responseCode = conn.getResponseCode();
    if (trace.isLoggable(TraceLevel.DEBUG)) {
      trace.log(TraceLevel.DEBUG, "HTTP POST response code=" + responseCode);
    }
    if (responseCode >= 400) {
      getnPostErrorCodeResponses().incrementValue(1L);
      responseStream = conn.getErrorStream();
    } else {
      responseStream = conn.getInputStream();
    }

    final BufferedReader rd = new BufferedReader(new InputStreamReader(responseStream));

    String line;
    while ((line = rd.readLine()) != null) {
      trace.log(TraceLevel.DEBUG, line);
    }
    wr.close();
    rd.close();
    responseStream.close();

    return true;
  }
}
