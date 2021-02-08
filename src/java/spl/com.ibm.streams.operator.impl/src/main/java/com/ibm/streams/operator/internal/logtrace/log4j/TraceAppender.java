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

import com.ibm.streams.operator.internal.logtrace.MessageSender;
import com.ibm.streams.operator.internal.logtrace.Trace;
import com.ibm.streams.operator.log4j.TraceLevel;
import org.apache.log4j.Level;

/** Appender for SPL trace facility. */
public class TraceAppender extends OperatorRuntimeAppender<Trace> {

  public static final String APPENDER_PROPERTY = "com.ibm.streams.operator.log4j.appender.SPLTRACE";

  public TraceAppender(MessageSender<Trace> sender) {
    super(sender);
  }

  @Override
  protected Trace getLevelEnum(final Level level) {

    final int il = level.toInt();

    if (il < TraceLevel.DEBUG.toInt()) return Trace.trace;

    if (il < TraceLevel.INFO.toInt()) return Trace.debug;

    if (il < TraceLevel.WARN.toInt()) return Trace.info;

    if (il < TraceLevel.ERROR.toInt()) return Trace.warn;

    return Trace.error;
  }
}
