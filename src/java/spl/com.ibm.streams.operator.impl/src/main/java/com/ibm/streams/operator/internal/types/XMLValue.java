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

import com.ibm.streams.operator.types.XML;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;
import javax.xml.transform.stream.StreamSource;

/** Representation of an SPL XML value that simply wraps the raw bytes of a serialized XML value. */
public class XMLValue implements XML, Serializable {

  /** */
  private static final long serialVersionUID = -4899073554231197007L;

  /** Encoded data according to the XML declaration. */
  private final byte[] data;

  /** Has this instance been verified as well formed. */
  private transient boolean wellFormed;

  static final XMLValue EMPTY = new XMLValue(new byte[0]);

  public XMLValue(byte[] data) {
    this.data = data;
  }

  /**
   * Return the raw length of the value.
   *
   * @return
   */
  int getRawLength() {
    return data.length;
  }

  /**
   * Write the raw bytes out to the ByteBuffer for serialization.
   *
   * @param bytes
   */
  void writeRawData(ByteBuffer bytes) {
    bytes.put(data);
  }

  /**
   * Return true if the value is empty, the default value for an SPL value.
   *
   * @return True if empty, false otherwise.
   */
  public boolean isDefaultValue() {
    return data.length == 0;
  }

  /**
   * Check the value is valid.
   *
   * @return This if it is valid.
   */
  final void checkNotDefaultValue() {
    if (isDefaultValue()) {
      throw new IllegalStateException();
    }
  }

  /*
   * Interface methods
   */

  @Override
  public StreamSource getStreamSource() {
    checkNotDefaultValue();
    return new StreamSource(getInputStream());
  }

  private String getEncoding() throws XMLStreamException {
    final XMLStreamReader xsr =
        XMLInputFactory.newInstance().createXMLStreamReader(getInputStream());
    try {
      return xsr.getEncoding();
    } finally {
      xsr.close();
    }
  }

  @Override
  public InputStream getInputStream() {
    checkNotDefaultValue();
    return new ByteArrayInputStream(data);
  }

  /** {@inheritDoc} */
  @Override
  public int hashCode() {
    return Arrays.hashCode(data);
  }

  /** {@inheritDoc} */
  @Override
  public boolean equals(Object other) {
    if (!(other instanceof XMLValue)) return false;

    XMLValue oxml = (XMLValue) other;
    return Arrays.equals(data, oxml.data);
  }

  @Override
  public String toString() {
    if (isDefaultValue()) return "";

    try {
      return new String(data, getEncoding());
    } catch (UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    } catch (XMLStreamException e) {
      throw new RuntimeException(e);
    }
  }

  synchronized boolean isWellFormed() {
    return wellFormed;
  }

  synchronized void setWellFormed() {
    wellFormed = true;
  }
}
