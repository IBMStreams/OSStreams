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

package com.ibm.streams.operator.management;

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.management.OperatorManagement.Type;
import java.net.UnknownHostException;
import java.util.Map;

/**
 * The management interface for a {@link ProcessingElement}.
 *
 * <p>A {@code ProcessingElementMXBean} represents the containing processing element (PE). The
 * management bean is automatically registered with the platform MBeanServer before {@link
 * Operator#initialize(OperatorContext)} is called. The management bean is unregistered
 * automatically once {@link Operator#shutdown()} completes.
 *
 * <p>The {@code ObjectName} for uniquely identifying the MXBean for a PE in the PE's platform MBean
 * Server is: <br>
 * {@code com.ibm.streams.operator:type=pe}
 *
 * @see OperatorManagement#STREAMS_OPERATOR_JMX_DOMAIN
 * @see Type#PE
 * @see ProcessingElement
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface ProcessingElementMXBean {
  /**
   * Notification the SPL trace level has changed. <br>
   * The notification's {@code userData} will be the value of {@link
   * com.ibm.streams.operator.logging.TraceLevel#getName()} of the new SPL trace level. <br>
   * The value of this notification identifier is {@value}.
   *
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public static final String TRACE_LEVEL = "com.ibm.streams.operator.pe.traceLevel";

  /**
   * Return the name of the job group for the job the PE is running in.
   *
   * @return name of the job group for the job the PE is running in
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public String getJobGroup();

  /**
   * Get the runtime identifier for job containing this processing element.
   *
   * @return The job identifier for the current application.
   */
  public String getJobId();

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
  public String getPEId();

  /**
   * Return the absolute path of the output directory for the compiled SPL application.
   *
   * @return Output directory of the application.
   */
  public String getOutputDirectory();

  /**
   * Return the absolute path of the data directory for the SPL application.
   *
   * @return Data directory of the application.
   */
  public String getDataDirectory();

  /**
   * Return the absolute path of the application directory for the SPL application.
   *
   * @return application directory for the SPL application.
   */
  public String getApplicationDirectory();

  /**
   * Return the name of the application the PE is running.
   *
   * @return name of the application the PE is running
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public String getApplicationName();

  /**
   * Return the application scope for the job the PE is running in.
   *
   * @return application scope for the job the PE is running in
   */
  public String getApplicationScope();

  /**
   * Return true if this processing element is running in standalone mode.
   *
   * @return True if this processing element is running in standalone mode, false otherwise.
   */
  public boolean isStandalone();

  /**
   * Return the number of times this PE has been relaunched. For the first execution, the value will
   * be 0.
   *
   * @return number of times this PE has been relaunched.
   */
  public int getRelaunchCount();

  /**
   * Return the Streams instance identifier for this PE. When in standalone this will be the current
   * operating system user identifier.
   *
   * @return Instance identifier for this PE or the user identifier for standalone mode.
   */
  public String getInstanceId();

  /**
   * Get the host name this PE is executing on.
   *
   * <p>When running in distributed mode this returns the host name for the interface configured for
   * application use by the Streams instance. This may differ from from {@code
   * java.net.InetAddress.getLocalHost().getCanonicalHostName()} if there are multiple network
   * interfaces on this host.
   *
   * <p>When running in standalone or using the test framework, this returns {@code
   * java.net.InetAddress.getLocalHost().getCanonicalHostName()}.
   *
   * @return Host name this PE is executing on
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public String getConfiguredHostName() throws UnknownHostException;

  /**
   * Return the Streams domain identifier for this PE. When in standalone this will be the current
   * operating system user identifier.
   *
   * @return Domain identifier for this PE or the user identifier for standalone mode.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public String getDomainId();

  /**
   * Get the properties of the application configuration specified by {@code name}. This method
   * returns an unmodifiable map which provides users with read-only access. An empty map is
   * returned if the configuration is not found.
   *
   * @param name the configuration name
   * @return the configuration properties
   * @since IBM Streams Version 4.2
   */
  public Map<String, String> getApplicationConfiguration(String name);
}
