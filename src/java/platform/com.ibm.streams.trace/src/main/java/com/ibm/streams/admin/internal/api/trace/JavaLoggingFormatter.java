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

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.MessageFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;
import java.util.logging.Formatter;
import java.util.logging.Level;
import java.util.logging.LogRecord;

public class JavaLoggingFormatter extends Formatter {
  private static SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ");

  static {
    df.setTimeZone(TimeZone.getTimeZone("GMT"));
  }

  public String format(LogRecord rec) {
    StringBuffer sb = new StringBuffer();

    Date date = new Date(rec.getMillis());

    synchronized (df) {
      sb.append(df.format(date));
    }
    sb.append(" ");

    sb.append(formatLevel(rec.getLevel()));

    sb.append(" ");

    int threadID = rec.getThreadID();
    sb.append("thread:");
    sb.append(threadID);

    sb.append(" ");

    sb.append("[");
    sb.append(rec.getLoggerName());
    sb.append("]");

    sb.append(" ");

    String msg = rec.getMessage();
    Object[] parms = rec.getParameters();
    if (parms == null) {
      sb.append(msg);
    } else {
      sb.append(MessageFormat.format(msg, parms));
    }

    sb.append(" ");

    sb.append("[");
    sb.append(rec.getSourceClassName());
    sb.append(".");
    sb.append(rec.getSourceMethodName());
    sb.append("()]");

    Throwable t = rec.getThrown();
    if (t != null) {
      sb.append("\n");
      StringWriter swriter = new StringWriter();
      PrintWriter writer = new PrintWriter(swriter);
      t.printStackTrace(writer);
      sb.append(swriter.toString());
      try {
        swriter.close();
      } catch (IOException ioe) {
      }
      writer.close();
    }

    sb.append("\n");

    return sb.toString();
  }

  private String formatLevel(Level level) {
    if (level == Level.SEVERE) {
      return "ERROR:";
    } else if (level == Level.WARNING) {
      return "WARNING:";
    } else if (level == Level.INFO) {
      return "INFO:";
    } else if (level == Level.CONFIG) {
      return "CONFIG:";
    } else if (level == Level.FINE) {
      return "DEBUG:";
    } else if (level == Level.FINER) {
      return "DEBUG:";
    } else if (level == Level.FINEST) {
      return "TRACE:";
    } else {
      return level.toString() + ":";
    }
  }
}
