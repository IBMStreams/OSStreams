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

import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.spi.internal.TypeProvider;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.ByteArrayOutputStream;
import java.io.DataInput;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.stream.StreamResult;
import org.apache.commons.math.complex.Complex;

/**
 * Factory for creating instances of Streams specific attribute value objects.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public class ValueFactory {

  /** Buffer size used for reading from streams. */
  private static final int BUFFER_SIZE = 16 * 1024;

  /**
   * Create an {@link XML} document value from a byte stream. The resulting {@code XML} is a
   * faithful representation of the bytes in the byte stream as no XML processing or normalization
   * is performed. The {@code InputStream input} will be read until the end of file marker but the
   * stream will not be closed.
   *
   * @param input Document as a byte stream.
   * @return Document as a {@code XML}.
   * @throws IOException Exception reading stream.
   */
  public static XML newXML(final InputStream input) throws IOException {
    return TypeProvider.provider.newXML(getBytes(input));
  }

  private static byte[] getBytes(final InputStream input) throws IOException {
    final byte[] b = new byte[BUFFER_SIZE];
    ByteArrayOutputStream aos = new ByteArrayOutputStream(BUFFER_SIZE);

    int rc;
    while ((rc = input.read(b)) != -1) {
      aos.write(b, 0, rc);
    }
    return aos.toByteArray();
  }

  /**
   * Create an {@link XML} document value from a byte stream contained in a {@code ByteBuffer}. The
   * resulting {@code XML} is a faithful representation of the bytes in the byte stream as no XML
   * processing or normalization is performed.
   *
   * @param bytes {@code ByteBuffer} containing document as a byte stream.
   * @param length number of bytes to extract.
   * @return Document as a {@code XML}.
   */
  public static XML newXML(final ByteBuffer bytes, final int length) {
    final byte[] data = new byte[length];
    bytes.get(data);
    return TypeProvider.provider.newXML(data);
  }

  /**
   * Create an {@link XML} document value from an XML source input. The resulting value corresponds
   * to {@code source} as transformed according to the state of {@code transformer}.
   *
   * @param transformer Transformer to use.
   * @param source XML source input.
   * @return Document as a {@code XML}.
   */
  public static XML newXML(final Transformer transformer, final Source source)
      throws TransformerException {
    ByteArrayOutputStream aos = new ByteArrayOutputStream(BUFFER_SIZE);
    StreamResult result = new StreamResult(aos);

    transformer.transform(source, result);

    return TypeProvider.provider.newXML(aos.toByteArray());
  }

  /**
   * Create an {@link Blob} value from a byte stream contained in a {@code ByteBuffer}.
   *
   * @param bytes {@code ByteBuffer} containing bytes for the {@code blob} value.
   * @param length number of bytes to extract.
   * @return Value as a {@code Blob}.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public static Blob newBlob(final ByteBuffer bytes, final int length) {
    if (length == 0) return newBlob(null, 0, 0);
    final byte[] data = new byte[length];
    bytes.get(data);
    return newBlobNoCopy(data);
  }

  /**
   * Create a new {@link Blob} with the raw byte contents of an {@link RString}.
   *
   * @param value Value to be converted to a {@code Blob}.
   * @return Converted value.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public static Blob newBlob(RString value) {
    return newBlobNoCopy(value.getRawData());
  }

  private static Blob newBlobNoCopy(byte[] data) {
    return TypeProvider.provider.newBlob(false, data, 0, data.length);
  }

  /**
   * Create an {@link Blob} value from an array of bytes.
   *
   * @param data Data to be copied from.
   * @param offest Offset into {@code data} to start copying.
   * @param length Length to copy.
   * @return Value as a {@code Blob}.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public static Blob newBlob(final byte[] data, int offest, int length) {
    return TypeProvider.provider.newBlob(true, data, offest, length);
  }
  /**
   * Create an {@link Blob} value from an array of bytes.
   *
   * @param data Data to be copied from.
   * @return Value as a {@code Blob}.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public static Blob newBlob(final byte[] data) {
    return newBlob(data, 0, data.length);
  }

  /**
   * Read a {@link Blob} value from an {@code InputStream}. The input stream is completely read.
   *
   * @param input Input stream to read {@code Blob} value from.
   * @return {@code Blob} containing the contents of {@code input}.
   * @throws IOException Exception reading from the stream.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public static Blob readBlob(final InputStream input) throws IOException {
    return newBlobNoCopy(getBytes(input));
  }

  /**
   * Read a {@link Blob} value from an {@code InputStream}. The input stream must contain {@code
   * length} bytes.
   *
   * @param input Input stream to read {@code Blob} value from.
   * @param length Number of bytes to read from {@code input}.
   * @return {@code Blob} containing the contents of {@code input} with {@code length} number of
   *     bytes.
   * @throws IOException Exception reading from the stream.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public static Blob readBlob(final InputStream input, final long length) throws IOException {
    if (length == 0) return TypeProvider.provider.newBlob(false, null, 0, 0);
    if (length > (long) Integer.MAX_VALUE)
      throw new IllegalArgumentException(
          new Message(Key.VF_INVALID_BLOB_LENGTH, length).getLocalizedMessageAndPrefix());
    final byte[] data = new byte[(int) length];
    final DataInput din;
    if (input instanceof DataInput) {
      din = (DataInput) input;
    } else {
      din = new DataInputStream(input);
    }

    din.readFully(data);
    return newBlobNoCopy(data);
  }

  /**
   * Read a {@link Blob} value from an {@code InputStream} where the number of bytes in the {@code
   * Blob} is represented as a four byte length value in the stream before the value. The length is
   * read using in network byte order, using {@code java.io.DataInput.readInt()}.
   *
   * @param input Input stream to read {@code Blob} value from.
   * @return {@code Blob} containing the contents of {@code input} with number of bytes determined
   *     from the first four bytes of the stream.
   * @throws IOException Exception reading from the stream.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public static Blob readBlobWithPrefixLength(InputStream input) throws IOException {
    final DataInput din;
    if (input instanceof DataInput) {
      din = (DataInput) input;
    } else {
      DataInputStream dis = new DataInputStream(input);
      input = dis;
      din = dis;
    }
    return readBlob(input, din.readInt());
  }

  /*
   * Regular expressions used to build up the
   * the pattern for the SPL character representation
   * of a complex value
   * (real, imaginary)
   * where real and imaginary are character representations
   * of floating point numbers.
   */
  private static final String Digits = "(\\d+)";
  private static final String Exp = "[eE][+-]?" + Digits;
  private static final String FloatExpression =
      "[+-]?" + Digits + "(\\.?" + Digits + "?(" + Exp + ")?)";

  private static final Pattern splComplexPattern =
      Pattern.compile(
          "\\x28\\x20*" + FloatExpression + "\\x20*,\\x20*" + FloatExpression + "\\x20*\\x29");
  private static final Pattern splitter = Pattern.compile("[\\x28\\x20,\\x29]++");

  /**
   * Create a Complex representing a {@link MetaType#COMPLEX64 complex64} from its SPL character
   * encoding format of {@code (real, imaginary)}. Space characters (\x20) surrounding the {@code
   * real} and {@code imaginary} values are ignored.
   *
   * @param value SPL character encoded {@link MetaType#COMPLEX64 complex64}.
   * @return Complex number
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public static Complex newComplex64(CharSequence value) {
    verifyComplex(value);
    final String[] comps = splitter.split(value, 0);
    final double real = Double.valueOf(comps[1]);
    final double imaginary = Double.valueOf(comps[2]);
    return new Complex(real, imaginary);
  }
  /**
   * Create a Complex representing a {@link MetaType#COMPLEX32 complex32} from its SPL character
   * encoding format of {@code (real, imaginary)}. Space characters (\x20) surrounding the {@code
   * real} and {@code imaginary} values are ignored.
   *
   * @param value SPL character encoded {@link MetaType#COMPLEX32 complex32}.
   * @return Complex number
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public static Complex newComplex32(CharSequence value) {
    verifyComplex(value);
    final String[] comps = splitter.split(value, 0);
    final float real = Float.valueOf(comps[1]);
    final float imaginary = Float.valueOf(comps[2]);
    return new Complex(real, imaginary);
  }
  /** Verify a SPL encoded complex is valid. */
  private static void verifyComplex(CharSequence value) {
    Matcher m = splComplexPattern.matcher(value);
    if (!m.matches())
      throw new NumberFormatException(
          new Message(Key.VF_INVALID_COMPLEX_FORMAT, value).getLocalizedMessageAndPrefix());
  }
}
