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
import java.util.HashMap;

public class TraceManager {

  protected static HashMap<TraceContextType, TraceContext> contextMap =
      new HashMap<TraceContextType, TraceContext>();

  static {
    contextMap.put(TraceContextType.NONE, new TraceContext(TraceContextType.NONE));
  }

  private static final InheritableThreadLocal<TraceContextType> currentContext =
      new InheritableThreadLocal<TraceContextType>() {
        protected TraceContextType initialValue() {
          return TraceContextType.NONE;
        }

        protected TraceContextType childValue(TraceContextType parentValue) {
          return parentValue;
        }
      };

  public static void setTraceLogger(TraceLogger logger) {
    TraceContext context = getThreadContextType();
    context.close();
    context.setTraceLogger(logger);
  }

  public static void setTraceLogger(TraceContextType contextType, TraceLogger logger) {
    TraceContext context = getContext(contextType);
    context.setTraceLogger(logger);
  }

  public static TraceLogger getTraceLogger() {
    TraceContext context = getThreadContextType();
    return context.getTraceLogger();
  }

  public static TraceLogger getTraceLogger(TraceContextType type) {
    TraceContext context = getContext(type);
    return context.getTraceLogger();
  }

  public static boolean isOpen(TraceContextType type) {
    if (contextMap.containsKey(type)) {
      return contextMap.get(type).isOpen();
    }
    return false;
  }

  public static void close() {
    TraceContext context = getThreadContextType();
    context.close();
  }

  public static void close(TraceContextType type) {
    TraceContext context = contextMap.remove(type);
    if (context != null) {
      context.close();
    }
  }

  public static void setLevel(Level level) {
    TraceContext context = getThreadContextType();
    context.setLevel(level);
  }

  public static void setLevel(TraceContextType type, Level level) {
    TraceContext context = getContext(type);
    context.setLevel(level);
  }

  public static Level getLevel() {
    TraceContext context = getThreadContextType();
    return context.getLevel();
  }

  public static Level getLevel(TraceContextType type) {
    TraceContext context = getContext(type);
    return context.getLevel();
  }

  public static boolean isLevelOn(Level level) {
    TraceContext context = getThreadContextType();
    return context.isLevelOn(level);
  }

  public static boolean isLevelOn(TraceContextType type, Level level) {
    TraceContext context = getContext(type);
    return context.isLevelOn(level);
  }

  public static boolean isEnabled(Level level) {
    return isLevelOn(level);
  }

  public static boolean isEnabled(TraceContextType type, Level level) {
    return isLevelOn(type, level);
  }

  public static TraceContextType getCurrentContextType() {
    return currentContext.get();
  }

  public static void setCurrentContextType(TraceContextType type) {
    currentContext.set(type);
  }

  public static void clearCurrentContextType() {
    currentContext.set(TraceContextType.NONE);
  }

  public static void applyContext(TraceContextType type, TraceContextType existingType) {
    TraceContext existingCtxt = contextMap.get(existingType);
    if (existingCtxt != null) {
      contextMap.put(type, existingCtxt);
    }
  }

  protected static TraceContext getThreadContextType() {
    TraceContextType type = currentContext.get();
    return getContext(type);
  }

  protected static TraceContext getContext(TraceContextType type) {
    TraceContext context = contextMap.get(type);
    if (context == null) {
      context = new TraceContext(type);
      contextMap.put(type, context);
    }

    return context;
  }
}
