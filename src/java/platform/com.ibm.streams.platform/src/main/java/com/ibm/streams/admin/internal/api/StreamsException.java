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

package com.ibm.streams.admin.internal.api;

import com.ibm.distillery.utils.DistilleryException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.messages.Message;
import com.ibm.streams.messages.MessageKey;
import com.ibm.streams.messages.MessageUtilities;
import com.ibm.streams.messages.StreamsAdminMessagesKey.Key;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Vector;

/**
 * Generic exception class for Streams admin interfaces. This class is used as the base class for
 * the specific exceptions.
 */
public class StreamsException extends Exception {
  protected String msgID = null;
  protected MessageKey key = null;
  protected Object[] substText = null;

  // data structure to hold all mested additional exception data.
  protected AdditionalExceptionData additionalData = null;

  public StreamsException() {}

  public StreamsException(String msg) {
    super(msg);
  }

  public StreamsException(DistilleryException de) {
    super(de);

    String id = de.getExceptionCode().getMessageId();
    String fullMsg = id + " " + de.getMessage();

    StreamsMessage streamsMessage = StreamsMessage.isStreamsMessage(fullMsg);
    if (streamsMessage != null) {
      this.substText = de.getSubstitutionText().toArray();
      this.msgID = streamsMessage.getMessageID();
    } else {
      this.key = Key.ERR_UNEXPECTED;
      this.substText = new Object[] {de.getMessage()};
    }
  }

  public StreamsException(Throwable cause) {
    super(cause);

    // transfer key/substText from StreamsException
    if (cause instanceof StreamsException) {
      StreamsException streamsCause = (StreamsException) cause;
      this.key = streamsCause.key;
      this.substText = streamsCause.substText;
    } else if (cause instanceof DistilleryException) {
      DistilleryException de = (DistilleryException) cause;
      String id = de.getExceptionCode().getMessageId();
      String fullMsg = id + " " + de.getMessage();

      StreamsMessage streamsMessage = StreamsMessage.isStreamsMessage(fullMsg);
      if (streamsMessage != null) {
        this.substText = de.getSubstitutionText().toArray();
        this.msgID = streamsMessage.getMessageID();
      } else {
        this.key = Key.ERR_UNEXPECTED;
        this.substText = new Object[] {de.getMessage()};
      }
    } else {
      this.key = Key.ERR_UNEXPECTED;
      if (cause.getMessage() != null) {
        this.substText = new Object[] {cause.getMessage()};
      } else {
        this.substText = new Object[] {cause.toString()};
      }
    }
  }

  public StreamsException(Message msg) {
    super();
    this.key = msg.getMessageKey();
    this.substText = msg.getSubstitutions();
  }

  public StreamsException(Message msg, Throwable cause) {
    super(cause);
    this.key = msg.getMessageKey();
    this.substText = msg.getSubstitutions();
  }

  public StreamsException(Message msg, String freeText) {
    super(freeText);
    this.key = msg.getMessageKey();
    this.substText = msg.getSubstitutions();
  }

  public StreamsException(MessageKey key) {
    super();
    this.key = key;
    this.substText = null;
  }

  public StreamsException(MessageKey key, Throwable cause) {
    super(cause);
    this.key = key;
    this.substText = null;
  }

  public StreamsException(MessageKey key, Object... subst) {
    super();
    this.key = key;
    this.substText = subst;
  }

  public StreamsException(Throwable cause, MessageKey key, Object... subst) {
    super(cause);
    this.key = key;
    this.substText = subst;
  }

  public StreamsException(StreamsMessage msg, Throwable cause) {
    super(msg.getMessageText(), cause);
    this.msgID = msg.getMessageID();
  }

  public StreamsException(StreamsMessage msg) {
    super(msg.getMessageText());
    this.msgID = msg.getMessageID();

    List<String> subst = msg.getSubstitution();
    if (subst != null) {
      this.substText = subst.toArray();
    }
  }

  /**
   * Get addition exception data of this StreamsException object.
   *
   * @return addition exception data
   */
  public AdditionalExceptionData getAdditionalExceptionData() {
    return additionalData;
  }

  /**
   * Set addition exception data of this StreamsException object.
   *
   * @param additionalData addition exception data
   */
  public void setAdditionalExceptionData(AdditionalExceptionData additionalData) {
    this.additionalData = additionalData;
  }

  /**
   * Check if the cause of this StreamsException is from Streams Runtime (DistilleryException) and
   * if it has nested additional exception data.
   *
   * @return true if cause is an instance of DistilleryException and it has additional exception
   *     data; false otherwise.
   */
  public boolean hasAdditionalData() {
    Throwable cause = getCause();
    if (cause != null && cause instanceof DistilleryException) {
      return ((DistilleryException) cause).hasAdditionalData();
    } else {
      return false;
    }
  }

  /**
   * Get the additional exception data from the cause (DistilleryException) of this exception.
   *
   * @return If the cause of this StreamsException is from Streams Runtime (DistilleryException) and
   *     it has nested additional exception data, return the additional exception data object;
   *     otherwise, return null.
   */
  /*
      public AdditionalExceptionData getAdditionalData() throws StreamsException {
          try {
              Throwable cause = getCause();
              if (cause != null && cause instanceof DistilleryException) {
                  return new AdditionalExceptionData(((DistilleryException)cause).getAdditionalData());
              } else {
                  return null;
              }
          } catch (DistilleryException de) {
              throw new StreamsException(de);
          }
      }
  */

  public String getMessage() {
    String excMsg = null;

    if (key == null && msgID == null) {
      Throwable cause = getCause();
      if (cause != null && cause instanceof DistilleryException) {
        Locale locale = Locale.getDefault();
        excMsg = getStreamsMessage(cause, locale);
      }
    } else {
      Locale locale = Locale.getDefault();
      excMsg = getMessageByID(locale);
    }

    if (excMsg == null) {
      excMsg = super.getMessage();
      if (excMsg == null) {
        Throwable cause = getCause();
        if (cause != null) {
          excMsg = cause.getMessage();
        }
      }
    }

    return excMsg;
  }

  public String getLocalizedMessage() {
    String excMsg = null;

    if (key == null && msgID == null) {
      Throwable cause = getCause();
      if (cause != null) {
        if (cause instanceof DistilleryException) {
          Locale locale = Locale.getDefault();
          excMsg = getStreamsMessage(cause, locale);
        }
      }
    } else {
      Locale locale = Locale.getDefault();
      excMsg = getMessageByID(locale);
    }

    if (excMsg == null) {
      excMsg = super.getLocalizedMessage();
    }

    return excMsg;
  }

  public String getLocalizedMessage(Locale locale) {
    String excMsg = null;

    if (key == null && msgID == null) {
      Throwable cause = getCause();
      if (cause != null) {
        if (cause instanceof DistilleryException) {
          excMsg = getStreamsMessage(cause, locale);
        }
      }
    } else {
      excMsg = getMessageByID(locale);
    }

    if (excMsg == null) {
      excMsg = super.getLocalizedMessage();
    }
    return excMsg;
  }

  public MessageKey getMessageKey() {
    return key;
  }

  public String getMessageId() {
    return (key != null ? key.getMessageId() : msgID);
  }

  public Object[] getSubstitutions() {
    return substText;
  }

  public List<List<StreamsMessage>> getNestedMessages() {
    List<List<StreamsMessage>> nestedMessages = new ArrayList<List<StreamsMessage>>();

    if (key != null) {
      List<StreamsMessage> msgList = new ArrayList<StreamsMessage>();
      StreamsMessage streamsMessage = StreamsMessage.getStreamsMessage(key, substText);
      Trace.logDebug("Adding streams message " + key + " " + streamsMessage);
      msgList.add(streamsMessage);
      nestedMessages.add(msgList);
    }

    if (this.additionalData != null) {
      List<StreamsMessage> msgs = new ArrayList<StreamsMessage>();
      StreamsMessage.processAdditionalData(this.additionalData, msgs);
      nestedMessages.add(msgs);
    }

    Throwable cause = getCause();
    Trace.logDebug("Adding cause: " + cause);
    if (cause != null) {
      if (cause instanceof DistilleryException) {
        while (cause != null) {
          DistilleryException de = (DistilleryException) cause;
          List<StreamsMessage> msgs = new ArrayList<StreamsMessage>();
          StreamsMessage msg = StreamsMessage.getDistilleryExceptionMessage(de);
          if (msg != null) {
            Trace.logDebug("Adding primary distillery exception message " + msg);
            msgs.add(msg);
          }

          if (de.hasAdditionalData()) {
            try {
              AdditionalExceptionData additionalData =
                  new AdditionalExceptionData(de.getAdditionalData());

              StreamsMessage.processAdditionalData(additionalData, msgs);
            } catch (DistilleryException e) {
              Trace.logError("Error while parsing distillery exception", e);
            }
          }

          nestedMessages.add(msgs);
          cause = cause.getCause();
        }
      } else if (cause instanceof StreamsException) {
        StreamsException se = (StreamsException) cause;
        List<List<StreamsMessage>> msglist1 = se.getNestedMessages();
        for (List<StreamsMessage> msgList2 : msglist1) {
          Trace.logDebug("Adding list from streams exception " + msgList2);
          nestedMessages.add(msgList2);
        }
      }
    }
    Trace.logDebug("Exit getNestedMessages");
    return nestedMessages;
  }

  /*
  private List<StreamsMessage> processAdditionalData(AdditionalExceptionData additionalData, List<StreamsMessage> msgs) {
    StreamsMessage primaryInformation = additionalData.getPrimaryInformation();
    if (primaryInformation != null) {
      msgs.add(primaryInformation);
    }

    List<StreamsMessage> secondaryInformation = additionalData.getSecondaryInformation();
    if (secondaryInformation != null) {
      msgs.addAll(secondaryInformation);
    }

    List<AdditionalExceptionData> additionalNestedData = additionalData.getAdditionalNestedData();
    for (AdditionalExceptionData ad : additionalNestedData) {
      processAdditionalData(ad, msgs);
    }

    return msgs;
  }*/

  private String getStreamsMessage(Throwable cause, Locale locale) {
    String excMsg = null;
    if (cause instanceof DistilleryException) {
      DistilleryException de = (DistilleryException) cause;
      String id = de.getExceptionCode().getMessageId();
      String fullMsg = id + " " + de.getMessage();
      StreamsMessage streamsMessage = StreamsMessage.isStreamsMessage(fullMsg);
      if (streamsMessage != null) {
        Vector<String> subst = de.getSubstitutionText();
        excMsg = streamsMessage.getLocalizedMessage(locale, subst.toArray());
      } else {
        excMsg = null;
      }
    } else {
      excMsg = null;
    }

    return excMsg;
  }

  private String getMessageByID(Locale locale) {
    String msg = null;

    if (key != null) {
      if (substText != null) {
        msg = MessageUtilities.getMessageAndPrefix(locale, key, substText);
      } else {
        msg = MessageUtilities.getMessageAndPrefix(locale, key);
      }
    } else if (msgID != null) {
      if (substText != null) {
        msg = MessageUtilities.getMessageAndPrefix(locale, msgID, substText);
      } else {
        msg = MessageUtilities.getMessageAndPrefix(locale, msgID);
      }
    }

    return msg;
  }
}
