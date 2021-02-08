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

import java.util.logging.Formatter;
import java.util.logging.LogRecord;

/**
 * Log formatter that simply gets the message from the LogRecord. If the logged message was a
 * message key with an associated ResourceBundle then format() returns the expanded message
 * including parameters.
 *
 * <p>Plain formatter is used as the SPL C++ runtime formats the message by including the time,
 * thread information etc.
 */
class PlainFormatter extends Formatter {

  @Override
  public String format(LogRecord r) {
    return formatMessage(r);
  }
}
