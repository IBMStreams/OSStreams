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

package com.ibm.streams.admin.internal.api.trace;

import java.io.*;
import java.io.IOException;
import java.util.logging.FileHandler;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

public class StreamsTraceLogger extends TraceLogger {
  private Logger myLogger;
  private static String loggerName = "com.ibm.streams.sws";

  public StreamsTraceLogger(String traceDir) {
    // pass config info in here and set the logger properties for rolling file/size and path
    String logPathAndFile = new String(traceDir + "/sws.%g.log");
    int logFileSize = 1024000;
    int numLogs = 5;
    myLogger = Logger.getLogger(loggerName);
    try {
      FileHandler handler = new FileHandler(logPathAndFile, logFileSize, numLogs, true);
      handler.setFormatter(new SimpleFormatter());
      myLogger.addHandler(handler);
      myLogger.setLevel(java.util.logging.Level.ALL);
    } catch (IOException es) {
    }
  }

  public boolean needsClose() {
    return true;
  }

  public void plainText(Object message) {
    myLogger.log(mapToLoggerLevel(TraceLogger.Level.INFO), message.toString());
  }

  public void info(Object message) {
    if (isLevelEnabled(TraceLogger.Level.INFO)) {
      traceMessage(TraceLogger.Level.INFO, message.toString(), null, "", null);
    }
  }

  public void info(Object message, Throwable throwable) {
    if (isLevelEnabled(TraceLogger.Level.INFO)) {
      traceMessage(TraceLogger.Level.INFO, message.toString(), throwable, "", null);
    }
  }

  public void warn(Object message) {
    if (isLevelEnabled(TraceLogger.Level.WARN)) {
      traceMessage(TraceLogger.Level.WARN, message.toString(), null, "", null);
    }
  }

  public void warn(Object message, Throwable throwable) {
    if (isLevelEnabled(TraceLogger.Level.WARN)) {
      traceMessage(TraceLogger.Level.WARN, message.toString(), throwable, "", null);
    }
  }

  public void error(Object message) {
    if (isLevelEnabled(TraceLogger.Level.ERROR)) {
      traceMessage(TraceLogger.Level.ERROR, message.toString(), null, "", null);
    }
  }

  public void error(Object message, Throwable throwable) {
    if (isLevelEnabled(TraceLogger.Level.ERROR)) {
      traceMessage(TraceLogger.Level.ERROR, message.toString(), throwable, "", null);
    }
  }

  public void trace(Object message) {
    if (isLevelEnabled(TraceLogger.Level.TRACE)) {
      traceMessage(TraceLogger.Level.TRACE, message.toString(), null, "", null);
    }
  }

  public void trace(Object message, Throwable throwable) {
    if (isLevelEnabled(TraceLogger.Level.TRACE)) {
      traceMessage(TraceLogger.Level.TRACE, message.toString(), throwable, "", null);
    }
  }

  public void debug(Object message) {
    if (isLevelEnabled(TraceLogger.Level.DEBUG)) {
      traceMessage(TraceLogger.Level.DEBUG, message.toString(), null, "", null);
    }
  }

  public void debug(Object message, Throwable throwable) {
    if (isLevelEnabled(TraceLogger.Level.DEBUG)) {
      traceMessage(TraceLogger.Level.DEBUG, message.toString(), throwable, "", null);
    }
  }

  public void fatal(Object message, Throwable throwable) {
    if (isLevelEnabled(TraceLogger.Level.ERROR)) {
      traceMessage(TraceLogger.Level.ERROR, message.toString(), throwable, "", null);
    }
  }

  public void close() {}

  public boolean isLevelEnabled(TraceLogger.Level level) {
    return TraceManager.isLevelOn(level);
  }

  //  private void traceMessage(final TraceLogger.Level level, final String msg, final Throwable t,
  // final String
  // methodName, TRCAspect... aspects) {
  private void traceMessage(
      final TraceLogger.Level level,
      final String msg,
      final Throwable t,
      final String methodName,
      final String aspects) {
    // put the aspects and method name into the log4j MDC
    if (t != null) myLogger.log(mapToLoggerLevel(level), msg, t);
    else myLogger.log(mapToLoggerLevel(level), msg);
  }

  private static java.util.logging.Level mapToLoggerLevel(final TraceLogger.Level level) {
    if (level == null) return java.util.logging.Level.OFF;
    switch (level) {
      case TRACE:
        return java.util.logging.Level.FINE;
      case DEBUG:
        return java.util.logging.Level.FINEST;
      case INFO:
        return java.util.logging.Level.INFO;
      case WARN:
        return java.util.logging.Level.WARNING;
      case ERROR:
        return java.util.logging.Level.SEVERE;
      default:
        return java.util.logging.Level.OFF;
    }
  }
}
