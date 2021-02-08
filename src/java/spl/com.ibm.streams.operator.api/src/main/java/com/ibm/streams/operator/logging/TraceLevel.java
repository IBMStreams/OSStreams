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

package com.ibm.streams.operator.logging;

import java.util.logging.Level;

/**
 * Convenience class to provide SPL trace levels through the {@code java.util.logging} facility.
 * <br>
 * The SPL trace levels in descending order are:
 *
 * <UL>
 *   <LI>{@link TraceLevel#ERROR} (highest value)
 *   <LI>{@link TraceLevel#WARN}
 *   <LI>{@link TraceLevel#INFO}
 *   <LI>{@link TraceLevel#DEBUG}
 *   <LI>{@link TraceLevel#TRACE} (lowest value)
 * </UL>
 *
 * <br>
 * When a message is logged to the SPL trace facility its {@code java.util.logging.Level} is mapped
 * to the SPL trace level as follows:
 *
 * <TABLE border="1" summary="This table describes the mapping of Java logging API log levels to SPL trace levels">
 * <CAPTION><B><I>SPL trace level mapping</I></B></CAPTION>
 * <TBODY>
 * <TR><TH scope="col">SPL Trace Level</TH><TH scope="col">{@code java.util.logging.Level}</TH></TR>
 * <TR><TD></TD><TD></TD></TR>
 * <TR><TD rowspan="3">{@code error}</TD><TD>{@code Level.SEVERE}</TD></TR>
 * <TR><TD>{@link TraceLevel#ERROR}</TD></TR>
 * <TR><TD>{@code >= 1000}</TD></TR>
 *
 * <TR><TD rowspan="3">{@code warn}</TD><TD>{@code Level.WARNING}</TD></TR>
 * <TR><TD>{@link TraceLevel#WARN}</TD></TR>
 * <TR><TD>{@code >= 900 AND < 1000}</TD></TR>
 *
 * <TR><TD rowspan="4">{@code info}</TD><TD>{@code Level.INFO}</TD></TR>
 * <TR><TD>{@code Level.CONFIG}</TD></TR>
 * <TR><TD>{@link TraceLevel#INFO}</TD></TR>
 * <TR><TD>{@code >= 700 AND < 900}</TD></TR>
 *
 * <TR><TD rowspan="4">{@code debug}</TD><TD>{@code Level.FINE}</TD></TR>
 * <TR><TD>{@code Level.FINER}</TD></TR>
 * <TR><TD>{@link TraceLevel#DEBUG}</TD></TR>
 * <TR><TD>{@code >= 400 AND < 700}</TD></TR>
 *
 * <TR><TD rowspan="3">{@code trace}</TD><TD>{@code Level.FINEST}</TD></TR>
 * <TR><TD>{@link TraceLevel#TRACE}</TD></TR>
 * <TR><TD>{@code < 400}</TD></TR>
 *
 * </TBODY>
 * </TABLE>
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public class TraceLevel extends Level {

  /** */
  private static final long serialVersionUID = 167127300471593497L;

  private TraceLevel(String name, int level) {
    super(name, level);
  }
  /**
   * ERROR is a level indicating the operator cannot continue. All internal or unexpected failures
   * should be traced at this level.
   *
   * <p>This level is initialized to 1000 and is equal to {@code Level.SEVERE}.
   */
  public static final TraceLevel ERROR = new TraceLevel("ERROR", Level.SEVERE.intValue());

  /**
   * WARN is a level indicating a potential error or impending error. This level can also indicate a
   * progressive failure (for example, the potential leaking of resources).
   *
   * <p>This level is initialized to 900 and is equal to {@code Level.WARNING}.
   */
  public static final TraceLevel WARN = new TraceLevel("WARN", Level.WARNING.intValue());

  /**
   * INFO is level indicating general information outlining overall operator progress. Information
   * that helps to give context to a trace, often when an operator is being initialized, handling
   * {@code allPortsReady()} or being shutdown.
   *
   * <p>This level is initialized to 700 and is equal to {@code Level.CONFIG}.
   */
  public static final TraceLevel INFO = new TraceLevel("INFO", Level.CONFIG.intValue());

  /**
   * DEBUG is a message level for debug level tracing information including method entry, exit, and
   * return values.
   *
   * <p>This level is initialized to 400 and is equal to {@code Level.FINER}.
   */
  public static final TraceLevel DEBUG = new TraceLevel("DEBUG", Level.FINER.intValue());

  /**
   * TRACE is a level for more detailed trace that includes all the detail that is needed to debug
   * problems.
   *
   * <p>This level is initialized to 300 and is equal to {@code Level.FINEST}.
   */
  public static final TraceLevel TRACE = new TraceLevel("TRACE", Level.FINEST.intValue());
}
