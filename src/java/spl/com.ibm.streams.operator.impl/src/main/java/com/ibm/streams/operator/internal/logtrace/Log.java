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

package com.ibm.streams.operator.internal.logtrace;

import com.ibm.streams.operator.logging.LogLevel;
import java.util.logging.Level;

/**
 * Internal enumeration SPL log levels. Since log is exposed through two different apis
 * (java.util.logging and log4j) we provide our own enumeration of log levels to have consistency
 * within the SPL Java code. This is internal code only and not exposed to the Java Operator API.
 */
public enum Log {
  off(Level.OFF, org.apache.log4j.Level.OFF),
  error(LogLevel.ERROR, com.ibm.streams.operator.log4j.LogLevel.ERROR),
  warn(LogLevel.WARN, com.ibm.streams.operator.log4j.LogLevel.WARN),
  info(LogLevel.INFO, com.ibm.streams.operator.log4j.LogLevel.INFO);

  /** java.util.logging level for SPL log level. */
  private final Level loggingLevel;

  /** log4j level for SPL log level. */
  private final org.apache.log4j.Level log4jLevel;

  private Log(Level loggingLevel, org.apache.log4j.Level log4jLevel) {
    this.loggingLevel = loggingLevel;
    this.log4jLevel = log4jLevel;
  }

  /** java.util.logging level for SPL log level. */
  public Level getLoggingLevel() {
    return loggingLevel;
  }

  /** log4j level for SPL log level. */
  public org.apache.log4j.Level getLog4jLevel() {
    return log4jLevel;
  }
}
