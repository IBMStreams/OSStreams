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

package com.ibm.streams.operator.types;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type.MetaType;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * SPL {@code blob}, which is a sequence of bytes. {@link ValueFactory} is used as a factory class
 * to create instances of this interface.
 *
 * <p>Instances of {@code Blob} are immutable and implement {@code java.io.Serializable}.
 *
 * <p>Note: {@code toString()} returns this blob value as a {@code String} using the SPL character
 * encoding. The value returned has pairs of hexadecimal characters, one for each byte of the blob.
 * The format is identical to one returned by {@code
 * javax.xml.bind.DatatypeConverter.printHexBinary(byte[])}.
 *
 * @see MetaType#BLOB
 * @see Tuple#getBlob(int)
 * @see OutputTuple#setBlob(int, Blob)
 * @see ValueFactory#newBlob(byte[])
 * @see ValueFactory#newBlob(byte[], int, int)
 * @see ValueFactory#newBlob(ByteBuffer, int)
 * @see ValueFactory#readBlob(InputStream)
 * @see ValueFactory#readBlob(InputStream, long)
 * @see ValueFactory#readBlobWithPrefixLength(InputStream)
 */
public interface Blob {

  /**
   * Get the number of bytes in this blob.
   *
   * @return Number of bytes in this blob
   */
  public long getLength();

  /**
   * Return a read-only Byte Buffer that represents this complete blob. If this blob cannot be
   * contained in a single ByteBuffer then an exception is thrown.
   *
   * @return A read-only ByteBuffer representing this blob.
   */
  public ByteBuffer getByteBuffer();

  /**
   * Return a read-only Byte Buffer that represents the portion of this blob value defined b
   *
   * @param position Position of the first byte to be returned. 0 is the first byte of the blob.
   * @param length Number of bytes in the buffer.
   * @return A read-only ByteBuffer representing a portion of this blob.
   */
  public ByteBuffer getByteBuffer(long position, int length);

  /**
   * Return an InputStream representing the value positioned at the first byte.
   *
   * @return An InputStream to read the value.
   */
  public InputStream getInputStream();

  /**
   * Get a copy of the data for this blob. If this blob cannot be contained in a {@code byte[]} then
   * an exception is thrown.
   *
   * @return A copy of the data in this blob.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public byte[] getData();

  /**
   * Put this value into a byte buffer. Will write the data representing this value.
   *
   * @param buf ByteBuffer to be written into.
   * @return The passed in ByteBuffer.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public ByteBuffer put(final ByteBuffer buf);
}
