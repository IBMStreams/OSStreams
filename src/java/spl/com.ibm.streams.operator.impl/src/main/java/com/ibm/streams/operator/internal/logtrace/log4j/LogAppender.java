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

import com.ibm.streams.operator.internal.logtrace.Log;
import com.ibm.streams.operator.internal.logtrace.MessageSender;
import com.ibm.streams.operator.log4j.LogLevel;
import org.apache.log4j.Level;
import org.apache.log4j.Priority;

/** Appender for SPL log facility. */
public class LogAppender extends OperatorRuntimeAppender<Log> {

  public static final String APPENDER_PROPERTY = "com.ibm.streams.operator.log4j.appender.SPLLOG";

  public LogAppender(MessageSender<Log> sender) {
    super(sender);
    setThreshold(LogLevel.INFO);
  }

  /** LogHandler only ever logs at LogLevel.INFO or higher. */
  @Override
  public void setThreshold(final Priority newLevel) {
    if (Level.OFF.equals(newLevel)) {
      super.setThreshold(newLevel);
      return;
    }
    if (newLevel.toInt() >= LogLevel.INFO.toInt()) {
      super.setThreshold(newLevel);
      return;
    }
    super.setThreshold(LogLevel.INFO);
  }

  @Override
  protected Log getLevelEnum(final Level level) {
    final int il = level.toInt();

    assert il >= LogLevel.INFO.toInt();

    if (il < LogLevel.WARN.toInt()) return Log.info;

    if (il < LogLevel.ERROR.toInt()) return Log.warn;

    return Log.error;
  }
}
