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

package com.ibm.streams.operator.eventtime;

import com.ibm.streams.operator.OptionalContext;
import com.ibm.streams.operator.types.Timestamp;

/**
 * This interface manages the sending and receiving of watermarks by an operator that handles a
 * stream with an event-time attribute.
 *
 * @since IBM Streams Version 4.3.0
 */
public interface EventTimeContext extends OptionalContext {

  /** Event-time watermark processing modes. */
  public enum WatermarkMode {
    /**
     * The operator's watermark is set by the Streams runtime as the minimum of watermark values
     * received on all input streams.
     */
    INPUT_WATERMARK,

    /**
     * The operator's watermark is set by the Streams runtime based on the event time of submitted
     * tuples.
     *
     * <p>Watermarks are generated assuming that tuples may be submitted out of order, but only up
     * to a certain limit. After a tuple with event-time {@code N} the operator may submit a tuple
     * with an event-time greater than or equal to {@code N - lag}, where {@code lag} is defined by
     * the <tt>@eventTime</tt> SPL annotation in effect for the operator associated with this
     * context. In order for such tuples to not be considered late, the runtime sets the operator's
     * watermark as the maximum event-time of submitted tuples minus {@code lag}.
     */
    OUTPUT_EVENT_TIME,

    /**
     * The operator's watermark is set by calling {@link EventTimeContext#setWatermark(Timestamp)}.
     * A new watermark is set only if it is greater than the current operator watermark, otherwise
     * it is ignored.
     */
    MANUAL
  }

  /**
   * Sets the event-time operating mode. Should be called only while the operator is being
   * initialized.
   *
   * @param mode The event-time watermark generation mode.
   */
  void setMode(WatermarkMode mode);

  /**
   * Returns the event-time watermark generation mode.
   *
   * @return the mode
   */
  WatermarkMode getMode();

  /**
   * Sets the operator's current watermark value. A new watermark is set only if {@code value} is
   * greater than the current operator watermark, otherwise it is ignored.
   *
   * @param value the watermark value
   */
  void setWatermark(Timestamp value);

  /**
   * Returns the current watermark value for the operator associated with this event-time context.
   *
   * @return the current watermark value
   */
  Timestamp getWatermark();
}
