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

package com.ibm.streams.operator.samples.operators;

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.logging.LogLevel;
import com.ibm.streams.operator.logging.LoggerNames;
import com.ibm.streams.operator.logging.TraceLevel;
import java.util.logging.Logger;

/**
 * Operator that solely demonstrates source code use of {@code java.util.logging.Logger} to send
 * messages to the SPL log and trace facilities.
 *
 * @see LogLevel
 * @see TraceLevel
 * @see LoggerNames
 * @see AbstractOperator#setLoggerAspects(String, String...)
 */
public class LoggerUseExample extends AbstractOperator {

  private static final String CLASS_NAME = LoggerUseExample.class.getName();

  /**
   * Create a {@code Logger} specific to this class that will write to the SPL trace facility as a
   * child of the root {@code Logger}.
   */
  private final Logger trace = Logger.getLogger(CLASS_NAME);

  /**
   * Create a {@code Logger} specific to this class that will write to the SPL log facility as a
   * child of the {@link LoggerNames#LOG_FACILITY} {@code Logger}. The {@code Logger} uses a
   * resource bundle.
   */
  private final Logger log =
      Logger.getLogger(
          LoggerNames.LOG_FACILITY + "." + CLASS_NAME,
          "com.ibm.streams.operator.samples.operators.messages");

  /** Sample uses of log and trace. */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);

    // Note setLoggerAspects() must be called after super.initialize(context)
    // as it requires the context.

    // Associate the aspects exampleTrace and testTrace with trace
    // messages from the SPL trace logger
    setLoggerAspects(trace.getName(), "exampleTrace", "testTrace");

    // Associate the aspect exampleLog with messages from the SPL log logger.
    setLoggerAspects(log.getName(), "exampleLog");

    // Example use of trace to dump all parameters. The level uses
    // the Java Operator API specific class {@link TraceLevel}
    // that has constants for the SPL log levels.
    for (String parameterName : context.getParameterNames()) {
      for (String value : context.getParameterValues(parameterName)) {
        trace.log(
            TraceLevel.INFO, "Parameter {0} has value {1}", new Object[] {parameterName, value});
      }
    }

    // Example of logging a warning using a message from
    // the resource bundle with key ex01.

    log.warning("ex01");

    // Example of logging an exception using a message from
    // the resource bundle with key ex02. The level uses
    // the Java Operator API specific class {@link LogLevel}
    // that has constants for the SPL log levels.
    try {
      setup();
    } catch (Exception e) {
      log.log(LogLevel.ERROR, "ex02", e);
      throw e;
    }
  }

  /**
   * Dummy method to demonstrate logging an exception in initialize.
   *
   * @throws Exception
   */
  private void setup() throws Exception {}

  /** Example use of a standard {@code Logger} methods to trace method entry and exit. */
  @Override
  public void shutdown() throws Exception {
    trace.entering(CLASS_NAME, "shutdown");
    super.shutdown();

    // Perform operator specific shutdown

    trace.entering(CLASS_NAME, "shutdown");
  }
}
