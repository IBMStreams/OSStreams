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

package com.ibm.streams.operator.internal.types;

import com.ibm.streams.operator.types.Blob;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.Serializable;
import java.nio.ByteBuffer;
import java.util.Arrays;
import javax.xml.bind.DatatypeConverter;

/**
 * SPL blob implementation when the blob size fits in a ByteBuffer or byte array. That is, less than
 * 2^31-1 bytes.
 */
public class SmallBlob implements Blob, Serializable {

  private static final long serialVersionUID = -2909185427024487392L;
  public static final Blob EMPTY = new SmallBlob(new byte[0]);

  private final byte[] data;

  SmallBlob(byte[] data) {
    this.data = data;
  }

  @Override
  public ByteBuffer getByteBuffer() {
    return ByteBuffer.wrap(data).asReadOnlyBuffer();
  }

  @Override
  public ByteBuffer getByteBuffer(long position, int length) {
    return ByteBuffer.wrap(data, (int) position, length).asReadOnlyBuffer();
  }

  @Override
  public InputStream getInputStream() {
    return new ByteArrayInputStream(data);
  }

  @Override
  public long getLength() {
    return data.length;
  }

  @Override
  public int hashCode() {
    return Arrays.hashCode(data);
  }

  @Override
  public boolean equals(Object other) {
    if (!(other instanceof Blob)) return false;
    Blob ob = (Blob) other;
    if (getLength() != ob.getLength()) return false;

    return Arrays.equals(data, ((SmallBlob) ob).data);
  }

  @Override
  public String toString() {
    return DatatypeConverter.printHexBinary(data);
  }

  @Override
  public byte[] getData() {
    return data.clone();
  }

  @Override
  public ByteBuffer put(final ByteBuffer buf) {
    return buf.put(data);
  }
}
