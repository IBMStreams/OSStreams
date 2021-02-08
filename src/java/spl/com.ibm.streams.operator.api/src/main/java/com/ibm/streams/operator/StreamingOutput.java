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

import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

/**
 * StreamingOutput represents an output port for an {@link com.ibm.streams.operator.Operator}.
 *
 * @param T Type of the tuple this port supports.
 */
public interface StreamingOutput<T> extends StreamingData {

  /** Enumeration for how a port exports its stream. */
  public enum ExportType {

    /** Port exports its stream by name. */
    BY_NAME,
    /** Port exports its stream using stream properties. */
    BY_PROPERTIES,
    /** Port is not exported, is private to the application. */
    NOT_EXPORTED
  }

  /** Return how this port is exported. */
  public ExportType getExportType();

  /**
   * Does this port produce an exported (dynamic) stream.
   *
   * @return True if the port is exported, false otherwise.
   */
  public boolean isExported();

  /**
   * Return a new tuple that can be sent using this port. All attributes are are to default values,
   * zero for numeric values, empty strings, etc.
   *
   * @return A tuple.
   */
  public T newTuple();

  /**
   * Submit a tuple to this output port.
   *
   * <p>If a {@link StreamingData.Punctuation#FINAL_MARKER FINAL_MARKER} has been submitted to this
   * port using {@link #punctuate(com.ibm.streams.operator.StreamingData.Punctuation)}, then the
   * {@code tuple} will be discarded.
   *
   * @param tuple Tuple to be sent.
   * @return false, future versions might indicate flow control with this return.
   * @exception IllegalArgumentException Tuple's schema does not match this port.
   * @exception IllegalStateException Operator has not completed initialization or has been
   *     shutdown.
   */
  public boolean submit(T tuple) throws Exception;

  /**
   * Submit an immutable tuple to this output port.
   *
   * <p>If a {@link StreamingData.Punctuation#FINAL_MARKER} has been submitted to this port using
   * <code>punctuate()
   * </code>, then the <code>tuple</code> will be discarded.
   *
   * @param tuple Tuple to be sent.
   * @return false, future versions might indicate flow control with this return.
   * @exception IllegalArgumentException Tuple's schema does not match this port.
   * @exception IllegalStateException Operator has not completed initialization or has been
   *     shutdown.
   */
  public boolean submit(Tuple tuple) throws Exception;

  /**
   * Submit a tuple using variable list of values for the attributes. Attributes are set by
   * position, with the first value used for the attribute at index 0. <br>
   * The {@link StreamingOutput#submit(Tuple) submitted tuple} is identical to a tuple obtained from
   * <br>
   * {@code getStreamSchema().getTuple(attributeValues)} <br>
   * with the conversion of {@code attributeValues} to an array.
   *
   * @param attributeValues Values to populate submitted tuple.
   * @return false, future versions might indicate flow control with this return.
   * @exception IllegalStateException Operator has not completed initialization or has been
   *     shutdown.
   * @see StreamSchema#getTuple(Object[])
   * @see StreamingOutput#submit(Tuple)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public boolean submitAsTuple(Object... attributeValues) throws Exception;

  /**
   * Submit a tuple using {@code Map} containing attribute values with the attribute names as keys.
   * <br>
   * The {@link StreamingOutput#submit(Tuple) submitted tuple} is identical to a tuple obtained from
   * <br>
   * {@code getStreamSchema().getTuple(Map<String, ?>)}.
   *
   * @param map Map to set attribute values from.
   * @return false, future versions might indicate flow control with this return.
   * @exception IllegalStateException Operator has not completed initialization or has been
   *     shutdown.
   * @see StreamSchema#getTuple(Map)
   * @see StreamingOutput#submit(Tuple)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public boolean submitMapAsTuple(Map<String, ?> map) throws Exception;

  /**
   * Submit a tuple setting attribute values from the supplied list by position <br>
   * The {@link StreamingOutput#submit(Tuple) submitted tuple} is identical to a tuple obtained from
   * <br>
   * {@code getStreamSchema().getTuple(List<?>)}.
   *
   * @param list List to set attribute values from.
   * @return false, future versions might indicate flow control with this return.
   * @see StreamSchema#getTuple(List)
   * @see StreamingOutput#submit(Tuple)
   * @exception IllegalStateException Operator has not completed initialization or has been
   *     shutdown.
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public boolean submitListAsTuple(List<?> list) throws Exception;

  /**
   * Return a <code>Callable</code> object that may be used to submit a tuple in the future,
   * typically using the <code>
   * ScheduledExecutorService</code> from <code>OperatorContext</code>. The tuple will be submitted
   * to this port when the <code>call()</code> method of the returned object is invoked.
   *
   * @param tuple Tuple to be sent. Caller must not modify the tuple once this call is made,
   *     discarding the reference after the call is recommended.
   * @see OperatorContext#getScheduledExecutorService()
   */
  public Callable<Boolean> deferredSubmit(T tuple);

  /**
   * Submit a punctuation mark to this output port.
   *
   * <p>If a {@link StreamingData.Punctuation#FINAL_MARKER} has previously been submitted, then the
   * punctuation <code>
   * mark</code> will be discarded.
   *
   * @param mark Type of punctuation mark to submit, if the mark is NON_EXISTENT then no punctuation
   *     will be submitted to the port.
   * @return false, future versions might indicate flow control with this return.
   * @see StreamingData.Punctuation#NON_EXISTENT
   * @exception IllegalStateException Operator has not completed initialization or has been
   *     shutdown.
   */
  public boolean punctuate(Punctuation mark) throws Exception;

  /**
   * Get the collection of stream properties currently set for this output port. The returned <code>
   * Map</code> contains simple objects for a property's value. An exported port supports a limited
   * number of SPL types for stream properties, the mapping between SPL types for the properties and
   * Java types is described in the following table. <br>
   *
   * <TABLE width="60%" summary="This table describes the mapping between SPL types for exported stream properties and their Java object representation in the map returned by the method getStreamProperties">
   * <CAPTION><B><I>Stream Property Types</I></B></CAPTION>
   * <THEAD>
   * <TR>
   * <TH scope="col">SPL Property Type</TD>
   * <TH scope="col">Java Property Type in Map</TD>
   * </TR>
   * </THEAD> <TBODY>
   *
   * <TR>
   * <TD><code>rstring</code></TD>
   * <TD><code>java.lang.String</code></TD>
   * </TR>
   * <TR>
   * <TD><code>int64</code></TD>
   * <TD><code>java.lang.Long</code></TD>
   * </TR>
   * <TR>
   * <TD><code>float64</code></TD>
   * <TD><code>java.lang.Double</code></TD>
   * </TR>
   * <TR>
   * <TD><code>list&lt;rstring&gt;</code></TD>
   * <TD><code>String[]</code></TD>
   * </TR>
   * <TR>
   * <TD><code>list&lt;int64&gt;</code></TD>
   * <TD><code>long[]</code></TD>
   * </TR>
   * <TR>
   * <TD><code>list&lt;float64&gt;</code></TD>
   * <TD><code>double[]</code></TD>
   * </TR>
   * </TBODY>
   * </TABLE>
   *
   * <br>
   * All <code>rstring</code> values use UTF-8 encoding.
   *
   * <p>Returns a snapshot of the properties, changes to the returned map are not reflected by the
   * port. An exported stream's properties are explicitly updated using {@link
   * StreamingOutput#addStreamProperties(Map)} and {@link StreamingOutput#setStreamProperties(Map)}.
   *
   * @return A map of stream properties for this exported port.
   * @exception IllegalStateException if the port is not exported {@link ExportType#BY_PROPERTIES
   *     BY_PROPERTIES}.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public Map<String, Object> getStreamProperties();

  /**
   * Get the collection of stream properties currently set for this output port. Returns a snapshot
   * of the properties as a tuple including full meta-data for each property as an {@link
   * com.ibm.streams.operator.Attribute Attribute} of a {@link Tuple}.
   *
   * <p>Note that number and types of properties for an exported stream is not fixed, thus the
   * schema for the returned tuple just represents the current state of the port. Stream properties
   * only support a subset of SPL types, namely <code>rstring</code>, <code>int64</code>, <code>
   * float64</code>, <code>list&lt;rstring&gt;</code>, <code>
   * list&lt;int64&gt;</code> and <code>list&lt;float64&gt;</code>. All <code>rstring</code> values
   * for stream property use UTF-8 encoding.
   *
   * @return A tuple of stream properties for this exported port.
   * @exception IllegalStateException if the port is not exported {@link ExportType#BY_PROPERTIES
   *     BY_PROPERTIES}.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public Tuple getStreamPropertiesAsTuple();

  /**
   * Set the value of existing stream properties for this port. Updates existing stream properties
   * to the values provided in the given <code>properties</code> object. The given map must only
   * contain properties that are already exist for the port, though it may contain a subset of the
   * properties. This call may be used to update the value and type of a property.
   *
   * <p>The values contained in the map define the type of a property, following the mapping shown
   * in {@link StreamingOutput#getStreamProperties()}.
   *
   * @exception IllegalStateException if the port is not exported {@link ExportType#BY_PROPERTIES
   *     BY_PROPERTIES}.
   * @exception IllegalStateException if a property contained in the map does not currently exist on
   *     the port.
   * @exception UnsupportedOperationException if a value in the map is not of a supported object
   *     type for a stream property.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public void setStreamProperties(Map<String, ?> properties);

  /**
   * Add a collection of stream properties to the set for this output port. The passed in properties
   * is added to the existing properties for this port. The given <code>properties</code> must only
   * contain new properties, to update existing properties {@link
   * StreamingOutput#setStreamProperties(Map)} is used.
   *
   * <p>The values contained in the map define the type of a property, following the mapping shown
   * in {@link StreamingOutput#getStreamProperties()}.
   *
   * @exception IllegalStateException if this port is not exported {@link ExportType#BY_PROPERTIES
   *     BY_PROPERTIES}.
   * @exception IllegalStateException if a property already exists.
   * @exception UnsupportedOperationException if a value in the map is not of a supported object
   *     type for a stream property.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public void addStreamProperties(Map<String, ?> properties);

  /**
   * Remove the named stream properties from this output port.
   *
   * @exception IllegalStateException if the port is not exported {@link ExportType#BY_PROPERTIES
   *     BY_PROPERTIES}.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public void removeStreamProperties(String... name);
}
