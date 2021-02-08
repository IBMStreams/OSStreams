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

package com.ibm.streams.operator;

import com.ibm.streams.operator.metrics.PEMetrics;
import java.io.File;
import java.math.BigInteger;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Map;

/** Information about an {@link com.ibm.streams.operator.Operator Operator}'s processing element. */
public interface ProcessingElement {

  /**
   * Return the output directory for the compiled SPL application.
   *
   * @return Output directory of the application.
   */
  public File getOutputDirectory();

  /**
   * Return the data directory for the SPL application.
   *
   * @return Data directory of the application.
   * @throws IllegalStateException No data directory has been specified for the application.
   */
  public File getDataDirectory();

  /**
   * Does this job have a data directory.
   *
   * @return True if a data directory was specified for this job's execution, false otherwise.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  boolean hasDataDirectory();

  /**
   * Return the name of the job group for the job the PE is running in.
   *
   * @return name for the job group for the job the PE is running in
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public String getJobGroup();

  /**
   * Get the runtime identifier for job containing this processing element.
   *
   * @return The job identifier for the current application.
   */
  public BigInteger getJobId();

  /**
   * Return the name of the job the PE is running in.
   *
   * @return name for the job the PE is running in
   * @since InfoSphere&reg; Streams Version 4.0.1
   */
  public String getJobName();

  /**
   * Get the runtime identifier for this processing element.
   *
   * @return The identifier for the this PE.
   */
  public BigInteger getPEId();

  /**
   * Get the metrics information for this processing element.
   *
   * @return PEMetrics object.
   */
  public PEMetrics getMetrics();

  /** Get the number of output ports for this processing element. */
  public int getNumberOfOutputPorts();

  /** Get the number of input ports for this processing element. */
  public int getNumberOfInputPorts();

  /**
   * Return true if this processing element is running in standalone mode.
   *
   * @return True if this processing element is running in standalone mode, false otherwise.
   */
  public boolean isStandalone();

  /**
   * Set the aspects associated with a named {@code Logger}. The aspects replace any existing
   * aspects associated with any {@code Logger} of the given name. Aspects are not inherited by any
   * child loggers.
   *
   * <p>The aspects are handled to the SPL log or trace facilities for each message logged by the
   * named {@code Logger}. The operator's name is automatically added into the set of aspects by the
   * SPL runtime. <br>
   * Aspects allow subsets of messages to be extracted from the SPL log or trace.
   *
   * @param loggerName Logger name
   * @param aspects Aspects to associate with named logger.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public void setLoggerAspects(String loggerName, String... aspects);

  /**
   * Return the application directory for the SPL application.
   *
   * @return application directory for the SPL application.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public File getApplicationDirectory();

  /**
   * Return the name of the SPL application.
   *
   * @return name of the SPL application
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public String getApplicationName();

  /**
   * Return the application scope for the job the PE is running in.
   *
   * @return application scope for the job the PE is running in
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public String getApplicationScope();

  /**
   * Return if the PE is running under optimized mode. Optimized mode is set by the {@code -a} or
   * {@code --optimized-code-generation} flag to the SPL compiler ({@code sc}). <br>
   * Operators may choose to reduce runtime checking when in optimized mode.
   *
   * @return True if the PE is running under optimized mode, false otherwise.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public boolean isOptimized();

  /**
   * Return the number of times this PE has been relaunched. For the first execution, the value will
   * be 0.
   *
   * @return number of times this PE has been relaunched.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public int getRelaunchCount();

  /**
   * Return the Streams instance identifier for this PE. When in standalone this will be the current
   * operating system user identifier.
   *
   * @return Instance identifier for this PE or the user identifier for standalone mode.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public String getInstanceId();

  /**
   * Return if the PE is running under debug mode. Optimized mode is set by the {@code -g} or {@code
   * --debug} flag to the SPL compiler ({@code sc}).
   *
   * @return True if the PE is running under debug mode, false otherwise.
   * @since InfoSphere Streams 3.1
   */
  public boolean isDebugging();

  /**
   * Return if the PE has profiling turned on. Profiling mode is set by the {@code -P} or {@code
   * --profile-all-operators} flag to the SPL compiler ({@code sc}).
   *
   * @return True if the PE is running with profiling on, false otherwise.
   * @since InfoSphere Streams 3.1
   */
  public boolean isProfilingOn();

  /**
   * Get the host this PE is executing on.
   *
   * <p>When running in distributed mode this returns the {@code InetAddress} for the interface
   * configured for application use by the Streams instance. This may differ from from {@code
   * java.net.InetAddress.getLocalHost()} if there are multiple network interfaces on this host.
   *
   * <p>When running in standalone or using the test framework, this returns {@code
   * java.net.InetAddress.getLocalHost()}.
   *
   * @return Host this PE is executing on
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public InetAddress getConfiguredHost() throws UnknownHostException;

  /**
   * Return the Streams domain identifier for this PE. When in standalone this will be the current
   * operating system user identifier.
   *
   * @return Domain identifier for this PE or the user identifier for standalone mode.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public String getDomainId();

  /**
   * Set tag data for the PE. If this value is {@link Operator.TagNames#OperatorIGC}, the tags will
   * be registered in the IG catalog. Other tag names may be defined in future releases.
   *
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public void setTagData(String tagName, Map<String, String> tagValues);

  /**
   * Get the application configuration specified by {@code name}. This method returns an
   * unmodifiable map which provides users with read-only access. An empty map is returned if the
   * configuration is not found.
   *
   * @param name the configuration name
   * @return the configuration
   * @since IBM Streams Version 4.2
   */
  public Map<String, String> getApplicationConfiguration(String name);

  /**
   * Get the absolute path of the root directory for the specified toolkit.
   *
   * <p>At SPL compile time this is the location of the toolkit specified to the SPL compiler {@code
   * sc}. <br>
   * At runtime this is the location of the toolkit in the application landing zone that has been
   * unpacked from the application's bundle ({@code sab} file).
   *
   * <p>Java native functions use this method to retrieve the root directory of their toolkit or of
   * dependent toolkits which have been unpacked from the application's bundle. Code running within
   * the context of a Java operator should use {@link OperatorContext#getToolkitDirectory()} to get
   * their toolkit's root directory.
   *
   * @param toolkitName name of the toolkit
   * @return the absolute path of the toolkit root directory
   * @see OperatorContext#getToolkitDirectory()
   * @since IBM Streams Version 4.2
   */
  public File getToolkitDirectory(String toolkitName);
}
