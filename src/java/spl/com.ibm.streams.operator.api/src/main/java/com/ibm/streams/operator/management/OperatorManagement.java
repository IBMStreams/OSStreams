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

import com.ibm.streams.operator.OperatorContext;
import java.lang.management.ManagementFactory;
import java.util.Hashtable;
import java.util.Locale;
import javax.management.JMX;
import javax.management.MalformedObjectNameException;
import javax.management.ObjectName;

/**
 * Utility code for working with an operator's MXBeans.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public class OperatorManagement {

  private OperatorManagement() {};

  /**
   * ObjectName domain for MXBeans registered in the platform MBeanServer of the executing
   * processing element. Set to {@value}.
   */
  public static final String STREAMS_OPERATOR_JMX_DOMAIN = "com.ibm.streams.operator";

  /**
   * Enumeration of names for an {@code ObjectName}'s {@code type} key property for MXBeans in
   * {@link OperatorManagement#STREAMS_OPERATOR_JMX_DOMAIN}.
   *
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public enum Type {
    /** Management bean is an instance of {@link ProcessingElementMXBean}. */
    PE,

    /** Management bean is an instance of {@link OperatorContextMXBean}. */
    OPERATORCONTEXT,

    /** Management bean represents a metric and is an instance of {@link MetricMXBean}. */
    METRIC,
    ;

    /**
     * Return the value used for the {@code ObjectName}'s {@code type} key property for this
     * management bean type.
     *
     * @return Value used for the {@code type} key property.
     */
    public String type() {
      return name().toLowerCase(Locale.US);
    }
  }

  /**
   * Get the {@code ObjectName} for an MXBean in {@link
   * OperatorManagement#STREAMS_OPERATOR_JMX_DOMAIN}.
   *
   * @param type Type of the MXBean for the {@code type} key property.
   * @param name Name of the MXBean for the {@code name} key property. if null then no {@code name}
   *     key property will be added.
   * @return {@code ObjectName} for {@code type} and {@code name}.
   * @throws MalformedObjectNameException Invalid name syntax.
   */
  public static ObjectName getName(OperatorManagement.Type type, String name)
      throws MalformedObjectNameException {
    final Hashtable<String, String> properties = new Hashtable<String, String>();
    properties.put("type", type.type());
    if (name != null) properties.put("name", name);
    return new ObjectName(STREAMS_OPERATOR_JMX_DOMAIN, properties);
  }

  /**
   * Get the {@code ObjectName} for an MXBean specific to a named operator. The {@code ObjectName}
   * key properties {@code operator, type} and {@code name} are set from the parameters {@code
   * operatorName, type} and {@code name} respectively.
   *
   * @param operatorName Name of the operator
   * @param type Type of the MXBean, if null then it is ignored.
   * @param name Name of the MXBean, if null then it is ignored.
   * @param properties Arbitrary name value pairs to include in {@code ObjectName}.
   * @return {@code ObjectName} specific to the named operator.
   * @throws MalformedObjectNameException Invalid name syntax.
   */
  public static ObjectName getOperatorSpecificName(
      String operatorName,
      OperatorManagement.Type type,
      String name,
      Hashtable<String, String> properties)
      throws MalformedObjectNameException {
    if (properties == null) properties = new Hashtable<String, String>();
    if (type != null) properties.put("type", type.type());
    if (name != null) properties.put("name", name);
    properties.put("operator", operatorName);
    return new ObjectName(STREAMS_OPERATOR_JMX_DOMAIN, properties);
  }

  /**
   * Get the {@code ObjectName} for automatically registered {@link OperatorContextMXBean} for the
   * {@link OperatorContext}.
   *
   * @param context Context for the operator.
   * @return {@code ObjectName} for the operator
   */
  public static ObjectName getName(OperatorContext context) {
    try {
      return getName(Type.OPERATORCONTEXT, context.getName());
    } catch (MalformedObjectNameException e) {
      // can never happen since this by definition is a valid format.
      throw new IllegalStateException(e);
    }
  }

  /**
   * Get the {@code ObjectName} for automatically registered {@link ProcessingElementMXBean}.
   *
   * @return {@code ObjectName} for the operator
   */
  public static ObjectName getPEName() {
    try {
      return getName(Type.PE, null);
    } catch (MalformedObjectNameException e) {
      // can never happen since this by definition is a valid format.
      throw new IllegalStateException(e);
    }
  }

  /**
   * Get a proxy to the {@code OperatorContextMXBean} for {@code context}. The MXBean proxy is
   * obtained using:
   *
   * <pre>
   * <code>
   * JMX.newMXBeanProxy(
   * ManagementFactory.getPlatformMBeanServer(),
   * OperatorManagement.getName(context),
   * OperatorContextMXBean.class,
   * true);
   * </code>
   * </pre>
   *
   * <br>
   * The returned proxy will implement {@code NotificationEmitter}.
   *
   * @param context
   * @return Proxy to the {@code OperatorContextMXBean} for {@code context}.
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public static OperatorContextMXBean getOperatorContextMXBean(OperatorContext context) {
    return JMX.newMXBeanProxy(
        ManagementFactory.getPlatformMBeanServer(),
        OperatorManagement.getName(context),
        OperatorContextMXBean.class,
        true);
  }

  /**
   * Get a proxy to the {@code ProcessingElementMXBean} registered for the current Processing
   * Element. The MXBean proxy is obtained using:
   *
   * <pre>
   * <code>
   * JMX.newMXBeanProxy(
   * ManagementFactory.getPlatformMBeanServer(),
   * OperatorManagement.getPEName(),
   * ProcessingElementMXBean.class,
   * true);
   * </code>
   * </pre>
   *
   * <br>
   * The returned proxy will implement {@code NotificationEmitter}.
   *
   * @return Proxy to {@code ProcessingElementMXBean}.
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public static ProcessingElementMXBean getProcessingElementMXBean() {
    return JMX.newMXBeanProxy(
        ManagementFactory.getPlatformMBeanServer(),
        OperatorManagement.getPEName(),
        ProcessingElementMXBean.class,
        true);
  }
}
