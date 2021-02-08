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

package com.ibm.streams.operator.internal.logtrace.log4j;

import com.ibm.streams.operator.ProcessingElement;
import java.io.File;
import org.apache.log4j.PropertyConfigurator;

/**
 * Create a watcher on the log4j.properties file in the data directory. If it exists when watchFile
 * is called or when the trace level is changed then the log4j configuration is updated by the file.
 */
public class DynamicConfig {

  /* public static void watchFile(final ProcessingElement pe) {

      if (!pe.hasDataDirectory())
          return;

      final File log4jProps = new File(
              pe.getDataDirectory(), "log4j.properties");

      updateConfigurationfromProperties(log4jProps);

      final NotificationEmitter peMx = (NotificationEmitter) OperatorManagement
              .getProcessingElementMXBean();
      NotificationFilterSupport traceFilter = new NotificationFilterSupport();
      traceFilter.enableType(ProcessingElementMXBean.TRACE_LEVEL);

      peMx.addNotificationListener(new NotificationListener() {
          @Override
          public void handleNotification(Notification notification,
                  Object handback) {
              updateConfigurationfromProperties(log4jProps);
          }
      }, traceFilter, null);
  }
  */
  /** If the file exists then update the configuration now. */
  public static void updateConfigurationfromProperties(final ProcessingElement pe) {

    if (!pe.hasDataDirectory()) return;

    final File log4jProps = new File(pe.getDataDirectory(), "log4j.properties");

    if (log4jProps.exists() && log4jProps.canRead()) {
      PropertyConfigurator.configure(log4jProps.getAbsolutePath());
    }
  }
}
