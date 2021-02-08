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

package com.ibm.streams.operator.internal.management;

import com.ibm.streams.operator.management.OperatorManagement;
import java.lang.management.ManagementFactory;
import java.util.Set;
import javax.management.InstanceNotFoundException;
import javax.management.MBeanRegistrationException;
import javax.management.MBeanServer;
import javax.management.MalformedObjectNameException;
import javax.management.ObjectName;

public class MXBeanControl {

  public static void platformRegister(
      final Object mxbean, final ObjectName name, final boolean checkForExisting) {

    final MBeanServer server = ManagementFactory.getPlatformMBeanServer();
    if (checkForExisting && server.isRegistered(name)) return;
    try {
      server.registerMBean(mxbean, name);
    } catch (Exception e) {
      throw new IllegalStateException(e);
    }
  }

  public static void platformUnregister(final ObjectName name) {
    final MBeanServer server = ManagementFactory.getPlatformMBeanServer();
    if (server.isRegistered(name)) {
      try {
        server.unregisterMBean(name);
      } catch (InstanceNotFoundException e) {
        // do nothing, another thread unregistered it.
      } catch (Exception e) {
        throw new IllegalStateException(e);
      }
    }
  }

  /**
   * Query all registered MXBeans of a specific type.
   *
   * @param type
   * @return
   */
  public static Set<ObjectName> queryAll(OperatorManagement.Type type) {
    final MBeanServer server = ManagementFactory.getPlatformMBeanServer();
    try {
      return server.queryNames(OperatorManagement.getName(type, "*"), null);
    } catch (MalformedObjectNameException e) {
      throw new IllegalStateException(e);
    }
  }

  public static void unregisterOperatorMXBeans(String operatorName) {
    final MBeanServer server = ManagementFactory.getPlatformMBeanServer();
    try {
      Set<ObjectName> beans =
          server.queryNames(
              OperatorManagement.getOperatorSpecificName(operatorName, null, null, null), null);

      for (ObjectName bean : beans) {
        try {
          server.unregisterMBean(bean);
        } catch (MBeanRegistrationException e) {;
        } catch (InstanceNotFoundException e) {
          e.printStackTrace();
        }
      }
    } catch (MalformedObjectNameException e) {
      throw new IllegalStateException(e);
    }
  }
}
