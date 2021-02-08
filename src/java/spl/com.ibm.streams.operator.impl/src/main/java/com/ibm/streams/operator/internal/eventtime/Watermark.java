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

package com.ibm.streams.operator.internal.eventtime;

import com.ibm.streams.operator.types.Timestamp;
import java.util.concurrent.TimeUnit;

/**
 * This class is used to transmit watermark updates from one operator to another. The watermark
 * update consists of the timestamp value of the watermark, and the identity of the output port from
 * which the update originated.
 */
public class Watermark {

  /**
   * Create a new watermark
   *
   * @param seconds the seconds value of the timestamp
   * @param nanoseconds the nanoseconds value of the timestamp
   * @param operatorId the operator index of the operator sending the update
   * @param oportId the output port index on which the operator sent the update
   */
  public Watermark(long seconds, int nanoseconds, int operatorId, int oportId) {
    this.timestamp = new Timestamp(seconds, nanoseconds);
    this.operatorId = operatorId;
    this.oportId = oportId;
  }

  /**
   * Create a new watermark
   *
   * @param ts the value of the timestamp
   * @param operatorId the operator index of the operator sending the update
   * @param oportId the output port index on which the operator sent the update
   */
  public Watermark(Timestamp ts, int operatorId, int oportId) {
    this.timestamp = ts;
    this.operatorId = operatorId;
    this.oportId = oportId;
  }

  /**
   * Get the timestamp of this watermark.
   *
   * @return the timestamp.
   */
  public Timestamp getTimestamp() {
    return timestamp;
  }

  /**
   * Get the operator index of this watermark.
   *
   * @return the operator index
   */
  public int getOperatorIndex() {
    return operatorId;
  }

  /**
   * Get the output port index of this watermark.
   *
   * @return the output port index
   */
  public int getPortIndex() {
    return oportId;
  }

  @Override
  public String toString() {
    return "{timestamp=" + timestamp + ", operatorId=" + operatorId + ", oportId=" + oportId + "}";
  }

  private final Timestamp timestamp;
  private final int operatorId; // the operator from which this watermark originated
  private final int oportId; // the output port from which this watermark originated

  private static final int NanosecondsInSecond = (int) TimeUnit.SECONDS.toNanos(1);
  public static final Timestamp MaxTimestamp =
      new Timestamp(Long.MAX_VALUE, NanosecondsInSecond - 1);
  // We are using this as the minimum even though it is possible for the
  // seconds to be negative.
  public static final Timestamp ZeroTimestamp = new Timestamp(0L, 0);
}
