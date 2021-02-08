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

package com.ibm.streams.admin.internal.api.application.bundle;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey.Key;

/** Exception to be thrown when a request is made in application bundling support. */
public class ApplicationBundleException extends StreamsException {
  /** */
  private static final long serialVersionUID = 1L;

  private String _messageData;

  public ApplicationBundleException(String msgData) {
    super(Key.SAMUnexpectedException);
    Trace.logError("AppBundlEx: " + msgData);
    _messageData = msgData;
  }

  public ApplicationBundleException(String msgData, Throwable cause) {
    super(Key.SAMUnexpectedException, cause);
    Trace.logError("AppBundlEx: " + msgData);
    _messageData = msgData;
  }

  public String getInternalMessage() {
    return _messageData;
  }

  public String getMessage() {
    return (getInternalMessage() + " : " + super.getMessage());
  }
}
