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

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;
import java.util.logging.Formatter;
import java.util.logging.Level;
import java.util.logging.LogRecord;

public class SingleLineTraceFormatter extends Formatter {

  // private static DateFormat df = new SimpleDateFormat("MM/dd/yy", new Locale("en", "US"));
  // private static SimpleDateFormat tf = new SimpleDateFormat("HH:mm:ss:SSS", new Locale("en",
  // "US"));
  private static SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ");

  static {
    df.setTimeZone(TimeZone.getTimeZone("GMT"));
  }

  public String format(LogRecord rec) {
    StringBuffer sb = new StringBuffer();

    // DateFormat df = DateFormat.getDateInstance(DateFormat.SHORT);
    // SimpleDateFormat tf = new SimpleDateFormat("HH:mm:ss:SSS");
    Date date = new Date(rec.getMillis());

    synchronized (df) {
      sb.append(df.format(date));
    }
    sb.append(" ");
    // synchronized (tf) {
    //  sb.append(tf.format(date));
    // }
    // sb.append(" ");

    sb.append(formatLevel(rec.getLevel()));

    sb.append(" ");
    sb.append(rec.getMessage());
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
    } else if (level == Level.FINE) {
      return "INFO:";
    } else if (level == Level.FINER) {
      return "DEBUG:";
    } else if (level == Level.FINEST) {
      return "TRACE:";
    } else {
      return "";
    }
  }
}
