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

package com.ibm.streams.operator.internal.ports.actions;

import com.ibm.streams.operator.internal.eventtime.Watermark;
import com.ibm.streams.operator.internal.ports.StreamAction;

/**
 * This class is used to transmit watermark updates into and out of Java operators. Although it
 * implements StreamAction, it is simply a typed action, and its {@linkplain #call() call()} method
 * does nothing.
 */
public class WatermarkAction implements StreamAction<Void> {

  /**
   * Construct a new WatermarkAction containing a watermark.
   *
   * @param wm the watermark.
   */
  public WatermarkAction(Watermark wm) {
    this.wm = wm;
  }

  @Override
  public Type getType() {
    return Type.WATERMARK;
  }

  @Override
  public Void call() throws Exception {
    return null; // null and void
  }

  @Override
  public String toString() {
    return "WatermarkAction " + wm;
  }

  public Watermark getWatermark() {
    return wm;
  }

  private Watermark wm;
}
