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

package com.ibm.streams.operator.internal.logtrace.log4j;

import com.ibm.streams.operator.internal.logtrace.LoggerAspects;
import com.ibm.streams.operator.internal.logtrace.MessageSender;
import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.Level;
import org.apache.log4j.spi.LocationInfo;
import org.apache.log4j.spi.LoggingEvent;

/**
 * Abstract Appender for execution in an operator runtime.
 *
 * @param <L> Log level enumeration
 */
abstract class OperatorRuntimeAppender<L extends Enum<?>> extends AppenderSkeleton {

  private final MessageSender<L> sender;
  private boolean closed;

  OperatorRuntimeAppender(MessageSender<L> sender) {
    this.sender = sender;
  }

  /** */
  @Override
  public final synchronized void close() {
    closed = true;
  }

  @Override
  public boolean requiresLayout() {
    return false;
  }

  @Override
  protected synchronized void append(LoggingEvent event) {
    // Ignore logging methods if closed.
    if (closed) return;

    final String aspects = LoggerAspects.getLoggerAspectString(event.getLoggerName());

    final LocationInfo location = event.getLocationInformation();

    int line = MessageSender.LINE_NA;
    final String lineS = location.getLineNumber();
    if (!LocationInfo.NA.equals(lineS)) {
      try {
        line = Integer.parseInt(lineS);
      } catch (NumberFormatException e) {
      }
    }

    final L level = getLevelEnum(event.getLevel());

    sender.messageSend(
        level,
        event.getRenderedMessage(),
        aspects,
        location.getClassName().concat(".").concat(location.getMethodName()),
        location.getFileName(),
        line);

    final String[] excLines = event.getThrowableStrRep();
    if (excLines != null) {
      for (String excLine : excLines) sender.messageSend(level, excLine, aspects, null, null, 0);
    }
  }

  /**
   * Get the C++ enum value for the log record level.
   *
   * @param level LogRecord's level value.
   * @return Value matching ordinal for C++.
   */
  protected abstract L getLevelEnum(Level level);
}
