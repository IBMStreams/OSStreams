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

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class Trace {
  public static final String traceClassName = "com.ibm.streams.admin.internal.api.trace.Trace";
  public static final String consoleOSClassName =
      "com.ibm.streams.admin.internal.api.trace.ConsoleTraceOutputStream";
  public static final String tracePackageName = "com.ibm.streams.admin.internal.api.trace.";
  public static final String trcClassName = "com.ibm.distillery.jtrc.TRCLogger";

  private static long threadDumpInterval = 5 * 60 * 1000; // default every 5 min
  private static Timer threadDumpTimer = null;
  private static TimerTask threadDumpTimerTask = null;

  /*
  static {
    Locale locale = Locale.getDefault();
    DateFormatSymbols dfs = new DateFormatSymbols(locale);
    SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss", dfs);
  }*/

  public static void logPlainText(Object message) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        logger.plainText(message);
      }
    } catch (Exception e) {
    }
  }

  public static void logInfo(Object message) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.INFO)) {
          logger.info(addCallerInfo(message));
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logInfo(Object message, Throwable throwable) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.INFO)) {
          logger.info(addCallerInfo(message), throwable);
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logWarn(Object message) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.WARN)) {
          logger.warn(addCallerInfo(message));
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logWarn(Object message, Throwable throwable) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.WARN)) {
          logger.warn(addCallerInfo(message), throwable);
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logError(Object message) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.ERROR)) {
          logger.error(addCallerInfo(message));
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logError(Object message, Throwable throwable) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.ERROR)) {
          logger.error(addCallerInfo(message), throwable);
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logTrace(Object message) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.TRACE)) {
          logger.trace(addCallerInfo(message));
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logTrace(Object message, Throwable throwable) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.TRACE)) {
          logger.trace(addCallerInfo(message), throwable);
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logDebug(Object message) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.DEBUG)) {
          logger.debug(addCallerInfo(message));
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logDebug(Object message, boolean logStackTrace) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.DEBUG)) {
          logger.debug(addCallerInfo(message));
          if (logStackTrace) {
            dumpCallStack(logger);
          }
        }
      }
    } catch (Exception e) {
    }
  }

  public static void logDebug(Object message, Throwable throwable) {
    try {
      TraceLogger logger = TraceManager.getTraceLogger();
      if (logger != null) {
        if (TraceManager.isLevelOn(TraceLogger.Level.DEBUG)) {
          logger.debug(addCallerInfo(message), throwable);
        }
      }
    } catch (Exception e) {
    }
  }

  public static boolean isEnabled(TraceLogger.Level level) {
    TraceLogger logger = TraceManager.getTraceLogger();
    if (logger != null) {
      return TraceManager.isLevelOn(level);
    } else {
      return false;
    }
  }

  private static String addCallerInfo(Object message) {
    // TraceLogger logger = TraceManager.getTraceLogger();
    String msg = (message != null ? message.toString() : "null");
    StringBuilder sb = new StringBuilder(msg);

    Thread currentThread = Thread.currentThread();
    StackTraceElement[] stack = currentThread.getStackTrace();
    if (stack.length > 2) {
      // find caller
      StackTraceElement caller = null;

      // first find the trace class entry
      int traceIndex = 0;
      for (int i = 0; i < stack.length; i++) {
        if (stack[i].getClassName().equals(traceClassName)) {
          traceIndex = i;
          break;
        }
      }

      // see if the call came from ConsoleTraceOutputStream
      for (int i = traceIndex; i < stack.length; i++) {
        if (stack[i].getClassName().equals(consoleOSClassName)) {
          // logger.debug("Found console os class " + i);
          // advance past trace classes
          for (int j = i + 1; j < stack.length; j++) {
            String cName = stack[j].getClassName();
            if (!cName.startsWith("com.ibm.streams.admin.internal.api.trace.")) {
              traceIndex = j;
              break;
            }
          }
          // now find streams class
          for (int j = traceIndex; j < stack.length; j++) {
            // logger.debug("Looking at class " + stack[j].getClassName());
            if (stack[j].getClassName().startsWith("com.ibm.streams.")) {
              // logger.debug("Found streams class " + stack[j].getClassName() + " " + j);
              traceIndex = j;
              break;
            }
          }
          break;
        }
      }

      // now find the next entry that is not the trace class entry
      for (int i = traceIndex; i < stack.length; i++) {
        String cName = stack[i].getClassName();
        if (!cName.startsWith(tracePackageName)) {
          // logger.debug("Caller found " + cName + " " + i);
          caller = stack[i];
          if (cName.equals(trcClassName)) {
            caller = stack[i + 1];
          }
          break;
        }
      }

      if (caller != null) {
        sb.append(" [");
        sb.append(getStackElement(caller));
        sb.append("]");
      }
    }

    return sb.toString();
  }

  private static void dumpCallStack(TraceLogger logger) {
    Thread currentThread = Thread.currentThread();
    StackTraceElement[] stack = currentThread.getStackTrace();
    if (stack.length > 2) {
      // find caller
      int callerIndex = -1;

      // first find the trace class entry
      int traceIndex = 0;
      for (int i = 0; i < stack.length; i++) {
        if (stack[i].getClassName().equals(traceClassName)) {
          traceIndex = i;
          break;
        }
      }
      // now find the next entry that is not the trace class entry
      for (int i = traceIndex; i < stack.length; i++) {
        if (!stack[i].getClassName().startsWith(tracePackageName)) {
          callerIndex = i;
          break;
        }
      }

      if (callerIndex >= 0) {
        for (int i = callerIndex; i < stack.length; i++) {
          StackTraceElement stackElement = stack[i];
          String msg = "at " + getStackElement(stackElement);

          logger.debug(msg);
        }
      }
    }
  }

  public static String getStackElement(StackTraceElement caller) {
    StringBuilder sb = new StringBuilder();

    sb.append(caller.getClassName());
    sb.append(".");
    sb.append(caller.getMethodName());
    sb.append("(");
    String fn = caller.getFileName();
    if (fn != null) {
      sb.append(fn);
      sb.append(":");
      int ln = caller.getLineNumber();
      if (ln >= 0) {
        sb.append(ln);
      } else {
        sb.append("unavailable");
      }
    }
    sb.append(")");

    return sb.toString();
  }

  public static void setThreadDumpInterval(long interval) {
    threadDumpInterval = interval;
  }

  public static void startThreadDump() {
    if (threadDumpTimerTask == null) {
      // create new timer task
      threadDumpTimerTask = new ThreadDumpTimerTask();
    }
    if (threadDumpTimer == null) {
      // start timer
      threadDumpTimer = new Timer("Thread dump timer");
    }
    threadDumpTimer.scheduleAtFixedRate(
        threadDumpTimerTask, threadDumpInterval, threadDumpInterval);
  }

  public static void stopThreadDump() {
    if (threadDumpTimerTask != null) {
      threadDumpTimerTask.cancel();
      threadDumpTimerTask = null;
    }
    if (threadDumpTimer != null) {
      threadDumpTimer.cancel();
      threadDumpTimer = null;
    }
  }

  public static void dumpThreads(List<Thread.State> threadStates) {
    String threadDump = ThreadTool.dumpThreads(threadStates);
    if (isEnabled(TraceLogger.Level.DEBUG)) {
      logDebug(threadDump);
    }
  }

  public static void dumpThreads(String[] states) {
    List<Thread.State> threadStates = new ArrayList<Thread.State>();
    for (String state : states) {
      try {
        Thread.State threadState = Thread.State.valueOf(state);
        threadStates.add(threadState);
      } catch (Throwable t) {
      }
    }

    dumpThreads(threadStates);
  }

  public static String getThreadState(Thread.State state) {
    switch (state) {
      case NEW:
        return "New";
      case RUNNABLE:
        return "Runnable";
      case BLOCKED:
        return "Blocked";
      case WAITING:
        return "Waiting";
      case TIMED_WAITING:
        return "Timed wait";
      case TERMINATED:
        return "Terminated";
    }
    return "Unknown";
  }
}
