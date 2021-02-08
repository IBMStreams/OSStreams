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

import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.types.Blob;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.Random;

/**
 * Type implementation for SPL blob.
 *
 * <p>Format is N (written as a long) followed by N bytes.
 *
 * <p>Object type is an internal implementation of com.ibm.streams.operator.types.Blob.
 */
final class BlobSpl extends AbstractBufferType<ByteBuffer, Blob> implements TypeEncoding {

  BlobSpl() {
    super(MetaType.BLOB, SmallBlob.EMPTY);
  }

  @Override
  public Class<?> getObjectType() {
    return Blob.class;
  }

  public Blob convertFrom(Blob blob) {
    return blob;
  }

  @Override
  Blob _randomValue(Random rand) {
    byte[] data = new byte[rand.nextInt(1024)];
    rand.nextBytes(data);
    return new SmallBlob(data);
  }

  @Override
  public final int getSerializedSize() {
    return TypeEncoding.VARIABLE_LENGTH;
  }

  @Override
  public long getMaxSerializedSize(Object value) {
    Blob blob = (Blob) value;
    return blob.getLength() + 8;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    long length = bytes.getLong();
    if (length == 0) return SmallBlob.EMPTY;

    // TODO large blob handling!
    byte[] data = new byte[(int) length];
    bytes.get(data);
    return new SmallBlob(data);
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    Blob blob = (Blob) value;
    bytes.putLong(blob.getLength());
    bytes.put(blob.getByteBuffer());
  }

  @Override
  Buffer getReadOnlyBuffer(Blob value) {
    return value.getByteBuffer();
  }

  @Override
  public Blob getBlob(Object value) {
    return (Blob) value;
  }

  @Override
  public TypeEncoding getEncoding() {
    return this;
  }

  @Override
  Blob safeCopy(Blob internalValue) {
    return internalValue;
  }

  @Override
  public String getString(Object value) {
    return value.toString();
  }
}
