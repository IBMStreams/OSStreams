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

import org.apache.log4j.spi.LocationInfo;

public interface MessageSender<L extends Enum<?>> {

  public static final String NA = LocationInfo.NA;
  public static final int LINE_NA = -1;

  public void messageSend(
      L level, String message, String aspects, String classMethod, String file, int line);
}
