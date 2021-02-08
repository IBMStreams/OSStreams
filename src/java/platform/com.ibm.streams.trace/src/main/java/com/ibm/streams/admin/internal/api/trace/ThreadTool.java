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

import java.lang.management.LockInfo;
import java.lang.management.ManagementFactory;
import java.lang.management.MonitorInfo;
import java.lang.management.ThreadInfo;
import java.lang.management.ThreadMXBean;
import java.util.List;

public class ThreadTool {
  private static final String LINE_SEP = System.getProperty("line.separator");
  private static final String DBLQUOTE = "\"";
  private static final String AT = "@";
  private static final String UNOWNED = "<unowned>";
  private static final String UNKNOWN = "<unknown>";

  public static String dumpThreads() {
    return dumpThreads(null);
  }

  public static String dumpThreads(List<Thread.State> threadStates) {
    StringBuilder sb = new StringBuilder();

    ThreadMXBean threadBean = ManagementFactory.getThreadMXBean();

    indent(4, sb);
    sb.append("Thread count:");
    sb.append(threadBean.getThreadCount());
    sb.append(" Daemon thread count:");
    sb.append(threadBean.getDaemonThreadCount());
    sb.append(" Peak thread count:");
    sb.append(threadBean.getPeakThreadCount());
    sb.append(" Total started thread count:");
    sb.append(threadBean.getTotalStartedThreadCount());
    sb.append(LINE_SEP);

    sb.append(LINE_SEP);

    long[] deadlockedThreads = threadBean.findDeadlockedThreads();
    indent(4, sb);
    sb.append("Deadlocked threads: [");
    if (deadlockedThreads != null) {
      for (int i = 0; i < deadlockedThreads.length; i++) {
        if (i > 0) {
          sb.append(", ");
        }
        sb.append(deadlockedThreads[i]);
      }
    }
    sb.append("]");
    sb.append(LINE_SEP);

    long[] monitorDeadlockedThreads = threadBean.findMonitorDeadlockedThreads();
    indent(4, sb);
    sb.append("Monitor deadlocked threads: [");
    if (monitorDeadlockedThreads != null) {
      for (int i = 0; i < monitorDeadlockedThreads.length; i++) {
        if (i > 0) {
          sb.append(", ");
        }
        sb.append(monitorDeadlockedThreads[i]);
      }
    }
    sb.append("]");
    sb.append(LINE_SEP);

    sb.append(LINE_SEP);

    // ThreadCounter threadCounter = new ThreadCounter();
    ThreadInfo[] threads = threadBean.dumpAllThreads(true, true);
    for (int i = 0; i < threads.length; i++) {
      // threadCounter.count(threads[i]);
      if (includeThread(threads[i].getThreadState(), threadStates)) {
        threadInfoToString(threadBean, threads[i], sb);
      }
    }

    // output summary by thread states
    /*
    sb.append(LINE_SEP);
    indent(4, sb);
    sb.append("By thread state:");
    sb.append(LINE_SEP);
    Map<Thread.State, ThreadCounter.Counter> stateMap = threadCounter.getStateMap();
    Set<Thread.State> states = stateMap.keySet();
    for (Thread.State state : states) {
      ThreadCounter.Counter counter = stateMap.get(state);
      indent(8, sb);
      sb.append(state);
      sb.append("=");
      sb.append(counter.getCount());
      sb.append(LINE_SEP);
    }*/

    /*
    indent(4, sb);
    sb.append("By thread name:");
    sb.append(LINE_SEP);
    Map<String, ThreadCounter.Counter> nameMap = threadCounter.getNameMap();
    Set<String> names = nameMap.keySet();
    for (String name : names) {
      //ThreadCounter.Counter counter = nameMap.get(name);
      indent(8, sb);
      sb.append(name);
      sb.append("=");
      sb.append(counter.getCount());
      sb.append(LINE_SEP);
    }*/

    return sb.toString();
  }

  public static StringBuilder threadInfoToString(
      ThreadMXBean threadBean, ThreadInfo threadInfo, StringBuilder sb) {
    long threadID = threadInfo.getThreadId();

    indent(4, sb);
    sb.append(DBLQUOTE);
    sb.append(threadInfo.getThreadName());
    sb.append(DBLQUOTE);
    sb.append(" ID:");
    sb.append(threadInfo.getThreadId());
    sb.append(" State:");
    sb.append(threadInfo.getThreadState());
    sb.append(LINE_SEP);

    if (threadBean.isThreadCpuTimeEnabled()) {
      indent(8, sb);
      sb.append("CPU time:");
      sb.append(threadBean.getThreadCpuTime(threadID));
      sb.append(" User time:");
      sb.append(threadBean.getThreadUserTime(threadID));
      sb.append(LINE_SEP);
    }

    LockInfo lock = threadInfo.getLockInfo();
    if (lock != null) {
      indent(8, sb);
      sb.append("Waiting on:");
      sb.append(lock.getClassName());
      sb.append(AT);
      sb.append(lock.getIdentityHashCode());
      sb.append(" Owned by:");
      String lockOwnerName = threadInfo.getLockOwnerName();
      if (lockOwnerName != null) {
        sb.append(lockOwnerName);
        sb.append(" ID:");
        sb.append(threadInfo.getLockOwnerId());
      } else {
        sb.append(UNOWNED);
      }
      sb.append(LINE_SEP);
    }

    MonitorInfo[] monitors = threadInfo.getLockedMonitors();
    if ((monitors != null) && (monitors.length > 0)) {
      indent(8, sb);
      sb.append("Holding monitors:");
      sb.append(LINE_SEP);
      for (int i = 0; i < monitors.length; i++) {
        indent(12, sb);
        sb.append(monitors[i].getClassName());
        sb.append(AT);
        sb.append(monitors[i].getIdentityHashCode());
        sb.append(LINE_SEP);
      }
    }

    LockInfo[] synchronizers = threadInfo.getLockedSynchronizers();
    if ((synchronizers != null) & (synchronizers.length > 0)) {
      indent(8, sb);
      sb.append("Holding synchronizer:");
      sb.append(LINE_SEP);
      for (int i = 0; i < synchronizers.length; i++) {
        indent(12, sb);
        sb.append(synchronizers[i].getClassName());
        sb.append(AT);
        sb.append(synchronizers[i].getIdentityHashCode());
        sb.append(LINE_SEP);
      }
    }

    if (threadBean.isThreadContentionMonitoringEnabled()) {
      indent(8, sb);
      sb.append("Waited count:");
      sb.append(threadInfo.getWaitedCount());
      sb.append(" Waited time:");
      sb.append(threadInfo.getWaitedTime());
      sb.append(" Blocked count:");
      sb.append(threadInfo.getBlockedCount());
      sb.append(" Blocked time:");
      sb.append(threadInfo.getBlockedTime());
      sb.append(LINE_SEP);
    }

    StackTraceElement[] stack = threadInfo.getStackTrace();
    if (stack != null) {
      for (int i = 0; i < stack.length; i++) {
        indent(12, sb);
        sb.append("at ");
        sb.append(stack[i].getClassName());
        sb.append(".");
        sb.append(stack[i].getMethodName());
        sb.append("(");
        String fn = stack[i].getFileName();
        if (fn != null) {
          sb.append(fn);
        } else {
          sb.append(UNKNOWN);
        }
        sb.append(":");
        int lineNumber = stack[i].getLineNumber();
        if (lineNumber >= 0) {
          sb.append(lineNumber);
        } else {
          sb.append(UNKNOWN);
        }
        sb.append(")");
        sb.append(LINE_SEP);
      }
    }

    sb.append(LINE_SEP);

    return sb;
  }

  public static String detectDeadlocks() {

    String threadDump = null;

    ThreadMXBean threadBean = ManagementFactory.getThreadMXBean();
    long[] deadlockedThreads = threadBean.findDeadlockedThreads();
    long[] monitorDeadlockedThreads = threadBean.findMonitorDeadlockedThreads();
    if (((deadlockedThreads != null) && (deadlockedThreads.length > 0))
        || ((monitorDeadlockedThreads != null) && (monitorDeadlockedThreads.length > 0))) {
      StringBuilder sb = new StringBuilder();
      if ((deadlockedThreads != null) && (deadlockedThreads.length > 0)) {
        indent(4, sb);
        sb.append("Deadlocked threads: [");
        if (deadlockedThreads != null) {
          for (int i = 0; i < deadlockedThreads.length; i++) {
            if (i > 0) {
              sb.append(", ");
            }
            sb.append(deadlockedThreads[i]);
          }
        }
        sb.append("]");
        sb.append(LINE_SEP);

        ThreadInfo[] threads = threadBean.getThreadInfo(deadlockedThreads, true, true);
        for (int i = 0; i < threads.length; i++) {
          threadInfoToString(threadBean, threads[i], sb);
        }
      }
      if ((monitorDeadlockedThreads != null) && (monitorDeadlockedThreads.length > 0)) {
        indent(4, sb);
        sb.append("Monitor deadlocked threads: [");
        if (monitorDeadlockedThreads != null) {
          for (int i = 0; i < monitorDeadlockedThreads.length; i++) {
            if (i > 0) {
              sb.append(", ");
            }
            sb.append(monitorDeadlockedThreads[i]);
          }
        }
        sb.append("]");
        sb.append(LINE_SEP);

        sb.append(LINE_SEP);

        ThreadInfo[] threads = threadBean.getThreadInfo(monitorDeadlockedThreads, true, true);
        for (int i = 0; i < threads.length; i++) {
          threadInfoToString(threadBean, threads[i], sb);
        }
      }

      threadDump = sb.toString();
    }

    return threadDump;
  }

  private static StringBuilder indent(int n, StringBuilder sb) {
    for (int i = 0; i < n; i++) {
      sb.append(" ");
    }

    return sb;
  }

  private static boolean includeThread(Thread.State state, List<Thread.State> states) {
    if (states == null || states.isEmpty()) {
      return true;
    }

    return states.contains(state);
  }
}
