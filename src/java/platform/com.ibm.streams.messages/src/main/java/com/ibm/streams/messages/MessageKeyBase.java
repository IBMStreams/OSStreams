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

package com.ibm.streams.messages;

/** Base MessageKay implementation, support serialization constructor */
public class MessageKeyBase implements MessageKey {
  private String _messageId;
  private String _bundle;

  public MessageKeyBase(String msgId, String bundle) {
    _messageId = msgId;
    _bundle = bundle;
  }

  public MessageKeyBase(String msgId) {
    _messageId = msgId;

    String messagePrefix = msgId.substring(0, 5);
    if (messagePrefix.equals(MessageUtilities.RUNTIME_MESSAGE_PREFIX)) {
      _bundle = MessageUtilities.STREAMS_RUNTIME_MESSAGES_CLASS;
    } else if (messagePrefix.equals(MessageUtilities.ADMIN_MESSAGE_PREFIX)) {
      _bundle = MessageUtilities.STREAMS_ADMIN_MESSAGES_CLASS;
    } else if (messagePrefix.equals(MessageUtilities.SWS_MESSAGE_PREFIX)) {
      _bundle = MessageUtilities.STREAMS_SWS_MESSAGES_CLASS;
    } else if (messagePrefix.equals(MessageUtilities.INSTALL_MESSAGE_PREFIX)) {
      _bundle = MessageUtilities.STREAMS_INSTALL_MESSAGES_CLASS;
    }
  }

  public String getMessageId() {
    return _messageId;
  }

  public String getResourceBundleName() {
    return _bundle;
  }

  public String toString() {
    return _messageId + "," + _bundle;
  }
}
