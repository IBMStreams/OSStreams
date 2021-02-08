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

package com.ibm.streams.operator.internal.logtrace;

import com.ibm.streams.operator.logging.LogLevel;
import com.ibm.streams.operator.logging.LoggerNames;
import java.util.logging.Filter;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;
import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.TTCCLayout;

public class Setup {

  public static void setup(
      final MessageSender<Trace> traceAppSender, final MessageSender<Log> logAppSender) {
    setupLogging(traceAppSender, logAppSender, null);
  }

  public static void setup(
      final MessageSender<Trace> traceAppSender,
      final MessageSender<Log> logAppSender,
      final MessageSender<Trace> traceSplSender) {
    setupLogging(traceAppSender, logAppSender, traceSplSender);
  }
  // Maintain strong references to our root loggers for
  // trace and log to avoid them being gc'ed.
  private static final Logger traceAppLogger = Logger.getLogger(""); // Trace for user code.
  private static final Logger logAppLogger = Logger.getLogger(LoggerNames.LOG_FACILITY);
  /**
   * Logger to be used by the Java SPL runtime for trace messages that are associated with an
   * operator. Will have the aspect SPL_JAVAOP_ASPECT.
   */
  public static final Logger opTrace = Logger.getLogger("com.ibm.streams.operator");

  /** Aspect for messages related to the Java operator. */
  static final String SPL_JAVAOP_ASPECT = "spl_javaop";

  /**
   * Logger to be used by the Java SPL runtime for trace messages that for the runtime itself, i.e.
   * not associated with an operator.
   */
  public static final Logger rtTrace = Logger.getLogger("com.ibm.streams.operator.runtime");

  private static final Logger jvmTrace = Logger.getLogger("java");
  private static final Logger jvmxTrace = Logger.getLogger("javax");
  private static final Logger jaxbTrace = Logger.getLogger("com.sun.xml.internal");

  /** Aspect for messages related to the Java runtime. */
  static final String SPL_JAVART_ASPECT = "spl_javart";

  private static final org.apache.log4j.Logger traceLog4j = org.apache.log4j.Logger.getRootLogger();
  private static final org.apache.log4j.Logger logLog4j =
      org.apache.log4j.Logger.getLogger(LoggerNames.LOG_FACILITY);

  private static void setupLogging(
      final MessageSender<Trace> traceAppSender,
      final MessageSender<Log> logAppSender,
      final MessageSender<Trace> traceSplSender) {
    // Setup the trace facility as the root logger.
    for (Handler handler : traceAppLogger.getHandlers()) traceAppLogger.removeHandler(handler);

    traceAppLogger.addHandler(new TraceHandler(traceAppSender));
    // Default level, SPL runtime will overwrite.
    setTraceLevel(Trace.error);

    final Handler splrtHandler = new TraceHandler(traceSplSender);
    rtTrace.setUseParentHandlers(false);
    rtTrace.addHandler(splrtHandler);

    jvmTrace.setUseParentHandlers(false);
    jvmTrace.addHandler(splrtHandler);

    jvmxTrace.setUseParentHandlers(false);
    jvmxTrace.addHandler(splrtHandler);

    jaxbTrace.setUseParentHandlers(false);
    jaxbTrace.addHandler(splrtHandler);

    LoggerAspects.setLoggerAspects(rtTrace.getName(), SPL_JAVART_ASPECT);
    LoggerAspects.setLoggerAspects(opTrace.getName(), SPL_JAVAOP_ASPECT);

    // For JVM messages ensure they are not treated as application messages.

    // Add the log facility as a Logger disconnected from the root
    // Also add a handler for the trace facility for messages
    // at DEBUG or below.
    for (Handler handler : logAppLogger.getHandlers()) logAppLogger.removeHandler(handler);

    logAppLogger.setUseParentHandlers(false);
    logAppLogger.addHandler(new LogHandler(logAppSender));
    TraceHandler traceForLog = new TraceHandler(traceAppSender);
    traceForLog.setFilter(
        new Filter() {

          @Override
          public boolean isLoggable(LogRecord record) {
            return record.getLevel().intValue() < LogLevel.INFO.intValue();
          }
        });
    logAppLogger.addHandler(traceForLog);
  }

  /**
   * Set the current trace level. Ensure the root logger and the handler reflects the correct level,
   * so that other loggers that have set their own level do not waste cycles sending messages
   * through JNI that will be ignored. (E.g. when the trace level is info but a logger has set
   * trace).
   *
   * @param level Trace level to set.
   */
  public static String setTraceLevel(final Trace level) {
    final Level newLoggingLevel = setTraceForLogger(traceAppLogger, level);
    setTraceForLogger(rtTrace, level);

    traceLog4j.setLevel(level.getLog4jLevel());
    final AppenderSkeleton splAppender = (AppenderSkeleton) traceLog4j.getAppender("SPLTRACE");
    if (splAppender != null) splAppender.setThreshold(level.getLog4jLevel());

    return newLoggingLevel.getName();
  }

  private static Level setTraceForLogger(Logger logger, final Trace level) {
    final Level newLoggingLevel = level.getLoggingLevel();
    logger.setLevel(newLoggingLevel);
    for (Handler h : traceAppLogger.getHandlers()) {
      if (h instanceof TraceHandler) h.setLevel(newLoggingLevel);
    }
    return newLoggingLevel;
  }

  /**
   * Set the current log level.
   *
   * @param level Log Level to set.
   */
  public static void setLogLevel(final Log level) {
    logAppLogger.setLevel(level.getLoggingLevel());
    logLog4j.setLevel(level.getLog4jLevel());
  }

  /**
   * Set up the loggers for when we are embedded in another framework. Generally there is nothing to
   * be done, but if log4j has not been set up then perform a simple setup.
   */
  public static void setupForEmbedded() {
    if (!traceLog4j.getAllAppenders().hasMoreElements()) {
      traceLog4j.addAppender(new ConsoleAppender(new TTCCLayout(), ConsoleAppender.SYSTEM_ERR));
      Setup.setLogLevel(Log.warn);
      Setup.setTraceLevel(Trace.error);
    }
  }
}
