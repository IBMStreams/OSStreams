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

public abstract class TraceLogger {
  public static enum Level {
    OFF(0),
    ERROR(2),
    WARN(3),
    INFO(4),
    DEBUG(5),
    TRACE(6);

    private Integer level;

    private Level(int level) {
      this.level = level;
    }

    public boolean isLevel(Level l) {
      return this.level <= l.level;
    }
  }

  public abstract boolean needsClose();

  public abstract void plainText(Object message);

  public abstract void info(Object message);

  public abstract void info(Object message, Throwable throwable);

  public abstract void warn(Object message);

  public abstract void warn(Object message, Throwable throwable);

  public abstract void error(Object message);

  public abstract void error(Object message, Throwable throwable);

  public abstract void trace(Object message);

  public abstract void trace(Object message, Throwable throwable);

  public abstract void debug(Object message);

  public abstract void debug(Object message, Throwable throwable);

  public abstract void close();
}
