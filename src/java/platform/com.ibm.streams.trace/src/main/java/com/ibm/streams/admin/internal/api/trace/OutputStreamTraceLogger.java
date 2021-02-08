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
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

public class OutputStreamTraceLogger extends TraceLogger {
  private OutputStream os;
  private PrintWriter writer;

  // static private DateFormat df = new SimpleDateFormat("MM/dd/yy", new Locale("en", "US"));
  // static private SimpleDateFormat tf = new SimpleDateFormat("HH:mm:ss.SSS", new Locale("en",
  // "US"));
  private static SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ");

  static {
    df.setTimeZone(TimeZone.getTimeZone("GMT"));
  }

  public OutputStreamTraceLogger(OutputStream out) {
    os = out;
    writer = new PrintWriter(new OutputStreamWriter(os));
  }

  public boolean needsClose() {
    if (os == System.out || os == System.err) {
      return false;
    } else {
      return true;
    }
  }

  public void plainText(Object message) {
    writer.println(message.toString());
    writer.flush();
  }

  public void info(Object message) {
    if (TraceManager.isLevelOn(Level.INFO)) {
      String msg = formatMessage(Level.INFO, message);
      writer.println(msg);
      writer.flush();
    }
  }

  public void info(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.INFO)) {
      info(message);
      logException(throwable);
      writer.flush();
    }
  }

  public void warn(Object message) {
    if (TraceManager.isLevelOn(Level.WARN)) {
      String msg = formatMessage(Level.WARN, message);
      writer.println(msg);
      writer.flush();
    }
  }

  public void warn(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.WARN)) {
      warn(message);
      logException(throwable);
      writer.flush();
    }
  }

  public void error(Object message) {
    if (TraceManager.isLevelOn(Level.ERROR)) {
      String msg = formatMessage(Level.ERROR, message);
      writer.println(msg);
      writer.flush();
    }
  }

  public void error(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.ERROR)) {
      error(message);
      logException(throwable);
      writer.flush();
    }
  }

  public void trace(Object message) {
    if (TraceManager.isLevelOn(Level.TRACE)) {
      String msg = formatMessage(Level.TRACE, message);
      writer.println(msg);
      writer.flush();
    }
  }

  public void trace(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.TRACE)) {
      trace(message);
      logException(throwable);
      writer.flush();
    }
  }

  public void debug(Object message) {
    if (TraceManager.isLevelOn(Level.DEBUG)) {
      String msg = formatMessage(Level.DEBUG, message);
      writer.println(msg);
      writer.flush();
    }
  }

  public void debug(Object message, Throwable throwable) {
    if (TraceManager.isLevelOn(Level.DEBUG)) {
      debug(message);
      logException(throwable);
      writer.flush();
    }
  }

  public void close() {
    try {
      writer.flush();
      writer.close();
    } catch (Exception e) {
      error("Failed to close trace file", e);
    }
  }

  private String formatMessage(Level level, Object message) {
    long now = System.currentTimeMillis();
    Date d = new Date(now);
    StringBuilder sb = new StringBuilder();
    sb.append(df.format(d));
    // sb.append(" ");
    // sb.append(tf.format(d));
    sb.append(" ");
    switch (level) {
      case INFO:
        sb.append("INFO: ");
        break;
      case WARN:
        sb.append("WARNING: ");
        break;
      case ERROR:
        sb.append("ERROR: ");
        break;
      case TRACE:
        sb.append("TRACE: ");
        break;
      case DEBUG:
        sb.append("DEBUG: ");
        break;
      default:
        sb.append("INFO: ");
        break;
    }
    long threadID = Thread.currentThread().getId();
    sb.append("Thread:");
    sb.append(threadID);
    sb.append(" ");
    if (message != null) {
      sb.append(message.toString());
    }

    return sb.toString();
  }

  private void logException(Throwable throwable) {
    throwable.printStackTrace(writer);
  }
}
