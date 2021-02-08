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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.logging.FileHandler;
import java.util.logging.Logger;

// import com.ibm.streams.admin.internal.api.StreamsException;
// import com.ibm.streams.admin.internal.api.config.DomainConfig;
// import com.ibm.streams.admin.internal.core.SingleLineTraceFormatter;

public class JavaUtilTraceLogger extends TraceLogger {

  // constants
  public static final String LOGGER_ID = "com.ibm.streams.platform.trace";

  // static data members
  private static boolean _stdoutRedirected;
  private static PrintStream _stdout;
  private static PrintStream _stderr;

  // data members
  protected FileHandler fh;
  protected Logger logger;

  /**
   * Sets up a JavaUtilTraceLogger with the TraceManager. Also redirects stdout and stderr to the
   * logger
   *
   * @param tracePath - String
   * @param maxSizeBytes - int
   * @param maxCount - int
   * @param level - TraceLogger.Level
   * @return JavaUtilTraceLogger
   */
  public static JavaUtilTraceLogger createLogger(
      String tracePath, int maxSizeBytes, int maxCount, TraceLogger.Level level) {
    return createLogger(TraceContextType.NONE, tracePath, maxSizeBytes, maxCount, level, true);
  }

  /**
   * Sets up a JavaUtilTraceLogger with the TraceManager. Also redirects stdout and stderr to the
   * logger
   *
   * @param traceType - TraceContextType
   * @param tracePath - String
   * @param maxSizeBytes - int
   * @param maxCount - int
   * @param level - TraceLogger.Level
   * @return JavaUtilTraceLogger
   */
  public static JavaUtilTraceLogger createLogger(
      TraceContextType traceType,
      String tracePath,
      int maxSizeBytes,
      int maxCount,
      TraceLogger.Level level) {
    return createLogger(traceType, tracePath, maxSizeBytes, maxCount, level, true);
  }

  /**
   * Sets up a JavaUtilTraceLogger with the TraceManager. Also redirects stdout and stderr to the
   * logger
   *
   * @param traceType - TraceContextType
   * @param tracePath - String
   * @param maxSizeBytes - int
   * @param maxCount - int
   * @param level - TraceLogger.Level
   * @return JavaUtilTraceLogger
   */
  public static JavaUtilTraceLogger createLogger(
      TraceContextType traceType,
      String tracePath,
      int maxSizeBytes,
      int maxCount,
      TraceLogger.Level level,
      boolean closeStdoutStderr) {
    // set up logger
    JavaUtilTraceLogger traceLogger =
        new JavaUtilTraceLogger(traceType, tracePath, maxSizeBytes, maxCount);
    TraceManager.setCurrentContextType(traceType);
    TraceManager.setLevel(traceType, level);
    TraceManager.setTraceLogger(traceType, traceLogger);

    // re-direct stdout and stderr
    redirectStdoutStderr(closeStdoutStderr);

    return traceLogger;
  }

  /** Redirect stdout/stderr to the trace manager */
  public static void redirectStdoutStderr(boolean close) {
    // re-direct stdout and stderr
    if (!_stdoutRedirected) {
      ConsoleTraceOutputStream stdout = new ConsoleTraceOutputStream(true);
      ConsoleTraceOutputStream stderr = new ConsoleTraceOutputStream(false);
      PrintStream stdoutPs = new PrintStream(stdout);
      PrintStream stderrPs = new PrintStream(stderr);
      _stdout = System.out;
      _stderr = System.err;
      System.setOut(stdoutPs);
      System.setErr(stderrPs);
      _stdoutRedirected = true;

      if (close) {
        _stdout.close();
        _stderr.close();
        _stdout = null;
        _stderr = null;
      }
    }
  }

  /** Restore stdout/stderr */
  public static void restoreStdoutStderr() {
    if (_stdoutRedirected && _stdout != null && _stderr != null) {
      System.setOut(_stdout);
      System.setErr(_stderr);
      _stdout = null;
      _stderr = null;
      _stdoutRedirected = false;
    }
  }

  /**
   * Constructor
   *
   * @param path - String
   * @param maxSizeBytes - int
   * @param count - int
   */
  public JavaUtilTraceLogger(TraceContextType traceType, String path, int maxSizeBytes, int count) {
    logger = Logger.getLogger(traceType.toString());

    try {
      logger.setUseParentHandlers(false);
      fh = new FileHandler(path, maxSizeBytes, count);
      fh.setFormatter(new SingleLineTraceFormatter());
      logger.addHandler(fh);
      logger.setLevel(java.util.logging.Level.ALL);
    } catch (IOException ioe) {
      System.out.println("JavaUtilTraceLogger.constructor IOException: " + ioe.getMessage());
    }
  }

  public boolean needsClose() {
    return true;
  }

  public void close() {
    try {
      if (fh != null) {
        fh.flush();
        fh.close();
      }

      if (logger != null) {
        logger.removeHandler(fh);
      }
    } catch (Exception e) {
      error("Failed to close trace file", e);
    }
  }

  public String toString() {
    return logger.getName();
  }

  public void plainText(Object message) {
    if (logger != null) {
      logger.log(getJavaTraceLevel(TraceLogger.Level.INFO), message.toString());
    }
  }

  public void info(Object message) {
    if (TraceManager.isLevelOn(Level.INFO)) {
      if (logger != null) {
        logger.log(getJavaTraceLevel(TraceLogger.Level.INFO), formatMsg(message.toString()));
      }
    }
  }

  public void info(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.INFO)) {
      if (logger != null) {
        java.util.logging.Level javaLevel = getJavaTraceLevel(TraceLogger.Level.INFO);
        logger.log(javaLevel, formatMsg(message.toString()));
        logException(javaLevel, TraceLogger.Level.INFO, throwable);
      }
    }
  }

  public void warn(Object message) {
    if (TraceManager.isLevelOn(Level.WARN)) {
      if (logger != null) {
        logger.log(getJavaTraceLevel(TraceLogger.Level.WARN), formatMsg(message.toString()));
      }
    }
  }

  public void warn(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.WARN)) {
      if (logger != null) {
        java.util.logging.Level javaLevel = getJavaTraceLevel(TraceLogger.Level.WARN);
        logger.log(javaLevel, formatMsg(message.toString()));
        logException(javaLevel, TraceLogger.Level.WARN, throwable);
      }
    }
  }

  public void error(Object message) {
    if (TraceManager.isLevelOn(Level.ERROR)) {
      if (logger != null) {
        logger.log(getJavaTraceLevel(TraceLogger.Level.ERROR), formatMsg(message.toString()));
      }
    }
  }

  public void error(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.ERROR)) {
      if (logger != null) {
        java.util.logging.Level javaLevel = getJavaTraceLevel(TraceLogger.Level.ERROR);
        logger.log(javaLevel, formatMsg(message.toString()));
        logException(javaLevel, TraceLogger.Level.ERROR, throwable);
      }
    }
  }

  public void trace(Object message) {
    if (TraceManager.isLevelOn(Level.TRACE)) {
      if (logger != null) {
        logger.log(getJavaTraceLevel(TraceLogger.Level.TRACE), formatMsg(message.toString()));
      }
    }
  }

  public void trace(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.TRACE)) {
      if (logger != null) {
        java.util.logging.Level javaLevel = getJavaTraceLevel(TraceLogger.Level.TRACE);
        logger.log(javaLevel, formatMsg(message.toString()));
        logException(javaLevel, TraceLogger.Level.TRACE, throwable);
      }
    }
  }

  public void debug(Object message) {
    if (TraceManager.isLevelOn(Level.DEBUG)) {
      if (logger != null) {
        logger.log(getJavaTraceLevel(TraceLogger.Level.DEBUG), formatMsg(message.toString()));
      }
    }
  }

  public void debug(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.DEBUG)) {
      if (logger != null) {
        java.util.logging.Level javaLevel = getJavaTraceLevel(TraceLogger.Level.DEBUG);
        logger.log(javaLevel, formatMsg(message.toString()));
        logException(javaLevel, TraceLogger.Level.DEBUG, throwable);
      }
    }
  }

  private java.util.logging.Level getJavaTraceLevel(TraceLogger.Level level) {
    if (level == TraceLogger.Level.OFF) {
      return null;
    } else if (level == TraceLogger.Level.ERROR) {
      return java.util.logging.Level.SEVERE;
    } else if (level == TraceLogger.Level.WARN) {
      return java.util.logging.Level.WARNING;
    } else if (level == TraceLogger.Level.INFO) {
      return java.util.logging.Level.INFO;
    } else if (level == TraceLogger.Level.DEBUG) {
      return java.util.logging.Level.FINER;
    } else if (level == TraceLogger.Level.TRACE) {
      return java.util.logging.Level.FINEST;
    } else {
      return null;
    }
  }

  private String formatMsg(String msg) {
    Thread currentThread = Thread.currentThread();
    return "thread:" + currentThread.getId() + "[" + currentThread.getName() + "] " + msg;
  }

  private void logException(
      java.util.logging.Level level, TraceLogger.Level traceLevel, Throwable throwable) {
    StringWriter strWriter = new StringWriter();
    PrintWriter writer = new PrintWriter(strWriter);
    throwable.printStackTrace(writer);
    writer.flush();
    writer.close();
    String s = strWriter.toString();

    try {
      BufferedReader rdr = new BufferedReader(new StringReader(s));
      String line;
      while ((line = rdr.readLine()) != null) {
        logger.log(level, formatMsg(line));
      }
    } catch (IOException ioe) {
    }
  }
}
