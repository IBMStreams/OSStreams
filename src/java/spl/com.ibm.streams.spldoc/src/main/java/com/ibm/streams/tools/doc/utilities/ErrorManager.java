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

package com.ibm.streams.tools.doc.utilities;

/**
 * This class counts incidences of errors and warnings encountered by this utility.
 *
 * <p>This counts are used to report track and error status.
 *
 * @author afrenk
 */
public class ErrorManager {

  static ErrorManager errorManager = null;
  static int numberErrors = 0;
  static int numberWarnings = 0;

  public static synchronized ErrorManager getInstance() {
    if (errorManager == null) errorManager = new ErrorManager();
    return errorManager;
  }

  public static int getNumberErrors() {
    return numberErrors;
  }

  public static boolean hasErrors() {
    return (ErrorManager.numberErrors > 0);
  }

  public static void incrErrors() {
    ErrorManager.numberErrors++;
  }

  public static boolean hasWarnings() {
    return (ErrorManager.numberWarnings > 0);
  }

  public static int getNumberWarnings() {
    return numberWarnings;
  }

  public static void incrWarnings() {
    ErrorManager.numberWarnings++;
  }

  public static void throwFatalErrorException() {
    throw getInstance().new FatalErrorException();
  }

  @SuppressWarnings("serial")
  public class FatalErrorException extends RuntimeException {}
}
