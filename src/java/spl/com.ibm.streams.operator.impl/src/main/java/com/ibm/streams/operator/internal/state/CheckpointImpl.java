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

package com.ibm.streams.operator.internal.state;

import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.state.Checkpoint;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/** Represents a checkpoint. */
public class CheckpointImpl implements Checkpoint {

  private long key;
  private OperatorAdapter adapter;

  private ObjectInputStream _inputStream;
  private ObjectOutputStream _outputStream;

  private CheckpointTransport transport;

  private boolean isCheckpointing; // true=during checkpoint, false=during restore

  public CheckpointImpl(
      OperatorAdapter adapter, CheckpointTransport transport, long key, boolean isCheckpointing) {
    this.adapter = adapter;
    this.transport = transport;
    this.key = key;
    this.isCheckpointing = isCheckpointing;
  }
  /**
   * Sequence identifier of this checkpoint.
   *
   * @return identifier for the checkpoint
   */
  public long getSequenceId() {
    return key;
  }

  public long getTimestamp() throws IllegalStateException {
    if (isCheckpointing)
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_TIMESTAMP_UNAVAILABLE_DURING_CHECKPOINTING, "")
              .getLocalizedMessageAndPrefix());
    return transport.getTimestamp();
  }

  public ObjectInputStream getInputStream() throws IOException {
    if (isCheckpointing)
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_CHECKPOINT_INPUT_STREAM_RETRIEVED_RESTORE, "")
              .getLocalizedMessageAndPrefix());

    if (_inputStream == null) {
      _inputStream = new ObjectInputStream(new BufferedInputStream(transport.getInputStream()));
    }
    return _inputStream;
  }

  @Override
  public ObjectOutputStream getOutputStream() throws IOException {
    if (!isCheckpointing)
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_CHECKPOINT_OUTPUT_STREAM_RETRIEVED_CHECKPOINTING, "")
              .getLocalizedMessageAndPrefix());

    if (_outputStream == null)
      _outputStream = new ObjectOutputStream(new BufferedOutputStream(transport.getOutputStream()));
    return _outputStream;
  }

  public void close() throws IOException {
    if (_inputStream != null) {
      _inputStream.close();
    }
    if (_outputStream != null) {
      _outputStream.flush();
      _outputStream.close();
    }
  }
}
