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

package com.ibm.streams.sch.exceptions;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.messages.Message;
import com.ibm.streams.messages.MessageKey;
import com.ibm.streams.messages.MessageUtilities;
import com.ibm.streams.sch.SupportFunctions;
import java.util.ArrayList;
import java.util.List;

public class SCHException extends StreamsException {

  private static final long serialVersionUID = 1L;

  public SCHException() {}

  public SCHException(Message msg, String freeText) {
    super(msg, freeText);
  }

  public SCHException(Message msg) {
    super(msg);
  }

  public SCHException(MessageKey key, String substText) {
    this.key = key;
    List<Object> subsText = new ArrayList<>();
    subsText.add(substText);
    this.substText = subsText.toArray();
  }

  public SCHException(MessageKey key, List<String> substText) {
    this.key = key;
    this.substText = substText.toArray();
  }

  public SCHException(MessageKey key) {
    this.key = key;
  }

  public SCHException(MessageKey key, String substText, String substText2) {
    this.key = key;
    List<Object> subsText = new ArrayList<>();
    subsText.add(substText);
    subsText.add(substText2);
    this.substText = subsText.toArray();
  }

  public String toString() {
    StringBuilder str = new StringBuilder();
    str.append("SCHException: " + MessageUtilities.getMessageAndPrefix(this.key, this.substText));
    //    	str.append("SCHException - MsgId["+msgID+"] substText["+this.substText+"]\n");
    str.append("\n");
    if (this.additionalData != null)
      str.append(
          SupportFunctions.toStr_forAdditionalExceptionData(
              this.additionalData.getAdditionalExceptionData()));
    return str.toString();
  }
}
