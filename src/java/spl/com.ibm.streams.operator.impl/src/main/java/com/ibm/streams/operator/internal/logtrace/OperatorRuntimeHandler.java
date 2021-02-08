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

import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.LogRecord;

/**
 * Abstract Handler for the SPL runtime log and trace. Trace and Log specific versions will write
 * the message Uses a PlainFormatter() to simply get the text of the users message. Formatting of
 * the message is handled by the C++ functions.
 */
abstract class OperatorRuntimeHandler<L extends Enum<?>> extends Handler {

  private final MessageSender<L> sender;
  private boolean closed;

  OperatorRuntimeHandler(MessageSender<L> sender) {
    this.sender = sender;
    setFormatter(new PlainFormatter());
  }

  /** */
  @Override
  public final synchronized void close() {
    closed = true;
  }

  /** No need to any flush() action, messages are always passed directly to the SPL runtime. */
  @Override
  public final void flush() {}

  /**
   * Publish the log record consistently through a MessageSender instance that will call the SPL
   * runtime.
   */
  @Override
  public final synchronized void publish(final LogRecord record) {
    // Ignore logging methods if closed.
    if (closed) return;

    if (!isLoggable(record)) return;

    final L level = getLevelEnum(record.getLevel());

    // See LogRecord.getMessage(), a null value for the message
    // is equivalent to the empty string.

    // In Java 7, format will throw a NPE if message is null so handle that
    // case
    String msg = null;
    if (record.getMessage() == null) {
      msg = "";
    } else {
      msg = getFormatter().format(record);
      if (msg == null) msg = "";
    }

    final String aspects = LoggerAspects.getLoggerAspectString(record.getLoggerName());

    String sourceClass = record.getSourceClassName();
    if (sourceClass == null) sourceClass = MessageSender.NA;

    String sourceMethod = record.getSourceMethodName();
    if (sourceMethod == null) sourceMethod = MessageSender.NA;

    final String classMethod = sourceClass.concat(".").concat(sourceMethod);

    // TODO
    sender.messageSend(level, msg, aspects, classMethod, MessageSender.NA, MessageSender.LINE_NA);

    final Throwable t = record.getThrown();
    if (t == null) return;

    final StackTraceElement[] stack = t.getStackTrace();

    // TODO
    sender.messageSend(level, t.toString(), aspects, null, null, 0);
    for (StackTraceElement line : stack)
      sender.messageSend(level, line.toString(), aspects, null, null, 0);
  }

  /**
   * Get the C++ enum value for the log record level.
   *
   * @param level LogRecord's level value.
   * @return Value matching ordinal for C++.
   */
  protected abstract L getLevelEnum(Level level);
}
