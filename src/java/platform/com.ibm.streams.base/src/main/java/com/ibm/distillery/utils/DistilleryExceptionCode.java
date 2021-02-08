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

import java.io.Serializable;

public class DistilleryExceptionCode implements Serializable {

  public static final long serialVersionUID = 1L;
  public static String NO_MESSAGE_ID = "NoMessageId";

  // Message ID
  private String msgId;

  /** Default construct */
  public DistilleryExceptionCode() {
    msgId = new String(NO_MESSAGE_ID);
  }

  /**
   * Construct a new exception with the specified detail message
   *
   * @param msg the exception related message
   */
  public DistilleryExceptionCode(String msg) {
    msgId = msg;
  }

  /**
   * Obtain the location (source code information) of where an exception occurred
   *
   * @return the location where an exception occurred
   */
  public String getMessageId() {
    return msgId;
  }

  public String toString() {
    String ret = new String();

    ret = ret.concat(" DistilleryExceptionCode object - Message id = " + msgId);
    return ret;
  }
}
