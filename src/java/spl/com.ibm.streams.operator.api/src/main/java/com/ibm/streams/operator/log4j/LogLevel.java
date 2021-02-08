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
 * Convenience class to provide SPL log levels through the {@code org.apache.log4j} facility. <br>
 * The SPL log levels in descending order are:
 *
 * <UL>
 *   <LI>{@link LogLevel#ERROR} (highest value)
 *   <LI>{@link LogLevel#WARN}
 *   <LI>{@link LogLevel#INFO} (lowest value)
 * </UL>
 *
 * <br>
 * When a message is logged to the SPL log facility its {@code org.apache.log4j.Level} is mapped to
 * the SPL log level as follows:
 *
 * <TABLE border="1" summary="This table describes the mapping of Apache log4j API log levels to SPL log levels">
 * <CAPTION><B><I>SPL log level mapping</I></B></CAPTION>
 * <TBODY>
 * <TR><TH scope="col">SPL Log Level</TH><TH scope="col">{@code org.apache.log4j.Level}</TH></TR>
 * <TR><TD></TD><TD></TD></TR>
 * <TR><TD rowspan="3">{@code error}</TD><TD>{@code Level.FATAL}</TD></TR>
 * <TR><TD>{@link Level#ERROR}</TD></TR>
 * <TR><TD>{@link LogLevel#ERROR}</TD></TR>
 *
 * <TR><TD rowspan="2">{@code warn}</TD><TD>{@code Level.WARN}</TD></TR>
 * <TR><TD>{@link LogLevel#WARN}</TD></TR>
 *
 * <TR><TD rowspan="2">{@code info}</TD><TD>{@code Level.INFO}</TD></TR>
 * <TR><TD>{@link LogLevel#INFO}</TD></TR>
 * </TBODY>
 * </TABLE>
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public final class LogLevel extends Level {

  /** */
  private static final long serialVersionUID = 4695272363265847566L;

  private LogLevel(Level level) {
    super(level.toInt(), level.toString(), level.getSyslogEquivalent());
  }

  /**
   * ERROR is a level indicating the operator cannot continue. All internal or unexpected failures
   * should be logged at this level.
   *
   * <p>This level is equal to {@code Level.ERROR}.
   */
  public static final LogLevel ERROR = new LogLevel(Level.ERROR);

  /**
   * WARN is a level indicating a potential error or impending error. This level can also indicate a
   * progressive failure (for example, the potential leaking of resources).
   *
   * <p>This level is equal to {@code Level.WARN}.
   */
  public static final LogLevel WARN = new LogLevel(Level.WARN);

  /**
   * INFO is level indicating general information outlining overall operator progress. Information
   * that helps to give context to a log, often when an operator is being initialized, handling
   * {@code allPortsReady()} or being shutdown.
   *
   * <p>This level is equal to {@code Level.INFO}.
   */
  public static final LogLevel INFO = new LogLevel(Level.INFO);
}
