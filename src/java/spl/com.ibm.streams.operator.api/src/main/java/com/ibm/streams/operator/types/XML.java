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

import java.io.InputStream;
import javax.xml.transform.stream.StreamSource;

/**
 * SPL {@code xml} attribute. An SPL {@code xml} attribute contains an XML document. {@code XML}
 * maintains the document as a representation of its byte stream which is encoded according to the
 * document itself.
 *
 * <p>The default value for an {@code xml} attribute is the empty string which is not a valid XML
 * document. Most methods on this interface will throw {@code IllegalStateException} if {@link
 * XML#isDefaultValue()} is {@code true}.
 *
 * <p>{@code XML} is immutable.
 *
 * <p>Instances of {@code XML} implement {@code java.io.Serializable}.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface XML {

  /**
   * Is this value the default value for an SPL {@code xml} attribute. The default is an empty
   * string.
   *
   * @return True if the value is the default value, false otherwise.
   */
  public boolean isDefaultValue();

  /**
   * Get a {@code StreamSource} for this document.
   *
   * @return A {@code StreamSource} for this document.
   * @throws IllegalStateException Thrown if {@code isDefaultValue()} is {@code true}.
   */
  public StreamSource getStreamSource();

  /**
   * Get the byte stream for this document with character encoding specified by the XML declaration
   *
   * @return Byte stream for this document.
   * @throws IllegalStateException Thrown if {@code isDefaultValue()} is {@code true}.
   */
  public InputStream getInputStream();
}
