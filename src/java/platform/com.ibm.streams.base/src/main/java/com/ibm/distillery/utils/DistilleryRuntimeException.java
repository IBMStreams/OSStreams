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

package com.ibm.distillery.utils;

import java.util.Vector;

/** @author slp@us.ibm.com */
public class DistilleryRuntimeException extends RuntimeException {

  private Long exceptionCode;

  private Vector<String> substitutionText;

  public static final long serialVersionUID = 1;

  ///
  /// Constructor taking no parameters
  ///
  public DistilleryRuntimeException() {
    this(0L, new Vector<String>());
  }

  ///
  /// Constructor
  ///
  /// @param eExceptionCode The exception code related to the exception
  /// @param eSubstitutionText The vector of strings to be used as substitution text for
  ///                          the exception code.
  ///
  public DistilleryRuntimeException(Long eExceptionCode, Vector<String> eSubstitutionText) {
    super();
    exceptionCode = eExceptionCode;
    substitutionText = eSubstitutionText;
  }

  ///
  /// Constructor
  ///
  /// @param msg The message text related to the exception.
  ///
  public DistilleryRuntimeException(String msg) {
    this(msg, 0L, new Vector<String>());
  }

  ///
  /// Constructor
  ///
  /// @param msg The message text related to the exception.
  /// @param eExceptionCode The exception code related to the exception
  /// @param eSubstitutionText The vector of strings to be used as substitution text for
  ///                          the exception code.
  ///
  public DistilleryRuntimeException(
      String msg, Long eExceptionCode, Vector<String> eSubstitutionText) {
    super(msg);
    exceptionCode = eExceptionCode;
    substitutionText = eSubstitutionText;
  }

  ///
  /// Constructor
  ///
  /// @param msg The message text related to the exception.
  /// @param thr A Throwable cause
  ///
  public DistilleryRuntimeException(String msg, Throwable thr) {
    this(msg, thr, 0L, new Vector<String>());
  }

  ///
  /// Constructor
  ///
  /// @param msg The message text related to the exception.
  /// @param thr A Throwable cause
  /// @param eExceptionCode The exception code related to the exception
  /// @param eSubstitutionText The vector of strings to be used as substitution text for
  ///                          the exception code.
  ///
  public DistilleryRuntimeException(
      String msg, Throwable thr, Long eExceptionCode, Vector<String> eSubstitutionText) {
    super(msg, thr);
    exceptionCode = eExceptionCode;
    substitutionText = eSubstitutionText;
  }

  ///
  /// Constructor
  ///
  /// @param thr A Throwable cause
  ///
  public DistilleryRuntimeException(Throwable thr) {
    this(thr, 0L, new Vector<String>());
  }

  ///
  /// Constructor
  ///
  /// @param thr A Throwable cause
  /// @param eExceptionCode The exception code related to the exception
  /// @param eSubstitutionText The vector of strings to be used as substitution text for
  ///                          the exception code.
  ///
  public DistilleryRuntimeException(
      Throwable thr, Long eExceptionCode, Vector<String> eSubstitutionText) {
    super(thr);
    exceptionCode = eExceptionCode;
    substitutionText = eSubstitutionText;
  }

  ///
  /// Return the exception code related to the exception
  ///
  /// @param The exception code
  ///
  public Long getExceptionCode() {
    return exceptionCode;
  }

  ///
  /// Return the substitution texts related to the exception code
  ///
  /// @param The vector of substitution texts
  ///
  public Vector<String> getSubstitutionText() {
    return substitutionText;
  }
}
