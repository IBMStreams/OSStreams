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

import com.ibm.streams.operator.logging.TraceLevel;
import java.util.logging.Level;

public final class TraceHandler extends OperatorRuntimeHandler<Trace> {

  public TraceHandler(MessageSender<Trace> sender) {
    super(sender);
  }

  @Override
  protected Trace getLevelEnum(final Level level) {
    return convertLevelToEnum(level);
  }

  public static Trace convertLevelToEnum(final Level level) {
    final int il = level.intValue();

    if (il < TraceLevel.DEBUG.intValue()) return Trace.trace;

    if (il < TraceLevel.INFO.intValue()) return Trace.debug;

    if (il < TraceLevel.WARN.intValue()) return Trace.info;

    if (il < TraceLevel.ERROR.intValue()) return Trace.warn;

    return Trace.error;
  }
}
