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

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;

/**
 * SPL rstring. An rstring represents a sequence of raw bytes, typically used for simple string
 * representations with ASCII or UTF-8 encoding. RString is an immutable object.
 */
public final class RString implements Serializable {

  private static final long serialVersionUID = 1636707187908188702L;

  private final byte[] data;

  /**
   * Create an RString from the first <code>length</code> bytes of <code>buf</code>.
   *
   * @param buf ByteBuffer to read value from.
   * @param length Number of bytes to read.
   */
  public RString(ByteBuffer buf, int length) {
    buf.get(data = new byte[length]);
  }

  /**
   * Create an rstring set from <code>data</code>. A copy of the passed in byte array is made.
   *
   * @param data Byte array to clone as the value.
   */
  public RString(byte[] data) {
    this.data = data.clone();
  }

  /**
   * Create an rstring set from <code>value</code> using UTF-8 encoding.
   *
   * @param value Value used to initialize this.
   */
  public RString(String value) {
    try {
      this.data = value.getBytes("UTF-8");
    } catch (UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
  }

  /**
   * Get the number of bytes in the rstring value.
   *
   * @return Number of bytes in this rstring
   */
  public int getLength() {
    return data.length;
  }

  /**
   * Get a ByteBuffer that represents the contents of this value.
   *
   * @return A ByteBuffer positioned at the first byte.
   */
  public ByteBuffer getByteBuffer() {
    return ByteBuffer.wrap(data).asReadOnlyBuffer();
  }

  /**
   * Get the raw data for this value.
   *
   * @return A copy of the raw data for this value.
   */
  public byte[] getData() {
    return data.clone();
  }

  /**
   * Return the value as a String using UTF-8 encoding.
   *
   * @return Value as a String.
   */
  public String getString() {
    try {
      return new String(data, "UTF-8");
    } catch (UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
  }

  /**
   * Return the value as a String using the defined character set encoding.
   *
   * @return Value as a String.
   */
  public String getString(String charsetName) throws IOException {
    return new String(data, charsetName);
  }
  /**
   * Return the value as a String using the defined character set encoding.
   *
   * @return Value as a String.
   */
  public String getString(Charset charset) {
    return new String(data, charset);
  }

  /**
   * Returns a hash code for this {@code RString}. Equivalent to {@code
   * java.util.Arrays.hashCode(getData())}.
   */
  @Override
  public int hashCode() {
    return Arrays.hashCode(data);
  }

  /**
   * Compares this to {@code other}. The result is true if and only if {@code other} is an instance
   * of {@code RString} containing the exact same sequence of bytes as {@code this}.
   */
  @Override
  public boolean equals(Object other) {
    if (!(other instanceof RString)) return false;

    RString ors = (RString) other;
    return Arrays.equals(data, ors.data);
  }

  /** Returns getString(). */
  @Override
  public String toString() {
    return getString();
  }

  /**
   * Put this value into a byte buffer. Will write the raw bytes representing this value.
   *
   * @param buf ByteBuffer to be written into.
   * @return The passed in ByteBuffer.
   */
  public ByteBuffer put(final ByteBuffer buf) {
    return buf.put(data);
  }

  // Package access only, to ensure operators cannot modify the
  // value.
  final byte[] getRawData() {
    return data;
  }
}
