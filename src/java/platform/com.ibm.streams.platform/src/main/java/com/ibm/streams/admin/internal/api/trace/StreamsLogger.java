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

import com.ibm.streams.messages.MessageKey;

public interface StreamsLogger {
  public static enum StreamsLogLevel {
    ERROR,
    WARN,
    INFO
  }

  public void close();

  public void info(String message);

  public void info(String message, String[] aspects);

  public void info(String message, String[] aspects, long date);

  public void warn(String message);

  public void warn(String message, String[] aspects);

  public void warn(String message, String[] aspects, long date);

  public void error(String message);

  public void error(String message, String[] aspects);

  public void error(String message, String[] aspects, long date);

  public void logError(MessageKey msgKey);

  public void logError(MessageKey msgID, String[] aspects);

  public void logError(MessageKey msgID, String[] aspects, Object... replacements);

  public void logError(long date, MessageKey msgID, String[] aspects, Object... replacements);

  public void logWarning(MessageKey msgKey);

  public void logWarning(MessageKey msgID, String[] aspects);

  public void logWarning(MessageKey msgID, String[] aspects, Object... replacements);

  public void logWarning(long date, MessageKey msgID, String[] aspects, Object... replacements);

  public void logInfo(MessageKey msgKey);

  public void logInfo(MessageKey msgID, String[] aspects);

  public void logInfo(MessageKey msgID, String[] aspects, Object... replacements);

  public void logInfo(long date, MessageKey msgID, String[] aspects, Object... replacements);

  public void setLevel(StreamsLogLevel level);

  public void setLevel(String level);

  public StreamsLogLevel getStreamsLogLevel();
}
