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

import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;

public class TraceContext {
  protected TraceContextType contextType;
  protected Level level = Level.INFO;
  protected TraceLogger traceLogger = null;

  public TraceContext() {
    contextType = TraceContextType.NONE;
    level = Level.INFO;
    traceLogger = null;
  }

  public TraceContext(TraceContextType type) {
    contextType = type;
    level = Level.INFO;
    traceLogger = null;
  }

  public TraceContext(TraceContextType contextType, Level level, TraceLogger logger) {
    this.contextType = contextType;
    this.level = level;
    this.traceLogger = logger;
  }

  public void setTraceLogger(TraceLogger logger) {
    close();
    traceLogger = logger;
  }

  public TraceLogger getTraceLogger() {
    return traceLogger;
  }

  public boolean isOpen() {
    return (traceLogger != null);
  }

  public void close() {
    if (traceLogger != null) {
      if (traceLogger.needsClose()) {
        traceLogger.close();
      }
      traceLogger = null;
    }
  }

  public void setLevel(Level level) {
    this.level = level;
  }

  public Level getLevel() {
    if (traceLogger != null) {
      return level;
    } else {
      return Level.OFF;
    }
  }

  public boolean isLevelOn(Level level) {
    if (traceLogger != null) {
      return level.isLevel(this.level);
    } else {
      return false;
    }
  }

  public boolean isEnabled(Level level) {
    return isLevelOn(level);
  }
}
