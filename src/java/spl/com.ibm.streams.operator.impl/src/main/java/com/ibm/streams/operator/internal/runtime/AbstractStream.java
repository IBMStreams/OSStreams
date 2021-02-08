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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingData;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;

/** Abstract stream holding information common to input and output streams. */
public abstract class AbstractStream implements StreamingData {

  private final int port;

  private final String name;

  private final StreamSchema schema;

  public static final int NOT_CONNECTED_TO_PE_PORT = -1;
  private final int pePortNumber;

  protected AbstractStream(int port, String name, StreamSchema schema, int pePortNumber) {
    this.port = port;
    this.name = name;
    this.schema = schema;
    this.pePortNumber = pePortNumber;
  }

  @Override
  public final String getName() {
    return name;
  }

  @Override
  public final StreamSchema getStreamSchema() {
    return schema;
  }

  @Override
  public final int getPortNumber() {
    return port;
  }

  @Override
  public final boolean isConnectedToPEPort() {
    return pePortNumber != NOT_CONNECTED_TO_PE_PORT;
  }

  @Override
  public final int getPEPortNumber() {
    if (!isConnectedToPEPort())
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_PORT_NOT_CONNECTED_TO_PE_PORT, getName())
              .getLocalizedMessageAndPrefix());
    return pePortNumber;
  }
}
