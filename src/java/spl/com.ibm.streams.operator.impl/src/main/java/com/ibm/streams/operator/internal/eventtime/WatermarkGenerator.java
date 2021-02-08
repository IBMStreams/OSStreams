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

/** Extracts time data from stream tuples and generates watermarks. */
public class WatermarkGenerator {

  public WatermarkGenerator(EventTimeContextImpl ctx, double lag, double minGap) {
    ctx_ = ctx;
    lag_ = Timestamp.getTimestamp(lag);
    minGap_ = Timestamp.getTimestamp(minGap);
    nextWmOut_ = Watermark.ZeroTimestamp;
  }

  public Timestamp getWatermarkFor(Timestamp eventTime) {
    return eventTime.subtract(lag_);
  }

  public void setWatermark(Timestamp wm) {
    if (!wm.before(nextWmOut_)) {
      nextWmOut_ = wm.add(minGap_);
      ctx_.submit(wm);
    }
  }

  private EventTimeContextImpl ctx_;
  private final Timestamp lag_; // watermark lag to the highest submitted tuple event time
  private final Timestamp minGap_; // minimum event time gap between successive watermarks
  Timestamp nextWmOut_; // threshold for the next Watermark value to submit
}
