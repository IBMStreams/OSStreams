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

/**
 * Names for instances of {@code java.util.logging.Logger} with a specific purpose for the Java
 * Operator API.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface LoggerNames {

  /**
   * Name for the {@code Logger} to be used to send log messages to the SPL log facility. Has the
   * value {@code com.ibm.streams.operator.log}.
   */
  public static final String LOG_FACILITY = "com.ibm.streams.operator.log";
}
