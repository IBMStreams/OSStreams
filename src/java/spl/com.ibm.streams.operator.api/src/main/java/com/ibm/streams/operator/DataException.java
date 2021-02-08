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

package com.ibm.streams.operator;

/**
 * This is an exception that is thrown by Java primitive operators when an error condition is found
 * when processing input tuples. This exception is automatically caught by the SPL runtime when the
 * parameter exception of the @catch annotation is parameterized with spl, std, or all.
 */
public class DataException extends RuntimeException {

  /** Constructs a DataException. */
  public DataException() {}

  /**
   * Constructs a DataException with the specified message.
   *
   * @param message exception detailed message
   */
  public DataException(String message) {
    super(message);
  }

  /**
   * Constructs a DataException with the specified message and cause.
   *
   * @param message exception detailed message
   * @param cause exception cause
   */
  public DataException(String message, Throwable cause) {
    super(message, cause);
  }
}
