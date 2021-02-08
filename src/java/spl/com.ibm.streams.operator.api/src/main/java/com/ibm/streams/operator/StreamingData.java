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

import com.ibm.streams.operator.metrics.PEMetrics;

/** StreamingData describes an {@link com.ibm.streams.operator.Operator}'s port. */
public interface StreamingData {

  /**
   * Enumeration for punctuation marks.
   *
   * <p>Punctuation marks for input ports are received by an operator's {@link
   * Operator#processPunctuation(StreamingInput, StreamingData.Punctuation) processPunctuation}
   * method.
   *
   * <p>Punctuation marks are submitted to output ports using the {@link
   * StreamingOutput#punctuate(StreamingData.Punctuation) punctuate} method.
   *
   * @see Operator#processPunctuation(StreamingInput, StreamingData.Punctuation)
   * @see StreamingOutput#punctuate(StreamingData.Punctuation)
   */
  public enum Punctuation {
    /*
     * Implementation note. The order of the enum matches the order
     * in the C++ code which also corresponds to the network representation.
     */

    /** No punctuation, will be ignored by StreamingOutput.punctuate(). */
    /*
     *  Implementation note: this actually maps to 0 to indicate the data
     *  following in a network buffer is a tuple and not a punctuation.
     */
    NON_EXISTENT,
    /** Mark representing a window boundary. */
    WINDOW_MARKER,

    /**
     * Mark representing no more tuples or punctuation will be handled by a port.
     *
     * <p>Once a {@code FINAL_MARKER} is received for an input port, the operator will not receive
     * any further calls to its {@link Operator#process(StreamingInput, Tuple) process} and {@link
     * Operator#processPunctuation(StreamingInput, StreamingData.Punctuation) processPunctuation}
     * for that input port ({@link StreamingInput} instance).
     *
     * <p>After an operator submits a {@code FINAL_MARKER} to an output port, any tuples or
     * punctuation marks submitted to that port are silently discarded.
     */
    FINAL_MARKER
  };

  /**
   * Get the schema for this port.
   *
   * @return schema for the port
   */
  public StreamSchema getStreamSchema();

  /**
   * Get the name for this port as defined in the application.
   *
   * @return name of the port
   */
  public String getName();

  /**
   * Get the port number as defined in the application.
   *
   * @return port number
   */
  public int getPortNumber();

  /**
   * Is this port connected to a PE (processing element) port.
   *
   * @return True if port is connected to a PE port, false otherwise.
   * @see #getPEPortNumber()
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public boolean isConnectedToPEPort();

  /**
   * Get the PE (processing element) port number index for this port.
   *
   * @return PE port number
   * @see ProcessingElement
   * @see PEMetrics#getInputPortMetric(int, InputPortMetric)
   * @see PEMetrics#getOutputPortMetric(int, OutputPortMetric)
   * @throws IllegalStateException Port is not connected to a PE port ({@link
   *     #isConnectedToPEPort()} is false)
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public int getPEPortNumber();
}
