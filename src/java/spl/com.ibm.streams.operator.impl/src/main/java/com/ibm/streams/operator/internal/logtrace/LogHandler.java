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

public final class LogHandler extends OperatorRuntimeHandler<Log> {

  public LogHandler(MessageSender<Log> sender) {
    super(sender);
    setLevel(LogLevel.INFO);
  }

  /** LogHandler only ever logs at LogLevel.INFO or higher. */
  @Override
  public void setLevel(final Level newLevel) {
    if (Level.OFF.equals(newLevel)) {
      super.setLevel(newLevel);
      return;
    }
    if (newLevel.intValue() >= LogLevel.INFO.intValue()) {
      super.setLevel(newLevel);
      return;
    }
    super.setLevel(LogLevel.INFO);
  }

  @Override
  protected Log getLevelEnum(final Level level) {
    final int il = level.intValue();

    assert il >= LogLevel.INFO.intValue();

    if (il < LogLevel.WARN.intValue()) return Log.info;

    if (il < LogLevel.ERROR.intValue()) return Log.warn;

    return Log.error;
  }
}
