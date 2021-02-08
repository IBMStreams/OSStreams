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

package com.ibm.streams.operator.internal.jni;

import com.ibm.streams.operator.internal.state.CheckpointTransport;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;

class JNICheckpointTransport implements CheckpointTransport {

  private long ckptPtr;

  JNICheckpointTransport(long ckptPtr) {
    this.ckptPtr = ckptPtr;
  }

  private static native int getByteBuffer(long cppPointer, ByteBuffer buf, int size)
      throws EOFException, IOException;

  private static native void putByteBuffer(long cppPointer, ByteBuffer buf, int size);

  private static native byte getByte(long cppPointer) throws EOFException, IOException;

  private static native void putByte(long cppPointer, byte value);

  private static native long getTimestamp(long cppPointer);

  @Override
  public long getTimestamp() {
    return getTimestamp(ckptPtr);
  }

  @Override
  public InputStream getInputStream() {
    return new JNICheckpointInputStream();
  }

  @Override
  public OutputStream getOutputStream() {
    return new JNICheckpointOutputStream();
  }

  class JNICheckpointInputStream extends InputStream {

    public JNICheckpointInputStream() {}

    @Override
    public int read() throws IOException {
      try {
        return getByte(ckptPtr);
      } catch (EOFException e) {
        return -1;
      }
    }

    @Override
    public int read(byte[] b, int off, int len) throws IOException {
      if (b == null) {
        throw new NullPointerException();
      } else if (off < 0 || len < 0 || len > b.length - off) {
        throw new IndexOutOfBoundsException();
      } else if (len == 0) {
        return 0;
      }
      ByteBuffer buffer = ByteBuffer.allocateDirect(len);
      try {
        int returnSize = getByteBuffer(ckptPtr, buffer, buffer.limit());
        buffer.get(b, off, returnSize);
        return returnSize;
      } catch (EOFException e) {
        // EOF was reached in the backend data store so return -1
        return -1;
      }
    }
  }

  class JNICheckpointOutputStream extends OutputStream {

    public JNICheckpointOutputStream() {}

    @Override
    public void write(byte[] b, int off, int len) throws IOException {
      if (b == null) {
        throw new NullPointerException();
      } else if (off < 0 || len < 0 || len > b.length - off) {
        throw new IndexOutOfBoundsException();
      } else if (len == 0) {
        return;
      }
      ByteBuffer buffer = ByteBuffer.allocateDirect(len);
      buffer.put(b, off, len);
      putByteBuffer(ckptPtr, buffer, buffer.limit());
    }

    @Override
    public void write(int b) throws IOException {
      putByte(ckptPtr, (byte) b);
    }
  }
}
