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

/**
 * Event-time processing API.
 *
 * <p>Event-time is a simple model which supports streams processing where time is not derived from
 * the system time of the machine Streams is running on, but from a time value associated with each
 * tuple. In a graph enabled for event-time, tuples have an attribute which holds their time value.
 * Having a time value for each tuple enables operations such as grouping tuples with a time value
 * falling within specified time intervals, and running aggregate calculations on the group.
 *
 * <ul>
 *   <li><em>Watermarks</em> keep track of event-time progress in a data stream. A watermark
 *       provides a metric of event time progress in a data stream. Ideally:
 *       <ul>
 *         <li>In the context of an input stream, for a watermark with time X, all tuples with event
 *             time less than X have been received.
 *         <li>In the context of an output stream, for a watermark with time X, all tuples with
 *             event time less than X have been submitted.
 *       </ul>
 *       Note that a watermark is only an estimate of completeness. If the event time value is set
 *       before ingestion, the system cannot guarantee that late tuples are not observed.
 *   <li><em>Late tuples</em> - in real life scenarios, tuples with event times earlier than X may
 *       be observed after a watermark with time X. These tuples are late with respect to the
 *       watermark.
 *   <li><em>Event-time windows</em> collect tuples along event-time boundaries into window time
 *       intervals. A window time interval is specified by its low (t0) and high (t0+T) boundaries
 *       and contains all tuples with event-time values within the interval <tt>[t0, t0+T)</tt>. A
 *       new SPL window clause is provided to specify event time windows. An event-time window
 *       partitions the time domain into equally sized intervals of the form: <tt>[N *
 *       creationPeriod + intervalOffset, (N + 1) * creationPeriod + intervalOffset +
 *       intervalDuration)</tt> where intervalDuration, creationPeriod, and intervalOffset are
 *       parameters defined by the SPL window clause. An event-time window operates by assigning
 *       tuples to the window pane instances corresponding to the time intervals the tuple's event
 *       time belongs to.
 *   <li><em>Default trigger policy</em> - when the input event-time (as specified by the latest
 *       watermark) becomes later than the window time interval's high boundary, the time interval
 *       is complete and triggers. If tuples with event times within the time interval's interval
 *       arrive after the time interval has triggered (late tuples) the time interval triggers again
 *       and the operator which handles the trigger event re-calculates output taking into account
 *       the late data.
 *   <li>An event time window has a <em>maximum lateness</em> horizon characterized by the
 *       discardAge SPL window clause option. Tuples which arrive later than the maximum lateness
 *       value are ignored by the window. The window content is deleted after the window is closed.
 *   <li>An event-time graph starts from the output port of an <em>event-time source</em> operator.
 *       The operator inserts watermarks into the output stream from time to time.
 * </ul>
 *
 * @since IBM Streams Version 4.3.0
 */
package com.ibm.streams.operator.eventtime;
