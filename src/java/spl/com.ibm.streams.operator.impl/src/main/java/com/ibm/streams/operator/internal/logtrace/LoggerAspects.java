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

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public final class LoggerAspects {

  /** Default aspect value when user does not supply any. */
  public static final String DEFAULT_ASPECT = "";

  /** Map from logger names to associated aspects. */
  private static Map<String, String> loggerAspects = new ConcurrentHashMap<String, String>();

  /**
   * Set the aspects for a named logger.
   *
   * @param loggerName Logger name
   * @param aspects Aspects to associate with the logger.
   */
  public static void setLoggerAspects(String loggerName, String... aspects) {
    loggerAspects.put(loggerName, createAspectString(aspects));
  }

  /**
   * Get the Aspect String associated with the logger.
   *
   * @param loggerName Name of the logger, if null then the empty string will be returned.
   * @return The comma separated list of associated aspects or the empty string if no association
   *     exists.
   */
  public static String getLoggerAspectString(String loggerName) {
    if (loggerName == null) return DEFAULT_ASPECT;
    String aspectString = loggerAspects.get(loggerName);
    if (aspectString == null) return DEFAULT_ASPECT;
    return aspectString;
  }

  /**
   * Create a single comma separated string for the aspects. The single string is the value expected
   * by the C++ log and trace calls.
   *
   * @param aspects List of aspects
   * @return Comma separated aspects or "" if no aspects are provided.
   */
  public static String createAspectString(String... aspects) {
    if (aspects == null || aspects.length == 0) return DEFAULT_ASPECT;

    if (aspects.length == 1) {
      String a0 = aspects[0];
      if (a0 == null) return DEFAULT_ASPECT;
      return a0;
    }

    StringBuilder sb = new StringBuilder(128);
    for (String aspect : aspects) {
      if (aspect != null && aspect.length() != 0) {
        sb.append(aspect);
        sb.append(",");
      }
    }
    if (sb.length() == 0) return DEFAULT_ASPECT;
    sb.setLength(sb.length() - 1);
    return sb.toString();
  }
}
