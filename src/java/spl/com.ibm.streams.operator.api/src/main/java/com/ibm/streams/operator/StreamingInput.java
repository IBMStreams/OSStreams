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

package com.ibm.streams.operator;

import com.ibm.streams.operator.window.StreamWindow;

/**
 * StreamingInput represents an input port for an {@link com.ibm.streams.operator.Operator}.
 *
 * @param T Type of the tuple this port supports.
 */
public interface StreamingInput<T> extends StreamingData {

  /** Enumeration for how a port imports its streams. */
  public enum ImportType {
    /** Import is by name of an exported stream. */
    BY_NAME,
    /** Import is by a subscription against exported ports' stream properties. */
    BY_SUBSCRIPTION,

    /** Port is private to an application. */
    NOT_IMPORTED
  }

  /**
   * Return the type of this port's import.
   *
   * @return The import type.
   */
  public ImportType getImportType();

  /**
   * Is the port importing an exported stream.
   *
   * @return True if the port imports a stream, false if the port is connected only to streams
   *     within the application.
   */
  public boolean isImported();

  /**
   * Get the window configuration for this port.
   *
   * @return Description of the window configuration.
   */
  public StreamWindow<T> getStreamWindow();

  /**
   * Is the port a control port. This is valid at runtime only. At compile time, this method always
   * returns false regardless of whether the port is a control port or not.
   *
   * @return True if the port is a control port, false otherwise
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public boolean isControl();
}
