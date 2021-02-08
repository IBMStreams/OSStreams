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

import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.MessageKey;
import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.messages.Messages;
import java.io.PrintStream;
import java.text.MessageFormat;
import java.util.Locale;
import java.util.ResourceBundle;

/**
 * A class for logging spl-make-doc messages
 *
 * <p>The logger class is allocated by invoking the static newInstance() method. Thereafter, call
 * the Logger.getInstance() to access the logger.
 *
 * @author afrenk
 */
public class Logger {

  static Logger logger = null;
  ResourceBundle bundle;

  boolean verbose = false;
  String trace = null;
  String warning = null;
  String error = null;

  public enum LogType {
    TRACE,
    INFO,
    WARNING,
    ERROR
  };

  private Logger() {
    bundle = ResourceBundle.getBundle("com.ibm.streams.tools.doc.messages.Messages");
    trace = getMessage(Messages.MSG_TRACE_LOG_MESSAGE);
    warning = getMessage(Messages.MSG_WARNING_LOG_MESSAGE);
    error = getMessage(Messages.MSG_ERROR_LOG_MESSAGE);
  }

  public static synchronized Logger getInstance() {
    if (logger != null) {
      return logger;
    }
    logger = new Logger();
    Locale defaultLocale = Locale.getDefault();
    String[] args =
        new String[] {
          defaultLocale.getLanguage(), defaultLocale.getCountry(), defaultLocale.getVariant()
        };
    logger.printTrace(Key.CDISP7002I, args);
    return logger;
  }

  public void setVerbose(boolean verbose) {
    this.verbose = verbose;
  }

  public String getMessage(String id) {
    return getMessage(id, null);
  }

  public String getMessage(String id, String[] args) {
    String messageTemplate = bundle.getString(id);
    if (args == null || args.length == 0) {
      return messageTemplate;
    }
    MessageFormat mf = new MessageFormat(messageTemplate);
    return mf.format(args);
  }

  public void log(String id, String location, String message, LogType type) {
    if (type == LogType.TRACE && !verbose) return;
    if (id == null) {
      id = "";
    } else {
      id += " ";
    }
    if (location == null) {
      location = "";
    } else {
      location += ": ";
    }
    String typeString;
    PrintStream printStream;
    if (type == LogType.TRACE) {
      typeString = trace;
      printStream = System.out;
    } else if (type == LogType.INFO) {
      typeString = "";
      printStream = System.out;
    } else if (type == LogType.WARNING) {
      ErrorManager.incrWarnings();
      typeString = warning;
      printStream = System.out;
    } else {
      ErrorManager.incrErrors();
      typeString = error;
      printStream = System.err;
    }
    printStream.println(location + id + typeString + message);
  }

  public void printException(Throwable e) {
    StringBuilder stringStack = new StringBuilder();
    String message = e.getMessage();
    stringStack.append(e.getClass().getName() + "\n");
    if (message != null) {
      stringStack.append(" " + message + "\n");
    }
    for (StackTraceElement ste : e.getStackTrace()) {
      stringStack.append(" " + ste.toString() + "\n");
    }
    Message msg = new Message(Key.CDISP7010E, stringStack);
    log(msg.getMessageId(), null, msg.getLocalizedMessage(), LogType.ERROR);
  }

  public String getMessage(MessageKey key) {
    return new Message(key).getLocalizedMessageAndPrefix();
  }

  public String getMessage(MessageKey key, String[] args) {
    return new Message(key, (Object[]) args).getLocalizedMessageAndPrefix();
  }

  public void printError(MessageKey key) {
    Message message = new Message(key);
    log(message.getMessageId(), null, message.getLocalizedMessage(), LogType.ERROR);
  }

  public void printError(MessageKey key, String[] args) {
    Message message = new Message(key, (Object[]) args);
    log(message.getMessageId(), null, message.getLocalizedMessage(), LogType.ERROR);
  }

  public void printError(MessageKey key, String[] args, Throwable e) {
    Message message = new Message(key, (Object[]) args);
    log(message.getMessageId(), null, message.getLocalizedMessage(), LogType.ERROR);
    printException(e);
  }

  public void printError(MessageKey key, String location, String[] args) {
    Message message = new Message(key, (Object[]) args);
    log(message.getMessageId(), location, message.getLocalizedMessage(), LogType.ERROR);
  }

  public void printWarn(MessageKey key, String location, String[] args) {
    Message message = new Message(key, (Object[]) args);
    log(message.getMessageId(), location, message.getLocalizedMessage(), LogType.WARNING);
  }

  public void printInfo(MessageKey key) {
    Message message = new Message(key);
    log(message.getMessageId(), null, message.getLocalizedMessage(), LogType.INFO);
  }

  public void printInfo(MessageKey key, String[] args) {
    Message message = new Message(key, (Object[]) args);
    log(message.getMessageId(), null, message.getLocalizedMessage(), LogType.INFO);
  }

  public void printTrace(MessageKey key) {
    Message message = new Message(key);
    log(null, null, message.getLocalizedMessage(), LogType.TRACE);
  }

  public void printTrace(MessageKey key, String[] args) {
    Message message = new Message(key, (Object[]) args);
    log(null, null, message.getLocalizedMessage(), LogType.TRACE);
  }
}
