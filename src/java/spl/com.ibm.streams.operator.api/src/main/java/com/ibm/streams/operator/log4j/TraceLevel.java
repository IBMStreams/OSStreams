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

package com.ibm.streams.operator.log4j;

import org.apache.log4j.Level;

/**
 * Convenience class to provide SPL trace levels through the {@code org.apache.log4j} facility. <br>
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
 * When a message is logged to the SPL trace facility its {@code org.apache.log4j.Level} is mapped
 * to the SPL trace level as follows:
 *
 * <TABLE border="1" summary="This table describes the mapping of Apache log4j API log levels to SPL trace levels">
 * <CAPTION><B><I>SPL trace level mapping</I></B></CAPTION>
 * <TBODY>
 * <TR><TH scope="col">SPL Trace Level</TH><TH scope="col">{@code java.util.logging.Level}</TH></TR>
 * <TR><TD></TD><TD></TD></TR>
 * <TR><TD rowspan="3">{@code error}</TD><TD>{@code Level.FATAL}</TD></TR>
 * <TR><TD>{@link Level#ERROR}</TD></TR>
 * <TR><TD>{@link TraceLevel#ERROR}</TD></TR>
 *
 * <TR><TD rowspan="2">{@code warn}</TD><TD>{@code Level.WARN}</TD></TR>
 * <TR><TD>{@link TraceLevel#WARN}</TD></TR>
 *
 * <TR><TD rowspan="2">{@code info}</TD><TD>{@code Level.INFO}</TD></TR>
 * <TR><TD>{@link TraceLevel#INFO}</TD></TR>
 *
 * <TR><TD rowspan="2">{@code debug}</TD><TD>{@code Level.DEBUG}</TD></TR>
 * <TR><TD>{@link TraceLevel#DEBUG}</TD></TR>
 *
 * <TR><TD rowspan="2">{@code trace}</TD><TD>{@code Level.TRACE}</TD></TR>
 * <TR><TD>{@link TraceLevel#TRACE}</TD></TR>
 *
 * </TBODY>
 * </TABLE>
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public class TraceLevel extends Level {

  /** */
  private static final long serialVersionUID = -3941678919385245861L;

  private TraceLevel(Level level) {
    super(level.toInt(), level.toString(), level.getSyslogEquivalent());
  }
  /**
   * ERROR is a level indicating the operator cannot continue. All internal or unexpected failures
   * should be traced at this level.
   *
   * <p>This level is equal to {@code Level.ERROR}.
   */
  public static TraceLevel ERROR = new TraceLevel(Level.ERROR);

  /**
   * WARN is a level indicating a potential error or impending error. This level can also indicate a
   * progressive failure (for example, the potential leaking of resources).
   *
   * <p>This level and is equal to {@code Level.WARN}.
   */
  public static TraceLevel WARN = new TraceLevel(Level.WARN);

  /**
   * INFO is level indicating general information outlining overall operator progress. Information
   * that helps to give context to a trace, often when an operator is being initialized, handling
   * {@code allPortsReady()} or being shutdown.
   *
   * <p>This level is equal to {@code Level.INFO}.
   */
  public static TraceLevel INFO = new TraceLevel(Level.INFO);

  /**
   * DEBUG is a message level for debug level tracing information including method entry, exit, and
   * return values.
   *
   * <p>This level is equal to {@code Level.DEBUG}.
   */
  public static TraceLevel DEBUG = new TraceLevel(Level.DEBUG);

  /**
   * TRACE is a level for more detailed trace that includes all the detail that is needed to debug
   * problems.
   *
   * <p>This level is equal to {@code Level.TRACE}.
   */
  public static TraceLevel TRACE = new TraceLevel(Level.TRACE);
}
