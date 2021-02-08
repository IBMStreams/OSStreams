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

package com.ibm.streams.operator.internal.network;

import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.internal.types.ImplementationType;
import java.nio.ByteBuffer;

public class TupleSerialization {

  protected final Schema schema;

  /**
   * Array of encoding objects in the wire-order format. So the value of encodings[0] is the encoder
   * of the first attribute to be written or read in the serialized format.
   */
  protected final TypeEncoding[] encodings;

  protected TupleSerialization(Schema schema) {
    this.schema = schema;

    // Ordered by logical position in tuple.
    ImplementationType<?>[] types = schema.getImplementationTypes();
    encodings = new TypeEncoding[types.length];
    for (int i = 0; i < encodings.length; i++) {
      encodings[i] = types[i].getEncoding();
    }
  }
  /*
   * Utilities for use with a ByteBuffer.
   */

  public static final int MAX_ENCODED_32BIT_LEN = 5;

  /**
   * Read an encoded int32 SPL serialization length value. Format is either single byte for lengths
   * 0-127 or five bytes of 0x80 followed by the 4-byte length for other values. See RTC 2687.
   */
  public static final int getInt32V(final ByteBuffer buf) {
    byte vb = buf.get();
    if (vb < 0) return buf.getInt();
    return vb;
  }

  /** Write an encoded int32 SPL serialization length value. */
  public static final void putInt32V(final ByteBuffer buf, final int val) {
    if (val > 127) {
      buf.put((byte) 0x80);
      buf.putInt(val);
    } else {
      buf.put((byte) val);
    }
  }

  /** Read an encoded uint32 SPL serialization length value. */
  public static final int getUInt32V(final ByteBuffer buf) {
    // TODO handle the fact that C++ code is writing an unsigned integer
    return getInt32V(buf);
  }

  /** Write an encoded uint32 SPL serialization length value. */
  public static final void putUInt32V(final ByteBuffer buf, final int val) {
    putInt32V(buf, val);
  }

  /** Skip ahead skip bytes in the ByteBuffer. */
  public static final void skip(ByteBuffer bytes, int skip) {
    if (skip > 0) bytes.position(bytes.position() + skip);
  }

  /** Skip ahead skip N elements of fixedSize bytes in the ByteBuffer. */
  public static final void skip(ByteBuffer bytes, int n, int fixedSize) {
    skip(bytes, n * fixedSize);
  }

  /**
   * BLen represents how the number of used elements in a bounded type is written out. The format of
   * the used elements depends on N. For N < 256 it is a single byte as an unsigned byte. For N <
   * 65536 && > 255 it is two bytes as an unsigned short. Otherwise 4 bytes as a signed int (since
   * the maximum value of N is a signed integer).
   */
  public enum BLen {
    BUINT8 {
      @Override
      public int getSerializedSize() {
        return 1;
      }

      @Override
      public void putLength(ByteBuffer buf, int length) {
        buf.put((byte) length);
      }

      @Override
      public int getLength(ByteBuffer buf) {
        return buf.get() & 0xFF;
      }
    },
    BUINT16 {
      @Override
      public int getSerializedSize() {
        return 2;
      }

      @Override
      public void putLength(ByteBuffer buf, int length) {
        buf.putShort((short) length);
      }

      @Override
      public int getLength(ByteBuffer buf) {
        return buf.getShort() & 0xFFFF;
      }
    },
    BINT32 {
      @Override
      public int getSerializedSize() {
        return 4;
      }

      @Override
      public void putLength(ByteBuffer buf, int length) {
        buf.putInt(length);
      }

      @Override
      public int getLength(ByteBuffer buf) {
        return buf.getInt();
      }
    },

    UNBOUNDED {
      @Override
      public int getSerializedSize() {
        return TupleSerialization.MAX_ENCODED_32BIT_LEN;
      }

      @Override
      public int getLength(ByteBuffer buf) {
        return TupleSerialization.getInt32V(buf);
      }

      @Override
      public void putLength(ByteBuffer buf, int length) {
        TupleSerialization.putInt32V(buf, length);
      }
    };

    /** Get the number of bytes (fixed) used to write this length type. */
    public abstract int getSerializedSize();
    /** Write the given length into the ByteBuffer using this encoding. */
    public abstract void putLength(final ByteBuffer buf, final int length);
    /** Read a length from the ByteBuffer using this encoding. */
    public abstract int getLength(final ByteBuffer buf);
  }

  /** Create a BLen based upon the bounded length. */
  public static final BLen getBLen(final int boundedLength) {
    if (boundedLength < 256) return BLen.BUINT8;
    else if (boundedLength < 65536) return BLen.BUINT16;
    else return BLen.BINT32;
  }
}
