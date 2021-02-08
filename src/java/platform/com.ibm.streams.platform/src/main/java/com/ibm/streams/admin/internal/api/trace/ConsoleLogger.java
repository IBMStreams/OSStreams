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

import com.ibm.icu.text.DateFormat;
import com.ibm.icu.text.SimpleDateFormat;
import com.ibm.icu.util.TimeZone;
import com.ibm.streams.admin.internal.core.AdminMessageBundle;
import com.ibm.streams.messages.MessageKey;
import com.ibm.streams.messages.MessageUtilities;
import java.util.Date;
import java.util.Locale;
import java.util.logging.ConsoleHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ConsoleLogger implements StreamsLogger {

  private StreamsLogLevel logLevel = StreamsLogLevel.WARN;
  private Logger logger;
  private TimeZone tz;
  private DateFormat df;

  public ConsoleLogger() {
    // date formatting to ISO8601 format
    tz = TimeZone.getDefault();
    // df = new SimpleDateFormat("yyyy-MM-dd'T'HH:mmZ");
    df = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ");
    df.setTimeZone(tz);

    // create logger
    logger = Logger.getLogger("com.ibm.im.streams.log");
    Handler[] handlers = logger.getHandlers();
    if (handlers.length == 0) {
      // handler does not exist, must be a new one, so set up file handler
      logger.setUseParentHandlers(false);
      try {
        ConsoleHandler ch = new ConsoleHandler();
        ch.setFormatter(new StreamsLogLineFormatter());
        logger.addHandler(ch);
      } catch (Exception e) {
        System.out.println(
            AdminMessageBundle.getPlainFormattedString(
                Locale.getDefault(),
                AdminMessageBundle.Id.EXC_LOG_HANDLER.toString(),
                e.getMessage()));
      }
    }

    setLevel(logLevel);
  }

  public void close() {
    for (Handler handler : logger.getHandlers()) {
      handler.close();
      logger.removeHandler(handler);
    }
  }

  private String formatMessage(String message, String[] aspects, long date) {
    if (TraceManager.isLevelOn(TraceLogger.Level.DEBUG)) {
      Trace.logDebug("##### log msg: " + message);
    }

    StringBuilder sb = new StringBuilder();
    sb.append("[");
    if (0 == date) {
      sb.append(df.format(new Date()));
    } else {
      sb.append(df.format(new Date(date)));
    }
    sb.append("] [");
    if (aspects != null) {
      boolean firstOne = true;
      for (String aspect : aspects) {
        if (!firstOne) {
          sb.append(",");
        } else {
          firstOne = false;
        }
        sb.append(aspect);
      }
    }
    sb.append("]");

    sb.append(" ");
    sb.append(message);
    sb.append("\n");

    return sb.toString();
  }

  public void info(String message) {
    info(message, null, 0);
  }

  public void info(String message, String[] aspects) {
    info(message, aspects, 0);
  }

  public void info(String message, String[] aspects, long date) {
    logger.log(Level.INFO, formatMessage(message, aspects, date));
  }

  public void warn(String message) {
    warn(message, null, 0);
  }

  public void warn(String message, String[] aspects) {
    warn(message, aspects, 0);
  }

  public void warn(String message, String[] aspects, long date) {
    logger.log(Level.WARNING, formatMessage(message, aspects, date));
  }

  public void error(String message) {
    error(message, null, 0);
  }

  public void error(String message, String[] aspects) {
    error(message, aspects, 0);
  }

  public void error(String message, String[] aspects, long date) {
    logger.log(Level.SEVERE, formatMessage(message, aspects, date));
  }

  public void logError(MessageKey msgKey) {
    String message = MessageUtilities.getMessageAndPrefix(msgKey);
    error(message, null, 0);
  }

  public void logError(MessageKey msgID, String[] aspects) {
    String message = MessageUtilities.getMessageAndPrefix(msgID);
    error(message, aspects, 0);
  }

  public void logError(MessageKey msgID, String[] aspects, Object... replacements) {
    String message = MessageUtilities.getMessageAndPrefix(msgID, replacements);
    error(message, aspects, 0);
  }

  public void logError(long date, MessageKey msgID, String[] aspects, Object... replacements) {
    String message = MessageUtilities.getMessageAndPrefix(msgID, replacements);
    error(message, aspects, date);
  }

  public void logWarning(MessageKey msgKey) {
    String message = MessageUtilities.getMessageAndPrefix(msgKey);
    warn(message, null, 0);
  }

  public void logWarning(MessageKey msgID, String[] aspects) {
    String message = MessageUtilities.getMessageAndPrefix(msgID);
    warn(message, aspects, 0);
  }

  public void logWarning(MessageKey msgID, String[] aspects, Object... replacements) {
    String message = MessageUtilities.getMessageAndPrefix(msgID, replacements);
    warn(message, aspects, 0);
  }

  public void logWarning(long date, MessageKey msgID, String[] aspects, Object... replacements) {
    String message = MessageUtilities.getMessageAndPrefix(msgID, replacements);
    warn(message, aspects, date);
  }

  public void logInfo(MessageKey msgKey) {
    String message = MessageUtilities.getMessageAndPrefix(msgKey);
    info(message, null, 0);
  }

  public void logInfo(MessageKey msgID, String[] aspects) {
    String message = MessageUtilities.getMessageAndPrefix(msgID);
    info(message, aspects, 0);
  }

  public void logInfo(MessageKey msgID, String[] aspects, Object... replacements) {
    String message = MessageUtilities.getMessageAndPrefix(msgID, replacements);
    info(message, aspects, 0);
  }

  public void logInfo(long date, MessageKey msgID, String[] aspects, Object... replacements) {
    String message = MessageUtilities.getMessageAndPrefix(msgID, replacements);
    info(message, aspects, date);
  }

  public void setLevel(String streamsLevel) {
    if (streamsLevel.equalsIgnoreCase("error")) {
      logger.setLevel(Level.SEVERE);
      logLevel = StreamsLogLevel.ERROR;
    } else if (streamsLevel.equalsIgnoreCase("info")) {
      logger.setLevel(Level.INFO);
      logLevel = StreamsLogLevel.INFO;
    } else {
      logger.setLevel(Level.WARNING);
      logLevel = StreamsLogLevel.WARN;
    }
  }

  public void setLevel(StreamsLogLevel level) {
    logLevel = level;
    if (level == StreamsLogLevel.ERROR) {
      logger.setLevel(Level.SEVERE);
    } else if (level == StreamsLogLevel.INFO) {
      logger.setLevel(Level.INFO);
    } else {
      logger.setLevel(Level.WARNING);
    }
  }

  public StreamsLogLevel getStreamsLogLevel() {
    return logLevel;
  }
}
