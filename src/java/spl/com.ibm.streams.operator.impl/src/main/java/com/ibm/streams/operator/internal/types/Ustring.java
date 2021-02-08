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

import static com.ibm.streams.operator.internal.network.TupleSerialization.getUInt32V;
import static com.ibm.streams.operator.internal.network.TupleSerialization.putUInt32V;

import com.ibm.streams.operator.internal.network.TupleSerialization;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.CharBuffer;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.nio.charset.CodingErrorAction;
import java.util.Random;

/**
 * String type. <br>
 * Four bytes representing an unsigned integer length N (in network byte order) followed by N
 * code-points of UTF-16 encoded characters, each code point is two bytes.
 */
final class Ustring extends ImplementationType<String> {

  Ustring() {
    super(MetaType.USTRING, "");
  }

  /**
   * Shared Charset just used to create encoders and decoders for the specific instances of
   * TupleEncoding.
   */
  private final Charset UTF16 =
      Charset.forName(ByteOrder.nativeOrder() == ByteOrder.LITTLE_ENDIAN ? "UTF-16LE" : "UTF-16BE");

  /** Create a constant for maximum number of bytes per codepoint for encoding */
  private static final int MAX_BYTES_PER_CODEPOINT = 2;

  @Override
  public TypeEncoding getEncoding() {
    return new TypeEncoding() {

      /**
       * Decoder identical to one created by Charset.decode(), but here we manage the decoder
       * object, instead of one cached by Charset. This is a slight performance improvement as there
       * is no synchronization required to get the decoder reference (Charset as an object is
       * thread-safe). Note that a CharsetDecoder instance is not thread-safe, thus to honour
       * TypeEncoding's thread-safe contract we need to synchronize access to the decoder and
       * encoder objects.
       */
      private final CharsetDecoder decoder =
          UTF16
              .newDecoder()
              .onMalformedInput(CodingErrorAction.REPLACE)
              .onUnmappableCharacter(CodingErrorAction.REPLACE);

      /**
       * Encoder identical to one created by Charset.encode(), but here we manage the encoder
       * object, instead of one cached by Charset. See comments for decoder.
       */
      private final CharsetEncoder encoder =
          UTF16
              .newEncoder()
              .onMalformedInput(CodingErrorAction.REPLACE)
              .onUnmappableCharacter(CodingErrorAction.REPLACE);

      public int getSerializedSize() {
        return VARIABLE_LENGTH;
      }

      public Object readValue(final ByteBuffer bytes) {

        final int codePointCount = getUInt32V(bytes);
        if (codePointCount == 0) return "";

        final int tupleLimit = bytes.limit();
        bytes.limit(bytes.position() + (codePointCount * 2));

        String v;
        try {
          synchronized (decoder) {
            v = decoder.decode(bytes).toString();
          }
        } catch (CharacterCodingException e) {
          // can never happen due to the replacement policy
          // defined for the decoder. (See Charset.decode().
          throw new Error(e);
        }

        bytes.limit(tupleLimit);

        return v;
      }

      /** Return the maximum number of bytes that could be used to encode this value. */
      public long getMaxSerializedSize(Object value) {

        long strLength = ((String) value).length();

        return (strLength * MAX_BYTES_PER_CODEPOINT) + TupleSerialization.MAX_ENCODED_32BIT_LEN;
      }

      public void writeValue(final ByteBuffer bytes, final Object value) {
        final String s = (String) value;

        // Optimize the empty string case as bounded collection
        // types will write out empty ustrings.
        if (s.length() == 0) {
          putUInt32V(bytes, 0);
          return;
        }

        try {
          ByteBuffer encodedBytes;

          synchronized (encoder) {
            encodedBytes = encoder.encode(CharBuffer.wrap(s));
          }

          putUInt32V(bytes, encodedBytes.limit() / 2);
          bytes.put(encodedBytes);
        } catch (CharacterCodingException e) {
          // Cannot happen due to replacement policy, see
          // Charset.encode().
          throw new Error(e);
        }
      }
    };
  }

  @Override
  String _randomValue(Random rand) {
    return Ustring.randomString(rand);
  }

  static String randomString(Random rand) {
    final int length = rand.nextInt(40);
    final StringBuilder sb = new StringBuilder();
    for (int i = 0; i < length; i++) {
      int r = rand.nextInt(26 + 26 + 10);
      char ch;
      if (r < 10) ch = (char) ('0' + r);
      else if (r < (10 + 26)) ch = (char) ('a' + r - 10);
      else ch = (char) ('A' + r - 10 - 26);
      sb.append(ch);
    }
    return sb.toString();
  }

  @Override
  String safeCopy(String value) {
    return value;
  }

  @Override
  public String convertFrom(String value) {
    return value;
  }
}
